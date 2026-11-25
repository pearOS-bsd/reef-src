// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A Mach-O image: the file, its load commands, and where it is mapped.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <utility>
#include <vector>

namespace cutefish::reef {

struct Segment
{
    std::string name;
    uint64_t address;
    uint64_t size;
    uint64_t fileOffset;
    uint64_t fileSize;
    int protection;
};

struct Section
{
    std::string segment;
    std::string name;
    uint64_t address;
    uint64_t size;
    uint32_t flags;
};

enum class DylibKind { Normal, Weak, Reexport, Upward };

struct Dylib
{
    std::string path;
    DylibKind kind = DylibKind::Normal;
    struct Image* image = nullptr;
};

// A symbol an image needs.
struct Bind
{
    std::string_view name;
    int ordinal = 0;
    bool weak = false;
    uint64_t address = 0;
};

enum class ExportKind { Regular, ThreadLocal, Absolute, Reexport, Resolver };

struct Export
{
    ExportKind kind = ExportKind::Regular;
    uint64_t address = 0;
    uint64_t resolver = 0;
    bool weak = false;
    int ordinal = 0;
    std::string name;
};

struct NamedSymbol
{
    std::string_view name;
    uint64_t address;
};

uint64_t readUleb(const uint8_t*& at, const uint8_t* end);
int64_t readSleb(const uint8_t*& at, const uint8_t* end);
// Address space above the program's, where images go.
void* reserveMemory(size_t size);

struct Image
{
    ~Image();
    // Opens and maps the image; null with `error` set when it cannot be used.
    static std::unique_ptr<Image> open(const std::string& path, std::string& error);

    std::pair<const uint8_t*, const uint8_t*> linkedit(uint32_t offset, uint32_t size) const
    {
        if (!size)
            return {};
        return {data + offset, data + offset + size};
    }
    bool contains(uintptr_t address) const { return address >= reservation && address < reservation + reservationSize; }
    const Section* findSection(std::string_view segment, std::string_view name) const;
    bool hasObjC() const;
    // Text read-only again after the fixups.
    void protect();
    // The symbol before an address, and where it starts.
    const char* symbolAt(uintptr_t address, uint64_t* start);
    std::vector<Bind> binds() const;
    std::optional<Export> findExport(std::string_view name) const;

    bool main = false;
    bool linked = false;
    bool initialized = false;
    uint64_t index = 0;
    char unknown10[8];
    std::string path;
    std::string installName;
    const uint8_t* data = nullptr;
    size_t size = 0;
    int fd = -1;
    struct stat status = {};
    uint64_t sliceOffset = 0;
    uint32_t fileType = 0;
    uintptr_t reservation = 0;
    size_t reservationSize = 0;
    uintptr_t header = 0;
    uint64_t preferredBase = 0;
    intptr_t slide = 0;
    std::vector<Segment> segments;
    std::vector<Section> sections;
    std::vector<Dylib> dylibs;
    std::vector<std::string> rpaths;
    uint64_t entryOffset = 0;
    bool hasEntry = false;
    uint64_t threadEntry = 0;
    bool hasThread = false;
    bool entryFromThread = false;
    uint32_t rebaseOffset = 0;
    uint32_t rebaseSize = 0;
    uint32_t bindOffset = 0;
    uint32_t bindSize = 0;
    uint32_t lazyBindOffset = 0;
    uint32_t lazyBindSize = 0;
    uint32_t weakBindOffset = 0;
    uint32_t weakBindSize = 0;
    uint32_t chainedOffset = 0;
    uint32_t chainedSize = 0;
    uint32_t exportOffset = 0;
    uint32_t exportSize = 0;
    uint32_t symbolOffset = 0;
    uint32_t symbolCount = 0;
    uint32_t stringOffset = 0;
    uint32_t stringSize = 0;
    // From the symbol table when there is no export trie.
    std::vector<NamedSymbol> exports;
    std::vector<std::pair<uint32_t, uint32_t>> symbolsByAddress;

private:
    bool parse(std::string& error);
    bool map(std::string& error);
    void readSymbols();
};

} // namespace cutefish::reef
