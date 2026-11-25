// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// UDIF disk images: a raw image or a UDIF one read as sectors, written again in chunks of 1 MiB compressed with zlib, bzip2,
// LZFSE or LZMA by several threads, with the block tables (`mish`) in the property list and the `koly` trailer at the end.

#include "hdiutil.h"

#include "../../upstream/lzfse/lzfse.h"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <lzma.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <zlib.h>

namespace diskimage {

namespace {

constexpr uint64_t ChunkSectors = 2048;

enum : uint32_t {
    ZeroFill = 0,
    Raw = 1,
    Ignore = 2,
    Zlib = 0x80000005,
    Bzip2 = 0x80000006,
    Lzfse = 0x80000007,
    Lzma = 0x80000008,
    Comment = 0x7ffffffe,
    End = 0xffffffff,
};

// A run of sectors and where its data is.
struct Run
{
    uint32_t type;
    uint64_t sector;
    uint64_t count;
    uint64_t offset;
    uint64_t length;
};

// A block table of the image: its sectors and the CRC32 of their data.
struct Partition
{
    std::string name;
    uint64_t firstSector;
    uint64_t sectorCount;
    uint32_t id;
    uint32_t crc;
    std::vector<Run> runs;
};

struct SourceImage
{
    int fd = -1;
    uint64_t size = 0;
    bool udif = false;
    uint64_t sectorCount = 0;
    uint64_t dataOffset = 0;
    uint64_t dataLength = 0;
    uint32_t checksumType = 0;
    uint32_t dataCrc = 0;
    std::vector<Partition> partitions;
    // The run decompressed last.
    std::vector<uint8_t> cache;
    const Run* cached = nullptr;

    ~SourceImage()
    {
        if (fd >= 0)
            close(fd);
    }
};

struct Bzip2Functions
{
    int (*compress)(char* out, unsigned* outSize, char* in, unsigned inSize, int blockSize, int verbosity, int workFactor);
    int (*decompress)(char* out, unsigned* outSize, char* in, unsigned inSize, int small, int verbosity);
};

} // namespace

// libbz2 when it is there: it is opened the first time it is needed.
static const Bzip2Functions& bzip2()
{
    static const Bzip2Functions functions = [] {
        void* library = dlopen("libbz2.so.1.0", RTLD_NOW);
        if (!library)
            library = dlopen("libbz2.so.1", RTLD_NOW);
        if (!library)
            return Bzip2Functions{};
        return Bzip2Functions{reinterpret_cast<decltype(Bzip2Functions::compress)>(dlsym(library, "BZ2_bzBuffToBuffCompress")),
                              reinterpret_cast<decltype(Bzip2Functions::decompress)>(dlsym(library, "BZ2_bzBuffToBuffDecompress"))};
    }();
    return functions;
}

static bool readAll(int fd, uint64_t offset, void* data, size_t size)
{
    char* bytes = static_cast<char*>(data);
    while (size) {
        const ssize_t n = pread(fd, bytes, size, offset);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            return false;
        }
        if (n == 0)
            return false;
        bytes += n;
        offset += n;
        size -= n;
    }
    return true;
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

static uint32_t get32(const uint8_t* p)
{
    return uint32_t(p[0]) << 24 | p[1] << 16 | p[2] << 8 | p[3];
}

static uint64_t get64(const uint8_t* p)
{
    return uint64_t(get32(p)) << 32 | get32(p + 4);
}

// The block table of a partition; the last run ends at `end` of the data fork.
static std::vector<uint8_t> mishBlock(const Partition& partition, uint64_t end)
{
    std::vector<uint8_t> mish(0xcc + 0x28 * (partition.runs.size() + 1));
    uint8_t* p = mish.data();
    memcpy(p, "mish", 4);
    put32(p + 4, 1);
    put64(p + 8, partition.firstSector);
    put64(p + 16, partition.sectorCount);
    put64(p + 24, 0);
    put32(p + 32, 2056);
    put32(p + 64, 2);
    put32(p + 68, 32);
    put32(p + 72, partition.crc);
    put32(p + 200, partition.runs.size() + 1);
    uint8_t* entry = p + 204;
    for (const Run& run : partition.runs) {
        put32(entry, run.type);
        put64(entry + 8, run.sector - partition.firstSector);
        put64(entry + 16, run.count);
        put64(entry + 24, run.offset);
        put64(entry + 32, run.length);
        entry += 0x28;
    }
    put32(entry, End);
    put64(entry + 8, partition.sectorCount);
    put64(entry + 24, end);
    return mish;
}

// One chunk: its type in the block table, and its data in `output`.
static uint32_t compressChunk(int format, int level, const std::vector<uint8_t>& input, std::vector<uint8_t>* output)
{
    if (std::find_if(input.begin(), input.end(), [](uint8_t byte) { return byte != 0; }) == input.end()) {
        output->clear();
        return Ignore;
    }
    if (format == UDZO) {
        uLongf size = compressBound(input.size());
        output->resize(size);
        const int result = compress2(output->data(), &size, input.data(), input.size(), level > 0 ? level : 1);
        output->resize(size);
        if (result == Z_OK && output->size() < input.size())
            return Zlib;
    } else if (format == UDBZ) {
        const Bzip2Functions& functions = bzip2();
        unsigned size = input.size() + 600 + input.size() / 100;
        output->resize(size);
        if (functions.compress) {
            const int result = functions.compress(reinterpret_cast<char*>(output->data()), &size,
                                                  reinterpret_cast<char*>(const_cast<uint8_t*>(input.data())), input.size(),
                                                  level > 0 ? level : 9, 0, 0);
            output->resize(size);
            if (result == 0 && output->size() < input.size())
                return Bzip2;
        }
    } else if (format == ULFO) {
        std::vector<uint8_t> scratch(lzfse_encode_scratch_size());
        output->resize(input.size() + 0x1000);
        const size_t size = lzfse_encode_buffer(output->data(), output->size(), input.data(), input.size(), scratch.data());
        if (size) {
            output->resize(size);
            if (output->size() < input.size())
                return Lzfse;
        }
    } else if (format == ULMO) {
        lzma_options_lzma options;
        lzma_lzma_preset(&options, level > 0 ? std::min(level, 9) : 6);
        lzma_stream stream = LZMA_STREAM_INIT;
        output->resize(input.size() + 0x1000 + input.size() / 2);
        if (lzma_alone_encoder(&stream, &options) == LZMA_OK) {
            stream.next_in = input.data();
            stream.avail_in = input.size();
            stream.next_out = output->data();
            stream.avail_out = output->size();
            const lzma_ret result = lzma_code(&stream, LZMA_FINISH);
            output->resize(output->size() - stream.avail_out);
            lzma_end(&stream);
            if (result == LZMA_STREAM_END && output->size() < input.size())
                return Lzma;
        }
    }
    *output = input;
    return Raw;
}

bool parseFormat(const std::string& name, ConvertOptions* options)
{
    static const struct
    {
        const char* name;
        int format;
    } formats[] = {{"UDRO", UDRO}, {"UDZO", UDZO}, {"UDBZ", UDBZ}, {"ULFO", ULFO}, {"ULMO", ULMO}, {"UDTO", UDTO}, {"UDRW", UDRW}};
    for (const auto& format : formats) {
        if (name == format.name) {
            options->format = format.format;
            return true;
        }
    }
    return false;
}

const char* extension(int format)
{
    return format == UDTO ? ".cdr" : ".dmg";
}

static std::string escaped(const std::string& text)
{
    std::string result;
    for (char c : text) {
        if (c == '&')
            result += "&amp;";
        else if (c == '<')
            result += "&lt;";
        else if (c == '>')
            result += "&gt;";
        else
            result += c;
    }
    return result;
}

static std::string base64(const std::vector<uint8_t>& data)
{
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    for (size_t i = 0; i < data.size(); i += 3) {
        const uint32_t n = data[i] << 16 | (i + 1 < data.size() ? data[i + 1] << 8 : 0) | (i + 2 < data.size() ? data[i + 2] : 0);
        result += alphabet[n >> 18 & 63];
        result += alphabet[n >> 12 & 63];
        result += i + 1 < data.size() ? alphabet[n >> 6 & 63] : '=';
        result += i + 2 < data.size() ? alphabet[n & 63] : '=';
    }
    return result;
}

// The property list with the block table of every partition.
static std::string propertyList(const std::vector<Partition>& partitions, const std::vector<uint64_t>& ends)
{
    std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                      "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
                      "<plist version=\"1.0\">\n"
                      "<dict>\n"
                      "\t<key>resource-fork</key>\n"
                      "\t<dict>\n"
                      "\t\t<key>blkx</key>\n"
                      "\t\t<array>\n";
    for (size_t i = 0; i < partitions.size(); i++) {
        const std::string name = escaped(partitions[i].name);
        xml += "\t\t\t<dict>\n\t\t\t\t<key>Attributes</key>\n\t\t\t\t<string>0x0050</string>\n";
        xml += "\t\t\t\t<key>CFName</key>\n\t\t\t\t<string>" + name + "</string>\n";
        xml += "\t\t\t\t<key>Data</key>\n\t\t\t\t<data>\n\t\t\t\t";
        const std::string data = base64(mishBlock(partitions[i], ends[i]));
        for (size_t at = 0; at < data.size(); at += 68) {
            if (at)
                xml += "\n\t\t\t\t";
            xml += data.substr(at, 68);
        }
        xml += "\n\t\t\t\t</data>\n";
        xml += "\t\t\t\t<key>ID</key>\n\t\t\t\t<string>" + std::to_string(int(i)) + "</string>\n";
        xml += "\t\t\t\t<key>Name</key>\n\t\t\t\t<string>" + name + "</string>\n";
        xml += "\t\t\t</dict>\n";
    }
    xml += "\t\t</array>\n\t</dict>\n</dict>\n</plist>\n";
    return xml;
}

static bool readSectors(SourceImage& source, uint64_t sector, uint64_t count, uint8_t* buffer, std::string* error);
static bool openImage(const std::string& path, SourceImage* source, std::string* error);

bool verify(const std::string& image, const Progress& progress, std::string* report, std::string* error)
{
    SourceImage source;
    if (!openImage(image, &source, error))
        return false;
    if (!source.udif || source.checksumType != 2) {
        *error = "the image has no checksum to verify";
        return false;
    }
    uint64_t total = 0;
    for (const Partition& partition : source.partitions)
        total += partition.sectorCount;
    uint64_t done = 0;
    std::vector<uint8_t> buffer(ChunkSectors * 512);
    for (const Partition& partition : source.partitions) {
        *report += "Checksumming " + partition.name + "…\n";
        uint32_t crc = updateCrc(0, nullptr, 0);
        for (uint64_t at = 0; at < partition.sectorCount; at += ChunkSectors) {
            const uint64_t count = std::min(partition.sectorCount - at, ChunkSectors);
            if (!readSectors(source, partition.firstSector + at, count, buffer.data(), error))
                return false;
            crc = updateCrc(crc, buffer.data(), count * 512);
            done += count;
            if (progress)
                progress(done, total);
        }
        if (crc != partition.crc) {
            *error = "checksum of the image's data does not match";
            return false;
        }
        char line[256];
        snprintf(line, sizeof(line), "%s: verified   CRC32 $%08X\n", partition.name.c_str(), crc);
        *report += line;
    }
    // The data fork as it is in the file.
    uint32_t crc = updateCrc(0, nullptr, 0);
    std::vector<uint8_t> data(1 << 20);
    for (uint64_t at = 0; at < source.dataLength; at += data.size()) {
        const size_t size = std::min<uint64_t>(source.dataLength - at, data.size());
        if (!readAll(source.fd, source.dataOffset + at, data.data(), size)) {
            *error = "the image is damaged";
            return false;
        }
        crc = updateCrc(crc, data.data(), size);
    }
    if (crc != source.dataCrc) {
        *error = "checksum of the image's data does not match";
        return false;
    }
    char line[64];
    snprintf(line, sizeof(line), "verified   CRC32 $%08X\n", crc);
    *report += line;
    return true;
}

// The image again as UDIF: the chunks of every partition compressed by up to eight threads, a batch at a time.
static bool writeUdif(SourceImage& source, int fd, const ConvertOptions& options, const Progress& progress, std::string* error)
{
    unsigned threads = std::thread::hardware_concurrency();
    if (threads > 8)
        threads = 8;
    if (threads < 1)
        threads = 1;
    const size_t batch = threads * 4;
    std::vector<Partition> partitions;
    for (const Partition& partition : source.partitions)
        partitions.push_back({partition.name, partition.firstSector, partition.sectorCount, partition.id, 0, {}});
    std::vector<uint64_t> ends;
    uint64_t done = 0;
    uint64_t offset = 0;
    uint32_t dataCrc = updateCrc(0, nullptr, 0);
    for (Partition& partition : partitions) {
        partition.runs.clear();
        uint32_t crc = updateCrc(0, nullptr, 0);
        uint64_t sector = partition.firstSector;
        const uint64_t last = partition.firstSector + partition.sectorCount;
        while (sector < last) {
            std::vector<uint64_t> starts;
            std::vector<std::vector<uint8_t>> chunks;
            while (sector < last && chunks.size() < batch) {
                const uint64_t count = std::min(last - sector, ChunkSectors);
                std::vector<uint8_t> chunk(count * 512);
                if (!readSectors(source, sector, count, chunk.data(), error))
                    return false;
                crc = updateCrc(crc, chunk.data(), chunk.size());
                starts.push_back(sector);
                chunks.push_back(std::move(chunk));
                sector += count;
            }
            std::vector<std::vector<uint8_t>> compressed(chunks.size());
            std::vector<uint32_t> types(chunks.size());
            std::atomic<size_t> next = 0;
            auto work = [&] {
                for (size_t i = next++; i < chunks.size(); i = next++)
                    types[i] = compressChunk(options.format, options.level, chunks[i], &compressed[i]);
            };
            std::vector<std::thread> workers;
            for (unsigned i = 1; i < threads && i < chunks.size(); i++)
                workers.emplace_back(work);
            work();
            for (std::thread& worker : workers)
                worker.join();
            for (size_t i = 0; i < chunks.size(); i++) {
                const uint64_t count = chunks[i].size() / 512;
                if (!compressed[i].empty() && !writeAll(fd, offset, compressed[i].data(), compressed[i].size())) {
                    *error = std::string("write: ") + strerror(errno);
                    return false;
                }
                dataCrc = updateCrc(dataCrc, compressed[i].data(), compressed[i].size());
                partition.runs.push_back({types[i], starts[i], count, offset, compressed[i].size()});
                offset += compressed[i].size();
                done += count;
            }
            if (progress)
                progress(done, source.sectorCount);
        }
        partition.crc = crc;
        ends.push_back(offset);
    }
    const std::string xml = propertyList(partitions, ends);
    if (!writeAll(fd, offset, xml.data(), xml.size())) {
        *error = std::string("write: ") + strerror(errno);
        return false;
    }
    uint32_t master = updateCrc(0, nullptr, 0);
    for (const Partition& partition : partitions) {
        uint8_t value[4];
        put32(value, partition.crc);
        master = updateCrc(master, value, 4);
    }
    uint8_t koly[512] = {};
    memcpy(koly, "koly", 4);
    put32(koly + 4, 4);
    put32(koly + 8, 512);
    put32(koly + 12, 1);
    put64(koly + 32, offset);
    put32(koly + 56, 1);
    put32(koly + 60, 1);
    if (getrandom(koly + 64, 16, 0) != 16)
        memset(koly + 64, 0x5a, 16);
    put32(koly + 80, 2);
    put32(koly + 84, 32);
    put32(koly + 88, dataCrc);
    put64(koly + 216, offset);
    put64(koly + 224, xml.size());
    put32(koly + 352, 2);
    put32(koly + 356, 32);
    put32(koly + 360, master);
    put32(koly + 488, 1);
    put64(koly + 492, source.sectorCount);
    if (!writeAll(fd, offset + xml.size(), koly, sizeof(koly))) {
        *error = std::string("write: ") + strerror(errno);
        return false;
    }
    return true;
}

bool convert(const std::string& input, const std::string& output, const ConvertOptions& options, const Progress& progress, std::string* error)
{
    SourceImage source;
    if (!openImage(input, &source, error))
        return false;
    if (options.format == UDBZ && !bzip2().compress) {
        *error = "bzip2 is not available";
        return false;
    }
    const int fd = open(output.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
    if (fd < 0) {
        *error = output + ": " + strerror(errno);
        return false;
    }
    bool written;
    if (options.format == UDTO || options.format == UDRW) {
        written = ftruncate(fd, source.sectorCount * 512) == 0;
        if (!written) {
            *error = output + ": " + strerror(errno);
        } else {
            std::vector<uint8_t> buffer(ChunkSectors * 512);
            for (uint64_t sector = 0; written && sector < source.sectorCount; sector += ChunkSectors) {
                const uint64_t count = std::min(source.sectorCount - sector, ChunkSectors);
                written = readSectors(source, sector, count, buffer.data(), error);
                if (!written)
                    break;
                written = writeAll(fd, sector * 512, buffer.data(), count * 512);
                if (!written) {
                    *error = std::string("write: ") + strerror(errno);
                    break;
                }
                if (progress)
                    progress(sector + count, source.sectorCount);
            }
        }
    } else {
        written = writeUdif(source, fd, options, progress, error);
    }
    if (close(fd) != 0 && written) {
        *error = output + ": " + strerror(errno);
        written = false;
    }
    if (!written)
        unlink(output.c_str());
    return written;
}

// `count` sectors from `sector` on: zero where the image has nothing.
static bool readSectors(SourceImage& source, uint64_t sector, uint64_t count, uint8_t* buffer, std::string* error)
{
    const uint64_t size = count * 512;
    memset(buffer, 0, size);
    if (!source.udif) {
        if (sector * 512 < source.size) {
            const uint64_t available = std::min(source.size - sector * 512, size);
            if (available && !readAll(source.fd, sector * 512, buffer, available)) {
                *error = std::string("read: ") + strerror(errno);
                return false;
            }
        }
        return true;
    }
    for (const Partition& partition : source.partitions) {
        for (const Run& run : partition.runs) {
            const uint64_t from = std::max(sector, run.sector);
            const uint64_t to = std::min(sector + count, run.sector + run.count);
            if (from >= to)
                continue;
            if (source.cached != &run) {
                source.cache.assign(run.count * 512, 0);
                source.cached = nullptr;
                if ((run.type & ~2u) == 0) {
                    source.cached = &run;
                } else {
                    std::vector<uint8_t> data(run.length);
                    if (!readAll(source.fd, source.dataOffset + run.offset, data.data(), data.size())) {
                        *error = "the image is damaged";
                        return false;
                    }
                    bool decoded = false;
                    if (run.type == Raw) {
                        memcpy(source.cache.data(), data.data(), std::min(data.size(), source.cache.size()));
                        decoded = true;
                    } else if (run.type == Zlib) {
                        uLongf length = source.cache.size();
                        decoded = uncompress(source.cache.data(), &length, data.data(), data.size()) == Z_OK;
                    } else if (run.type == Bzip2) {
                        const Bzip2Functions& functions = bzip2();
                        unsigned length = source.cache.size();
                        if (!functions.decompress) {
                            *error = "bzip2 is not available";
                            return false;
                        }
                        decoded = functions.decompress(reinterpret_cast<char*>(source.cache.data()), &length,
                                                       reinterpret_cast<char*>(data.data()), data.size(), 0, 0) == 0;
                    } else if (run.type == Lzfse) {
                        std::vector<uint8_t> scratch(lzfse_decode_scratch_size());
                        decoded = lzfse_decode_buffer(source.cache.data(), source.cache.size(), data.data(), data.size(), scratch.data()) != 0;
                    } else if (run.type == Lzma) {
                        lzma_stream stream = LZMA_STREAM_INIT;
                        if (lzma_auto_decoder(&stream, UINT64_MAX, 0) == LZMA_OK) {
                            stream.next_in = data.data();
                            stream.avail_in = data.size();
                            stream.next_out = source.cache.data();
                            stream.avail_out = source.cache.size();
                            const lzma_ret result = lzma_code(&stream, LZMA_FINISH);
                            decoded = result == LZMA_STREAM_END || result == LZMA_OK;
                        }
                        lzma_end(&stream);
                    } else {
                        *error = "the image uses a compression that is not supported";
                        return false;
                    }
                    if (!decoded) {
                        *error = "the image is damaged";
                        return false;
                    }
                    source.cached = &run;
                }
            }
            memcpy(buffer + (from - sector) * 512, source.cache.data() + (from - run.sector) * 512, (to - from) * 512);
        }
    }
    return true;
}

static std::vector<uint8_t> unbase64(const std::string& text)
{
    std::vector<uint8_t> result;
    uint32_t bits = 0;
    int count = 0;
    for (char c : text) {
        int value;
        if (c >= 'A' && c <= 'Z')
            value = c - 'A';
        else if (c >= 'a' && c <= 'z')
            value = c - 'a' + 26;
        else if (c >= '0' && c <= '9')
            value = c - '0' + 52;
        else if (c == '+')
            value = 62;
        else if (c == '/')
            value = 63;
        else
            continue;
        bits = bits << 6 | value;
        count += 6;
        if (count >= 8) {
            count -= 8;
            result.push_back(bits >> count);
        }
    }
    return result;
}

// The value of the first `<tag>` after `from`, and where it ends.
static bool element(const std::string& xml, size_t from, size_t to, const char* tag, std::string* value, size_t* end)
{
    const std::string open = std::string("<") + tag + ">";
    const std::string close = std::string("</") + tag + ">";
    const size_t start = xml.find(open, from);
    if (start == std::string::npos || start >= to)
        return false;
    const size_t stop = xml.find(close, start + open.size());
    if (stop == std::string::npos || stop > to)
        return false;
    *value = xml.substr(start + open.size(), stop - start - open.size());
    *end = stop + close.size();
    return true;
}

static bool parseUdif(SourceImage* source, const uint8_t* koly, std::string* error)
{
    source->udif = true;
    source->dataOffset = get64(koly + 24);
    source->dataLength = get64(koly + 32);
    source->checksumType = get32(koly + 80);
    source->dataCrc = get32(koly + 88);
    source->sectorCount = get64(koly + 492);
    const uint64_t xmlOffset = get64(koly + 216);
    const uint64_t xmlLength = get64(koly + 224);
    if (get32(koly + 60) > 1) {
        *error = "segmented images are not supported";
        return false;
    }
    std::string xml(xmlLength, 0);
    if (!xmlLength || !readAll(source->fd, xmlOffset, xml.data(), xml.size())) {
        *error = "the image has no partition table";
        return false;
    }
    size_t at = xml.find("<key>blkx</key>");
    if (at == std::string::npos) {
        *error = "the image has no partition table";
        return false;
    }
    const size_t arrayEnd = xml.find("</array>", at);
    for (;;) {
        const size_t start = xml.find("<dict>", at);
        if (start == std::string::npos || start > arrayEnd)
            break;
        const size_t stop = xml.find("</dict>", start);
        if (stop == std::string::npos)
            break;
        Partition partition = {};
        std::string value;
        size_t end;
        const size_t nameKey = xml.find("<key>Name</key>", start);
        if (nameKey != std::string::npos && nameKey < stop && element(xml, nameKey, stop, "string", &value, &end))
            partition.name = value;
        const size_t dataKey = xml.find("<key>Data</key>", start);
        if (dataKey != std::string::npos && dataKey < stop && element(xml, dataKey, stop, "data", &value, &end)) {
            const std::vector<uint8_t> mish = unbase64(value);
            if (mish.size() >= 0xcc && memcmp(mish.data(), "mish", 4) == 0) {
                partition.firstSector = get64(&mish[8]);
                partition.sectorCount = get64(&mish[16]);
                partition.crc = get32(&mish[72]);
                const uint32_t count = get32(&mish[200]);
                for (uint32_t i = 0; i < count && 0xcc + 0x28 * (i + 1) <= mish.size(); i++) {
                    const uint8_t* entry = &mish[0xcc + 0x28 * i];
                    const uint32_t type = get32(entry);
                    if (type == End || type == Comment)
                        continue;
                    partition.runs.push_back({type, partition.firstSector + get64(entry + 8), get64(entry + 16), get64(entry + 24), get64(entry + 32)});
                }
                partition.id = source->partitions.size();
                source->partitions.push_back(std::move(partition));
            }
        }
        at = stop + 7;
    }
    if (source->partitions.empty()) {
        *error = "the image has no partition table";
        return false;
    }
    return true;
}

// A raw image is one partition, named after the file system that it starts with.
static bool openImage(const std::string& path, SourceImage* source, std::string* error)
{
    source->fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
    struct stat status;
    if (source->fd < 0 || fstat(source->fd, &status) != 0) {
        *error = path + ": " + strerror(errno);
        return false;
    }
    source->size = status.st_size;
    uint8_t koly[512];
    if (source->size >= 512 && readAll(source->fd, source->size - 512, koly, sizeof(koly)) && memcmp(koly, "koly", 4) == 0)
        return parseUdif(source, koly, error);
    uint64_t magic = 0;
    readAll(source->fd, 0, &magic, sizeof(magic));
    if (memcmp(&magic, "encrcdsa", 8) == 0) {
        *error = "encrypted images are not supported";
        return false;
    }
    source->sectorCount = (source->size + 511) >> 9;
    const char* kind = "Apple_UDF";
    char hfs[2] = {};
    char iso[6] = {};
    if (readAll(source->fd, 0x400, hfs, 2) && (memcmp(hfs, "H+", 2) == 0 || memcmp(hfs, "HX", 2) == 0))
        kind = "Apple_HFS";
    else if (readAll(source->fd, 0x8000, iso, 6) && memcmp(iso + 1, "CD001", 5) == 0)
        kind = "Apple_ISO";
    Partition partition = {std::string("whole disk (") + kind + " : 0)", 0, source->sectorCount, 0, 0, {}};
    source->partitions.push_back(partition);
    return true;
}

} // namespace diskimage
