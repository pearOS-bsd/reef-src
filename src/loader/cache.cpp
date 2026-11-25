// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "cache.h"
#include "linker.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

namespace cutefish::reef {

static bool readAll(int fd, void* buffer, size_t size)
{
    char* at = static_cast<char*>(buffer);
    while (size > 0) {
        const ssize_t count = read(fd, at, size);
        if (count <= 0)
            return false;
        at += count;
        size -= size_t(count);
    }
    return true;
}

uint32_t hash(std::string_view text)
{
    uint32_t value = 2166136261u;
    for (const unsigned char c : text)
        value = (value ^ c) * 16777619u;
    return value;
}

// What tells whether a file changed.
struct Identity
{
    dev_t device;
    ino_t inode;
    off_t size;
    timespec modified;
    timespec changed;
};

static void appendIdentity(std::string& key, const struct stat& status)
{
    const Identity identity = {status.st_dev, status.st_ino, status.st_size, status.st_mtim, status.st_ctim};
    key.append(reinterpret_cast<const char*>(&identity), sizeof identity);
}

std::string cacheKey(const std::vector<std::unique_ptr<Image>>& images, const std::string& options, int version)
{
    std::string key;
    struct stat status;
    if (stat("/proc/self/exe", &status) != 0)
        return {};
    appendIdentity(key, status);
    key += options;
    key += '\0';
    key += std::to_string(version);
    key += '\0';
    for (const std::unique_ptr<Image>& image : images) {
        key += image->path;
        key += '\0';
        appendIdentity(key, image->status);
    }
    return key;
}

bool readBinds(const std::string& path, const std::string& key, std::vector<uint64_t>& addresses, std::vector<uint32_t>& flags)
{
    const int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return false;
    char magic[8];
    uint64_t length = 0;
    uint64_t count = 0;
    if (readAll(fd, magic, sizeof magic) && memcmp(magic, "REEFBND1", 8) == 0 && readAll(fd, &length, sizeof length) && length == key.size()) {
        bool good = false;
        {
            std::string stored(length, '\0');
            if (readAll(fd, stored.data(), length) && stored == key && readAll(fd, &count, sizeof count) && count <= 0xffffff) {
                addresses.resize(count);
                flags.resize(count);
                good = readAll(fd, addresses.data(), count * 8) && readAll(fd, flags.data(), count * 4);
            }
        }
        close(fd);
        if (good)
            return true;
    } else {
        close(fd);
    }
    addresses.clear();
    flags.clear();
    return false;
}

// The home folder, as Mac programs see it.
static std::string homeFolder()
{
    const char* home = secure_getenv("CFFIXED_USER_HOME");
    if (home && *home == '/')
        return home;
    const passwd* user = getpwuid(getuid());
    if (user && user->pw_dir && *user->pw_dir)
        return user->pw_dir;
    return {};
}

std::string cachePath(const std::string& key)
{
    const std::string folder = homeFolder();
    if (folder.empty())
        return {};
    // FNV-1a.
    uint64_t value = 0xcbf29ce484222325;
    for (const unsigned char c : key)
        value = (value ^ c) * 0x100000001b3;
    char name[32];
    snprintf(name, sizeof name, "%016llx", static_cast<unsigned long long>(value));
    return folder + "/.cache/cutefish-reef/binds/" + name;
}

static std::string parent(const std::string& path) { return path.substr(0, path.rfind('/')); }

void writeBinds(const std::string& path, const std::string& key, const std::vector<uint64_t>& addresses, const std::vector<uint32_t>& flags)
{
    const std::string folder = parent(path);
    const std::string cache = parent(folder);
    mkdir(parent(cache).c_str(), 0755);
    mkdir(cache.c_str(), 0755);
    mkdir(folder.c_str(), 0755);
    const std::string temporary = path + "." + std::to_string(getpid());
    FILE* file = fopen(temporary.c_str(), "wb");
    if (!file)
        return;
    const uint64_t length = key.size();
    const uint64_t count = addresses.size();
    if (fwrite("REEFBND1", 8, 1, file) == 1 && fwrite(&length, 8, 1, file) == 1 && fwrite(key.data(), 1, key.size(), file) == key.size() &&
        fwrite(&count, 8, 1, file) == 1 && fwrite(addresses.data(), 8, addresses.size(), file) == addresses.size() &&
        fwrite(flags.data(), 4, flags.size(), file) == flags.size()) {
        if (fclose(file) == 0 && rename(temporary.c_str(), path.c_str()) == 0)
            return;
    } else {
        fclose(file);
    }
    unlink(temporary.c_str());
}

bool readState(const std::string& path, const std::string& key, std::string& data)
{
    const int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return false;
    char magic[8];
    uint64_t length = 0;
    uint64_t size = 0;
    if (readAll(fd, magic, sizeof magic) && memcmp(magic, "REEFSTA1", 8) == 0 && readAll(fd, &length, sizeof length) && length == key.size()) {
        bool good = false;
        {
            std::string stored(length, '\0');
            if (readAll(fd, stored.data(), length) && stored == key && readAll(fd, &size, sizeof size) && size <= 0x3fffffff) {
                data.resize(size);
                good = readAll(fd, data.data(), size);
            }
        }
        close(fd);
        if (good)
            return true;
    } else {
        close(fd);
    }
    data.clear();
    return false;
}

void writeState(const std::string& path, const std::string& key, const void* data, size_t size)
{
    const std::string temporary = path + "." + std::to_string(getpid());
    FILE* file = fopen(temporary.c_str(), "wb");
    if (!file)
        return;
    const uint64_t length = key.size();
    const uint64_t count = size;
    if (fwrite("REEFSTA1", 8, 1, file) == 1 && fwrite(&length, 8, 1, file) == 1 && fwrite(key.data(), 1, key.size(), file) == key.size() &&
        fwrite(&count, 8, 1, file) == 1 && fwrite(data, 1, size, file) == size) {
        if (fclose(file) == 0 && rename(temporary.c_str(), path.c_str()) == 0)
            return;
    } else {
        fclose(file);
    }
    unlink(temporary.c_str());
}

} // namespace cutefish::reef
