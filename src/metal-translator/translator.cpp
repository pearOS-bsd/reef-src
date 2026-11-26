// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The C interface: libraries from .metallib files and from source, their functions, translation and its reflection.

#include "translator.h"

#include <dlfcn.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBufferRef.h>
#include <llvm/Support/SHA256.h>
#include <llvm/Support/raw_ostream.h>

#include "../metal-msl/msl.h"
#include "cache.h"
#include "emitter.h"
#include "metadata.h"
#include "metallib.h"

using namespace cutefish::reef::metal;

namespace {

// A function of a library: its name, its stage and the module that holds it. (0x58 bytes)
struct FunctionEntry
{
    std::string name;                               // 0x00
    int stage = 0;                                  // 0x20
    std::shared_ptr<const std::string> bitcode;     // 0x28  shared by the functions of one compiled source
    std::string hash;                               // 0x38  SHA-256 of the bitcode in hex, a part of the cache keys
};

struct LibraryData
{
    std::vector<FunctionEntry> functions;
};

// An argument buffer of a function. (0x20 bytes)
struct ArgumentBuffer
{
    int size = 0;                                   // 0x00
    int alignment = 0;                              // 0x04
    std::vector<reef_mtl_argument_member> members;  // 0x08
};

// The texture types of MTLTextureType by the Metal type name without "texture" or "depth" and the template arguments.
const struct
{
    const char *name;
    int type;
} textureTypes[] = {
    {"1d_array", 1},
    {"1d", 0},
    {"2d_ms_array", 8},
    {"2d_ms", 4},
    {"2d_array", 3},
    {"2d", 2},
    {"cube_array", 6},
    {"cube", 5},
    {"3d", 7},
    {"_buffer", 9},
    {"buffer", 9},
};

// The functions of a module by the named metadata that lists them.
const struct
{
    const char *metadata;
    int stage;
} stages[] = {
    {"air.vertex", 1},
    {"air.fragment", 2},
    {"air.kernel", 3},
    {"air.visible", 5},
};

}

struct reef_mtl_library
{
    std::shared_ptr<LibraryData> data;
};

// A function of a library and its reflection. (0xd0 bytes)
struct reef_mtl_function
{
    std::shared_ptr<LibraryData> library;                   // 0x00  keeps `entry` alive
    const FunctionEntry *entry = nullptr;                   // 0x10
    int stage = 0;                                          // 0x18
    std::vector<reef_mtl_attribute> attributes;             // 0x20
    std::vector<reef_mtl_function_constant> constants;      // 0x38
    std::map<int, ArgumentBuffer> argumentBuffers;          // 0x50  by [[buffer(n)]]
    std::deque<std::string> strings;                        // 0x80  what the const char * of the structures point into
};

struct reef_mtl_shader
{
    std::shared_ptr<Shader> shader;
};

// The translations that are alive, by their cache key: a function translated again with the same options is shared.
static std::map<std::string, std::weak_ptr<Shader>> s_shaders;
static std::mutex s_shadersMutex;

// Copies a message into the caller's buffer, cut to its size.
static void setError(char *error, long size, const std::string &message)
{
    if (!error || size <= 0)
        return;
    const size_t length = std::min(static_cast<size_t>(size - 1), message.size());
    memcpy(error, message.data(), length);
    error[length] = 0;
}

// The Metal Shading Language compiler of libreef-metal-msl.so next to this library, null when it is not there.
static MslCompiler mslCompiler()
{
    static const MslCompiler compiler = [] () -> MslCompiler {
        Dl_info info = {};
        if (!dladdr(reinterpret_cast<void *>(&mslCompiler), &info) || !info.dli_fname)
            return nullptr;
        std::string path = info.dli_fname;
        path = path.substr(0, path.rfind('/') + 1) + mslLibrary;
        void *library = dlopen(path.c_str(), RTLD_NOW);
        if (!library)
            return nullptr;
        const auto *symbol = static_cast<const MslCompiler *>(dlsym(library, mslCompilerSymbol));
        return symbol ? *symbol : nullptr;
    }();
    return compiler;
}

static std::unique_ptr<llvm::Module> parseBitcode(llvm::StringRef bitcode, llvm::LLVMContext &context, std::string &error)
{
    const llvm::MemoryBufferRef buffer(bitcode, "air");
    llvm::Expected<std::unique_ptr<llvm::Module>> module = llvm::parseBitcodeFile(buffer, context);
    if (!module) {
        error = "unreadable bitcode: " + llvm::toString(module.takeError());
        return nullptr;
    }
    return std::move(*module);
}

static std::string sha256(const std::string &data)
{
    static const char digits[] = "0123456789abcdef";
    const std::array<uint8_t, 32> hash = llvm::SHA256::hash(llvm::arrayRefFromStringRef<uint8_t>(data));
    std::string text;
    for (const uint8_t byte : hash) {
        text += digits[byte >> 4];
        text += digits[byte & 15];
    }
    return text;
}

// Links the bodies of the visible functions that the module declares into it.
static bool linkFunctions(llvm::Module &module, const reef_mtl_function *const *linked, int count, std::string &error)
{
    for (int i = 0; i < count; ++i) {
        const FunctionEntry &entry = *linked[i]->entry;
        const llvm::Function *declaration = module.getFunction(entry.name);
        if (!declaration || !declaration->isDeclaration())
            continue;
        std::unique_ptr<llvm::Module> other = parseBitcode(*entry.bitcode, module.getContext(), error);
        if (!other)
            return false;
        // Its own lists of used globals and its entry points would clash with the module's.
        for (const char *name : {"llvm.used", "llvm.compiler.used"})
            if (llvm::GlobalVariable *used = other->getGlobalVariable(name))
                used->eraseFromParent();
        for (auto node = other->named_metadata_begin(); node != other->named_metadata_end();) {
            llvm::NamedMDNode &metadata = *node++;
            if (metadata.getName().starts_with("air."))
                other->eraseNamedMetadata(&metadata);
        }
        const llvm::Function *definition = other->getFunction(entry.name);
        if (!definition || definition->isDeclaration()) {
            error = "the linked function " + entry.name + " has no body";
            return false;
        }
        // Only the linked function itself may resolve the module's declarations.
        for (llvm::GlobalValue &value : other->global_values())
            if (&value != definition && !value.isDeclaration())
                value.setLinkage(llvm::GlobalValue::InternalLinkage);
        other->setTargetTriple(module.getTargetTriple());
        other->setDataLayout(module.getDataLayout());
        if (llvm::Linker::linkModules(module, std::move(other), llvm::Linker::LinkOnlyNeeded)) {
            error = "the linked function " + entry.name + " does not fit its declaration";
            return false;
        }
    }
    return true;
}

// Compiles Metal source to AIR bitcode and lists its functions. The compiler's messages go to `error` in any case.
static bool compileSource(const std::string &source, const MslOptions &options, CompiledSource &result, char *error,
                          long errorSize)
{
    const MslCompiler compile = mslCompiler();
    if (!compile) {
        setError(error, errorSize, "this build has no Metal Shading Language compiler");
        return false;
    }
    llvm::LLVMContext context;
    std::string log;
    std::unique_ptr<llvm::Module> module = compile(context, source, options, log);
    setError(error, errorSize, log);
    if (!module)
        return false;
    {
        llvm::raw_string_ostream stream(result.bitcode);
        llvm::WriteBitcodeToFile(*module, stream);
    }
    for (const auto &[metadata, stage] : stages) {
        const llvm::NamedMDNode *node = module->getNamedMetadata(metadata);
        if (!node)
            continue;
        for (const llvm::MDNode *operand : node->operands()) {
            if (operand->getNumOperands() == 0)
                continue;
            const auto *value = llvm::dyn_cast_or_null<llvm::ValueAsMetadata>(operand->getOperand(0).get());
            const auto *function = value ? llvm::dyn_cast<llvm::Function>(value->getValue()) : nullptr;
            if (function)
                result.functions.push_back({function->getName().str(), stage});
        }
    }
    return true;
}

// What a member of an argument buffer is, for the C structure.
static reef_mtl_argument_member argumentMember(const Member &member, std::deque<std::string> &strings)
{
    int kind;
    switch (member.role) {
    case Role::Buffer:
        kind = member.indirect ? 8 : 4;
        break;
    case Role::Texture:
        kind = 2;
        break;
    case Role::Sampler:
        kind = 3;
        break;
    case Role::CommandBuffer:
        kind = 5;
        break;
    case Role::RenderPipelineState:
        kind = 6;
        break;
    case Role::ComputePipelineState:
        kind = 7;
        break;
    default:
        kind = 1;
        break;
    }
    // An array member: the element type, "array<T, N>".
    std::string type = member.typeName;
    if (type.starts_with("array<") && type.rfind(',') != std::string::npos)
        type = type.substr(6, type.rfind(',') - 6);
    int dataType = 0;
    int textureType = 0;
    int textureDataType = 0;
    if (kind == 2) {
        const size_t open = type.find('<');
        std::string name = type.substr(0, open);
        if (name.starts_with("texture"))
            name = name.substr(7);
        else if (name.starts_with("depth"))
            name = name.substr(5);
        textureType = 2;
        for (const auto &entry : textureTypes) {
            if (name == entry.name) {
                textureType = entry.type;
                break;
            }
        }
        std::string component = "float";
        if (open != std::string::npos)
            component = type.substr(open + 1, type.find_first_of(",>", open) - open - 1);
        if (component == "half")
            textureDataType = 16;
        else if (component == "uint")
            textureDataType = 33;
        else if (component == "int")
            textureDataType = 29;
        else if (component == "short")
            textureDataType = 37;
        else if (component == "ushort")
            textureDataType = 41;
        else
            textureDataType = 3;
    } else if (kind == 1) {
        dataType = cutefish::reef::metal::dataType(type);
    }
    strings.push_back(member.name);
    const char *name = strings.back().c_str();
    strings.push_back(member.typeName);
    const char *typeName = strings.back().c_str();
    return {kind,
            member.location,
            member.count,
            member.offset,
            member.size,
            dataType,
            textureType,
            textureDataType,
            static_cast<int>(member.access & 7),
            member.addressSpace,
            member.typeSize,
            member.alignment,
            name,
            typeName};
}

int reef_mtl_translator_version()
{
    return 1;
}

int reef_mtl_library_function_count(const reef_mtl_library *library)
{
    return library ? library->data->functions.size() : 0;
}

const char *reef_mtl_library_function_name(const reef_mtl_library *library, int index)
{
    if (!library || index < 0)
        return nullptr;
    const std::vector<FunctionEntry> &functions = library->data->functions;
    if (static_cast<size_t>(index) >= functions.size())
        return nullptr;
    return functions[index].name.c_str();
}

int reef_mtl_library_function_stage(const reef_mtl_library *library, int index)
{
    if (!library || index < 0)
        return 0;
    const std::vector<FunctionEntry> &functions = library->data->functions;
    if (static_cast<size_t>(index) >= functions.size())
        return 0;
    return functions[index].stage;
}

const char *reef_mtl_function_name(const reef_mtl_function *function)
{
    return function ? function->entry->name.c_str() : nullptr;
}

int reef_mtl_function_stage(const reef_mtl_function *function)
{
    return function ? function->stage : 0;
}

int reef_mtl_function_attribute_count(const reef_mtl_function *function)
{
    return function ? function->attributes.size() : 0;
}

const reef_mtl_attribute *reef_mtl_function_attribute(const reef_mtl_function *function, int index)
{
    if (!function || index < 0 || static_cast<size_t>(index) >= function->attributes.size())
        return nullptr;
    return &function->attributes[index];
}

int reef_mtl_function_argument_buffer(const reef_mtl_function *function, int index, int *size, int *alignment)
{
    if (size)
        *size = 0;
    if (alignment)
        *alignment = 0;
    if (!function)
        return 0;
    const auto found = function->argumentBuffers.find(index);
    if (found == function->argumentBuffers.end())
        return 0;
    if (size)
        *size = found->second.size;
    if (alignment)
        *alignment = found->second.alignment;
    return found->second.members.size();
}

const reef_mtl_argument_member *reef_mtl_function_argument_member(const reef_mtl_function *function, int index, int member)
{
    if (!function)
        return nullptr;
    const auto found = function->argumentBuffers.find(index);
    if (found == function->argumentBuffers.end() || member < 0)
        return nullptr;
    const std::vector<reef_mtl_argument_member> &members = found->second.members;
    if (static_cast<size_t>(member) >= members.size())
        return nullptr;
    return &members[member];
}

int reef_mtl_function_constant_count(const reef_mtl_function *function)
{
    return function ? function->constants.size() : 0;
}

const reef_mtl_function_constant *reef_mtl_function_constant_at(const reef_mtl_function *function, int index)
{
    if (!function || index < 0 || static_cast<size_t>(index) >= function->constants.size())
        return nullptr;
    return &function->constants[index];
}

int reef_mtl_shader_stage(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->stage : 0;
}

const char *reef_mtl_shader_glsl(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->glsl.c_str() : nullptr;
}

uint64_t reef_mtl_shader_flags(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->flags : 0;
}

int reef_mtl_shader_binding_count(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->bindings.size() : 0;
}

const reef_mtl_binding *reef_mtl_shader_binding(const reef_mtl_shader *shader, int index)
{
    if (!shader)
        return nullptr;
    const std::vector<reef_mtl_binding> &bindings = shader->shader->bindings;
    if (index < 0 || static_cast<size_t>(index) >= bindings.size())
        return nullptr;
    return &bindings[index];
}

int reef_mtl_shader_constant_sampler_count(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->samplers.size() : 0;
}

const reef_mtl_constant_sampler *reef_mtl_shader_constant_sampler(const reef_mtl_shader *shader, int index)
{
    if (!shader)
        return nullptr;
    const std::vector<reef_mtl_constant_sampler> &samplers = shader->shader->samplers;
    if (index < 0 || static_cast<size_t>(index) >= samplers.size())
        return nullptr;
    return &samplers[index];
}

int reef_mtl_shader_attribute_count(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->attributes.size() : 0;
}

const reef_mtl_attribute *reef_mtl_shader_attribute(const reef_mtl_shader *shader, int index)
{
    if (!shader)
        return nullptr;
    const std::vector<reef_mtl_attribute> &attributes = shader->shader->attributes;
    if (index < 0 || static_cast<size_t>(index) >= attributes.size())
        return nullptr;
    return &attributes[index];
}

int reef_mtl_shader_varying_count(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->varyings.size() : 0;
}

const reef_mtl_varying *reef_mtl_shader_varying(const reef_mtl_shader *shader, int index)
{
    if (!shader)
        return nullptr;
    const std::vector<reef_mtl_varying> &varyings = shader->shader->varyings;
    if (index < 0 || static_cast<size_t>(index) >= varyings.size())
        return nullptr;
    return &varyings[index];
}

int reef_mtl_shader_render_target_count(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->renderTargets.size() : 0;
}

const reef_mtl_render_target *reef_mtl_shader_render_target(const reef_mtl_shader *shader, int index)
{
    if (!shader)
        return nullptr;
    const std::vector<reef_mtl_render_target> &renderTargets = shader->shader->renderTargets;
    if (index < 0 || static_cast<size_t>(index) >= renderTargets.size())
        return nullptr;
    return &renderTargets[index];
}

int reef_mtl_shader_clip_distance_count(const reef_mtl_shader *shader)
{
    return shader ? shader->shader->clipDistances : 0;
}

int reef_mtl_shader_uniform_location(const reef_mtl_shader *shader, int index)
{
    if (!shader || index < 0 || index >= 2)
        return -1;
    return shader->shader->uniformLocations[index];
}

void reef_mtl_shader_threadgroup_size(const reef_mtl_shader *shader, int *x, int *y, int *z)
{
    int width = 1;
    int height = 1;
    int depth = 1;
    if (shader) {
        width = shader->shader->threadgroupSize[0];
        height = shader->shader->threadgroupSize[1];
        depth = shader->shader->threadgroupSize[2];
    }
    if (x)
        *x = width;
    if (y)
        *y = height;
    if (z)
        *z = depth;
}

void reef_mtl_library_release(reef_mtl_library *library)
{
    delete library;
}

void reef_mtl_shader_release(reef_mtl_shader *shader)
{
    delete shader;
}

void reef_mtl_function_release(reef_mtl_function *function)
{
    delete function;
}

reef_mtl_library *reef_mtl_library_create_from_data(const void *data, long size, char *error, long errorSize)
{
    std::vector<LibraryFunction> functions;
    std::string message;
    if (!data || size <= 0 || !readLibrary(data, size, functions, message)) {
        setError(error, errorSize, message.empty() ? std::string("no library data") : message);
        return nullptr;
    }
    const auto library = std::make_shared<LibraryData>();
    for (LibraryFunction &function : functions) {
        FunctionEntry entry;
        entry.name = std::move(function.name);
        entry.stage = function.stage;
        entry.hash = sha256(function.bitcode);
        entry.bitcode = std::make_shared<const std::string>(std::move(function.bitcode));
        library->functions.push_back(std::move(entry));
    }
    return new reef_mtl_library{library};
}

reef_mtl_library *reef_mtl_library_create_from_source(const char *source, long length, const reef_mtl_source_options *options,
                                                      char *error, long errorSize)
{
    MslOptions mslOptions;
    if (options) {
        mslOptions.languageVersion = options->languageVersion;
        mslOptions.fastMath = options->fastMath != 0;
        for (int i = 0; i < options->macroCount && options->names; ++i) {
            const char *name = options->names[i];
            const char *value = options->values && options->values[i] ? options->values[i] : "1";
            if (name)
                mslOptions.macros.emplace_back(name, value);
        }
    }
    const std::string text = source ? std::string(source, length < 0 ? strlen(source) : length) : std::string();
    // What decides the compilation: the options, then the source after a NUL.
    std::string key = "msl " + std::to_string(mslOptions.languageVersion) + ' ' + static_cast<char>('0' + mslOptions.fastMath);
    for (const auto &[name, value] : mslOptions.macros)
        key += '\n' + name + '=' + value;
    key += '\0' + text;
    CompiledSource compiled;
    if (!loadCompiledSource(key, compiled)) {
        compiled = CompiledSource();
        if (!compileSource(text, mslOptions, compiled, error, errorSize))
            return nullptr;
        storeCompiledSource(key, compiled);
    }
    const auto bitcode = std::make_shared<const std::string>(std::move(compiled.bitcode));
    const std::string hash = sha256(*bitcode);
    const auto library = std::make_shared<LibraryData>();
    for (CompiledFunction &function : compiled.functions) {
        FunctionEntry entry;
        entry.name = std::move(function.name);
        entry.stage = function.stage;
        entry.bitcode = bitcode;
        entry.hash = hash;
        library->functions.push_back(std::move(entry));
    }
    return new reef_mtl_library{library};
}

reef_mtl_function *reef_mtl_function_create(const reef_mtl_library *library, const char *name, char *error, long errorSize)
{
    if (!library || !name) {
        setError(error, errorSize, "no library or no name");
        return nullptr;
    }
    for (const FunctionEntry &entry : library->data->functions) {
        if (entry.name != name)
            continue;
        auto function = std::make_unique<reef_mtl_function>();
        function->library = library->data;
        function->entry = &entry;
        llvm::LLVMContext context;
        std::string message;
        const std::unique_ptr<llvm::Module> module = parseBitcode(*entry.bitcode, context, message);
        FunctionInfo info;
        if (!module || !readFunction(*module, entry.name, info, message)) {
            setError(error, errorSize, message);
            return nullptr;
        }
        function->stage = info.stage;
        function->attributes = attributesOf(info, function->strings);
        for (const Argument &argument : info.arguments) {
            if (argument.role != Role::Buffer || !argument.indirect)
                continue;
            ArgumentBuffer &buffer = function->argumentBuffers[argument.location];
            buffer.size = argument.size;
            buffer.alignment = argument.alignment;
            for (const Member &member : argument.members)
                buffer.members.push_back(argumentMember(member, function->strings));
        }
        for (const FunctionConstant &constant : info.constants) {
            function->strings.push_back(constant.name);
            reef_mtl_function_constant item = {};
            item.index = constant.index;
            item.type = constant.type;
            item.required = constant.required;
            item.name = function->strings.back().c_str();
            function->constants.push_back(item);
        }
        return function.release();
    }
    setError(error, errorSize, "the library has no function " + std::string(name));
    return nullptr;
}

reef_mtl_shader *reef_mtl_function_translate_linked(const reef_mtl_function *function, const reef_mtl_function *const *linked,
                                                    int linkedCount, const reef_mtl_translate_options *options, char *error,
                                                    long errorSize)
{
    if (!function) {
        setError(error, errorSize, "no function");
        return nullptr;
    }
    const FunctionEntry &entry = *function->entry;
    if (function->stage == 5) {
        setError(error, errorSize, entry.name + " is a visible function, linked into its callers");
        return nullptr;
    }
    for (int i = 0; i < linkedCount; ++i) {
        if (!linked || !linked[i] || linked[i]->stage != 5) {
            setError(error, errorSize, "only visible functions are linked");
            return nullptr;
        }
    }
    Options translation;
    if (options) {
        translation.firstBuffer = options->firstBuffer;
        translation.firstTexture = options->firstTexture;
        translation.firstImage = options->firstImage;
        translation.firstLocation = options->firstLocation;
        for (int i = 0; i < 3; ++i)
            translation.threadgroupSize[i] = options->threadgroupSize[i] > 0 ? options->threadgroupSize[i] : 1;
        translation.last = options->flags;
        for (int i = 0; i < options->valueCount && options->values; ++i) {
            const reef_mtl_constant_value &value = options->values[i];
            ConstantValue constant;
            constant.index = value.index;
            if (value.name)
                constant.name = value.name;
            constant.type = value.type;
            const int size = dataTypeSize(value.type);
            if (size <= 0 || !value.data) {
                setError(error, errorSize, "a function constant value of an unknown type");
                return nullptr;
            }
            constant.bytes.assign(static_cast<const char *>(value.data), size);
            translation.constants.push_back(std::move(constant));
        }
        for (int i = 0; i < options->lengthCount && options->lengths; ++i)
            translation.threadgroupMemory[options->lengths[i].index] = options->lengths[i].length;
    }
    // What decides the translation: the bitcode, the function, the options and the bitcode linked into it.
    std::string key = entry.hash + "\n" + entry.name + "\n" + optionsKey(translation);
    for (int i = 0; i < linkedCount; ++i)
        key += "\nlinked " + linked[i]->entry->hash + " " + linked[i]->entry->name;
    {
        const std::lock_guard<std::mutex> lock(s_shadersMutex);
        const auto found = s_shaders.find(key);
        if (found != s_shaders.end())
            if (std::shared_ptr<Shader> shader = found->second.lock())
                return new reef_mtl_shader{std::move(shader)};
    }
    if (std::shared_ptr<Shader> shader = loadShader(key)) {
        const std::lock_guard<std::mutex> lock(s_shadersMutex);
        s_shaders[key] = shader;
        return new reef_mtl_shader{std::move(shader)};
    }
    llvm::LLVMContext context;
    std::string message;
    std::unique_ptr<llvm::Module> module = parseBitcode(*entry.bitcode, context, message);
    if (module && linkedCount > 0 && !linkFunctions(*module, linked, linkedCount, message))
        module.reset();
    FunctionInfo info;
    auto shader = std::make_shared<Shader>();
    if (!module || !readFunction(*module, entry.name, info, message) || !translate(*module, info, translation, *shader, message)) {
        setError(error, errorSize, entry.name + ": " + message);
        return nullptr;
    }
    storeShader(key, *shader);
    const std::lock_guard<std::mutex> lock(s_shadersMutex);
    s_shaders[key] = shader;
    return new reef_mtl_shader{std::move(shader)};
}

reef_mtl_shader *reef_mtl_function_translate(const reef_mtl_function *function, const reef_mtl_translate_options *options,
                                             char *error, long errorSize)
{
    return reef_mtl_function_translate_linked(function, nullptr, 0, options, error, errorSize);
}
