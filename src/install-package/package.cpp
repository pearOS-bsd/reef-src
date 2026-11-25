// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "package.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <sys/stat.h>
#include <unistd.h>

#include <libxml/parser.h>
#include <lzma.h>
#include <zlib.h>

namespace cutefish::reef::package {

// Bounds on what a package may ask for.
static constexpr uint64_t MaximumFile = 64 << 20;
static constexpr uint64_t MaximumChunk = 256 << 20;
static constexpr size_t BlockSize = 65536;
static constexpr std::string_view Applications = "/Applications/";

struct Item
{
    Section section;
    bool directory = false;
};

using Items = std::map<std::string, Item>;

class Stream
{
public:
    virtual ~Stream() = default;
    // Like read(2).
    virtual ssize_t read(void* buffer, size_t size) = 0;

    bool readAll(void* buffer, size_t size)
    {
        unsigned char* at = static_cast<unsigned char*>(buffer);
        while (size > 0) {
            const ssize_t count = read(at, size);
            if (count <= 0)
                return false;
            at += count;
            size -= size_t(count);
        }
        return true;
    }

    bool skip(uint64_t size)
    {
        unsigned char buffer[BlockSize];
        while (size > 0) {
            const size_t count = size_t(std::min<uint64_t>(size, sizeof buffer));
            if (!readAll(buffer, count))
                return false;
            size -= count;
        }
        return true;
    }
};

// A part of a file.
class RangeStream : public Stream
{
public:
    RangeStream(int fd, uint64_t offset, uint64_t length)
        : m_fd(fd)
        , m_offset(offset)
        , m_length(length)
    {
    }

    ssize_t read(void* buffer, size_t size) override
    {
        size = std::min<uint64_t>(size, m_length);
        if (size == 0)
            return 0;
        const ssize_t count = pread(m_fd, buffer, size, off_t(m_offset));
        if (count <= 0)
            return -1;
        m_offset += uint64_t(count);
        m_length -= uint64_t(count);
        return count;
    }

private:
    int m_fd;
    uint64_t m_offset;
    uint64_t m_length;
};

// Some bytes, then another stream.
class PrefixedStream : public Stream
{
public:
    PrefixedStream(std::vector<unsigned char> prefix, Stream& source)
        : m_prefix(std::move(prefix))
        , m_source(source)
    {
    }

    ssize_t read(void* buffer, size_t size) override
    {
        if (m_position < m_prefix.size()) {
            size = std::min(size, m_prefix.size() - m_position);
            memcpy(buffer, m_prefix.data() + m_position, size);
            m_position += size;
            return ssize_t(size);
        }
        return m_source.read(buffer, size);
    }

private:
    std::vector<unsigned char> m_prefix;
    size_t m_position = 0;
    Stream& m_source;
};

class InflateStream : public Stream
{
public:
    explicit InflateStream(Stream& source)
        : m_source(source)
    {
        m_valid = inflateInit2(&m_stream, 15 + 32) == Z_OK;
    }

    ~InflateStream() override { inflateEnd(&m_stream); }

    ssize_t read(void* buffer, size_t size) override
    {
        if (!m_valid)
            return -1;
        size = std::min<size_t>(size, 1 << 30);
        m_stream.next_out = static_cast<Bytef*>(buffer);
        m_stream.avail_out = uInt(size);
        while (m_stream.avail_out == uInt(size) && !m_ended) {
            if (m_stream.avail_in == 0) {
                const ssize_t count = m_source.read(m_buffer, sizeof m_buffer);
                if (count <= 0)
                    return -1;
                m_stream.next_in = m_buffer;
                m_stream.avail_in = uInt(count);
            }
            const int result = inflate(&m_stream, Z_NO_FLUSH);
            if (result == Z_STREAM_END)
                m_ended = true;
            else if (result != Z_OK)
                return -1;
        }
        return ssize_t(size - m_stream.avail_out);
    }

private:
    Stream& m_source;
    z_stream m_stream = {};
    unsigned char m_buffer[BlockSize];
    bool m_valid = false;
    bool m_ended = false;
};

static uint64_t bigEndian(const unsigned char* bytes, size_t count)
{
    uint64_t value = 0;
    for (size_t i = 0; i < count; i++)
        value = value << 8 | bytes[i];
    return value;
}

// Chunks of xz, each with its sizes.
class PbzxStream : public Stream
{
public:
    explicit PbzxStream(Stream& source)
        : m_source(source)
    {
    }

    ~PbzxStream() override { lzma_end(&m_stream); }

    bool start()
    {
        unsigned char header[12];
        if (!m_source.readAll(header, sizeof header) || memcmp(header, "pbzx", 4) != 0)
            return false;
        m_chunk = bigEndian(header + 4, 8);
        return true;
    }

    ssize_t read(void* buffer, size_t size) override
    {
        while (m_position == m_data.size()) {
            if (!next())
                return m_failed ? -1 : 0;
        }
        size = std::min(size, m_data.size() - m_position);
        memcpy(buffer, m_data.data() + m_position, size);
        m_position += size;
        return ssize_t(size);
    }

private:
    bool next()
    {
        unsigned char header[16];
        const ssize_t first = m_source.read(header, 1);
        if (first == 0)
            return false;
        if (first < 0 || !m_source.readAll(header + 1, sizeof header - 1)) {
            m_failed = true;
            return false;
        }
        const uint64_t size = bigEndian(header, 8);
        const uint64_t length = bigEndian(header + 8, 8);
        if (length > MaximumChunk || size > MaximumChunk) {
            m_failed = true;
            return false;
        }
        std::vector<unsigned char> chunk(length);
        if (!m_source.readAll(chunk.data(), chunk.size())) {
            m_failed = true;
            return false;
        }
        m_position = 0;
        if (chunk.size() < 6 || memcmp(chunk.data(), "\xfd"
                                                     "7zXZ\0",
                                    6)
                != 0) {
            m_data = std::move(chunk);
            return true;
        }
        m_data.assign(std::max(size, m_chunk), 0);
        lzma_end(&m_stream);
        m_stream = LZMA_STREAM_INIT;
        if (lzma_stream_decoder(&m_stream, UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK) {
            m_failed = true;
            return false;
        }
        m_stream.next_in = chunk.data();
        m_stream.avail_in = chunk.size();
        m_stream.next_out = m_data.data();
        m_stream.avail_out = m_data.size();
        if (lzma_code(&m_stream, LZMA_FINISH) != LZMA_STREAM_END) {
            m_failed = true;
            return false;
        }
        m_data.resize(m_data.size() - m_stream.avail_out);
        return true;
    }

    Stream& m_source;
    uint64_t m_chunk = 0;
    std::vector<unsigned char> m_data;
    size_t m_position = 0;
    lzma_stream m_stream = LZMA_STREAM_INIT;
    bool m_failed = false;
};

// A descriptor closed at the end of its scope.
struct Descriptor
{
    int fd;
    ~Descriptor()
    {
        if (fd >= 0)
            close(fd);
    }
};

struct Document
{
    xmlDoc* document;
    ~Document()
    {
        if (document)
            xmlFreeDoc(document);
    }
};

static constexpr int ParseOptions = XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NONET;

static xmlDoc* parse(const std::string& text)
{
    return xmlReadMemory(text.data(), int(text.size()), "member.xml", nullptr, ParseOptions);
}

static xmlNode* child(const xmlNode* parent, const char* name)
{
    for (xmlNode* node = parent ? parent->children : nullptr; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, BAD_CAST name) == 0)
            return node;
    }
    return nullptr;
}

static std::string attribute(xmlNode* node, const char* name)
{
    xmlChar* value = node ? xmlGetProp(node, BAD_CAST name) : nullptr;
    std::string text = value ? reinterpret_cast<const char*>(value) : "";
    xmlFree(value);
    return text;
}

static std::string childText(const xmlNode* parent, const char* name)
{
    for (const xmlNode* node = parent->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, BAD_CAST name) == 0) {
            xmlChar* value = xmlNodeGetContent(node);
            std::string text = value ? reinterpret_cast<const char*>(value) : "";
            xmlFree(value);
            return text;
        }
    }
    return {};
}

static bool readAt(int fd, uint64_t offset, void* buffer, size_t size)
{
    char* at = static_cast<char*>(buffer);
    while (size > 0) {
        const ssize_t count = pread(fd, at, size, off_t(offset));
        if (count <= 0)
            return false;
        at += count;
        offset += uint64_t(count);
        size -= size_t(count);
    }
    return true;
}

static std::optional<std::string> readSection(int fd, const Section& section)
{
    if (section.length > MaximumFile)
        return std::nullopt;
    std::string data(section.length, '\0');
    if (!readAt(fd, section.offset, data.data(), data.size()))
        return std::nullopt;
    if (section.encoding != "application/x-gzip")
        return data;
    std::string text;
    z_stream stream = {};
    if (inflateInit(&stream) != Z_OK)
        return std::nullopt;
    stream.next_in = reinterpret_cast<Bytef*>(data.data());
    stream.avail_in = uInt(data.size());
    char buffer[BlockSize];
    int result = Z_OK;
    while (result == Z_OK) {
        stream.next_out = reinterpret_cast<Bytef*>(buffer);
        stream.avail_out = sizeof buffer;
        result = inflate(&stream, Z_NO_FLUSH);
        text.append(buffer, sizeof buffer - stream.avail_out);
        if (text.size() > MaximumFile)
            result = Z_DATA_ERROR;
    }
    inflateEnd(&stream);
    return result == Z_STREAM_END ? std::optional<std::string>(text) : std::nullopt;
}

static void readItems(const xmlNode* parent, const std::string& prefix, uint64_t heap, Items& items)
{
    for (const xmlNode* node = parent->children; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE || xmlStrcmp(node->name, BAD_CAST "file") != 0)
            continue;
        const std::string name = childText(node, "name");
        if (name.empty() || name == "." || name == ".." || name.find('/') != std::string::npos)
            continue;
        const std::string path = prefix + name;
        Item item;
        item.directory = childText(node, "type") == "directory";
        if (xmlNode* data = child(node, "data")) {
            item.section.offset = heap + strtoull(childText(data, "offset").c_str(), nullptr, 10);
            item.section.length = strtoull(childText(data, "length").c_str(), nullptr, 10);
            item.section.encoding = attribute(child(data, "encoding"), "style");
        }
        items[path] = item;
        if (item.directory)
            readItems(node, path + "/", heap, items);
    }
}

static std::optional<Items> readItems(int fd)
{
    unsigned char header[28];
    if (!readAt(fd, 0, header, sizeof header) || memcmp(header, "xar!", 4) != 0)
        return std::nullopt;
    const uint64_t headerSize = bigEndian(header + 4, 2);
    const uint64_t packedSize = bigEndian(header + 8, 8);
    const uint64_t size = bigEndian(header + 16, 8);
    if (headerSize < sizeof header || packedSize > MaximumFile || size == 0 || size > MaximumFile)
        return std::nullopt;
    std::vector<unsigned char> packed(packedSize);
    if (!readAt(fd, headerSize, packed.data(), packed.size()))
        return std::nullopt;
    std::vector<unsigned char> text(size);
    uLongf length = text.size();
    if (uncompress(text.data(), &length, packed.data(), packed.size()) != Z_OK)
        return std::nullopt;
    const Document document = {xmlReadMemory(reinterpret_cast<const char*>(text.data()), int(length), "toc.xml", nullptr, ParseOptions)};
    const xmlNode* root = document.document ? xmlDocGetRootElement(document.document) : nullptr;
    const xmlNode* toc = child(root, "toc");
    if (!toc)
        return std::nullopt;
    Items items;
    readItems(toc, "", headerSize + packedSize, items);
    return items;
}

static std::optional<Component> readComponent(int fd, const Items& items, const std::string& prefix)
{
    const auto info = items.find(prefix + "PackageInfo");
    if (info == items.end())
        return std::nullopt;
    const auto text = readSection(fd, info->second.section);
    const Document document = {text ? parse(*text) : parse({})};
    if (!document.document)
        return std::nullopt;
    xmlNode* root = xmlDocGetRootElement(document.document);
    if (!root || xmlStrcmp(root->name, BAD_CAST "pkg-info") != 0)
        return std::nullopt;
    Component component;
    component.identifier = attribute(root, "identifier");
    component.version = attribute(root, "version");
    component.location = attribute(root, "install-location");
    if (component.location.empty())
        component.location = "/";
    if (xmlNode* scripts = child(root, "scripts"))
        component.hasScripts = child(scripts, "preinstall") || child(scripts, "postinstall");
    if (!component.hasScripts)
        component.hasScripts = items.find(prefix + "Scripts") != items.end();
    const auto payload = items.find(prefix + "Payload");
    if (payload != items.end() && !payload->second.directory) {
        component.payload = payload->second.section;
        component.hasPayload = true;
    }
    return component;
}

// The application that a file of the payload belongs to, and its path in it; false for what is not in an application.
static bool target(const std::string& location, const std::string& name, std::string& application, std::string& path)
{
    std::string file = name;
    if (file.rfind("./", 0) == 0)
        file.erase(0, 2);
    else if (file == ".")
        file.clear();
    std::string full = location;
    if (full.empty() || full.back() != '/')
        full += '/';
    full += file;
    std::string normal;
    size_t start = 0;
    while (start <= full.size()) {
        const size_t end = std::min(full.find('/', start), full.size());
        const std::string part(full, start, end - start);
        if (part == "..")
            return false;
        if (!part.empty() && part != ".")
            normal += '/' + part;
        start = end + 1;
    }
    if (normal.rfind(Applications, 0) != 0)
        return false;
    const std::string rest = normal.substr(Applications.size());
    const size_t slash = rest.find('/');
    application = rest.substr(0, slash);
    path = slash == std::string::npos ? std::string() : rest.substr(slash + 1);
    return application.size() > 4 && application.ends_with(".app");
}

// An entry of the cpio archive.
struct Entry
{
    std::string name;
    uint32_t mode = 0;
    uint32_t links = 0;
    uint64_t inode = 0;
    uint64_t time = 0;
    uint64_t size = 0;
    // The newer format pads to four bytes.
    bool padded = false;
};

static bool number(const unsigned char* text, size_t count, int base, uint64_t& value)
{
    value = 0;
    for (size_t i = 0; i < count; i++) {
        const int c = text[i];
        const int digit = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10 : base;
        if (digit >= base)
            return false;
        value = value * uint64_t(base) + uint64_t(digit);
    }
    return true;
}

static bool readEntry(Stream& stream, uint64_t& position, Entry& entry)
{
    char magic[6];
    if (!stream.readAll(magic, sizeof magic))
        return false;
    uint64_t nameSize = 0;
    uint64_t value = 0;
    const std::string_view kind(magic, sizeof magic);
    if (kind == "070707") {
        unsigned char header[70];
        if (!stream.readAll(header, sizeof header))
            return false;
        bool good = number(header + 6, 6, 8, entry.inode) && number(header + 12, 6, 8, value);
        entry.mode = uint32_t(value);
        good = good && number(header + 30, 6, 8, value);
        entry.links = uint32_t(value);
        good = good && number(header + 42, 11, 8, entry.time) && number(header + 53, 6, 8, nameSize) && number(header + 59, 11, 8, entry.size);
        if (!good)
            return false;
        position += 76;
        entry.padded = false;
    } else if (kind == "070701" || kind == "070702") {
        unsigned char header[104];
        if (!stream.readAll(header, sizeof header))
            return false;
        bool good = number(header, 8, 16, entry.inode) && number(header + 8, 8, 16, value);
        entry.mode = uint32_t(value);
        good = good && number(header + 32, 8, 16, value);
        entry.links = uint32_t(value);
        good = good && number(header + 40, 8, 16, entry.time) && number(header + 48, 8, 16, entry.size) && number(header + 88, 8, 16, nameSize);
        if (!good)
            return false;
        position += 110;
        entry.padded = true;
    } else {
        return false;
    }
    if (nameSize == 0 || nameSize > 4096)
        return false;
    std::string name(nameSize, '\0');
    if (!stream.readAll(name.data(), name.size()))
        return false;
    position += nameSize;
    name.resize(strlen(name.c_str()));
    entry.name = name;
    if (entry.padded && position & 3) {
        const uint64_t padding = 4 - (position & 3);
        if (!stream.skip(padding))
            return false;
        position += padding;
    }
    return true;
}

static bool extractPayload(Stream& stream, const std::string& location, const std::string& folder, Contents& contents,
                           std::set<std::string>& applications, uint64_t& written, const std::function<void(uint64_t)>& progress)
{
    // Paths that are symbolic links: nothing is written through them.
    std::set<std::string> links;
    // Files with several names.
    std::map<uint64_t, std::string> files;
    std::map<uint64_t, std::vector<std::string>> waiting;
    std::vector<std::pair<std::string, Entry>> directories;
    uint64_t position = 0;
    for (;;) {
        Entry entry;
        if (!readEntry(stream, position, entry)) {
            contents.error = "The payload is damaged.";
            return false;
        }
        if (entry.name == "TRAILER!!!")
            break;
        const uint32_t type = entry.mode & S_IFMT;
        std::string application;
        std::string inside;
        bool wanted = target(location, entry.name, application, inside) && (type == S_IFREG || type == S_IFLNK || type == S_IFDIR);
        const std::string path = !wanted ? std::string() : inside.empty() ? application : application + "/" + inside;
        for (size_t slash = path.find('/'); slash != std::string::npos && wanted; slash = path.find('/', slash + 1))
            wanted = links.find(path.substr(0, slash)) == links.end();
        if (wanted)
            applications.insert(application);
        else if (type != S_IFDIR)
            contents.skipped++;
        const std::string full = folder + "/" + path;
        if (wanted && !folder.empty()) {
            for (size_t slash = path.find('/'); slash != std::string::npos; slash = path.find('/', slash + 1))
                mkdir((folder + "/" + path.substr(0, slash)).c_str(), 0755);
        }
        std::string linkTarget;
        if (type == S_IFLNK) {
            if (entry.size > 4096) {
                contents.error = "The payload is damaged.";
                return false;
            }
            linkTarget.resize(entry.size);
            if (!stream.readAll(linkTarget.data(), linkTarget.size())) {
                contents.error = "The payload is damaged.";
                return false;
            }
            if (wanted) {
                if (!folder.empty()) {
                    unlink(full.c_str());
                    if (symlink(linkTarget.c_str(), full.c_str()) != 0) {
                        contents.error = "Cannot write " + path + ": " + strerror(errno);
                        return false;
                    }
                }
                links.insert(path);
            }
        } else if (wanted && type == S_IFREG && !folder.empty()) {
            if (entry.links > 1 && entry.size == 0) {
                if (files.find(entry.inode) != files.end()) {
                    unlink(full.c_str());
                    if (link(files[entry.inode].c_str(), full.c_str()) != 0) {
                        contents.error = "Cannot write " + path + ": " + strerror(errno);
                        return false;
                    }
                } else {
                    waiting[entry.inode].push_back(full);
                }
            } else {
                const int fd = ::open(full.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW | O_CLOEXEC, 0600);
                if (fd < 0) {
                    contents.error = "Cannot write " + path + ": " + strerror(errno);
                    return false;
                }
                char buffer[BlockSize];
                bool good = true;
                uint64_t left = entry.size;
                while (left > 0 && good) {
                    const size_t count = size_t(std::min<uint64_t>(left, sizeof buffer));
                    good = stream.readAll(buffer, count);
                    for (size_t done = 0; good && done < count;) {
                        const ssize_t size = ::write(fd, buffer + done, count - done);
                        good = size > 0;
                        done += size_t(std::max<ssize_t>(size, 0));
                    }
                    if (good) {
                        left -= count;
                        written += count;
                    }
                    if (progress)
                        progress(written);
                }
                const timespec times[2] = {{time_t(entry.time), 0}, {time_t(entry.time), 0}};
                good = good && fchmod(fd, entry.mode & 0777) == 0 && futimens(fd, times) == 0;
                close(fd);
                if (!good) {
                    contents.error = "Cannot write " + path;
                    return false;
                }
                if (entry.links > 1) {
                    files[entry.inode] = full;
                    for (const std::string& other : waiting[entry.inode]) {
                        unlink(other.c_str());
                        link(full.c_str(), other.c_str());
                    }
                    waiting.erase(entry.inode);
                }
            }
        } else {
            if (wanted && type == S_IFDIR && !folder.empty()) {
                if (mkdir(full.c_str(), 0700) != 0 && errno != EEXIST) {
                    contents.error = "Cannot write " + path + ": " + strerror(errno);
                    return false;
                }
                struct stat status;
                if (lstat(full.c_str(), &status) != 0 || !S_ISDIR(status.st_mode)) {
                    contents.error = "Cannot write " + path;
                    return false;
                }
                directories.emplace_back(full, entry);
            }
            if (!stream.skip(entry.size)) {
                contents.error = "The payload is damaged.";
                return false;
            }
        }
        position += entry.size;
        if (entry.padded && position & 3) {
            const uint64_t padding = 4 - (position & 3);
            if (!stream.skip(padding)) {
                contents.error = "The payload is damaged.";
                return false;
            }
            position += padding;
        }
    }
    // The inner ones first, so that their dates stay.
    std::sort(directories.begin(), directories.end(),
        [](const auto& first, const auto& second) { return first.first.size() > second.first.size(); });
    for (const auto& [path, entry] : directories) {
        chmod(path.c_str(), (entry.mode & 0777) | 0700);
        const timespec times[2] = {{time_t(entry.time), 0}, {time_t(entry.time), 0}};
        utimensat(AT_FDCWD, path.c_str(), times, AT_SYMLINK_NOFOLLOW);
    }
    return true;
}

static Contents process(const Package& package, const std::string& folder, const std::function<void(uint64_t)>& progress)
{
    Contents contents;
    const Descriptor file = {::open(package.path.c_str(), O_RDONLY | O_CLOEXEC)};
    if (file.fd < 0) {
        contents.error = std::string("Cannot open the package: ") + strerror(errno);
        return contents;
    }
    std::set<std::string> applications;
    uint64_t written = 0;
    for (const Component& component : package.components) {
        contents.hasScripts = contents.hasScripts || component.hasScripts;
        if (!component.hasPayload)
            continue;
        RangeStream range(file.fd, component.payload.offset, component.payload.length);
        std::unique_ptr<Stream> decoded;
        Stream* source = &range;
        if (component.payload.encoding == "application/x-gzip") {
            decoded = std::make_unique<InflateStream>(range);
            source = decoded.get();
        }
        std::vector<unsigned char> magic(4);
        if (!source->readAll(magic.data(), magic.size())) {
            contents.error = "The payload is damaged.";
            return contents;
        }
        PrefixedStream prefixed(magic, *source);
        std::unique_ptr<Stream> opened;
        Stream* payload = &prefixed;
        if (magic[0] == 0x1f && magic[1] == 0x8b) {
            opened = std::make_unique<InflateStream>(prefixed);
            payload = opened.get();
        } else if (memcmp(magic.data(), "pbzx", 4) == 0) {
            auto pbzx = std::make_unique<PbzxStream>(prefixed);
            if (!pbzx->start()) {
                contents.error = "The payload is damaged.";
                return contents;
            }
            opened = std::move(pbzx);
            payload = opened.get();
        } else if (memcmp(magic.data(), "0707", 4) != 0) {
            contents.error = "The payload is in a format that cannot be read.";
            return contents;
        }
        if (!extractPayload(*payload, component.location, folder, contents, applications, written, progress))
            return contents;
    }
    contents.applications.assign(applications.begin(), applications.end());
    return contents;
}

std::optional<Package> load(const std::string& path)
{
    const Descriptor file = {::open(path.c_str(), O_RDONLY | O_CLOEXEC)};
    if (file.fd < 0)
        return std::nullopt;
    const std::optional<Items> items = readItems(file.fd);
    if (!items)
        return std::nullopt;
    Package package;
    package.path = path;
    if (const auto component = readComponent(file.fd, *items, ""))
        package.components.push_back(*component);
    for (const auto& [name, item] : *items) {
        if (!item.directory || name.find('/') != std::string::npos || name.size() <= 4 || !name.ends_with(".pkg"))
            continue;
        if (const auto component = readComponent(file.fd, *items, name + "/"))
            package.components.push_back(*component);
    }
    if (package.components.empty())
        return std::nullopt;
    const auto distribution = items->find("Distribution");
    if (distribution != items->end()) {
        if (const auto text = readSection(file.fd, distribution->second.section)) {
            const Document document = {parse(*text)};
            if (xmlNode* root = document.document ? xmlDocGetRootElement(document.document) : nullptr) {
                package.title = childText(root, "title");
                package.version = attribute(child(root, "product"), "version");
            }
        }
    }
    if (package.title.empty())
        package.title = std::filesystem::path(path).stem().string();
    if (package.version.empty())
        package.version = package.components.front().version;
    return package;
}

Contents contents(const Package& package)
{
    return process(package, {}, {});
}

Contents extract(const Package& package, const std::string& folder, const std::function<void(uint64_t)>& progress)
{
    return process(package, folder, progress);
}

} // namespace cutefish::reef::package
