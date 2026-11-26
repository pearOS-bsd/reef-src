// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Runs clang over a program, with the standard library from memory.

#include "frontend.h"
#include "msl.h"

#include <clang/AST/ASTConsumer.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/CodeGen/ModuleBuilder.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/MultiplexConsumer.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Sema/ParsedAttr.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <iterator>
#include <mutex>

using namespace cutefish::reef::metal;

namespace {

constexpr char directory[] = "/metal";
constexpr char sourceName[] = "program_source";

const char* const fastMathFlags[] = {
    "-ffast-math", "-menable-no-infs", "-menable-no-nans", "-fapprox-func", "-funsafe-math-optimizations",
    "-fno-signed-zeros", "-mreassociate", "-freciprocal-math", "-ffp-contract=fast",
};

struct Output {
    std::unique_ptr<llvm::Module> module;
    std::vector<msl::EntryPoint> entries;
};

// Builds the entry points when the whole program is there and the code generator has seen it.
class EntryConsumer : public clang::ASTConsumer {
public:
    EntryConsumer(clang::CompilerInstance& compiler, clang::CodeGenerator& code, Output& output)
        : m_compiler(compiler)
        , m_code(code)
        , m_output(output)
    {
    }

    void HandleTranslationUnit(clang::ASTContext& context) override
    {
        if (m_compiler.getDiagnostics().hasErrorOccurred())
            return;
        if (!msl::buildEntryPoints(context, m_code, m_compiler.getDiagnostics(), m_output.entries))
            return;
        m_output.module.reset(m_code.ReleaseModule());
    }

private:
    clang::CompilerInstance& m_compiler;
    clang::CodeGenerator& m_code;
    Output& m_output;
};

// The address spaces are those of AIR: constant is its own one, which SPIR has no number for.
struct MetalTarget : clang::TargetInfo {
    static void setAddressSpaces(clang::TargetInfo& target)
    {
        static clang::LangASMap map;
        static std::once_flag once;
        auto& metal = static_cast<MetalTarget&>(target);
        std::call_once(once, [&] {
            std::copy(std::begin(*metal.AddrSpaceMap), std::end(*metal.AddrSpaceMap), map);
            map[unsigned(clang::LangAS::sycl_global_device)] = 2;
        });
        metal.AddrSpaceMap = &map;
    }
};

class CompileAction : public clang::ASTFrontendAction {
public:
    CompileAction(llvm::LLVMContext& context, Output& output)
        : m_context(context)
        , m_output(output)
    {
    }

    bool BeginSourceFileAction(clang::CompilerInstance& compiler) override
    {
        MetalTarget::setAddressSpaces(compiler.getTarget());
        return true;
    }

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef) override
    {
        std::unique_ptr<clang::CodeGenerator> code(
            clang::CreateLLVMCodeGen(compiler.getDiagnostics(), "program", &compiler.getVirtualFileSystem(), compiler.getHeaderSearchOpts(),
                                     compiler.getPreprocessorOpts(), compiler.getCodeGenOpts(), m_context));
        clang::CodeGenerator& generator = *code;
        std::vector<std::unique_ptr<clang::ASTConsumer>> consumers;
        consumers.push_back(std::move(code));
        consumers.push_back(std::make_unique<EntryConsumer>(compiler, generator, m_output));
        return std::make_unique<clang::MultiplexConsumer>(std::move(consumers));
    }

private:
    llvm::LLVMContext& m_context;
    Output& m_output;
};

std::unique_ptr<llvm::Module> compile(llvm::LLVMContext& context, const std::string& source, const MslOptions& options, int major, int minor,
                                      std::string& error)
{
    auto files = llvm::makeIntrusiveRefCnt<llvm::vfs::InMemoryFileSystem>();
    for (const msl::Header* header = msl::headers; header->name; header++) {
        files->addFile(std::string(directory) + "/include/" + header->name, 0,
                       llvm::MemoryBuffer::getMemBuffer(header->text, header->name));
    }
    files->addFile(std::string(directory) + "/" + sourceName, 0, llvm::MemoryBuffer::getMemBufferCopy(source, sourceName));
    files->setCurrentWorkingDirectory(directory);

    std::vector<std::string> arguments = {
        "-triple", "spir64-unknown-unknown",
        "-fsycl-is-device",
        "-fdeclspec",
        "-std=c++17",
        "-fno-rtti",
        "-fno-threadsafe-statics",
        "-fno-builtin",
        "-fpreserve-vec3-type",
        "-cl-single-precision-constant",
        "-O2",
        "-nostdsysteminc",
        "-nobuiltininc",
        "-isystem", std::string(directory) + "/include",
        "-include", "metal_types",
        "-Wno-duplicate-decl-specifier",
        "-ferror-limit", "20",
        "-main-file-name", sourceName,
        "-D__METAL__=1",
        "-D__METAL_MACOS__=1",
        "-D__METAL_VERSION__=" + std::to_string(major * 100 + minor * 10),
        "-D__HAVE_NATIVE_FAST_MATH__=1",
    };
    if (options.fastMath) {
        for (const char* flag : fastMathFlags)
            arguments.push_back(flag);
    } else {
        arguments.push_back("-ffp-contract=off");
    }
    for (const auto& [name, value] : options.macros)
        arguments.push_back("-D" + name + "=" + value);
    arguments.push_back("-x");
    arguments.push_back("c++");
    arguments.push_back(sourceName);

    std::string log;
    llvm::raw_string_ostream stream(log);
    auto diagnosticOptions = llvm::makeIntrusiveRefCnt<clang::DiagnosticOptions>();
    diagnosticOptions->ShowColors = 0;
    diagnosticOptions->ShowOptionNames = 0;
    auto* printer = new clang::TextDiagnosticPrinter(stream, diagnosticOptions.get());
    clang::CompilerInstance compiler;
    compiler.createDiagnostics(printer, true);

    std::vector<const char*> pointers;
    for (const std::string& argument : arguments)
        pointers.push_back(argument.c_str());
    if (!clang::CompilerInvocation::CreateFromArgs(compiler.getInvocation(), pointers, compiler.getDiagnostics())) {
        error = log.empty() ? "the compiler rejected its own options" : log;
        return nullptr;
    }
    compiler.getDiagnosticOpts().ShowColors = 0;
    compiler.getDiagnosticOpts().ShowOptionNames = 0;
    clang::ProcessWarningOptions(compiler.getDiagnostics(), compiler.getDiagnosticOpts());
    compiler.getDiagnostics().setErrorLimit(compiler.getDiagnosticOpts().ErrorLimit);
    compiler.createFileManager(files);

    Output output;
    CompileAction action(context, output);
    bool done = false;
    if (compiler.createTarget() && action.BeginSourceFile(compiler, compiler.getFrontendOpts().Inputs[0])) {
        done = !llvm::errorToBool(action.Execute());
        action.EndSourceFile();
    }
    compiler.getDiagnosticClient().finish();
    stream.flush();
    error = log;
    if (!done || !output.module || compiler.getDiagnostics().hasErrorOccurred())
        return nullptr;

    std::string message;
    if (!msl::lowerModule(*output.module, output.entries, {options.fastMath, major, minor}, message)) {
        error += std::string(sourceName) + ": error: " + message + "\n";
        return nullptr;
    }
    return std::move(output.module);
}

}

namespace cutefish::reef::metal {

__attribute__((visibility("default"))) std::unique_ptr<llvm::Module> compileMsl(llvm::LLVMContext& context, const std::string& source,
                                                                                const MslOptions& options, std::string& error)
{
    // clang makes the list of attributes when it first looks at it, without a lock.
    static std::once_flag once;
    std::call_once(once, [] { clang::getAttributePluginInstances(); });

    int major = 3;
    int minor = 2;
    if (options.languageVersion) {
        major = options.languageVersion >> 16;
        minor = options.languageVersion & 0xffff;
    }
    std::string text = msl::prepareSource(source);
    for (int attempt = 0;; attempt++) {
        std::unique_ptr<llvm::Module> module = compile(context, text, options, major, minor, error);
        if (module || attempt == 8 || !msl::relaxConstants(text, error))
            return module;
    }
}

}

extern "C" __attribute__((visibility("default"))) const MslCompiler reef_mtl_msl_compiler = compileMsl;
