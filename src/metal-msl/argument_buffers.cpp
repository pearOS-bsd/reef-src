// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Argument buffers: structures of resources that a buffer argument points to, and AIR's description of them.

#include "frontend.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/RecordLayout.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Metadata.h>

namespace cutefish::reef::metal::msl {

// A texture, a sampler or a pointer, or an array of them: what an argument buffer holds as a resource.
static bool isResource(clang::QualType type, clang::QualType* element, int64_t* count)
{
    int64_t number = 1;
    if (const clang::CXXRecordDecl* array = metalTemplate(type, "array")) {
        const clang::TemplateArgumentList& arguments = llvm::cast<clang::ClassTemplateSpecializationDecl>(array)->getTemplateArgs();
        type = arguments[0].getAsType();
        number = arguments[1].getAsIntegral().getExtValue();
    } else if (const auto* constant = llvm::dyn_cast<clang::ConstantArrayType>(type.getCanonicalType())) {
        type = constant->getElementType();
        number = constant->getSize().getZExtValue();
    }
    if (element)
        *element = type;
    if (count)
        *count = number;
    return textureType(type) || isSampler(type) || type->isPointerType();
}

bool isArgumentBuffer(const clang::RecordDecl* record)
{
    for (const clang::FieldDecl* field : record->fields()) {
        const clang::QualType type = field->getType().getCanonicalType();
        if (isResource(type, nullptr, nullptr))
            return true;
        const clang::RecordDecl* inner = type->getAsRecordDecl();
        if (inner && !metalTemplate(type) && isArgumentBuffer(inner))
            return true;
    }
    return false;
}

static llvm::Metadata* integer(llvm::LLVMContext& llvm, uint64_t value)
{
    return llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm), value));
}

// Seven entries for each member, structures inside taken apart: offset, size, 0, type, name, "air.indirect_argument" and what
// the member is, as an argument of its own would say it.
static void describeMembers(clang::ASTContext& context, llvm::LLVMContext& llvm, const clang::RecordDecl* record, int64_t offset,
                            int64_t& next, std::vector<llvm::Metadata*>& members)
{
    const clang::ASTRecordLayout& layout = context.getASTRecordLayout(record);
    for (const clang::FieldDecl* field : record->fields()) {
        const int64_t at = layout.getFieldOffset(field->getFieldIndex()) / 8 + offset;
        const clang::QualType type = field->getType().getCanonicalType();
        const std::vector<Annotation> notes = annotations(field);
        const clang::RecordDecl* inner = type->getAsRecordDecl();
        if (inner && !metalTemplate(type) && !textureType(type) && !isSampler(type)) {
            if (const Annotation* id = findAnnotation(notes, "id"))
                next = annotationInteger(*id, 0);
            describeMembers(context, llvm, inner, at, next, members);
            continue;
        }
        int64_t count = 1;
        clang::QualType element;
        const bool resource = isResource(type, &element, &count);
        if (!resource)
            count = 1;
        const int64_t id = findAnnotation(notes, "id") ? annotationInteger(*findAnnotation(notes, "id"), 0) : next;
        next = count + id;
        const std::string name = field->getName().str();
        std::string typeText;
        if (resource) {
            typeText = typeName(element);
            if (count > 1)
                typeText = "array<" + typeText + ", " + std::to_string(count) + ">";
        } else {
            typeText = typeName(type);
        }

        std::vector<llvm::Metadata*> info = {integer(llvm, members.size() / 7)};
        std::optional<Texture> texture;
        if (!resource) {
            info.push_back(llvm::MDString::get(llvm, "air.indirect_constant"));
        } else if ((texture = textureType(element))) {
            info.push_back(llvm::MDString::get(llvm, "air.texture"));
        } else if (!isSampler(element)) {
            info.push_back(llvm::MDString::get(llvm, "air.buffer"));
        } else {
            info.push_back(llvm::MDString::get(llvm, "air.sampler"));
        }
        info.push_back(llvm::MDString::get(llvm, "air.location_index"));
        info.push_back(integer(llvm, id));
        info.push_back(integer(llvm, count));
        if (texture) {
            info.push_back(llvm::MDString::get(llvm, std::string("air.") + accessName(texture->access)));
        } else if (resource && element->isPointerType()) {
            const clang::QualType pointee = element->getPointeeType();
            const clang::LangAS space = pointee.getAddressSpace();
            info.push_back(llvm::MDString::get(
                llvm, space == clang::LangAS::sycl_global_device || pointee.isConstQualified() ? "air.read" : "air.read_write"));
            info.push_back(llvm::MDString::get(llvm, "air.address_space"));
            info.push_back(integer(llvm, (space == clang::LangAS::sycl_global_device) + 1));
            const clang::QualType object = pointee.getUnqualifiedType();
            const bool sized = !object->isVoidType() && !object->isIncompleteType();
            info.push_back(llvm::MDString::get(llvm, "air.arg_type_size"));
            info.push_back(integer(llvm, sized ? context.getTypeSizeInChars(object).getQuantity() : 0));
            info.push_back(llvm::MDString::get(llvm, "air.arg_type_align_size"));
            info.push_back(integer(llvm, sized ? context.getTypeAlignInChars(object).getQuantity() : 0));
            typeText = typeName(object);
        }
        info.push_back(llvm::MDString::get(llvm, "air.arg_type_name"));
        info.push_back(llvm::MDString::get(llvm, typeText));
        info.push_back(llvm::MDString::get(llvm, "air.arg_name"));
        info.push_back(llvm::MDString::get(llvm, name));

        members.push_back(integer(llvm, at));
        members.push_back(integer(llvm, context.getTypeSizeInChars(field->getType()).getQuantity()));
        members.push_back(integer(llvm, 0));
        members.push_back(llvm::MDString::get(llvm, typeText));
        members.push_back(llvm::MDString::get(llvm, name));
        members.push_back(llvm::MDString::get(llvm, "air.indirect_argument"));
        members.push_back(llvm::MDTuple::get(llvm, info));
    }
}

llvm::MDNode* argumentBufferInfo(clang::ASTContext& context, llvm::LLVMContext& llvm, const clang::RecordDecl* record)
{
    int64_t next = 0;
    std::vector<llvm::Metadata*> members;
    describeMembers(context, llvm, record, 0, next, members);
    return llvm::MDTuple::get(llvm, members);
}

}
