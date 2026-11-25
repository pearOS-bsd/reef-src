// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Installer packages: a xar archive, with a cpio archive of files in each component.

#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace cutefish::reef::package {

// A file of the xar archive.
struct Section
{
    uint64_t offset = 0;
    uint64_t length = 0;
    std::string encoding;
};

struct Component
{
    std::string identifier;
    std::string version;
    std::string location;
    Section payload;
    bool hasPayload = false;
    bool hasScripts = false;
};

struct Package
{
    std::string path;
    std::string title;
    std::string version;
    std::vector<Component> components;
};

// What a package puts into the Applications folder.
struct Contents
{
    std::vector<std::string> applications;
    // Files meant for elsewhere.
    uint64_t skipped = 0;
    bool hasScripts = false;
    std::string error;
};

// Nothing for what is not an installer package.
std::optional<Package> load(const std::string& path);
Contents contents(const Package& package);
// Puts the applications into the folder; `progress` gets the bytes of the payload read so far.
Contents extract(const Package& package, const std::string& folder, const std::function<void(uint64_t)>& progress);

} // namespace cutefish::reef::package
