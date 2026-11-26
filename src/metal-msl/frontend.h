// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the parts of the front end have in common.

#pragma once

#include <clang/AST/Type.h>
#include <llvm/ADT/StringRef.h>

#include <optional>
#include <string>
#include <vector>

namespace clang {
class ASTContext;
class BuiltinType;
class CXXRecordDecl;
class CodeGenerator;
class Decl;
class DiagnosticsEngine;
}

namespace clang {
class RecordDecl;
}

namespace llvm {
class Function;
class LLVMContext;
class MDNode;
class Metadata;
class Module;
}

namespace cutefish::reef::metal::msl {

// The headers of the Metal standard library; the list ends with a null name.
struct Header {
    const char* name;
    const char* text;
};

extern const Header headers[];

// Address spaces that qualify member functions and what else is not C++ in a way the preprocessor can not mend.
std::string prepareSource(const std::string& source);

// Takes constexpr from the variables that the compiler reports as not constant.
bool relaxConstants(std::string& source, const std::string& errors);

// What an attribute left on a declaration: [[buffer(2)]] is {"buffer", {"2"}}.
struct Annotation {
    std::string name;
    std::vector<std::string> arguments;
};

std::vector<Annotation> annotations(const clang::Decl* decl);
const Annotation* findAnnotation(const std::vector<Annotation>& annotations, llvm::StringRef name);
// The argument as a number, 0 when there is none.
long long annotationInteger(const Annotation& annotation, size_t index);

// A texture type of the standard library: texture2d<float, access::read> is {"texture2d", "float", 1}.
struct Texture {
    std::string name;
    std::string scalar;
    int access = 0;
};

// A class template of namespace metal, of the given name if there is one.
const clang::CXXRecordDecl* metalTemplate(clang::QualType type, llvm::StringRef name = {});
std::optional<Texture> textureType(clang::QualType type);
bool isSampler(clang::QualType type);
const char* accessName(int access);
std::string scalarName(const clang::BuiltinType* type);
// The name of a type as Metal spells it: float4, texture2d<float, sample>, float4x4.
std::string typeName(clang::QualType type);

// An argument buffer: a structure of resources that a buffer argument points to.
bool isArgumentBuffer(const clang::RecordDecl* record);
// What AIR says of the members of an argument buffer.
llvm::MDNode* argumentBufferInfo(clang::ASTContext& context, llvm::LLVMContext& llvm, const clang::RecordDecl* record);

struct EntryPoint {
    std::string name;
    // 1 vertex, 2 fragment, 3 kernel, 5 visible.
    int kind = 0;
    llvm::Function* function = nullptr;
    std::vector<llvm::Metadata*> outputs;
    std::vector<std::vector<llvm::Metadata*>> arguments;
    // air.early_fragment_tests, air.max_total_threads_per_threadgroup.
    std::vector<llvm::Metadata*> attributes;
};

bool buildEntryPoints(clang::ASTContext& context, clang::CodeGenerator& code, clang::DiagnosticsEngine& diagnostics,
                      std::vector<EntryPoint>& entries);

struct ModuleOptions {
    bool fastMath;
    int majorVersion;
    int minorVersion;
};

// Turns what clang made into a module of AIR.
bool lowerModule(llvm::Module& module, std::vector<EntryPoint>& entries, const ModuleOptions& options, std::string& error);

}
