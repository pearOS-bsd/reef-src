// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The dialog about a program that does not run, or that stopped because of something that is not supported.

#include "host.h"

#include <cstdio>
#include <string>
#include <unistd.h>

namespace cutefish::reef {

static std::string programPath;

void setProgramPath(const std::string& path)
{
    programPath = path;
}

std::string unsupportedPath()
{
    char self[4096];
    const ssize_t length = readlink("/proc/self/exe", self, sizeof self - 1);
    if (length <= 0)
        return {};
    std::string directory(self, size_t(length));
    directory.erase(directory.rfind('/'));
    return directory + '/' + "../libexec/cutefish-reef/reef-unsupported";
}

// Becomes the dialog; returns when there is none.
void showUnsupported(bool running, const std::string& reason)
{
    if (programPath.empty())
        return;
    const std::string path = unsupportedPath();
    if (path.empty() || access(path.c_str(), X_OK) != 0)
        return;
    fflush(stdout);
    fflush(stderr);
    const char* const arguments[] = {path.c_str(), running ? "--running" : "--launch", programPath.c_str(), reason.c_str(), nullptr};
    execv(path.c_str(), const_cast<char* const*>(arguments));
}

} // namespace cutefish::reef
