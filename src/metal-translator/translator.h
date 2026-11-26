// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The C interface of libreef-metal-translator.so: libraries, functions and translated shaders.
#pragma once

#include "shader.h"

// A Metal library: from a .metallib file or from source. Released with reef_mtl_library_release.
struct reef_mtl_library;
// One function of a library, with its reflection. Released with reef_mtl_function_release.
struct reef_mtl_function;
// The GLSL of a function and what to bind to it. Released with reef_mtl_shader_release.
struct reef_mtl_shader;

// How to compile Metal source. (0x20 bytes)
struct reef_mtl_source_options
{
    int languageVersion;            // 0x00  as MTLLanguageVersion, 0 for the newest
    int fastMath;                   // 0x04
    int macroCount;                 // 0x08
    int reserved;                   // 0x0c
    const char *const *names;       // 0x10  of the preprocessor macros
    const char *const *values;      // 0x18  null, or a null value: "1"
};

// The value of a function constant. (0x18 bytes)
struct reef_mtl_constant_value
{
    int index;                      // 0x00  [[function_constant(n)]]
    int type;                       // 0x04  MTLDataType
    const char *name;               // 0x08
    const void *data;               // 0x10  as many bytes as the type has
};

// The length of a [[threadgroup(n)]] buffer of a kernel. (8 bytes)
struct reef_mtl_threadgroup_length
{
    int index;                      // 0x00
    int length;                     // 0x04  bytes
};

// How to translate. (0x38 bytes)
struct reef_mtl_translate_options
{
    int firstBuffer;                // 0x00  the first GL binding point of the buffers
    int firstTexture;               // 0x04  the first texture unit
    int firstImage;                 // 0x08  the first image unit
    int firstLocation;              // 0x0c  the first uniform location
    int threadgroupSize[3];         // 0x10  of a kernel; less than 1 counts as 1
    int valueCount;                 // 0x1c
    const reef_mtl_constant_value *values;              // 0x20
    int lengthCount;                // 0x28
    int flags;                      // 0x2c  bit 0: quad_broadcast with GL_KHR_shader_subgroup
    const reef_mtl_threadgroup_length *lengths;         // 0x30
};

// A function constant that a function reads. (0x18 bytes)
struct reef_mtl_function_constant
{
    int index;                      // 0x00  [[function_constant(n)]]
    int type;                       // 0x04  MTLDataType
    int required;                   // 0x08  not only tested with is_function_constant_defined
    const char *name;               // 0x10
};

// A member of an argument buffer, flattened. (0x40 bytes)
struct reef_mtl_argument_member
{
    int kind;                       // 0x00  1 data, 2 texture, 3 sampler, 4 buffer, 5 indirect command buffer,
                                    //       6 render pipeline state, 7 compute pipeline state, 8 argument buffer
    int index;                      // 0x04  [[id(n)]]
    int count;                      // 0x08  array length
    int offset;                     // 0x0c  in the argument buffer
    int size;                       // 0x10
    int dataType;                   // 0x14  MTLDataType of a data member
    int textureType;                // 0x18  MTLTextureType of a texture
    int textureDataType;            // 0x1c  MTLDataType of its components
    int access;                     // 0x20  1 read, 2 write, 4 sample
    int addressSpace;               // 0x24
    int typeSize;                   // 0x28  air.arg_type_size
    int alignment;                  // 0x2c  air.arg_type_align_size
    const char *name;               // 0x30
    const char *typeName;           // 0x38
};

#pragma GCC visibility push(default)
extern "C" {

// 1: the version of this interface.
int reef_mtl_translator_version();

// A library from the bytes of a .metallib file. Null on failure, with the reason in `error`.
reef_mtl_library *reef_mtl_library_create_from_data(const void *data, long size, char *error, long errorSize);
// A library from Metal source (`length` -1: up to the NUL). The compiler's messages, warnings too, go to `error`.
reef_mtl_library *reef_mtl_library_create_from_source(const char *source, long length, const reef_mtl_source_options *options,
                                                      char *error, long errorSize);
void reef_mtl_library_release(reef_mtl_library *library);
int reef_mtl_library_function_count(const reef_mtl_library *library);
const char *reef_mtl_library_function_name(const reef_mtl_library *library, int index);
// 1 vertex, 2 fragment, 3 kernel, 5 visible.
int reef_mtl_library_function_stage(const reef_mtl_library *library, int index);

reef_mtl_function *reef_mtl_function_create(const reef_mtl_library *library, const char *name, char *error, long errorSize);
void reef_mtl_function_release(reef_mtl_function *function);
const char *reef_mtl_function_name(const reef_mtl_function *function);
int reef_mtl_function_stage(const reef_mtl_function *function);
int reef_mtl_function_attribute_count(const reef_mtl_function *function);
const reef_mtl_attribute *reef_mtl_function_attribute(const reef_mtl_function *function, int index);
// The members of the argument buffer at [[buffer(index)]]; 0 when there is none.
int reef_mtl_function_argument_buffer(const reef_mtl_function *function, int index, int *size, int *alignment);
const reef_mtl_argument_member *reef_mtl_function_argument_member(const reef_mtl_function *function, int index, int member);
int reef_mtl_function_constant_count(const reef_mtl_function *function);
const reef_mtl_function_constant *reef_mtl_function_constant_at(const reef_mtl_function *function, int index);
reef_mtl_shader *reef_mtl_function_translate(const reef_mtl_function *function, const reef_mtl_translate_options *options,
                                             char *error, long errorSize);
// Translates with the bodies of the visible functions `linked`, which the function calls.
reef_mtl_shader *reef_mtl_function_translate_linked(const reef_mtl_function *function, const reef_mtl_function *const *linked,
                                                    int linkedCount, const reef_mtl_translate_options *options, char *error,
                                                    long errorSize);

void reef_mtl_shader_release(reef_mtl_shader *shader);
int reef_mtl_shader_stage(const reef_mtl_shader *shader);
const char *reef_mtl_shader_glsl(const reef_mtl_shader *shader);
uint64_t reef_mtl_shader_flags(const reef_mtl_shader *shader);
int reef_mtl_shader_binding_count(const reef_mtl_shader *shader);
const reef_mtl_binding *reef_mtl_shader_binding(const reef_mtl_shader *shader, int index);
int reef_mtl_shader_constant_sampler_count(const reef_mtl_shader *shader);
const reef_mtl_constant_sampler *reef_mtl_shader_constant_sampler(const reef_mtl_shader *shader, int index);
int reef_mtl_shader_attribute_count(const reef_mtl_shader *shader);
const reef_mtl_attribute *reef_mtl_shader_attribute(const reef_mtl_shader *shader, int index);
int reef_mtl_shader_varying_count(const reef_mtl_shader *shader);
const reef_mtl_varying *reef_mtl_shader_varying(const reef_mtl_shader *shader, int index);
int reef_mtl_shader_render_target_count(const reef_mtl_shader *shader);
const reef_mtl_render_target *reef_mtl_shader_render_target(const reef_mtl_shader *shader, int index);
int reef_mtl_shader_clip_distance_count(const reef_mtl_shader *shader);
// The uniform location of an index (0: reef_threads_per_grid), -1 when the shader has none.
int reef_mtl_shader_uniform_location(const reef_mtl_shader *shader, int index);
void reef_mtl_shader_threadgroup_size(const reef_mtl_shader *shader, int *x, int *y, int *z);

}
#pragma GCC visibility pop
