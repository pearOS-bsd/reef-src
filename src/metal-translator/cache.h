// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The disk cache of compiled Metal sources and translated shaders.
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "shader.h"

namespace cutefish::reef::metal {

// A function of a compiled source, as the library lists it. (0x28 bytes)
struct CompiledFunction
{
    std::string name;                           // 0x00
    int stage = 0;                              // 0x20
};

// What the Metal Shading Language compiler made of a source. (0x38 bytes)
struct CompiledSource
{
    std::string bitcode;                        // 0x00  the AIR module
    std::vector<CompiledFunction> functions;    // 0x20
};

// The file of a key is <home>/.cache/cutefish-reef/metal/<16 hex digits of the build ids of the translator, LLVM and the
// compiler>/<SHA-256 of the key in hex>. The callers make the keys; a miss, a damaged file or no home is never an error.

bool loadCompiledSource(const std::string &key, CompiledSource &source);       // 0xca430  REEFMSL1
void storeCompiledSource(const std::string &key, const CompiledSource &source); // 0xcf5c0
std::shared_ptr<Shader> loadShader(const std::string &key);                    // 0xcacd0  REEFMTL1, null on a miss
void storeShader(const std::string &key, const Shader &shader);                 // 0xcd4b0

// "name=id\n" for each file: its GNU build id in hex, else "size:seconds.nanoseconds" of its modification, else nothing.
std::string buildIds(const std::vector<std::string> &files);                   // 0xc3be0
// Removes everything in `root` but `keep`: the caches of other builds.
void removeOthers(const std::string &root, const std::string &keep);           // 0xc2e30

}
