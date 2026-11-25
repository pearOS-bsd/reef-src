// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// hdiutil of Mac programs: disk images made from a folder (HFS+, ISO 9660, Joliet, UDF) and UDIF images converted and verified.

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

// A file, folder or link of the source folder, as it was read.
struct Entry
{
    enum Type { Folder, File, Link };

    int type = File;
    std::string name;
    std::string path;
    uint64_t size = 0;
    uint32_t mode = 0;
    uint32_t uid = 0;
    uint32_t gid = 0;
    int64_t accessed = 0;
    int64_t modified = 0;
    int64_t changed = 0;
    // What a link points at.
    std::string target;
    std::vector<Entry> children;

    // The bytes of the files inside.
    uint64_t totalSize() const
    {
        if (type != Folder)
            return size;
        uint64_t total = 0;
        for (const Entry& child : children)
            total += child.totalSize();
        return total;
    }
};

// Done of total, as the work goes.
using Progress = std::function<void(uint64_t done, uint64_t total)>;

// tree.cpp
bool scanFolder(const std::string& folder, Entry& root, std::string* error);
// All of `size` bytes at `offset`.
bool writeAll(int fd, uint64_t offset, const void* data, size_t size);
std::u16string utf16(const std::string& text);
uint32_t updateCrc(uint32_t crc, const void* data, size_t size);
// The data of a file into the image at `offset`; `done` counts the bytes of `total` for the progress.
bool copyFile(const std::string& path, uint64_t size, int fd, uint64_t offset, const Progress& progress, uint64_t* done, uint64_t total,
              std::string* error);

// What makehybrid makes.
struct HybridOptions
{
    bool iso = true;
    bool joliet = true;
    bool udf = true;
    std::string volumeName;
};

// hfs.cpp
bool makeHfs(const Entry& root, const std::string& volumeName, const std::string& output, const Progress& progress, std::string* error);
// optical.cpp
bool makeHybrid(const Entry& root, const HybridOptions& options, const std::string& output, const Progress& progress, std::string* error);

namespace diskimage {

enum Format { UDRO, UDZO, UDBZ, ULFO, ULMO, UDTO, UDRW };

struct ConvertOptions
{
    int format = UDZO;
    // Compression level, 0 for the default of the format.
    int level = 0;
};

bool parseFormat(const std::string& name, ConvertOptions* options);
const char* extension(int format);
bool convert(const std::string& input, const std::string& output, const ConvertOptions& options, const Progress& progress, std::string* error);
bool verify(const std::string& image, const Progress& progress, std::string* report, std::string* error);

} // namespace diskimage
