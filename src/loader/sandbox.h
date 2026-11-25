// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What a sandboxed program may do, for the broker: what Landlock cannot express it asks the broker, which decides here.

#pragma once

#include <cstdint>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cutefish::reef {

constexpr uint32_t SandboxRead = 1;
constexpr uint32_t SandboxWrite = 2;

// A part of a grant that is refused: the path, and what is inside unless it is literal.
struct SandboxHole
{
    bool literal;
    std::string path;
};

struct SandboxGrant
{
    uint32_t access;
    std::string path;
    std::vector<SandboxHole> holes;
};

// What one profile says.
struct SandboxLayer
{
    bool unconfined = false;
    // What is confined of files.
    uint32_t files = 0;
    std::vector<SandboxGrant> grants;
    // Mach services that may be looked up.
    std::unordered_set<std::string> names;
    std::vector<std::regex> regexes;
    // Classes of extensions: what they give.
    std::unordered_map<std::string, uint32_t> extensions;
    // Classes of extensions that may be issued.
    std::unordered_set<std::string> issued;
};

// An extension that the program consumed.
struct SandboxExtension
{
    uint64_t id;
    std::string name;
    std::string path;
};

struct SandboxToken
{
    std::string name;
    std::string path;
};

using SandboxTokens = std::unordered_map<std::string, SandboxToken>;

struct SandboxPolicy
{
    std::vector<SandboxLayer> layers;
    std::vector<SandboxExtension> extensions;
    uint64_t nextExtension = 1;

    void release(uint64_t id);
    bool allows(const std::string& path, uint32_t access) const;
    int open(const std::string& path, int flags, int mode, int* fd) const;
    bool mayIssue(const std::string& name) const;
    // mkdir, unlink and rename.
    int change(int operation, const std::string& path, const std::string& to, int mode) const;
    uint64_t consume(const std::string& name, const std::string& path);
    bool mayLookUp(const std::string& name) const;
    // Adds what a profile says, as the program's sandbox described it.
    void add(const std::string& description);
};

bool findSandboxToken(const SandboxTokens& tokens, const std::string& token, std::string& name, std::string& path);
std::string issueSandboxToken(SandboxTokens& tokens, const std::string& name, const std::string& path);

} // namespace cutefish::reef
