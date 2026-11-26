// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The texture arguments of a function and the GLSL samplers and images they become.
#pragma once

#include <map>
#include <string>
#include <utility>

namespace cutefish::reef::metal {

struct Argument;
struct Root;

// What the components of a texture are in GLSL.
enum class Component
{
    Float,
    Int,
    Uint,
};

// A texture argument that the function uses. (0x88 bytes)
struct Texture
{
    const Argument *argument = nullptr;                             // 0x00
    std::string kind;                                               // 0x08  "2d", "cube_array" ...
    Component component = Component::Float;                         // 0x28
    int dataType = 3;                                               // 0x2c  MTLDataType of a component
    bool depth = false;                                             // 0x30
    bool multisampled = false;                                      // 0x31
    bool array = false;                                             // 0x32
    // GLSL samplers by the Metal sampler (none when read by pixel) and whether it compares
    std::map<std::pair<const Root *, bool>, std::string> samplers;  // 0x38
    std::string image;                                              // 0x68  GLSL image, when written or queried as one
};

}
