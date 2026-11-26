// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The emitter: the preparation of a function, names, types and constants as GLSL, the instructions, the inputs.

#include "emitter.h"

#include <algorithm>
#include <bit>
#include <cctype>
#include <cmath>
#include <cstdio>

#include <llvm/Analysis/ConstantFolding.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/Scalar/ADCE.h>
#include <llvm/Transforms/Scalar/EarlyCSE.h>
#include <llvm/Transforms/Scalar/LoopUnrollPass.h>
#include <llvm/Transforms/Scalar/SCCP.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Scalar/SROA.h>
#include <llvm/Transforms/Scalar/StructurizeCFG.h>
#include <llvm/Transforms/Utils/FixIrreducible.h>
#include <llvm/Transforms/Utils/LCSSA.h>
#include <llvm/Transforms/Utils/LoopSimplify.h>
#include <llvm/Transforms/Utils/LoopUtils.h>
#include <llvm/Transforms/Utils/LowerSwitch.h>
#include <llvm/Transforms/Utils/UnifyLoopExits.h>

namespace cutefish::reef::metal {

static const char *const components[] = {"x", "y", "z", "w"};

// What the components of a Metal type are in GLSL: 0 float, 1 int, 2 uint (bools too).
static int componentKind(const std::string &typeName)
{
    int element = 0;
    int count = 0;
    if (!splitDataType(dataType(typeName), element, count))
        return 0;
    switch (element) {
    case 33: // uint
    case 41: // ushort
    case 49: // uchar
    case 53: // bool
    case 85: // ulong
        return 2;
    case 29: // int
    case 37: // short
    case 45: // char
    case 81: // long
        return 1;
    default:
        return 0;
    }
}

// Removes what a map knows of instructions; arguments and globals stay.
template<typename Map>
static void forgetInstructions(Map &map)
{
    llvm::SmallVector<const llvm::Value *, 64> instructions;
    for (const auto &entry : map) {
        if (llvm::isa<llvm::Instruction>(entry.first))
            instructions.push_back(entry.first);
    }
    for (const llvm::Value *value : instructions)
        map.erase(value);
}

// A floating-point literal that reads back as the same value; infinities and NaNs by their bits.
static std::string floatLiteral(double value, bool isDouble)
{
    char buffer[64];
    if (!std::isfinite(value)) {
        if (!isDouble) {
            snprintf(buffer, sizeof(buffer), "uintBitsToFloat(0x%xu)", std::bit_cast<uint32_t>(float(value)));
        } else {
            const uint64_t bits = std::bit_cast<uint64_t>(value);
            snprintf(buffer, sizeof(buffer), "packDouble2x32(uvec2(0x%xu, 0x%xu))", unsigned(bits), unsigned(bits >> 32));
        }
        return buffer;
    }
    snprintf(buffer, sizeof(buffer), isDouble ? "%.17g" : "%.9g", value);
    std::string text = buffer;
    if (text.find_first_of(".e") == std::string::npos)
        text += ".0";
    if (isDouble)
        text += "lf";
    if (text[0] == '-')
        text = "(" + text + ")";
    return text;
}

// Keeps a string for the structures of the reflection, which point into it.
static const char *keep(Shader &shader, std::string text)
{
    shader.strings.push_back(std::move(text));
    return shader.strings.back().c_str();
}

Emitter::Emitter(llvm::Module &module, const FunctionInfo &info, const Options &options, Shader &shader)
    : m_module(module)
    , m_info(info)
    , m_options(options)
    , m_shader(shader)
    , m_function(info.function)
    , m_layout(module.getDataLayout())
{
    m_prefix = info.stage == 1 ? "v" : info.stage == 2 ? "f" : "k";
}

void Emitter::reset()
{
    forgetInstructions(m_names);
    forgetInstructions(m_addresses);
    forgetInstructions(m_pointerRoots);
    m_values.clear();
}

void Emitter::expandConstantExpressions()
{
    bool changed;
    do {
        changed = false;
        for (llvm::BasicBlock &block : *m_function) {
            for (llvm::Instruction &instruction : block) {
                for (unsigned i = 0; i < instruction.getNumOperands(); ++i) {
                    auto *expression = llvm::dyn_cast<llvm::ConstantExpr>(instruction.getOperand(i));
                    if (!expression)
                        continue;
                    // What a phi takes from a block is computed at the end of that block.
                    llvm::Instruction *position = &instruction;
                    if (auto *phi = llvm::dyn_cast<llvm::PHINode>(&instruction))
                        position = phi->getIncomingBlock(i)->getTerminator();
                    llvm::Instruction *expanded = expression->getAsInstruction();
                    expanded->insertBefore(position);
                    instruction.setOperand(i, expanded);
                    changed = true;
                }
            }
        }
    } while (changed);
}

void Emitter::line(const std::string &text)
{
    for (int i = 0; i < m_indent; ++i)
        m_body << "    ";
    m_body << text << '\n';
}

void Emitter::require(const std::string &extension)
{
    m_extensions.insert(extension);
}

bool Emitter::isInteger(llvm::Type *type)
{
    return scalar(type)->isIntegerTy();
}

unsigned Emitter::width(llvm::Type *type)
{
    if (auto *vector = llvm::dyn_cast<llvm::FixedVectorType>(type))
        return vector->getNumElements();
    return 1;
}

llvm::Type *Emitter::scalar(llvm::Type *type)
{
    if (auto *vector = llvm::dyn_cast<llvm::FixedVectorType>(type))
        return vector->getElementType();
    return type;
}

unsigned Emitter::bits(llvm::Type *type)
{
    if (type->isIntegerTy())
        return type->getIntegerBitWidth();
    if (type->isPointerTy())
        return 32;
    return type->getPrimitiveSizeInBits().getFixedValue();
}

std::string Emitter::mask(unsigned bits)
{
    if (bits >= 64)
        return "0xffffffffffffffffUL";
    char buffer[32];
    if (bits <= 32)
        snprintf(buffer, sizeof(buffer), "0x%lxu", bits == 32 ? 0xffffffffUL : (1UL << bits) - 1);
    else
        snprintf(buffer, sizeof(buffer), "0x%lxUL", (1UL << bits) - 1);
    return buffer;
}

std::string Emitter::component(unsigned index)
{
    return components[index & 3];
}

std::string Emitter::identifier(const std::string &name)
{
    static const char digits[] = "0123456789abcdef";
    std::string text;
    for (const unsigned char c : name) {
        if (isalnum(c)) {
            text += char(c);
            continue;
        }
        text += '_';
        text += digits[c >> 4];
        text += digits[c & 15];
    }
    return text;
}

std::string Emitter::vectorPrefix(llvm::Type *element)
{
    if (element->isIntegerTy(1))
        return "bvec";
    if (element->isIntegerTy()) {
        if (element->getIntegerBitWidth() > 64)
            throw Unsupported("integers wider than 64 bits are not supported");
        if (element->getIntegerBitWidth() > 32) {
            require("GL_ARB_gpu_shader_int64");
            return "u64vec";
        }
        return "uvec";
    }
    if (element->isDoubleTy())
        return "dvec";
    if (element->isHalfTy() || element->isBFloatTy() || element->isFloatTy())
        return "vec";
    if (!element->isPointerTy())
        throw Unsupported("vectors of this element type are not supported");
    return "uvec";
}

// Integers are kept as uint with only their bits set; signed operations see them through this.
std::string Emitter::asSigned(const std::string &value, llvm::Type *type)
{
    const unsigned size = bits(scalar(type));
    const unsigned count = width(type);
    const std::string suffix = count > 1 ? std::to_string(count) : "";
    if (size == 1)
        return (count > 1 ? "-ivec" + suffix : std::string("-int")) + "(" + value + ")";
    if (size > 32) {
        require("GL_ARB_gpu_shader_int64");
        const std::string wide = (count > 1 ? "i64vec" + suffix : std::string("int64_t")) + "(" + value + ")";
        if (size == 64)
            return wide;
        const std::string shift = std::to_string(64 - size);
        return "((" + wide + " << " + shift + ") >> " + shift + ")";
    }
    const std::string narrow = (count > 1 ? "ivec" + suffix : std::string("int")) + "(" + value + ")";
    if (size == 32)
        return narrow;
    return "bitfieldExtract(" + narrow + ", 0, " + std::to_string(size) + ")";
}

std::string Emitter::masked(const std::string &value, llvm::Type *type)
{
    const unsigned size = bits(scalar(type));
    if (size == 1 || size == 32 || size == 64)
        return value;
    return "(" + value + " & " + mask(size) + ")";
}

std::string Emitter::element(const std::string &value, llvm::Type *type, unsigned index)
{
    if (width(type) == 1)
        return value;
    return value + "." + component(index);
}

std::string Emitter::integer(uint64_t value, llvm::Type *type)
{
    if (bits(scalar(type)) > 32) {
        require("GL_ARB_gpu_shader_int64");
        return std::to_string(value) + "UL";
    }
    return std::to_string(unsigned(value)) + "u";
}

std::string Emitter::scalarConstant(llvm::Constant *constant, llvm::Type *type)
{
    if (llvm::isa<llvm::UndefValue>(constant) || constant->isNullValue()) {
        if (type->isIntegerTy(1))
            return "false";
        if (type->isIntegerTy() || type->isPointerTy())
            return integer(0, type);
        if (type->isDoubleTy())
            return "0.0lf";
        return "0.0";
    }
    if (auto *number = llvm::dyn_cast<llvm::ConstantInt>(constant)) {
        if (type->isIntegerTy(1))
            return number->isZero() ? "false" : "true";
        return integer(number->getZExtValue(), type);
    }
    if (auto *real = llvm::dyn_cast<llvm::ConstantFP>(constant)) {
        if (type->isDoubleTy())
            return floatLiteral(real->getValueAPF().convertToDouble(), true);
        llvm::APFloat value = real->getValueAPF();
        bool lost = false;
        value.convert(llvm::APFloat::IEEEsingle(), llvm::APFloat::rmNearestTiesToEven, &lost);
        return floatLiteral(value.convertToFloat(), false);
    }
    if (llvm::isa<llvm::ConstantPointerNull>(constant))
        return "0u";
    if (!llvm::isa<llvm::GlobalValue>(constant))
        throw Unsupported("an unsupported constant");
    return offsetOf(addressOf(constant));
}

std::string Emitter::elementOf(llvm::Value *value, const std::string &text, unsigned index)
{
    llvm::Type *type = value->getType();
    auto *constant = llvm::dyn_cast<llvm::Constant>(value);
    if (constant && !llvm::isa<llvm::GlobalValue>(constant))
        return scalarConstant(constant->getAggregateElement(index), scalar(type));
    return element(text, type, index);
}

std::string Emitter::fromSigned(const std::string &value, llvm::Type *type)
{
    const unsigned size = bits(scalar(type));
    const unsigned count = width(type);
    if (size == 1)
        return count > 1 ? "notEqual(" + value + ", ivec" + std::to_string(count) + "(0))" : "(" + value + " != 0)";
    std::string converted;
    if (size > 32)
        converted = (count > 1 ? "u64vec" + std::to_string(count) : std::string("uint64_t")) + "(" + value + ")";
    else
        converted = (count > 1 ? "uvec" + std::to_string(count) : std::string("uint")) + "(" + value + ")";
    return masked(converted, type);
}

std::string Emitter::helper(const std::string &name, const std::string &text)
{
    if (m_helpers.emplace(name, text).second)
        m_helperOrder.push_back(name);
    return name;
}

void Emitter::requireHalf()
{
    // The rounding of a float to half, as the conversions of Metal do it.
    helper("reef_half",
        "uint reef_half_bits(float f)\n"
        "{\n"
        "    uint x = floatBitsToUint(f);\n"
        "    uint sign = (x >> 16) & 0x8000u;\n"
        "    uint e = (x >> 23) & 0xffu;\n"
        "    uint m = x & 0x7fffffu;\n"
        "    if (e == 0xffu)\n"
        "        return sign | 0x7c00u | (m != 0u ? 0x200u | (m >> 13) : 0u);\n"
        "    int exponent = int(e) - 112;\n"
        "    if (exponent >= 31)\n"
        "        return sign | 0x7c00u;\n"
        "    if (exponent <= 0) {\n"
        "        if (exponent < -10)\n"
        "            return sign;\n"
        "        m |= 0x800000u;\n"
        "        uint shift = uint(14 - exponent);\n"
        "        uint h = m >> shift;\n"
        "        uint rest = m & ((1u << shift) - 1u);\n"
        "        uint halfway = 1u << (shift - 1u);\n"
        "        if (rest > halfway || (rest == halfway && (h & 1u) != 0u))\n"
        "            h += 1u;\n"
        "        return sign | h;\n"
        "    }\n"
        "    uint h = (uint(exponent) << 10) | (m >> 13);\n"
        "    uint rest = m & 0x1fffu;\n"
        "    if (rest > 0x1000u || (rest == 0x1000u && (h & 1u) != 0u))\n"
        "        h += 1u;\n"
        "    return sign | h;\n"
        "}\n"
        "float reef_half(float x) { return unpackHalf2x16(reef_half_bits(x)).x; }\n"
        "vec2 reef_half(vec2 x) { return vec2(reef_half(x.x), reef_half(x.y)); }\n"
        "vec3 reef_half(vec3 x) { return vec3(reef_half(x.x), reef_half(x.y), reef_half(x.z)); }\n"
        "vec4 reef_half(vec4 x) { return vec4(reef_half(x.x), reef_half(x.y), reef_half(x.z), reef_half(x.w)); }\n");
}

std::string Emitter::structName(llvm::StructType *type)
{
    const auto known = m_structNames.find(type);
    if (known != m_structNames.end())
        return known->second;
    std::vector<std::string> members;
    for (unsigned i = 0; i < type->getNumElements(); ++i)
        members.push_back(typeName(type->getElementType(i)) + " m" + std::to_string(i) + ";");
    // GLSL has no empty structures.
    if (members.empty())
        members.push_back("uint m0;");
    const std::string name = "reef_s" + std::to_string(m_structNames.size());
    m_structNames.emplace(type, name);
    std::string definition = "struct " + name + " {";
    for (const std::string &member : members)
        definition += " " + member;
    definition += " };";
    m_structs.push_back(definition);
    return name;
}

std::string Emitter::typeName(llvm::Type *type)
{
    if (type->isVoidTy())
        return "void";
    if (type->isIntegerTy(1))
        return "bool";
    switch (type->getTypeID()) {
    case llvm::Type::HalfTyID:
    case llvm::Type::BFloatTyID:
    case llvm::Type::FloatTyID:
        return "float";
    case llvm::Type::DoubleTyID:
        return "double";
    case llvm::Type::IntegerTyID:
        if (type->getIntegerBitWidth() > 64)
            throw Unsupported("integers wider than 64 bits are not supported");
        if (type->getIntegerBitWidth() > 32) {
            require("GL_ARB_gpu_shader_int64");
            return "uint64_t";
        }
        return "uint";
    case llvm::Type::PointerTyID:
        return "uint";
    case llvm::Type::StructTyID:
        return structName(llvm::cast<llvm::StructType>(type));
    case llvm::Type::ArrayTyID:
        if (type->getArrayNumElements() == 0)
            throw Unsupported("zero-length arrays are not supported as values");
        return typeName(llvm::cast<llvm::ArrayType>(type)->getElementType()) + "[" + std::to_string(type->getArrayNumElements()) + "]";
    case llvm::Type::FixedVectorTyID: {
        auto *vector = llvm::cast<llvm::FixedVectorType>(type);
        const unsigned count = vector->getNumElements();
        if (count == 1)
            return typeName(vector->getElementType());
        if (count > 4)
            throw Unsupported("vectors of more than four elements are not supported");
        return vectorPrefix(vector->getElementType()) + std::to_string(count);
    }
    default:
        throw Unsupported("a value of an unsupported type");
    }
}

std::string Emitter::interfaceType(llvm::Type *type)
{
    if (!scalar(type)->isIntegerTy(1))
        return typeName(type);
    if (width(type) > 1)
        return "uvec" + std::to_string(width(type));
    return "uint";
}

std::string Emitter::componentwise(llvm::Type *type, const std::vector<std::string> &values, const std::string &pattern)
{
    // The pattern for one component: $n is the n-th value, or that component of it.
    const auto substitute = [&](unsigned index) {
        std::string text;
        for (size_t i = 0; i < pattern.size(); ++i) {
            const char c = pattern[i];
            if (c == '$' && i + 1 < pattern.size() && unsigned(pattern[i + 1] - '0') < 10) {
                const std::string &value = values[pattern[i + 1] - '0'];
                text += width(type) > 1 ? value + "." + component(index) : value;
                ++i;
                continue;
            }
            text += c;
        }
        return text;
    };
    if (width(type) == 1)
        return substitute(0);
    std::string text = typeName(type) + "(";
    for (unsigned i = 0; i < width(type); ++i)
        text += (i ? ", " : "") + substitute(i);
    return text + ")";
}

std::string Emitter::constant(llvm::Constant *constant)
{
    llvm::Type *type = constant->getType();
    if (auto *vector = llvm::dyn_cast<llvm::FixedVectorType>(type)) {
        const unsigned count = vector->getNumElements();
        std::vector<std::string> elements;
        for (unsigned i = 0; i < count; ++i)
            elements.push_back(scalarConstant(constant->getAggregateElement(i), vector->getElementType()));
        if (count == 1)
            return elements[0];
        // A splat is written once.
        bool same = true;
        for (const std::string &element : elements)
            same = same && element == elements[0];
        std::string text = typeName(type) + "(";
        for (unsigned i = 0; i < (same ? 1 : count); ++i)
            text += (i ? ", " : "") + elements[i];
        return text + ")";
    }
    unsigned count;
    if (auto *structure = llvm::dyn_cast<llvm::StructType>(type))
        count = structure->getNumElements();
    else if (auto *array = llvm::dyn_cast<llvm::ArrayType>(type))
        count = array->getNumElements();
    else
        return scalarConstant(constant, type);
    std::string text = typeName(type) + "(";
    if (count == 0)
        text += "0u";
    for (unsigned i = 0; i < count; ++i)
        text += (i ? ", " : "") + this->constant(constant->getAggregateElement(i));
    return text + ")";
}

std::string Emitter::nameOf(llvm::Value *value)
{
    const auto known = m_names.find(value);
    if (known != m_names.end())
        return known->second;
    const std::string name = "v" + std::to_string(m_nextVariable++);
    m_names[value] = name;
    m_variables.emplace_back(typeName(value->getType()), name);
    return name;
}

std::string Emitter::valueOf(llvm::Value *value)
{
    if (value->getType()->isPointerTy()) {
        const Root &root = rootOf(value);
        if (root.kind == Root::Texture || root.kind == Root::Sampler)
            throw Unsupported("a texture or sampler used as a value");
        return offsetOf(addressOf(value));
    }
    if (llvm::isa<llvm::Argument>(value)) {
        const auto known = m_arguments.find(value);
        if (known == m_arguments.end())
            throw Unsupported("an argument without a role is used");
        return known->second;
    }
    if (auto *constant = llvm::dyn_cast<llvm::Constant>(value))
        return this->constant(constant);
    return nameOf(value);
}

std::string Emitter::select(llvm::SelectInst &select)
{
    const std::string condition = valueOf(select.getCondition());
    const std::string whenTrue = valueOf(select.getTrueValue());
    const std::string whenFalse = valueOf(select.getFalseValue());
    if (select.getCondition()->getType()->isVectorTy())
        return "mix(" + whenFalse + ", " + whenTrue + ", " + condition + ")";
    return "(" + condition + " ? " + whenTrue + " : " + whenFalse + ")";
}

std::string Emitter::shuffle(llvm::ShuffleVectorInst &shuffle)
{
    llvm::Type *type = shuffle.getType();
    const int sourceWidth = width(shuffle.getOperand(0)->getType());
    const std::string first = valueOf(shuffle.getOperand(0));
    const std::string second = valueOf(shuffle.getOperand(1));
    llvm::SmallVector<int, 4> mask;
    shuffle.getShuffleMask(mask);
    if (mask.size() > 4)
        throw Unsupported("vectors of more than four elements are not supported");
    // Only components of the first vector: a swizzle.
    bool swizzle = sourceWidth > 1;
    for (const int index : mask)
        swizzle = swizzle && index >= 0 && index < sourceWidth;
    if (mask.size() > 1 && swizzle) {
        std::string text = first + ".";
        bool identity = unsigned(sourceWidth) == mask.size();
        for (unsigned i = 0; i < mask.size(); ++i) {
            text += component(mask[i]);
            identity = identity && mask[i] == int(i);
        }
        return identity ? first : text;
    }
    llvm::Type *element = scalar(type);
    std::vector<std::string> parts;
    for (const int index : mask) {
        if (index < 0) {
            parts.push_back(scalarConstant(llvm::UndefValue::get(element), element));
            continue;
        }
        const bool fromSecond = index >= sourceWidth;
        parts.push_back(elementOf(shuffle.getOperand(fromSecond), fromSecond ? second : first, fromSecond ? index - sourceWidth : index));
    }
    if (mask.size() == 1)
        return parts[0];
    std::string text = typeName(type) + "(";
    for (unsigned i = 0; i < mask.size(); ++i)
        text += (i ? ", " : "") + parts[i];
    return text + ")";
}

std::string Emitter::binary(llvm::BinaryOperator &operation)
{
    llvm::Type *type = operation.getType();
    const std::string a = valueOf(operation.getOperand(0));
    const std::string b = valueOf(operation.getOperand(1));
    const unsigned count = width(type);
    llvm::Type *element = scalar(type);
    if (!element->isIntegerTy()) {
        switch (operation.getOpcode()) {
        case llvm::Instruction::FAdd:
            return "(" + a + " + " + b + ")";
        case llvm::Instruction::FSub:
            return "(" + a + " - " + b + ")";
        case llvm::Instruction::FMul:
            return "(" + a + " * " + b + ")";
        case llvm::Instruction::FDiv:
            return "(" + a + " / " + b + ")";
        case llvm::Instruction::FRem:
            // fmod: the sign of the dividend, unlike mod() of GLSL.
            return "(" + a + " - " + b + " * trunc(" + a + " / " + b + "))";
        default:
            throw Unsupported("an unsupported floating-point operation");
        }
    }
    if (element->isIntegerTy(1)) {
        // Bools: arithmetic modulo 2.
        const std::string size = std::to_string(count);
        const auto toUint = [&](const std::string &value) {
            return count > 1 ? "uvec" + size + "(" + value + ")" : "uint(" + value + ")";
        };
        const auto toBool = [&](const std::string &value) {
            return count > 1 ? "notEqual(" + value + ", uvec" + size + "(0u))" : "(" + value + " != 0u)";
        };
        switch (operation.getOpcode()) {
        case llvm::Instruction::Add:
        case llvm::Instruction::Sub:
        case llvm::Instruction::Xor:
            if (count < 2)
                return "(" + a + " != " + b + ")";
            return "notEqual(" + a + ", " + b + ")";
        case llvm::Instruction::Mul:
        case llvm::Instruction::And:
            if (count < 2)
                return "(" + a + " && " + b + ")";
            return toBool(toUint(a) + " & " + toUint(b));
        case llvm::Instruction::Or:
            if (count > 1)
                return toBool(toUint(a) + " | " + toUint(b));
            return "(" + a + " || " + b + ")";
        case llvm::Instruction::UDiv:
        case llvm::Instruction::SDiv:
        case llvm::Instruction::AShr:
            return a;
        case llvm::Instruction::URem:
        case llvm::Instruction::SRem:
        case llvm::Instruction::Shl:
        case llvm::Instruction::LShr:
            if (count < 2)
                return "false";
            return "bvec" + size + "(false)";
        default:
            throw Unsupported("an unsupported operation on bools");
        }
    }
    switch (operation.getOpcode()) {
    case llvm::Instruction::Add:
        return masked("(" + a + " + " + b + ")", type);
    case llvm::Instruction::Sub:
        return masked("(" + a + " - " + b + ")", type);
    case llvm::Instruction::Mul:
        return masked("(" + a + " * " + b + ")", type);
    case llvm::Instruction::UDiv:
        return "(" + a + " / " + b + ")";
    case llvm::Instruction::SDiv:
        return fromSigned("(" + asSigned(a, type) + " / " + asSigned(b, type) + ")", type);
    case llvm::Instruction::URem:
        return "(" + a + " % " + b + ")";
    case llvm::Instruction::SRem: {
        // The sign of the dividend, as in C.
        const std::string x = asSigned(a, type);
        const std::string y = asSigned(b, type);
        return fromSigned("(" + x + " - " + y + " * (" + x + " / " + y + "))", type);
    }
    case llvm::Instruction::Shl:
        return masked("(" + a + " << " + b + ")", type);
    case llvm::Instruction::LShr:
        return "(" + a + " >> " + b + ")";
    case llvm::Instruction::AShr:
        return fromSigned("(" + asSigned(a, type) + " >> " + b + ")", type);
    case llvm::Instruction::And:
        return "(" + a + " & " + b + ")";
    case llvm::Instruction::Or:
        return "(" + a + " | " + b + ")";
    case llvm::Instruction::Xor:
        return "(" + a + " ^ " + b + ")";
    default:
        throw Unsupported("an unsupported integer operation");
    }
}

std::string Emitter::compare(llvm::CmpInst &compare)
{
    llvm::Type *type = compare.getOperand(0)->getType();
    std::string a = valueOf(compare.getOperand(0));
    std::string b = valueOf(compare.getOperand(1));
    const llvm::CmpInst::Predicate predicate = compare.getPredicate();
    const unsigned count = width(type);
    const bool vector = count > 1;
    const auto relation = [&](const std::string &op, const std::string &function) {
        return vector ? function + "(" + a + ", " + b + ")" : "(" + a + " " + op + " " + b + ")";
    };
    if (llvm::isa<llvm::ICmpInst>(compare)) {
        if (!scalar(type)->isIntegerTy(1) || llvm::ICmpInst::isEquality(predicate)) {
            if (llvm::CmpInst::isSigned(predicate)) {
                a = asSigned(a, type);
                b = asSigned(b, type);
            }
        } else {
            // Bools in an order: true is 1 unsigned and -1 signed.
            const std::string size = std::to_string(count);
            if (!llvm::CmpInst::isSigned(predicate)) {
                a = vector ? "uvec" + size + "(" + a + ")" : "uint(" + a + ")";
                b = vector ? "uvec" + size + "(" + b + ")" : "uint(" + b + ")";
            } else {
                a = vector ? "-ivec" + size + "(" + a + ")" : "-int(" + a + ")";
                b = vector ? "-ivec" + size + "(" + b + ")" : "-int(" + b + ")";
            }
        }
        switch (predicate) {
        case llvm::CmpInst::ICMP_EQ:
            return relation("==", "equal");
        case llvm::CmpInst::ICMP_NE:
            return relation("!=", "notEqual");
        case llvm::CmpInst::ICMP_UGT:
        case llvm::CmpInst::ICMP_SGT:
            return relation(">", "greaterThan");
        case llvm::CmpInst::ICMP_UGE:
        case llvm::CmpInst::ICMP_SGE:
            return relation(">=", "greaterThanEqual");
        case llvm::CmpInst::ICMP_ULT:
        case llvm::CmpInst::ICMP_SLT:
            return relation("<", "lessThan");
        case llvm::CmpInst::ICMP_ULE:
        case llvm::CmpInst::ICMP_SLE:
            return relation("<=", "lessThanEqual");
        default:
            throw Unsupported("an unsupported integer comparison");
        }
    }
    // Ordered comparisons are false with a NaN, unordered ones true; without NaNs they are the same.
    const bool noNaNs = compare.hasNoNaNs();
    const std::string size = std::to_string(count);
    // Two conditions together, component by component.
    const auto combine = [&](const std::string &first, const std::string &second, bool any) {
        if (vector) {
            const std::string size = std::to_string(count);
            return "notEqual(uvec" + size + "(" + first + ")" + (any ? " | " : " & ") + "uvec" + size + "(" + second + "), uvec" + size
                + "(0u))";
        }
        return "(" + first + (any ? " || " : " && ") + second + ")";
    };
    const auto negate = [&](const std::string &value) {
        return vector ? "not(" + value + ")" : "!" + value;
    };
    const auto unordered = [&]() {
        return combine("isnan(" + a + ")", "isnan(" + b + ")", true);
    };
    const auto ordered = [&]() {
        return combine(negate("isnan(" + a + ")"), negate("isnan(" + b + ")"), false);
    };
    switch (predicate) {
    case llvm::CmpInst::FCMP_FALSE:
        if (vector)
            return "bvec" + size + "(false)";
        return "false";
    case llvm::CmpInst::FCMP_OEQ:
        return relation("==", "equal");
    case llvm::CmpInst::FCMP_OGT:
        return relation(">", "greaterThan");
    case llvm::CmpInst::FCMP_OGE:
        return relation(">=", "greaterThanEqual");
    case llvm::CmpInst::FCMP_OLT:
        return relation("<", "lessThan");
    case llvm::CmpInst::FCMP_OLE:
        return relation("<=", "lessThanEqual");
    case llvm::CmpInst::FCMP_ONE:
        if (!noNaNs)
            return combine(relation("!=", "notEqual"), ordered(), false);
        return relation("!=", "notEqual");
    case llvm::CmpInst::FCMP_ORD:
        if (!noNaNs)
            return ordered();
        if (vector)
            return "bvec" + size + "(true)";
        return "true";
    case llvm::CmpInst::FCMP_UNO:
        if (!noNaNs)
            return unordered();
        if (vector)
            return "bvec" + size + "(false)";
        return "false";
    case llvm::CmpInst::FCMP_UEQ:
        if (noNaNs)
            return relation("==", "equal");
        return combine(relation("==", "equal"), unordered(), true);
    case llvm::CmpInst::FCMP_UGT:
        if (!noNaNs)
            return negate(relation("<=", "lessThanEqual"));
        return relation(">", "greaterThan");
    case llvm::CmpInst::FCMP_UGE:
        if (noNaNs)
            return relation(">=", "greaterThanEqual");
        return negate(relation("<", "lessThan"));
    case llvm::CmpInst::FCMP_ULT:
        if (noNaNs)
            return relation("<", "lessThan");
        return negate(relation(">=", "greaterThanEqual"));
    case llvm::CmpInst::FCMP_ULE:
        if (noNaNs)
            return relation("<=", "lessThanEqual");
        return negate(relation(">", "greaterThan"));
    case llvm::CmpInst::FCMP_UNE:
        return relation("!=", "notEqual");
    case llvm::CmpInst::FCMP_TRUE:
        if (vector)
            return "bvec" + size + "(true)";
        return "true";
    default:
        throw Unsupported("an unsupported floating-point comparison");
    }
}

std::string Emitter::cast(llvm::CastInst &cast)
{
    llvm::Type *type = cast.getType();
    llvm::Value *operand = cast.getOperand(0);
    llvm::Type *source = operand->getType();
    const unsigned count = width(type);
    const std::string size = std::to_string(count);
    const std::string name = typeName(type);
    switch (cast.getOpcode()) {
    case llvm::Instruction::Trunc: {
        const std::string value = valueOf(operand);
        if (!scalar(type)->isIntegerTy(1))
            return masked(name + "(" + value + ")", type);
        // To bool: the lowest bit.
        const std::string one = integer(1, source);
        const std::string zero = count < 2 ? integer(0, source) : typeName(source) + "(" + integer(0, source) + ")";
        if (count < 2)
            return "((" + value + " & " + one + ") != " + zero + ")";
        return "notEqual(" + value + " & " + one + ", " + zero + ")";
    }
    case llvm::Instruction::ZExt:
        return name + "(" + valueOf(operand) + ")";
    case llvm::Instruction::SExt: {
        const std::string value = valueOf(operand);
        if (!scalar(source)->isIntegerTy(1))
            return fromSigned(asSigned(value, source), type);
        return "(" + name + "(" + value + ") * " + mask(bits(scalar(type))) + ")";
    }
    case llvm::Instruction::FPToUI:
    case llvm::Instruction::FPToSI: {
        const std::string value = valueOf(operand);
        if (scalar(type)->isIntegerTy(1)) {
            if (count < 2)
                return "(" + value + " != 0.0)";
            return "notEqual(" + value + ", " + typeName(source) + "(0.0))";
        }
        if (cast.getOpcode() == llvm::Instruction::FPToUI)
            return masked(name + "(" + value + ")", type);
        std::string signedName;
        if (bits(scalar(type)) > 32)
            signedName = count < 2 ? std::string("int64_t") : "i64vec" + size;
        else
            signedName = count < 2 ? std::string("int") : "ivec" + size;
        return fromSigned(signedName + "(" + value + ")", type);
    }
    case llvm::Instruction::UIToFP: {
        std::string text = name + "(" + valueOf(operand) + ")";
        if (scalar(type)->isHalfTy()) {
            requireHalf();
            text = "reef_half(" + text + ")";
        }
        return text;
    }
    case llvm::Instruction::SIToFP: {
        std::string text = name + "(" + asSigned(valueOf(operand), source) + ")";
        if (scalar(type)->isHalfTy()) {
            requireHalf();
            text = "reef_half(" + text + ")";
        }
        return text;
    }
    case llvm::Instruction::FPTrunc: {
        const std::string value = valueOf(operand);
        if (scalar(type)->isHalfTy()) {
            // A half is a float rounded to the precision of a half.
            requireHalf();
            const std::string single = scalar(source)->isDoubleTy() ? (count < 2 ? std::string("float") : "vec" + size) + "(" + value + ")" : value;
            return "reef_half(" + single + ")";
        }
        return name + "(" + value + ")";
    }
    case llvm::Instruction::FPExt:
        return name + "(" + valueOf(operand) + ")";
    case llvm::Instruction::PtrToInt:
        return masked(name + "(" + offsetOf(addressOf(operand)) + ")", type);
    case llvm::Instruction::BitCast: {
        const std::string value = valueOf(operand);
        if (type == source)
            return value;
        if (typeName(source) == name && type->isPointerTy() == source->isPointerTy()
            && scalar(source)->getTypeID() == scalar(type)->getTypeID())
            return value;
        return fromPieces(piecesOf(value, source), type);
    }
    default:
        throw Unsupported("an unsupported conversion");
    }
}

void Emitter::instruction(llvm::Instruction &instruction)
{
    // Terminators, phis and allocas are the business of the control flow and of the memory.
    if (instruction.isTerminator() || llvm::isa<llvm::AllocaInst>(instruction) || llvm::isa<llvm::PHINode>(instruction))
        return;
    if (llvm::isa<llvm::DbgInfoIntrinsic>(instruction))
        return;
    // A 64-bit extension that only indexes addresses: the address does its arithmetic itself.
    if ((llvm::isa<llvm::ZExtInst>(instruction) || llvm::isa<llvm::SExtInst>(instruction)) && instruction.getType()->isIntegerTy(64)
        && !instruction.use_empty()) {
        bool indexOnly = true;
        for (const llvm::User *user : instruction.users()) {
            indexOnly = indexOnly && llvm::isa<llvm::GetElementPtrInst>(user)
                && llvm::cast<llvm::GetElementPtrInst>(user)->getPointerOperand() != &instruction;
        }
        if (indexOnly)
            return;
    }
    if (instruction.getType()->isPointerTy()) {
        // Pointers are addresses; only a choice between two has to be written.
        const Root &root = rootOf(&instruction);
        if (root.kind == Root::Texture || root.kind == Root::Sampler || root.kind == Root::ReadSampler)
            return;
        if (llvm::isa<llvm::CallBase>(instruction))
            throw Unsupported("calls returning pointers are not supported");
        if (auto *select = llvm::dyn_cast<llvm::SelectInst>(&instruction)) {
            line(nameOf(&instruction) + " = (" + valueOf(select->getCondition()) + " ? " + offsetOf(addressOf(select->getTrueValue()))
                 + " : " + offsetOf(addressOf(select->getFalseValue())) + ");");
            return;
        }
        addressOf(&instruction);
        return;
    }
    switch (instruction.getOpcode()) {
    case llvm::Instruction::FNeg:
        line(nameOf(&instruction) + " = (-" + valueOf(instruction.getOperand(0)) + ");");
        return;
    case llvm::Instruction::Load: {
        auto &load = llvm::cast<llvm::LoadInst>(instruction);
        Root &root = rootOf(load.getPointerOperand());
        if (root.kind == Root::Texture || root.kind == Root::Sampler)
            throw Unsupported("loading a texture or sampler handle is not supported");
        line(nameOf(&instruction) + " = " + loadValue(root, addressOf(load.getPointerOperand()), load.getAlign().value(), load.getType()) + ";");
        return;
    }
    case llvm::Instruction::Store: {
        auto &store = llvm::cast<llvm::StoreInst>(instruction);
        Root &root = rootOf(store.getPointerOperand());
        if (root.kind == Root::Constant || root.kind == Root::Texture || root.kind == Root::Sampler || root.readOnly)
            throw Unsupported("a store to read-only memory");
        llvm::Value *value = store.getValueOperand();
        if (value->getType()->isPointerTy())
            throw Unsupported("storing pointers to memory is not supported");
        storeValue(root, addressOf(store.getPointerOperand()), store.getAlign().value(), value->getType(), valueOf(value));
        return;
    }
    case llvm::Instruction::Fence:
        line("memoryBarrier();");
        return;
    case llvm::Instruction::AtomicCmpXchg: {
        auto &exchange = llvm::cast<llvm::AtomicCmpXchgInst>(instruction);
        Root &root = rootOf(exchange.getPointerOperand());
        llvm::Type *type = exchange.getNewValOperand()->getType();
        const std::string expected = valueOf(exchange.getCompareOperand());
        const std::string old = atomic(root, addressOf(exchange.getPointerOperand()), exchange.getAlign().value(), "cmpxchg", type,
                                       valueOf(exchange.getNewValOperand()), expected, false, false);
        // The result is { the old value, whether it was the expected one }.
        const std::string name = nameOf(&instruction);
        line(name + ".m0 = " + old + ";");
        line(name + ".m1 = (" + name + ".m0 == " + expected + ");");
        return;
    }
    case llvm::Instruction::AtomicRMW: {
        auto &update = llvm::cast<llvm::AtomicRMWInst>(instruction);
        Root &root = rootOf(update.getPointerOperand());
        std::string operation;
        bool isSigned = false;
        bool isFloat = false;
        switch (update.getOperation()) {
        case llvm::AtomicRMWInst::Xchg:
            operation = "xchg";
            break;
        case llvm::AtomicRMWInst::Add:
            operation = "add";
            break;
        case llvm::AtomicRMWInst::Sub:
            operation = "sub";
            break;
        case llvm::AtomicRMWInst::And:
            operation = "and";
            break;
        case llvm::AtomicRMWInst::Or:
            operation = "or";
            break;
        case llvm::AtomicRMWInst::Xor:
            operation = "xor";
            break;
        case llvm::AtomicRMWInst::Max:
            operation = "max";
            isSigned = true;
            break;
        case llvm::AtomicRMWInst::Min:
            operation = "min";
            isSigned = true;
            break;
        case llvm::AtomicRMWInst::UMax:
            operation = "max";
            break;
        case llvm::AtomicRMWInst::UMin:
            operation = "min";
            break;
        case llvm::AtomicRMWInst::FAdd:
            operation = "add";
            isFloat = true;
            break;
        case llvm::AtomicRMWInst::FSub:
            operation = "sub";
            isFloat = true;
            break;
        case llvm::AtomicRMWInst::FMax:
            operation = "max";
            isFloat = true;
            break;
        case llvm::AtomicRMWInst::FMin:
            operation = "min";
            isFloat = true;
            break;
        default:
            throw Unsupported("an unsupported atomic operation");
        }
        line(nameOf(&instruction) + " = "
             + atomic(root, addressOf(update.getPointerOperand()), update.getAlign().value(), operation, update.getType(),
                      valueOf(update.getValOperand()), "", isSigned, isFloat)
             + ";");
        return;
    }
    case llvm::Instruction::ICmp:
    case llvm::Instruction::FCmp:
        line(nameOf(&instruction) + " = " + compare(llvm::cast<llvm::CmpInst>(instruction)) + ";");
        return;
    case llvm::Instruction::Call:
        emitCall(llvm::cast<llvm::CallInst>(instruction));
        return;
    case llvm::Instruction::Select:
        line(nameOf(&instruction) + " = " + select(llvm::cast<llvm::SelectInst>(instruction)) + ";");
        return;
    case llvm::Instruction::ExtractElement: {
        auto &extract = llvm::cast<llvm::ExtractElementInst>(instruction);
        const std::string vector = valueOf(extract.getVectorOperand());
        llvm::Type *type = extract.getVectorOperand()->getType();
        if (auto *index = llvm::dyn_cast<llvm::ConstantInt>(extract.getIndexOperand())) {
            line(nameOf(&instruction) + " = " + element(vector, type, index->getZExtValue()) + ";");
            return;
        }
        const std::string value = width(type) != 1 ? vector + "[int(" + valueOf(extract.getIndexOperand()) + ")]" : vector;
        line(nameOf(&instruction) + " = " + value + ";");
        return;
    }
    case llvm::Instruction::InsertElement: {
        const std::string name = nameOf(&instruction);
        llvm::Type *type = instruction.getType();
        const std::string element = valueOf(instruction.getOperand(1));
        if (width(type) == 1) {
            line(name + " = " + element + ";");
            return;
        }
        line(name + " = " + valueOf(instruction.getOperand(0)) + ";");
        if (auto *index = llvm::dyn_cast<llvm::ConstantInt>(instruction.getOperand(2)))
            line(name + "." + component(index->getZExtValue()) + " = " + element + ";");
        else
            line(name + "[int(" + valueOf(instruction.getOperand(2)) + ")] = " + element + ";");
        return;
    }
    case llvm::Instruction::ShuffleVector:
        line(nameOf(&instruction) + " = " + shuffle(llvm::cast<llvm::ShuffleVectorInst>(instruction)) + ";");
        return;
    case llvm::Instruction::ExtractValue: {
        auto &extract = llvm::cast<llvm::ExtractValueInst>(instruction);
        std::string value = valueOf(extract.getAggregateOperand());
        llvm::Type *type = extract.getAggregateOperand()->getType();
        for (const unsigned index : extract.indices()) {
            if (type->isStructTy()) {
                value += ".m" + std::to_string(index);
                type = type->getContainedType(index);
            } else {
                value += "[" + std::to_string(index) + "]";
                type = type->getContainedType(0);
            }
        }
        line(nameOf(&instruction) + " = " + value + ";");
        return;
    }
    case llvm::Instruction::InsertValue: {
        auto &insert = llvm::cast<llvm::InsertValueInst>(instruction);
        const std::string name = nameOf(&instruction);
        line(name + " = " + valueOf(insert.getAggregateOperand()) + ";");
        std::string target = name;
        llvm::Type *type = insert.getType();
        for (const unsigned index : insert.indices()) {
            if (type->isStructTy()) {
                target += ".m" + std::to_string(index);
                type = type->getContainedType(index);
            } else {
                target += "[" + std::to_string(index) + "]";
                type = type->getContainedType(0);
            }
        }
        line(target + " = " + valueOf(insert.getInsertedValueOperand()) + ";");
        return;
    }
    case llvm::Instruction::Freeze:
        line(nameOf(&instruction) + " = " + valueOf(instruction.getOperand(0)) + ";");
        return;
    default:
        if (llvm::isa<llvm::BinaryOperator>(instruction)) {
            line(nameOf(&instruction) + " = " + binary(llvm::cast<llvm::BinaryOperator>(instruction)) + ";");
            return;
        }
        if (llvm::isa<llvm::CastInst>(instruction)) {
            line(nameOf(&instruction) + " = " + cast(llvm::cast<llvm::CastInst>(instruction)) + ";");
            return;
        }
        throw Unsupported(std::string("the instruction ") + instruction.getOpcodeName() + " is not supported");
    }
}

void Emitter::inputs()
{
    static const char *const swizzles[] = {"", ".x", ".xy", ".xyz", ""};
    for (const Argument &argument : m_info.arguments) {
        llvm::Argument *parameter = m_function->getArg(argument.number);
        llvm::Type *type = parameter->getType();
        // A built-in input: only the components the parameter has, and integers masked to their bits.
        const auto define = [&type, this, &parameter](const std::string &text, int components) {
            std::string value = text;
            if (components != 1) {
                const unsigned count = width(type);
                if (count == 1)
                    value = text + ".x";
                else if (count == 2)
                    value = text + ".xy";
                else if (count == 3)
                    value = text + ".xyz";
            }
            if (scalar(type)->isIntegerTy())
                value = masked(value, type);
            m_arguments[parameter] = value;
        };
        switch (argument.role) {
        case Role::Buffer: {
            auto root = std::make_unique<Root>();
            root->index = argument.location;
            root->name = "reef_" + m_prefix + "b" + std::to_string(argument.location) + ".w";
            root->readOnly = argument.addressSpace == 2 || !(argument.access & 2);
            root->argument = &argument;
            m_argumentRoots[parameter] = root.get();
            m_roots.push_back(std::move(root));
            break;
        }
        case Role::Texture:
        case Role::Sampler: {
            auto root = std::make_unique<Root>();
            root->kind = argument.role == Role::Texture ? Root::Texture : Root::Sampler;
            root->index = argument.location;
            root->argument = &argument;
            m_argumentRoots[parameter] = root.get();
            m_roots.push_back(std::move(root));
            break;
        }
        case Role::Threadgroup: {
            auto root = std::make_unique<Root>();
            root->kind = Root::Threadgroup;
            root->index = argument.location;
            root->name = "reef_tg" + std::to_string(argument.location);
            // The length set by the caller, else the size of the type, else 4 KB.
            const auto length = m_options.threadgroupMemory.find(argument.location);
            if (length != m_options.threadgroupMemory.end())
                root->size = std::max<uint64_t>((std::max<int64_t>(length->second, 0) + 3) / 4, 1);
            else
                root->size = argument.size > 0 ? (int64_t(argument.size) + 3) / 4 : 1024;
            root->argument = &argument;
            m_argumentRoots[parameter] = root.get();
            m_roots.push_back(std::move(root));
            break;
        }
        case Role::StageIn: {
            if (parameter->use_empty())
                break;
            if (type->isPointerTy() || type->isStructTy() || type->isArrayTy())
                throw Unsupported("vertex attribute " + argument.name + " is not a scalar or vector");
            const int kind = componentKind(argument.typeName);
            const unsigned count = width(type);
            const std::string size = count > 1 ? std::to_string(count) : "";
            std::string declared;
            if (kind == 1)
                declared = count > 1 ? "ivec" + size : "int";
            else if (kind == 2)
                declared = count > 1 ? "uvec" + size : "uint";
            else
                declared = typeName(type);
            if (kind != 0 && !scalar(type)->isIntegerTy())
                throw Unsupported("vertex attribute " + argument.name + " has an integer type and a float value");
            const std::string name = "reef_attr" + std::to_string(argument.location);
            m_interface.push_back("layout(location = " + std::to_string(argument.location) + ") in " + declared + " " + name + ";");
            std::string value = name;
            if (scalar(type)->isIntegerTy()) {
                if (type->isIntOrIntVectorTy(1))
                    value = count > 1 ? "notEqual(" + name + ", " + declared + "(0))" : "(" + name + " != 0)";
                else if (kind == 2)
                    value = masked(name, type);
                else
                    value = masked((count > 1 ? "uvec" + size : "uint") + "(" + name + ")", type);
            }
            m_arguments[parameter] = value;
            break;
        }
        case Role::FragmentInput: {
            if (parameter->use_empty())
                break;
            if (type->isPointerTy() || type->isStructTy() || type->isArrayTy())
                throw Unsupported("fragment input " + argument.name + " has an unsupported type");
            const std::string name = "reef_v_" + identifier(argument.semantic);
            std::string qualifier;
            if (argument.interpolation == 2 || scalar(type)->isIntegerTy() || scalar(type)->isDoubleTy())
                qualifier = "flat ";
            else if (argument.interpolation == 1)
                qualifier = "noperspective ";
            if (qualifier != "flat ") {
                if (argument.sampling == 1)
                    qualifier += "centroid ";
                else if (argument.sampling == 2)
                    qualifier += "sample ";
            }
            m_interface.push_back(qualifier + "in " + interfaceType(type) + " " + name + ";");
            if (!scalar(type)->isIntegerTy(1))
                m_arguments[parameter] = name;
            else
                m_arguments[parameter] = width(type) > 1 ? "notEqual(" + name + ", uvec" + std::to_string(width(type)) + "(0u))" : "(" + name + " != 0u)";
            m_shader.varyings.push_back({dataType(argument.typeName), argument.interpolation, argument.sampling, 0,
                                         keep(m_shader, argument.semantic), keep(m_shader, name)});
            break;
        }
        case Role::Position:
            m_arguments[parameter] = "gl_FragCoord";
            break;
        case Role::VertexId:
            m_shader.flags |= 0x1;
            define("uint(gl_VertexID)", 1);
            break;
        case Role::InstanceId:
            // Metal counts instances from the base instance, GL does not.
            m_shader.flags |= 0x2;
            require("GL_ARB_shader_draw_parameters");
            define("uint(gl_InstanceID + gl_BaseInstanceARB)", 1);
            break;
        case Role::BaseVertex:
            m_shader.flags |= 0x4;
            require("GL_ARB_shader_draw_parameters");
            define("uint(gl_BaseVertexARB)", 1);
            break;
        case Role::BaseInstance:
            m_shader.flags |= 0x8;
            require("GL_ARB_shader_draw_parameters");
            define("uint(gl_BaseInstanceARB)", 1);
            break;
        case Role::FrontFacing:
            m_shader.flags |= 0x1000;
            m_arguments[parameter] = type->isIntegerTy(1) ? "gl_FrontFacing" : "uint(gl_FrontFacing)";
            break;
        case Role::PointCoord:
            m_arguments[parameter] = "gl_PointCoord";
            break;
        case Role::SampleId:
            m_shader.flags |= 0x2000;
            define("uint(gl_SampleID)", 1);
            break;
        case Role::SampleMask:
            define("uint(gl_SampleMaskIn[0])", 1);
            break;
        case Role::PrimitiveId:
            define("uint(gl_PrimitiveID)", 1);
            break;
        case Role::RenderTargetArrayIndex:
            define("uint(gl_Layer)", 1);
            break;
        case Role::ViewportArrayIndex:
            define("uint(gl_ViewportIndex)", 1);
            break;
        case Role::ThreadPositionInGrid:
            define("gl_GlobalInvocationID", 3);
            break;
        case Role::ThreadPositionInThreadgroup:
            define("gl_LocalInvocationID", 3);
            break;
        case Role::ThreadgroupPositionInGrid:
            define("gl_WorkGroupID", 3);
            break;
        case Role::ThreadsPerThreadgroup:
            define("gl_WorkGroupSize", 3);
            break;
        case Role::ThreadsPerGrid:
            // GL has no such input: a uniform at the first location.
            m_shader.uniformLocations[0] = m_options.firstLocation;
            define("reef_threads_per_grid", 3);
            break;
        case Role::ThreadgroupsPerGrid:
            define("gl_NumWorkGroups", 3);
            break;
        case Role::ThreadIndexInThreadgroup:
            define("gl_LocalInvocationIndex", 1);
            break;
        case Role::ThreadIndexInSimdgroup:
            require("GL_ARB_shader_ballot");
            define("gl_SubGroupInvocationARB", 1);
            break;
        case Role::ThreadsPerSimdgroup:
            require("GL_ARB_shader_ballot");
            define("gl_SubGroupSizeARB", 1);
            break;
        case Role::SimdgroupIndexInThreadgroup:
            require("GL_ARB_shader_ballot");
            define("(gl_LocalInvocationIndex / gl_SubGroupSizeARB)", 1);
            break;
        case Role::SimdgroupsPerThreadgroup:
            require("GL_ARB_shader_ballot");
            define("((gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z + gl_SubGroupSizeARB - 1u) / gl_SubGroupSizeARB)", 1);
            break;
        case Role::Color: {
            // A colour attachment read back: a texture of the framebuffer fetched at the fragment.
            if (parameter->use_empty())
                break;
            if (type->isPointerTy() || type->isStructTy() || type->isArrayTy())
                throw Unsupported("colour input " + argument.name + " is not a scalar or vector");
            const int kind = componentKind(argument.typeName);
            const unsigned count = width(type);
            const std::string name = "reef_fb" + std::to_string(argument.location);
            m_framebuffers.push_back({argument.location, kind});
            std::string value = "texelFetch(" + name + ", ivec2(gl_FragCoord.xy), 0)" + swizzles[std::min(count, 4u)];
            if (scalar(type)->isIntegerTy()) {
                const std::string size = count > 1 ? std::to_string(count) : "";
                if (kind != 2)
                    value = (count > 1 ? "uvec" + size : "uint") + "(" + value + ")";
                value = masked(value, type);
            } else if (kind != 0) {
                throw Unsupported("colour input " + argument.name + " has an integer type and a float value");
            }
            m_arguments[parameter] = value;
            break;
        }
        default:
            if (!parameter->use_empty())
                throw Unsupported("argument " + argument.name + " has an unsupported role");
            break;
        }
    }
}

std::vector<reef_mtl_attribute> attributesOf(const FunctionInfo &info, std::deque<std::string> &strings)
{
    std::vector<reef_mtl_attribute> attributes;
    for (const Argument &argument : info.arguments) {
        if (argument.role != Role::StageIn)
            continue;
        reef_mtl_attribute attribute = {};
        attribute.index = argument.location;
        attribute.type = dataType(argument.typeName);
        attribute.kind = componentKind(argument.typeName);
        int element = 0;
        int components = 1;
        splitDataType(attribute.type, element, components);
        attribute.components = components;
        attribute.active = !argument.unused;
        strings.push_back(argument.name);
        attribute.name = strings.back().c_str();
        attributes.push_back(attribute);
    }
    std::sort(attributes.begin(), attributes.end(), [](const reef_mtl_attribute &a, const reef_mtl_attribute &b) {
        return a.index < b.index;
    });
    return attributes;
}

void Emitter::resolveFunctionConstants()
{
    // A global becomes a constant of that value, and what loads it, the value.
    const auto makeConstant = [](llvm::GlobalVariable *global, llvm::Constant *value) {
        global->setInitializer(value);
        global->setConstant(true);
        global->setLinkage(llvm::GlobalValue::InternalLinkage);
        llvm::SmallVector<llvm::LoadInst *, 8> loads;
        for (llvm::User *user : global->users()) {
            auto *load = llvm::dyn_cast<llvm::LoadInst>(user);
            if (load && load->getType() == value->getType())
                loads.push_back(load);
        }
        for (llvm::LoadInst *load : loads) {
            load->replaceAllUsesWith(value);
            load->eraseFromParent();
        }
    };
    llvm::DenseMap<const llvm::Value *, bool> given;
    for (const FunctionConstant &constant : m_info.constants) {
        // By name when the caller gave no index.
        const ConstantValue *value = nullptr;
        for (const ConstantValue &candidate : m_options.constants) {
            if (candidate.index >= 0 ? candidate.index == constant.index : !constant.name.empty() && candidate.name == constant.name) {
                value = &candidate;
                break;
            }
        }
        if (!value && constant.required && constant.value && !constant.value->use_empty())
            throw Unsupported("no value for the function constant " + constant.name);
        if (constant.defined)
            makeConstant(constant.defined, llvm::ConstantInt::get(constant.defined->getValueType(), value != nullptr));
        if (!constant.value)
            continue;
        llvm::Type *type = constant.value->getValueType();
        llvm::Constant *initial = llvm::Constant::getNullValue(type);
        if (value) {
            // The bytes, in the order of memory, short ones filled with zeros.
            std::vector<uint8_t> bytes(value->bytes.begin(), value->bytes.end());
            const uint64_t size = m_layout.getTypeStoreSize(type);
            if (bytes.size() < size)
                bytes.resize(size, 0);
            if (type->isIntegerTy(1)) {
                initial = llvm::ConstantInt::get(type, bytes[0] != 0);
            } else {
                llvm::APInt number(size * 8, 0);
                for (uint64_t i = 0; i < size; ++i)
                    number.insertBits(bytes[i], i * 8, 8);
                initial = llvm::ConstantFoldCastOperand(llvm::Instruction::BitCast,
                                                        llvm::ConstantInt::get(llvm::IntegerType::get(type->getContext(), size * 8), number), type,
                                                        m_layout);
                if (!initial)
                    throw Unsupported("the value of function constant " + constant.name + " does not fit its type");
            }
        }
        given[constant.value] = value != nullptr;
        makeConstant(constant.value, initial);
    }
    llvm::Function *query = m_module.getFunction("air.is_function_constant_defined");
    if (!query)
        return;
    llvm::SmallVector<llvm::CallInst *, 8> calls;
    for (llvm::User *user : query->users()) {
        if (auto *call = llvm::dyn_cast<llvm::CallInst>(user))
            calls.push_back(call);
    }
    for (llvm::CallInst *call : calls) {
        if (call->arg_size() != 1)
            throw Unsupported("is_function_constant_defined asks about something not a function constant");
        const auto known = given.find(call->getArgOperand(0)->stripPointerCasts());
        if (known == given.end())
            throw Unsupported("is_function_constant_defined asks about something not a function constant");
        call->replaceAllUsesWith(llvm::ConstantInt::get(call->getType(), known->second));
        call->eraseFromParent();
    }
}

void Emitter::runInitializers()
{
    llvm::GlobalVariable *constructors = m_module.getGlobalVariable("llvm.global_ctors", true);
    if (!constructors || constructors->isDeclaration())
        return;
    auto *list = llvm::dyn_cast<llvm::ConstantArray>(constructors->getInitializer());
    if (!list)
        return;
    std::vector<llvm::Function *> functions;
    for (llvm::Value *entry : list->operands()) {
        auto *structure = llvm::dyn_cast<llvm::ConstantStruct>(entry);
        if (!structure || structure->getNumOperands() < 2)
            continue;
        auto *function = llvm::dyn_cast<llvm::Function>(structure->getOperand(1)->stripPointerCasts());
        if (function && !function->isDeclaration())
            functions.push_back(function);
    }
    // Each initializer runs here, on constants: straight code that reads and writes globals.
    for (llvm::Function *function : functions) {
        llvm::DenseMap<llvm::Value *, llvm::Constant *> values;
        const auto known = [&](llvm::Value *value) -> llvm::Constant * {
            if (llvm::Constant *constant = values.lookup(value))
                return constant;
            return llvm::dyn_cast<llvm::Constant>(value);
        };
        for (llvm::Instruction &instruction : function->getEntryBlock()) {
            if (auto *load = llvm::dyn_cast<llvm::LoadInst>(&instruction)) {
                auto *global = llvm::dyn_cast<llvm::GlobalVariable>(load->getPointerOperand()->stripPointerCasts());
                if (!global || global->isDeclaration() || load->getType() != global->getValueType())
                    throw Unsupported("a static initializer reads memory the translator cannot know");
                values[load] = global->getInitializer();
                continue;
            }
            if (auto *store = llvm::dyn_cast<llvm::StoreInst>(&instruction)) {
                auto *global = llvm::dyn_cast<llvm::GlobalVariable>(store->getPointerOperand()->stripPointerCasts());
                llvm::Constant *value = known(store->getValueOperand());
                if (!value || !global || value->getType() != global->getValueType())
                    throw Unsupported("a static initializer stores what the translator cannot know");
                global->setInitializer(value);
                global->setConstant(true);
                global->setExternallyInitialized(false);
                // Other functions read the value itself.
                llvm::SmallVector<llvm::LoadInst *, 8> loads;
                for (llvm::User *user : global->users()) {
                    auto *read = llvm::dyn_cast<llvm::LoadInst>(user);
                    if (read && read->getFunction() != function && read->getType() == value->getType())
                        loads.push_back(read);
                }
                for (llvm::LoadInst *read : loads) {
                    read->replaceAllUsesWith(value);
                    read->eraseFromParent();
                }
                continue;
            }
            if (auto *call = llvm::dyn_cast<llvm::CallInst>(&instruction)) {
                llvm::Function *callee = call->getCalledFunction();
                llvm::Constant *argument = call->arg_size() == 1 ? known(call->getArgOperand(0)) : nullptr;
                if (callee && callee->getName().starts_with("air.normalize_function_constant_predicate") && argument) {
                    values[call] = llvm::ConstantInt::get(call->getType(), !llvm::cast<llvm::ConstantInt>(argument)->isZero());
                    continue;
                }
                throw Unsupported("a static initializer makes a call the translator cannot evaluate: "
                                  + (callee ? callee->getName().str() : std::string("an indirect call")));
            }
            if (llvm::isa<llvm::ReturnInst>(instruction))
                break;
            llvm::SmallVector<llvm::Constant *, 4> operands;
            for (llvm::Value *operand : instruction.operands()) {
                llvm::Constant *value = known(operand);
                if (!value)
                    throw Unsupported("a static initializer computes what the translator cannot know");
                operands.push_back(value);
            }
            llvm::Constant *folded = llvm::ConstantFoldInstOperands(&instruction, operands, m_layout);
            if (!folded)
                throw Unsupported("a static initializer computes what the translator cannot fold");
            values[&instruction] = folded;
        }
    }
    constructors->eraseFromParent();
    for (llvm::Function *function : functions) {
        if (function->use_empty())
            function->eraseFromParent();
    }
}

void Emitter::prepare()
{
    // Everything is inlined into the function.
    for (llvm::Function &function : m_module) {
        if (&function == m_function || function.isDeclaration())
            continue;
        function.removeFnAttr(llvm::Attribute::NoInline);
        function.removeFnAttr(llvm::Attribute::OptimizeNone);
        function.addFnAttr(llvm::Attribute::AlwaysInline);
    }
    llvm::LoopAnalysisManager loopAnalyses;
    llvm::FunctionAnalysisManager functionAnalyses;
    llvm::CGSCCAnalysisManager sccAnalyses;
    llvm::ModuleAnalysisManager moduleAnalyses;
    llvm::PassBuilder builder;
    builder.registerModuleAnalyses(moduleAnalyses);
    builder.registerCGSCCAnalyses(sccAnalyses);
    builder.registerFunctionAnalyses(functionAnalyses);
    builder.registerLoopAnalyses(loopAnalyses);
    builder.crossRegisterProxies(loopAnalyses, functionAnalyses, sccAnalyses, moduleAnalyses);
    llvm::ModulePassManager inlining;
    inlining.addPass(llvm::AlwaysInlinerPass(false));
    inlining.run(m_module, moduleAnalyses);
    for (llvm::BasicBlock &block : *m_function) {
        for (llvm::Instruction &instruction : block) {
            auto *call = llvm::dyn_cast<llvm::CallBase>(&instruction);
            if (!call)
                continue;
            llvm::Function *callee = call->getCalledFunction();
            if (!callee)
                throw Unsupported("indirect calls are not supported");
            if (!callee->isDeclaration())
                throw Unsupported("the call of " + callee->getName().str() + " could not be inlined");
        }
    }
    expandResourceArrays();
    // any() and all() of vectors of bools become their components, or-ed or and-ed.
    std::vector<llvm::CallInst *> reductions;
    for (llvm::BasicBlock &block : *m_function) {
        for (llvm::Instruction &instruction : block) {
            auto *call = llvm::dyn_cast<llvm::CallInst>(&instruction);
            if (!call || !call->getCalledFunction() || call->arg_size() != 1)
                continue;
            const llvm::StringRef name = call->getCalledFunction()->getName();
            auto *vector = llvm::dyn_cast<llvm::FixedVectorType>(call->getArgOperand(0)->getType());
            if ((name.starts_with("air.any.") || name.starts_with("air.all.")) && vector && vector->getElementType()->isIntegerTy(1)
                && call->getType()->isIntegerTy(1))
                reductions.push_back(call);
        }
    }
    for (llvm::CallInst *call : reductions) {
        const bool any = call->getCalledFunction()->getName().starts_with("air.any.");
        llvm::Value *vector = call->getArgOperand(0);
        const unsigned count = llvm::cast<llvm::FixedVectorType>(vector->getType())->getNumElements();
        llvm::IRBuilder<> builder(call);
        llvm::Value *result = nullptr;
        for (unsigned i = 0; i < count; ++i) {
            llvm::Value *element = builder.CreateExtractElement(vector, builder.getInt64(i));
            result = !result ? element : any ? builder.CreateOr(result, element) : builder.CreateAnd(result, element);
        }
        call->replaceAllUsesWith(result);
        call->eraseFromParent();
    }
    llvm::FunctionPassManager passes;
    passes.addPass(llvm::SROAPass(llvm::SROAOptions::ModifyCFG));
    passes.addPass(llvm::EarlyCSEPass());
    passes.addPass(llvm::SCCPPass());
    passes.addPass(llvm::SimplifyCFGPass(llvm::SimplifyCFGOptions().bonusInstThreshold(4)));
    passes.addPass(llvm::ADCEPass());
    passes.addPass(llvm::SimplifyCFGPass(llvm::SimplifyCFGOptions().bonusInstThreshold(4)));
    passes.run(*m_function, functionAnalyses);
    unrollLoops(functionAnalyses);
    if (splitResourceIndexing()) {
        llvm::FunctionPassManager cleaning;
        cleaning.addPass(llvm::SROAPass(llvm::SROAOptions::ModifyCFG));
        cleaning.addPass(llvm::EarlyCSEPass());
        cleaning.addPass(llvm::SimplifyCFGPass(llvm::SimplifyCFGOptions().bonusInstThreshold(4)));
        cleaning.addPass(llvm::ADCEPass());
        cleaning.run(*m_function, functionAnalyses);
    }
}

void Emitter::unrollLoops(llvm::FunctionAnalysisManager &analyses)
{
    llvm::FunctionPassManager simplify;
    simplify.addPass(llvm::LoopSimplifyPass());
    simplify.addPass(llvm::LCSSAPass());
    simplify.run(*m_function, analyses);
    llvm::LoopInfo &loops = analyses.getResult<llvm::LoopAnalysis>(*m_function);
    llvm::ScalarEvolution &evolution = analyses.getResult<llvm::ScalarEvolutionAnalysis>(*m_function);
    // Short innermost loops that index private memory: unrolled, the indices are constants and the arrays become registers.
    bool marked = false;
    for (llvm::Loop *loop : loops.getLoopsInPreorder()) {
        const unsigned trips = evolution.getSmallConstantTripCount(loop);
        if (trips < 1 || trips > 16 || !loop->isInnermost())
            continue;
        bool local = false;
        size_t size = 0;
        for (llvm::BasicBlock *block : loop->blocks()) {
            size += block->size();
            for (llvm::Instruction &instruction : *block) {
                if (!llvm::isa<llvm::LoadInst>(instruction) && !llvm::isa<llvm::StoreInst>(instruction))
                    continue;
                llvm::Value *pointer = llvm::getLoadStorePointerOperand(&instruction);
                for (int step = 0; pointer && step < 16; ++step) {
                    if (llvm::isa<llvm::AllocaInst>(pointer)) {
                        local = true;
                        break;
                    }
                    if (auto *offset = llvm::dyn_cast<llvm::GEPOperator>(pointer))
                        pointer = offset->getPointerOperand();
                    else if (auto *bitcast = llvm::dyn_cast<llvm::BitCastOperator>(pointer))
                        pointer = bitcast->getOperand(0);
                    else
                        break;
                }
            }
        }
        if (!local || uint64_t(trips) * size > 1024)
            continue;
        llvm::addStringMetadataToLoop(loop, "llvm.loop.unroll.full", 1);
        marked = true;
    }
    if (!marked)
        return;
    llvm::FunctionPassManager passes;
    passes.addPass(llvm::LoopUnrollPass(
        llvm::LoopUnrollOptions(2, true, false).setPartial(false).setRuntime(false).setUpperBound(false).setProfileBasedPeeling(false)));
    passes.addPass(llvm::SROAPass(llvm::SROAOptions::ModifyCFG));
    passes.addPass(llvm::EarlyCSEPass());
    passes.addPass(llvm::SCCPPass());
    passes.addPass(llvm::SimplifyCFGPass(llvm::SimplifyCFGOptions().bonusInstThreshold(4)));
    passes.addPass(llvm::ADCEPass());
    passes.addPass(llvm::SimplifyCFGPass(llvm::SimplifyCFGOptions().bonusInstThreshold(4)));
    passes.run(*m_function, analyses);
}

void Emitter::structurize()
{
    llvm::LoopAnalysisManager loopAnalyses;
    llvm::FunctionAnalysisManager functionAnalyses;
    llvm::CGSCCAnalysisManager sccAnalyses;
    llvm::ModuleAnalysisManager moduleAnalyses;
    llvm::PassBuilder builder;
    builder.registerModuleAnalyses(moduleAnalyses);
    builder.registerCGSCCAnalyses(sccAnalyses);
    builder.registerFunctionAnalyses(functionAnalyses);
    builder.registerLoopAnalyses(loopAnalyses);
    builder.crossRegisterProxies(loopAnalyses, functionAnalyses, sccAnalyses, moduleAnalyses);
    llvm::FunctionPassManager passes;
    passes.addPass(llvm::LowerSwitchPass());
    passes.addPass(llvm::FixIrreduciblePass());
    passes.addPass(llvm::UnifyLoopExitsPass());
    passes.addPass(llvm::StructurizeCFGPass());
    passes.run(*m_function, functionAnalyses);
    expandConstantExpressions();
    m_dominators = std::make_unique<llvm::DominatorTree>(*m_function);
    m_loops = std::make_unique<llvm::LoopInfo>(*m_dominators);
    // The body is written again from the start.
    m_body.str("");
    m_body.clear();
    m_written.clear();
    m_merges.clear();
    m_tails.clear();
    m_repeated = 0;
}

void Emitter::translate()
{
    m_shader.stage = m_info.stage;
    resolveFunctionConstants();
    runInitializers();
    prepare();
    expandConstantExpressions();
    m_dominators = std::make_unique<llvm::DominatorTree>(*m_function);
    m_loops = std::make_unique<llvm::LoopInfo>(*m_dominators);
    inputs();
    collectTextures();
    // The control flow as it is, then made structured by the passes, then a state machine, which takes anything.
    std::string body;
    bool machine = false;
    try {
        body = structured();
    } catch (const Unstructured &) {
        reset();
        structurize();
        try {
            body = structured();
        } catch (const Unstructured &) {
            machine = true;
        }
    }
    if (machine) {
        m_body.str("");
        m_body.clear();
        m_written.clear();
        m_addresses.clear();
        m_machine = true;
        m_indent = 1;
        body = stateMachine();
    }
    assemble(body);
}

bool translate(llvm::Module &module, const FunctionInfo &info, const Options &options, Shader &shader, std::string &error)
{
    try {
        Emitter emitter(module, info, options, shader);
        emitter.translate();
    } catch (const Unsupported &unsupported) {
        error = unsupported.what();
        return false;
    }
    return true;
}

std::string optionsKey(const Options &options)
{
    std::ostringstream text;
    text << options.firstBuffer << ',' << options.firstTexture << ',' << options.firstImage << ',' << options.firstLocation << ';'
         << options.threadgroupSize[0] << ',' << options.threadgroupSize[1] << ',' << options.threadgroupSize[2] << ';' << options.last
         << ';';
    for (const ConstantValue &constant : options.constants) {
        text << constant.index << ':' << constant.name << ':' << constant.type << ':';
        for (const unsigned char byte : constant.bytes)
            text << int(byte) << '.';
        text << ';';
    }
    for (const auto &[index, length] : options.threadgroupMemory)
        text << index << '=' << length << ';';
    return text.str();
}

}
