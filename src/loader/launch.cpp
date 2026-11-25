// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The first start of an application that came from elsewhere: reef-open asks the user before it runs.

#include "launch.h"
#include "signature.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <pwd.h>
#include <unistd.h>
#include <vector>

namespace cutefish::reef {

static std::string selfPath()
{
    char self[4096];
    const ssize_t length = readlink("/proc/self/exe", self, sizeof self - 1);
    if (length <= 0)
        return {};
    return std::string(self, size_t(length));
}

static std::string userHome()
{
    const passwd* user = getpwuid(getuid());
    if (!user || !user->pw_dir || !*user->pw_dir)
        return {};
    return user->pw_dir;
}

// Applications in an Applications folder were installed by the user: they are not asked about.
static bool installed(const char* bundle)
{
    char resolved[4096];
    if (!realpath(bundle, resolved))
        return false;
    const std::string path = resolved;
    if (path.starts_with("/Applications/"))
        return true;
    const std::string home = userHome();
    if (home.empty())
        return false;
    char applications[4096];
    if (!realpath((home + "/Applications").c_str(), applications))
        return false;
    const std::string folder = applications;
    return path.size() > folder.size() && path.starts_with(folder) && path[folder.size()] == '/';
}

// The settings file of reef-open, and a key as it writes them.
static std::string settingsPath()
{
    const std::string home = userHome();
    if (home.empty())
        return {};
    return home + "/.config/cutefish/reef.conf";
}

static std::string settingsKey(const std::string& name)
{
    std::string key;
    for (const char c : name) {
        if (c == '/') {
            key += '\\';
        } else if (c == '-' || c == '.' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= 'a' && c <= 'z')) {
            key += c;
        } else {
            key += '%';
            key += "0123456789ABCDEF"[static_cast<unsigned char>(c) >> 4];
            key += "0123456789ABCDEF"[c & 0xf];
        }
    }
    key += '=';
    return key;
}

// The team an application was opened with the last time, as reef-open noted it.
static std::optional<std::string> openedTeam(const std::string& identifier)
{
    const std::string path = settingsPath();
    if (path.empty())
        return std::nullopt;
    std::ifstream file(path);
    const std::string key = settingsKey(identifier);
    std::string line;
    bool inGroup = false;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (!line.empty() && line[0] == '[')
            inGroup = line == "[OpenedApplications]";
        else if (inGroup && line.compare(0, key.size(), key) == 0)
            return line.substr(key.size());
    }
    return std::nullopt;
}

// The first start of an application that came from elsewhere, or that another team signed since, goes through
// reef-open: it asks the user and becomes the loader again.
void launchThroughOpen(const std::string& bundle, const std::string& identifier, const std::string& executable, char** argv)
{
    if (identifier.empty())
        return;
    if (installed(bundle.c_str()))
        return;
    const std::optional<Signature> signature = readSignature(executable);
    const std::string team = signature && !signature->adhoc ? signature->team : std::string();
    const std::optional<std::string> opened = openedTeam(identifier);
    if (opened && *opened == team)
        return;
    const std::string self = selfPath();
    if (self.empty())
        return;
    std::string open = self;
    open.erase(open.rfind('/'));
    open += "/../libexec/cutefish-reef/reef-open";
    if (access(open.c_str(), X_OK) != 0)
        return;
    std::vector<const char*> arguments = {open.c_str(), identifier.c_str(), executable.c_str(), self.c_str()};
    for (char** argument = argv; *argument; argument++)
        arguments.push_back(*argument);
    arguments.push_back(nullptr);
    fflush(stdout);
    fflush(stderr);
    execv(open.c_str(), const_cast<char* const*>(arguments.data()));
}

} // namespace cutefish::reef
