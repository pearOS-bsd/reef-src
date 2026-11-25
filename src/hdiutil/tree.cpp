// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The source folder as a tree, the data of its files, and what the file systems share: UTF-16 names and checksums.

#include "hdiutil.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

bool writeAll(int fd, uint64_t offset, const void* data, size_t size)
{
    const char* bytes = static_cast<const char*>(data);
    while (size) {
        const ssize_t written = pwrite(fd, bytes, size, offset);
        if (written < 0) {
            if (errno == EINTR)
                continue;
            return false;
        }
        if (written == 0)
            return false;
        bytes += written;
        offset += written;
        size -= written;
    }
    return true;
}

// UTF-8 to UTF-16; what is not UTF-8 becomes U+FFFD.
std::u16string utf16(const std::string& text)
{
    std::u16string result;
    const size_t size = text.size();
    for (size_t i = 0; i < size;) {
        const char byte = text[i++];
        uint32_t c = byte;
        if (byte < 0) {
            if ((byte & 0xe0) == 0xc0) {
                c = 0xfffd;
                if (i < size)
                    c = (byte & 0x1f) << 6 | (text[i++] & 0x3f);
            } else if ((byte & 0xf0) == 0xe0 && i + 1 < size) {
                c = byte << 12 | (text[i] & 0x3f) << 6 | (text[i + 1] & 0x3f);
                i += 2;
            } else if ((byte & 0xf8) == 0xf0 && i + 2 < size) {
                c = (byte & 7) << 18 | (text[i] & 0x3f) << 12 | (text[i + 1] & 0x3f) << 6 | (text[i + 2] & 0x3f);
                i += 3;
                if (c >= 0x10000) {
                    c -= 0x10000;
                    result.push_back(0xd800 + (c >> 10));
                    result.push_back(0xdc00 + (c & 0x3ff));
                    continue;
                }
            } else {
                c = 0xfffd;
            }
        }
        result.push_back(c);
    }
    return result;
}

uint32_t updateCrc(uint32_t crc, const void* data, size_t size)
{
    if (!size)
        return crc;
    return crc32_z(crc, static_cast<const Bytef*>(data), size);
}

bool copyFile(const std::string& path, uint64_t size, int fd, uint64_t offset, const Progress& progress, uint64_t* done, uint64_t total,
              std::string* error)
{
    const int input = open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (input < 0) {
        *error = path + ": " + strerror(errno);
        return false;
    }
    std::vector<char> buffer(1 << 20);
    uint64_t copied = 0;
    while (copied < size) {
        const ssize_t n = read(input, buffer.data(), std::min<uint64_t>(size - copied, buffer.size()));
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0) {
            if (n < 0)
                *error = path + ": " + strerror(errno);
            close(input);
            return n == 0;
        }
        if (!writeAll(fd, offset + copied, buffer.data(), n)) {
            *error = std::string("write: ") + strerror(errno);
            close(input);
            return false;
        }
        *done += n;
        if (progress)
            progress(*done, total);
        copied += n;
    }
    close(input);
    return true;
}

// A file, a link, or a folder with what is in it: other kinds of files are left out.
static bool scan(const std::string& path, const struct stat& status, Entry& entry, std::string* error)
{
    entry.path = path;
    entry.mode = status.st_mode;
    entry.uid = status.st_uid;
    entry.gid = status.st_gid;
    entry.accessed = status.st_atim.tv_sec;
    entry.modified = status.st_mtim.tv_sec;
    entry.changed = status.st_ctim.tv_sec;
    switch (status.st_mode & S_IFMT) {
    case S_IFLNK: {
        entry.type = Entry::Link;
        std::string target(status.st_size + 1, 0);
        const ssize_t size = readlink(path.c_str(), target.data(), target.size());
        if (size < 0) {
            *error = path + ": " + strerror(errno);
            return false;
        }
        target.resize(size);
        entry.target = target;
        entry.size = target.size();
        return true;
    }
    case S_IFREG:
        entry.type = Entry::File;
        entry.size = status.st_size;
        return true;
    }
    entry.type = Entry::Folder;
    DIR* folder = opendir(path.c_str());
    if (!folder) {
        *error = path + ": " + strerror(errno);
        return false;
    }
    while (dirent* item = readdir(folder)) {
        if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)
            continue;
        const std::string child = path + "/" + item->d_name;
        struct stat childStatus;
        if (lstat(child.c_str(), &childStatus) != 0) {
            *error = child + ": " + strerror(errno);
            closedir(folder);
            return false;
        }
        if (!S_ISDIR(childStatus.st_mode) && (childStatus.st_mode & 0xd000) != S_IFREG)
            continue;
        Entry entryOfChild;
        entryOfChild.name = item->d_name;
        if (!scan(child, childStatus, entryOfChild, error)) {
            closedir(folder);
            return false;
        }
        entry.children.push_back(std::move(entryOfChild));
    }
    closedir(folder);
    return true;
}

bool scanFolder(const std::string& folder, Entry& root, std::string* error)
{
    struct stat status;
    if (stat(folder.c_str(), &status) != 0) {
        *error = folder + ": " + strerror(errno);
        return false;
    }
    if (!S_ISDIR(status.st_mode)) {
        *error = folder + ": Not a directory";
        return false;
    }
    std::string path = folder;
    while (path.size() > 1 && path.back() == '/')
        path.pop_back();
    const size_t slash = path.rfind('/');
    root.name = slash == std::string::npos ? path : path.substr(slash + 1);
    return scan(path, status, root, error);
}
