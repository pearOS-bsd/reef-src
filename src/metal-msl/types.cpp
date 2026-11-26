// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the attributes left on declarations, and the names Metal gives types.

#include "frontend.h"

#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/Type.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/SmallVector.h>

namespace cutefish::reef::metal::msl {

static const char* const textureNames[] = {
    "texture1d", "texture1d_array", "texture2d", "texture2d_array", "texture3d", "texturecube", "texturecube_array", "texture2d_ms",
    "texture2d_ms_array", "texture_buffer", "depth2d", "depth2d_array", "depthcube", "depthcube_array", "depth2d_ms", "depth2d_ms_array",
};

std::vector<Annotation> annotations(const clang::Decl* decl)
{
    std::vector<Annotation> result;
    for (const clang::AnnotateAttr* attribute : decl->specific_attrs<clang::AnnotateAttr>()) {
        llvm::StringRef text = attribute->getAnnotation();
        if (!text.consume_front("msl:"))
            continue;
        llvm::SmallVector<llvm::StringRef, 4> parts;
        text.split(parts, ':');
        Annotation annotation;
        annotation.name = parts[0].str();
        for (size_t i = 1; i < parts.size(); i++)
            annotation.arguments.push_back(parts[i].str());
        result.push_back(std::move(annotation));
    }
    return result;
}

const Annotation* findAnnotation(const std::vector<Annotation>& annotations, llvm::StringRef name)
{
    for (const Annotation& annotation : annotations) {
        if (annotation.name == name)
            return &annotation;
    }
    return nullptr;
}

long long annotationInteger(const Annotation& annotation, size_t index)
{
    if (index >= annotation.arguments.size())
        return 0;
    return std::stoll(annotation.arguments[index]);
}

const clang::CXXRecordDecl* metalTemplate(clang::QualType type, llvm::StringRef name)
{
    const clang::CXXRecordDecl* record = type->getAsCXXRecordDecl();
    if (!record || !llvm::isa<clang::ClassTemplateSpecializationDecl>(record))
        return nullptr;
    const auto* space = llvm::dyn_cast<clang::NamespaceDecl>(record->getDeclContext());
    if (!space || space->getName() != "metal")
        return nullptr;
    if (name.empty())
        return record;
    return record->getName() == name ? record : nullptr;
}

std::string scalarName(const clang::BuiltinType* type)
{
    switch (type->getKind()) {
    case clang::BuiltinType::Void:
        return "void";
    case clang::BuiltinType::Bool:
        return "bool";
    case clang::BuiltinType::Char_S:
    case clang::BuiltinType::SChar:
        return "char";
    case clang::BuiltinType::Char_U:
    case clang::BuiltinType::UChar:
        return "uchar";
    case clang::BuiltinType::Short:
        return "short";
    case clang::BuiltinType::UShort:
        return "ushort";
    case clang::BuiltinType::Int:
        return "int";
    case clang::BuiltinType::UInt:
        return "uint";
    case clang::BuiltinType::Long:
    case clang::BuiltinType::LongLong:
        return "long";
    case clang::BuiltinType::ULong:
    case clang::BuiltinType::ULongLong:
        return "ulong";
    case clang::BuiltinType::Half:
    case clang::BuiltinType::Float16:
        return "half";
    case clang::BuiltinType::Float:
        return "float";
    case clang::BuiltinType::Double:
        return "double";
    default:
        return "";
    }
}

bool isSampler(clang::QualType type)
{
    const clang::CXXRecordDecl* record = type->getAsCXXRecordDecl();
    if (!record || record->getName() != "sampler")
        return false;
    const auto* space = llvm::dyn_cast<clang::NamespaceDecl>(record->getDeclContext());
    return space && space->getName() == "metal";
}

const char* accessName(int access)
{
    switch (access) {
    case 2:
        return "write";
    case 3:
        return "read_write";
    case 1:
        return "read";
    default:
        return "sample";
    }
}

std::optional<Texture> textureType(clang::QualType type)
{
    const clang::CXXRecordDecl* record = metalTemplate(type);
    if (!record)
        return std::nullopt;
    const llvm::StringRef name = record->getName();
    bool known = false;
    for (const char* texture : textureNames)
        known = known || name == texture;
    if (!known)
        return std::nullopt;
    const clang::TemplateArgumentList& arguments = llvm::cast<clang::ClassTemplateSpecializationDecl>(record)->getTemplateArgs();
    Texture texture;
    texture.name = name.str();
    if (arguments.size() >= 1 && arguments[0].getKind() == clang::TemplateArgument::Type) {
        if (const clang::BuiltinType* scalar = arguments[0].getAsType().getCanonicalType()->getAs<clang::BuiltinType>())
            texture.scalar = scalarName(scalar);
    }
    if (arguments.size() >= 2 && arguments[1].getKind() == clang::TemplateArgument::Integral)
        texture.access = arguments[1].getAsIntegral().getExtValue();
    return texture;
}

std::string typeName(clang::QualType type)
{
    const clang::QualType canonical = type.getCanonicalType().getUnqualifiedType();
    if (const auto* scalar = canonical->getAs<clang::BuiltinType>())
        return scalarName(scalar);
    if (const auto* vector = canonical->getAs<clang::VectorType>()) {
        const clang::BuiltinType* scalar = vector->getElementType().getCanonicalType()->getAs<clang::BuiltinType>();
        return (scalar ? scalarName(scalar) : std::string()) + std::to_string(vector->getNumElements());
    }
    if (const auto* enumeration = canonical->getAs<clang::EnumType>())
        return typeName(enumeration->getDecl()->getIntegerType());
    if (const auto* array = llvm::dyn_cast<clang::ConstantArrayType>(canonical))
        return typeName(array->getElementType());
    if (const auto* pointer = canonical->getAs<clang::PointerType>())
        return typeName(pointer->getPointeeType()) + " *";
    if (const std::optional<Texture> texture = textureType(canonical))
        return texture->name + "<" + texture->scalar + ", " + accessName(texture->access) + ">";
    if (isSampler(canonical))
        return "sampler";
    if (const clang::CXXRecordDecl* matrix = metalTemplate(canonical, "matrix")) {
        const clang::TemplateArgumentList& arguments = llvm::cast<clang::ClassTemplateSpecializationDecl>(matrix)->getTemplateArgs();
        const clang::BuiltinType* scalar = arguments[0].getAsType().getCanonicalType()->getAs<clang::BuiltinType>();
        return (scalar ? scalarName(scalar) : std::string()) + std::to_string(arguments[1].getAsIntegral().getExtValue()) + "x"
            + std::to_string(arguments[2].getAsIntegral().getExtValue());
    }
    if (const clang::CXXRecordDecl* atomic = metalTemplate(canonical, "atomic")) {
        const clang::TemplateArgumentList& arguments = llvm::cast<clang::ClassTemplateSpecializationDecl>(atomic)->getTemplateArgs();
        return "atomic_" + typeName(arguments[0].getAsType());
    }
    const clang::CXXRecordDecl* record = canonical->getAsCXXRecordDecl();
    if (!record)
        return canonical.getAsString();
    if (const auto* specialization = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(record)) {
        if (specialization->getName() == "__msl_bool_vector")
            return "bool" + std::to_string(specialization->getTemplateArgs()[0].getAsIntegral().getExtValue());
        if (specialization->getName() == "__msl_packed3")
            return "packed_" + typeName(specialization->getTemplateArgs()[0].getAsType()) + "3";
    }
    return record->getNameAsString();
}

}
