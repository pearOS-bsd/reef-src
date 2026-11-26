// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The entry points: AIR's function for each vertex, fragment and kernel function, and what its arguments are.

#include "frontend.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/GlobalDecl.h>
#include <clang/AST/Mangle.h>
#include <clang/AST/RecordLayout.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/CodeGen/CodeGenABITypes.h>
#include <clang/CodeGen/ModuleBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace cutefish::reef::metal::msl {
namespace {

struct Interpolation {
    const char* name;
    const char* sampling;
    const char* perspective;
};

const Interpolation interpolations[] = {
    {"center_perspective", "air.center", "air.perspective"},
    {"center_no_perspective", "air.center", "air.no_perspective"},
    {"centroid_perspective", "air.centroid", "air.perspective"},
    {"centroid_no_perspective", "air.centroid", "air.no_perspective"},
    {"sample_perspective", "air.sample", "air.perspective"},
    {"sample_no_perspective", "air.sample", "air.no_perspective"},
};

// The attributes of arguments the hardware fills in, and their names in AIR.
struct Builtin {
    const char* name;
    const char* air;
};

const Builtin kernelInputs[] = {
    {"thread_position_in_grid", "air.thread_position_in_grid"},
    {"thread_position_in_threadgroup", "air.thread_position_in_threadgroup"},
    {"threadgroup_position_in_grid", "air.threadgroup_position_in_grid"},
    {"threads_per_threadgroup", "air.threads_per_threadgroup"},
    {"threads_per_grid", "air.threads_per_grid"},
    {"threadgroups_per_grid", "air.threadgroups_per_grid"},
    {"thread_index_in_threadgroup", "air.thread_index_in_threadgroup"},
    {"thread_index_in_simdgroup", "air.thread_index_in_simdgroup"},
    {"thread_index_in_quadgroup", "air.thread_index_in_quadgroup"},
    {"threads_per_simdgroup", "air.threads_per_simdgroup"},
    {"thread_execution_width", "air.thread_execution_width"},
    {"simdgroup_index_in_threadgroup", "air.simdgroup_index_in_threadgroup"},
    {"simdgroups_per_threadgroup", "air.simdgroups_per_threadgroup"},
    {"quadgroup_index_in_threadgroup", "air.quadgroup_index_in_threadgroup"},
    {"quadgroups_per_threadgroup", "air.quadgroups_per_threadgroup"},
};

const Builtin fragmentInputs[] = {
    {"front_facing", "air.front_facing"},
    {"point_coord", "air.point_coord"},
    {"sample_id", "air.sample_id"},
    {"sample_mask", "air.sample_mask_in"},
    {"primitive_id", "air.primitive_id"},
    {"render_target_array_index", "air.render_target_array_index"},
    {"viewport_array_index", "air.viewport_array_index"},
    {"amplification_id", "air.amplification_id"},
    {"amplification_count", "air.amplification_count"},
    {"barycentric_coord", "air.barycentric_coord"},
};

const Builtin vertexInputs[] = {
    {"vertex_id", "air.vertex_id"},
    {"instance_id", "air.instance_id"},
    {"base_vertex", "air.base_vertex"},
    {"base_instance", "air.base_instance"},
    {"amplification_id", "air.amplification_id"},
    {"amplification_count", "air.amplification_count"},
};

// A member of a structure that is passed or returned piece by piece, with its offset in bytes.
struct Leaf {
    const clang::ValueDecl* decl;
    clang::QualType type;
    int64_t offset;
};

// An argument of the function that AIR sees.
struct Argument {
    llvm::Type* type = nullptr;
    std::vector<llvm::Metadata*> metadata;
    bool nonnull = false;
};

// How a parameter of the function in the program is made of the arguments that AIR sees.
struct Parameter {
    // Direct: one argument. Members: one argument for each leaf, stored at its offset. Memory: one argument stored whole.
    enum Kind { Direct, Members, Memory };
    int kind = Direct;
    unsigned first = 0;
    std::vector<Leaf> leaves;
    clang::QualType type;
};

// An argument without an index of its own: it gets the first one its kind leaves free.
struct Unbound {
    std::string attribute;
    size_t argument;
};

// A scalar, a vector or an enumeration: what a single argument or output can be.
inline bool isScalar(clang::QualType type)
{
    const clang::Type* canonical = type.getCanonicalType().getTypePtr();
    return canonical->isBuiltinType() || canonical->isVectorType() || canonical->isEnumeralType();
}

class EntryBuilder {
public:
    EntryBuilder(clang::ASTContext& context, clang::CodeGenerator& code, clang::DiagnosticsEngine& diagnostics)
        : m_context(context)
        , m_code(code)
        , m_diagnostics(diagnostics)
        , m_module(*code.GetModule())
        , m_llvm(m_module.getContext())
        , m_mangle(clang::ItaniumMangleContext::create(context, diagnostics))
    {
    }

    clang::CodeGenerator& code() { return m_code; }
    llvm::Module& module() { return m_module; }
    llvm::LLVMContext& llvm() { return m_llvm; }

    llvm::MDString* text(llvm::StringRef value) { return llvm::MDString::get(m_llvm, value); }

    void fail(const clang::Decl* decl, const std::string& message)
    {
        const unsigned id = m_diagnostics.getCustomDiagID(clang::DiagnosticsEngine::Error, "%0");
        m_diagnostics.Report(decl->getLocation(), id) << message;
        m_failed = true;
    }

    bool buildEntryPoint(const clang::FunctionDecl* function, int kind, EntryPoint& entry);

    void listFunctionConstants(const clang::TranslationUnitDecl* unit)
    {
        std::vector<llvm::Metadata*> constants;
        std::function<void(const clang::DeclContext*)> visit = [&](const clang::DeclContext* scope) {
            for (const clang::Decl* decl : scope->decls()) {
                if (const auto* space = llvm::dyn_cast<clang::NamespaceDecl>(decl)) {
                    visit(space);
                    continue;
                }
                const auto* variable = llvm::dyn_cast<clang::VarDecl>(decl);
                if (!variable)
                    continue;
                const std::vector<Annotation> notes = annotations(variable);
                const Annotation* constant = findAnnotation(notes, "function_constant");
                if (!constant)
                    continue;
                if (variable->getInit()) {
                    fail(variable, "a function constant takes its value from the pipeline, not an initializer");
                    continue;
                }
                const std::string index = constant->arguments.empty() ? std::string() : constant->arguments[0];
                if (index.empty() || index.find_first_not_of("0123456789") != std::string::npos) {
                    fail(variable, "a function constant needs an index: [[function_constant(n)]]");
                    continue;
                }
                const long long number = std::stoll(index);
                llvm::Value* global = m_code.GetAddrOfGlobal(clang::GlobalDecl(variable), false)->stripPointerCasts();
                if (!llvm::isa<llvm::GlobalVariable>(global))
                    continue;
                constants.push_back(llvm::MDTuple::get(m_llvm, {
                    llvm::ValueAsMetadata::get(global),
                    text("air.function_constant"),
                    llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_llvm), number)),
                    text("air.arg_type_name"),
                    text(typeName(variable->getType())),
                    text("air.arg_name"),
                    text(variable->getName()),
                }));
            }
        };
        visit(unit);
        if (constants.empty())
            return;
        llvm::NamedMDNode* node = m_module.getOrInsertNamedMetadata("air.function_constants");
        for (llvm::Metadata* constant : constants)
            node->addOperand(llvm::cast<llvm::MDNode>(constant));
    }

private:
    llvm::Metadata* integer(uint64_t value)
    {
        return llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_llvm), value));
    }

    llvm::Type* memoryType(clang::QualType type)
    {
        if (type->isBooleanType())
            return llvm::Type::getInt1Ty(m_llvm);
        return clang::CodeGen::convertTypeForMemory(m_code.CGM(), type);
    }

    void collectLeaves(const clang::RecordDecl* record, int64_t offset, std::vector<Leaf>& leaves);
    std::string locationName(const clang::ValueDecl* decl);
    bool addInput(int kind, const Leaf& leaf, std::vector<Argument>& arguments);
    bool addOutput(int kind, const Leaf& leaf, std::vector<llvm::Metadata*>& outputs, bool& position);
    bool addParameter(int kind, const clang::ParmVarDecl* parameter, std::vector<Argument>& arguments, Parameter& result,
                      std::map<std::string, std::set<long>>& used, std::vector<Unbound>& unbound);

    clang::ASTContext& m_context;
    clang::CodeGenerator& m_code;
    clang::DiagnosticsEngine& m_diagnostics;
    llvm::Module& m_module;
    llvm::LLVMContext& m_llvm;
    std::unique_ptr<clang::MangleContext> m_mangle;
    bool m_failed = false;
};

void EntryBuilder::collectLeaves(const clang::RecordDecl* record, int64_t offset, std::vector<Leaf>& leaves)
{
    const clang::ASTRecordLayout& layout = m_context.getASTRecordLayout(record);
    for (const clang::FieldDecl* field : record->fields()) {
        const int64_t at = offset + layout.getFieldOffset(field->getFieldIndex()) / 8;
        const clang::QualType type = field->getType().getCanonicalType();
        const clang::RecordDecl* inner = type->getAsRecordDecl();
        if (inner && !isScalar(type) && !textureType(type)) {
            collectLeaves(inner, at, leaves);
            continue;
        }
        leaves.push_back({field, field->getType(), at});
    }
}

std::string EntryBuilder::locationName(const clang::ValueDecl* decl)
{
    const std::vector<Annotation> notes = annotations(decl);
    const Annotation* user = findAnnotation(notes, "user");
    if (user && !user->arguments.empty())
        return "user(" + user->arguments[0] + ")";
    // Without [[user(name)]] the name and the type of the member make the location, as Metal does.
    std::string mangled;
    llvm::raw_string_ostream stream(mangled);
    m_mangle->mangleCXXRTTIName(decl->getType().getCanonicalType().getUnqualifiedType(), stream);
    stream.flush();
    llvm::StringRef type = mangled;
    type.consume_front("_ZTS");
    const std::string name = decl->getNameAsString();
    return "generated(" + std::to_string(name.size()) + name + type.str() + ")";
}

bool EntryBuilder::addInput(int kind, const Leaf& leaf, std::vector<Argument>& arguments)
{
    const std::vector<Annotation> notes = annotations(leaf.decl);
    Argument argument;
    if (!isScalar(leaf.type)) {
        fail(leaf.decl, "'" + leaf.decl->getNameAsString() + "' has a type an input cannot have");
        return false;
    }
    argument.type = memoryType(leaf.type);
    const std::string type = typeName(leaf.type);
    const std::string name = leaf.decl->getNameAsString();
    if (kind == 1) {
        const Annotation* attribute = findAnnotation(notes, "attribute");
        if (!attribute) {
            fail(leaf.decl, "the stage_in member '" + name + "' needs an [[attribute(n)]]");
            return false;
        }
        argument.metadata = {
            text("air.vertex_input"),
            text("air.location_index"),
            integer(annotationInteger(*attribute, 0)),
            integer(1),
        };
    } else if (kind == 2) {
        if (findAnnotation(notes, "position")) {
            argument.metadata = {
                text("air.position"),
                text("air.center"),
                text("air.no_perspective"),
            };
        } else {
            bool builtin = false;
            for (const Builtin& input : fragmentInputs) {
                if (findAnnotation(notes, input.name)) {
                    argument.metadata = {text(input.air)};
                    builtin = true;
                    break;
                }
            }
            if (!builtin) {
                argument.metadata = {text("air.fragment_input"), text(locationName(leaf.decl))};
                const std::function<llvm::Metadata*(llvm::StringRef)> qualifier = [this](llvm::StringRef value) {
                    return llvm::MDString::get(m_llvm, value);
                };
                const clang::Type* scalar = leaf.type.getCanonicalType().getTypePtr();
                if (const auto* vector = scalar->getAs<clang::VectorType>())
                    scalar = vector->getElementType().getTypePtr();
                const bool floating = scalar->isFloatingType();
                if (!findAnnotation(notes, "flat") && floating) {
                    bool interpolated = false;
                    for (const Interpolation& interpolation : interpolations) {
                        if (findAnnotation(notes, interpolation.name)) {
                            argument.metadata.push_back(qualifier(interpolation.sampling));
                            argument.metadata.push_back(qualifier(interpolation.perspective));
                            interpolated = true;
                            break;
                        }
                    }
                    if (!interpolated) {
                        argument.metadata.push_back(qualifier("air.center"));
                        argument.metadata.push_back(qualifier("air.perspective"));
                    }
                } else {
                    argument.metadata.push_back(qualifier("air.flat"));
                }
            }
        }
    } else {
        fail(leaf.decl, "kernels take no [[stage_in]] here");
        return false;
    }
    argument.metadata.push_back(text("air.arg_type_name"));
    argument.metadata.push_back(text(type));
    argument.metadata.push_back(text("air.arg_name"));
    argument.metadata.push_back(text(name));
    arguments.push_back(std::move(argument));
    return true;
}

bool EntryBuilder::addOutput(int kind, const Leaf& leaf, std::vector<llvm::Metadata*>& outputs, bool& position)
{
    const std::vector<Annotation> notes = annotations(leaf.decl);
    const std::string name = leaf.decl->getNameAsString();
    clang::QualType type = leaf.type;
    std::vector<llvm::Metadata*> metadata;
    if (kind == 1) {
        if (findAnnotation(notes, "position")) {
            metadata.push_back(text("air.position"));
            position = true;
        } else if (findAnnotation(notes, "point_size")) {
            metadata.push_back(text("air.point_size"));
        } else if (findAnnotation(notes, "clip_distance")) {
            metadata.push_back(text("air.clip_distance"));
            const clang::ArrayType* array = m_context.getAsArrayType(type);
            if (const auto* constant = llvm::dyn_cast_or_null<clang::ConstantArrayType>(array)) {
                metadata.push_back(text("air.clip_distance_array_size"));
                metadata.push_back(integer(constant->getSize().getZExtValue()));
                type = constant->getElementType();
            }
        } else if (findAnnotation(notes, "render_target_array_index")) {
            metadata.push_back(text("air.render_target_array_index"));
        } else if (findAnnotation(notes, "viewport_array_index")) {
            metadata.push_back(text("air.viewport_array_index"));
        } else {
            metadata.push_back(text("air.vertex_output"));
            metadata.push_back(text(locationName(leaf.decl)));
        }
    } else if (const Annotation* color = findAnnotation(notes, "color")) {
        const Annotation* index = findAnnotation(notes, "index");
        metadata.push_back(text("air.render_target"));
        metadata.push_back(integer(annotationInteger(*color, 0)));
        metadata.push_back(integer(index ? annotationInteger(*index, 0) : 0));
    } else if (const Annotation* depth = findAnnotation(notes, "depth")) {
        const std::string qualifier = depth->arguments.empty() ? std::string("any") : depth->arguments[0];
        metadata.push_back(text("air.depth"));
        metadata.push_back(text("air.depth_qualifier"));
        metadata.push_back(text("air." + qualifier));
    } else if (findAnnotation(notes, "sample_mask")) {
        metadata.push_back(text("air.sample_mask"));
    } else if (findAnnotation(notes, "stencil")) {
        metadata.push_back(text("air.stencil"));
    } else {
        fail(leaf.decl, "the fragment output '" + name + "' needs [[color(n)]], [[depth]], [[sample_mask]] or [[stencil]]");
        return false;
    }
    if (!isScalar(type)) {
        fail(leaf.decl, "'" + name + "' has a type an output cannot have");
        return false;
    }
    // An output that a function constant turns off: [[function_constant(name)]] on the member.
    if (const Annotation* condition = findAnnotation(notes, "function_constant_condition"); condition && !condition->arguments.empty()) {
        clang::IdentifierInfo& identifier = m_context.Idents.get(condition->arguments[0]);
        llvm::Value* constant = nullptr;
        for (clang::NamedDecl* found : m_context.getTranslationUnitDecl()->lookup(&identifier)) {
            auto* variable = llvm::dyn_cast<clang::VarDecl>(found);
            if (!variable)
                continue;
            llvm::Value* global = m_code.GetAddrOfGlobal(clang::GlobalDecl(variable), false)->stripPointerCasts();
            if (llvm::isa<llvm::GlobalVariable>(global))
                constant = global;
            break;
        }
        if (!constant) {
            fail(leaf.decl, "the function constant of the output '" + name + "' is not a boolean constant");
            return false;
        }
        metadata.push_back(text("air.function_constant_condition"));
        metadata.push_back(llvm::ValueAsMetadata::get(constant));
    }
    metadata.push_back(text("air.arg_type_name"));
    metadata.push_back(text(typeName(type)));
    metadata.push_back(text("air.arg_name"));
    metadata.push_back(text(name));
    outputs.push_back(llvm::MDTuple::get(m_llvm, metadata));
    return true;
}

bool EntryBuilder::addParameter(int kind, const clang::ParmVarDecl* parameter, std::vector<Argument>& arguments, Parameter& result,
                                std::map<std::string, std::set<long>>& used, std::vector<Unbound>& unbound)
{
    const std::vector<Annotation> notes = annotations(parameter);
    const clang::QualType type = parameter->getType();
    const std::string name = parameter->getNameAsString();
    result.type = type;
    result.first = arguments.size();

    if (findAnnotation(notes, "stage_in")) {
        const clang::RecordDecl* record = type->getAsRecordDecl();
        if (!record) {
            fail(parameter, "a [[stage_in]] argument must be a struct");
            return false;
        }
        result.kind = Parameter::Members;
        std::vector<Leaf> leaves;
        collectLeaves(record, 0, leaves);
        for (const Leaf& leaf : leaves) {
            const std::vector<Annotation> member = annotations(leaf.decl);
            // What only the rasterizer reads is not an input of a fragment function.
            if (kind == 2 && (findAnnotation(member, "point_size") || findAnnotation(member, "clip_distance")))
                continue;
            if (!addInput(kind, leaf, arguments))
                return false;
            result.leaves.push_back(leaf);
        }
        return true;
    }

    const auto add = [&](Argument argument, const std::string& typeName) {
        argument.metadata.push_back(text("air.arg_type_name"));
        argument.metadata.push_back(text(typeName));
        argument.metadata.push_back(text("air.arg_name"));
        argument.metadata.push_back(text(name));
        arguments.push_back(std::move(argument));
    };
    const auto location = [&](const char* attribute, Argument& argument) {
        argument.metadata.push_back(text("air.location_index"));
        if (const Annotation* binding = findAnnotation(notes, attribute)) {
            used[attribute].insert(annotationInteger(*binding, 0));
            argument.metadata.push_back(integer(annotationInteger(*binding, 0)));
        } else {
            unbound.emplace_back(attribute, arguments.size());
            argument.metadata.push_back(integer(0));
        }
        argument.metadata.push_back(integer(1));
    };

    // An array of textures or samplers is one argument for each element.
    if (const clang::CXXRecordDecl* array = metalTemplate(type, "array")) {
        const clang::TemplateArgumentList& templateArguments = llvm::cast<clang::ClassTemplateSpecializationDecl>(array)->getTemplateArgs();
        const clang::QualType element = templateArguments[0].getAsType();
        const long long count = templateArguments[1].getAsIntegral().getExtValue();
        const std::optional<Texture> texture = textureType(element);
        if ((texture || isSampler(element)) && count > 0) {
            const char* attribute = texture ? "texture" : "sampler";
            const Annotation* binding = findAnnotation(notes, attribute);
            if (!binding) {
                fail(parameter, "the array '" + name + "' needs [[" + attribute + "(n)]]");
                return false;
            }
            result.kind = Parameter::Members;
            const int64_t size = m_context.getTypeSizeInChars(element).getQuantity();
            const std::string elementName = typeName(element);
            int64_t offset = 0;
            for (long long i = 0; i < count; i++) {
                const long long index = annotationInteger(*binding, 0) + i;
                used[attribute].insert(index);
                Argument argument;
                argument.type = llvm::PointerType::get(m_llvm, 2 - texture.has_value());
                argument.nonnull = true;
                argument.metadata.push_back(text(texture ? "air.texture" : "air.sampler"));
                argument.metadata.push_back(text("air.location_index"));
                argument.metadata.push_back(llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_llvm), index)));
                argument.metadata.push_back(llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_llvm), 1)));
                if (texture)
                    argument.metadata.push_back(text(std::string("air.") + accessName(texture->access)));
                argument.metadata.push_back(text("air.arg_type_name"));
                argument.metadata.push_back(text(elementName));
                argument.metadata.push_back(text("air.arg_name"));
                argument.metadata.push_back(text(name + "[" + std::to_string(i) + "]"));
                arguments.push_back(std::move(argument));
                result.leaves.push_back({nullptr, element, offset});
                offset += size;
            }
            return true;
        }
    }

    if (const std::optional<Texture> texture = textureType(type)) {
        result.kind = Parameter::Memory;
        Argument argument;
        argument.type = llvm::PointerType::get(m_llvm, 1);
        argument.nonnull = true;
        argument.metadata.push_back(text("air.texture"));
        location("texture", argument);
        argument.metadata.push_back(text(std::string("air.") + accessName(texture->access)));
        add(std::move(argument), typeName(type));
        return true;
    }
    if (isSampler(type)) {
        result.kind = Parameter::Memory;
        Argument argument;
        argument.type = llvm::PointerType::get(m_llvm, 2);
        argument.nonnull = true;
        argument.metadata.push_back(text("air.sampler"));
        location("sampler", argument);
        add(std::move(argument), std::string("sampler"));
        return true;
    }

    if (type->isPointerType() || type->isReferenceType()) {
        const clang::QualType pointee = type->getPointeeType();
        const clang::LangAS space = pointee.getAddressSpace();
        unsigned airSpace;
        if (space == clang::LangAS::sycl_global)
            airSpace = 1;
        else if (space == clang::LangAS::sycl_global_device)
            airSpace = 2;
        else if (space == clang::LangAS::sycl_local)
            airSpace = 3;
        else {
            fail(parameter, "the buffer argument '" + name + "' must point to device, constant or threadgroup memory");
            return false;
        }
        result.kind = Parameter::Direct;
        Argument argument;
        argument.type = llvm::PointerType::get(m_llvm, airSpace);
        const clang::QualType object = pointee.getUnqualifiedType();
        int64_t size = 0;
        int64_t alignment = 0;
        bool sized = false;
        if (!object->isVoidType() && !object->isIncompleteType()) {
            size = m_context.getTypeSizeInChars(object).getQuantity();
            alignment = m_context.getTypeAlignInChars(object).getQuantity();
            sized = true;
        }
        if (space == clang::LangAS::sycl_local) {
            argument.metadata.push_back(text("air.threadgroup"));
            location("threadgroup", argument);
        } else {
            argument.metadata.push_back(text("air.buffer"));
            if (type->isReferenceType() && sized) {
                argument.metadata.push_back(text("air.buffer_size"));
                argument.metadata.push_back(integer(size));
            }
            location("buffer", argument);
            argument.metadata.push_back(llvm::MDString::get(
                m_llvm, space == clang::LangAS::sycl_global_device || pointee.isConstQualified() ? "air.read" : "air.read_write"));
            argument.metadata.push_back(text("air.address_space"));
            argument.metadata.push_back(integer(airSpace));
            const clang::RecordDecl* record = object->getAsRecordDecl();
            if (record && sized && !metalTemplate(object) && !record->getName().starts_with("__msl_")) {
                argument.metadata.push_back(text("air.struct_type_info"));
                if (isArgumentBuffer(record)) {
                    // A structure of resources: an argument buffer, which AIR describes member by member.
                    argument.metadata[0] = text("air.indirect_buffer");
                    argument.metadata.push_back(argumentBufferInfo(m_context, m_llvm, record));
                } else {
                    std::vector<llvm::Metadata*> fields;
                    const clang::ASTRecordLayout& layout = m_context.getASTRecordLayout(record);
                    for (const clang::FieldDecl* field : record->fields()) {
                        clang::QualType member = field->getType();
                        uint64_t count = 0;
                        if (const auto* constant = llvm::dyn_cast_or_null<clang::ConstantArrayType>(m_context.getAsArrayType(member))) {
                            count = constant->getSize().getZExtValue();
                            member = constant->getElementType();
                        }
                        fields.push_back(llvm::ConstantAsMetadata::get(
                            llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_llvm), layout.getFieldOffset(field->getFieldIndex()) / 8)));
                        fields.push_back(llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(
                            llvm::Type::getInt32Ty(m_llvm), m_context.getTypeSizeInChars(field->getType()).getQuantity())));
                        fields.push_back(llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_llvm), count)));
                        fields.push_back(text(typeName(member)));
                        fields.push_back(text(field->getName()));
                    }
                    argument.metadata.push_back(llvm::MDTuple::get(m_llvm, fields));
                }
            }
        }
        argument.metadata.push_back(text("air.arg_type_size"));
        argument.metadata.push_back(integer(size));
        argument.metadata.push_back(text("air.arg_type_align_size"));
        argument.metadata.push_back(integer(alignment));
        add(std::move(argument), typeName(object));
        return true;
    }

    if (!isScalar(type)) {
        fail(parameter, "the argument '" + name + "' needs [[stage_in]] or an attribute that says what it is");
        return false;
    }
    result.kind = Parameter::Direct;
    Argument argument;
    argument.type = memoryType(type);
    bool known = false;
    if (kind == 1) {
        for (const Builtin& input : vertexInputs) {
            if (findAnnotation(notes, input.name)) {
                argument.metadata.push_back(text(input.air));
                known = true;
                break;
            }
        }
    } else if (kind == 2) {
        for (const Builtin& input : fragmentInputs) {
            if (findAnnotation(notes, input.name)) {
                argument.metadata.push_back(text(input.air));
                known = true;
                break;
            }
        }
        if (!known && findAnnotation(notes, "position")) {
            argument.metadata = {
                text("air.position"),
                text("air.center"),
                text("air.no_perspective"),
            };
            known = true;
        } else if (!known && findAnnotation(notes, "color")) {
            // The color of the attachment, read back: framebuffer fetch.
            argument.metadata = {
                text("air.color"),
                integer(annotationInteger(*findAnnotation(notes, "color"), 0)),
            };
            known = true;
        }
    } else {
        for (const Builtin& input : kernelInputs) {
            if (findAnnotation(notes, input.name)) {
                argument.metadata.push_back(text(input.air));
                known = true;
                break;
            }
        }
    }
    if (!known) {
        fail(parameter, "the argument '" + name + "' needs an attribute that says what it is");
        return false;
    }
    add(std::move(argument), typeName(type));
    return true;
}

bool EntryBuilder::buildEntryPoint(const clang::FunctionDecl* function, int kind, EntryPoint& entry)
{
    auto* original = llvm::dyn_cast_or_null<llvm::Function>(m_code.GetAddrOfGlobal(clang::GlobalDecl(function), false)->stripPointerCasts());
    if (!original || original->isDeclaration()) {
        fail(function, "the entry point '" + function->getNameAsString() + "' has no body");
        return false;
    }
    const std::vector<Annotation> notes = annotations(function);
    std::string name = function->getNameAsString();
    if (const Annotation* host = findAnnotation(notes, "host_name"); host && !host->arguments.empty())
        name = host->arguments[0];

    std::map<std::string, std::set<long>> used;
    std::vector<Argument> arguments;
    std::vector<Unbound> unbound;
    std::vector<Parameter> parameters;
    for (const clang::ParmVarDecl* parameter : function->parameters()) {
        Parameter result;
        if (!addParameter(kind, parameter, arguments, result, used, unbound))
            return false;
        parameters.push_back(std::move(result));
    }
    for (const Unbound& item : unbound) {
        std::set<long>& taken = used[item.attribute];
        long index = 0;
        while (taken.count(index))
            index++;
        taken.insert(index);
        std::vector<llvm::Metadata*>& metadata = arguments[item.argument].metadata;
        for (size_t i = 1; i < metadata.size(); i++) {
            const auto* text = llvm::dyn_cast<llvm::MDString>(metadata[i - 1]);
            if (text && text->getString() == "air.location_index") {
                metadata[i] = integer(index);
                break;
            }
        }
    }

    const clang::QualType returned = function->getReturnType();
    std::vector<Leaf> results;
    std::vector<llvm::Metadata*> outputs;
    llvm::Type* returnType = llvm::Type::getVoidTy(m_llvm);
    bool structured = false;
    if (!returned->isVoidType()) {
        if (kind == 3) {
            fail(function, "a kernel returns void");
            return false;
        }
        if (isScalar(returned)) {
            returnType = memoryType(returned);
            if (kind == 1) {
                outputs.push_back(llvm::MDTuple::get(m_llvm, {
                    text("air.position"),
                    text("air.arg_type_name"),
                    text(typeName(returned)),
                }));
            } else {
                outputs.push_back(llvm::MDTuple::get(m_llvm, {
                    text("air.render_target"),
                    integer(0),
                    integer(0),
                    text("air.arg_type_name"),
                    text(typeName(returned)),
                }));
            }
        } else if (const clang::RecordDecl* record = returned->getAsRecordDecl()) {
            collectLeaves(record, 0, results);
            bool position = false;
            std::vector<llvm::Type*> members;
            for (const Leaf& leaf : results) {
                if (!addOutput(kind, leaf, outputs, position))
                    return false;
                if (clang::CodeGen::convertTypeForMemory(m_code.CGM(), leaf.type)->isIntegerTy(8) && leaf.type->isBooleanType())
                    members.push_back(llvm::Type::getInt1Ty(m_llvm));
                else
                    members.push_back(clang::CodeGen::convertTypeForMemory(m_code.CGM(), leaf.type));
            }
            if (kind == 1 && !position) {
                fail(function, "the vertex function '" + function->getNameAsString() + "' returns no [[position]]");
                return false;
            }
            returnType = llvm::StructType::get(m_llvm, members, true);
            structured = true;
        } else {
            fail(function, "the entry point returns a type it cannot return");
            return false;
        }
    }

    std::vector<llvm::Type*> types;
    for (const Argument& argument : arguments)
        types.push_back(argument.type);
    llvm::FunctionType* functionType = llvm::FunctionType::get(returnType, types, false);
    original->setName("__msl.entry." + name);
    if (m_module.getFunction(name)) {
        fail(function, "the entry point name '" + name + "' is taken");
        return false;
    }
    llvm::Function* wrapper = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage, name, m_module);
    wrapper->addFnAttr(llvm::Attribute::NoUnwind);
    for (size_t i = 0; i < arguments.size(); i++) {
        if (arguments[i].nonnull)
            wrapper->addParamAttr(i, llvm::Attribute::NonNull);
    }
    llvm::IRBuilder<> builder(llvm::BasicBlock::Create(m_llvm, "entry", wrapper));
    const llvm::DataLayout& layout = m_module.getDataLayout();
    const unsigned allocaSpace = layout.getAllocaAddrSpace();

    std::vector<llvm::Value*> values;
    llvm::Value* result = nullptr;
    unsigned first = 0;
    if (original->arg_size() && original->hasParamAttribute(0, llvm::Attribute::StructRet)) {
        result = builder.CreateAlloca(original->getParamStructRetType(0), allocaSpace);
        llvm::Value* pointer = result;
        if (original->getArg(0)->getType() != pointer->getType())
            pointer = builder.CreateAddrSpaceCast(pointer, original->getArg(0)->getType());
        values.push_back(pointer);
        first = 1;
    }
    if (original->arg_size() != first + parameters.size()) {
        fail(function, "the entry point's arguments are passed in a way the front end does not know");
        return false;
    }
    for (size_t i = 0; i < parameters.size(); i++) {
        const Parameter& parameter = parameters[i];
        llvm::Argument* target = original->getArg(first + i);
        llvm::Value* value = wrapper->getArg(parameter.first);
        if (parameter.kind == Parameter::Direct) {
            llvm::Type* wanted = target->getType();
            if (wanted != value->getType()) {
                if (wanted->isPointerTy()) {
                    if (value->getType()->isPointerTy())
                        value = builder.CreateAddrSpaceCast(value, wanted);
                } else if (wanted->isIntegerTy() && value->getType()->isIntegerTy()) {
                    value = builder.CreateZExtOrTrunc(value, wanted);
                }
            }
            values.push_back(value);
        } else if (parameter.kind == Parameter::Members || parameter.kind == Parameter::Memory) {
            llvm::Type* memory = clang::CodeGen::convertTypeForMemory(m_code.CGM(), parameter.type);
            llvm::AllocaInst* slot = builder.CreateAlloca(memory, allocaSpace);
            builder.CreateMemSet(slot, builder.getInt8(0), layout.getTypeAllocSize(memory), llvm::MaybeAlign());
            if (parameter.kind == Parameter::Memory) {
                builder.CreateStore(value, slot);
            } else {
                for (size_t j = 0; j < parameter.leaves.size(); j++) {
                    const Leaf& leaf = parameter.leaves[j];
                    llvm::Value* address = builder.CreateConstInBoundsGEP1_64(builder.getInt8Ty(), slot, leaf.offset);
                    llvm::Type* member = clang::CodeGen::convertTypeForMemory(m_code.CGM(), leaf.type);
                    llvm::Value* field = wrapper->getArg(parameter.first + j);
                    if (field->getType()->isIntegerTy(1) && member->isIntegerTy(8))
                        field = builder.CreateZExt(field, member);
                    builder.CreateStore(field, address);
                }
            }
            if (!target->hasByValAttr() && !target->getType()->isPointerTy()) {
                values.push_back(builder.CreateLoad(target->getType(), slot));
            } else {
                llvm::Value* pointer = slot;
                if (slot->getType() != target->getType())
                    pointer = builder.CreateAddrSpaceCast(slot, target->getType());
                values.push_back(pointer);
            }
        }
    }
    llvm::CallInst* call = builder.CreateCall(original->getFunctionType(), original, values);
    call->setCallingConv(original->getCallingConv());
    if (returnType->isVoidTy()) {
        builder.CreateRetVoid();
    } else if (structured) {
        if (!result) {
            result = builder.CreateAlloca(call->getType(), allocaSpace);
            builder.CreateStore(call, result);
        }
        llvm::Value* aggregate = llvm::PoisonValue::get(returnType);
        for (size_t i = 0; i < results.size(); i++) {
            const Leaf& leaf = results[i];
            llvm::Value* address = builder.CreateConstInBoundsGEP1_64(builder.getInt8Ty(), result, leaf.offset);
            llvm::Type* member = returnType->getStructElementType(i);
            llvm::Type* memory = clang::CodeGen::convertTypeForMemory(m_code.CGM(), leaf.type);
            llvm::Value* value = builder.CreateLoad(memory, address);
            if (member->isIntegerTy(1) && memory->isIntegerTy(8))
                value = builder.CreateICmpNE(value, llvm::ConstantInt::get(memory, 0));
            aggregate = builder.CreateInsertValue(aggregate, value, i);
        }
        builder.CreateRet(aggregate);
    } else {
        llvm::Value* value = call;
        if (returnType != call->getType() && call->getType()->isIntegerTy() && returnType->isIntegerTy())
            value = builder.CreateZExtOrTrunc(call, returnType);
        builder.CreateRet(value);
    }

    entry.name = name;
    entry.kind = kind;
    entry.function = wrapper;
    entry.outputs = std::move(outputs);
    for (Argument& argument : arguments)
        entry.arguments.push_back(std::move(argument.metadata));
    if (findAnnotation(notes, "early_fragment_tests"))
        entry.attributes.push_back(text("air.early_fragment_tests"));
    if (const Annotation* limit = findAnnotation(notes, "max_total_threads_per_threadgroup")) {
        entry.attributes.push_back(llvm::MDTuple::get(m_llvm, {
            text("air.max_total_threads_per_threadgroup"),
            integer(annotationInteger(*limit, 0)),
        }));
    }
    return !m_failed;
}

}

bool buildEntryPoints(clang::ASTContext& context, clang::CodeGenerator& code, clang::DiagnosticsEngine& diagnostics,
                      std::vector<EntryPoint>& entries)
{
    EntryBuilder builder(context, code, diagnostics);
    bool ok = true;
    std::function<void(const clang::DeclContext*)> visit = [&](const clang::DeclContext* scope) {
        for (const clang::Decl* decl : scope->decls()) {
            if (const auto* space = llvm::dyn_cast<clang::NamespaceDecl>(decl)) {
                visit(space);
                continue;
            }
            if (const auto* linkage = llvm::dyn_cast<clang::LinkageSpecDecl>(decl)) {
                visit(linkage);
                continue;
            }
            const auto* function = llvm::dyn_cast<clang::FunctionDecl>(decl);
            if (!function)
                continue;
            const std::vector<Annotation> notes = annotations(function);
            if (findAnnotation(notes, "visible") || findAnnotation(notes, "stitchable")) {
                if (!function->doesThisDeclarationHaveABody()) {
                    // A visible function that another library defines: it is linked by its plain name.
                    const clang::FunctionDecl* definition = nullptr;
                    if (function->isDefined(definition) && definition)
                        continue;
                    llvm::Function* declared = builder.module().getFunction(builder.code().GetMangledName(clang::GlobalDecl(function)));
                    if (!declared || !declared->isDeclaration())
                        continue;
                    const std::string name = function->getNameAsString();
                    llvm::Function* other = builder.module().getFunction(name);
                    if (other && other != declared) {
                        declared->replaceAllUsesWith(other);
                        declared->eraseFromParent();
                    } else {
                        declared->setName(name);
                    }
                    continue;
                }
                EntryPoint entry;
                auto* defined = llvm::dyn_cast_or_null<llvm::Function>(
                    builder.code().GetAddrOfGlobal(clang::GlobalDecl(function), false)->stripPointerCasts());
                if (!defined || defined->isDeclaration()) {
                    builder.fail(function, "the visible function '" + function->getNameAsString() + "' has no body");
                    ok = false;
                    continue;
                }
                const std::string name = function->getNameAsString();
                if (llvm::Function* other = builder.module().getFunction(name); other && other != defined) {
                    if (!other->isDeclaration()) {
                        builder.fail(function, "the visible function name '" + name + "' is taken");
                        ok = false;
                        continue;
                    }
                    other->replaceAllUsesWith(defined);
                    other->eraseFromParent();
                }
                defined->setName(name);
                defined->setLinkage(llvm::GlobalValue::ExternalLinkage);
                entry.name = name;
                entry.kind = 5;
                entry.function = defined;
                llvm::LLVMContext& llvm = builder.llvm();
                if (!function->getReturnType()->isVoidType()) {
                    entry.outputs.push_back(llvm::MDTuple::get(llvm, {
                        builder.text("air.visible_output"),
                        builder.text("air.arg_type_name"),
                        builder.text(typeName(function->getReturnType())),
                    }));
                }
                for (const clang::ParmVarDecl* parameter : function->parameters()) {
                    entry.arguments.push_back({
                        builder.text("air.visible_input"),
                        builder.text("air.arg_type_name"),
                        builder.text(typeName(parameter->getType())),
                        builder.text("air.arg_name"),
                        builder.text(parameter->getName()),
                    });
                }
                entries.push_back(std::move(entry));
                continue;
            }
            if (!function->doesThisDeclarationHaveABody())
                continue;
            int kind;
            if (findAnnotation(notes, "vertex"))
                kind = 1;
            else if (findAnnotation(notes, "fragment"))
                kind = 2;
            else if (findAnnotation(notes, "kernel"))
                kind = 3;
            else
                continue;
            EntryPoint entry;
            if (builder.buildEntryPoint(function, kind, entry))
                entries.push_back(std::move(entry));
            else
                ok = false;
        }
    };
    visit(context.getTranslationUnitDecl());
    builder.listFunctionConstants(context.getTranslationUnitDecl());
    return ok && !diagnostics.hasErrorOccurred();
}

}
