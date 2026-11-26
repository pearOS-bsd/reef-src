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
#include <llvm/IR/PassManager.h>
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
#include "textures.h"

namespace cutefish::reef::metal {

// Thrown for anything GLSL cannot say. The text becomes the error of the translation.
class Unsupported : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

// Thrown when the control flow has no structured form (caught: the structurizing passes, then the state machine).
struct Unstructured
{
};

// What a pointer points into: a buffer, a local, a global, a texture ... (0x50 bytes)
struct Root
{
    enum : int {
        Buffer,                 // an argument: "reef_kb0.w"
        Private,                // an alloca ("reef_p3") or a private global ("reef_g3")
        Threadgroup,            // a threadgroup argument ("reef_tg0") or a global of address space 3 ("reef_g3")
        Constant,               // a global constant with its value: "reef_c3"
        Texture,
        Sampler,                // also a constant sampler (__air_sampler_state)
        ReadSampler,            // a handle the shader only passes on: the sampler of air.get_read_sampler, a pipeline state
    };
    int kind = Buffer;          // 0x00
    int index = 0;              // 0x04  the Metal slot of an argument: [[buffer(n)]], [[texture(n)]], [[threadgroup(n)]] ...
    std::string name;           // 0x08  the GLSL array of words
    bool readOnly = false;      // 0x28
    uint64_t size = 0;          // 0x30  in words, at least one
    const Argument *argument = nullptr; // 0x38  of a buffer, texture or sampler
    llvm::GlobalVariable *global = nullptr; // 0x40
    int sampler = -1;           // 0x48  its place in the constant samplers
    bool used = false;          // 0x4c  it needs a declaration
    bool addressTaken = false;  // 0x4d  its address is a uniform
};

// A place inside a root, in bytes: a GLSL expression and a constant added to it. (0x28 bytes)
struct Address
{
    std::string offset;         // 0x00  empty: none
    int64_t constant = 0;       // 0x20
};

// Some bits of a value on their way to or from memory: a GLSL uint and how many of its low bits count. (0x28 bytes)
struct Piece
{
    std::string text;           // 0x00
    unsigned bits = 0;          // 0x20
};

// Where control goes from the statement that is being written. (0x50 bytes)
struct Flow
{
    llvm::BasicBlock *next = nullptr;       // 0x00  what follows anyway: nothing to write
    llvm::Loop *loop = nullptr;             // 0x08
    llvm::BasicBlock *exit = nullptr;       // 0x10  what a "break" of the loop reaches
    std::vector<llvm::BasicBlock *> exits;  // 0x18  its other exits
    std::string selector;                   // 0x30  the variable that says which of them was taken
};

// The state of one translation. (0x5e8 bytes)
class Emitter
{
public:
    Emitter(llvm::Module &module, const FunctionInfo &info, const Options &options, Shader &shader); // 0x2db40

    void translate(); // 0x43280

private:
    // emitter.cpp: preparation of the function
    void resolveFunctionConstants();                        // 0x35ce0  values of the caller, is_function_constant_defined
    void runInitializers();                                 // 0x36c50  llvm.global_ctors, run at translation time
    void prepare();                                         // 0x2fc40  inlining, any/all of bools, the cleaning passes
    void unrollLoops(llvm::FunctionAnalysisManager &analyses); // 0x27cc0  short loops over private memory
    void expandConstantExpressions();                       // 0x27380  every constant expression operand becomes an instruction
    void structurize();                                     // 0x29060  the passes that make control flow structured
    void reset();                                           // 0x272f0  forget names and addresses of instructions
    // emitter.cpp: text
    void line(const std::string &text);                     // 0x275b0  one indented line of the body
    void require(const std::string &extension);             // 0x27680
    bool isInteger(llvm::Type *type);                       // 0x27690  an integer or a vector of integers
    unsigned width(llvm::Type *type);                       // 0x276b0  elements of a vector, 1 for a scalar
    llvm::Type *scalar(llvm::Type *type);                   // 0x276d0  element of a vector
    unsigned bits(llvm::Type *type);                        // 0x276f0  32 for a pointer
    std::string mask(unsigned bits);                        // 0x27730  a literal with that many low bits set
    std::string component(unsigned index);                  // 0x27900  "x", "y", "z", "w"
    static std::string identifier(const std::string &name); // 0x28960  other characters as _ and two hex digits
    std::string vectorPrefix(llvm::Type *element);          // 0x28f20  "vec", "uvec" ...
    std::string asSigned(const std::string &value, llvm::Type *type);   // 0x2a6d0  the value as a GLSL int
    std::string masked(const std::string &value, llvm::Type *type);     // 0x2ae20  only the bits of the type
    std::string element(const std::string &value, llvm::Type *type, unsigned index); // 0x2ccf0
    std::string integer(uint64_t value, llvm::Type *type);  // 0x2ede0  a literal
    std::string scalarConstant(llvm::Constant *constant, llvm::Type *type); // 0x2f300
    std::string elementOf(llvm::Value *value, const std::string &text, unsigned index); // 0x2f5a0
    std::string fromSigned(const std::string &value, llvm::Type *type); // 0x2f700  a GLSL int as the value of the type
    std::string helper(const std::string &name, const std::string &text); // 0x31a90  adds a GLSL function once, gives its name
    void requireHalf();                                     // 0x31e50  reef_half_bits, reef_half
    std::string structName(llvm::StructType *type);         // 0x31fa0
    std::string typeName(llvm::Type *type);                 // 0x32f60
    std::string interfaceType(llvm::Type *type);            // 0x33250  typeName, but uint for bool
    std::string componentwise(llvm::Type *type, const std::vector<std::string> &values, const std::string &pattern); // 0x33350  $0 ... in the pattern
    std::string constant(llvm::Constant *constant);         // 0x34060
    std::string nameOf(llvm::Value *value);                 // 0x38260  the variable of an instruction
    std::string valueOf(llvm::Value *value);                // 0x38730  an expression for any value
    std::string select(llvm::SelectInst &select);           // 0x388b0
    std::string shuffle(llvm::ShuffleVectorInst &shuffle);  // 0x38d20
    std::string binary(llvm::BinaryOperator &operation);    // 0x39a40
    std::string compare(llvm::CmpInst &compare);            // 0x3b310
    std::string cast(llvm::CastInst &cast);                 // 0x3cd60
    void instruction(llvm::Instruction &instruction);       // 0x3e430
    void inputs();                                          // 0x40de0  roots and expressions of the arguments

    // intrinsics.cpp
    void requireClamp64();                                  // 0x4c920  reef_clamp64 and 64-bit integers
    std::string convert(llvm::CallInst &call, const std::vector<std::string> &parts); // 0x50f90  air.convert
    bool emitLlvmIntrinsic(llvm::CallInst &call, const std::string &target); // 0x53440  false: not one GLSL can say
    // 0x565f0  name: without "air.", "fast_" and types; parts: the name after "air." split at the dots
    std::string mathIntrinsic(llvm::CallInst &call, const std::string &name, const std::vector<std::string> &parts, bool &handled);
    void emitAirIntrinsic(llvm::CallInst &call, const std::string &fullName, const std::string &target); // 0x5cc60
    void emitCall(llvm::CallInst &call);                    // 0x60170
    // intrinsics_commands.cpp
    std::string pipelineWord(llvm::Value *pipeline, unsigned word); // 0x61de0
    std::string bufferAddress(llvm::Value *pointer, bool high); // 0x64460  a word of the GPU address of a buffer
    bool emitCommand(llvm::CallInst &call, const std::string &name, const std::string &target); // 0x68e20  indirect command buffers

    // memory.cpp
    uint64_t alignmentOf(const Address &address, uint64_t alignment); // 0x84830  what the constant part tells too
    std::string loadValue(Root &root, const Address &address, uint64_t alignment, llvm::Type *type); // 0x84860
    void constantBytes(llvm::Constant *constant, std::vector<uint8_t> &bytes, uint64_t offset); // 0x84990
    std::string constantWords(llvm::Constant *constant, uint64_t size); // 0x84f80  "0x...u, 0x...u" of the words
    std::string offsetOf(const Address &address);           // 0x86900  the address in bytes
    std::vector<Piece> piecesOf(const std::string &value, llvm::Type *type); // 0x87290
    void storeValue(Root &root, const Address &address, uint64_t alignment, llvm::Type *type, const std::string &value); // 0x88460
    Root *globalRoot(llvm::GlobalVariable *global);         // 0x88c60
    std::string wordIndex(const Address &address, uint64_t alignment, uint64_t offset); // 0x895f0  of the word at the address
    std::string atomicWord(Root &root, const Address &address, uint64_t alignment); // 0x8a2b0
    std::vector<Piece> loadPieces(Root &root, const Address &address, uint64_t alignment, uint64_t size); // 0x8a650
    std::string extract(const std::vector<Piece> &pieces, unsigned low, unsigned count); // 0x8bc50  bits of the pieces
    std::string scalarOf(const std::vector<Piece> &pieces, llvm::Type *type); // 0x8c9c0
    std::string fromPieces(const std::vector<Piece> &pieces, llvm::Type *type); // 0x8ce10
    void storePieces(Root &root, const Address &address, uint64_t alignment, const std::vector<Piece> &pieces); // 0x8e480
    // 0x92320  operation: "add", "sub", "and", "or", "xor", "min", "max", "xchg", "store", "load", "cmpxchg"; gives the old value
    std::string atomic(Root &root, const Address &address, uint64_t alignment, const std::string &operation, llvm::Type *type,
                       const std::string &value, const std::string &expected, bool isSigned, bool isFloat);
    Root &rootOf(llvm::Value *pointer);                     // 0x942a0
    Address addressOf(llvm::Value *pointer);                // 0x94b70
    void copyMemory(llvm::CallInst &call, llvm::Intrinsic::ID id); // 0x95b50  memcpy, memmove, memset
    // pointers.cpp
    const Argument *argumentOf(llvm::Value *value);         // 0x9bc90  the metadata of a parameter of the function, or null
    bool splitResourceIndexing();                           // 0x9bce0  loads of textures and samplers at run-time indices
    void collectArgumentBuffers();                          // 0x9eda0  textures and samplers of argument buffers
    void expandResourceArrays();                            // 0x9f7e0  arrays of textures and samplers: one global each
    Root *loadedRoot(llvm::LoadInst *load);                 // 0xa2210  the root of a pointer that an argument buffer holds

    // textures.cpp
    Texture &textureOf(llvm::Value *value);                 // 0xa67e0
    std::string imageOf(Texture &texture);                  // 0xaa610
    std::string texelOf(llvm::Type *type, const std::string &text, bool compared); // 0xaaeb0
    std::string coordinatesOf(const Texture &texture, const std::string &sampler, const Root *root, const std::string &coordinates,
                              const std::string &layer, unsigned components, bool compared, const std::string &reference); // 0xac820
    void addTexture(const Argument &argument);              // 0xaebe0
    void collectTextures();                                 // 0xb0310
    std::string pixelOf(llvm::Value *value, unsigned components); // 0xb03f0
    std::string samplerOf(Texture &texture, const Root *sampler, bool compared); // 0xb0f10
    std::string textureCall(const llvm::CallBase &call, const std::string &name); // 0xb1c20  name: without "air." and types

    // structure.cpp
    void writeInstructions(llvm::BasicBlock *block);        // 0x6d1e0  all but the terminator
    llvm::BasicBlock *mergeOf(llvm::BasicBlock *block, const Flow &flow); // 0x6d250  where the branches of a block meet again
    llvm::BasicBlock *commonMerge(const std::vector<llvm::BasicBlock *> &blocks, const Flow &flow); // 0x6d370
    bool isPresent(const Argument &output);                 // 0x6e130  no function constant removes it
    void findMerges();                                      // 0x6e430
    std::string caseCondition(llvm::SwitchInst *select, llvm::BasicBlock *target); // 0x6ff80
    void copyPhis(llvm::BasicBlock *from, llvm::BasicBlock *to); // 0x706a0
    void writeReturn(llvm::ReturnInst *instruction);        // 0x727d0
    void writeTerminator(llvm::Instruction *terminator, const Flow &flow); // 0x73a60
    void writeBlock(llvm::BasicBlock *block, const Flow &flow); // 0x74450
    void writeBranch(llvm::BasicBlock *target, const Flow &flow); // 0x74740
    void writeEdge(llvm::BasicBlock *from, llvm::BasicBlock *to, const Flow &flow); // 0x74e60
    void writeLoop(llvm::Loop *loop, const Flow &flow);     // 0x74e90
    std::string structured();                               // 0x75d40  the body, or Unstructured
    std::string stateMachine();                             // 0x75f70  the body as a switch over the blocks
    void assemble(const std::string &body);                 // 0x7a330  the shader: declarations and main()

    llvm::Module &m_module;                                 // 0x000
    const FunctionInfo &m_info;                             // 0x008
    const Options &m_options;                               // 0x010
    Shader &m_shader;                                       // 0x018
    llvm::Function *m_function;                             // 0x020
    const llvm::DataLayout &m_layout;                       // 0x028
    std::string m_prefix;                                   // 0x030  "v", "f" or "k": of the names of buffers
    std::unique_ptr<llvm::DominatorTree> m_dominators;      // 0x050
    std::unique_ptr<llvm::LoopInfo> m_loops;                // 0x058
    std::ostringstream m_body;                              // 0x060
    int m_indent = 1;                                       // 0x1d8
    std::set<std::string> m_extensions;                     // 0x1e0
    std::vector<std::string> m_structs;                     // 0x210  definitions, in order
    std::map<llvm::StructType *, std::string> m_structNames; // 0x228
    std::vector<std::string> m_interface;                   // 0x258  declarations of inputs
    std::vector<std::pair<int, int>> m_framebuffers;        // 0x270  colour attachments that are read: index, component (1 int, 2 uint)
    std::vector<std::string> m_helperOrder;                 // 0x288
    std::map<std::string, std::string> m_helpers;           // 0x2a0  name, text
    std::vector<std::string> m_prologue;                    // 0x2d0  first statements of main()
    llvm::DenseMap<const llvm::Value *, std::string> m_names;     // 0x2e8  variables of instructions
    llvm::DenseMap<const llvm::Value *, std::string> m_arguments; // 0x300  expressions of the parameters
    llvm::DenseMap<const llvm::Value *, Address> m_addresses;     // 0x318
    llvm::SmallPtrSet<const llvm::Value *, 16> m_values;    // 0x330  only reset() touches it
    std::vector<std::pair<std::string, std::string>> m_variables; // 0x3d0  type, name
    int m_nextVariable = 0;                                 // 0x3e8
    std::vector<std::unique_ptr<Root>> m_roots;             // 0x3f0
    llvm::DenseMap<const llvm::Value *, Root *> m_pointerRoots;   // 0x408
    llvm::DenseMap<const llvm::Value *, Root *> m_argumentRoots;  // 0x420
    std::map<int, Texture> m_textures;                      // 0x438  by the Metal slot: [[texture(n)]]
    // pointers.cpp
    std::deque<Argument> m_members;                         // 0x468  elements of resource arrays and resources of argument buffers
    std::vector<std::pair<const llvm::Value *, Argument *>> m_memberValues; // 0x4b8  the globals expandResourceArrays() made, and theirs
    std::map<std::pair<Root *, int64_t>, Root *> m_loadedRoots; // 0x4d0  by the argument buffer's root and the offset in it
    void *m_unused500 = nullptr;                            // 0x500  only the constructor touches it
    std::set<llvm::BasicBlock *> m_written;                 // 0x508  blocks that are in the body
    std::map<llvm::BasicBlock *, int> m_numbers;            // 0x538  blocks of the state machine
    bool m_machine = false;                                 // 0x568  the body is the state machine (nothing reads it)
    std::set<llvm::BasicBlock *> m_tails;                   // 0x570  blocks that soon leave the function: written at each branch
    std::map<llvm::BasicBlock *, llvm::BasicBlock *> m_merges; // 0x5a0  immediate post-dominators
    size_t m_repeated = 0;                                  // 0x5d0  instructions that were written again
    size_t m_repeatLimit = 0;                               // 0x5d8
    bool m_barrier = false;                                 // 0x5e0  a barrier of the threadgroup: main() must not return early
    bool m_unused5e1 = false;                               // 0x5e1  only the constructor touches it
};

// Translates the function of `info`. On failure, the text of the Unsupported is in `error`. 0x439c0
bool translate(llvm::Module &module, const FunctionInfo &info, const Options &options, Shader &shader, std::string &error);

// The vertex attributes of a function, their names kept in `strings`, by their index. 0x35750
std::vector<reef_mtl_attribute> attributesOf(const FunctionInfo &info, std::deque<std::string> &strings);

// The options as text: a part of the key of a cached translation. 0x2e130
std::string optionsKey(const Options &options);

}
