// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// An HFS+ volume made from a folder (Apple's Technical Note TN1150): no journal, blocks of 4 KiB, the allocation file, an
// empty extents file, the catalog in nodes of 8 KiB and the data of every file in one extent after it.

#include "hdiutil.h"
#include "lowercase.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <sys/random.h>
#include <unicode/unorm2.h>
#include <unistd.h>

namespace {

constexpr uint32_t BlockSize = 4096;
constexpr uint32_t CatalogNodeSize = 8192;
constexpr uint32_t ExtentsNodeSize = 4096;
constexpr uint32_t RootParent = 1;
constexpr uint32_t RootFolder = 2;
constexpr uint32_t FirstUserId = 16;
// Seconds from 1904 to 1970.
constexpr int64_t HfsEpoch = 2082844800;

// A file or folder of the volume, in the order of the walk.
struct Node
{
    const Entry* entry;
    uint32_t id;
    uint32_t parent;
    std::u16string name;
    uint32_t startBlock;
    uint32_t blockCount;
};

struct Tree
{
    std::vector<std::vector<uint8_t>> nodes;
    uint32_t depth;
    uint32_t root;
    uint32_t leafRecords;
    uint32_t firstLeaf;
    uint32_t lastLeaf;
};

} // namespace

// A record of the catalog: its key and what follows it.
struct Record
{
    uint32_t parent;
    std::u16string name;
    std::vector<uint8_t> data;
};

// A node being filled: records from the front, their offsets at the end.
struct NodeBuffer
{
    std::vector<uint8_t> data = std::vector<uint8_t>(CatalogNodeSize);
    uint32_t size = CatalogNodeSize;
    size_t free = 14;
    std::vector<uint16_t> offsets;
};

static void put16(uint8_t* p, uint16_t value)
{
    p[0] = value >> 8;
    p[1] = value;
}

static void put32(uint8_t* p, uint32_t value)
{
    p[0] = value >> 24;
    p[1] = value >> 16;
    p[2] = value >> 8;
    p[3] = value;
}

static void put64(uint8_t* p, uint64_t value)
{
    put32(p, value >> 32);
    put32(p + 4, value);
}

static uint32_t hfsTime(int64_t time)
{
    const int64_t seconds = time + HfsEpoch;
    if (seconds < 0)
        return 0;
    if (seconds > 0xffffffff)
        return 0xffffffff;
    return seconds;
}

// Created, content modified, attributes modified, accessed, backed up.
static void putDates(uint8_t* p, const Entry& entry)
{
    put32(p, hfsTime(std::min(entry.modified, entry.changed)));
    put32(p + 4, hfsTime(entry.modified));
    put32(p + 8, hfsTime(entry.changed));
    put32(p + 12, hfsTime(entry.accessed));
    put32(p + 16, 0);
}

static uint16_t fold(uint16_t c)
{
    if (c < 0x100)
        return lowerCaseLow[c];
    const uint16_t table = lowerCaseHigh[c >> 8];
    if (!table)
        return c;
    return lowerCaseHigh[table + (c & 0xff)];
}

// The order of names in the catalog: case does not count, characters that fold to nothing are skipped.
static int compareNames(const std::u16string& a, const std::u16string& b)
{
    size_t i = 0;
    size_t j = 0;
    for (;;) {
        uint16_t c1 = 0;
        uint16_t c2 = 0;
        while (i < a.size() && c1 == 0)
            c1 = fold(a[i++]);
        while (j < b.size() && c2 == 0)
            c2 = fold(b[j++]);
        if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        if (c1 == 0)
            return 0;
    }
}

static bool keyBefore(const Record& a, const Record& b)
{
    if (a.parent != b.parent)
        return a.parent < b.parent;
    return compareNames(a.name, b.name) == -1;
}

// The header node of a B-tree: the header record, the user data record and the map of the nodes in use.
static std::vector<uint8_t> headerNode(const Tree& tree, uint32_t nodeSize, uint32_t totalNodes, uint16_t maxKeyLength, uint8_t keyCompareType,
                                       uint32_t attributes)
{
    std::vector<uint8_t> node(nodeSize);
    std::vector<uint8_t> header(106);
    put16(&header[0], tree.depth);
    put32(&header[2], tree.root);
    put32(&header[6], tree.leafRecords);
    put32(&header[10], tree.firstLeaf);
    put32(&header[14], tree.lastLeaf);
    put16(&header[18], nodeSize);
    put16(&header[20], maxKeyLength);
    put32(&header[22], totalNodes);
    put32(&header[26], totalNodes - tree.nodes.size());
    put32(&header[32], nodeSize * totalNodes);
    header[37] = keyCompareType;
    put32(&header[38], attributes);
    std::vector<uint16_t> offsets;
    offsets.push_back(14);
    memcpy(&node[14], header.data(), header.size());
    std::vector<uint8_t> user(128);
    offsets.push_back(14 + 106);
    memcpy(&node[14 + 106], user.data(), user.size());
    std::vector<uint8_t> map(nodeSize - 256);
    for (size_t i = 0; i < tree.nodes.size(); i++)
        map[i >> 3] |= 0x80 >> (i & 7);
    offsets.push_back(14 + 106 + 128);
    memcpy(&node[14 + 106 + 128], map.data(), map.size());
    offsets.push_back(nodeSize - 8);
    node[8] = 1;
    put16(&node[10], 3);
    for (size_t i = 0; i < offsets.size(); i++)
        put16(&node[nodeSize - 2 * (i + 1)], offsets[i]);
    return node;
}

// What a name needs to be in HFS+: decomposed, except the characters that Apple keeps composed.
static void appendDecomposed(const std::u16string& text, std::u16string* out)
{
    if (text.empty())
        return;
    UErrorCode status = U_ZERO_ERROR;
    const UNormalizer2* nfd = unorm2_getNFDInstance(&status);
    std::u16string decomposed((text.size() + 4) * 4, 0);
    status = U_ZERO_ERROR;
    if (nfd) {
        const int32_t size = unorm2_normalize(nfd, text.data(), text.size(), decomposed.data(), decomposed.size(), &status);
        if (status <= U_ZERO_ERROR) {
            decomposed.resize(size);
            out->append(decomposed);
            return;
        }
    }
    out->append(text);
}

static bool keptComposed(uint32_t c)
{
    return (c >= 0x2000 && c < 0x3000) || (c >= 0xf900 && c < 0xfb00) || (c >= 0x2f800 && c < 0x2fb00);
}

// The name as HFS+ keeps it: ':' is what the Mac shows as '/'.
static std::u16string hfsName(const std::string& name)
{
    std::string mac = name;
    for (char& c : mac) {
        if (c == ':')
            c = '/';
    }
    const std::u16string units = utf16(mac);
    std::u16string result;
    std::u16string run;
    for (size_t i = 0; i < units.size();) {
        const uint16_t unit = units[i];
        uint32_t c = unit;
        size_t length = 1;
        if (unit - 0xd800u < 0x400 && i + 1 < units.size()) {
            c = (unit - 0xd800) * 0x400 + units[i + 1] + 0x2400;
            length = 2;
        }
        if (keptComposed(c)) {
            appendDecomposed(run, &result);
            run.clear();
            result.append(units, i, length);
        } else {
            run.append(units, i, length);
        }
        i += length;
    }
    appendDecomposed(run, &result);
    return result;
}

// The catalog records: a folder or file record under its parent and a thread record under its own number.
static std::vector<Record> catalogRecords(const std::vector<Node>& nodes)
{
    std::vector<Record> records;
    for (const Node& node : nodes) {
        const Entry& entry = *node.entry;
        const bool folder = entry.type == Entry::Folder;
        std::vector<uint8_t> data;
        if (folder) {
            data.resize(88);
            put16(&data[0], 1);
            put32(&data[4], entry.children.size());
            put32(&data[8], node.id);
            putDates(&data[12], entry);
            put32(&data[32], entry.uid);
            put32(&data[36], entry.gid);
            put16(&data[42], entry.mode);
        } else {
            data.resize(248);
            put16(&data[0], 2);
            put16(&data[2], 2);
            put32(&data[8], node.id);
            putDates(&data[12], entry);
            put32(&data[32], entry.uid);
            put32(&data[36], entry.gid);
            put16(&data[42], entry.mode);
            if (entry.type == Entry::Link)
                memcpy(&data[48], "slnkrhap", 8);
            put64(&data[88], entry.size);
            put32(&data[100], node.blockCount);
            if (node.blockCount) {
                put32(&data[104], node.startBlock);
                put32(&data[108], node.blockCount);
            }
        }
        records.push_back({node.parent, node.name, data});
        std::vector<uint8_t> thread(10 + 2 * node.name.size());
        put16(&thread[0], folder ? 3 : 4);
        put32(&thread[4], node.parent);
        put16(&thread[8], node.name.size());
        for (size_t i = 0; i < node.name.size(); i++)
            put16(&thread[10 + 2 * i], node.name[i]);
        records.push_back({node.id, {}, thread});
    }
    std::sort(records.begin(), records.end(), keyBefore);
    return records;
}

// Every file and folder under `entry`, numbered; the root is folder 2, the others from 16 on.
static bool collect(const Entry& entry, uint32_t parent, const std::u16string& name, uint32_t* nextId, std::vector<Node>* nodes, std::string* error)
{
    if (name.size() > 255) {
        *error = entry.path + ": File name too long";
        return false;
    }
    const uint32_t id = parent == RootParent ? RootFolder : (*nextId)++;
    nodes->push_back({&entry, id, parent, name, 0, 0});
    if (entry.type != Entry::Folder)
        return true;
    std::vector<std::u16string> names;
    for (const Entry& child : entry.children)
        names.push_back(hfsName(child.name));
    for (size_t i = 0; i < names.size(); i++) {
        for (size_t j = i + 1; j < names.size(); j++) {
            if (compareNames(names[i], names[j]) == 0) {
                *error = entry.children[j].path + ": the name differs from " + entry.children[i].name +
                         " only in case, which this file system cannot keep apart";
                return false;
            }
        }
    }
    for (size_t i = 0; i < entry.children.size(); i++) {
        if (!collect(entry.children[i], id, names[i], nextId, nodes, error))
            return false;
    }
    return true;
}

// The key of a catalog record: its length, the parent and the name.
static std::vector<uint8_t> catalogKey(const Record& record)
{
    std::vector<uint8_t> key(8 + 2 * record.name.size());
    put16(&key[0], key.size() - 2);
    put32(&key[2], record.parent);
    put16(&key[6], record.name.size());
    for (size_t i = 0; i < record.name.size(); i++)
        put16(&key[8 + 2 * i], record.name[i]);
    return key;
}

// The B-tree of the catalog: leaf nodes in key order, then index nodes over them up to one root; node 0 is left for the header.
static bool buildTree(const std::vector<Record>& records, Tree* tree, std::string* error)
{
    tree->nodes.assign(1, {});
    NodeBuffer node;
    std::vector<std::vector<uint8_t>> leaves;
    std::vector<std::vector<uint8_t>> firstKeys;
    std::vector<uint8_t> firstKey;
    auto finish = [&] {
        uint8_t* data = node.data.data();
        put32(data, 0);
        put32(data + 4, 0);
        data[8] = 0xff;
        data[9] = 1;
        put16(data + 10, node.offsets.size());
        std::vector<uint16_t> offsets = node.offsets;
        offsets.push_back(node.free);
        for (size_t i = 0; i < offsets.size(); i++)
            put16(data + node.size - 2 * (i + 1), offsets[i]);
        leaves.push_back(node.data);
        firstKeys.push_back(firstKey);
        node = NodeBuffer();
    };
    for (const Record& record : records) {
        const std::vector<uint8_t> key = catalogKey(record);
        std::vector<uint8_t> bytes = key;
        bytes.insert(bytes.end(), record.data.begin(), record.data.end());
        if (node.size < node.offsets.size() * 2 + bytes.size() + node.free + 4)
            finish();
        if (node.offsets.empty())
            firstKey = key;
        node.offsets.push_back(node.free);
        memcpy(node.data.data() + node.free, bytes.data(), bytes.size());
        node.free += bytes.size();
    }
    finish();

    // The leaves are nodes 1 to n, linked in order; each level above has an index record for every node below.
    struct Index
    {
        std::vector<uint8_t> key;
        uint32_t node;
    };
    std::vector<Index> level;
    const uint32_t leafCount = leaves.size();
    for (uint32_t i = 0; i < leafCount; i++) {
        put32(leaves[i].data(), i + 1 < leafCount ? i + 2 : 0);
        put32(leaves[i].data() + 4, i);
        tree->nodes.push_back(std::move(leaves[i]));
        level.push_back({std::move(firstKeys[i]), i + 1});
    }
    tree->leafRecords = records.size();
    tree->firstLeaf = 1;
    tree->depth = 1;
    tree->lastLeaf = leafCount;
    while (level.size() > 1) {
        tree->depth++;
        std::vector<std::vector<uint8_t>> made;
        std::vector<Index> above;
        NodeBuffer index;
        std::vector<uint8_t> first;
        auto close = [&] {
            uint8_t* data = index.data.data();
            put32(data, 0);
            put32(data + 4, 0);
            data[8] = 0;
            data[9] = tree->depth;
            put16(data + 10, index.offsets.size());
            std::vector<uint16_t> offsets = index.offsets;
            offsets.push_back(index.free);
            for (size_t i = 0; i < offsets.size(); i++)
                put16(data + index.size - 2 * (i + 1), offsets[i]);
            above.push_back({first, uint32_t(tree->nodes.size() + made.size())});
            made.push_back(index.data);
            index = NodeBuffer();
        };
        for (const Index& entry : level) {
            std::vector<uint8_t> bytes = entry.key;
            bytes.resize(bytes.size() + 4);
            put32(&bytes[bytes.size() - 4], entry.node);
            if (index.size < index.offsets.size() * 2 + bytes.size() + index.free + 4)
                close();
            if (index.offsets.empty())
                first = entry.key;
            index.offsets.push_back(index.free);
            memcpy(index.data.data() + index.free, bytes.data(), bytes.size());
            index.free += bytes.size();
        }
        close();
        const uint32_t base = tree->nodes.size();
        for (uint32_t i = 0; i < made.size(); i++) {
            put32(made[i].data(), i + 1 < made.size() ? base + i + 1 : 0);
            put32(made[i].data() + 4, i ? base + i - 1 : 0);
            tree->nodes.push_back(std::move(made[i]));
        }
        level = std::move(above);
    }
    tree->root = level[0].node;
    if (tree->nodes.size() > (CatalogNodeSize - 256) * 8) {
        *error = "too many files for one disk image";
        return false;
    }
    return true;
}

bool makeHfs(const Entry& root, const std::string& volumeName, const std::string& output, const Progress& progress, std::string* error)
{
    std::vector<Node> nodes;
    uint32_t nextId = FirstUserId;
    if (!collect(root, RootParent, hfsName(volumeName), &nextId, &nodes, error))
        return false;
    // The catalog is made once to know its size, which places the data of the files.
    Tree catalog = {};
    if (!buildTree(catalogRecords(nodes), &catalog, error))
        return false;
    const uint32_t catalogNodes = catalog.nodes.size();
    const uint32_t catalogBlocks = catalogNodes * 2;
    uint32_t folders = 0;
    uint32_t files = 0;
    uint64_t dataBlocks = 0;
    for (const Node& node : nodes) {
        if (node.entry->type == Entry::Folder) {
            folders += node.id != RootFolder;
        } else {
            files++;
            dataBlocks += (node.entry->size + BlockSize - 1) / BlockSize;
        }
    }
    uint64_t total = catalogBlocks + 3 + dataBlocks;
    total += 16 + total / 100;
    if (total < 256)
        total = 256;
    // The allocation file is part of what it maps.
    uint64_t bitmapBlocks = (total + 0x7fff) >> 15;
    uint64_t counted = 0;
    for (;;) {
        const uint64_t blocks = bitmapBlocks;
        total += blocks - counted;
        bitmapBlocks = (total + 0x7fff) >> 15;
        counted = blocks;
        if (bitmapBlocks == blocks)
            break;
    }
    if (total >> 32) {
        *error = "the folder is too large for one disk image";
        return false;
    }
    const uint32_t extentsBlock = bitmapBlocks + 1;
    const uint32_t catalogBlock = bitmapBlocks + 2;
    uint32_t used = catalogBlock + catalogBlocks;
    for (Node& node : nodes) {
        if (node.entry->type == Entry::Folder)
            continue;
        node.blockCount = (node.entry->size + BlockSize - 1) / BlockSize;
        node.startBlock = node.blockCount ? used : 0;
        used += node.blockCount;
    }
    if (!buildTree(catalogRecords(nodes), &catalog, error))
        return false;
    catalog.nodes[0] = headerNode(catalog, CatalogNodeSize, catalogNodes, 516, 0xcf, 6);
    Tree extents = {};
    extents.nodes.assign(1, {});
    const std::vector<uint8_t> extentsNode = headerNode(extents, ExtentsNodeSize, 1, 10, 0, 2);

    std::vector<uint8_t> bitmap(bitmapBlocks * BlockSize);
    for (uint32_t block = 0; block < used; block++)
        bitmap[block >> 3] |= 0x80 >> (block & 7);
    const uint32_t last = total - 1;
    bitmap[last >> 3] |= 0x80 >> (last & 7);

    const time_t now = time(nullptr);
    tm local;
    localtime_r(&now, &local);
    std::vector<uint8_t> header(512);
    memcpy(&header[0], "H+", 2);
    put16(&header[2], 4);
    put32(&header[4], 0x100);
    memcpy(&header[8], "10.0", 4);
    put32(&header[16], hfsTime(now + local.tm_gmtoff));
    put32(&header[20], hfsTime(now));
    put32(&header[28], hfsTime(now));
    put32(&header[32], files);
    put32(&header[36], folders);
    put32(&header[40], BlockSize);
    put32(&header[44], total);
    put32(&header[48], last - used);
    put32(&header[52], used);
    put32(&header[56], 0x10000);
    put32(&header[60], 0x10000);
    put32(&header[64], nextId);
    put32(&header[68], 1);
    put64(&header[72], 1);
    uint64_t identifier;
    if (getrandom(&identifier, sizeof(identifier), 0) != sizeof(identifier))
        identifier = 0x5a5a5a5a5a5a5a5a;
    memcpy(&header[104], &identifier, sizeof(identifier));
    // The allocation, extents and catalog files, each in one extent.
    put64(&header[112], bitmapBlocks * BlockSize);
    put32(&header[120], bitmapBlocks * BlockSize);
    put32(&header[124], bitmapBlocks);
    put32(&header[128], 1);
    put32(&header[132], bitmapBlocks);
    put64(&header[192], ExtentsNodeSize);
    put32(&header[200], ExtentsNodeSize);
    put32(&header[204], 1);
    put32(&header[208], extentsBlock);
    put32(&header[212], 1);
    put64(&header[272], uint64_t(catalogNodes) * CatalogNodeSize);
    put32(&header[280], catalogNodes * CatalogNodeSize);
    put32(&header[284], catalogBlocks);
    put32(&header[288], catalogBlock);
    put32(&header[292], catalogBlocks);

    const int fd = open(output.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
    if (fd < 0) {
        *error = output + ": " + strerror(errno);
        return false;
    }
    bool written = ftruncate(fd, total * BlockSize) == 0 && writeAll(fd, 0x400, header.data(), header.size()) &&
                   writeAll(fd, total * BlockSize - 0x400, header.data(), header.size()) &&
                   writeAll(fd, BlockSize, bitmap.data(), bitmap.size()) &&
                   writeAll(fd, uint64_t(extentsBlock) * BlockSize, extentsNode.data(), extentsNode.size());
    for (size_t i = 0; written && i < catalog.nodes.size(); i++)
        written = writeAll(fd, uint64_t(catalogBlock) * BlockSize + i * CatalogNodeSize, catalog.nodes[i].data(), catalog.nodes[i].size());
    if (!written) {
        *error = output + ": " + strerror(errno);
        close(fd);
        unlink(output.c_str());
        return false;
    }
    const uint64_t size = root.totalSize();
    uint64_t done = 0;
    bool copied = true;
    for (const Node& node : nodes) {
        if (!copied)
            break;
        const Entry& entry = *node.entry;
        if (entry.type == Entry::Link) {
            const bool ok = writeAll(fd, uint64_t(node.startBlock) * BlockSize, entry.target.data(), entry.target.size());
            done += entry.size;
            if (!ok) {
                *error = output + ": " + strerror(errno);
                close(fd);
                unlink(output.c_str());
                return false;
            }
        } else if (entry.type == Entry::File && node.blockCount) {
            copied = copyFile(entry.path, entry.size, fd, uint64_t(node.startBlock) * BlockSize, progress, &done, size, error);
        }
    }
    if (close(fd) != 0) {
        if (copied)
            *error = output + ": " + strerror(errno);
        unlink(output.c_str());
        return false;
    }
    if (!copied) {
        unlink(output.c_str());
        return false;
    }
    return true;
}
