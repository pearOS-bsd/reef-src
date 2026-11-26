// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Texture functions: sampling, gathering, reads, writes and queries. Metal keeps textures and samplers apart; GLSL gets
// one combined sampler for every pair that meets, and an image for a texture that is written.

#include "emitter.h"

#include <cstdint>

#include <llvm/Support/raw_ostream.h>

namespace cutefish::reef::metal {

struct TextureKind
{
    const char *name;
    const char *glsl;
    unsigned dimensions;
    bool array;
    bool multisampled;
    bool offsets;       // its functions take texel offsets
    int type;           // MTLTextureType
};

static const TextureKind textureKinds[] = {
    {"1d", "1D", 1, false, false, true, 0},
    {"1d_array", "1DArray", 1, true, false, true, 1},
    {"2d", "2D", 2, false, false, true, 2},
    {"2d_array", "2DArray", 2, true, false, true, 3},
    {"2d_ms", "2DMS", 2, false, true, false, 4},
    {"cube", "Cube", 3, false, false, false, 5},
    {"cube_array", "CubeArray", 3, true, false, false, 6},
    {"3d", "3D", 3, false, false, true, 7},
    {"2d_ms_array", "2DMSArray", 2, true, true, false, 8},
    {"_buffer", "Buffer", 1, false, false, false, 9},
    {"buffer", "Buffer", 1, false, false, false, 9},
};

static const TextureKind &kindOf(const Texture &texture)
{
    for (const TextureKind &kind : textureKinds) {
        if (texture.kind == kind.name)
            return kind;
    }
    throw Unsupported("the texture type " + texture.argument->typeName + " is not supported");
}

// Offsets and minimum levels that change nothing.
static bool isZero(const llvm::Value *value)
{
    const auto *constant = llvm::dyn_cast<llvm::Constant>(value);
    return constant && (constant->isNullValue() || llvm::isa<llvm::UndefValue>(constant));
}

void Emitter::addTexture(const Argument &argument)
{
    Texture texture;
    texture.argument = &argument;
    const size_t bracket = argument.typeName.find('<');
    std::string kind = argument.typeName.substr(0, bracket);
    if (kind.starts_with("texture")) {
        kind = kind.substr(7);
    } else if (kind.starts_with("depth")) {
        kind = kind.substr(5);
        texture.depth = true;
    } else {
        throw Unsupported("the texture type " + argument.typeName + " is not supported");
    }
    texture.kind = kind;
    std::string component = "float";
    if (bracket != std::string::npos) {
        const size_t end = argument.typeName.find_first_of(",>", bracket);
        component = argument.typeName.substr(bracket + 1, end - bracket - 1);
    }
    if (component == "half") {
        texture.dataType = 16;
    } else if (component == "int") {
        texture.component = Component::Int;
        texture.dataType = 29;
    } else if (component == "short") {
        texture.component = Component::Int;
        texture.dataType = 37;
    } else if (component == "uint") {
        texture.component = Component::Uint;
        texture.dataType = 33;
    } else if (component == "ushort") {
        texture.component = Component::Uint;
        texture.dataType = 41;
    }
    const TextureKind &known = kindOf(texture);
    texture.array = known.array;
    texture.multisampled = known.multisampled;
    m_textures[argument.location] = texture;
}

void Emitter::collectTextures()
{
    for (const auto &[value, root] : m_argumentRoots) {
        if (root->kind != Root::Texture || value->use_empty())
            continue;
        if (root->argument->count > 1)
            throw Unsupported("arrays of textures are not supported");
        addTexture(*root->argument);
    }
    collectArgumentBuffers();
}

Texture &Emitter::textureOf(llvm::Value *value)
{
    const Root &root = rootOf(value);
    if (root.kind != Root::Texture)
        throw Unsupported("a texture function used on something that is not a texture");
    const auto found = m_textures.find(root.argument->location);
    if (found == m_textures.end())
        throw Unsupported("a texture argument that is not known");
    return found->second;
}

// The GLSL sampler for a texture read through a Metal sampler.
std::string Emitter::samplerOf(Texture &texture, const Root *sampler, bool compared)
{
    const std::pair<const Root *, bool> key(sampler, compared);
    const auto found = texture.samplers.find(key);
    if (found != texture.samplers.end())
        return found->second;
    const std::string name = "reef_" + m_prefix + "t" + std::to_string(texture.argument->location) + "_"
        + std::to_string(texture.samplers.size());
    texture.samplers.emplace(key, name);
    return name;
}

std::string Emitter::imageOf(Texture &texture)
{
    if (!(texture.argument->access & 2))
        throw Unsupported("writing texture " + texture.argument->name + ", which is not writable");
    texture.image = "reef_" + m_prefix + "i" + std::to_string(texture.argument->location);
    return texture.image;
}

std::string Emitter::pixelOf(llvm::Value *value, unsigned components)
{
    const std::string text = valueOf(value);
    if (components == 1)
        return "int(" + text + ")";
    return "ivec" + std::to_string(components) + "(" + text + ")";
}

// The coordinates of a sampling function: in pixels when the sampler says so, with the layer and the value to compare
// with where GLSL wants them.
std::string Emitter::coordinatesOf(const Texture &texture, const std::string &sampler, const Root *root,
                                   const std::string &coordinates, const std::string &layer, unsigned components,
                                   bool compared, const std::string &reference)
{
    std::string position = coordinates;
    const TextureKind &kind = kindOf(texture);
    const bool pixel = root && root->sampler >= 0 && !m_shader.samplers[root->sampler].normalized;
    const std::string size = components == 1
        ? "float(textureSize(" + sampler + ", 0)" + (kind.array ? ".x" : "") + ")"
        : "vec" + std::to_string(components) + "(textureSize(" + sampler + ", 0)"
            + (kind.array ? std::string(".") + (components == 2 ? "xy" : "xyz") : std::string()) + ")";
    if (pixel) {
        position = "(" + coordinates + " / " + size + ")";
    } else if (root && root->sampler < 0 && components < 3) {
        // only known when the sampler is bound
        const std::string pixels = sampler + "_pixels";
        position = "(" + pixels + " != 0u ? " + coordinates + " / " + size + " : " + coordinates + ")";
    }
    std::vector<std::string> parts{position};
    if (!layer.empty()) {
        parts.push_back("float(" + layer + ")");
        ++components;
    }
    if (compared && texture.kind != "cube_array") {
        if (components == 1) {
            parts.push_back("0.0");
            ++components;
        }
        parts.push_back(reference);
        ++components;
    }
    if (parts.size() == 1)
        return parts[0];
    std::string text = "vec" + std::to_string(components) + "(";
    for (size_t i = 0; i < parts.size(); ++i)
        text += (i ? ", " : "") + parts[i];
    return text + ")";
}

// The four components GLSL returns, as the type Metal's function returns. A sparse access returns a structure: the texel
// and whether it is resident.
std::string Emitter::texelOf(llvm::Type *type, const std::string &text, bool compared)
{
    llvm::Type *element = type;
    if (type->isStructTy())
        element = type->getStructElementType(0);
    std::string texel = text;
    const unsigned components = width(element);
    if (isInteger(element)) {
        const std::string converted = (components == 1 ? "uint" : "uvec" + std::to_string(components)) + "(" + texel
            + (components == 1 ? ".x" : components < 4 ? "." + std::string("xyzw").substr(0, components) : std::string())
            + ")";
        texel = masked(converted, element);
    } else if (components == 1) {
        if (!compared)
            texel = "(" + texel + ").x";
    } else if (compared) {
        texel = typeName(element) + "(" + texel + ")";
    } else if (components < 4) {
        texel = "(" + texel + ")." + std::string("xyzw").substr(0, components);
    }
    if (!type->isStructTy())
        return texel;
    std::string fields = typeName(type) + "(" + texel;
    for (unsigned i = 1; i < type->getStructNumElements(); ++i)
        fields += ", " + constant(llvm::Constant::getNullValue(type->getStructElementType(i)));
    return fields + ")";
}

std::string Emitter::textureCall(const llvm::CallBase &call, const std::string &name)
{
    Texture &texture = textureOf(call.getArgOperand(0));
    const TextureKind &kind = kindOf(texture);
    const unsigned count = call.arg_size();
    const auto describe = [&] {
        std::string type;
        llvm::raw_string_ostream stream(type);
        call.getFunctionType()->print(stream);
        return "air." + name + " " + type;
    };
    const auto argument = [&](unsigned index) {
        if (index >= count)
            throw Unsupported("too few arguments: " + describe());
        return call.getArgOperand(index);
    };
    llvm::Type *type = call.getType();
    const bool image = !texture.image.empty() || texture.argument->access & 2;
    // the sampler for access by pixel
    const std::string query = "reef_" + m_prefix + "t" + std::to_string(texture.argument->location) + "_q";

    if (name.starts_with("get_")) {
        std::string what = name.substr(4);
        size_t end = what.find("_texture");
        if (end == std::string::npos)
            end = what.find("_depth");
        what = what.substr(0, end);
        const bool levels = !kind.multisampled && texture.kind != "_buffer" && texture.kind != "buffer";
        const std::string level = count > 1 ? "int(" + valueOf(argument(1)) + ")" : "0";
        std::string size;
        if (image) {
            size = "imageSize(" + imageOf(texture) + ")";
        } else {
            texture.samplers.emplace(std::pair(nullptr, false), query);
            size = "textureSize(" + query + (levels ? ", " + level : std::string()) + ")";
        }
        const bool cube = texture.kind == "cube" || texture.kind == "cube_array";
        const unsigned components = (cube ? 2 : kind.dimensions) + kind.array;
        const auto part = [&](unsigned index) {
            return components == 1 ? size : size + "." + component(index);
        };
        std::string text;
        if (what == "width")
            text = part(0);
        else if (what == "height")
            text = part(1);
        else if (what == "depth")
            text = part(2);
        else if (what == "array_size")
            text = part(components - 1);
        else if (what == "num_mip_levels")
            text = image ? "1" : "textureQueryLevels(" + query + ")";
        else if (what == "num_samples")
            text = image ? "imageSamples(" + texture.image + ")" : "textureSamples(" + query + ")";
        else
            throw Unsupported("the texture query " + describe() + " is not supported");
        return masked(typeName(type) + "(" + text + ")", type);
    }

    if (name.starts_with("read_")) {
        // newer AIR passes a sampler that says how to read, and an offset
        const bool sampled = argument(1)->getType()->isPointerTy();
        llvm::Value *coordinates = argument(sampled ? 2 : 1);
        const unsigned components = width(coordinates->getType());
        unsigned next = sampled ? 3 : 2;
        std::string pixel = pixelOf(coordinates, components);
        if (texture.kind == "cube" || texture.kind == "cube_array")
            throw Unsupported("reading cube textures by pixel is not supported");
        if (sampled && next < count && argument(next)->getType() == coordinates->getType()) {
            llvm::Value *offset = argument(next++);
            if (!isZero(offset))
                pixel = "(" + pixel + " + " + pixelOf(offset, components) + ")";
        }
        if (kind.array) {
            const std::string layer = "int(" + valueOf(argument(next++)) + ")";
            pixel = "ivec" + std::to_string(components + 1) + "(" + pixel + ", " + layer + ")";
        }
        std::string sample;
        if (kind.multisampled)
            sample = "int(" + valueOf(argument(next++)) + ")";
        std::string text;
        if (image && texture.argument->access & 1) {
            require("GL_EXT_shader_image_load_formatted");
            text = "imageLoad(" + imageOf(texture) + ", " + pixel + (sample.empty() ? "" : ", " + sample) + ")";
        } else {
            texture.samplers.emplace(std::pair(nullptr, false), query);
            if (texture.kind == "_buffer" || texture.kind == "buffer") {
                text = "texelFetch(" + query + ", " + pixel + ")";
            } else if (kind.multisampled) {
                text = "texelFetch(" + query + ", " + pixel + ", " + sample + ")";
            } else {
                text = "texelFetch(" + query + ", " + pixel + ", "
                    + (next < count ? "int(" + valueOf(argument(next)) + ")" : "0") + ")";
            }
        }
        return texelOf(type, text, false);
    }

    if (name.starts_with("write_")) {
        llvm::Value *coordinates = argument(1);
        const unsigned components = width(coordinates->getType());
        std::string pixel = pixelOf(coordinates, components);
        unsigned next = 2;
        if (texture.kind == "cube" || texture.kind == "cube_array") {
            std::string face = "int(" + valueOf(argument(next++)) + ")";
            if (kind.array)
                face = "(int(" + valueOf(argument(next++)) + ") * 6 + " + face + ")";
            pixel = "ivec3(" + pixel + ", " + face + ")";
        } else if (kind.array) {
            const std::string layer = "int(" + valueOf(argument(next++)) + ")";
            pixel = "ivec" + std::to_string(components + 1) + "(" + pixel + ", " + layer + ")";
        }
        llvm::Value *texel = argument(next);
        const unsigned size = width(texel->getType());
        std::string text = valueOf(texel);
        if (size != 4)
            throw Unsupported("texture writes take four components: " + describe());
        if (texture.component == Component::Int)
            text = "ivec4(" + asSigned(text, texel->getType()) + ")";
        else if (texture.component == Component::Uint)
            text = "uvec4(" + text + ")";
        else
            text = "vec4(" + text + ")";
        return "imageStore(" + imageOf(texture) + ", " + pixel + ", " + text + ")";
    }

    const size_t compare = name.find("compare");
    const bool compared = compare != std::string::npos;
    const bool gather = name.starts_with("gather");
    const bool gradient = name.size() > 5 && name.ends_with("_grad");

    const Root &root = rootOf(argument(1));
    if (root.kind != Root::Sampler)
        throw Unsupported("a texture sampled without a sampler: " + describe());
    llvm::Value *coordinates = argument(2);
    const unsigned components = width(coordinates->getType());
    unsigned next = 3;
    std::string layer;
    if (kind.array)
        layer = valueOf(argument(next++));
    std::string reference;
    if (compared)
        reference = valueOf(argument(next++));
    std::string dx, dy;
    llvm::Value *minimum = nullptr;
    if (gradient) {
        dx = valueOf(argument(next++));
        dy = valueOf(argument(next++));
        if (next < count && !argument(next)->getType()->isIntegerTy(1))
            minimum = argument(next++);
    }
    std::string offset;
    // a flag and the offset
    if (kind.offsets && next + 1 < count && argument(next)->getType()->isIntegerTy(1)
        && argument(next + 1)->getType()->getScalarType()->isIntegerTy(32)) {
        llvm::Value *value = argument(next + 1);
        if (!isZero(value)) {
            const auto *constant = llvm::dyn_cast<llvm::Constant>(value);
            if (!constant)
                throw Unsupported("texture offsets known only at run time are not supported");
            const unsigned size = width(constant->getType());
            std::string text = size > 1 ? "ivec" + std::to_string(size) + "(" : "";
            for (unsigned i = 0; i < size; ++i) {
                const llvm::Constant *element = size == 1 ? constant : constant->getAggregateElement(i);
                const int64_t number = llvm::isa<llvm::UndefValue>(element) ? 0 : llvm::cast<llvm::ConstantInt>(element)->getSExtValue();
                text += (i ? ", " : "") + std::to_string(number);
            }
            offset = text + (size > 1 ? ")" : "");
        }
        next += 2;
    }
    const std::string sampler = samplerOf(texture, &root, compared);
    const std::string position = coordinatesOf(texture, sampler, &root, valueOf(coordinates), layer, components,
                                               compared && !gather, reference);

    if (gather) {
        std::string component;
        if (!compared) {
            component = "0";
            if (next < count) {
                const auto *constant = llvm::dyn_cast<llvm::ConstantInt>(argument(next));
                if (!constant)
                    throw Unsupported("gather components known only at run time are not supported");
                component = std::to_string(constant->getZExtValue());
            }
        }
        const std::string last = compared ? reference : component;
        const std::string text = offset.empty()
            ? "textureGather(" + sampler + ", " + position + ", " + last + ")"
            : "textureGatherOffset(" + sampler + ", " + position + ", " + offset + ", " + last + ")";
        return texelOf(type, text, false);
    }

    // a flag, the bias or level, the minimum level and which of the two it is
    std::string level;
    bool leveled = false;
    int64_t mode = 0;
    if (!gradient && next < count && argument(next)->getType()->isIntegerTy(1)) {
        const auto *flag = llvm::dyn_cast<llvm::ConstantInt>(argument(next));
        if (!flag)
            throw Unsupported("a sample level option known only at run time: " + describe());
        leveled = !flag->isZero();
        if (next + 1 < count) {
            level = valueOf(argument(next + 1));
            unsigned last = next + 2;
            if (last < count && argument(last)->getType()->isFloatingPointTy())
                minimum = argument(last++);
            if (last < count) {
                const auto *constant = llvm::dyn_cast<llvm::ConstantInt>(argument(last));
                if (!constant)
                    throw Unsupported("a sample level mode known only at run time: " + describe());
                mode = constant->getSExtValue();
            }
        }
    }
    const bool clamped = minimum && !isZero(minimum);
    const std::string clamp = clamped ? valueOf(minimum) : std::string();
    const bool fragment = m_info.stage == 2;
    // GLSL has no textureLod for these
    const bool cubeArray = compared && texture.kind == "cube_array";
    const bool noLevels = compared && (texture.kind == "2d_array" || texture.kind == "cube");
    const std::string zero = kind.dimensions == 1 ? "0.0" : "vec" + std::to_string(kind.dimensions) + "(0.0)";
    const auto sampleWith = [&](const std::string &function, const std::string &rest) {
        if (offset.empty())
            return function + "(" + sampler + ", " + position + rest + ")";
        if (function == "texture")
            return function + "Offset(" + sampler + ", " + position + ", " + offset + rest + ")";
        return function + "Offset(" + sampler + ", " + position + rest + ", " + offset + ")";
    };
    const auto sampleLevel = [&](const std::string &lod) {
        if (cubeArray)
            return "texture(" + sampler + ", " + position + ", " + reference + ")";
        if (noLevels)
            return sampleWith("textureGrad", ", " + zero + ", " + zero);
        if (!offset.empty())
            return "textureLodOffset(" + sampler + ", " + position + ", " + lod + ", " + offset + ")";
        return "textureLod(" + sampler + ", " + position + ", " + lod + ")";
    };
    std::string text;
    if (gradient) {
        if (cubeArray)
            throw Unsupported("gradients on cube array depth textures are not supported");
        text = sampleWith("textureGrad", ", " + dx + ", " + dy);
    } else if (leveled && mode == 1) {
        text = sampleLevel(clamped ? "max(" + level + ", " + clamp + ")" : level);
    } else if (!fragment) {
        text = sampleLevel("0.0");
    } else if (clamped && !cubeArray) {
        const std::string plain = coordinatesOf(texture, sampler, &root, valueOf(coordinates), "", components, false, "");
        std::string lod = "textureQueryLod(" + sampler + ", " + plain + ").y";
        if (leveled && mode == 0)
            lod = "(" + lod + " + " + level + ")";
        text = sampleLevel("max(" + lod + ", " + clamp + ")");
    } else if (cubeArray) {
        text = "texture(" + sampler + ", " + position + ", " + reference + ")";
    } else if (leveled && mode == 0) {
        text = sampleWith("texture", ", " + level);
    } else {
        text = sampleWith("texture", "");
    }
    return texelOf(type, text, compared);
}

}
