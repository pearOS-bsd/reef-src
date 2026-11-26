// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Pointers to resources: arrays of textures and samplers, argument buffers, and the passes that make every such pointer known.

#include "emitter.h"

#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/GetElementPtrTypeIterator.h>
#include <llvm/IR/IRBuilder.h>

using namespace llvm;

namespace cutefish::reef::metal {

namespace {

// How a load reaches its resource: GEPs and bitcasts from an alloca or an argument, one of them at an index known only at run time.
struct Path
{
    Value *base = nullptr;              // 0x00
    std::vector<Instruction *> steps;   // 0x08  from the base to the load
    Instruction *indexed = nullptr;     // 0x20  the GEP with the run-time index
    unsigned operand = 0;               // 0x28  its operand that is the index
    int64_t stride = 0;                 // 0x30  bytes from one element to the next
    int64_t offset = 0;                 // 0x38  bytes of the constant indices
};

}

// The type name of an element of a Metal array: "texture2d<float>" of "array<texture2d<float>, 4>".
static std::string elementTypeName(const std::string &name)
{
    if (name.compare(0, 6, "array<") == 0) {
        const size_t comma = name.rfind(',');
        if (comma != std::string::npos && comma >= 6)
            return name.substr(6, comma - 6);
    }
    return name;
}

// The steps of the path again, with the run-time index replaced by a constant, and the load at their end.
static Instruction *rebuild(const Path &path, LoadInst *load, ConstantInt *index, Instruction *before)
{
    Value *pointer = path.base;
    for (Instruction *step : path.steps) {
        Instruction *copy = step->clone();
        copy->setOperand(0, pointer);
        if (step == path.indexed)
            copy->setOperand(path.operand, ConstantInt::get(step->getOperand(path.operand)->getType(), index->getSExtValue()));
        copy->insertBefore(before);
        pointer = copy;
    }
    Instruction *copy = load->clone();
    copy->setOperand(0, pointer);
    copy->insertBefore(before);
    return copy;
}

// The bytes the constant indices of a GEP add, without the index at this operand.
static int64_t constantOffset(const DataLayout &layout, GEPOperator *gep, unsigned skip)
{
    int64_t offset = 0;
    unsigned operand = 1;
    for (auto type = gep_type_begin(gep), end = gep_type_end(gep); type != end; ++type, ++operand) {
        if (operand == skip)
            continue;
        auto *index = llvm::cast<ConstantInt>(type.getOperand());
        if (StructType *structure = type.getStructTypeOrNull())
            offset += layout.getStructLayout(structure)->getElementOffset(unsigned(index->getZExtValue()));
        else
            offset += index->getSExtValue() * int64_t(type.getSequentialElementStride(layout));
    }
    return offset;
}

// The metadata of a parameter of the function.
const Argument *Emitter::argumentOf(Value *value)
{
    auto *argument = dyn_cast<llvm::Argument>(value);
    if (!argument || argument->getParent() != m_function)
        return nullptr;
    for (const Argument &info : m_info.arguments) {
        if (info.number == int(argument->getArgNo()))
            return &info;
    }
    return nullptr;
}

// GLSL cannot index arrays of samplers or images at run time: a load of a texture or sampler at a run-time index becomes one
// load and one call for each element, chosen by a switch (or, for samples in fragment shaders, which need their derivatives,
// by selects).
bool Emitter::splitResourceIndexing()
{
    bool changed = false;
    const int stage = m_info.stage;
    Path path;
    auto splitOne = [&]() {
        for (BasicBlock &block : *m_function) {
            for (Instruction &instruction : block) {
                auto *load = dyn_cast<LoadInst>(&instruction);
                if (!load || !load->getType()->isPointerTy() || load->getType()->getPointerAddressSpace() - 1 > 1)
                    continue;
                if (!all_of(load->users(), [](User *user) { return isa<CallBase>(user); }))
                    continue;
                const DataLayout &layout = m_layout;
                // The chain from an alloca or an argument.
                Value *base = load->getPointerOperand();
                std::vector<Instruction *> chain;
                bool known = true;
                while (auto *step = dyn_cast<Instruction>(base)) {
                    if (isa<AllocaInst>(step))
                        break;
                    if (!isa<GetElementPtrInst>(step) && !isa<BitCastInst>(step)) {
                        known = false;
                        break;
                    }
                    chain.push_back(step);
                    base = step->getOperand(0);
                }
                if (!known || (!isa<Instruction>(base) && !isa<llvm::Argument>(base)) || chain.empty())
                    continue;
                path.steps.assign(chain.rbegin(), chain.rend());
                path.base = base;
                path.indexed = nullptr;
                path.operand = 0;
                path.stride = 0;
                path.offset = 0;
                // Exactly one index known only at run time, into an array or a vector.
                auto findIndex = [&]() {
                    for (Instruction *step : path.steps) {
                        auto *gep = dyn_cast<GetElementPtrInst>(step);
                        if (!gep)
                            continue;
                        unsigned operand = 1;
                        unsigned variable = 0;
                        for (auto type = gep_type_begin(gep), end = gep_type_end(gep); type != end; ++type, ++operand) {
                            if (isa<ConstantInt>(type.getOperand()))
                                continue;
                            if (variable || path.indexed || type.isStruct())
                                return false;
                            variable = operand;
                            path.stride = int64_t(type.getSequentialElementStride(layout));
                        }
                        if (variable) {
                            path.indexed = gep;
                            path.operand = variable;
                        }
                        path.offset += constantOffset(layout, llvm::cast<GEPOperator>(gep), variable);
                    }
                    return true;
                };
                if (!findIndex())
                    continue;
                if (!path.indexed || path.stride <= 0)
                    continue;
                if (!isa<AllocaInst>(base)) {
                    const Argument *info = argumentOf(base);
                    if (!info || !info->indirect)
                        continue;
                }
                // The elements: every pointer-sized place of the array, or of the argument buffer where a texture or
                // sampler is.
                const Argument *buffer = nullptr;
                int64_t size;
                if (auto *local = dyn_cast<AllocaInst>(path.base)) {
                    size = int64_t(layout.getTypeAllocSize(local->getAllocatedType()));
                } else {
                    buffer = argumentOf(path.base);
                    size = buffer->size;
                }
                Value *index = path.indexed->getOperand(path.operand);
                std::vector<ConstantInt *> cases;
                uint64_t element = 0;
                for (int64_t at = path.offset; at + 7 < size; at += path.stride, ++element) {
                    if (at < 0 || (at & 7) != 0)
                        continue;
                    if (buffer) {
                        const bool resource = any_of(buffer->members, [&](const Member &member) {
                            return (member.role == Role::Texture || member.role == Role::Sampler) && member.offset <= at
                                   && member.offset + member.count * 8 > at;
                        });
                        if (!resource)
                            continue;
                    }
                    cases.push_back(llvm::cast<ConstantInt>(ConstantInt::get(index->getType(), element)));
                }
                if (cases.empty())
                    throw Unsupported("a texture or sampler array indexed outside itself");
                std::vector<User *> users;
                for (User *user : load->users())
                    users.push_back(user);
                for (User *user : users) {
                    auto *call = llvm::cast<CallBase>(user);
                    IRBuilder<> builder(call);
                    // The switch wants an i32: the index as it was before the GEP widened it.
                    Value *selector;
                    if ((isa<ZExtInst>(index) || isa<SExtInst>(index)) && llvm::cast<CastInst>(index)->getOperand(0)->getType()->isIntegerTy(32))
                        selector = llvm::cast<CastInst>(index)->getOperand(0);
                    else if (index->getType()->isIntegerTy(32))
                        selector = index;
                    else
                        selector = builder.CreateTrunc(index, builder.getInt32Ty());
                    Function *callee = call->getCalledFunction();
                    if (stage == 2 && callee
                        && (callee->getName().starts_with("air.sample_") || callee->getName().starts_with("air.calculate_"))) {
                        // Every element is sampled where the call was: derivatives stay in uniform control flow.
                        Value *result = nullptr;
                        for (ConstantInt *value : cases) {
                            Instruction *loaded = rebuild(path, load, value, call);
                            Instruction *copy = call->clone();
                            copy->replaceUsesOfWith(load, loaded);
                            copy->insertBefore(call);
                            if (!result) {
                                result = copy;
                                continue;
                            }
                            Value *condition = builder.CreateICmpEQ(selector, ConstantInt::get(selector->getType(), value->getZExtValue()));
                            result = builder.CreateSelect(condition, copy, result);
                        }
                        call->replaceAllUsesWith(result);
                        call->eraseFromParent();
                        continue;
                    }
                    BasicBlock *head = call->getParent();
                    BasicBlock *tail = head->splitBasicBlock(call->getIterator());
                    head->getTerminator()->eraseFromParent();
                    std::vector<std::pair<BasicBlock *, Instruction *>> results;
                    for (ConstantInt *value : cases) {
                        BasicBlock *body = BasicBlock::Create(m_module.getContext(), "", m_function, tail);
                        BranchInst *jump = BranchInst::Create(tail, body);
                        Instruction *loaded = rebuild(path, load, value, jump);
                        Instruction *copy = call->clone();
                        copy->replaceUsesOfWith(load, loaded);
                        copy->insertBefore(jump);
                        results.push_back({body, copy});
                    }
                    SwitchInst *choice = SwitchInst::Create(selector, results[0].first, unsigned(results.size()), head);
                    for (size_t i = 1; i < results.size(); ++i)
                        choice->addCase(llvm::cast<ConstantInt>(ConstantInt::get(selector->getType(), cases[i]->getZExtValue())), results[i].first);
                    if (!call->getType()->isVoidTy()) {
                        PHINode *phi = PHINode::Create(call->getType(), unsigned(results.size()), "", &tail->front());
                        for (const auto &[body, copy] : results)
                            phi->addIncoming(copy, body);
                        call->replaceAllUsesWith(phi);
                    }
                    call->eraseFromParent();
                }
                load->eraseFromParent();
                return true;
            }
        }
        return false;
    };
    while (splitOne())
        changed = true;
    return changed;
}

// The textures and samplers that expandResourceArrays() made globals of get roots; then every load of a resource from an
// argument buffer gets one.
void Emitter::collectArgumentBuffers()
{
    for (const auto &[value, member] : m_memberValues) {
        if (value->use_empty())
            continue;
        auto root = std::make_unique<Root>();
        root->kind = member->role == Role::Texture ? Root::Texture : Root::Sampler;
        root->argument = member;
        root->index = member->location;
        m_pointerRoots[value] = root.get();
        if (member->role == Role::Texture)
            addTexture(*member);
        m_roots.push_back(std::move(root));
    }
    for (BasicBlock &block : *m_function) {
        for (Instruction &instruction : block) {
            auto *load = dyn_cast<LoadInst>(&instruction);
            if (!load || !load->getType()->isPointerTy())
                continue;
            const Argument *info = argumentOf(getUnderlyingObject(load->getPointerOperand(), 6));
            if (info && info->indirect)
                rootOf(load);
        }
    }
}

// An array of textures or samplers as a parameter: GLSL has no handle to keep in memory, so each element becomes a global of
// its own with an argument of its own, and the parameter an array of them in private memory.
void Emitter::expandResourceArrays()
{
    IRBuilder<> builder(&*m_function->getEntryBlock().getFirstInsertionPt());
    Type *byte = builder.getInt8Ty();
    for (const Argument &argument : m_info.arguments) {
        if ((argument.role != Role::Texture && argument.role != Role::Sampler) || argument.count <= 1)
            continue;
        llvm::Argument *parameter = m_function->getArg(unsigned(argument.number));
        if (parameter->use_empty() || !parameter->getType()->isPointerTy())
            continue;
        const unsigned space = argument.role == Role::Texture ? 1 : 2;
        Type *pointer = PointerType::get(m_module.getContext(), space);
        AllocaInst *array = builder.CreateAlloca(ArrayType::get(pointer, uint64_t(argument.count)));
        for (int i = 0; i < argument.count; ++i) {
            auto *global = new GlobalVariable(m_module, byte, false, GlobalValue::ExternalLinkage, nullptr,
                                              "reef.resource." + std::to_string(argument.number) + "." + std::to_string(i), nullptr,
                                              GlobalValue::NotThreadLocal, space);
            Argument element = argument;
            element.location = argument.location + i;
            element.count = 1;
            element.typeName = elementTypeName(argument.typeName);
            element.name = argument.name + "[" + std::to_string(i) + "]";
            m_members.push_back(element);
            m_memberValues.push_back({global, &m_members.back()});
            builder.CreateStore(global, builder.CreateConstInBoundsGEP1_64(byte, array, uint64_t(i) * 8));
        }
        parameter->replaceAllUsesWith(array);
    }
}

// A texture, sampler or buffer that an argument buffer holds, by the constant offset of the load. Each becomes an argument
// and a root of its own; a second load of the same place finds the same root.
Root *Emitter::loadedRoot(LoadInst *load)
{
    APInt offset(64, 0);
    Value *base = load->getPointerOperand()->stripAndAccumulateConstantOffsets(m_layout, offset, true);
    const Argument *info = argumentOf(base);
    if (!info || info->role != Role::Buffer || !info->indirect) {
        if (isa<llvm::Argument>(getUnderlyingObject(load->getPointerOperand(), 6)))
            throw Unsupported("a resource an argument buffer holds at an offset known only at run time");
        throw Unsupported("a pointer loaded from memory");
    }
    Root &buffer = rootOf(base);
    const int64_t at = offset.getSExtValue();
    const auto known = m_loadedRoots.find({&buffer, at});
    if (known != m_loadedRoots.end())
        return known->second;
    const Member *member = nullptr;
    for (const Member &candidate : info->members) {
        if (candidate.offset > at)
            continue;
        const int count = candidate.count > 0 ? candidate.count : 1;
        if (candidate.offset + count * 8 > at && ((at - candidate.offset) & 7) == 0)
            member = &candidate;
    }
    if (!member)
        throw Unsupported("argument buffer " + info->name + " holds no texture, sampler or buffer at byte " + std::to_string(at));
    // Pipeline states are handles the shader only passes on.
    const bool handle = member->role == Role::RenderPipelineState || member->role == Role::ComputePipelineState;
    if (!handle && member->role != Role::Buffer && member->role != Role::Texture && member->role != Role::Sampler
        && member->role != Role::CommandBuffer)
        throw Unsupported("argument buffer " + info->name + " holds no texture, sampler or buffer at byte " + std::to_string(at));
    if (member->indirect)
        throw Unsupported("argument buffers that point at other argument buffers are not supported");
    Argument element;
    element.role = member->role;
    element.location = int(m_loadedRoots.size()) + 1000;
    element.count = 1;
    element.access = member->access;
    element.addressSpace = member->addressSpace;
    element.size = member->typeSize;
    element.alignment = member->alignment;
    element.typeName = elementTypeName(member->typeName);
    element.name = info->name + "." + member->name + (member->count > 1 ? "[" + std::to_string((at - member->offset) / 8) + "]" : std::string());
    element.argumentBuffer = info->location;
    element.argumentMember = int(at);
    m_members.push_back(element);
    const Argument &stored = m_members.back();
    auto root = std::make_unique<Root>();
    root->argument = &stored;
    root->index = stored.location;
    if (member->role == Role::Texture) {
        root->kind = Root::Texture;
        addTexture(stored);
    } else if (member->role == Role::Sampler) {
        root->kind = Root::Sampler;
    } else if (handle) {
        root->kind = Root::ReadSampler;
    } else {
        root->kind = Root::Buffer;
        root->name = "reef_" + m_prefix + "b" + std::to_string(root->index) + ".w";
        root->readOnly = member->role != Role::CommandBuffer && (stored.addressSpace == 2 || (stored.access & 2) == 0);
    }
    Root *result = root.get();
    m_roots.push_back(std::move(root));
    m_loadedRoots[{&buffer, at}] = result;
    return result;
}

}
