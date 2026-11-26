// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The AIR metadata of a function: its arguments, outputs and function constants, and the Metal data types.
#pragma once

#include <string>
#include <vector>

namespace llvm {
class Function;
class GlobalVariable;
class Module;
}

namespace cutefish::reef::metal {

// What an argument, an output or a member of an argument buffer is, by its AIR name.
enum class Role
{
    None,
    Buffer,                      // air.buffer, air.indirect_buffer
    Texture,
    Sampler,
    Threadgroup,                 // a buffer in threadgroup memory
    StageIn,                     // air.stage_in, air.vertex_input
    FragmentInput,
    Position,
    VertexId,
    InstanceId,
    BaseVertex,
    BaseInstance,
    FrontFacing,
    PointCoord,
    SampleId,
    SampleMask,                  // air.sample_mask, air.sample_mask_in
    PrimitiveId,
    RenderTargetArrayIndex,
    ViewportArrayIndex,
    ThreadPositionInGrid,
    ThreadPositionInThreadgroup,
    ThreadgroupPositionInGrid,
    ThreadsPerThreadgroup,
    ThreadsPerGrid,
    ThreadgroupsPerGrid,
    ThreadIndexInThreadgroup,
    ThreadIndexInSimdgroup,      // and air.thread_index_in_quadgroup
    ThreadsPerSimdgroup,         // and air.thread_execution_width
    SimdgroupIndexInThreadgroup,
    SimdgroupsPerThreadgroup,
    VertexOutput,
    PointSize,
    ClipDistance,
    RenderTarget,
    Depth,
    Stencil,
    Color,
    IndirectConstant,
    CommandBuffer,
    RenderPipelineState,
    ComputePipelineState,
};

// A field of air.struct_type_info that has an air.indirect_argument, flattened: the resources and values inside an argument
// buffer. (0x68 bytes)
struct Member
{
    Role role = Role::None;      // 0x00  of its air.indirect_argument: IndirectConstant for plain data
    int offset = 0;              // 0x04  from the start of the outermost structure
    int size = 0;                // 0x08
    int location = 0;            // 0x0c  [[id(n)]], from the air.indirect_argument
    int count = 1;               // 0x10  array length
    unsigned access = 0;         // 0x14  1 read, 2 write, 4 sample
    int addressSpace = 0;        // 0x18
    int typeSize = 0;            // 0x1c  air.arg_type_size
    int alignment = 0;           // 0x20  air.arg_type_align_size
    bool indirect = false;       // 0x24  an argument buffer itself (air.indirect_buffer)
    std::string typeName;        // 0x28
    std::string name;            // 0x48
};

// An argument or an output of a function. (0xc8 bytes)
struct Argument
{
    Role role = Role::None;                      // 0x00
    int number = -1;                             // 0x04  the parameter of an argument; the member of the returned structure of an output, -1 for the whole value
    int location = 0;                            // 0x08  [[buffer(n)]], [[attribute(n)]], [[color(n)]], [[user(n)]] ...
    int count = 1;                               // 0x0c  array length of a resource: the second number of air.location_index
    unsigned access = 0;                         // 0x10  1 read, 2 write, 4 sample
    int addressSpace = 0;                        // 0x14  1 device, 2 constant, 3 threadgroup
    int size = 0;                                // 0x18  air.arg_type_size
    int alignment = 0;                           // 0x1c  air.arg_type_align_size
    bool unused = false;                         // 0x20  air.arg_unused
    std::string semantic;                        // 0x28  the user name of a fragment input or a vertex output: "user(uv)"
    int interpolation = 0;                       // 0x48  0 perspective, 1 no_perspective, 2 flat
    int sampling = 0;                            // 0x4c  0 center, 1 centroid, 2 sample
    int depthQualifier = 0;                      // 0x50  1 greater, 2 less
    int index = 0;                               // 0x54  [[index(n)]] of a render target, for dual-source blending
    llvm::GlobalVariable *condition = nullptr;   // 0x58  air.function_constant_condition
    std::string typeName;                        // 0x60  air.arg_type_name
    std::string name;                            // 0x80  air.arg_name
    bool indirect = false;                       // 0xa0  air.indirect_buffer: an argument buffer
    std::vector<Member> members;                 // 0xa8  of an argument buffer
    int argumentBuffer = -1;                     // 0xc0  the argument buffer holding this resource, set by the emitter
    int argumentMember = 0;                      // 0xc4  its member there
};

// One entry of air.function_constants. (0x40 bytes)
struct FunctionConstant
{
    llvm::GlobalVariable *value = nullptr;       // 0x00
    llvm::GlobalVariable *defined = nullptr;     // 0x08  is_function_constant_defined
    int index = 0;                               // 0x10  [[function_constant(n)]]
    int type = 0;                                // 0x14  MTLDataType
    bool required = true;                        // 0x18  false only from the old form's flag
    std::string name;                            // 0x20
};

// What the metadata says of one function of a module. (0x60 bytes)
struct FunctionInfo
{
    int stage = 0;                               // 0x00  1 vertex, 2 fragment, 3 kernel, 5 visible
    llvm::Function *function = nullptr;          // 0x08
    std::vector<Argument> arguments;             // 0x10
    std::vector<Argument> outputs;               // 0x28
    std::vector<FunctionConstant> constants;     // 0x40
    bool earlyFragmentTests = false;             // 0x58
};

// The MTLDataType of a Metal type name ("float4", "half3x3", "texture2d<float>", "sampler"), 0 when unknown.
int dataType(const std::string &name);                                                           // 0xb7790
// A vector type is its scalar and its width; a matrix type is its column type and its column count.
bool splitDataType(int type, int &element, int &count);                                         // 0xb7ad0
// Bytes of a value of an MTLDataType in a buffer, 0 when it has no size.
int dataTypeSize(int type);                                                                      // 0xb7b90

// Reads the metadata of `name`. On failure, says why in `error`.
bool readFunction(const llvm::Module &module, const std::string &name, FunctionInfo &info, std::string &error); // 0xbe1d0

}
