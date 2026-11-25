// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The emitter: one AIR function to GLSL.
#pragma once

#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Operator.h>

#include "metadata.h"
#include "shader.h"

namespace cutefish::reef::metal {

// Thrown for anything GLSL cannot say. The text becomes the error of the translation.
class Unsupported : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

// Thrown when the control flow has no structured form (caught: the state machine is used instead).
struct Unstructured
{
};

// What a pointer points into: a buffer, a local, a global, a texture ... (0x50 bytes)
struct Root
{
    int kind = 0;               // 0x00  1: local variable, 4/5: texture or sampler, 6: the sampler of air.get_read_sampler
    int index = 0;              // 0x04  its place in m_roots
    std::string name;           // 0x08
    uint64_t unknown28 = 0;     // 0x28
    uint64_t size = 0;          // 0x30
    uint64_t unknown38 = 0;     // 0x38
    uint64_t unknown40 = 0;     // 0x40
    int unknown48 = -1;         // 0x48
};

// A place inside a root, in bytes: a GLSL expression and a constant added to it. (0x28 bytes)
struct Address
{
    std::string offset;         // 0x00  empty: none
    int64_t constant = 0;       // 0x20
};

// The state of one translation. (0x5e8 bytes)
class Emitter
{
public:
    Emitter(llvm::Module &module, const FunctionInfo &info, const Options &options, Shader &shader); // 0x2db40

    void translate(); // 0x43280

private:
    // emitter.cpp
    void line(const std::string &text);                     // 0x275b0  one indented line of the body
    void require(const std::string &extension);             // 0x27680
    bool isInteger(llvm::Type *type);                       // 0x27690  an integer or a vector of integers
    unsigned width(llvm::Type *type);                       // 0x276b0  elements of a vector, 1 for a scalar
    llvm::Type *scalar(llvm::Type *type);                   // 0x276d0  element of a vector
    unsigned bits(llvm::Type *type);                        // 0x276f0  32 for a pointer
    std::string component(unsigned index);                  // 0x27900  "x", "y", "z", "w"
    std::string mask(unsigned bits);                        // 0x27730  a literal with that many low bits set
    std::string identifier(const std::string &name);        // 0x28960
    std::string vectorPrefix(llvm::Type *element);          // 0x28f20  "vec", "uvec" ...
    std::string typeName(llvm::Type *type);                 // 0x32f60
    std::string nameOf(llvm::Value *value);                 // 0x38260  the variable of an instruction
    std::string valueOf(llvm::Value *value);                // 0x38730  an expression for any value

    // memory.cpp, pointers.cpp
    Root &rootOf(llvm::Value *pointer);                     // 0x942a0
    Address addressOf(llvm::Value *pointer);                // 0x94b70
    std::string offsetOf(const Address &address);           // 0x86900
    std::string loadValue(Root &root, const Address &address, uint64_t alignment, llvm::Type *type);
    void storeValue(Root &root, const Address &address, uint64_t alignment, llvm::Type *type, const std::string &value);

    llvm::Module &m_module;                                 // 0x000
    const FunctionInfo &m_info;                             // 0x008
    const Options &m_options;                               // 0x010
    Shader &m_shader;                                       // 0x018
    llvm::Function *m_function;                             // 0x020
    const llvm::DataLayout &m_layout;                       // 0x028
    std::string m_prefix;                                   // 0x030  "v", "f" or "k"
    std::unique_ptr<llvm::DominatorTree> m_dominators;      // 0x050
    std::unique_ptr<llvm::LoopInfo> m_loops;                // 0x058
    std::ostringstream m_body;                              // 0x060
    int m_indent = 1;                                       // 0x1d8
    std::set<std::string> m_extensions;                     // 0x1e0
    std::vector<std::string> m_strings210;                  // 0x210
    std::map<llvm::Type *, std::string> m_map228;           // 0x228  key of 8 bytes
    std::vector<std::string> m_strings258;                  // 0x258
    std::vector<void *> m_vector270;                        // 0x270
    std::vector<std::string> m_strings288;                  // 0x288
    std::map<std::string, std::string> m_map2a0;            // 0x2a0
    std::vector<std::string> m_strings2d0;                  // 0x2d0
    llvm::DenseMap<const llvm::Value *, std::string> m_names;     // 0x2e8  variables of instructions
    llvm::DenseMap<const llvm::Value *, std::string> m_arguments; // 0x300
    llvm::DenseMap<const llvm::Value *, Address> m_addresses;     // 0x318
    llvm::SmallPtrSet<const llvm::Value *, 16> m_set330;    // 0x330
    std::vector<std::pair<std::string, std::string>> m_variables; // 0x3d0  type, name
    int m_nextVariable = 0;                                 // 0x3e8
    std::vector<std::unique_ptr<Root>> m_roots;             // 0x3f0
    llvm::DenseMap<const llvm::Value *, Root *> m_pointerRoots;   // 0x408
    llvm::DenseMap<const llvm::Value *, Root *> m_argumentRoots;  // 0x420
    std::set<int> m_set438;                                 // 0x438
    // 0x468: std::deque of 200 or 400 byte elements
    std::deque<char[400]> m_deque468;                       // 0x468
    std::vector<void *> m_vector4b8;                        // 0x4b8
    std::map<void *, std::pair<void *, void *>> m_map4d0;   // 0x4d0  nodes of 0x38 bytes
    void *m_500 = nullptr;                                  // 0x500
    std::set<void *> m_set508;                              // 0x508
    std::map<void *, void *> m_map538;                      // 0x538  nodes of 0x30 bytes
    bool m_568 = false;                                     // 0x568
    std::set<void *> m_set570;                              // 0x570
    std::map<void *, void *> m_map5a0;                      // 0x5a0  nodes of 0x30 bytes
    void *m_5d0 = nullptr;                                  // 0x5d0
    void *m_5d8 = nullptr;                                  // 0x5d8
    bool m_5e0 = false;                                     // 0x5e0
    bool m_5e1 = false;                                     // 0x5e1
};

// 0x439c0
bool translate(llvm::Module &module, const FunctionInfo &info, const Options &options, Shader &shader, std::string &error);

}
