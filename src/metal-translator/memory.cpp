// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Memory: what pointers point into, loads, stores, atomics and copies. Every buffer is an array of words in GLSL.

#include "emitter.h"

#include <llvm/IR/GetElementPtrTypeIterator.h>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdio>

using namespace llvm;

namespace cutefish::reef::metal {

// The part of a root's GLSL name that its functions carry: "kb0" of "reef_kb0.w".
static std::string suffixOf(const Root &root)
{
    std::string name = root.name;
    if (name.compare(0, 5, "reef_") == 0)
        name = name.substr(5);
    const size_t dot = name.find('.');
    if (dot != std::string::npos)
        name = name.substr(0, dot);
    return name;
}

// Functions that load from any address, one for each size.
static std::string loadFunctions(const Root &root, const std::string &suffix)
{
    return "uint reef_ld8_" + suffix + "(uint a) { return (" + root.name + "[a >> 2] >> ((a & 3u) << 3)) & 0xffu; }\n"
           "uint reef_ld16_" + suffix + "(uint a) { if ((a & 1u) == 0u) return (" + root.name
           + "[a >> 2] >> ((a & 2u) << 3)) & 0xffffu; return reef_ld8_" + suffix + "(a) | (reef_ld8_" + suffix
           + "(a + 1u) << 8); }\n"
             "uint reef_ld32_" + suffix + "(uint a) { uint s = (a & 3u) << 3; if (s == 0u) return " + root.name
           + "[a >> 2]; return (" + root.name + "[a >> 2] >> s) | (" + root.name + "[(a >> 2) + 1u] << (32u - s)); }\n";
}

// Functions that store to any address. The neighbours in a word of shared memory may be written by others meanwhile.
static std::string storeFunctions(const Root &root, const std::string &suffix)
{
    const bool shared = root.kind == Root::Buffer || root.kind == Root::Threadgroup;
    const std::string &name = root.name;
    auto replace = [&](const std::string &mask, const std::string &bits) {
        if (shared)
            return "atomicAnd(" + name + "[a >> 2], ~(" + mask + ")); atomicOr(" + name + "[a >> 2], " + bits + ");";
        return name + "[a >> 2] = (" + name + "[a >> 2] & ~(" + mask + ")) | " + bits + ";";
    };
    return "void reef_st8_" + suffix + "(uint a, uint v) { uint s = (a & 3u) << 3; " + replace("0xffu << s", "((v & 0xffu) << s)")
           + " }\n"
             "void reef_st16_" + suffix + "(uint a, uint v) { if ((a & 1u) == 0u) { uint s = (a & 2u) << 3; "
           + replace("0xffffu << s", "((v & 0xffffu) << s)") + " return; } reef_st8_" + suffix + "(a, v); reef_st8_" + suffix
           + "(a + 1u, v >> 8); }\n"
             "void reef_st32_" + suffix + "(uint a, uint v) { if ((a & 3u) == 0u) { " + name
           + "[a >> 2] = v; return; } reef_st16_" + suffix + "(a, v); reef_st16_" + suffix + "(a + 2u, v >> 16); }\n";
}

// The alignment of an address, from what is known of its root.
uint64_t Emitter::alignmentOf(const Address &address, uint64_t alignment)
{
    if (!address.offset.empty())
        return alignment;
    const unsigned constant = unsigned(address.constant);
    return constant ? std::min<uint64_t>(constant & -constant, 16) : 16;
}

std::string Emitter::loadValue(Root &root, const Address &address, uint64_t alignment, Type *type)
{
    // A part of the value lies further than the address: its alignment is what both tell.
    auto alignmentAt = [&](uint64_t offset) {
        const Address at{address.offset, address.constant + int64_t(offset)};
        if (address.offset.empty())
            return alignmentOf(at, alignment);
        return offset ? std::min<uint64_t>(alignment, offset & -offset) : alignment;
    };
    std::function<std::string(Type *, uint64_t)> load = [&](Type *type, uint64_t offset) -> std::string {
        if (auto *structure = dyn_cast<StructType>(type)) {
            const StructLayout *layout = m_layout.getStructLayout(structure);
            std::string text = typeName(type) + "(";
            for (unsigned i = 0; i < structure->getNumElements(); ++i)
                text += (i ? ", " : "") + load(structure->getElementType(i), offset + layout->getElementOffset(i));
            if (structure->getNumElements() == 0)
                text += "0u";
            return text + ")";
        }
        if (auto *array = dyn_cast<ArrayType>(type)) {
            const uint64_t stride = m_layout.getTypeAllocSize(array->getElementType());
            std::string text = typeName(type) + "(";
            for (uint64_t i = 0; i < array->getNumElements(); ++i)
                text += (i ? ", " : "") + load(array->getElementType(), offset + i * stride);
            return text + ")";
        }
        if (scalar(type)->isIntegerTy(1) && type->isVectorTy())
            throw Unsupported("vectors of bools in memory are not supported");
        if (type->isPointerTy())
            throw Unsupported("loading pointers from memory is not supported");
        const uint64_t size = m_layout.getTypeStoreSize(type);
        const Address at{address.offset, address.constant + int64_t(offset)};
        return fromPieces(loadPieces(root, at, alignmentAt(offset), size), type);
    };
    return load(type, 0);
}

// The bytes of a constant, at this offset of the vector.
void Emitter::constantBytes(Constant *constant, std::vector<uint8_t> &bytes, uint64_t offset)
{
    if (isa<UndefValue>(constant) || constant->isNullValue())
        return;
    Type *type = constant->getType();
    if (auto *integer = dyn_cast<ConstantInt>(constant)) {
        const uint64_t size = m_layout.getTypeStoreSize(type);
        for (uint64_t i = 0; i < size && offset + i < bytes.size(); ++i)
            bytes[offset + i] = uint8_t(integer->getValue().extractBitsAsZExtValue(std::min(integer->getBitWidth() - unsigned(i) * 8, 8u), unsigned(i) * 8));
        return;
    }
    if (auto *number = dyn_cast<ConstantFP>(constant)) {
        const APInt bits = number->getValueAPF().bitcastToAPInt();
        for (unsigned i = 0; i * 8 < bits.getBitWidth() && offset + i < bytes.size(); ++i)
            bytes[offset + i] = uint8_t(bits.extractBitsAsZExtValue(8, i * 8));
        return;
    }
    if (auto *structure = dyn_cast<StructType>(type)) {
        const StructLayout *layout = m_layout.getStructLayout(structure);
        for (unsigned i = 0; i < structure->getNumElements(); ++i)
            constantBytes(constant->getAggregateElement(i), bytes, offset + layout->getElementOffset(i));
        return;
    }
    if (auto *array = dyn_cast<ArrayType>(type)) {
        const uint64_t stride = m_layout.getTypeAllocSize(array->getElementType());
        for (uint64_t i = 0; i < array->getNumElements(); ++i)
            constantBytes(constant->getAggregateElement(unsigned(i)), bytes, offset + i * stride);
        return;
    }
    if (auto *vector = dyn_cast<FixedVectorType>(type)) {
        const uint64_t stride = m_layout.getTypeStoreSize(vector->getElementType());
        if (vector->getElementType()->isIntegerTy(1))
            throw Unsupported("vectors of bools in constant data are not supported");
        for (unsigned i = 0; i < vector->getNumElements(); ++i)
            constantBytes(constant->getAggregateElement(i), bytes, offset + i * stride);
        return;
    }
    throw Unsupported("constant data holding pointers is not supported");
}

// A constant as the list of words its GLSL array is made from.
std::string Emitter::constantWords(Constant *constant, uint64_t size)
{
    std::vector<uint8_t> bytes(size);
    constantBytes(constant, bytes, 0);
    std::string words;
    for (uint64_t i = 0; i < size; i += 4) {
        unsigned word = 0;
        for (unsigned k = 0; k < 4 && i + k < size; ++k)
            word |= unsigned(bytes[i + k]) << (8 * k);
        char text[16];
        snprintf(text, sizeof(text), "0x%xu", word);
        words += (i ? ", " : "") + std::string(text);
    }
    return words;
}

std::string Emitter::offsetOf(const Address &address)
{
    const std::string constant = std::to_string(unsigned(address.constant)) + "u";
    if (address.offset.empty())
        return constant;
    if (unsigned(address.constant) == 0)
        return address.offset;
    return "(" + address.offset + " + " + constant + ")";
}

// A value taken apart into numbers of up to 32 bits, the low ones first.
std::vector<Piece> Emitter::piecesOf(const std::string &value, Type *type)
{
    std::vector<Piece> result;
    if (auto *vector = dyn_cast<FixedVectorType>(type)) {
        for (unsigned i = 0; i < vector->getNumElements(); ++i) {
            const std::vector<Piece> part = piecesOf(element(value, type, i), vector->getElementType());
            result.insert(result.end(), part.begin(), part.end());
        }
        return result;
    }
    if (type->isIntegerTy(1)) {
        result.push_back({"uint(" + value + ")", 1});
        return result;
    }
    switch (type->getTypeID()) {
    case Type::HalfTyID:
        requireHalf();
        result.push_back({"reef_half_bits(" + value + ")", 16});
        break;
    case Type::BFloatTyID:
        result.push_back({"(floatBitsToUint(" + value + ") >> 16u)", 16});
        break;
    case Type::FloatTyID:
        result.push_back({"floatBitsToUint(" + value + ")", 32});
        break;
    case Type::DoubleTyID:
        result.push_back({"unpackDouble2x32(" + value + ").x", 32});
        result.push_back({"unpackDouble2x32(" + value + ").y", 32});
        break;
    case Type::IntegerTyID: {
        const unsigned width = type->getIntegerBitWidth();
        if (width > 32) {
            result.push_back({"uint(" + value + ")", 32});
            result.push_back({"uint(" + value + " >> 32u)", width - 32});
        } else {
            result.push_back({value, width});
        }
        break;
    }
    default:
        throw Unsupported("the bits of this type cannot be taken apart");
    }
    return result;
}

// Some of the bits of a value that is in pieces.
std::string Emitter::extract(const std::vector<Piece> &pieces, unsigned low, unsigned count)
{
    std::vector<std::string> terms;
    unsigned start = 0;
    for (const Piece &piece : pieces) {
        const unsigned from = std::max(start, low);
        const unsigned to = std::min(start + piece.bits, low + count);
        if (from < to) {
            std::string text = piece.text;
            const unsigned skipped = from - start;
            if (skipped)
                text = "(" + text + " >> " + std::to_string(skipped) + "u)";
            if (to - start < piece.bits)
                text = "(" + text + " & " + mask(to - from) + ")";
            if (from > low)
                text = "(" + text + " << " + std::to_string(from - low) + "u)";
            terms.push_back(text);
        }
        start += piece.bits;
    }
    if (terms.empty())
        return "0u";
    if (terms.size() == 1)
        return terms[0];
    std::string text = "(";
    for (size_t i = 0; i < terms.size(); ++i)
        text += (i ? " | " : "") + terms[i];
    return text + ")";
}

std::string Emitter::scalarOf(const std::vector<Piece> &pieces, Type *type)
{
    const std::string low = extract(pieces, 0, std::min(bits(type), 32u));
    if (type->isIntegerTy(1))
        return "(" + low + " != 0u)";
    switch (type->getTypeID()) {
    case Type::HalfTyID:
        return "unpackHalf2x16(" + low + ").x";
    case Type::BFloatTyID:
        return "uintBitsToFloat(" + low + " << 16u)";
    case Type::FloatTyID:
        return "uintBitsToFloat(" + low + ")";
    case Type::DoubleTyID:
        return "packDouble2x32(uvec2(" + low + ", " + extract(pieces, 32, 32) + "))";
    case Type::IntegerTyID:
        if (type->getIntegerBitWidth() <= 32)
            return low;
        require("GL_ARB_gpu_shader_int64");
        return "packUint2x32(uvec2(" + low + ", " + extract(pieces, 32, type->getIntegerBitWidth() - 32) + "))";
    default:
        throw Unsupported("a value of this type cannot be made from bits");
    }
}

// The value of a type from its bits.
std::string Emitter::fromPieces(const std::vector<Piece> &pieces, Type *type)
{
    auto *vector = dyn_cast<FixedVectorType>(type);
    if (!vector)
        return scalarOf(pieces, type);
    Type *scalar = vector->getElementType();
    const unsigned size = bits(scalar);
    std::vector<std::string> elements;
    for (unsigned i = 0; i < vector->getNumElements(); ++i) {
        std::vector<Piece> part;
        part.push_back({extract(pieces, i * size, std::min(size, 32u)), std::min(size, 32u)});
        if (size > 32)
            part.push_back({extract(pieces, i * size + 32, size - 32), size - 32});
        elements.push_back(scalarOf(part, scalar));
    }
    if (elements.size() == 1)
        return elements[0];
    std::string text = typeName(type) + "(";
    for (size_t i = 0; i < elements.size(); ++i)
        text += (i ? ", " : "") + elements[i];
    return text + ")";
}

// The word of a root's array that holds an address with an offset added, as a GLSL index.
std::string Emitter::wordIndex(const Address &address, uint64_t, uint64_t offset)
{
    const uint64_t constant = uint32_t(address.constant + offset);
    if (address.offset.empty())
        return std::to_string(constant >> 2) + "u";
    if (constant == 0)
        return "(" + address.offset + " >> 2)";
    return "((" + address.offset + " + " + std::to_string(constant) + "u) >> 2)";
}

std::string Emitter::atomicWord(Root &root, const Address &address, uint64_t alignment)
{
    if (alignmentOf(address, alignment) < 4)
        throw Unsupported("an atomic access that is not word aligned");
    if (root.kind == Root::Constant || root.kind == Root::Texture || root.kind == Root::Sampler || root.readOnly)
        throw Unsupported("an atomic access to read-only memory");
    root.used = true;
    return root.name + "[" + wordIndex(address, 4, 0) + "]";
}

void Emitter::storeValue(Root &root, const Address &address, uint64_t alignment, Type *type, const std::string &value)
{
    auto alignmentAt = [&](uint64_t offset) {
        const Address at{address.offset, address.constant + int64_t(offset)};
        if (address.offset.empty())
            return alignmentOf(at, alignment);
        return offset ? std::min<uint64_t>(alignment, offset & -offset) : alignment;
    };
    std::function<void(Type *, uint64_t, const std::string &)> store = [&](Type *type, uint64_t offset, const std::string &value) {
        if (auto *structure = dyn_cast<StructType>(type)) {
            const StructLayout *layout = m_layout.getStructLayout(structure);
            for (unsigned i = 0; i < structure->getNumElements(); ++i)
                store(structure->getElementType(i), offset + layout->getElementOffset(i), value + ".m" + std::to_string(i));
            return;
        }
        if (auto *array = dyn_cast<ArrayType>(type)) {
            const uint64_t stride = m_layout.getTypeAllocSize(array->getElementType());
            for (uint64_t i = 0; i < array->getNumElements(); ++i)
                store(array->getElementType(), offset + i * stride, value + "[" + std::to_string(i) + "]");
            return;
        }
        if (scalar(type)->isIntegerTy(1) && type->isVectorTy())
            throw Unsupported("vectors of bools in memory are not supported");
        std::vector<Piece> pieces = piecesOf(value, type);
        // A bool takes a byte in memory.
        if (type->isIntegerTy(1))
            pieces[0].bits = 8;
        const Address at{address.offset, address.constant + int64_t(offset)};
        storePieces(root, at, alignmentAt(offset), pieces);
    };
    // The parts of an aggregate made by a call or a constructor are read many times: the value gets a variable first.
    std::string text = value;
    if ((type->isAggregateType() || type->isVectorTy()) && text.find('(') != std::string::npos) {
        const std::string name = "reef_k" + std::to_string(m_nextVariable++);
        m_variables.emplace_back(typeName(type), name);
        line(name + " = " + text + ";");
        text = name;
    }
    store(type, 0, text);
}

static const int compareFunctions[16] = {-1, 1, 3, 4, 6, 2, 5, 7, 0, -1, -1, -1, -1, -1, -1, -1};
static const int addressModes[8] = {4, 0, 5, 2, 3, 1, 0, 0};

// The value of a half of a constant sampler's state.
static double halfValue(unsigned bits)
{
    const unsigned exponent = (bits >> 10) & 0x1f;
    const unsigned mantissa = bits & 0x3ff;
    double value;
    if (exponent == 0)
        value = std::ldexp(double(mantissa), -24);
    else if (exponent == 31)
        value = mantissa ? NAN : INFINITY;
    else
        value = std::ldexp(double(mantissa | 0x400), int(exponent) - 25);
    return (bits >> 15) ? -value : value;
}

Root *Emitter::globalRoot(GlobalVariable *global)
{
    auto root = std::make_unique<Root>();
    root->index = int(m_roots.size());
    root->global = global;
    // Constant samplers are globals that air.sampler_states names, and the frontends name them so.
    bool sampler = global->getName().starts_with("__air_sampler_state");
    if (const NamedMDNode *states = m_module.getNamedMetadata("air.sampler_states")) {
        for (unsigned i = 0; i < states->getNumOperands(); ++i) {
            for (const MDOperand &operand : states->getOperand(i)->operands()) {
                if (auto *value = dyn_cast_or_null<ValueAsMetadata>(operand.get()))
                    sampler |= value->getValue()->stripPointerCasts() == global;
            }
        }
    }
    const uint64_t size = m_layout.getTypeAllocSize(global->getValueType());
    if (sampler) {
        Constant *state = global->isDeclaration() ? nullptr : global->getInitializer();
        if (!state)
            throw Unsupported("a constant sampler without its state");
        if (auto *data = dyn_cast<ConstantDataArray>(state); data && data->getElementType()->isIntegerTy(64) && data->getNumElements()) {
            state = data->getElementAsConstant(0);
            if (!state)
                throw Unsupported("a constant sampler without its state");
        }
        auto *bits = dyn_cast<ConstantInt>(state);
        if (!bits)
            throw Unsupported("a constant sampler without its state");
        root->kind = Root::Sampler;
        root->sampler = int(m_shader.samplers.size());
        const uint64_t value = bits->getZExtValue();
        reef_mtl_constant_sampler sampler{};
        sampler.sAddress = addressModes[value & 7];
        sampler.tAddress = addressModes[(value >> 3) & 7];
        sampler.rAddress = addressModes[(value >> 6) & 7];
        sampler.minFilter = (value & 0x1800) != 0;
        sampler.magFilter = (value & 0x600) != 0;
        sampler.mipFilter = (value >> 13) & 3;
        sampler.normalized = !((value >> 15) & 1);
        sampler.compare = compareFunctions[(value >> 56) & 0xf];
        sampler.borderColor = (value >> 16) & 3;
        const unsigned anisotropy = (value >> 19) & 0x1f;
        sampler.maxAnisotropy = anisotropy ? anisotropy : 1;
        sampler.lodMinClamp = halfValue((value >> 24) & 0xffff);
        // The largest half stands for no limit.
        sampler.lodMaxClamp = (value & 0xffff0000000000) == 0x7bff0000000000 ? FLT_MAX : halfValue((value >> 40) & 0xffff);
        m_shader.samplers.push_back(sampler);
    } else if (global->getAddressSpace() == 3) {
        root->kind = Root::Threadgroup;
        root->name = "reef_g" + std::to_string(root->index);
        root->size = std::max<uint64_t>((size + 3) / 4, 1);
    } else if (global->isConstant() && !global->isDeclaration()) {
        root->kind = Root::Constant;
        root->name = "reef_c" + std::to_string(root->index);
        root->readOnly = true;
        root->size = std::max<uint64_t>((size + 3) / 4, 1);
    } else {
        root->kind = Root::Private;
        root->name = "reef_g" + std::to_string(root->index);
        root->size = std::max<uint64_t>((size + 3) / 4, 1);
    }
    Root *result = root.get();
    m_roots.push_back(std::move(root));
    return result;
}

// The bits of memory at an address, in pieces of up to a word.
std::vector<Piece> Emitter::loadPieces(Root &root, const Address &address, uint64_t alignment, uint64_t size)
{
    std::vector<Piece> pieces;
    root.used = true;
    if (address.offset.empty()) {
        for (uint64_t done = 0; done < size;) {
            const unsigned at = unsigned(address.constant + done);
            const unsigned shift = (at & 3) * 8;
            const uint64_t count = std::min<uint64_t>(4 - (at & 3), size - done);
            std::string text = root.name + "[" + std::to_string(at >> 2) + "u]";
            if (shift)
                text = "(" + text + " >> " + std::to_string(shift) + "u)";
            if (count < 4)
                text = "(" + text + " & " + mask(unsigned(count) * 8) + ")";
            pieces.push_back({text, unsigned(count) * 8});
            done += count;
        }
        return pieces;
    }
    if (alignment >= 4) {
        for (uint64_t done = 0; done < size;) {
            const uint64_t count = std::min<uint64_t>(4, size - done);
            std::string text = root.name + "[" + wordIndex(address, alignment, done) + "]";
            if (count < 4)
                text = "(" + text + " & " + mask(unsigned(count) * 8) + ")";
            pieces.push_back({text, unsigned(count) * 8});
            done += count;
        }
        return pieces;
    }
    // Not aligned to a word: the functions put the value together from the bytes.
    const std::string suffix = suffixOf(root);
    helper("reef_ld_" + suffix, loadFunctions(root, suffix));
    for (uint64_t done = 0; done < size;) {
        const Address at{address.offset, address.constant + int64_t(done)};
        if (size - done >= 4) {
            pieces.push_back({"reef_ld32_" + suffix + "(" + offsetOf(at) + ")", 32});
            done += 4;
        } else if (size - done >= 2) {
            pieces.push_back({"reef_ld16_" + suffix + "(" + offsetOf(at) + ")", 16});
            done += 2;
        } else {
            pieces.push_back({"reef_ld8_" + suffix + "(" + offsetOf(at) + ")", 8});
            done += 1;
        }
    }
    return pieces;
}

// Writes the bits to memory: whole words where it can, the bytes of a word where it must.
void Emitter::storePieces(Root &root, const Address &address, uint64_t alignment, const std::vector<Piece> &pieces)
{
    root.used = true;
    uint64_t bits = 0;
    for (const Piece &piece : pieces)
        bits += piece.bits;
    const uint64_t size = (bits + 7) / 8;
    const bool direct = address.offset.empty() || alignment >= 4;
    // Other threads may write the rest of a word of shared memory meanwhile.
    const bool shared = root.kind == Root::Buffer || root.kind == Root::Threadgroup;
    if (direct) {
        for (uint64_t done = 0; done < size;) {
            std::string word;
            unsigned shift = 0;
            uint64_t count;
            if (address.offset.empty()) {
                const unsigned at = unsigned(address.constant + done);
                shift = (at & 3) * 8;
                count = std::min<uint64_t>(4 - (at & 3), size - done);
                word = root.name + "[" + std::to_string(at >> 2) + "u]";
            } else {
                count = std::min<uint64_t>(4, size - done);
                word = root.name + "[" + wordIndex(address, alignment, done) + "]";
            }
            const std::string value = extract(pieces, unsigned(done) * 8, unsigned(count) * 8);
            if (count == 4) {
                line(word + " = " + value + ";");
            } else {
                char mask[32];
                snprintf(mask, sizeof(mask), "0x%xu", unsigned(((uint64_t(1) << (count * 8)) - 1) << shift));
                const std::string placed = shift ? "(" + value + " << " + std::to_string(shift) + "u)" : value;
                if (shared) {
                    line("atomicAnd(" + word + ", ~" + std::string(mask) + ");");
                    line("atomicOr(" + word + ", " + placed + ");");
                } else {
                    line(word + " = (" + word + " & ~" + mask + ") | " + placed + ";");
                }
            }
            done += count;
        }
        return;
    }
    const std::string suffix = suffixOf(root);
    helper("reef_ld_" + suffix, loadFunctions(root, suffix));
    helper("reef_st_" + suffix, storeFunctions(root, suffix));
    for (uint64_t done = 0; done < size;) {
        const Address at{address.offset, address.constant + int64_t(done)};
        const uint64_t count = size - done >= 4 ? 4 : size - done >= 2 ? 2 : 1;
        const std::string value = extract(pieces, unsigned(done) * 8, unsigned(count) * 8);
        line("reef_st" + std::to_string(count * 8) + "_" + suffix + "(" + offsetOf(at) + ", " + value + ");");
        done += count;
    }
}

std::string Emitter::atomic(Root &root, const Address &address, uint64_t alignment, const std::string &operation, Type *type,
                            const std::string &value, const std::string &expected, bool isSigned, bool isFloat)
{
    if (bits(type) != 32)
        throw Unsupported("atomics on values other than 32 bits are not supported");
    const std::string word = atomicWord(root, address, alignment);
    // GLSL has atomics of int and uint only: a float travels as its bits.
    const std::string bits = isFloat ? "floatBitsToUint(" + value + ")" : value;
    auto result = [&](const std::string &old) { return isFloat ? "uintBitsToFloat(" + old + ")" : old; };
    // The new value from the old one: for a loop of atomicCompSwap, or for memory no other thread sees.
    std::string update;
    const bool shared = root.kind == Root::Buffer || root.kind == Root::Threadgroup;
    if (isFloat) {
        const std::string old = "uintBitsToFloat(old)";
        const std::string v = "uintBitsToFloat(v)";
        if (operation == "add")
            update = "floatBitsToUint(" + old + " + " + v + ")";
        else if (operation == "sub")
            update = "floatBitsToUint(" + old + " - " + v + ")";
        else if (operation == "min")
            update = "floatBitsToUint(min(" + old + ", " + v + "))";
        else if (operation == "max")
            update = "floatBitsToUint(max(" + old + ", " + v + "))";
        else if (operation == "xchg")
            update = "v";
    } else if (operation == "add") {
        update = "old + v";
    } else if (operation == "sub") {
        update = "old - v";
    } else if (operation == "and") {
        update = "old & v";
    } else if (operation == "or") {
        update = "old | v";
    } else if (operation == "xor") {
        update = "old ^ v";
    } else if (operation == "xchg" || operation == "store") {
        update = "v";
    } else if (operation == "load") {
        update = "old";
    } else if (operation == "min") {
        update = isSigned ? "uint(min(int(old), int(v)))" : "min(old, v)";
    } else if (operation == "max") {
        update = isSigned ? "uint(max(int(old), int(v)))" : "max(old, v)";
    }
    if (shared) {
        if (!isFloat) {
            if (operation == "add")
                return "atomicAdd(" + word + ", " + bits + ")";
            if (operation == "sub")
                return "atomicAdd(" + word + ", 0u - " + bits + ")";
            if (operation == "and")
                return "atomicAnd(" + word + ", " + bits + ")";
            if (operation == "or")
                return "atomicOr(" + word + ", " + bits + ")";
            if (operation == "xor")
                return "atomicXor(" + word + ", " + bits + ")";
        }
        if (operation == "xchg" || operation == "store")
            return result("atomicExchange(" + word + ", " + bits + ")");
        if (operation == "load")
            return result("atomicOr(" + word + ", 0u)");
        if (operation == "cmpxchg") {
            if (isFloat)
                return result("atomicCompSwap(" + word + ", floatBitsToUint(" + expected + "), " + bits + ")");
            return "atomicCompSwap(" + word + ", " + expected + ", " + bits + ")";
        }
        if (!isFloat && (operation == "min" || operation == "max") && !isSigned)
            return std::string(operation == "min" ? "atomicMin(" : "atomicMax(") + word + ", " + bits + ")";
        // The rest is a loop of atomicCompSwap in a function of its own.
        if (update.empty())
            throw Unsupported("an unsupported atomic operation");
        const std::string suffix = suffixOf(root);
        const std::string name = "reef_atomic_" + std::string(isFloat ? "f" : isSigned ? "s" : "u") + operation + "_" + suffix;
        helper(name, "uint " + name + "(uint i, uint v) { uint old = " + root.name
                         + "[i]; while (true) { uint seen = atomicCompSwap(" + root.name + "[i], old, " + update
                         + "); if (seen == old) return old; old = seen; } }\n");
        return result(name + "(" + wordIndex(address, 4, 0) + ", " + bits + ")");
    }
    // Memory of this thread alone: a plain read and write.
    const std::string old = "reef_k" + std::to_string(m_nextVariable++);
    m_variables.emplace_back("uint", old);
    line(old + " = " + word + ";");
    if (operation == "cmpxchg") {
        line("if (" + old + " == " + expected + ") " + word + " = " + bits + ";");
    } else {
        if (update.empty())
            throw Unsupported("an unsupported atomic operation");
        std::string text = update;
        // Only whole words: "v" in "uintBitsToFloat(v)", not the v of "old".
        auto replace = [&](const std::string &name, const std::string &with) {
            size_t at = 0;
            while ((at = text.find(name, at)) != std::string::npos) {
                if ((at > 0 && isalnum(static_cast<unsigned char>(text[at - 1])))
                    || (at + name.size() < text.size() && isalnum(static_cast<unsigned char>(text[at + name.size()])))) {
                    at += name.size();
                    continue;
                }
                text.replace(at, name.size(), with);
                at += with.size();
            }
        };
        replace("old", old);
        replace("v", "(" + bits + ")");
        line(word + " = " + text + ";");
    }
    return result(old);
}

// What a pointer points into. Every pointer of the function has one root, found through its definition.
Root &Emitter::rootOf(Value *pointer)
{
    const auto known = m_pointerRoots.find(pointer);
    if (known != m_pointerRoots.end()) {
        // A phi that is being looked at is in the map without a root.
        if (!known->second)
            throw Unsupported("a pointer that only points at itself");
        return *known->second;
    }
    Root *root = nullptr;
    if (auto *argument = dyn_cast<llvm::Argument>(pointer)) {
        const auto found = m_argumentRoots.find(argument);
        if (found == m_argumentRoots.end())
            throw Unsupported("a pointer argument without a known role");
        root = found->second;
    } else if (auto *global = dyn_cast<GlobalVariable>(pointer)) {
        root = globalRoot(global);
    } else if (auto *local = dyn_cast<AllocaInst>(pointer)) {
        auto *count = dyn_cast<ConstantInt>(local->getArraySize());
        if (!count)
            throw Unsupported("stack memory of a size known only at run time");
        auto created = std::make_unique<Root>();
        created->kind = Root::Private;
        created->index = int(m_roots.size());
        created->name = "reef_p" + std::to_string(created->index);
        const uint64_t size = m_layout.getTypeAllocSize(local->getAllocatedType()) * count->getZExtValue();
        created->size = std::max<uint64_t>((size + 3) / 4, 1);
        root = created.get();
        m_roots.push_back(std::move(created));
    } else if (auto *gep = dyn_cast<GEPOperator>(pointer)) {
        root = &rootOf(gep->getPointerOperand());
    } else if (Operator::getOpcode(pointer) == Instruction::BitCast || Operator::getOpcode(pointer) == Instruction::AddrSpaceCast
               || Operator::getOpcode(pointer) == Instruction::Freeze) {
        root = &rootOf(llvm::cast<User>(pointer)->getOperand(0));
    } else if (auto *load = dyn_cast<LoadInst>(pointer)) {
        root = loadedRoot(load);
    } else if (auto *call = dyn_cast<CallBase>(pointer); call && call->getCalledFunction()
                                                             && call->getCalledFunction()->getName() == "air.get_read_sampler") {
        auto created = std::make_unique<Root>();
        created->kind = Root::ReadSampler;
        created->index = int(m_roots.size());
        root = created.get();
        m_roots.push_back(std::move(created));
    } else if (auto *phi = dyn_cast<PHINode>(pointer)) {
        m_pointerRoots[pointer] = nullptr;
        for (Value *incoming : phi->incoming_values()) {
            if (isa<UndefValue>(incoming))
                continue;
            // The value that comes back around a loop says nothing new.
            const auto seen = m_pointerRoots.find(incoming);
            if (seen != m_pointerRoots.end() && !seen->second)
                continue;
            Root *other = &rootOf(incoming);
            if (root && other != root)
                throw Unsupported("a pointer that may point into two different buffers");
            root = other;
        }
        if (!root)
            throw Unsupported("a pointer with no known source");
    } else if (auto *select = dyn_cast<SelectInst>(pointer)) {
        root = &rootOf(select->getTrueValue());
        if (&rootOf(select->getFalseValue()) != root)
            throw Unsupported("a pointer that may point into two different buffers");
    } else {
        throw Unsupported("a pointer of unknown provenance");
    }
    m_pointerRoots[pointer] = root;
    return *root;
}

// Where a pointer points inside its root.
Address Emitter::addressOf(Value *pointer)
{
    const auto known = m_addresses.find(pointer);
    if (known != m_addresses.end())
        return known->second;
    Address address;
    if (isa<llvm::Argument>(pointer) || isa<GlobalVariable>(pointer) || isa<AllocaInst>(pointer) || isa<ConstantPointerNull>(pointer)
        || isa<UndefValue>(pointer)) {
        address = Address();
    } else if (auto *gep = dyn_cast<GetElementPtrInst>(pointer)) {
        address = addressOf(gep->getPointerOperand());
        // The indices known only at run time become one sum, in a variable of the GEP.
        std::vector<std::string> terms;
        for (auto type = gep_type_begin(gep), end = gep_type_end(gep); type != end; ++type) {
            Value *index = type.getOperand();
            if (StructType *structure = type.getStructTypeOrNull()) {
                address.constant += m_layout.getStructLayout(structure)->getElementOffset(unsigned(llvm::cast<ConstantInt>(index)->getZExtValue()));
                continue;
            }
            const uint64_t stride = type.getSequentialElementStride(m_layout);
            if (auto *constant = dyn_cast<ConstantInt>(index)) {
                address.constant += int64_t(stride * constant->getSExtValue());
                continue;
            }
            if (index->getType()->isVectorTy())
                throw Unsupported("vectors of pointers are not supported");
            // An index widened to 64 bits is used as it was.
            Value *source = index;
            if ((isa<ZExtInst>(index) || isa<SExtInst>(index)) && llvm::cast<CastInst>(index)->getOperand(0)->getType()->getIntegerBitWidth() <= 32)
                source = llvm::cast<CastInst>(index)->getOperand(0);
            const unsigned width = source->getType()->getIntegerBitWidth();
            std::string text = valueOf(source);
            if (width > 32) {
                text = "uint(" + text + ")";
            } else if (width != 32) {
                if (source == index || isa<SExtInst>(index))
                    text = "uint(" + asSigned(text, source->getType()) + ")";
                if (width == 1)
                    text = "uint(" + text + ")";
            }
            if (stride != 1)
                text = "(" + text + " * " + std::to_string(unsigned(stride)) + "u)";
            terms.push_back(text);
        }
        if (!terms.empty()) {
            std::string sum = address.offset;
            for (const std::string &term : terms)
                sum += (sum.empty() ? "" : " + ") + term;
            const std::string name = nameOf(pointer);
            line(name + " = " + sum + ";");
            address.offset = name;
        }
    } else if (Operator::getOpcode(pointer) == Instruction::BitCast || Operator::getOpcode(pointer) == Instruction::AddrSpaceCast
               || Operator::getOpcode(pointer) == Instruction::Freeze) {
        address = addressOf(llvm::cast<User>(pointer)->getOperand(0));
    } else if (isa<PHINode>(pointer) || isa<SelectInst>(pointer)) {
        // The variable of the phi or the select holds the offset.
        address = Address{nameOf(pointer), 0};
    } else if (isa<LoadInst>(pointer) && rootOf(pointer).argument && rootOf(pointer).argument->argumentBuffer >= 0) {
        // A resource of an argument buffer is a root of its own.
        address = Address();
    } else {
        throw Unsupported("a pointer of unknown provenance");
    }
    m_addresses[pointer] = address;
    return address;
}

// llvm.memcpy, llvm.memmove and llvm.memset: a loop over words when both sides are aligned to words, else over bytes.
void Emitter::copyMemory(CallInst &call, Intrinsic::ID id)
{
    Value *target = call.getArgOperand(0);
    Root &to = rootOf(target);
    if (to.kind == Root::Texture || to.kind == Root::Sampler || to.readOnly)
        throw Unsupported("a copy into read-only memory");
    const Address toAddress = addressOf(target);
    const uint64_t toAlignment = call.getParamAlign(0).valueOrOne().value();
    Value *length = call.getArgOperand(2);
    const std::string counter = "reef_i" + std::to_string(m_nextVariable++);
    const std::string lengthText = valueOf(length);
    const std::string size = length->getType()->getIntegerBitWidth() > 32 ? "uint(" + lengthText + ")" : lengthText;
    to.used = true;
    const std::string toSuffix = suffixOf(to);
    Value *source = call.getArgOperand(1);
    if (id == Intrinsic::memset) {
        const std::string byte = valueOf(source);
        auto *constant = dyn_cast<ConstantInt>(length);
        if (constant && alignmentOf(toAddress, toAlignment) >= 4 && constant->getZExtValue() % 4 == 0) {
            line("for (uint " + counter + " = 0u; " + counter + " < " + std::to_string(constant->getZExtValue() / 4) + "u; ++" + counter + ")");
            line("    " + to.name + "[" + wordIndex(toAddress, 4, 0) + " + " + counter + "] = " + byte + " * 0x01010101u;");
        } else {
            helper("reef_ld_" + toSuffix, loadFunctions(to, toSuffix));
            helper("reef_st_" + toSuffix, storeFunctions(to, toSuffix));
            line("for (uint " + counter + " = 0u; " + counter + " < " + size + "; ++" + counter + ")");
            line("    reef_st8_" + toSuffix + "(" + offsetOf(toAddress) + " + " + counter + ", " + byte + ");");
        }
        return;
    }
    Root &from = rootOf(source);
    if (from.kind == Root::Texture || from.kind == Root::Sampler)
        throw Unsupported("a copy from a texture or sampler");
    from.used = true;
    const Address fromAddress = addressOf(source);
    const uint64_t fromAlignment = call.getParamAlign(1).valueOrOne().value();
    const std::string fromSuffix = suffixOf(from);
    // A move inside one root may have to copy from the end.
    const bool overlap = &to == &from && id == Intrinsic::memmove;
    auto *constant = dyn_cast<ConstantInt>(length);
    if (constant && !overlap && alignmentOf(toAddress, toAlignment) >= 4 && alignmentOf(fromAddress, fromAlignment) >= 4
        && constant->getZExtValue() % 4 == 0) {
        line("for (uint " + counter + " = 0u; " + counter + " < " + std::to_string(constant->getZExtValue() / 4) + "u; ++" + counter + ")");
        line("    " + to.name + "[" + wordIndex(toAddress, 4, 0) + " + " + counter + "] = " + from.name + "[" + wordIndex(fromAddress, 4, 0)
             + " + " + counter + "];");
        return;
    }
    helper("reef_ld_" + fromSuffix, loadFunctions(from, fromSuffix));
    helper("reef_ld_" + toSuffix, loadFunctions(to, toSuffix));
    helper("reef_st_" + toSuffix, storeFunctions(to, toSuffix));
    const std::string copy = "reef_st8_" + toSuffix + "(" + offsetOf(toAddress) + " + " + counter + ", reef_ld8_" + fromSuffix + "("
                             + offsetOf(fromAddress) + " + " + counter + "));";
    if (overlap) {
        line("if (" + offsetOf(toAddress) + " > " + offsetOf(fromAddress) + ") {");
        line("    for (uint " + counter + " = " + size + "; " + counter + "-- > 0u;)");
        line("        " + copy);
        line("} else {");
        line("    for (uint " + counter + " = 0u; " + counter + " < " + size + "; ++" + counter + ")");
        line("        " + copy);
        line("}");
    } else {
        line("for (uint " + counter + " = 0u; " + counter + " < " + size + "; ++" + counter + ")");
        line("    " + copy);
    }
}

}
