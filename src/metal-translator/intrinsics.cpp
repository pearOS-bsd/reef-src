// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Calls of the functions of AIR and LLVM: math, conversions, packing, SIMD groups, derivatives, atomics and barriers.

#include "emitter.h"

#include <cmath>
#include <cstdio>

using namespace llvm;

namespace cutefish::reef::metal {

namespace {

// The packing functions that GLSL has under another name.
struct PackFormat
{
    const char *name;
    const char *pack;
    const char *unpack;
};

const PackFormat packFormats[] = {
    {"unorm4x8", "packUnorm4x8", "unpackUnorm4x8"},
    {"snorm4x8", "packSnorm4x8", "unpackSnorm4x8"},
    {"unorm2x16", "packUnorm2x16", "unpackUnorm2x16"},
    {"snorm2x16", "packSnorm2x16", "unpackSnorm2x16"},
};

}

static std::vector<std::string> split(const std::string &text)
{
    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        const size_t end = text.find('.', start);
        parts.push_back(text.substr(start, end == std::string::npos ? std::string::npos : end - start));
        if (end == std::string::npos)
            break;
        start = end + 1;
    }
    return parts;
}

// Whether one of the parts behind the name of an intrinsic ("s", "f") is this one.
static bool hasPart(const std::vector<std::string> &parts, const char *part)
{
    for (size_t i = 1; i < parts.size(); ++i) {
        if (parts[i] == part)
            return true;
    }
    return false;
}

static std::string literal(double value)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.17g", value);
    std::string text = buffer;
    if (text.find_first_of(".e") == std::string::npos)
        text += ".0";
    return text[0] == '-' ? "(" + text + ")" : text;
}

void Emitter::requireClamp64()
{
    require("GL_ARB_gpu_shader_int64");
    helper("reef_clamp64",
           "int64_t reef_clamp64(int64_t x, int64_t low, int64_t high) { return x < low ? low : (x > high ? high : x); }\n");
}

std::string Emitter::convert(CallInst &call, const std::vector<std::string> &parts)
{
    if (parts.size() < 5)
        throw Unsupported("an unreadable conversion intrinsic");
    const bool saturate = parts[1] == "sat";
    const size_t first = saturate ? 2 : 1;
    const std::string to = parts[first];
    const std::string from = first + 2 < parts.size() ? parts[first + 2] : "";
    Type *type = call.getType();
    Type *source = call.getArgOperand(0)->getType();
    const std::string value = valueOf(call.getArgOperand(0));
    const unsigned count = width(type);
    const std::string suffix = count > 1 ? std::to_string(count) : "";
    const bool fromFloat = !isInteger(source);
    const bool toInteger = isInteger(type);
    const std::string floats = count > 1 ? "vec" + suffix : "float";

    if (!toInteger) {
        std::string result;
        if (fromFloat)
            result = typeName(type) + "(" + value + ")";
        else if (scalar(source)->isIntegerTy(1))
            result = typeName(type) + "(" + value + ")";
        else if (from == "s")
            result = typeName(type) + "(" + asSigned(value, source) + ")";
        else
            result = typeName(type) + "(" + value + ")";
        if (scalar(type)->isHalfTy() && !scalar(source)->isHalfTy()) {
            requireHalf();
            result = "reef_half(" + result + ")";
        }
        if (saturate)
            result = "clamp(" + result + ", 0.0, 1.0)";
        return result;
    }

    const unsigned size = bits(scalar(type));
    if (scalar(type)->isIntegerTy(1)) {
        if (fromFloat)
            return count > 1 ? "notEqual(" + value + ", " + floats + "(0.0))" : "(" + value + " != 0.0)";
        if (scalar(source)->isIntegerTy(1))
            return value;
        return count > 1 ? "notEqual(" + value + ", " + typeName(source) + "(0u))" : "(" + value + " != 0u)";
    }

    if (fromFloat) {
        const bool wide = size > 32;
        if (wide)
            require("GL_ARB_gpu_shader_int64");
        const bool isSigned = to == "s";
        const double low = isSigned ? -std::ldexp(1.0, size - 1) : 0.0;
        const double high = isSigned ? std::ldexp(1.0, size - 1) : std::ldexp(1.0, size);
        const uint64_t all = size >= 64 ? ~0ull : (1ull << size) - 1;
        const uint64_t maximum = isSigned ? (1ull << (size - 1)) - 1 : all;
        const uint64_t minimum = isSigned ? (~0ull << (size - 1)) & all : 0;
        Type *element = scalar(type);
        const std::string signedType = wide ? "int64_t" : "int";
        const std::string unsignedType = wide ? "uint64_t" : "uint";
        const std::string converted = isSigned ? masked(unsignedType + "(" + signedType + "($0))", element) : unsignedType + "($0)";
        return componentwise(type, {value},
                             "(isnan($0) ? " + integer(0, element) + " : $0 >= " + literal(high) + " ? " + integer(maximum, element)
                                 + " : $0 <= " + literal(low) + " ? " + integer(minimum, element) + " : " + converted + ")");
    }

    const unsigned sourceSize = bits(scalar(source));
    std::string converted = value;
    if (scalar(source)->isIntegerTy(1))
        converted = (count > 1 ? (size > 32 ? "u64vec" : "uvec") + suffix : size > 32 ? "uint64_t" : "uint") + "(" + value + ")";
    if (saturate && (size < sourceSize || from != to)) {
        if (from == "s") {
            const std::string number = asSigned(value, source);
            const std::string low = to == "s" ? std::to_string(-(int64_t(1) << (size - 1))) : "0";
            const std::string high = to == "s" ? std::to_string((int64_t(1) << (size - 1)) - 1)
                                               : std::to_string(size < 32 ? (int64_t(1) << size) - 1 : int64_t(0x7fffffff));
            const std::string end = sourceSize > 32 ? "L" : "";
            if (sourceSize > 32)
                requireClamp64();
            converted = fromSigned(std::string(sourceSize > 32 ? "reef_clamp64(" : "clamp(") + number + ", (" + low + end + "), " + high + end + ")",
                                   type);
        } else {
            const uint64_t maximum = to == "s" ? (1ull << (size - 1)) - 1 : size >= 64 ? ~0ull : (1ull << size) - 1;
            converted = typeName(type) + "(min(" + value + ", " + typeName(source) + "(" + integer(maximum, source) + ")))";
        }
        return masked(converted, type);
    }
    if (size > sourceSize && from == "s")
        return fromSigned(asSigned(value, source), type);
    return masked(typeName(type) + "(" + converted + ")", type);
}

bool Emitter::emitLlvmIntrinsic(CallInst &call, const std::string &target)
{
    Type *type = call.getType();
    std::vector<std::string> args;
    for (unsigned i = 0; i < call.arg_size(); ++i)
        args.push_back(valueOf(call.getArgOperand(i)));
    const unsigned size = isInteger(type) ? bits(scalar(type)) : 0;
    const std::string sizeText = std::to_string(size) + "u";
    const auto assign = [&](const std::string &value) {
        line(target + " = " + value + ";");
    };

    const Intrinsic::ID id = call.getCalledFunction()->getIntrinsicID();
    switch (id) {
    case Intrinsic::fabs:
        assign("abs(" + args[0] + ")");
        return true;
    case Intrinsic::floor:
        assign("floor(" + args[0] + ")");
        return true;
    case Intrinsic::ceil:
        assign("ceil(" + args[0] + ")");
        return true;
    case Intrinsic::trunc:
        assign("trunc(" + args[0] + ")");
        return true;
    case Intrinsic::rint:
    case Intrinsic::nearbyint:
    case Intrinsic::roundeven:
        assign("roundEven(" + args[0] + ")");
        return true;
    case Intrinsic::round:
        assign("(sign(" + args[0] + ") * floor(abs(" + args[0] + ") + 0.5))");
        return true;
    case Intrinsic::sqrt:
        assign("sqrt(" + args[0] + ")");
        return true;
    case Intrinsic::sin:
        assign("sin(" + args[0] + ")");
        return true;
    case Intrinsic::cos:
        assign("cos(" + args[0] + ")");
        return true;
    case Intrinsic::exp:
        assign("exp(" + args[0] + ")");
        return true;
    case Intrinsic::exp2:
        assign("exp2(" + args[0] + ")");
        return true;
    case Intrinsic::log:
        assign("log(" + args[0] + ")");
        return true;
    case Intrinsic::log2:
        assign("log2(" + args[0] + ")");
        return true;
    case Intrinsic::log10:
        assign("(log2(" + args[0] + ") * 0.30102999566)");
        return true;
    case Intrinsic::pow:
        assign("pow(" + args[0] + ", " + args[1] + ")");
        return true;
    case Intrinsic::minnum:
    case Intrinsic::minimum:
        assign("min(" + args[0] + ", " + args[1] + ")");
        return true;
    case Intrinsic::maxnum:
    case Intrinsic::maximum:
        assign("max(" + args[0] + ", " + args[1] + ")");
        return true;
    case Intrinsic::fma:
        assign("fma(" + args[0] + ", " + args[1] + ", " + args[2] + ")");
        return true;
    case Intrinsic::fmuladd:
        assign("(" + args[0] + " * " + args[1] + " + " + args[2] + ")");
        return true;
    case Intrinsic::copysign:
        assign(componentwise(type, args, "uintBitsToFloat((floatBitsToUint($0) & 0x7fffffffu) | (floatBitsToUint($1) & 0x80000000u))"));
        return true;
    case Intrinsic::umin:
        assign("min(" + args[0] + ", " + args[1] + ")");
        return true;
    case Intrinsic::umax:
        assign("max(" + args[0] + ", " + args[1] + ")");
        return true;
    case Intrinsic::smin:
        assign(fromSigned("min(" + asSigned(args[0], type) + ", " + asSigned(args[1], type) + ")", type));
        return true;
    case Intrinsic::smax:
        assign(fromSigned("max(" + asSigned(args[0], type) + ", " + asSigned(args[1], type) + ")", type));
        return true;
    case Intrinsic::abs:
        assign(fromSigned("abs(" + asSigned(args[0], type) + ")", type));
        return true;
    case Intrinsic::ctpop:
        if (size > 32)
            break;
        if (width(type) > 1)
            assign("uvec" + std::to_string(width(type)) + "(bitCount(" + args[0] + "))");
        else
            assign("uint(bitCount(" + args[0] + "))");
        return true;
    case Intrinsic::ctlz:
        if (size > 32)
            break;
        assign(componentwise(type, args, "($0 == 0u ? " + sizeText + " : " + std::to_string(size - 1) + "u - uint(findMSB($0)))"));
        return true;
    case Intrinsic::cttz:
        if (size > 32)
            break;
        assign(componentwise(type, args, "($0 == 0u ? " + sizeText + " : uint(findLSB($0)))"));
        return true;
    case Intrinsic::bitreverse:
        if (size > 32)
            break;
        if (size == 32)
            assign("bitfieldReverse(" + args[0] + ")");
        else
            assign("(bitfieldReverse(" + args[0] + ") >> " + std::to_string(32 - size) + "u)");
        return true;
    case Intrinsic::bswap:
        if (size == 16) {
            assign("(((" + args[0] + " >> 8u) | (" + args[0] + " << 8u)) & 0xffffu)");
            return true;
        }
        if (size == 32) {
            assign("((" + args[0] + " >> 24u) | ((" + args[0] + " >> 8u) & 0xff00u) | ((" + args[0] + " << 8u) & 0xff0000u) | (" + args[0]
                   + " << 24u))");
            return true;
        }
        break;
    case Intrinsic::fshl:
    case Intrinsic::fshr: {
        if (size > 32)
            break;
        const std::string shift = "($2 % " + sizeText + ")";
        const std::string pattern = id == Intrinsic::fshl
            ? "(" + shift + " == 0u ? $0 : (($0 << " + shift + ") | ($1 >> (" + sizeText + " - " + shift + "))) & " + mask(size) + ")"
            : "(" + shift + " == 0u ? $1 : (($1 >> " + shift + ") | ($0 << (" + sizeText + " - " + shift + "))) & " + mask(size) + ")";
        assign(componentwise(type, args, pattern));
        return true;
    }
    case Intrinsic::vector_reduce_fadd:
    case Intrinsic::vector_reduce_add:
    case Intrinsic::vector_reduce_fmul:
    case Intrinsic::vector_reduce_mul:
    case Intrinsic::vector_reduce_and:
    case Intrinsic::vector_reduce_or:
    case Intrinsic::vector_reduce_xor:
    case Intrinsic::vector_reduce_fmin:
    case Intrinsic::vector_reduce_umin:
    case Intrinsic::vector_reduce_fmax:
    case Intrinsic::vector_reduce_umax: {
        const bool hasStart = id == Intrinsic::vector_reduce_fadd || id == Intrinsic::vector_reduce_fmul;
        Value *vector = call.getArgOperand(hasStart ? 1 : 0);
        const std::string operand = args[hasStart ? 1 : 0];
        std::string result = hasStart ? args[0] : element(operand, vector->getType(), 0);
        if (scalar(vector->getType())->isIntegerTy(1))
            break;
        for (unsigned i = hasStart ? 0 : 1; i < width(vector->getType()); ++i) {
            const std::string next = element(operand, vector->getType(), i);
            switch (id) {
            case Intrinsic::vector_reduce_fadd:
            case Intrinsic::vector_reduce_add:
                result = "(" + result + " + " + next + ")";
                break;
            case Intrinsic::vector_reduce_fmul:
            case Intrinsic::vector_reduce_mul:
                result = "(" + result + " * " + next + ")";
                break;
            case Intrinsic::vector_reduce_and:
                result = "(" + result + " & " + next + ")";
                break;
            case Intrinsic::vector_reduce_or:
                result = "(" + result + " | " + next + ")";
                break;
            case Intrinsic::vector_reduce_xor:
                result = "(" + result + " ^ " + next + ")";
                break;
            case Intrinsic::vector_reduce_fmin:
            case Intrinsic::vector_reduce_umin:
                result = "min(" + result + ", " + next + ")";
                break;
            default:
                result = "max(" + result + ", " + next + ")";
                break;
            }
        }
        assign(isInteger(type) ? masked(result, type) : result);
        return true;
    }
    default:
        break;
    }
    return false;
}

std::string Emitter::mathIntrinsic(CallInst &call, const std::string &name, const std::vector<std::string> &parts, bool &handled)
{
    Type *type = call.getType();
    handled = true;
    std::vector<std::string> args;
    for (unsigned i = 0; i < call.arg_size(); ++i)
        args.push_back(valueOf(call.getArgOperand(i)));
    const auto expect = [&](size_t count) {
        if (args.size() < count)
            throw Unsupported("the intrinsic air." + name + " has too few arguments");
    };
    const auto unary = [&](const std::string &function) {
        expect(1);
        return function + "(" + args[0] + ")";
    };
    const auto binary = [&](const std::string &function) {
        expect(2);
        return function + "(" + args[0] + ", " + args[1] + ")";
    };
    const auto ternary = [&](const std::string &function) {
        expect(3);
        return function + "(" + args[0] + ", " + args[1] + ", " + args[2] + ")";
    };

    if (isInteger(type) && !scalar(type)->isIntegerTy(1) && name.find("pack") == std::string::npos) {
        const bool isSigned = hasPart(parts, "s");
        Type *operandType = call.arg_size() ? call.getArgOperand(0)->getType() : type;
        const auto argument = [&](size_t index) {
            return isSigned ? asSigned(args[index], operandType) : args[index];
        };
        const auto result = [&](const std::string &value) {
            return isSigned ? fromSigned(value, type) : masked(value, type);
        };
        const unsigned size = bits(scalar(type));
        const std::string sizeText = std::to_string(size) + "u";

        if (name == "abs") {
            expect(1);
            return isSigned ? fromSigned("abs(" + argument(0) + ")", type) : args[0];
        }
        if (name == "min" || name == "max") {
            expect(2);
            return result(name + "(" + argument(0) + ", " + argument(1) + ")");
        }
        if (name == "clamp") {
            expect(3);
            return result("clamp(" + argument(0) + ", " + argument(1) + ", " + argument(2) + ")");
        }
        if (name == "absdiff") {
            expect(2);
            if (!isSigned)
                return "(max(" + args[0] + ", " + args[1] + ") - min(" + args[0] + ", " + args[1] + "))";
            return masked(componentwise(type, {argument(0), argument(1)}, "uint(max($0, $1) - min($0, $1))"), type);
        }
        if (name == "clz") {
            expect(1);
            return componentwise(type, args, "($0 == 0u ? " + sizeText + " : " + std::to_string(size - 1) + "u - uint(findMSB($0)))");
        }
        if (name == "ctz") {
            expect(1);
            return componentwise(type, args, "($0 == 0u ? " + sizeText + " : uint(findLSB($0)))");
        }
        if (name == "popcount") {
            expect(1);
            return width(type) > 1 ? "uvec" + std::to_string(width(type)) + "(bitCount(" + args[0] + "))" : "uint(bitCount(" + args[0] + "))";
        }
        if (name == "reverse_bits") {
            expect(1);
            return size == 32 ? "bitfieldReverse(" + args[0] + ")" : "(bitfieldReverse(" + args[0] + ") >> " + std::to_string(32 - size) + "u)";
        }
        if (name == "rotate") {
            expect(2);
            return componentwise(type, args,
                                 "((($0 << ($1 % " + sizeText + ")) | ($0 >> ((" + sizeText + " - ($1 % " + sizeText + ")) % " + sizeText + "))) & "
                                     + mask(size) + ")");
        }
        if (name == "mul_hi") {
            expect(2);
            if (size == 32) {
                if (isSigned)
                    return componentwise(type, {argument(0), argument(1)}, "uint(int((int64_t($0) * int64_t($1)) >> 32))");
                require("GL_ARB_gpu_shader_int64");
                return componentwise(type, args, "uint((uint64_t($0) * uint64_t($1)) >> 32u)");
            }
            if (isSigned)
                return result(componentwise(type, {argument(0), argument(1)}, "(($0 * $1) >> " + std::to_string(size) + ")"));
            return componentwise(type, args, "(($0 * $1) >> " + sizeText + ")");
        }
        if (name == "add_sat" || name == "sub_sat") {
            expect(2);
            requireClamp64();
            const std::string operation = name == "add_sat" ? " + " : " - ";
            if (isSigned) {
                const std::string low = "(" + std::to_string(-(int64_t(1) << (size - 1))) + "L)";
                const std::string high = std::to_string((int64_t(1) << (size - 1)) - 1) + "L";
                return result(componentwise(type, {argument(0), argument(1)},
                                            "int(reef_clamp64(int64_t($0)" + operation + "int64_t($1), int64_t(" + low + "), int64_t(" + high + ")))"));
            }
            const std::string high = std::to_string(size == 32 ? 0xffffffffull : (1ull << size) - 1);
            return componentwise(type, args, "uint(reef_clamp64(int64_t($0)" + operation + "int64_t($1), int64_t(0), int64_t(" + high + "ul)))");
        }
        if (name == "hadd" || name == "rhadd") {
            expect(2);
            require("GL_ARB_gpu_shader_int64");
            const bool round = name == "rhadd";
            if (isSigned)
                return result(componentwise(type, {argument(0), argument(1)},
                                            "int((int64_t($0) + int64_t($1)" + std::string(round ? " + 1" : "") + ") >> 1)"));
            return componentwise(type, args, "uint((uint64_t($0) + uint64_t($1)" + std::string(round ? " + 1ul" : "") + ") >> 1u)");
        }
        if (name == "madhi" || name == "mad_hi") {
            expect(3);
            require("GL_ARB_gpu_shader_int64");
            if (isSigned)
                return result(componentwise(type, {argument(0), argument(1), argument(2)}, "(int((int64_t($0) * int64_t($1)) >> 32) + $2)"));
            return masked(componentwise(type, args, "(uint((uint64_t($0) * uint64_t($1)) >> 32u) + $2)"), type);
        }
        if (name == "extract_bits") {
            expect(3);
            return result("bitfieldExtract(" + argument(0) + ", int(" + args[1] + "), int(" + args[2] + "))");
        }
        if (name == "insert_bits") {
            expect(4);
            return masked("bitfieldInsert(" + args[0] + ", " + args[1] + ", int(" + args[2] + "), int(" + args[3] + "))", type);
        }
        if (name == "mad_sat" || name == "mul24" || name == "mad24") {
            requireClamp64();
            if (name == "mul24") {
                expect(2);
                return result("(" + argument(0) + " * " + argument(1) + ")");
            }
            expect(3);
            if (name == "mad24")
                return result("(" + argument(0) + " * " + argument(1) + " + " + argument(2) + ")");
            const std::string low = isSigned ? "(" + std::to_string(-(int64_t(1) << (size - 1))) + "L)" : "0";
            const std::string high = isSigned ? std::to_string((int64_t(1) << (size - 1)) - 1) + "L"
                                              : std::to_string(size == 32 ? 0xffffffffull : (1ull << size) - 1);
            if (isSigned)
                return result(componentwise(type, {argument(0), argument(1), argument(2)},
                                            "int(reef_clamp64(int64_t($0) * int64_t($1) + int64_t($2), int64_t(" + low + "), int64_t(" + high + ")))"));
            return componentwise(type, args, "uint(reef_clamp64(int64_t($0) * int64_t($1) + int64_t($2), int64_t(0), int64_t(" + high + "ul)))");
        }
        if (name.rfind("simd_", 0) != 0 && name.rfind("quad_", 0) != 0) {
            handled = false;
            return std::string();
        }
    }

    if (name.find("pack") != std::string::npos) {
        const bool pack = name.rfind("unpack", 0) != 0;
        std::string joined;
        for (const std::string &part : parts)
            joined += part + ".";
        for (const PackFormat &format : packFormats) {
            if (joined.find(format.name) == std::string::npos)
                continue;
            expect(1);
            if (!pack)
                return typeName(type) + "(" + format.unpack + "(" + args[0] + "))";
            const unsigned count = width(call.getArgOperand(0)->getType());
            const std::string value = count == 2 || count == 4 ? "vec" + std::to_string(count) + "(" + args[0] + ")" : args[0];
            return std::string(format.pack) + "(" + value + ")";
        }
        if (joined.find("half2") != std::string::npos) {
            expect(1);
            if (!pack)
                return typeName(type) + "(unpackHalf2x16(" + args[0] + "))";
            requireHalf();
            return "(reef_half_bits(" + args[0] + ".x) | (reef_half_bits(" + args[0] + ".y) << 16))";
        }
        if (joined.find("unorm10a2") != std::string::npos || joined.find("unorm1010102") != std::string::npos) {
            expect(1);
            if (!pack)
                return componentwise(type, {args[0]},
                                     "vec4(float($0 & 1023u), float(($0 >> 10u) & 1023u), float(($0 >> 20u) & 1023u), 0.0) / 1023.0");
            return "(uint(round(clamp(" + args[0] + ".x, 0.0, 1.0) * 1023.0)) | (uint(round(clamp(" + args[0]
                + ".y, 0.0, 1.0) * 1023.0)) << 10u) | (uint(round(clamp(" + args[0] + ".z, 0.0, 1.0) * 1023.0)) << 20u) | (uint(round(clamp("
                + args[0] + ".w, 0.0, 1.0) * 3.0)) << 30u))";
        }
    }

    if (name == "all" || name == "any") {
        expect(1);
        return width(call.getArgOperand(0)->getType()) == 1 ? args[0] : name + "(" + args[0] + ")";
    }
    if (name == "isnan" || name == "isinf")
        return unary(name);
    if (name == "isfinite") {
        if (width(type) > 1)
            return "not(bvec" + std::to_string(width(type)) + "(uvec" + std::to_string(width(type)) + "(isinf(" + args[0] + ")) | uvec"
                + std::to_string(width(type)) + "(isnan(" + args[0] + "))))";
        return "!(isinf(" + args[0] + ") || isnan(" + args[0] + "))";
    }
    if (name == "signbit")
        return componentwise(type, args, "((floatBitsToUint($0) >> 31u) != 0u)");
    if (name == "isnormal")
        return componentwise(type, args, "(((floatBitsToUint($0) >> 23u) & 0xffu) != 0u && ((floatBitsToUint($0) >> 23u) & 0xffu) != 0xffu)");

    static const std::map<std::string, std::string> functions = {
        {"fabs", "abs"},
        {"abs", "abs"},
        {"ceil", "ceil"},
        {"floor", "floor"},
        {"trunc", "trunc"},
        {"rint", "roundEven"},
        {"sqrt", "sqrt"},
        {"rsqrt", "inversesqrt"},
        {"exp", "exp"},
        {"exp2", "exp2"},
        {"log", "log"},
        {"log2", "log2"},
        {"sin", "sin"},
        {"cos", "cos"},
        {"tan", "tan"},
        {"asin", "asin"},
        {"acos", "acos"},
        {"atan", "atan"},
        {"sinh", "sinh"},
        {"cosh", "cosh"},
        {"tanh", "tanh"},
        {"asinh", "asinh"},
        {"acosh", "acosh"},
        {"atanh", "atanh"},
        {"sign", "sign"},
        {"normalize", "normalize"},
        {"length", "length"},
    };
    const auto function = functions.find(name);
    if (function != functions.end())
        return unary(function->second);

    if (name == "round") {
        expect(1);
        return "(sign(" + args[0] + ") * floor(abs(" + args[0] + ") + 0.5))";
    }
    if (name == "fract") {
        expect(1);
        return "min(fract(" + args[0] + "), 0.99999994)";
    }
    if (name == "saturate") {
        expect(1);
        return "clamp(" + args[0] + ", 0.0, 1.0)";
    }
    if (name == "exp10") {
        expect(1);
        return "exp2(" + args[0] + " * 3.32192809489)";
    }
    if (name == "log10") {
        expect(1);
        return "(log2(" + args[0] + ") * 0.30102999566)";
    }
    if (name == "sinpi" || name == "cospi" || name == "tanpi") {
        expect(1);
        return name.substr(0, 3) + "(" + args[0] + " * 3.14159265359)";
    }
    if (name == "length_squared") {
        expect(1);
        return "dot(" + args[0] + ", " + args[0] + ")";
    }
    if (name == "pow" || name == "powr")
        return binary("pow");
    if (name == "atan2")
        return binary("atan");
    if (name == "fmin" || name == "min")
        return binary("min");
    if (name == "fmax" || name == "max")
        return binary("max");
    if (name == "fmod") {
        expect(2);
        return "(" + args[0] + " - " + args[1] + " * trunc(" + args[0] + " / " + args[1] + "))";
    }
    if (name == "fdim") {
        expect(2);
        return "max(" + args[0] + " - " + args[1] + ", 0.0)";
    }
    if (name == "copysign")
        return componentwise(type, args, "uintBitsToFloat((floatBitsToUint($0) & 0x7fffffffu) | (floatBitsToUint($1) & 0x80000000u))");
    if (name == "step")
        return binary("step");
    if (name == "dot")
        return binary("dot");
    if (name == "cross")
        return binary("cross");
    if (name == "distance")
        return binary("distance");
    if (name == "reflect")
        return binary("reflect");
    if (name == "ldexp") {
        expect(2);
        Type *exponentType = call.getArgOperand(1)->getType();
        return "ldexp(" + args[0] + ", " + asSigned(args[1], exponentType) + ")";
    }
    if (name == "nextafter")
        return componentwise(type, args,
                             "($0 == $1 ? $1 : ($0 == 0.0 ? uintBitsToFloat((floatBitsToUint($1) & 0x80000000u) | 1u) : uintBitsToFloat((($0 < $1) == ($0 > 0.0)) "
                             "? floatBitsToUint($0) + 1u : floatBitsToUint($0) - 1u)))");
    if (name == "fma")
        return ternary("fma");
    if (name == "mix")
        return ternary("mix");
    if (name == "clamp" || name == "fclamp")
        return ternary("clamp");
    if (name == "smoothstep")
        return ternary("smoothstep");
    if (name == "refract")
        return ternary("refract");
    if (name == "faceforward")
        return ternary("faceforward");
    if (name == "fmin3" || name == "min3") {
        expect(3);
        return "min(min(" + args[0] + ", " + args[1] + "), " + args[2] + ")";
    }
    if (name == "fmax3" || name == "max3") {
        expect(3);
        return "max(max(" + args[0] + ", " + args[1] + "), " + args[2] + ")";
    }
    if (name == "fmedian3" || name == "median3") {
        expect(3);
        return "max(min(" + args[0] + ", " + args[1] + "), min(max(" + args[0] + ", " + args[1] + "), " + args[2] + "))";
    }
    if (name == "simd_broadcast") {
        expect(2);
        require("GL_ARB_shader_ballot");
        return "readInvocationARB(" + args[0] + ", " + args[1] + ")";
    }
    if (name == "quad_broadcast") {
        expect(2);
        // Bit 0 of the last word of the options: the GL has the shuffles of GL_KHR_shader_subgroup.
        if (m_options.last & 1) {
            require("GL_KHR_shader_subgroup_basic");
            require("GL_KHR_shader_subgroup_shuffle");
            return "subgroupShuffle(" + args[0] + ", (gl_SubgroupInvocationID & ~3u) | uint(" + args[1] + "))";
        }
        require("GL_ARB_shader_ballot");
        return "readInvocationARB(" + args[0] + ", (gl_SubGroupInvocationARB & ~3u) | uint(" + args[1] + "))";
    }
    if (name == "simd_broadcast_first") {
        expect(1);
        require("GL_ARB_shader_ballot");
        return "readFirstInvocationARB(" + args[0] + ")";
    }
    if (name == "simd_all" || name == "simd_any") {
        expect(1);
        require("GL_ARB_shader_group_vote");
        return std::string(name == "simd_all" ? "allInvocationsARB(" : "anyInvocationARB(") + args[0] + ")";
    }
    if (name == "simd_ballot") {
        expect(1);
        require("GL_ARB_shader_ballot");
        require("GL_ARB_gpu_shader_int64");
        return typeName(type) + "(ballotARB(" + args[0] + "))";
    }
    if (name == "simd_is_first") {
        require("GL_ARB_shader_ballot");
        return "(gl_SubGroupInvocationARB == readFirstInvocationARB(gl_SubGroupInvocationARB))";
    }
    handled = false;
    return std::string();
}

void Emitter::emitAirIntrinsic(CallInst &call, const std::string &fullName, const std::string &target)
{
    const std::vector<std::string> parts = split(fullName.substr(4));
    std::string name = parts[0];
    if (name.rfind("fast_", 0) == 0)
        name = name.substr(5);
    if (name.rfind("precise_", 0) == 0)
        name = name.substr(8);

    if (name == "convert") {
        line(target + " = " + convert(call, parts) + ";");
        return;
    }
    if (emitCommand(call, name, target))
        return;
    if (name == "discard_fragment") {
        m_shader.flags |= 0x100; // the fragment function discards
        line("discard;");
        return;
    }
    if (name == "get_num_samples") {
        line(target + " = uint(gl_NumSamples);");
        return;
    }
    if ((name == "wg" || name == "simdgroup" || name == "quadgroup") && parts.size() > 1 && parts[1] == "barrier") {
        uint64_t memory = 3;
        if (call.arg_size()) {
            if (const auto *constant = dyn_cast<ConstantInt>(call.getArgOperand(0)))
                memory = constant->getZExtValue();
        }
        if (m_info.stage != 3)
            throw Unsupported("barriers outside kernels are not supported");
        if (memory & 1)
            line("memoryBarrierBuffer();");
        if (memory & 2)
            line("memoryBarrierShared();");
        if (memory & 4)
            line("memoryBarrierImage();");
        if (name == "wg") {
            m_barrier = true;
            m_shader.flags |= 0x8000; // the threads of a threadgroup wait for each other
            line("barrier();");
        }
        return;
    }
    if (name == "mem_barrier" || name == "atomic_fence") {
        line("memoryBarrier();");
        return;
    }
    if (name == "atomic") {
        if (parts.size() < 3)
            throw Unsupported("an unreadable atomic intrinsic " + fullName);
        const std::string operation = parts[2];
        Value *pointer = call.getArgOperand(0);
        Root &root = rootOf(pointer);
        const bool isFloat = hasPart(parts, "f");
        const bool isSigned = hasPart(parts, "s");
        if (operation == "cmpxchg") {
            Value *expectedPointer = call.getArgOperand(1);
            Root &expectedRoot = rootOf(expectedPointer);
            Type *type = call.getArgOperand(2)->getType();
            const Address expectedAddress = addressOf(expectedPointer);
            const std::string expected = "reef_k" + std::to_string(m_nextVariable++);
            m_variables.emplace_back(typeName(type), expected);
            line(expected + " = " + loadValue(expectedRoot, expectedAddress, 4, type) + ";");
            const std::string previous = "reef_k" + std::to_string(m_nextVariable++);
            m_variables.emplace_back(typeName(type), previous);
            line(previous + " = "
                 + atomic(root, addressOf(pointer), 4, "cmpxchg", type, valueOf(call.getArgOperand(2)), expected, false, isFloat) + ";");
            storeValue(expectedRoot, expectedAddress, 4, type, previous);
            line(target + " = (" + previous + " == " + expected + ");");
            return;
        }
        if (operation == "load") {
            line(target + " = " + atomic(root, addressOf(pointer), 4, "load", call.getType(), isFloat ? "0.0" : "0u", "", false, isFloat) + ";");
            return;
        }
        if (operation == "store") {
            line(atomic(root, addressOf(pointer), 4, "xchg", call.getArgOperand(1)->getType(), valueOf(call.getArgOperand(1)), "", false, isFloat)
                 + ";");
            return;
        }
        static const std::map<std::string, std::string> operations = {
            {"add", "add"},
            {"sub", "sub"},
            {"and", "and"},
            {"or", "or"},
            {"xor", "xor"},
            {"min", "min"},
            {"max", "max"},
            {"xchg", "xchg"},
        };
        const auto known = operations.find(operation);
        if (known == operations.end())
            throw Unsupported("the atomic operation " + fullName + " is not supported");
        line(target + " = "
             + atomic(root, addressOf(pointer), 4, known->second, call.getType(), valueOf(call.getArgOperand(1)), "", isSigned, isFloat) + ";");
        return;
    }
    const char *derivative = name == "dfdx" ? "dFdx" : name == "dfdy" ? "dFdy" : name == "fwidth" ? "fwidth" : nullptr;
    if (derivative) {
        if (m_info.stage != 2)
            throw Unsupported("derivatives outside fragment functions");
        line(target + " = " + derivative + "(" + valueOf(call.getArgOperand(0)) + ");");
        return;
    }
    if (name.rfind("sample", 0) == 0 || name.rfind("read_", 0) == 0 || name.rfind("write_", 0) == 0 || name.rfind("gather", 0) == 0
        || name.rfind("get_", 0) == 0) {
        const std::string value = textureCall(call, name);
        if (!value.empty())
            line((target.empty() ? "" : target + " = ") + value + ";");
        return;
    }
    if (name == "fence" || name.rfind("fence_", 0) == 0) {
        line("memoryBarrierImage();");
        return;
    }
    bool handled;
    const std::string value = mathIntrinsic(call, name, parts, handled);
    if (!handled)
        throw Unsupported("the intrinsic " + fullName + " is not supported");
    line(target + " = " + value + ";");
}

void Emitter::emitCall(CallInst &call)
{
    Function *callee = call.getCalledFunction();
    if (!callee)
        throw Unsupported("indirect calls are not supported");
    const std::string name = callee->getName().str();
    const std::string target = call.getType()->isVoidTy() ? "" : nameOf(&call);
    if (callee->isIntrinsic()) {
        switch (callee->getIntrinsicID()) {
        case Intrinsic::assume:
        case Intrinsic::donothing:
        case Intrinsic::experimental_noalias_scope_decl:
        case Intrinsic::invariant_end:
        case Intrinsic::invariant_start:
        case Intrinsic::lifetime_end:
        case Intrinsic::lifetime_start:
        case Intrinsic::sideeffect:
            return;
        case Intrinsic::memcpy:
        case Intrinsic::memcpy_inline:
        case Intrinsic::memmove:
        case Intrinsic::memset:
        case Intrinsic::memset_inline: {
            Intrinsic::ID id = Intrinsic::memcpy;
            switch (callee->getIntrinsicID()) {
            case Intrinsic::memmove:
                id = Intrinsic::memmove;
                break;
            case Intrinsic::memset:
            case Intrinsic::memset_inline:
                id = Intrinsic::memset;
                break;
            default:
                break;
            }
            copyMemory(call, id);
            return;
        }
        default:
            break;
        }
        if (!emitLlvmIntrinsic(call, target))
            throw Unsupported("the intrinsic " + name + " is not supported");
        return;
    }
    if (name.rfind("air.", 0) != 0)
        throw Unsupported("a call of the external function " + name);
    emitAirIntrinsic(call, name, target);
}

}
