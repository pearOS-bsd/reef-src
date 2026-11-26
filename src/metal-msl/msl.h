// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The Metal Shading Language front end: source text to a module of AIR.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace llvm {
class LLVMContext;
class Module;
}

namespace cutefish::reef::metal {

struct MslOptions {
    // As MTLLanguageVersion: the major version in the upper 16 bits, the minor one in the lower; 0 for the newest.
    int languageVersion = 0;
    bool fastMath = true;
    std::vector<std::pair<std::string, std::string>> macros;
};

// Compiles a program. Without a module `error` says why; with one it holds the warnings.
std::unique_ptr<llvm::Module> compileMsl(llvm::LLVMContext& context, const std::string& source, const MslOptions& options,
                                         std::string& error);

using MslCompiler = std::unique_ptr<llvm::Module> (*)(llvm::LLVMContext& context, const std::string& source,
                                                      const MslOptions& options, std::string& error);

// What the translator looks up in libreef-metal-msl.so, which it loads when a program comes as source.
constexpr char mslLibrary[] = "libreef-metal-msl.so";
constexpr char mslCompilerSymbol[] = "reef_mtl_msl_compiler";

}
