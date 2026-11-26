// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A translated function: its GLSL and what the caller binds to it, and the options of a translation.
#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>

// A resource of the shader and where GL wants it. (0x58 bytes)
struct reef_mtl_binding
{
    int kind;                   // 0x00  1 buffer, 2 texture, 3 image, 4 threadgroup memory, 5 colour attachment read back
    int index;                  // 0x04  the Metal slot: [[buffer(n)]], [[texture(n)]], [[color(n)]] ...
    int count;                  // 0x08
    int binding;                // 0x0c  the GL binding point or texture unit, -1 none
    int access;                 // 0x10  1 read, 2 write, 4 sample
    int type;                   // 0x14  buffer: 1 device, 2 constant; texture: its kind (0 1d ... 9 buffer); 3 threadgroup
    int dataType;               // 0x18  MTLDataType of the components of a texture
    int sampler;                // 0x1c  [[sampler(n)]] used with the texture, -1 none
    int constantSampler;        // 0x20  its place in the constant samplers, -1 none
    int size;                   // 0x24  bytes
    int alignment;              // 0x28
    int location;               // 0x2c  uniform: address of a buffer, size of a texture read by pixel; -1 none
    const char *name;           // 0x30  the Metal name
    const char *variable;       // 0x38  the GLSL name
    int indirect;               // 0x40  1: the resource is in an argument buffer, 2: its sampler is
    int argumentBuffer;         // 0x44
    int argumentMember;         // 0x48
    int samplerArgumentBuffer;  // 0x4c
    int samplerArgumentMember;  // 0x50
    int reserved;               // 0x54
};

// A sampler declared in the shader source. (0x38 bytes)
struct reef_mtl_constant_sampler
{
    int index;                  // 0x00
    int minFilter;              // 0x04
    int magFilter;              // 0x08
    int mipFilter;              // 0x0c
    int maxAnisotropy;          // 0x10
    int sAddress;               // 0x14
    int normalized;             // 0x18  0: coordinates in pixels
    int rest[7];                // 0x1c
};

// A vertex attribute. (0x20 bytes)
struct reef_mtl_attribute
{
    int index;                  // 0x00  [[attribute(n)]]
    int type;                   // 0x04  MTLDataType
    int kind;                   // 0x08  0 float, 1 int, 2 uint
    int components;             // 0x0c
    int active;                 // 0x10
    const char *name;           // 0x18
};

// An output of a vertex function that the fragment function reads. (0x20 bytes)
struct reef_mtl_varying
{
    int type;                   // 0x00  MTLDataType
    int interpolation;          // 0x04  0 perspective, 1 no_perspective, 2 flat
    int sampling;               // 0x08
    int used;                   // 0x0c
    const char *name;           // 0x10  the user name
    const char *variable;       // 0x18  the GLSL name
};

// A colour output. (8 bytes)
struct reef_mtl_render_target
{
    int index;                  // 0x00  [[color(n)]]
    int type;                   // 0x04  MTLDataType
};

namespace cutefish::reef::metal {

// The value of a function constant given by the caller. (0x50 bytes)
struct ConstantValue
{
    int index = -1;                             // 0x00
    std::string name;                           // 0x08
    int type = 0;                               // 0x28  MTLDataType
    std::string bytes;                          // 0x30
};

// How to translate. (0x70 bytes)
struct Options
{
    int firstBuffer = 0;                        // 0x00  the first GL binding point of the buffers
    int firstTexture = 0;                       // 0x04  the first texture unit
    int firstImage = 0;                         // 0x08  the first image unit
    int firstLocation = 0;                      // 0x0c  the first uniform location
    int threadgroupSize[3] = {1, 1, 1};         // 0x10
    std::vector<ConstantValue> constants;       // 0x20
    std::map<int, int> threadgroupMemory;       // 0x38  [[threadgroup(n)]]: its length
    int last = 0;                               // 0x68  bit 0: quad_broadcast with GL_KHR_shader_subgroup
};

// The result of a translation. (0x110 bytes)
struct Shader
{
    int stage = 0;                                      // 0x00
    std::string glsl;                                   // 0x08
    uint64_t flags = 0;                                 // 0x28  what the shader writes and needs
    std::vector<reef_mtl_binding> bindings;             // 0x30
    std::vector<reef_mtl_constant_sampler> samplers;    // 0x48  constant samplers
    std::vector<reef_mtl_attribute> attributes;         // 0x60
    std::vector<reef_mtl_varying> varyings;             // 0x78
    std::vector<reef_mtl_render_target> renderTargets;  // 0x90
    int clipDistances = 0;                              // 0xa8
    int uniformLocations[2] = {-1, -1};                 // 0xac  [0]: reef_threads_per_grid
    int threadgroupSize[3] = {1, 1, 1};                 // 0xb4
    std::deque<std::string> strings;                    // 0xc0  what the const char * of the structures point into
};

}
