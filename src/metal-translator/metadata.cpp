// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The AIR metadata of a function: its arguments, outputs and function constants, and the Metal data types.

#include "metadata.h"

#include <cstdint>
#include <cstring>
#include <map>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>

namespace cutefish::reef::metal {

// The scalar Metal types by name; a vector type follows its scalar ("float4" is float + 3).
static const struct
{
    const char *name;
    int type;
} scalarTypes[] = {
    {"float", 3},  {"half", 16},  {"int", 29},  {"uint", 33},  {"short", 37}, {"ushort", 41},
    {"char", 45},  {"uchar", 49}, {"bool", 53}, {"long", 81},  {"ulong", 85}, {"bfloat", 121},
};

static const int scalars[] = {3, 16, 29, 33, 37, 41, 45, 49, 53, 81, 85, 121};

// The kinds of entry point, by the named metadata that lists them.
static const struct
{
    const char *name;
    int stage;
} stages[] = {
    {"air.vertex", 1},
    {"air.fragment", 2},
    {"air.kernel", 3},
};

// The operands of a metadata node, read one after another.
struct Operands
{
    const llvm::MDNode *node;
    unsigned next;
};

// Whether the next operand is an integer.
static bool atInteger(const Operands &operands)
{
    return operands.next < operands.node->getNumOperands()
        && llvm::mdconst::dyn_extract_or_null<llvm::ConstantInt>(operands.node->getOperand(operands.next));
}

// The next operand when it is a string; anything else stays the next operand and gives "".
static std::string nextString(Operands &operands)
{
    if (operands.next >= operands.node->getNumOperands())
        return {};
    const auto *string = llvm::dyn_cast_or_null<llvm::MDString>(operands.node->getOperand(operands.next));
    if (!string)
        return {};
    ++operands.next;
    return string->getString().str();
}

// The next operand when it is an integer; anything else stays the next operand and gives 0.
static int64_t nextInteger(Operands &operands)
{
    if (operands.next >= operands.node->getNumOperands())
        return 0;
    const auto *value = llvm::mdconst::dyn_extract_or_null<llvm::ConstantInt>(operands.node->getOperand(operands.next));
    if (!value)
        return 0;
    ++operands.next;
    return value->getSExtValue();
}

int dataType(const std::string &name)
{
    for (const auto &scalar : scalarTypes) {
        const size_t length = strlen(scalar.name);
        if (name.compare(0, length, scalar.name) != 0)
            continue;
        const std::string rest = name.substr(length);
        if (rest.empty())
            return scalar.type;
        if (rest.size() == 1) {
            if (rest[0] >= '2' && rest[0] <= '4')
                return scalar.type + rest[0] - '1';
        } else if ((scalar.type == 3 || scalar.type == 16) && rest.size() == 3 && rest[1] == 'x' && rest[0] >= '2' && rest[0] <= '4'
                   && rest[2] >= '2' && rest[2] <= '4') {
            // floatNxM and halfNxM follow the vectors: 2x2, 2x3, 2x4, 3x2 ...
            return scalar.type + 4 + (rest[0] - '2') * 3 + (rest[2] - '2');
        }
    }
    if (name == "sampler")
        return 59;
    if (name.starts_with("texture") || name.starts_with("depth"))
        return 58;
    return 0;
}

bool splitDataType(int type, int &element, int &count)
{
    for (const int scalar : scalars) {
        if (type >= scalar && type <= scalar + 3) {
            element = scalar;
            count = type - scalar + 1;
            return true;
        }
        if ((scalar == 3 || scalar == 16) && type > scalar + 3 && type <= scalar + 12) {
            const int matrix = type - scalar - 4;
            element = scalar + 1 + matrix % 3;
            count = matrix / 3 + 2;
            return true;
        }
    }
    return false;
}

int dataTypeSize(int type)
{
    int element = 0;
    int count = 0;
    if (!splitDataType(type, element, count))
        return 0;
    // A matrix is its columns.
    if ((element >= 4 && element <= 6) || (element >= 17 && element <= 19))
        return count * dataTypeSize(element);
    int size = 0;
    switch (element) {
    case 3:
    case 29:
    case 33:
        size = 4;
        break;
    case 16:
    case 37:
    case 41:
    case 121:
        size = 2;
        break;
    case 45:
    case 49:
    case 53:
        size = 1;
        break;
    case 81:
    case 85:
        size = 8;
        break;
    }
    // A vector of three takes the room of four.
    return (count == 3 ? 4 : count) * size;
}

// What each AIR kind of argument or output is.
static const std::map<std::string, Role> &roles()
{
    static const std::map<std::string, Role> roles = {
        {"air.buffer", Role::Buffer},
        {"air.indirect_buffer", Role::Buffer},
        {"air.texture", Role::Texture},
        {"air.sampler", Role::Sampler},
        {"air.threadgroup", Role::Threadgroup},
        {"air.vertex_input", Role::StageIn},
        {"air.stage_in", Role::StageIn},
        {"air.fragment_input", Role::FragmentInput},
        {"air.position", Role::Position},
        {"air.vertex_id", Role::VertexId},
        {"air.instance_id", Role::InstanceId},
        {"air.base_vertex", Role::BaseVertex},
        {"air.base_instance", Role::BaseInstance},
        {"air.front_facing", Role::FrontFacing},
        {"air.point_coord", Role::PointCoord},
        {"air.sample_id", Role::SampleId},
        {"air.sample_mask", Role::SampleMask},
        {"air.sample_mask_in", Role::SampleMask},
        {"air.primitive_id", Role::PrimitiveId},
        {"air.render_target_array_index", Role::RenderTargetArrayIndex},
        {"air.viewport_array_index", Role::ViewportArrayIndex},
        {"air.thread_position_in_grid", Role::ThreadPositionInGrid},
        {"air.thread_position_in_threadgroup", Role::ThreadPositionInThreadgroup},
        {"air.threadgroup_position_in_grid", Role::ThreadgroupPositionInGrid},
        {"air.threads_per_threadgroup", Role::ThreadsPerThreadgroup},
        {"air.threads_per_grid", Role::ThreadsPerGrid},
        {"air.threadgroups_per_grid", Role::ThreadgroupsPerGrid},
        {"air.thread_index_in_threadgroup", Role::ThreadIndexInThreadgroup},
        {"air.thread_index_in_simdgroup", Role::ThreadIndexInSimdgroup},
        {"air.thread_index_in_quadgroup", Role::ThreadIndexInSimdgroup},
        {"air.threads_per_simdgroup", Role::ThreadsPerSimdgroup},
        {"air.thread_execution_width", Role::ThreadsPerSimdgroup},
        {"air.simdgroup_index_in_threadgroup", Role::SimdgroupIndexInThreadgroup},
        {"air.simdgroups_per_threadgroup", Role::SimdgroupsPerThreadgroup},
        {"air.vertex_output", Role::VertexOutput},
        {"air.point_size", Role::PointSize},
        {"air.clip_distance", Role::ClipDistance},
        {"air.render_target", Role::RenderTarget},
        {"air.depth", Role::Depth},
        {"air.stencil", Role::Stencil},
        {"air.color", Role::Color},
        {"air.indirect_constant", Role::IndirectConstant},
        {"air.command_buffer", Role::CommandBuffer},
        {"air.render_pipeline_state", Role::RenderPipelineState},
        {"air.compute_pipeline_state", Role::ComputePipelineState},
    };
    return roles;
}

static bool readArgument(const llvm::MDNode *node, bool numbered, Argument &argument, std::string &error);

// The fields of air.struct_type_info: offset, size, array length, type name and name, then attributes with their values. The
// fields with an air.indirect_argument become members; a structure inside is read with its offset added.
static void readStructure(const llvm::MDNode *structure, int offset, std::vector<Member> &members)
{
    Operands operands{structure, 0};
    while (operands.next < structure->getNumOperands()) {
        if (!atInteger(operands)) {
            ++operands.next;
            continue;
        }
        Member member;
        member.offset = offset + nextInteger(operands);
        member.size = nextInteger(operands);
        nextInteger(operands);
        member.typeName = nextString(operands);
        member.name = nextString(operands);
        const llvm::MDNode *indirect = nullptr;
        const llvm::MDNode *nested = nullptr;
        // The attributes end where the offset of the next field is.
        while (operands.next < structure->getNumOperands() && !atInteger(operands)) {
            const std::string attribute = nextString(operands);
            if (attribute.empty()) {
                ++operands.next;
                continue;
            }
            const llvm::MDNode *value = nullptr;
            if (operands.next < structure->getNumOperands())
                value = llvm::dyn_cast_or_null<llvm::MDNode>(structure->getOperand(operands.next));
            if (value) {
                if (attribute == "air.indirect_argument")
                    indirect = value;
                else if (attribute == "air.struct_type_info")
                    nested = value;
                ++operands.next;
            } else if (atInteger(operands)) {
                ++operands.next;
            }
        }
        Argument argument;
        std::string error;
        if (indirect && readArgument(indirect, true, argument, error)) {
            member.role = argument.role;
            member.location = argument.location;
            member.count = argument.count;
            member.access = argument.access;
            member.addressSpace = argument.addressSpace;
            member.typeSize = argument.size;
            member.alignment = argument.alignment;
            member.indirect = argument.indirect;
            members.push_back(member);
        } else if (nested) {
            readStructure(nested, member.offset, members);
        }
    }
}

// The attributes of an argument or an output after its kind, up to the end of its metadata.
static void readAttributes(Operands &operands, Argument &argument)
{
    while (operands.next < operands.node->getNumOperands()) {
        // A number that no attribute took.
        if (atInteger(operands)) {
            ++operands.next;
            continue;
        }
        const std::string name = nextString(operands);
        if (name.empty()) {
            ++operands.next;
            continue;
        }
        if (name == "air.function_constant_condition") {
            argument.condition = nullptr;
            if (operands.next < operands.node->getNumOperands())
                argument.condition = llvm::mdconst::dyn_extract_or_null<llvm::GlobalVariable>(operands.node->getOperand(operands.next));
            ++operands.next;
        } else if (name == "air.location_index") {
            argument.location = nextInteger(operands);
            if (atInteger(operands))
                argument.count = nextInteger(operands);
        } else if (name == "air.read") {
            argument.access |= 1;
        } else if (name == "air.write") {
            argument.access |= 2;
        } else if (name == "air.read_write") {
            argument.access |= 3;
        } else if (name == "air.sample") {
            // A texture that is sampled; an input interpolated at each sample.
            if (argument.role == Role::FragmentInput)
                argument.sampling = 2;
            else
                argument.access |= 4;
        } else if (name == "air.center") {
            argument.sampling = 0;
        } else if (name == "air.centroid") {
            argument.sampling = 1;
        } else if (name == "air.perspective") {
            argument.interpolation = 0;
        } else if (name == "air.no_perspective") {
            argument.interpolation = 1;
        } else if (name == "air.flat") {
            argument.interpolation = 2;
        } else if (name == "air.address_space") {
            argument.addressSpace = nextInteger(operands);
        } else if (name == "air.arg_type_size") {
            argument.size = nextInteger(operands);
        } else if (name == "air.arg_type_align_size") {
            argument.alignment = nextInteger(operands);
        } else if (name == "air.arg_type_name") {
            argument.typeName = nextString(operands);
        } else if (name == "air.arg_name") {
            argument.name = nextString(operands);
        } else if (name == "air.arg_unused") {
            argument.unused = true;
        } else if (name == "air.depth_qualifier") {
            const std::string qualifier = nextString(operands);
            if (qualifier == "air.greater" || qualifier == "greater")
                argument.depthQualifier = 1;
            else if (qualifier == "air.less" || qualifier == "less")
                argument.depthQualifier = 2;
            else
                argument.depthQualifier = 0;
        } else if (name == "air.struct_type_info") {
            const llvm::MDNode *structure = nullptr;
            if (operands.next < operands.node->getNumOperands())
                structure = llvm::dyn_cast_or_null<llvm::MDNode>(operands.node->getOperand(operands.next));
            if (structure && argument.indirect)
                readStructure(structure, 0, argument.members);
            ++operands.next;
        } else if (name == "air.arg_type_info") {
            ++operands.next;
        }
    }
}

// One entry of the arguments ({i32 parameter, !"air.buffer", ...}, `numbered`) or of the outputs ({!"air.position", ...}).
static bool readArgument(const llvm::MDNode *node, bool numbered, Argument &argument, std::string &error)
{
    Operands operands{node, 0};
    if (numbered) {
        if (!atInteger(operands)) {
            error = "an argument's metadata has no argument number";
            return false;
        }
        argument.number = nextInteger(operands);
    }
    std::string kind = nextString(operands);
    // A function constant in front of the kind.
    if (kind == "air.function_constant") {
        ++operands.next;
        kind = nextString(operands);
    }
    const auto found = roles().find(kind);
    if (found == roles().end()) {
        error = "unknown argument kind " + kind;
        return false;
    }
    argument.role = found->second;
    argument.indirect = kind == "air.indirect_buffer";
    switch (argument.role) {
    case Role::FragmentInput:
    case Role::VertexOutput:
        argument.semantic = nextString(operands);
        break;
    case Role::RenderTarget:
        argument.location = nextInteger(operands);
        if (atInteger(operands))
            argument.index = nextInteger(operands);
        break;
    case Role::Color:
        argument.location = nextInteger(operands);
        break;
    default:
        break;
    }
    readAttributes(operands, argument);
    if (argument.role == Role::Buffer && argument.addressSpace == 3)
        argument.role = Role::Threadgroup;
    return true;
}

// The entries of air.function_constants that name a global: {global, !"air.function_constant", i32 index, attributes},
// {global, !"air.function_constant_defined", i32 index, attributes} and the old {global, !"type", !"name", i32 index, i1 required}.
static bool readConstants(const llvm::Module &module, FunctionInfo &info, std::string &error)
{
    const llvm::NamedMDNode *constants = module.getNamedMetadata("air.function_constants");
    if (!constants)
        return true;
    // Where each index is in info.constants.
    std::map<int, size_t> positions;
    for (unsigned i = 0; i < constants->getNumOperands(); ++i) {
        const llvm::MDNode *entry = constants->getOperand(i);
        if (entry->getNumOperands() < 3)
            continue;
        llvm::GlobalVariable *global = nullptr;
        if (const auto *value = llvm::dyn_cast_or_null<llvm::ValueAsMetadata>(entry->getOperand(0)))
            global = llvm::dyn_cast<llvm::GlobalVariable>(value->getValue()->stripPointerCasts());
        Operands operands{entry, 1};
        const std::string kind = nextString(operands);
        if (!global || kind.empty()) {
            error = "unreadable function constant metadata";
            return false;
        }
        Argument argument;
        int index = 0;
        bool required = true;
        if (kind == "air.function_constant" || kind == "air.function_constant_defined") {
            index = nextInteger(operands);
            readAttributes(operands, argument);
        } else {
            argument.typeName = kind;
            argument.name = nextString(operands);
            index = nextInteger(operands);
            required = nextInteger(operands) != 0;
        }
        const auto [position, added] = positions.try_emplace(index, info.constants.size());
        if (added) {
            FunctionConstant constant;
            constant.index = index;
            info.constants.push_back(std::move(constant));
        }
        FunctionConstant &constant = info.constants[position->second];
        if (!argument.name.empty())
            constant.name = argument.name;
        if (kind == "air.function_constant_defined") {
            // The function asks whether it is defined: it may be left out.
            constant.defined = global;
            constant.required = false;
        } else {
            constant.value = global;
            constant.type = dataType(argument.typeName);
            constant.required = constant.required && required;
        }
    }
    return true;
}

bool readFunction(const llvm::Module &module, const std::string &name, FunctionInfo &info, std::string &error)
{
    for (const auto &stage : stages) {
        const llvm::NamedMDNode *functions = module.getNamedMetadata(stage.name);
        if (!functions || !functions->getNumOperands())
            continue;
        for (unsigned i = 0; i < functions->getNumOperands(); ++i) {
            const llvm::MDNode *entry = functions->getOperand(i);
            if (entry->getNumOperands() < 3)
                continue;
            const auto *value = llvm::dyn_cast_or_null<llvm::ValueAsMetadata>(entry->getOperand(0));
            if (!value)
                continue;
            auto *function = llvm::dyn_cast<llvm::Function>(value->getValue());
            if (!function || function->getName() != name)
                continue;
            info = FunctionInfo();
            info.stage = stage.stage;
            info.function = function;
            const auto *outputs = llvm::dyn_cast_or_null<llvm::MDNode>(entry->getOperand(1));
            const auto *arguments = llvm::dyn_cast_or_null<llvm::MDNode>(entry->getOperand(2));
            if (!outputs || !arguments) {
                error = "the metadata of " + name + " is not a list of outputs and arguments";
                return false;
            }
            // The outputs of a returned structure are its members.
            const bool returnsStructure = function->getReturnType()->isStructTy();
            for (unsigned j = 0; j < outputs->getNumOperands(); ++j) {
                const auto *node = llvm::dyn_cast_or_null<llvm::MDNode>(outputs->getOperand(j));
                if (!node) {
                    error = "an output of " + name + " has no metadata";
                    return false;
                }
                Argument output;
                if (!readArgument(node, false, output, error))
                    return false;
                output.number = returnsStructure ? int(j) : -1;
                info.outputs.push_back(output);
            }
            for (unsigned j = 0; j < arguments->getNumOperands(); ++j) {
                const auto *node = llvm::dyn_cast_or_null<llvm::MDNode>(arguments->getOperand(j));
                if (!node) {
                    error = "an argument of " + name + " has no metadata";
                    return false;
                }
                Argument argument;
                if (!readArgument(node, true, argument, error))
                    return false;
                if (argument.number < 0 || size_t(argument.number) >= function->arg_size()) {
                    error = "an argument of " + name + " names no parameter";
                    return false;
                }
                info.arguments.push_back(argument);
            }
            // Attributes of the function: a string, or a node that starts with one.
            for (unsigned j = 3; j < entry->getNumOperands(); ++j) {
                const llvm::Metadata *attribute = entry->getOperand(j);
                if (const auto *node = llvm::dyn_cast_or_null<llvm::MDNode>(attribute))
                    attribute = node->getNumOperands() ? node->getOperand(0).get() : nullptr;
                const auto *string = llvm::dyn_cast_or_null<llvm::MDString>(attribute);
                if (string && string->getString() == "air.early_fragment_tests")
                    info.earlyFragmentTests = true;
            }
            return readConstants(module, info, error);
        }
    }
    // A visible function has no inputs or outputs of a stage: {function}.
    if (const llvm::NamedMDNode *functions = module.getNamedMetadata("air.visible")) {
        for (unsigned i = 0; i < functions->getNumOperands(); ++i) {
            const llvm::MDNode *entry = functions->getOperand(i);
            if (!entry->getNumOperands())
                continue;
            const auto *value = llvm::dyn_cast_or_null<llvm::ValueAsMetadata>(entry->getOperand(0));
            if (!value)
                continue;
            auto *function = llvm::dyn_cast<llvm::Function>(value->getValue());
            if (!function || function->getName() != name)
                continue;
            info = FunctionInfo();
            info.stage = 5;
            info.function = function;
            return readConstants(module, info, error);
        }
    }
    error = "the library has no entry point " + name;
    return false;
}

}
