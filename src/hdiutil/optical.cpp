// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// makehybrid: one image with ISO 9660, Joliet and UDF 1.02 side by side; the data of every file is there once, the three
// file systems point at it.  Links are only in UDF.

#include "hdiutil.h"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>

namespace {

constexpr uint32_t SectorSize = 2048;
// UDF: the anchor, then the partition.
constexpr uint32_t AnchorSector = 256;
constexpr uint32_t PartitionStart = 257;

// A file, folder or link of the image.
struct Item
{
    const Entry* entry;
    Item* parent;
    std::vector<Item*> children;
    std::string isoName;
    std::string jolietName;
    // Folders: their directories in ISO 9660 and Joliet.
    uint32_t isoExtent;
    uint32_t isoSize;
    uint32_t jolietExtent;
    uint32_t jolietSize;
    // Files and links: the data.
    uint32_t dataExtent;
    uint32_t dataSectors;
    // UDF: the file entry, the directory of a folder, the unique id.
    uint32_t udfEntry;
    uint32_t udfDirectory;
    uint32_t udfDirectorySize;
    uint64_t uniqueId;
    std::vector<uint8_t> linkData;
};

} // namespace

static void le16(uint8_t* p, uint16_t value)
{
    p[0] = value;
    p[1] = value >> 8;
}

static void le32(uint8_t* p, uint32_t value)
{
    le16(p, value);
    le16(p + 2, value >> 16);
}

static void le64(uint8_t* p, uint64_t value)
{
    le32(p, value);
    le32(p + 4, value >> 32);
}

static void be16(uint8_t* p, uint16_t value)
{
    p[0] = value >> 8;
    p[1] = value;
}

static void be32(uint8_t* p, uint32_t value)
{
    be16(p, value >> 16);
    be16(p + 2, value);
}

// Both byte orders, as ISO 9660 keeps numbers.
static void both16(uint8_t* p, uint16_t value)
{
    le16(p, value);
    be16(p + 2, value);
}

static void both32(uint8_t* p, uint32_t value)
{
    le32(p, value);
    be32(p + 4, value);
}

static uint32_t sectors(uint64_t size)
{
    return (size + SectorSize - 1) / SectorSize;
}

// The characters of ISO 9660: upper case letters, digits and '_'; one '_' for any other character.
static std::string isoCharacters(const std::string& name, size_t max)
{
    std::string result;
    for (unsigned char c : name) {
        if (result.size() == max)
            break;
        if ((c & 0xc0) == 0x80)
            continue;
        if (c >= 'a' && c <= 'z')
            result += char(c - 0x20);
        else if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || c == '_')
            result += char(c);
        else
            result += '_';
    }
    return result;
}

// The ISO 9660 names of the children of a folder, unique in it, and the children in their order.
static void isoNames(Item* folder)
{
    std::vector<Item*>& children = folder->children;
    std::vector<std::string> used;
    for (Item* child : children) {
        if (child->entry->type == Entry::Link)
            continue;
        const std::string& name = child->entry->name;
        std::string base;
        std::string extension;
        const bool folderName = child->entry->type == Entry::Folder;
        const size_t dot = name.rfind('.');
        if (folderName || dot == std::string::npos || dot == 0) {
            base = isoCharacters(name, 30);
        } else {
            extension = isoCharacters(name.substr(dot + 1), 8);
            base = isoCharacters(name.substr(0, dot), 30 - 1 - extension.size());
        }
        std::string candidate = folderName ? base : base + "." + extension;
        for (int number = 1; std::find(used.begin(), used.end(), candidate) != used.end(); number++) {
            const std::string suffix = std::to_string(number);
            const size_t room = 30 - (folderName ? 0 : 1 + extension.size()) - suffix.size();
            const std::string shortened = base.substr(0, std::min(base.size(), room)) + suffix;
            candidate = folderName ? shortened : shortened + "." + extension;
        }
        used.push_back(candidate);
        child->isoName = folderName ? candidate : candidate + ";1";
    }
    std::stable_sort(children.begin(), children.end(), [](const Item* a, const Item* b) { return a->isoName < b->isoName; });
}

static std::string ucs2(const std::u16string& text)
{
    std::string result;
    for (char16_t c : text) {
        result += char(c >> 8);
        result += char(c);
    }
    return result;
}

// Joliet: the names in UCS-2, at most 64 characters, ';' and other characters it does not take as '_'.
static void jolietNames(Item* folder)
{
    std::vector<std::string> used;
    for (Item* child : folder->children) {
        if (child->entry->type == Entry::Link)
            continue;
        std::u16string name = utf16(child->entry->name);
        for (char16_t& c : name) {
            if (c < 0x20 || c == '*' || c == '/' || c == ':' || c == ';' || c == '?' || c == '\\')
                c = '_';
        }
        const bool folderName = child->entry->type == Entry::Folder;
        // A file keeps its extension; the name before it is shortened.
        std::u16string extension;
        const size_t dot = name.rfind(u'.');
        if (!folderName && dot != std::u16string::npos && dot != 0) {
            extension = name.substr(dot);
            name.resize(dot);
        }
        const size_t max = 64 - std::min<size_t>(extension.size(), 64);
        if (name.size() > max)
            name.resize(max);
        std::u16string candidate = name + extension;
        for (int number = 1; std::find(used.begin(), used.end(), ucs2(candidate)) != used.end(); number++) {
            const std::u16string suffix = utf16(std::to_string(number));
            candidate = name.substr(0, std::min(name.size(), max - suffix.size())) + suffix + extension;
        }
        used.push_back(ucs2(candidate));
        child->jolietName = ucs2(candidate) + (folderName ? "" : std::string("\0;\0" "1", 4));
    }
}

static void flatten(Item* item, std::vector<Item*>* items)
{
    items->push_back(item);
    for (Item* child : item->children)
        flatten(child, items);
}

static uint8_t recordLength(size_t identifier)
{
    return (33 + identifier + 1) & ~1;
}

static void directoryRecord(uint8_t* p, const std::string& identifier, uint32_t extent, uint32_t size, bool folder, time_t time)
{
    p[0] = recordLength(identifier.size());
    both32(p + 2, extent);
    both32(p + 10, size);
    tm when;
    gmtime_r(&time, &when);
    p[18] = std::clamp(when.tm_year, 0, 255);
    p[19] = when.tm_mon + 1;
    p[20] = when.tm_mday;
    p[21] = when.tm_hour;
    p[22] = when.tm_min;
    p[23] = when.tm_sec;
    p[24] = 0;
    p[25] = folder ? 2 : 0;
    both16(p + 28, 1);
    p[32] = identifier.size();
    memcpy(p + 33, identifier.data(), identifier.size());
}

// The records of a directory: they do not cross a sector.
static std::vector<std::pair<std::string, const Item*>> directoryEntries(const Item* folder, bool joliet)
{
    std::vector<std::pair<std::string, const Item*>> entries;
    entries.push_back({std::string(1, '\0'), folder});
    entries.push_back({std::string(1, '\1'), folder->parent ? folder->parent : folder});
    std::vector<const Item*> children(folder->children.begin(), folder->children.end());
    if (joliet)
        std::stable_sort(children.begin(), children.end(), [](const Item* a, const Item* b) { return a->jolietName < b->jolietName; });
    for (const Item* child : children) {
        if (child->entry->type != Entry::Link)
            entries.push_back({joliet ? child->jolietName : child->isoName, child});
    }
    return entries;
}

static uint32_t directorySize(const Item* folder, bool joliet)
{
    uint32_t size = 0;
    for (const auto& [identifier, item] : directoryEntries(folder, joliet)) {
        const uint32_t length = recordLength(identifier.size());
        if (size % SectorSize + length > SectorSize)
            size = (size / SectorSize + 1) * SectorSize;
        size += length;
    }
    return sectors(size) * SectorSize;
}

static std::vector<uint8_t> directory(const Item* folder, bool joliet)
{
    std::vector<uint8_t> data(joliet ? folder->jolietSize : folder->isoSize);
    uint32_t at = 0;
    for (const auto& [identifier, item] : directoryEntries(folder, joliet)) {
        const uint32_t length = recordLength(identifier.size());
        if (at % SectorSize + length > SectorSize)
            at = (at / SectorSize + 1) * SectorSize;
        const bool isFolder = item->entry->type == Entry::Folder;
        const uint32_t extent = isFolder ? (joliet ? item->jolietExtent : item->isoExtent) : item->dataExtent;
        const uint32_t size = isFolder ? (joliet ? item->jolietSize : item->isoSize) : item->entry->size;
        directoryRecord(&data[at], identifier, extent, size, isFolder, item->entry->modified);
        at += length;
    }
    return data;
}

// The folders in the order of the path table: by depth, then parent, then name.
static std::vector<Item*> pathOrder(const std::vector<Item*>& items, bool joliet)
{
    std::vector<Item*> order;
    for (Item* item : items) {
        if (item->entry->type == Entry::Folder && !item->parent)
            order.push_back(item);
    }
    for (size_t i = 0; i < order.size(); i++) {
        std::vector<Item*> folders;
        for (Item* child : order[i]->children) {
            if (child->entry->type == Entry::Folder)
                folders.push_back(child);
        }
        std::stable_sort(folders.begin(), folders.end(),
                         [joliet](const Item* a, const Item* b) { return (joliet ? a->jolietName : a->isoName) < (joliet ? b->jolietName : b->isoName); });
        order.insert(order.end(), folders.begin(), folders.end());
    }
    return order;
}

static std::vector<uint8_t> pathTable(const std::vector<Item*>& order, bool joliet, bool bigEndian)
{
    std::vector<uint8_t> table;
    for (const Item* folder : order) {
        const std::string name = folder->parent ? (joliet ? folder->jolietName : folder->isoName) : std::string(1, '\0');
        const size_t at = table.size();
        table.resize(at + 8 + name.size() + (name.size() & 1));
        table[at] = name.size();
        const uint32_t extent = joliet ? folder->jolietExtent : folder->isoExtent;
        uint16_t parent = 1;
        if (folder->parent)
            parent = std::find(order.begin(), order.end(), folder->parent) - order.begin() + 1;
        if (bigEndian) {
            be32(&table[at + 2], extent);
            be16(&table[at + 6], parent);
        } else {
            le32(&table[at + 2], extent);
            le16(&table[at + 6], parent);
        }
        memcpy(&table[at + 8], name.data(), name.size());
    }
    return table;
}

static void volumeDate(uint8_t* p, time_t time)
{
    if (!time) {
        memset(p, '0', 16);
        p[16] = 0;
        return;
    }
    tm when;
    gmtime_r(&time, &when);
    char text[64];
    snprintf(text, sizeof(text), "%04d%02d%02d%02d%02d%02d00", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min,
             when.tm_sec);
    memcpy(p, text, 16);
    p[16] = 0;
}

// The primary volume descriptor, or the supplementary one of Joliet.
static std::vector<uint8_t> volumeDescriptor(const std::vector<Item*>& order, bool joliet, const std::string& name, uint32_t total, uint32_t tableSize,
                                             uint32_t tableL, uint32_t tableM, time_t now)
{
    std::vector<uint8_t> d(SectorSize);
    d[0] = joliet ? 2 : 1;
    memcpy(&d[1], "CD001", 5);
    d[6] = 1;
    auto text = [&](size_t at, size_t length, const std::string& value) {
        if (joliet) {
            for (size_t i = 0; i + 1 < length; i += 2) {
                d[at + i] = 0;
                d[at + i + 1] = ' ';
            }
            memcpy(&d[at], value.data(), std::min(value.size(), length & ~size_t(1)));
        } else {
            memset(&d[at], ' ', length);
            memcpy(&d[at], value.data(), std::min(value.size(), length));
        }
    };
    text(8, 32, "");
    text(40, 32, joliet ? ucs2(utf16(name)).substr(0, 32) : isoCharacters(name, 32));
    both32(&d[80], total);
    if (joliet)
        memcpy(&d[88], "%/E", 3);
    both16(&d[120], 1);
    both16(&d[124], 1);
    both16(&d[128], SectorSize);
    both32(&d[132], tableSize);
    le32(&d[140], tableL);
    be32(&d[148], tableM);
    const Item* root = order[0];
    directoryRecord(&d[156], std::string(1, '\0'), joliet ? root->jolietExtent : root->isoExtent, joliet ? root->jolietSize : root->isoSize, true,
                    root->entry->modified);
    text(190, 128, "");
    text(318, 128, "");
    text(446, 128, "");
    text(574, 128, "");
    text(702, 37, "");
    text(739, 37, "");
    text(776, 37, "");
    volumeDate(&d[813], now);
    volumeDate(&d[830], now);
    volumeDate(&d[847], 0);
    volumeDate(&d[864], 0);
    d[881] = 1;
    return d;
}

// UDF

static uint16_t crcItu(const uint8_t* data, size_t size)
{
    uint16_t crc = 0;
    for (size_t i = 0; i < size; i++) {
        crc ^= data[i] << 8;
        for (int bit = 0; bit < 8; bit++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

// A descriptor tag over `size` bytes of the descriptor.
static void udfTag(uint8_t* d, uint16_t id, uint32_t location, size_t size)
{
    le16(d, id);
    le16(d + 2, 2);
    le16(d + 6, 1);
    le16(d + 10, size - 16);
    le32(d + 12, location);
    le16(d + 8, crcItu(d + 16, size - 16));
    uint8_t sum = 0;
    for (int i = 0; i < 16; i++) {
        if (i != 4)
            sum += d[i];
    }
    d[4] = sum;
}

static void udfTime(uint8_t* p, time_t time)
{
    tm when;
    gmtime_r(&time, &when);
    le16(p, 0x1000);
    le16(p + 2, when.tm_year + 1900);
    p[4] = when.tm_mon + 1;
    p[5] = when.tm_mday;
    p[6] = when.tm_hour;
    p[7] = when.tm_min;
    p[8] = when.tm_sec;
}

// An identifier of an implementation or a domain: flags, then the name.
static void entityId(uint8_t* p, const char* name, const char* suffix = nullptr, size_t suffixSize = 0)
{
    memcpy(p + 1, name, strlen(name));
    if (suffix)
        memcpy(p + 24, suffix, suffixSize);
}

static void charSpec(uint8_t* p)
{
    memcpy(p + 1, "OSTA Compressed Unicode", 23);
}

// A name as OSTA Compressed Unicode: 8 bits when every character fits, else 16.
static std::string osta(const std::string& name)
{
    const std::u16string units = utf16(name);
    const bool wide = std::any_of(units.begin(), units.end(), [](char16_t c) { return c > 0xff; });
    std::string result(1, wide ? 16 : 8);
    for (char16_t c : units) {
        if (wide)
            result += char(c >> 8);
        result += char(c);
    }
    return result;
}

static void dstring(uint8_t* p, size_t length, const std::string& value)
{
    if (value.size() <= 1)
        return;
    const size_t size = std::min(value.size(), length - 1);
    memcpy(p, value.data(), size);
    p[length - 1] = size;
}

// UDF permissions from the mode: the owner may also change attributes and delete.
static uint32_t udfPermissions(const Entry& entry)
{
    if (entry.type == Entry::Link)
        return 0x7fff;
    const uint32_t mode = entry.mode;
    uint32_t permissions = 0;
    permissions |= (mode & 1) | (mode & 2) | (mode & 4);
    permissions |= ((mode >> 3) & 1) << 5 | ((mode >> 4) & 1) << 6 | ((mode >> 5) & 1) << 7;
    permissions |= ((mode >> 6) & 1) << 10 | ((mode >> 7) & 1) << 11 | ((mode >> 8) & 1) << 12;
    return permissions | 0x6000;
}

static std::vector<uint8_t> fileIdentifiers(const Item* folder)
{
    std::vector<uint8_t> data;
    auto add = [&](const Item* item, uint8_t characteristics, const std::string& name) {
        const size_t at = data.size();
        const size_t size = (38 + name.size() + 3) & ~size_t(3);
        data.resize(at + size);
        uint8_t* p = &data[at];
        le16(p + 16, 1);
        p[18] = characteristics;
        p[19] = name.size();
        le32(p + 20, SectorSize);
        le32(p + 24, item->udfEntry);
        memcpy(p + 38, name.data(), name.size());
        udfTag(p, 257, folder->udfDirectory + at / SectorSize, size);
    };
    add(folder->parent ? folder->parent : folder, 0x0a, "");
    for (const Item* child : folder->children)
        add(child, child->entry->type == Entry::Folder ? 2 : 0, osta(child->entry->name));
    return data;
}

// The data of a link in UDF: its path as components.
static std::vector<uint8_t> linkComponents(const std::string& target)
{
    std::vector<uint8_t> data;
    size_t at = 0;
    if (!target.empty() && target[0] == '/') {
        data.insert(data.end(), {2, 0, 0, 0});
        at = 1;
    }
    while (at <= target.size()) {
        size_t end = target.find('/', at);
        if (end == std::string::npos)
            end = target.size();
        const std::string part = target.substr(at, end - at);
        if (part == "..")
            data.insert(data.end(), {3, 0, 0, 0});
        else if (part == ".")
            data.insert(data.end(), {4, 0, 0, 0});
        else if (!part.empty()) {
            const std::string name = osta(part);
            data.push_back(5);
            data.push_back(name.size());
            data.push_back(0);
            data.push_back(0);
            data.insert(data.end(), name.begin(), name.end());
        }
        at = end + 1;
    }
    return data;
}

static std::vector<uint8_t> fileEntry(const Item* item)
{
    const Entry& entry = *item->entry;
    std::vector<uint8_t> d(SectorSize);
    uint8_t* p = d.data();
    le16(p + 16 + 4, 4);
    le16(p + 16 + 8, 1);
    p[16 + 11] = entry.type == Entry::Folder ? 4 : entry.type == Entry::Link ? 12 : 5;
    le32(p + 36, entry.uid);
    le32(p + 40, entry.gid);
    le32(p + 44, udfPermissions(entry));
    uint16_t links = 1;
    if (entry.type == Entry::Folder) {
        for (const Item* child : item->children)
            links += child->entry->type == Entry::Folder;
    }
    le16(p + 48, links);
    uint64_t length;
    uint32_t extent;
    if (entry.type == Entry::Folder) {
        length = item->udfDirectorySize;
        extent = item->udfDirectory;
    } else if (entry.type == Entry::Link) {
        length = item->linkData.size();
        extent = item->dataExtent - PartitionStart;
    } else {
        length = entry.size;
        extent = item->dataExtent - PartitionStart;
    }
    le64(p + 56, length);
    le64(p + 64, sectors(length));
    udfTime(p + 72, entry.accessed);
    udfTime(p + 84, entry.modified);
    udfTime(p + 96, entry.changed);
    le32(p + 108, 1);
    entityId(p + 128, "*reef");
    le64(p + 160, item->uniqueId);
    size_t size = 176;
    if (length) {
        le32(p + 172, 8);
        le32(p + 176, length);
        le32(p + 180, extent);
        size += 8;
    }
    udfTag(p, 261, item->udfEntry, size);
    return d;
}

bool makeHybrid(const Entry& root, const HybridOptions& options, const std::string& output, const Progress& progress, std::string* error)
{
    // The items, in the order of the walk.
    std::vector<Item> storage;
    std::vector<const Entry*> entries;
    std::vector<std::pair<const Entry*, size_t>> stack = {{&root, size_t(-1)}};
    std::vector<size_t> parents;
    while (!stack.empty()) {
        auto [entry, parent] = stack.back();
        stack.pop_back();
        storage.push_back({entry, nullptr, {}, {}, {}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}});
        parents.push_back(parent);
        const size_t self = storage.size() - 1;
        for (auto child = entry->children.rbegin(); child != entry->children.rend(); ++child)
            stack.push_back({&*child, self});
    }
    for (size_t i = 0; i < storage.size(); i++) {
        if (parents[i] != size_t(-1)) {
            storage[i].parent = &storage[parents[i]];
            storage[parents[i]].children.push_back(&storage[i]);
        }
    }
    std::vector<Item*> items;
    for (Item& item : storage)
        items.push_back(&item);
    for (Item* item : items) {
        if (item->entry->type != Entry::Folder)
            continue;
        if (options.iso || options.joliet)
            isoNames(item);
        if (options.joliet)
            jolietNames(item);
    }
    if (options.iso || options.joliet) {
        for (Item* item : items) {
            if (item->entry->type == Entry::File && item->entry->size > 0xffffffff) {
                *error = item->entry->path + ": the file is too large for ISO 9660";
                return false;
            }
        }
    }
    // Joliet needs the primary volume of ISO 9660 beside it.
    const bool iso = options.iso || options.joliet;
    uint32_t next = 16;
    const uint32_t isoDescriptor = iso ? next++ : 0;
    const uint32_t jolietDescriptor = options.joliet ? next++ : 0;
    const uint32_t terminator = iso ? next++ : next - 1;
    uint32_t udfFiles = 0;
    uint32_t udfFolders = 0;
    if (options.udf) {
        next = PartitionStart;
        // The FSD and its terminator, then a file entry for every item, then the directories.
        uint32_t block = 2;
        for (Item* item : items) {
            item->udfEntry = block++;
            item->uniqueId = item->parent ? 16 + (item - items[0]) - 1 : 0;
            if (item->entry->type == Entry::Folder)
                udfFolders++;
            else
                udfFiles++;
        }
        for (Item* item : items) {
            if (item->entry->type != Entry::Folder)
                continue;
            item->udfDirectory = block;
            item->udfDirectorySize = fileIdentifiers(item).size();
            block += sectors(item->udfDirectorySize);
        }
        next = PartitionStart + block;
    }
    std::vector<Item*> isoOrder;
    std::vector<Item*> jolietOrder;
    uint32_t isoTable[2] = {};
    uint32_t jolietTable[2] = {};
    std::vector<uint8_t> isoTables[2];
    std::vector<uint8_t> jolietTables[2];
    if (iso) {
        isoOrder = pathOrder(items, false);
        for (Item* folder : isoOrder) {
            folder->isoSize = directorySize(folder, false);
            folder->isoExtent = next;
            next += folder->isoSize / SectorSize;
        }
        isoTables[0] = pathTable(isoOrder, false, false);
        isoTable[0] = next;
        next += sectors(isoTables[0].size());
        isoTable[1] = next;
        next += sectors(isoTables[0].size());
    }
    if (options.joliet) {
        jolietOrder = pathOrder(items, true);
        for (Item* folder : jolietOrder) {
            folder->jolietSize = directorySize(folder, true);
            folder->jolietExtent = next;
            next += folder->jolietSize / SectorSize;
        }
        jolietTables[0] = pathTable(jolietOrder, true, false);
        jolietTable[0] = next;
        next += sectors(jolietTables[0].size());
        jolietTable[1] = next;
        next += sectors(jolietTables[0].size());
    }
    for (Item* item : items) {
        if (item->entry->type == Entry::Link && options.udf)
            item->linkData = linkComponents(item->entry->target);
        const uint64_t size = item->entry->type == Entry::File ? item->entry->size : item->linkData.size();
        if (item->entry->type == Entry::Folder || !size)
            continue;
        item->dataExtent = next;
        item->dataSectors = sectors(size);
        next += item->dataSectors;
    }
    if (iso) {
        isoTables[0] = pathTable(isoOrder, false, false);
        isoTables[1] = pathTable(isoOrder, false, true);
    }
    if (options.joliet) {
        jolietTables[0] = pathTable(jolietOrder, true, false);
        jolietTables[1] = pathTable(jolietOrder, true, true);
    }
    const uint32_t total = next + (options.udf ? 1 : 0);
    const time_t now = time(nullptr);

    const int fd = open(output.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
    if (fd < 0) {
        *error = output + ": " + strerror(errno);
        return false;
    }
    bool written = ftruncate(fd, uint64_t(total) * SectorSize) == 0;
    auto put = [&](uint32_t sector, const std::vector<uint8_t>& data) {
        if (written && !data.empty())
            written = writeAll(fd, uint64_t(sector) * SectorSize, data.data(), data.size());
    };
    if (iso) {
        put(isoDescriptor, volumeDescriptor(isoOrder, false, options.volumeName, total, isoTables[0].size(), isoTable[0], isoTable[1], now));
        for (const Item* folder : isoOrder)
            put(folder->isoExtent, directory(folder, false));
        put(isoTable[0], isoTables[0]);
        put(isoTable[1], isoTables[1]);
    }
    if (options.joliet) {
        put(jolietDescriptor,
            volumeDescriptor(jolietOrder, true, options.volumeName, total, jolietTables[0].size(), jolietTable[0], jolietTable[1], now));
        for (const Item* folder : jolietOrder)
            put(folder->jolietExtent, directory(folder, true));
        put(jolietTable[0], jolietTables[0]);
        put(jolietTable[1], jolietTables[1]);
    }
    std::vector<uint8_t> end(SectorSize);
    end[0] = 0xff;
    memcpy(&end[1], "CD001", 5);
    end[6] = 1;
    if (iso)
        put(terminator, end);
    if (options.udf) {
        const uint32_t partitionLength = total - 1 - PartitionStart;
        auto sector = [](uint8_t value) { return std::vector<uint8_t>(SectorSize, value); };
        std::vector<uint8_t> vrs = sector(0);
        for (const char* id : {"BEA01", "NSR02", "TEA01"}) {
            vrs[0] = 0;
            memcpy(&vrs[1], id, 5);
            vrs[6] = 1;
            put(terminator + 1 + (id[0] == 'B' ? 0 : id[0] == 'N' ? 1 : 2), vrs);
        }
        char hex[16];
        snprintf(hex, sizeof(hex), "%08X", unsigned(now));
        const std::string volumeId = osta(options.volumeName);
        const std::string setId = std::string(1, volumeId[0]) + (volumeId[0] == 16 ? ucs2(utf16(hex)) : std::string(hex)) + volumeId.substr(1);
        for (uint32_t base : {32u, 48u}) {
            std::vector<uint8_t> d = sector(0);
            uint8_t* p = d.data();
            dstring(p + 24, 32, volumeId);
            le16(p + 56, 1);
            le16(p + 58, 1);
            le16(p + 60, 2);
            le16(p + 62, 2);
            le32(p + 64, 1);
            le32(p + 68, 1);
            dstring(p + 72, 128, setId);
            charSpec(p + 200);
            charSpec(p + 264);
            udfTime(p + 376, now);
            entityId(p + 388, "*reef");
            udfTag(p, 1, base, 512);
            put(base, d);

            d = sector(0);
            p = d.data();
            le32(p + 16, 1);
            entityId(p + 20, "*UDF LV Info", "\x02\x01", 2);
            charSpec(p + 52);
            dstring(p + 116, 128, volumeId);
            entityId(p + 352, "*reef");
            udfTag(p, 4, base + 1, 512);
            put(base + 1, d);

            d = sector(0);
            p = d.data();
            le32(p + 16, 2);
            le16(p + 20, 1);
            entityId(p + 24, "+NSR02");
            le32(p + 184, 1);
            le32(p + 188, PartitionStart);
            le32(p + 192, partitionLength);
            entityId(p + 196, "*reef");
            udfTag(p, 5, base + 2, 512);
            put(base + 2, d);

            d = sector(0);
            p = d.data();
            le32(p + 16, 3);
            charSpec(p + 20);
            dstring(p + 84, 128, volumeId);
            le32(p + 212, SectorSize);
            entityId(p + 216, "*OSTA UDF Compliant", "\x02\x01", 2);
            le32(p + 248, SectorSize);
            le32(p + 264, 6);
            le32(p + 268, 1);
            entityId(p + 272, "*reef");
            le32(p + 432, 0x1000);
            le32(p + 436, 64);
            p[440] = 1;
            p[441] = 6;
            le16(p + 442, 1);
            udfTag(p, 6, base + 3, 446);
            put(base + 3, d);

            d = sector(0);
            p = d.data();
            le32(p + 16, 4);
            udfTag(p, 7, base + 4, 24);
            put(base + 4, d);

            d = sector(0);
            udfTag(d.data(), 8, base + 5, 512);
            put(base + 5, d);
        }
        std::vector<uint8_t> d = sector(0);
        uint8_t* p = d.data();
        udfTime(p + 16, now);
        le32(p + 28, 1);
        le64(p + 40, 16 + items.size() - 1);
        le32(p + 72, 1);
        le32(p + 76, 46);
        le32(p + 80, 0);
        le32(p + 84, partitionLength);
        entityId(p + 88, "*reef");
        le32(p + 120, udfFiles);
        le32(p + 124, udfFolders);
        le16(p + 128, 0x102);
        le16(p + 130, 0x102);
        le16(p + 132, 0x102);
        udfTag(p, 9, 64, 134);
        put(64, d);
        d = sector(0);
        udfTag(d.data(), 8, 65, 512);
        put(65, d);

        for (uint32_t at : {AnchorSector, total - 1}) {
            d = sector(0);
            p = d.data();
            le32(p + 16, 0x8000);
            le32(p + 20, 32);
            le32(p + 24, 0x8000);
            le32(p + 28, 48);
            udfTag(p, 2, at, 512);
            put(at, d);
        }
        d = sector(0);
        p = d.data();
        udfTime(p + 16, now);
        le16(p + 28, 3);
        le16(p + 30, 3);
        le32(p + 32, 1);
        le32(p + 36, 1);
        charSpec(p + 48);
        dstring(p + 112, 128, volumeId);
        charSpec(p + 240);
        dstring(p + 304, 32, volumeId);
        le32(p + 400, SectorSize);
        le32(p + 404, items[0]->udfEntry);
        entityId(p + 416, "*OSTA UDF Compliant", "\x02\x01", 2);
        udfTag(p, 256, 0, 512);
        put(PartitionStart, d);
        d = sector(0);
        udfTag(d.data(), 8, 1, 512);
        put(PartitionStart + 1, d);
        for (const Item* item : items) {
            put(PartitionStart + item->udfEntry, fileEntry(item));
            if (item->entry->type == Entry::Folder)
                put(PartitionStart + item->udfDirectory, fileIdentifiers(item));
            if (!item->linkData.empty())
                put(item->dataExtent, item->linkData);
        }
    }
    if (!written) {
        *error = output + ": " + strerror(errno);
        close(fd);
        unlink(output.c_str());
        return false;
    }
    const uint64_t size = root.totalSize();
    uint64_t done = 0;
    bool copied = true;
    for (const Item* item : items) {
        if (!copied)
            break;
        if (item->entry->type == Entry::File && item->dataSectors)
            copied = copyFile(item->entry->path, item->entry->size, fd, uint64_t(item->dataExtent) * SectorSize, progress, &done, size, error);
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
