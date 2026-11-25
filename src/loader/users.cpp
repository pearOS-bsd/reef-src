// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The users of the system, as getpwuid and getpwnam of Darwin return them.

#include "host.h"

#include <climits>
#include <cstdlib>
#include <cstring>
#include <pwd.h>
#include <vector>

namespace cutefish::reef {

// A path without links in its directory: /bin/sh and /usr/bin/sh are the same shell.
std::string resolvedDirectory(const char* path)
{
    if (!path)
        return {};
    const char* slash = strrchr(path, '/');
    if (!slash || path[0] != '/')
        return path;
    const std::string directory(path, slash);
    char resolved[PATH_MAX];
    if (!realpath(directory.empty() ? "/" : directory.c_str(), resolved))
        return path;
    return std::string(strcmp(resolved, "/") == 0 ? "" : resolved) + slash;
}

int getPassword(uint32_t uid, const char* name, uint32_t* ids, char** strings, char* buffer, size_t size)
{
    passwd entry;
    passwd* found = nullptr;
    std::vector<char> storage(16384);
    const int error = name ? getpwnam_r(name, &entry, storage.data(), storage.size(), &found)
                           : getpwuid_r(uid, &entry, storage.data(), storage.size(), &found);
    if (error)
        return darwinError(error);
    if (!found)
        return -1;
    const std::string shell = resolvedDirectory(entry.pw_shell);
    const char* const values[] = {entry.pw_name, entry.pw_passwd, entry.pw_gecos, entry.pw_dir, shell.c_str()};
    size_t used = 0;
    for (int i = 0; i < 5; i++) {
        const char* value = values[i] ? values[i] : "";
        const size_t length = strlen(value) + 1;
        if (used + length > size)
            return 34;
        strings[i] = static_cast<char*>(memcpy(buffer + used, value, length));
        used += length;
    }
    ids[0] = entry.pw_uid;
    ids[1] = entry.pw_gid;
    return 0;
}

} // namespace cutefish::reef
