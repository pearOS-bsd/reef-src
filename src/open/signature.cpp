// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "signature.h"

#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

namespace cutefish::reef {

namespace {

constexpr uint32_t kFatMagic = 0xcafebabe;
constexpr uint32_t kFatMagic64 = 0xcafebabf;
constexpr uint32_t kMagic32 = 0xfeedface;
constexpr uint32_t kMagic64 = 0xfeedfacf;
constexpr uint32_t kCpuX86_64 = 0x01000007;
constexpr uint32_t kCodeSignature = 0x1d;
constexpr uint32_t kMaxSlices = 32;

constexpr uint32_t kEmbeddedSignature = 0xfade0cc0;
constexpr uint32_t kCodeDirectory = 0xfade0c02;
constexpr uint32_t kBlobWrapper = 0xfade0b01;
constexpr uint32_t kCodeDirectorySlot = 0;
constexpr uint32_t kSignatureSlot = 0x10000;
constexpr uint32_t kAdhoc = 0x2;
constexpr uint32_t kSupportsTeam = 0x20200;

uint32_t big32(const char* data)
{
    uint32_t value;
    memcpy(&value, data, sizeof(value));
    return __builtin_bswap32(value);
}

uint32_t little32(const char* data)
{
    uint32_t value;
    memcpy(&value, data, sizeof(value));
    return value;
}

uint64_t big64(const char* data)
{
    uint64_t value;
    memcpy(&value, data, sizeof(value));
    return __builtin_bswap64(value);
}

// Where the x86-64 slice starts, the first slice without one; 0 in a file of one slice.
bool programOffset(int fd, uint64_t& start)
{
    char header[8];
    if (pread(fd, header, sizeof(header), 0) != ssize_t(sizeof(header)))
        return false;
    const uint32_t magic = big32(header);
    if (magic != kFatMagic && magic != kFatMagic64)
        return true;
    const uint32_t count = big32(header + 4);
    const bool wide = magic == kFatMagic64;
    const size_t size = wide ? 32 : 20;
    if (count == 0)
        return false;
    for (uint32_t i = 0; i < count && i < kMaxSlices; i++) {
        char arch[32];
        if (pread(fd, arch, size, off_t(sizeof(header) + i * size)) != ssize_t(size))
            return false;
        const uint64_t offset = wide ? big64(arch + 8) : big32(arch + 8);
        if (i == 0)
            start = offset;
        if (big32(arch) == kCpuX86_64) {
            start = offset;
            return true;
        }
    }
    return true;
}

std::optional<std::string> signatureData(int fd)
{
    uint64_t start = 0;
    if (!programOffset(fd, start))
        return std::nullopt;
    char header[32];
    if (pread(fd, header, sizeof(header), off_t(start)) != ssize_t(sizeof(header)))
        return std::nullopt;
    const uint32_t magic = little32(header);
    if (magic != kMagic32 && magic != kMagic64)
        return std::nullopt;
    const uint32_t commandCount = little32(header + 16);
    const uint32_t commandsSize = little32(header + 20);
    const size_t headerSize = magic == kMagic64 ? 32 : 28;
    std::vector<char> commands(commandsSize);
    if (pread(fd, commands.data(), commands.size(), off_t(start + headerSize)) != ssize_t(commands.size()))
        return std::nullopt;

    uint32_t dataOffset = 0;
    uint32_t dataSize = 0;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < commandCount && offset + 8 <= commandsSize; i++) {
        const uint32_t command = little32(commands.data() + offset);
        const uint32_t size = little32(commands.data() + offset + 4);
        if (command == kCodeSignature && offset + 16 <= commandsSize) {
            dataOffset = little32(commands.data() + offset + 8);
            dataSize = little32(commands.data() + offset + 12);
        }
        if (size < 8)
            break;
        offset += size;
    }
    if (dataSize == 0)
        return std::nullopt;
    std::string data(dataSize, '\0');
    if (pread(fd, data.data(), data.size(), off_t(start + dataOffset)) != ssize_t(data.size()))
        return std::nullopt;
    return data;
}

} // namespace

std::optional<Signature> readSignature(const std::string& path)
{
    const int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return std::nullopt;
    const std::optional<std::string> data = signatureData(fd);
    close(fd);
    if (!data)
        return std::nullopt;
    if (data->size() < 12 || big32(data->data()) != kEmbeddedSignature)
        return std::nullopt;

    Signature signature;
    bool found = false;
    const uint32_t count = big32(data->data() + 8);
    for (uint32_t i = 0; i < count; i++) {
        if (data->size() < 12 + size_t(i) * 8 + 8)
            break;
        const uint32_t slot = big32(data->data() + 12 + i * 8);
        const uint32_t offset = big32(data->data() + 12 + i * 8 + 4);
        if (data->size() < size_t(offset) + 8)
            return std::nullopt;
        const char* blob = data->data() + offset;
        const uint32_t length = big32(blob + 4);
        if (length < 8 || data->size() < size_t(offset) + length)
            return std::nullopt;
        if (slot == kCodeDirectorySlot && big32(blob) == kCodeDirectory) {
            signature.codeDirectory.assign(blob, length);
            found = true;
        } else if (slot == kSignatureSlot && big32(blob) == kBlobWrapper) {
            signature.cms.assign(blob + 8, length - 8);
        }
    }
    if (!found || signature.codeDirectory.size() < 44)
        return std::nullopt;

    const std::string& directory = signature.codeDirectory;
    signature.adhoc = (big32(directory.data() + 12) & kAdhoc) != 0;
    if (big32(directory.data() + 8) >= kSupportsTeam && directory.size() >= 52) {
        const uint32_t team = big32(directory.data() + 48);
        if (team != 0 && team < directory.size())
            signature.team.assign(directory.data() + team, strnlen(directory.data() + team, directory.size() - team));
    }
    if (signature.adhoc)
        signature.cms.clear();
    return signature;
}

} // namespace cutefish::reef
