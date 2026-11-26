// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Encoding of indirect command buffers: the air.*_render_command and air.*_compute_command calls write words into the buffer.

#include "emitter.h"

using namespace llvm;

namespace cutefish::reef::metal {

namespace {

// A command buffer is a word with the count of commands, then the commands. The words of a command:
// 0 kind, 1-6 the arguments of the draw or the dispatch, 7 barrier, 10-11 the pipeline state, 12 whether it is set,
// 13-14 masks of the vertex (or kernel) and fragment buffers that are set, from 16 three words per vertex buffer,
// from 109 two words per fragment buffer (and the threadgroup memory lengths of a compute command).
const int headerWords = 64;
const int commandWords = 192;

}

// A word of a render or compute pipeline state in an argument buffer.
std::string Emitter::pipelineWord(Value *pipeline, unsigned word)
{
    if (isa<LoadInst>(pipeline) && rootOf(pipeline).kind == 6) {
        auto *load = llvm::cast<LoadInst>(pipeline);
        Root &root = rootOf(load->getPointerOperand());
        Address address = addressOf(load->getPointerOperand());
        address.constant += word * 4;
        return loadValue(root, address, 4, Type::getInt32Ty(m_module.getContext()));
    }
    throw Unsupported("an indirect command takes a pipeline state that is not in an argument buffer");
}

// The GPU address of a buffer as two words.
std::string Emitter::bufferAddress(Value *pointer, bool high)
{
    if (isa<ConstantPointerNull>(pointer) || isa<UndefValue>(pointer))
        return "0u";
    Root &root = rootOf(pointer);
    if (root.kind != 0 || !root.argument)
        throw Unsupported("an indirect command takes an address that is not in a buffer");
    root.used = true;
    root.addressTaken = true;
    const std::string address = "reef_" + m_prefix + "ba" + std::to_string(root.index);
    if (high)
        return address + ".y";
    return "(" + address + ".x + " + offsetOf(addressOf(pointer)) + ")";
}

bool Emitter::emitCommand(CallInst &call, const std::string &name, const std::string &target)
{
    const bool render = name.ends_with("_render_command");
    const bool compute = name.ends_with("_compute_command");
    if (!render && !compute && name != "get_size_command_buffer")
        return false;

    Root &root = rootOf(call.getArgOperand(0));
    if (root.kind != 0 || !root.argument || root.argument->role != Role::CommandBuffer)
        throw Unsupported("an indirect command buffer that is not in an argument buffer");
    root.used = true;
    const std::string &buffer = root.name;
    if (name == "get_size_command_buffer") {
        line(target + " = " + buffer + "[" + std::to_string(0) + "];");
        return true;
    }

    const std::string base = "reef_cmd" + std::to_string(m_nextVariable++);
    line("uint " + base + " = " + std::to_string(headerWords) + "u + uint(" + valueOf(call.getArgOperand(1)) + ") * "
         + std::to_string(commandWords) + "u;");

    const auto operand = [&](unsigned index) {
        return call.getArgOperand(index);
    };
    const auto argument = [&](unsigned index) {
        return "uint(" + valueOf(operand(index)) + ")";
    };
    const auto component = [&](unsigned index, unsigned i) {
        return "uint(" + elementOf(operand(index), valueOf(operand(index)), i) + ")";
    };
    const auto literal = [](int value) {
        return std::to_string(value) + "u";
    };
    const auto write = [&](const std::string &word, const std::string &value) {
        line(buffer + "[" + base + " + " + word + "] = " + value + ";");
    };
    // The address of a buffer, its stride when it has one, and the bit of the buffer in the mask.
    const auto setBuffer = [&](int first, int words, int mask, int stride, int index) {
        const std::string number = argument(index);
        const std::string slot = "(" + literal(first) + " + " + number + " * " + std::to_string(words) + "u)";
        write(slot, bufferAddress(operand(2), false));
        write(slot + " + 1u", bufferAddress(operand(2), true));
        if (stride != -1)
            write(slot + " + 2u", argument(stride));
        write(literal(mask), buffer + "[" + base + " + " + literal(mask) + "] | (1u << " + number + ")");
    };

    const std::string operation = name.substr(0, name.find(render ? "_render_command" : "_compute_command"));
    if (operation == "reset") {
        write(literal(0), literal(1));
    } else if (operation == "set_pipeline_state") {
        write(literal(10), pipelineWord(operand(2), 0));
        write(literal(11), pipelineWord(operand(2), 1));
        write(literal(12), "1u");
    } else if (render && operation == "set_vertex_buffer") {
        setBuffer(16, 3, 13, 3, 4);
    } else if (compute && operation == "set_kernel_buffer") {
        setBuffer(16, 3, 13, 3, 4);
    } else if (render && operation == "set_fragment_buffer") {
        setBuffer(109, 2, 14, -1, 3);
    } else if (render && operation == "draw_primitives") {
        write(literal(1), argument(2));
        write(literal(2), argument(3));
        write(literal(3), argument(4));
        write(literal(4), argument(5));
        write(literal(5), argument(6));
        write(literal(0), literal(2));
    } else if (render && operation == "draw_indexed_primitives") {
        const bool wide = call.getCalledFunction()->getName().ends_with(".p1i32");
        write(literal(1), argument(2));
        write(literal(3), argument(3));
        write(literal(7), bufferAddress(operand(4), false));
        write(literal(8), bufferAddress(operand(4), true));
        write(literal(9), wide ? "4u" : "2u");
        write(literal(4), argument(5));
        write(literal(6), argument(6));
        write(literal(5), argument(7));
        write(literal(0), literal(3));
    } else if (compute && operation == "set_threadgroup_memory_length") {
        write("(" + literal(109) + " + " + argument(3) + ")", argument(2));
    } else if (compute && (operation == "concurrent_dispatch_threadgroups" || operation == "concurrent_dispatch_threads")) {
        for (unsigned i = 0; i < 3; ++i) {
            write(literal(1 + i), component(2, i));
            write(literal(4 + i), component(3, i));
        }
        write(literal(0), literal(operation == "concurrent_dispatch_threads" ? 5 : 4));
    } else if (compute && operation == "set_barrier") {
        write(literal(7), "1u");
    } else if (compute && operation == "clear_barrier") {
        write(literal(7), "0u");
    } else if (!(compute && operation == "set_imageblock_size")) {
        throw Unsupported("the indirect command air." + name + " is not supported");
    }
    return true;
}

}
