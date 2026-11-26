// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The reader of Metal library files (.metallib): the functions and their AIR bitcode.

#include "metallib.h"

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace cutefish::reef::metal {

// The start of a Metal library file: where its sections are.
struct Header
{
    char magic[4];              // 0x00  "MTLB"
    uint8_t version[20];        // 0x04  target and file versions, file size
    uint64_t listOffset;        // 0x18  the function list: a count, then the entries
    uint64_t listSize;          // 0x20
    uint64_t metadataSections[4]; // 0x28  public and private metadata
    uint64_t bitcodeOffset;     // 0x48  the bitcode of all functions
    uint64_t bitcodeSize;       // 0x50
};

static uint16_t read16(const uint8_t *bytes)
{
    return bytes[0] | bytes[1] << 8;
}

static uint32_t read32(const uint8_t *bytes)
{
    return read16(bytes) | uint32_t(read16(bytes + 2)) << 16;
}

static uint64_t read64(const uint8_t *bytes)
{
    return read32(bytes) | uint64_t(read32(bytes + 4)) << 32;
}

bool readLibrary(const void *data, size_t size, std::vector<LibraryFunction> &functions, std::string &error)
{
    const auto *bytes = static_cast<const uint8_t *>(data);
    if (size < 0x58 || memcmp(bytes, "MTLB", 4) != 0) {
        error = "not a Metal library";
        return false;
    }
    Header header;
    memcpy(&header, bytes, sizeof header);
    if (size < header.listOffset || size - header.listOffset < header.listSize || header.listSize < 4 || size < header.bitcodeOffset
        || size - header.bitcodeOffset < header.bitcodeSize) {
        error = "the Metal library's sections lie outside it";
        return false;
    }
    functions.clear();
    const size_t end = std::min<size_t>(size, header.listOffset + 4 + header.listSize);
    const uint32_t count = read32(bytes + header.listOffset);
    size_t position = header.listOffset + 4;
    for (uint32_t i = 0; i < count; ++i) {
        if (end < position + 4) {
            error = "the Metal library's function list is cut short";
            return false;
        }
        // An entry is its length, then tags of a name, a length and a value, up to ENDT.
        const size_t entryEnd = position + read32(bytes + position);
        if (end < entryEnd || entryEnd < position + 8) {
            error = "a function entry of the Metal library is cut short";
            return false;
        }
        position += 4;
        LibraryFunction function;
        bool hasSize = false;
        uint64_t bitcodeSize = 0;
        bool hasOffset = false;
        uint64_t bitcodeOffset = 0;
        while (memcmp(bytes + position, "ENDT", 4) != 0) {
            if (entryEnd < position + 6) {
                error = "a function entry of the Metal library is cut short";
                return false;
            }
            const uint16_t length = read16(bytes + position + 4);
            const uint8_t *value = bytes + position + 6;
            if (entryEnd < position + 6 + length) {
                error = "a field of the Metal library's function list is cut short";
                return false;
            }
            if (memcmp(bytes + position, "NAME", 4) == 0) {
                function.name.assign(reinterpret_cast<const char *>(value), strnlen(reinterpret_cast<const char *>(value), length));
            } else if (memcmp(bytes + position, "TYPE", 4) == 0) {
                if (length) {
                    switch (value[0]) {
                    case 0:
                        function.stage = 1;
                        break;
                    case 1:
                        function.stage = 2;
                        break;
                    case 2:
                        function.stage = 3;
                        break;
                    case 4:
                        function.stage = 5;
                        break;
                    default:
                        function.stage = 0;
                        break;
                    }
                }
            } else if (memcmp(bytes + position, "MDSZ", 4) == 0) {
                if (length >= 8) {
                    hasSize = true;
                    bitcodeSize = read64(value);
                }
            } else if (memcmp(bytes + position, "OFFT", 4) == 0 && length >= 24) {
                // The offsets of the public metadata, the private metadata and the bitcode.
                hasOffset = true;
                bitcodeOffset = read64(value + 16);
            }
            position += 6 + length;
            if (entryEnd < position + 4) {
                error = "a function entry of the Metal library has no end tag";
                return false;
            }
        }
        if (function.name.empty() || !hasOffset || !hasSize) {
            error = "a function of the Metal library has no name or no bitcode";
            return false;
        }
        if (header.bitcodeSize < bitcodeOffset || header.bitcodeSize - bitcodeOffset < bitcodeSize) {
            error = "the bitcode of " + function.name + " lies outside the Metal library";
            return false;
        }
        function.bitcode.assign(reinterpret_cast<const char *>(bytes + header.bitcodeOffset + bitcodeOffset), bitcodeSize);
        functions.push_back(std::move(function));
        position = entryEnd;
    }
    return true;
}

}
