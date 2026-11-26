// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Basic blocks to statements, and the shader around them: declarations and main().
#include "emitter.h"

#include <algorithm>

#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/IR/CFG.h>

namespace cutefish::reef::metal {

void Emitter::writeInstructions(llvm::BasicBlock *block)
{
    for (llvm::Instruction &instruction : *block) {
        if (!instruction.isTerminator())
            this->instruction(instruction);
    }
}

llvm::BasicBlock *Emitter::mergeOf(llvm::BasicBlock *block, const Flow &flow)
{
    auto it = m_merges.find(block);
    if (it == m_merges.end())
        return nullptr;
    llvm::BasicBlock *merge = it->second;
    if (!merge)
        return nullptr;
    // Inside a loop only what stays in it or leaves it by the break.
    if (flow.loop && !flow.loop->contains(merge) && merge != flow.exit)
        return nullptr;
    return merge;
}

llvm::BasicBlock *Emitter::commonMerge(const std::vector<llvm::BasicBlock *> &blocks, const Flow &flow)
{
    std::vector<std::vector<llvm::BasicBlock *>> chains;
    for (llvm::BasicBlock *block : blocks) {
        std::vector<llvm::BasicBlock *> chain;
        while (block && std::find(chain.begin(), chain.end(), block) == chain.end()) {
            chain.push_back(block);
            block = mergeOf(block, flow);
        }
        chains.push_back(std::move(chain));
    }
    for (llvm::BasicBlock *block : chains.front()) {
        bool common = true;
        for (const auto &chain : chains)
            common &= std::find(chain.begin(), chain.end(), block) != chain.end();
        if (common)
            return block;
    }
    return nullptr;
}

bool Emitter::isPresent(const Argument &output)
{
    if (!output.condition)
        return true;
    if (!output.condition->isDeclaration()) {
        if (auto *value = llvm::dyn_cast<llvm::ConstantInt>(output.condition->getInitializer()))
            return !value->isZero();
    }
    throw Unsupported("the function constant deciding output " + output.name + " has no value");
}

void Emitter::findMerges()
{
    std::vector<llvm::BasicBlock *> blocks;
    std::map<llvm::BasicBlock *, size_t> index;
    for (llvm::BasicBlock &block : *m_function) {
        index[&block] = blocks.size();
        blocks.push_back(&block);
    }
    const size_t count = blocks.size();

    // Blocks that leave the function after a few instructions are no place to merge: each branch
    // to them gets its own copy.
    constexpr size_t none = SIZE_MAX;
    std::vector<size_t> lengths(count, none);
    bool changed;
    do {
        changed = false;
        for (size_t i = 0; i < count; ++i) {
            llvm::BasicBlock *block = blocks[i];
            llvm::Instruction *terminator = block->getTerminator();
            const bool leaves = llvm::isa<llvm::ReturnInst>(terminator) || llvm::isa<llvm::UnreachableInst>(terminator);
            size_t length = none;
            if (leaves) {
                length = block->size();
            } else if (auto *branch = llvm::dyn_cast<llvm::BranchInst>(terminator); branch && branch->isUnconditional()) {
                const size_t next = lengths[index[branch->getSuccessor(0)]];
                if (next != none && !m_loops->getLoopFor(block))
                    length = next + block->size();
            }
            if (length > 24 || (!block->phis().empty() && !leaves))
                length = none;
            if (lengths[i] != length) {
                lengths[i] = length;
                changed = true;
            }
        }
    } while (changed);
    for (size_t i = 0; i < count; ++i) {
        if (lengths[i] != none)
            m_tails.insert(blocks[i]);
    }

    // Post-dominators. The last one is the end of the function.
    std::vector<std::vector<bool>> post(count + 1, std::vector<bool>(count + 1, true));
    post[count].assign(count + 1, false);
    post[count][count] = true;
    do {
        changed = false;
        for (size_t i = count; i-- > 0;) {
            std::vector<bool> common(count + 1, true);
            std::vector<size_t> successors;
            if (m_tails.count(blocks[i])) {
                successors.push_back(count);
            } else {
                bool leaves = false;
                for (llvm::BasicBlock *successor : llvm::successors(blocks[i])) {
                    if (m_tails.count(successor))
                        leaves = true;
                    else
                        successors.push_back(index[successor]);
                }
                if (leaves || successors.empty())
                    successors.push_back(count);
            }
            for (size_t successor : successors) {
                for (size_t j = 0; j <= count; ++j)
                    common[j] = common[j] && post[successor][j];
            }
            common[i] = true;
            if (common != post[i]) {
                post[i] = common;
                changed = true;
            }
        }
    } while (changed);

    // The immediate one is the one with the most post-dominators of its own.
    for (size_t i = 0; i < blocks.size(); ++i) {
        size_t best = none;
        size_t depth = 0;
        for (size_t j = 0; j < count; ++j) {
            if (j == i || !post[i][j])
                continue;
            const size_t size = std::count(post[j].begin(), post[j].end(), true);
            if (size > depth) {
                depth = size;
                best = j;
            }
        }
        m_merges[blocks[i]] = best == none ? nullptr : blocks[best];
    }
}

std::string Emitter::caseCondition(llvm::SwitchInst *select, llvm::BasicBlock *target)
{
    const std::string value = valueOf(select->getCondition());
    llvm::Type *type = select->getCondition()->getType();
    std::string condition;
    for (const auto &branch : select->cases()) {
        if (branch.getCaseSuccessor() != target)
            continue;
        const std::string test = type->isIntegerTy(1)
            ? (branch.getCaseValue()->isZero() ? "!" + value : value)
            : "(" + value + " == " + integer(branch.getCaseValue()->getZExtValue(), type) + ")";
        condition += (condition.empty() ? "" : " || ") + test;
    }
    return condition;
}

void Emitter::copyPhis(llvm::BasicBlock *from, llvm::BasicBlock *to)
{
    std::vector<std::pair<std::string, std::string>> copies;
    bool overlap = false;
    for (llvm::PHINode &phi : to->phis()) {
        llvm::Value *value = phi.getIncomingValueForBlock(from);
        if (!value || llvm::isa<llvm::UndefValue>(value))
            continue;
        if (phi.getType()->isPointerTy()) {
            // Textures and samplers are no values in GLSL: their uses name them.
            const Root &root = rootOf(&phi);
            if (root.kind == Root::Texture || root.kind == Root::Sampler)
                continue;
        }
        if (auto *other = llvm::dyn_cast<llvm::PHINode>(value))
            overlap |= other->getParent() == to && other != &phi;
        copies.emplace_back(nameOf(&phi), valueOf(value));
    }
    if (overlap) {
        // The copies happen at the same time: one must not see what another wrote.
        std::vector<std::string> temporaries;
        for (size_t i = 0; i < copies.size(); ++i) {
            const std::string name = "reef_k" + std::to_string(m_nextVariable++);
            std::string type;
            for (const auto &variable : m_variables) {
                if (variable.second == copies[i].first)
                    type = variable.first;
            }
            m_variables.emplace_back(type, name);
            line(name + " = " + copies[i].second + ";");
            temporaries.push_back(name);
        }
        for (size_t i = 0; i < copies.size(); ++i)
            line(copies[i].first + " = " + temporaries[i] + ";");
    } else {
        for (const auto &[name, value] : copies) {
            if (name != value)
                line(name + " = " + value + ";");
        }
    }
}

// The variable of a colour output.
static std::string colorName(const Argument &output)
{
    return "reef_color" + std::to_string(output.location) + (output.index ? "_" + std::to_string(output.index) : "");
}

// What a colour output holds: 1 signed integers, 2 unsigned integers, 0 anything else.
static inline int colorKind(const std::string &typeName)
{
    enum { Int = 29, UInt = 33, Short = 37, UShort = 41, Char = 45, UChar = 49 };
    int element = 0;
    int count = 0;
    splitDataType(dataType(typeName), element, count);
    if (element == Int || element == Short || element == Char)
        return 1;
    return element == UInt || element == UShort || element == UChar ? 2 : 0;
}

void Emitter::writeReturn(llvm::ReturnInst *instruction)
{
    llvm::Value *value = instruction->getReturnValue();
    if (value && !m_info.outputs.empty()) {
        std::string text = valueOf(value);
        if (llvm::isa<llvm::Constant>(value) && value->getType()->isStructTy()) {
            const std::string name = "reef_k" + std::to_string(m_nextVariable++);
            m_variables.emplace_back(typeName(value->getType()), name);
            line(name + " = " + text + ";");
            text = name;
        }
        llvm::Type *type = value->getType();
        for (const Argument &output : m_info.outputs) {
            if (!isPresent(output))
                continue;
            const std::string part = output.number >= 0 ? text + ".m" + std::to_string(output.number) : text;
            llvm::Type *partType = output.number >= 0 ? llvm::cast<llvm::StructType>(type)->getElementType(output.number) : type;
            switch (output.role) {
            case Role::Position:
                line("gl_Position = " + part + ";");
                break;
            case Role::PointSize:
                line("gl_PointSize = " + part + ";");
                break;
            case Role::ClipDistance:
                if (auto *array = llvm::dyn_cast<llvm::ArrayType>(partType)) {
                    for (uint64_t i = 0; i < array->getNumElements(); ++i)
                        line("gl_ClipDistance[" + std::to_string(i) + "] = " + part + "[" + std::to_string(i) + "];");
                } else {
                    line("gl_ClipDistance[0] = " + part + ";");
                }
                break;
            case Role::RenderTargetArrayIndex:
                line("gl_Layer = int(" + part + ");");
                break;
            case Role::ViewportArrayIndex:
                line("gl_ViewportIndex = int(" + part + ");");
                break;
            case Role::VertexOutput: {
                const std::string varying = scalar(partType)->isIntegerTy(1)
                    ? typeName(partType).replace(0, 4, width(partType) > 1 ? "uvec" : "uint") + "(" + part + ")"
                    : part;
                line("reef_v_" + identifier(output.semantic) + " = " + varying + ";");
                break;
            }
            case Role::RenderTarget: {
                const int kind = colorKind(output.typeName);
                std::string color = part;
                if (kind == 1)
                    color = (width(partType) > 1 ? "ivec" + std::to_string(width(partType)) : "int") + "(" + asSigned(part, partType) + ")";
                line(colorName(output) + " = " + color + ";");
                break;
            }
            case Role::Depth:
                line("gl_FragDepth = " + part + ";");
                break;
            case Role::SampleMask:
                line("gl_SampleMask[0] = int(" + part + ");");
                break;
            case Role::Stencil:
                line("gl_FragStencilRefARB = int(" + part + ");");
                break;
            default:
                throw Unsupported("an output of an unsupported kind");
            }
        }
    }
    line("return;");
}

void Emitter::writeTerminator(llvm::Instruction *terminator, const Flow &flow)
{
    if (auto *instruction = llvm::dyn_cast<llvm::ReturnInst>(terminator)) {
        writeReturn(instruction);
        return;
    }
    if (llvm::isa<llvm::UnreachableInst>(terminator)) {
        line("return;");
        return;
    }
    llvm::BasicBlock *block = terminator->getParent();
    if (auto *branch = llvm::dyn_cast<llvm::BranchInst>(terminator)) {
        if (branch->isUnconditional() || branch->getSuccessor(0) == branch->getSuccessor(1)) {
            writeEdge(block, branch->getSuccessor(0), flow);
            return;
        }
        llvm::BasicBlock *merge = mergeOf(block, flow);
        Flow inner = flow;
        if (merge)
            inner.next = merge;
        line("if (" + valueOf(branch->getCondition()) + ") {");
        ++m_indent;
        writeEdge(block, branch->getSuccessor(0), inner);
        --m_indent;
        line("} else {");
        ++m_indent;
        writeEdge(block, branch->getSuccessor(1), inner);
        --m_indent;
        line("}");
        if (merge)
            writeBranch(merge, flow);
        return;
    }
    if (auto *select = llvm::dyn_cast<llvm::SwitchInst>(terminator)) {
        llvm::BasicBlock *merge = mergeOf(block, flow);
        Flow inner = flow;
        if (merge)
            inner.next = merge;
        std::vector<llvm::BasicBlock *> targets;
        for (const auto &branch : select->cases()) {
            llvm::BasicBlock *target = branch.getCaseSuccessor();
            if (target != select->getDefaultDest() && std::find(targets.begin(), targets.end(), target) == targets.end())
                targets.push_back(target);
        }
        bool first = true;
        for (llvm::BasicBlock *target : targets) {
            line((first ? "if (" : "} else if (") + caseCondition(select, target) + ") {");
            first = false;
            ++m_indent;
            writeEdge(block, target, inner);
            --m_indent;
        }
        if (!first) {
            line("} else {");
            ++m_indent;
        }
        writeEdge(block, select->getDefaultDest(), inner);
        if (!first) {
            --m_indent;
            line("}");
        }
        if (merge)
            writeBranch(merge, flow);
        return;
    }
    throw Unsupported(std::string("the terminator ") + terminator->getOpcodeName() + " is not supported");
}

void Emitter::writeBlock(llvm::BasicBlock *block, const Flow &flow)
{
    llvm::Loop *loop = m_loops->getLoopFor(block);
    if (loop && loop->getHeader() == block && loop != flow.loop) {
        writeLoop(loop, flow);
        return;
    }
    if (loop != flow.loop)
        throw Unstructured();
    if (!m_written.insert(block).second) {
        m_repeated += block->size();
        if (m_repeated > m_repeatLimit)
            throw Unstructured();
        for (llvm::Instruction &instruction : *block)
            m_addresses.erase(&instruction);
    }
    writeInstructions(block);
    writeTerminator(block->getTerminator(), flow);
}

void Emitter::writeBranch(llvm::BasicBlock *target, const Flow &flow)
{
    if (target == flow.next)
        return;
    if (m_tails.count(target)) {
        Flow none;
        writeBlock(target, none);
        return;
    }
    if (flow.loop) {
        if (target == flow.loop->getHeader()) {
            line("continue;");
            return;
        }
        if (target == flow.exit) {
            line("break;");
            return;
        }
        auto it = std::find(flow.exits.begin(), flow.exits.end(), target);
        if (it != flow.exits.end()) {
            line(flow.selector + " = " + std::to_string(it - flow.exits.begin() + 1) + "u;");
            line("break;");
            return;
        }
        if (!flow.loop->contains(target))
            throw Unstructured();
    }
    writeBlock(target, flow);
}

void Emitter::writeEdge(llvm::BasicBlock *from, llvm::BasicBlock *to, const Flow &flow)
{
    copyPhis(from, to);
    writeBranch(to, flow);
}

void Emitter::writeLoop(llvm::Loop *loop, const Flow &flow)
{
    llvm::SmallVector<llvm::BasicBlock *, 4> exits;
    loop->getUniqueExitBlocks(exits);
    Flow inner;
    llvm::BasicBlock *exit = nullptr;
    for (llvm::BasicBlock *block : exits) {
        if (m_tails.count(block))
            continue;
        if (!exit)
            exit = block;
        else
            inner.exits.push_back(block);
    }
    if (!inner.exits.empty()) {
        inner.selector = "reef_k" + std::to_string(m_nextVariable++);
        line("uint " + inner.selector + " = 0u;");
    }
    line("while (true) {");
    ++m_indent;
    inner.loop = loop;
    inner.exit = exit;
    writeBlock(loop->getHeader(), inner);
    --m_indent;
    line("}");
    if (!exit)
        return;
    if (inner.exits.empty()) {
        writeBranch(exit, flow);
        return;
    }
    std::vector<llvm::BasicBlock *> targets{exit};
    targets.insert(targets.end(), inner.exits.begin(), inner.exits.end());
    llvm::BasicBlock *merge = commonMerge(targets, flow);
    Flow after = flow;
    after.next = merge;
    for (size_t i = 0; i < inner.exits.size(); ++i) {
        line((i == 0 ? "if (" : "} else if (") + inner.selector + " == " + std::to_string(i + 1) + "u) {");
        ++m_indent;
        writeBranch(inner.exits[i], after);
        --m_indent;
    }
    line("} else {");
    ++m_indent;
    writeBranch(exit, after);
    --m_indent;
    line("}");
    if (merge)
        writeBranch(merge, flow);
}

std::string Emitter::structured()
{
    m_body.str("");
    m_indent = 1;
    size_t count = 0;
    for (llvm::BasicBlock &block : *m_function)
        count += block.size();
    m_repeatLimit = count * 2 + 64;
    findMerges();
    writeBlock(&m_function->getEntryBlock(), Flow{});
    return m_body.str();
}

std::string Emitter::stateMachine()
{
    m_body.str("");
    m_indent = 1;
    llvm::ReversePostOrderTraversal<llvm::Function *> order(m_function);
    int count = 0;
    for (llvm::BasicBlock *block : order)
        m_numbers[block] = count++;
    auto number = [this](llvm::BasicBlock *block) {
        return std::to_string(m_numbers.at(block)) + "u";
    };
    m_variables.emplace_back("uint", "reef_block");
    line("reef_block = 0u;");
    line("while (true) {");
    ++m_indent;
    line("switch (reef_block) {");
    for (llvm::BasicBlock *block : order) {
        line("case " + number(block) + ": {");
        ++m_indent;
        writeInstructions(block);
        llvm::Instruction *terminator = block->getTerminator();
        auto jump = [&](llvm::BasicBlock *target) {
            copyPhis(block, target);
            line("reef_block = " + number(target) + ";");
        };
        if (auto *instruction = llvm::dyn_cast<llvm::ReturnInst>(terminator)) {
            writeReturn(instruction);
        } else if (llvm::isa<llvm::UnreachableInst>(terminator)) {
            line("return;");
        } else if (auto *branch = llvm::dyn_cast<llvm::BranchInst>(terminator)) {
            if (branch->isUnconditional()) {
                jump(branch->getSuccessor(0));
            } else {
                line("if (" + valueOf(branch->getCondition()) + ") {");
                ++m_indent;
                jump(branch->getSuccessor(0));
                --m_indent;
                line("} else {");
                ++m_indent;
                jump(branch->getSuccessor(1));
                --m_indent;
                line("}");
            }
            line("continue;");
        } else if (auto *select = llvm::dyn_cast<llvm::SwitchInst>(terminator)) {
            std::vector<llvm::BasicBlock *> targets;
            for (const auto &branch : select->cases()) {
                llvm::BasicBlock *target = branch.getCaseSuccessor();
                if (target != select->getDefaultDest() && std::find(targets.begin(), targets.end(), target) == targets.end())
                    targets.push_back(target);
            }
            bool first = true;
            for (llvm::BasicBlock *target : targets) {
                line((first ? "if (" : "} else if (") + caseCondition(select, target) + ") {");
                first = false;
                ++m_indent;
                jump(target);
                --m_indent;
            }
            if (!first) {
                line("} else {");
                ++m_indent;
            }
            jump(select->getDefaultDest());
            if (!first) {
                --m_indent;
                line("}");
            }
            line("continue;");
        } else {
            throw Unsupported(std::string("the terminator ") + terminator->getOpcodeName() + " is not supported");
        }
        --m_indent;
        line("}");
    }
    line("default:");
    line("    return;");
    line("}");
    --m_indent;
    line("}");
    return m_body.str();
}

// The kinds of AIR textures and their GLSL names.
static const struct
{
    const char *air;
    const char *glsl;
    int type;
} textureKinds[] = {
    {"1d", "1D", 0},
    {"1d_array", "1DArray", 1},
    {"2d", "2D", 2},
    {"2d_array", "2DArray", 3},
    {"2d_ms", "2DMS", 4},
    {"cube", "Cube", 5},
    {"cube_array", "CubeArray", 6},
    {"3d", "3D", 7},
    {"2d_ms_array", "2DMSArray", 8},
    {"_buffer", "Buffer", 9},
    {"buffer", "Buffer", 9},
};

// A string that lives as long as the shader, for the structures of the C interface.
static const char *keep(Shader &shader, std::string text)
{
    shader.strings.push_back(std::move(text));
    return shader.strings.back().c_str();
}

void Emitter::assemble(const std::string &body)
{
    std::vector<std::string> declarations;

    llvm::Type *returned = m_function->getReturnType();
    const int stage = m_info.stage;
    for (const Argument &output : m_info.outputs) {
        if (!isPresent(output))
            continue;
        llvm::Type *type = output.number >= 0 ? llvm::cast<llvm::StructType>(returned)->getElementType(output.number) : returned;
        switch (output.role) {
        case Role::SampleMask:
            m_shader.flags |= 0x400;
            break;
        case Role::RenderTargetArrayIndex:
            m_shader.flags |= 0x40;
            if (stage == 1)
                require("GL_ARB_shader_viewport_layer_array");
            break;
        case Role::ViewportArrayIndex:
            m_shader.flags |= 0x80;
            if (stage == 1)
                require("GL_ARB_shader_viewport_layer_array");
            break;
        case Role::VertexOutput: {
            if (type->isPointerTy() || type->isStructTy() || type->isArrayTy())
                throw Unsupported("vertex output " + output.name + " has an unsupported type");
            const std::string name = "reef_v_" + identifier(output.semantic);
            const bool flat = isInteger(type) || scalar(type)->isDoubleTy();
            declarations.push_back(std::string(flat ? "flat " : "") + "out " + interfaceType(type) + " " + name + ";");
            m_shader.varyings.push_back({dataType(output.typeName), flat ? 2 : 0, 0, 0, keep(m_shader, output.semantic), keep(m_shader, name)});
            break;
        }
        case Role::PointSize:
            m_shader.flags |= 0x10;
            break;
        case Role::ClipDistance: {
            const int count = type->isArrayTy() ? type->getArrayNumElements() : 1;
            m_shader.flags |= 0x20;
            m_shader.clipDistances = count;
            declarations.push_back("out float gl_ClipDistance[" + std::to_string(count) + "];");
            break;
        }
        case Role::RenderTarget: {
            const int kind = colorKind(output.typeName);
            const unsigned components = width(type);
            const std::string count = components > 1 ? std::to_string(components) : "";
            const std::string glsl = kind == 1 ? (components > 1 ? "ivec" + count : "int")
                : kind == 2                    ? (components > 1 ? "uvec" + count : "uint")
                                               : typeName(type);
            const std::string index = output.index ? ", index = " + std::to_string(output.index) : "";
            declarations.push_back("layout(location = " + std::to_string(output.location) + index + ") out " + glsl + " " + colorName(output) + ";");
            if (!output.index)
                m_shader.renderTargets.push_back({output.location, dataType(output.typeName)});
            break;
        }
        case Role::Depth:
            m_shader.flags |= 0x200;
            if (output.depthQualifier == 1)
                declarations.push_back("layout(depth_greater) out float gl_FragDepth;");
            else if (output.depthQualifier == 2)
                declarations.push_back("layout(depth_less) out float gl_FragDepth;");
            break;
        case Role::Stencil:
            m_shader.flags |= 0x800;
            require("GL_ARB_shader_stencil_export");
            break;
        default:
            break;
        }
    }

    // Buffers, by their Metal slot.
    std::vector<Root *> buffers;
    for (const auto &root : m_roots) {
        if (root->kind == Root::Buffer && root->used)
            buffers.push_back(root.get());
    }
    std::sort(buffers.begin(), buffers.end(), [](const Root *a, const Root *b) { return a->index < b->index; });
    int binding = m_options.firstBuffer;
    int location = m_options.firstLocation + 2;
    for (Root *root : buffers) {
        const std::string number = std::to_string(root->index);
        const std::string block = "reef_" + m_prefix + "buffer" + number;
        declarations.push_back("layout(std430, binding = " + std::to_string(binding) + ") " + (root->readOnly ? "readonly " : "") + "buffer " + block
                               + " { uint w[]; } reef_" + m_prefix + "b" + number + ";");
        reef_mtl_binding entry = {};
        entry.kind = 1;
        entry.index = root->index;
        entry.count = 1;
        entry.binding = binding++;
        entry.access = root->readOnly ? 1 : 3;
        entry.type = root->argument->addressSpace == 2 ? 2 : 1;
        entry.sampler = -1;
        entry.constantSampler = -1;
        entry.size = root->argument->size;
        entry.alignment = root->argument->alignment;
        if (root->addressTaken) {
            entry.location = location;
            declarations.push_back("layout(location = " + std::to_string(location++) + ") uniform uvec2 reef_" + m_prefix + "ba" + number + ";");
        } else {
            entry.location = -1;
        }
        entry.name = keep(m_shader, root->argument->name);
        entry.variable = keep(m_shader, block);
        if (root->argument && root->argument->argumentBuffer >= 0) {
            entry.indirect = 1;
            entry.argumentBuffer = root->argument->argumentBuffer;
            entry.argumentMember = root->argument->argumentMember;
        }
        m_shader.bindings.push_back(entry);
    }

    // Memory that is not a buffer.
    for (const auto &root : m_roots) {
        if (!root->used)
            continue;
        const std::string size = std::to_string(root->size);
        if (root->kind == Root::Threadgroup) {
            declarations.push_back("shared uint " + root->name + "[" + size + "];");
            if (root->argument) {
                reef_mtl_binding entry = {};
                entry.kind = 4;
                entry.index = root->index;
                entry.count = 1;
                entry.binding = -1;
                entry.access = 3;
                entry.type = 3;
                entry.sampler = -1;
                entry.constantSampler = -1;
                entry.size = root->size * 4;
                entry.alignment = 16;
                entry.location = -1;
                entry.name = keep(m_shader, root->argument->name);
                entry.variable = keep(m_shader, root->name);
                m_shader.bindings.push_back(entry);
            }
        } else if (root->kind == Root::Constant) {
            declarations.push_back("const uint " + root->name + "[" + size + "] = uint[](" + constantWords(root->global->getInitializer(), root->size * 4) + ");");
        } else if (root->kind == Root::Private) {
            declarations.push_back("uint " + root->name + "[" + size + "];");
            llvm::GlobalVariable *global = root->global;
            if (global && !global->isDeclaration() && !global->getInitializer()->isNullValue() && !llvm::isa<llvm::UndefValue>(global->getInitializer())) {
                declarations.push_back("const uint " + root->name + "_initial[" + size + "] = uint[](" + constantWords(global->getInitializer(), root->size * 4) + ");");
                m_prologue.push_back(root->name + " = " + root->name + "_initial;");
            } else if (root->global && !root->global->isDeclaration()) {
                m_prologue.push_back("for (int i = 0; i < " + size + "; ++i) " + root->name + "[i] = 0u;");
            }
        }
    }

    // Textures: a GLSL sampler for each Metal sampler used with them, and an image when they are written.
    int unit = m_options.firstTexture;
    int image = m_options.firstImage;
    for (const auto &[index, texture] : m_textures) {
        const std::string prefix = texture.component == Component::Int ? "i" : texture.component == Component::Uint ? "u" : "";
        std::string glsl;
        int type = 0;
        for (const auto &kind : textureKinds) {
            if (texture.kind == kind.air) {
                glsl = kind.glsl;
                type = kind.type;
            }
        }
        const unsigned access = texture.argument->access & 7;
        std::vector<std::pair<std::pair<const Root *, bool>, std::string>> samplers(texture.samplers.begin(), texture.samplers.end());
        std::sort(samplers.begin(), samplers.end(), [](const auto &a, const auto &b) { return a.second < b.second; });
        // Reads by pixel need no sampler of their own when one that filters exists.
        std::string plain;
        for (const auto &sampler : samplers) {
            if (sampler.first.first && !sampler.first.second) {
                plain = sampler.second;
                break;
            }
        }
        for (const auto &[key, name] : samplers) {
            const Root *sampler = key.first;
            if (!sampler && !plain.empty()) {
                declarations.push_back("#define " + name + " " + plain);
                continue;
            }
            const std::string samplerType = prefix + "sampler" + glsl + (key.second ? "Shadow" : "");
            declarations.push_back("layout(binding = " + std::to_string(unit) + ") uniform " + samplerType + " " + name + ";");
            reef_mtl_binding entry = {};
            entry.kind = 2;
            entry.index = index;
            entry.count = 1;
            entry.binding = unit++;
            entry.access = access;
            entry.type = type;
            entry.dataType = texture.dataType;
            entry.sampler = -1;
            entry.constantSampler = -1;
            entry.location = -1;
            if (sampler) {
                entry.constantSampler = sampler->sampler;
                if (sampler->sampler < 0) {
                    // A sampler argument: whether its coordinates are pixels is known when drawing.
                    entry.sampler = sampler->argument->location;
                    entry.location = location;
                    declarations.push_back("layout(location = " + std::to_string(location++) + ") uniform uint " + name + "_pixels;");
                }
            }
            entry.name = keep(m_shader, texture.argument->name);
            entry.variable = keep(m_shader, name);
            if (texture.argument && texture.argument->argumentBuffer >= 0) {
                entry.indirect |= 1;
                entry.argumentBuffer = texture.argument->argumentBuffer;
                entry.argumentMember = texture.argument->argumentMember;
            }
            if (sampler && sampler->sampler < 0 && sampler->argument && sampler->argument->argumentBuffer >= 0) {
                entry.indirect |= 2;
                entry.samplerArgumentBuffer = sampler->argument->argumentBuffer;
                entry.samplerArgumentMember = sampler->argument->argumentMember;
            }
            m_shader.bindings.push_back(entry);
        }
        if (!texture.image.empty()) {
            const bool readable = texture.argument->access & 1;
            if (readable)
                require("GL_EXT_shader_image_load_formatted");
            const std::string imageType = prefix + "image" + glsl;
            declarations.push_back("layout(binding = " + std::to_string(image) + ") uniform " + (readable ? "" : "writeonly ") + imageType + " " + texture.image + ";");
            reef_mtl_binding entry = {};
            entry.kind = 3;
            entry.index = index;
            entry.count = 1;
            entry.binding = image++;
            entry.access = access;
            entry.type = type;
            entry.dataType = texture.dataType;
            entry.sampler = -1;
            entry.constantSampler = -1;
            entry.location = -1;
            entry.name = keep(m_shader, texture.argument->name);
            entry.variable = keep(m_shader, texture.image);
            if (texture.argument && texture.argument->argumentBuffer >= 0) {
                entry.indirect = 1;
                entry.argumentBuffer = texture.argument->argumentBuffer;
                entry.argumentMember = texture.argument->argumentMember;
            }
            m_shader.bindings.push_back(entry);
        }
    }

    // Colour attachments that the fragment function reads.
    for (const auto &[attachment, component] : m_framebuffers) {
        const std::string name = "reef_fb" + std::to_string(attachment);
        declarations.push_back("layout(binding = " + std::to_string(unit) + ") uniform " + (component == 1 ? "i" : component == 2 ? "u" : "") + "sampler2D " + name + ";");
        reef_mtl_binding entry = {};
        entry.kind = 5;
        entry.index = attachment;
        entry.count = 1;
        entry.binding = unit++;
        entry.access = 1;
        entry.type = 2;
        entry.sampler = -1;
        entry.constantSampler = -1;
        entry.location = -1;
        entry.name = keep(m_shader, name);
        entry.variable = keep(m_shader, name);
        m_shader.bindings.push_back(entry);
    }

    std::string header;
    if (stage == 3) {
        m_shader.threadgroupSize[0] = m_options.threadgroupSize[0];
        m_shader.threadgroupSize[1] = m_options.threadgroupSize[1];
        m_shader.threadgroupSize[2] = m_options.threadgroupSize[2];
        header = "layout(local_size_x = " + std::to_string(m_options.threadgroupSize[0]) + ", local_size_y = " + std::to_string(m_options.threadgroupSize[1])
            + ", local_size_z = " + std::to_string(m_options.threadgroupSize[2]) + ") in;\n";
        // The grid of a dispatch need not be whole threadgroups. A barrier must be reached by all of them.
        if (!m_barrier) {
            m_shader.flags |= 0x4000;
            m_shader.uniformLocations[0] = m_options.firstLocation;
            m_prologue.insert(m_prologue.begin(), "if (any(greaterThanEqual(gl_GlobalInvocationID, reef_threads_per_grid))) return;");
        }
    }
    if (m_shader.uniformLocations[0] >= 0)
        declarations.push_back("layout(location = " + std::to_string(m_shader.uniformLocations[0]) + ") uniform uvec3 reef_threads_per_grid;");
    if (stage == 2 && m_info.earlyFragmentTests) {
        m_shader.flags |= 0x10000;
        header += "layout(early_fragment_tests) in;\n";
    }

    std::ostringstream out;
    out << "#version 450 core\n";
    for (const std::string &extension : m_extensions)
        out << "#extension " << extension << " : require\n";
    out << header;
    for (const std::string &text : m_structs)
        out << text << '\n';
    for (const std::string &text : m_interface)
        out << text << '\n';
    for (const std::string &text : declarations)
        out << text << '\n';
    for (const std::string &name : m_helperOrder)
        out << m_helpers[name];
    out << "void main()\n{\n";
    for (const auto &[type, name] : m_variables)
        out << "    " << type << " " << name << ";\n";
    for (const std::string &text : m_prologue)
        out << "    " << text << '\n';
    out << body << "}\n";
    m_shader.glsl = out.str();
    m_shader.attributes = attributesOf(m_info, m_shader.strings);
}

}
