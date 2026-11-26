// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What turns clang's module into AIR: inlining into the entry points, AIR's names for intrinsics, and AIR's metadata.

#include "frontend.h"

#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/IPO/GlobalDCE.h>
#include <llvm/Transforms/IPO/GlobalOpt.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/EarlyCSE.h>
#include <llvm/Transforms/Scalar/InferAddressSpaces.h>
#include <llvm/Transforms/Scalar/SROA.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils/LowerMemIntrinsics.h>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace cutefish::reef::metal::msl {
namespace {

// The named metadata of each kind of entry point.
const char* const entryKinds[] = {"", "air.vertex", "air.fragment", "air.kernel", "", "air.visible"};

struct Operation {
    llvm::Intrinsic::ID id;
    const char* name;
};

// Intrinsics on integers that AIR has a function for: air.<name>.<type>.
const Operation integerOperations[] = {
    {llvm::Intrinsic::smax, "max.s"},
    {llvm::Intrinsic::smin, "min.s"},
    {llvm::Intrinsic::umax, "max.u"},
    {llvm::Intrinsic::umin, "min.u"},
    {llvm::Intrinsic::ctpop, "popcount.u"},
    {llvm::Intrinsic::bitreverse, "reverse_bits.u"},
    {llvm::Intrinsic::sadd_sat, "add_sat.s"},
    {llvm::Intrinsic::uadd_sat, "add_sat.u"},
    {llvm::Intrinsic::ssub_sat, "sub_sat.s"},
    {llvm::Intrinsic::usub_sat, "sub_sat.u"},
};

// Intrinsics on floating point values that AIR has a function for: air.<name>.<type>, air.fast_<name>.<type> in fast math.
const Operation floatOperations[] = {
    {llvm::Intrinsic::fabs, "fabs"},
    {llvm::Intrinsic::floor, "floor"},
    {llvm::Intrinsic::ceil, "ceil"},
    {llvm::Intrinsic::trunc, "trunc"},
    {llvm::Intrinsic::rint, "rint"},
    {llvm::Intrinsic::nearbyint, "rint"},
    {llvm::Intrinsic::round, "round"},
    {llvm::Intrinsic::sqrt, "sqrt"},
    {llvm::Intrinsic::sin, "sin"},
    {llvm::Intrinsic::cos, "cos"},
    {llvm::Intrinsic::exp, "exp"},
    {llvm::Intrinsic::exp2, "exp2"},
    {llvm::Intrinsic::log, "log"},
    {llvm::Intrinsic::log2, "log2"},
    {llvm::Intrinsic::log10, "log10"},
    {llvm::Intrinsic::pow, "pow"},
    {llvm::Intrinsic::fma, "fma"},
    {llvm::Intrinsic::copysign, "copysign"},
    {llvm::Intrinsic::minnum, "fmin"},
    {llvm::Intrinsic::maxnum, "fmax"},
    {llvm::Intrinsic::minimum, "fmin"},
    {llvm::Intrinsic::maximum, "fmax"},
};

// Functions of the standard library with an effect: what is left is pure.
const char* const effects[] = {
    "air.write_", "air.wg.barrier", "air.simdgroup.barrier", "air.discard_fragment", "air.atomic.",
    "reef.msl.sample", "reef.msl.gather", "reef.msl.read", "air.get_", "reef.msl.function_constant_defined",
};

// Derivatives, barriers and what samples with implicit derivatives depend on the other threads.
bool isConvergent(llvm::StringRef name)
{
    return name.starts_with("air.dfdx") || name.starts_with("air.dfdy") || name.starts_with("air.fwidth")
        || name.starts_with("air.wg.barrier") || name.starts_with("air.simdgroup.barrier") || name.starts_with("reef.msl.sample")
        || name.starts_with("reef.msl.gather");
}

// Address space casts of a global back to its own type, the evaluator of GlobalOpt follows none.
bool foldCasts(llvm::Value* value)
{
    bool changed = false;
    const std::vector<llvm::User*> users(value->user_begin(), value->user_end());
    for (llvm::User* user : users) {
        auto* expression = llvm::dyn_cast<llvm::ConstantExpr>(user);
        if (!expression)
            continue;
        if (expression->getOpcode() == llvm::Instruction::AddrSpaceCast && expression->getType() == value->getType()) {
            foldCasts(expression);
            expression->replaceAllUsesWith(value);
            changed = true;
        } else {
            changed |= foldCasts(expression);
        }
    }
    return changed;
}

// Leaves the constructors of program-scope variables in a shape GlobalOpt evaluates.
struct TidyForEvaluator : llvm::PassInfoMixin<TidyForEvaluator> {
    llvm::PreservedAnalyses run(llvm::Module& module, llvm::ModuleAnalysisManager&)
    {
        bool changed = false;
        for (llvm::GlobalVariable& global : module.globals()) {
            if (!global.use_empty())
                changed |= foldCasts(&global);
        }
        for (llvm::Function& function : module) {
            for (llvm::BasicBlock& block : function) {
                for (llvm::Instruction& instruction : llvm::make_early_inc_range(block)) {
                    if (auto* call = llvm::dyn_cast<llvm::CallInst>(&instruction)) {
                        llvm::Function* callee = call->getCalledFunction();
                        if (!callee || !callee->isIntrinsic())
                            continue;
                        const llvm::Intrinsic::ID id = callee->getIntrinsicID();
                        if (id != llvm::Intrinsic::experimental_noalias_scope_decl
                            && ((id != llvm::Intrinsic::invariant_end && id != llvm::Intrinsic::invariant_start) || !call->use_empty()))
                            continue;
                        call->eraseFromParent();
                        changed = true;
                    } else if (auto* cast = llvm::dyn_cast<llvm::AddrSpaceCastInst>(&instruction)) {
                        if (cast->getPointerOperand()->getType()->getPointerAddressSpace() != cast->getType()->getPointerAddressSpace())
                            continue;
                        cast->replaceAllUsesWith(cast->getPointerOperand());
                        cast->eraseFromParent();
                        changed = true;
                    }
                }
            }
        }
        return changed ? llvm::PreservedAnalyses::none() : llvm::PreservedAnalyses::all();
    }
};

// Stores of one element of a vector in a global, as a load of the vector, an insertelement and a store of the vector: what
// the evaluator of GlobalOpt can follow.
void storeWholeVectors(llvm::Module& module)
{
    llvm::GlobalVariable* constructors = module.getGlobalVariable("llvm.global_ctors", true);
    if (!constructors || constructors->isDeclaration())
        return;
    auto* list = llvm::dyn_cast<llvm::ConstantArray>(constructors->getInitializer());
    if (!list)
        return;
    const llvm::DataLayout& layout = module.getDataLayout();
    for (llvm::Use& entry : list->operands()) {
        auto* function = llvm::dyn_cast<llvm::Function>(llvm::cast<llvm::ConstantStruct>(entry.get())->getOperand(1)->stripPointerCasts());
        if (!function || function->isDeclaration())
            continue;
        for (llvm::Instruction& instruction : llvm::make_early_inc_range(llvm::instructions(*function))) {
            auto* store = llvm::dyn_cast<llvm::StoreInst>(&instruction);
            if (!store || store->getValueOperand()->getType()->isVectorTy())
                continue;
            llvm::APInt offset(64, 0);
            llvm::Value* base = store->getPointerOperand()->stripAndAccumulateConstantOffsets(layout, offset, true);
            auto* global = llvm::dyn_cast<llvm::GlobalVariable>(base);
            if (!global)
                continue;
            uint64_t remaining = offset.getZExtValue();
            uint64_t start = 0;
            llvm::Type* type = global->getValueType();
            bool found = true;
            while (type->getTypeID() != llvm::Type::FixedVectorTyID) {
                uint64_t skipped;
                if (auto* structure = llvm::dyn_cast<llvm::StructType>(type)) {
                    const llvm::StructLayout* members = layout.getStructLayout(structure);
                    if (uint64_t(members->getSizeInBytes()) <= remaining) {
                        found = false;
                        break;
                    }
                    const unsigned index = members->getElementContainingOffset(remaining);
                    skipped = members->getElementOffset(index);
                    remaining -= skipped;
                    type = structure->getElementType(index);
                } else if (auto* array = llvm::dyn_cast<llvm::ArrayType>(type)) {
                    const uint64_t size = layout.getTypeAllocSize(array->getElementType());
                    if (!size || remaining / size >= array->getNumElements()) {
                        found = false;
                        break;
                    }
                    skipped = size * (remaining / size);
                    remaining %= size;
                    type = array->getElementType();
                } else {
                    found = false;
                    break;
                }
                start += skipped;
            }
            if (!found || llvm::cast<llvm::VectorType>(type)->getElementType() != store->getValueOperand()->getType())
                continue;
            const uint64_t element = layout.getTypeAllocSize(store->getValueOperand()->getType());
            llvm::IRBuilder<> builder(store);
            llvm::Value* address = builder.CreateInBoundsGEP(builder.getInt8Ty(), global, builder.getInt64(start));
            llvm::Value* vector = builder.CreateLoad(type, address);
            vector = builder.CreateInsertElement(vector, store->getValueOperand(), builder.getInt64((offset.getZExtValue() - start) / element));
            builder.CreateStore(vector, address);
            store->eraseFromParent();
        }
    }
}

void runPasses(llvm::Module& module, bool first)
{
    // Vectorizing is left to the GPU's own compiler.
    llvm::PipelineTuningOptions tuning;
    tuning.LoopInterleaving = false;
    tuning.LoopVectorization = false;
    tuning.SLPVectorization = false;
    llvm::PassBuilder builder(nullptr, tuning);
    llvm::LoopAnalysisManager loops;
    llvm::FunctionAnalysisManager functions;
    llvm::CGSCCAnalysisManager sccs;
    llvm::ModuleAnalysisManager modules;
    builder.registerModuleAnalyses(modules);
    builder.registerCGSCCAnalyses(sccs);
    builder.registerFunctionAnalyses(functions);
    builder.registerLoopAnalyses(loops);
    builder.crossRegisterProxies(loops, functions, sccs, modules);

    llvm::ModulePassManager passes;
    passes.addPass(llvm::AlwaysInlinerPass());
    llvm::FunctionPassManager tidy;
    tidy.addPass(llvm::InferAddressSpacesPass(4));
    tidy.addPass(llvm::SROAPass(llvm::SROAOptions::ModifyCFG));
    tidy.addPass(llvm::EarlyCSEPass());
    tidy.addPass(llvm::InstCombinePass());
    tidy.addPass(llvm::InferAddressSpacesPass(4));
    passes.addPass(llvm::createModuleToFunctionPassAdaptor(std::move(tidy)));
    passes.addPass(TidyForEvaluator());
    if (first) {
        passes.addPass(builder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2));
        passes.addPass(TidyForEvaluator());
    }
    llvm::FunctionPassManager last;
    last.addPass(llvm::InferAddressSpacesPass(4));
    last.addPass(llvm::SROAPass(llvm::SROAOptions::ModifyCFG));
    last.addPass(llvm::InstCombinePass());
    last.addPass(llvm::SimplifyCFGPass());
    passes.addPass(llvm::createModuleToFunctionPassAdaptor(std::move(last)));
    passes.addPass(llvm::GlobalDCEPass());
    passes.run(module, modules);
    if (!first)
        return;

    storeWholeVectors(module);
    llvm::ModulePassManager evaluation;
    evaluation.addPass(llvm::GlobalOptPass());
    evaluation.addPass(llvm::createModuleToFunctionPassAdaptor(llvm::InstCombinePass()));
    evaluation.addPass(llvm::GlobalDCEPass());
    evaluation.run(module, modules);
}

// AIR's suffix of a type in the name of an overloaded function: f32, v4f32, i32, v2i16 ...
std::string typeSuffix(llvm::Type* type)
{
    std::string prefix;
    if (auto* vector = llvm::dyn_cast<llvm::FixedVectorType>(type)) {
        prefix = "v" + std::to_string(vector->getNumElements());
        type = vector->getElementType();
    }
    if (type->isFloatTy())
        return prefix + "f32";
    if (type->isHalfTy())
        return prefix + "f16";
    if (type->isDoubleTy())
        return prefix + "f64";
    if (type->isIntegerTy())
        return prefix + "i" + std::to_string(type->getIntegerBitWidth());
    return prefix + "x";
}

// A call of a pure function of AIR, declared when it is first used.
llvm::CallInst* callAir(llvm::IRBuilder<>& builder, llvm::Module& module, const std::string& name, llvm::Type* result,
                        llvm::ArrayRef<llvm::Value*> arguments)
{
    std::vector<llvm::Type*> types;
    for (llvm::Value* argument : arguments)
        types.push_back(argument->getType());
    auto* function = llvm::cast<llvm::Function>(module.getOrInsertFunction(name, llvm::FunctionType::get(result, types, false)).getCallee());
    function->addFnAttr(llvm::Attribute::NoUnwind);
    function->addFnAttr(llvm::Attribute::WillReturn);
    function->setDoesNotAccessMemory();
    return builder.CreateCall(function, arguments);
}

// Program-scope variables that need code to get their value: the code runs at the start of every entry point, and what it
// stores replaces what the entry point loads.
bool runConstructors(llvm::Module& module, const std::vector<EntryPoint>& entries, std::string& error)
{
    llvm::GlobalVariable* list = module.getGlobalVariable("llvm.global_ctors", true);
    if (!list)
        return true;
    auto* array = llvm::dyn_cast<llvm::ConstantArray>(list->getInitializer());
    if (!array) {
        list->eraseFromParent();
        return true;
    }
    std::vector<llvm::Function*> constructors;
    for (llvm::Use& entry : array->operands()) {
        if (!llvm::isa<llvm::ConstantStruct>(entry.get()))
            continue;
        if (auto* function = llvm::dyn_cast<llvm::Function>(llvm::cast<llvm::Constant>(entry.get())->getOperand(1)->stripPointerCasts()))
            constructors.push_back(function);
    }
    list->eraseFromParent();
    if (constructors.empty())
        return true;

    std::set<llvm::GlobalVariable*> initialized;
    for (llvm::Function* constructor : constructors) {
        for (llvm::Instruction& instruction : llvm::instructions(*constructor)) {
            if (auto* store = llvm::dyn_cast<llvm::StoreInst>(&instruction)) {
                if (auto* global = llvm::dyn_cast<llvm::GlobalVariable>(llvm::getUnderlyingObject(store->getPointerOperand(), 6)))
                    initialized.insert(global);
            }
        }
    }
    for (llvm::GlobalVariable* global : initialized)
        global->setConstant(false);
    for (const EntryPoint& entry : entries) {
        llvm::IRBuilder<> builder(&*entry.function->getEntryBlock().getFirstInsertionPt());
        for (llvm::Function* constructor : constructors)
            builder.CreateCall(constructor, {});
    }
    for (llvm::Function* constructor : constructors) {
        constructor->setLinkage(llvm::GlobalValue::InternalLinkage);
        constructor->addFnAttr(llvm::Attribute::AlwaysInline);
    }
    std::vector<std::string> names;
    for (llvm::GlobalVariable* global : initialized)
        names.push_back(global->getName().str());
    runPasses(module, true);

    std::set<llvm::GlobalVariable*> remaining;
    for (const std::string& name : names) {
        if (llvm::GlobalVariable* global = module.getGlobalVariable(name, true))
            remaining.insert(global);
    }
    for (const EntryPoint& entry : entries) {
        // The constructors ran at the start of the entry block: its last store to a variable is the variable's value.
        std::map<llvm::GlobalVariable*, llvm::StoreInst*> stored;
        for (llvm::Instruction& instruction : entry.function->getEntryBlock()) {
            auto* store = llvm::dyn_cast<llvm::StoreInst>(&instruction);
            if (!store)
                continue;
            auto* global = llvm::dyn_cast<llvm::GlobalVariable>(store->getPointerOperand()->stripPointerCasts());
            if (global && remaining.count(global))
                stored[global] = store;
        }
        std::vector<llvm::LoadInst*> answered;
        for (llvm::Instruction& instruction : llvm::instructions(*entry.function)) {
            auto* load = llvm::dyn_cast<llvm::LoadInst>(&instruction);
            if (!load)
                continue;
            auto* global = llvm::dyn_cast<llvm::GlobalVariable>(load->getPointerOperand()->stripPointerCasts());
            if (!global)
                continue;
            auto found = stored.find(global);
            if (found == stored.end())
                continue;
            llvm::StoreInst* store = found->second;
            if (load->getType() == store->getValueOperand()->getType()
                && (store->getParent() != load->getParent() || store->comesBefore(load)))
                answered.push_back(load);
        }
        for (llvm::LoadInst* load : answered) {
            auto* global = llvm::cast<llvm::GlobalVariable>(load->getPointerOperand()->stripPointerCasts());
            load->replaceAllUsesWith(stored[global]->getValueOperand());
            load->eraseFromParent();
        }
    }
    for (llvm::GlobalVariable* global : remaining) {
        std::vector<llvm::Instruction*> stores;
        bool other = false;
        for (llvm::User* user : global->users()) {
            if (auto* store = llvm::dyn_cast<llvm::StoreInst>(user))
                stores.push_back(store);
            else
                other = true;
        }
        if (!other) {
            for (llvm::Instruction* store : stores)
                store->eraseFromParent();
        }
    }
    for (llvm::GlobalVariable* global : remaining) {
        for (llvm::User* user : global->users()) {
            if (llvm::isa<llvm::Instruction>(user) || llvm::isa<llvm::ConstantExpr>(user)) {
                error = "the program-scope constant '" + global->getName().str() + "' needs a value the compiler can work out";
                return false;
            }
        }
    }
    return true;
}

// Drops the function constants whose variable is gone.
void pruneFunctionConstants(llvm::Module& module)
{
    llvm::NamedMDNode* constants = module.getNamedMetadata("air.function_constants");
    if (!constants)
        return;
    std::vector<llvm::MDNode*> kept;
    for (llvm::MDNode* constant : constants->operands()) {
        if (constant->getNumOperands() && constant->getOperand(0))
            kept.push_back(constant);
    }
    if (constants->getNumOperands() == kept.size())
        return;
    constants->clearOperands();
    for (llvm::MDNode* constant : kept)
        constants->addOperand(constant);
}

// The functions that the standard library leaves to the front end: reef.msl.*.
bool lowerBuiltins(llvm::Module& module, std::string& error)
{
    llvm::LLVMContext& context = module.getContext();
    pruneFunctionConstants(module);
    std::map<llvm::GlobalVariable*, llvm::MDNode*> constants;
    if (llvm::NamedMDNode* list = module.getNamedMetadata("air.function_constants")) {
        for (llvm::MDNode* constant : list->operands()) {
            auto* value = llvm::dyn_cast_or_null<llvm::ValueAsMetadata>(constant->getOperand(0).get());
            if (!value)
                continue;
            if (auto* global = llvm::dyn_cast<llvm::GlobalVariable>(value->getValue()))
                constants[global] = constant;
        }
    }
    std::map<uint64_t, llvm::GlobalVariable*> samplers;
    std::map<llvm::GlobalVariable*, llvm::GlobalVariable*> defined;

    std::vector<llvm::Function*> builtins;
    for (llvm::Function& function : module) {
        if (function.isDeclaration() && function.getName().starts_with("reef.msl."))
            builtins.push_back(&function);
    }
    for (llvm::Function* builtin : builtins) {
        const llvm::StringRef name = builtin->getName().drop_front(9);
        std::vector<llvm::CallInst*> calls;
        for (llvm::User* user : builtin->users()) {
            if (auto* call = llvm::dyn_cast<llvm::CallInst>(user))
                calls.push_back(call);
        }
        for (llvm::CallInst* call : calls) {
            llvm::IRBuilder<> builder(call);
            llvm::Value* replacement = nullptr;
            if (name == "constant_sampler") {
                auto* state = llvm::dyn_cast<llvm::ConstantInt>(call->getArgOperand(0));
                if (!state) {
                    // A sampler that a buffer holds: its handle is there at run time, the state is never used.
                    if (!llvm::isa<llvm::LoadInst>(call->getArgOperand(0))) {
                        error = "a sampler declared in the program needs a state known at compile time";
                        return false;
                    }
                    call->replaceAllUsesWith(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(call->getType())));
                    call->eraseFromParent();
                    continue;
                }
                llvm::GlobalVariable*& global = samplers[state->getZExtValue()];
                if (!global) {
                    global = new llvm::GlobalVariable(module, llvm::Type::getInt64Ty(context), true, llvm::GlobalValue::InternalLinkage, state,
                                                      "__air_sampler_state", nullptr, llvm::GlobalValue::NotThreadLocal, 2);
                    global->setAlignment(llvm::Align(8));
                }
                replacement = global;
                if (call->getType() != global->getType())
                    replacement = builder.CreateAddrSpaceCast(global, call->getType());
            } else if (name == "function_constant_defined") {
                auto* global = llvm::dyn_cast<llvm::GlobalVariable>(call->getArgOperand(0)->stripPointerCasts());
                auto found = global ? constants.find(global) : constants.end();
                if (found == constants.end()) {
                    error = "is_function_constant_defined needs a function constant";
                    return false;
                }
                llvm::GlobalVariable*& flag = defined[global];
                if (!flag) {
                    flag = new llvm::GlobalVariable(module, llvm::Type::getInt8Ty(context), true, llvm::GlobalValue::ExternalLinkage, nullptr,
                                                    global->getName() + ".defined", nullptr, llvm::GlobalValue::NotThreadLocal, 2);
                    // The constant's own entry, for the flag: AIR's way to ask whether the pipeline gave a value.
                    std::vector<llvm::Metadata*> operands = {llvm::ValueAsMetadata::get(flag),
                                                             llvm::MDString::get(context, "air.function_constant_defined")};
                    operands.insert(operands.end(), found->second->op_begin() + 2, found->second->op_end());
                    module.getNamedMetadata("air.function_constants")->addOperand(llvm::MDTuple::get(context, operands));
                }
                llvm::Value* value = builder.CreateLoad(builder.getInt8Ty(), flag);
                value = builder.CreateICmpNE(value, builder.getInt8(0));
                replacement = builder.CreateZExt(value, call->getType());
            } else {
                // Textures: AIR's function returns the value and whether the texel was resident.
                std::vector<llvm::Type*> types;
                for (llvm::Value* argument : call->args())
                    types.push_back(argument->getType());
                llvm::Type* result = llvm::StructType::get(context, {call->getType(), builder.getInt8Ty()});
                llvm::FunctionCallee callee = module.getOrInsertFunction("air." + name.str(), llvm::FunctionType::get(result, types, false));
                auto* function = llvm::cast<llvm::Function>(callee.getCallee());
                function->addFnAttr(llvm::Attribute::NoUnwind);
                function->addFnAttr(llvm::Attribute::WillReturn);
                function->setOnlyReadsMemory();
                if (isConvergent(builtin->getName()))
                    function->addFnAttr(llvm::Attribute::Convergent);
                std::vector<llvm::Value*> arguments(call->arg_begin(), call->arg_end());
                replacement = builder.CreateExtractValue(builder.CreateCall(function, arguments), 0);
            }
            call->replaceAllUsesWith(replacement);
            call->eraseFromParent();
        }
        builtin->eraseFromParent();
    }
    if (!samplers.empty()) {
        llvm::NamedMDNode* states = module.getOrInsertNamedMetadata("air.sampler_states");
        for (const auto& [state, global] : samplers)
            states->addOperand(llvm::MDTuple::get(context, {llvm::MDString::get(context, "air.sampler_state"), llvm::ValueAsMetadata::get(global)}));
    }
    return true;
}

// Conversions between integers and floating point values are AIR functions.
void lowerConversions(llvm::Module& module)
{
    std::vector<llvm::Instruction*> conversions;
    for (llvm::Function& function : module) {
        for (llvm::Instruction& instruction : llvm::instructions(function)) {
            if (llvm::isa<llvm::FPToUIInst>(instruction) || llvm::isa<llvm::FPToSIInst>(instruction) || llvm::isa<llvm::UIToFPInst>(instruction)
                || llvm::isa<llvm::SIToFPInst>(instruction))
                conversions.push_back(&instruction);
        }
    }
    for (llvm::Instruction* conversion : conversions) {
        const char* source = "u";
        const char* target = "f";
        if (conversion->getOpcode() != llvm::Instruction::UIToFP) {
            source = "s";
            if (conversion->getOpcode() != llvm::Instruction::SIToFP) {
                source = "f";
                target = conversion->getOpcode() == llvm::Instruction::FPToSI ? "s" : "u";
            }
        }
        llvm::IRBuilder<> builder(conversion);
        llvm::Value* operand = conversion->getOperand(0);
        const std::string name = std::string("air.convert.") + target + "." + typeSuffix(conversion->getType()) + "." + source + "."
            + typeSuffix(operand->getType());
        llvm::CallInst* call = callAir(builder, module, name, conversion->getType(), {operand});
        conversion->replaceAllUsesWith(call);
        conversion->eraseFromParent();
    }
}

// LLVM's intrinsics that the optimizer wrote, as AIR's functions.
bool lowerIntrinsics(llvm::Module& module, bool fastMath, std::string& error)
{
    const std::string fast = fastMath ? "fast_" : "";
    std::vector<llvm::CallInst*> calls;
    for (llvm::Function& function : module) {
        for (llvm::Instruction& instruction : llvm::instructions(function)) {
            auto* call = llvm::dyn_cast<llvm::CallInst>(&instruction);
            if (call && call->getCalledFunction() && call->getCalledFunction()->isIntrinsic())
                calls.push_back(call);
        }
    }
    for (llvm::CallInst* call : calls) {
        const llvm::Intrinsic::ID id = call->getCalledFunction()->getIntrinsicID();
        llvm::IRBuilder<> builder(call);
        llvm::Type* type = call->getType();
        llvm::Value* replacement = nullptr;
        if (id == llvm::Intrinsic::cttz) {
            replacement = callAir(builder, module, "air.ctz.u." + typeSuffix(type), type, {call->getArgOperand(0)});
        } else if (id == llvm::Intrinsic::abs) {
            replacement = callAir(builder, module, "air.abs.s." + typeSuffix(type), type, {call->getArgOperand(0)});
        } else if (id == llvm::Intrinsic::ctlz) {
            replacement = callAir(builder, module, "air.clz.u." + typeSuffix(type), type, {call->getArgOperand(0)});
        } else if (id == llvm::Intrinsic::bswap) {
            // Bytes in the other order: shifts and masks.
            const unsigned bytes = type->getScalarSizeInBits() / 8;
            llvm::Value* value = call->getArgOperand(0);
            replacement = llvm::Constant::getNullValue(type);
            for (unsigned shift = 0; shift != bytes * 8; shift += 8) {
                llvm::Constant* mask = llvm::ConstantInt::get(type, 0xff);
                llvm::Value* part = builder.CreateAnd(builder.CreateLShr(value, llvm::ConstantInt::get(type, shift)), mask);
                part = builder.CreateShl(part, llvm::ConstantInt::get(type, bytes * 8 - 8 - shift));
                replacement = builder.CreateOr(replacement, part);
            }
        } else if (id == llvm::Intrinsic::fshl || id == llvm::Intrinsic::fshr) {
            llvm::Value* first = call->getArgOperand(0);
            llvm::Value* second = call->getArgOperand(1);
            llvm::Value* amount = call->getArgOperand(2);
            if (first == second && id == llvm::Intrinsic::fshl) {
                replacement = callAir(builder, module, "air.rotate.u." + typeSuffix(type), type, {first, amount});
            } else {
                // A funnel shift: the shifts that make it.
                llvm::Type* integer = first->getType();
                llvm::Constant* width = llvm::ConstantInt::get(integer, integer->getScalarSizeInBits());
                llvm::Value* shift = builder.CreateURem(amount, width);
                llvm::Value* rest = builder.CreateSub(width, shift);
                llvm::Value* value;
                if (id == llvm::Intrinsic::fshl) {
                    llvm::Value* low = builder.CreateLShr(second, rest);
                    value = builder.CreateOr(builder.CreateShl(first, shift), low);
                } else {
                    llvm::Value* low = builder.CreateLShr(second, shift);
                    value = builder.CreateOr(builder.CreateShl(first, rest), low);
                }
                llvm::Value* zero = builder.CreateICmpEQ(shift, llvm::Constant::getNullValue(integer));
                replacement = builder.CreateSelect(zero, id == llvm::Intrinsic::fshl ? first : second, value);
            }
        } else if (id == llvm::Intrinsic::fmuladd) {
            llvm::Value* product = builder.CreateFMulFMF(call->getArgOperand(0), call->getArgOperand(1), call);
            replacement = builder.CreateFAdd(product, call->getArgOperand(2));
            if (auto* instruction = llvm::dyn_cast<llvm::Instruction>(replacement))
                instruction->copyFastMathFlags(call);
        } else if (id == llvm::Intrinsic::assume || id == llvm::Intrinsic::dbg_declare || id == llvm::Intrinsic::dbg_label
                   || id == llvm::Intrinsic::dbg_value || id == llvm::Intrinsic::experimental_noalias_scope_decl
                   || id == llvm::Intrinsic::lifetime_end || id == llvm::Intrinsic::lifetime_start || id == llvm::Intrinsic::ptr_annotation
                   || id == llvm::Intrinsic::var_annotation) {
            call->eraseFromParent();
            continue;
        } else {
            const std::vector<llvm::Value*> arguments(call->arg_begin(), call->arg_end());
            const std::string suffix = typeSuffix(type);
            for (const Operation& operation : floatOperations) {
                if (operation.id != id)
                    continue;
                // copysign only moves a bit: there is nothing to make faster.
                const std::string prefix = id == llvm::Intrinsic::copysign ? std::string() : fast;
                replacement = callAir(builder, module, "air." + prefix + operation.name + "." + suffix, type, arguments);
                break;
            }
            if (!replacement) {
                for (const Operation& operation : integerOperations) {
                    if (operation.id != id)
                        continue;
                    replacement = callAir(builder, module, std::string("air.") + operation.name + "." + suffix, type, arguments);
                    break;
                }
            }
            if (!replacement) {
                error = "the optimizer wrote " + call->getCalledFunction()->getName().str() + ", which has no AIR form";
                return false;
            }
        }
        if (llvm::isa<llvm::CallInst>(replacement) && llvm::isa<llvm::FPMathOperator>(call))
            llvm::cast<llvm::CallInst>(replacement)->setFastMathFlags(call->getFastMathFlags());
        call->replaceAllUsesWith(replacement);
        call->eraseFromParent();
    }
    // The intrinsics are gone, and so are the functions of the library that nothing calls.
    std::vector<llvm::Function*> unused;
    for (llvm::Function& function : module) {
        if (function.isDeclaration() && function.use_empty())
            unused.push_back(&function);
    }
    for (llvm::Function* function : unused)
        function->eraseFromParent();
    return true;
}

// The uses of a pointer follow it to the same object in another address space: loads read through the new pointer, address
// computations are made again on it and casts start from it. Anything else keeps the object where it is.
bool moveUses(llvm::Value* from, llvm::Value* to)
{
    std::vector<llvm::User*> users(from->user_begin(), from->user_end());
    // Constant expressions on the pointer become instructions in front of the instructions that use them.
    for (llvm::User* user : users) {
        auto* expression = llvm::dyn_cast<llvm::ConstantExpr>(user);
        if (!expression)
            continue;
        llvm::Instruction* model = expression->getAsInstruction();
        llvm::Instruction* first = nullptr;
        for (llvm::User* inner : expression->users()) {
            if ((first = llvm::dyn_cast<llvm::Instruction>(inner)))
                break;
        }
        if (first && first->getFunction()) {
            const std::vector<llvm::User*> inner(expression->user_begin(), expression->user_end());
            for (llvm::User* use : inner) {
                auto* instruction = llvm::dyn_cast<llvm::Instruction>(use);
                if (!instruction)
                    return false;
                llvm::Instruction* copy = model->clone();
                copy->insertBefore(instruction);
                instruction->replaceUsesOfWith(expression, copy);
            }
        }
        model->deleteValue();
    }
    users.assign(from->user_begin(), from->user_end());
    for (llvm::User* user : users) {
        auto* instruction = llvm::dyn_cast<llvm::Instruction>(user);
        if (!instruction)
            return false;
        llvm::IRBuilder<> builder(instruction);
        if (auto* load = llvm::dyn_cast<llvm::LoadInst>(instruction)) {
            load->setOperand(0, to);
        } else if (auto* address = llvm::dyn_cast<llvm::GetElementPtrInst>(instruction)) {
            const std::vector<llvm::Value*> indices(address->idx_begin(), address->idx_end());
            llvm::Value* moved = builder.CreateGEP(address->getSourceElementType(), to, indices, "",
                                                   address->isInBounds() ? llvm::GEPNoWrapFlags::inBounds() : llvm::GEPNoWrapFlags::none());
            if (!moveUses(address, moved))
                return false;
            address->eraseFromParent();
        } else if (auto* cast = llvm::dyn_cast<llvm::AddrSpaceCastInst>(instruction)) {
            llvm::Value* value = to;
            if (to->getType() != cast->getType())
                value = builder.CreateAddrSpaceCast(to, cast->getType());
            cast->replaceAllUsesWith(value);
            cast->eraseFromParent();
        } else {
            return false;
        }
    }
    return true;
}

}

bool lowerModule(llvm::Module& module, std::vector<EntryPoint>& entries, const ModuleOptions& options, std::string& error)
{
    for (const char* name : {"llvm.global.annotations", "llvm.used", "llvm.compiler.used"}) {
        if (llvm::GlobalVariable* variable = module.getGlobalVariable(name, true))
            variable->eraseFromParent();
    }
    std::set<llvm::Function*> entryFunctions;
    for (const EntryPoint& entry : entries)
        entryFunctions.insert(entry.function);

    for (llvm::Function& function : module) {
        for (llvm::Instruction& instruction : llvm::make_early_inc_range(llvm::instructions(function))) {
            auto* call = llvm::dyn_cast<llvm::CallInst>(&instruction);
            llvm::Function* callee = call ? call->getCalledFunction() : nullptr;
            if (!callee || !callee->isIntrinsic())
                continue;
            if (callee->getIntrinsicID() == llvm::Intrinsic::ptr_annotation) {
                call->replaceAllUsesWith(call->getArgOperand(0));
                call->eraseFromParent();
            } else if (callee->getIntrinsicID() == llvm::Intrinsic::var_annotation) {
                call->eraseFromParent();
            }
        }
    }
    for (llvm::Function& function : module) {
        function.removeFnAttr(llvm::Attribute::Convergent);
        function.setCallingConv(llvm::CallingConv::C);
        for (llvm::Instruction& instruction : llvm::instructions(function)) {
            if (auto* call = llvm::dyn_cast<llvm::CallBase>(&instruction)) {
                call->removeFnAttr(llvm::Attribute::Convergent);
                call->setCallingConv(llvm::CallingConv::C);
            }
        }
        const llvm::StringRef name = function.getName();
        if (!function.isDeclaration()) {
            if (!entryFunctions.count(&function)) {
                function.setLinkage(llvm::GlobalValue::InternalLinkage);
                function.removeFnAttr(llvm::Attribute::NoInline);
                function.removeFnAttr(llvm::Attribute::OptimizeNone);
                function.addFnAttr(llvm::Attribute::AlwaysInline);
            }
        } else if (name.starts_with("air.") || name.starts_with("reef.msl.")) {
            function.addFnAttr(llvm::Attribute::NoUnwind);
            function.addFnAttr(llvm::Attribute::WillReturn);
            bool effect = false;
            for (const char* prefix : effects) {
                if (name.starts_with(prefix)) {
                    effect = true;
                    break;
                }
            }
            if (!effect)
                function.setDoesNotAccessMemory();
            else if (name.starts_with("air.get_") || name.starts_with("reef.msl."))
                function.setOnlyReadsMemory();
            if (isConvergent(name))
                function.addFnAttr(llvm::Attribute::Convergent);
        }
    }

    // The list of constructors again, without the data each one is associated with.
    llvm::GlobalVariable* constructors = module.getGlobalVariable("llvm.global_ctors", true);
    if (constructors && !constructors->isDeclaration()) {
        if (auto* list = llvm::dyn_cast<llvm::ConstantArray>(constructors->getInitializer())) {
            llvm::LLVMContext& context = module.getContext();
            llvm::PointerType* pointer = llvm::PointerType::get(context, 0);
            llvm::StructType* entryType = llvm::StructType::get(context, {llvm::Type::getInt32Ty(context), pointer, pointer});
            std::vector<llvm::Constant*> items;
            for (llvm::Use& use : list->operands()) {
                auto* item = llvm::cast<llvm::ConstantStruct>(use.get());
                llvm::Constant* fields[] = {item->getOperand(0), item->getOperand(1), llvm::ConstantPointerNull::get(pointer)};
                items.push_back(llvm::ConstantStruct::get(entryType, fields));
            }
            llvm::ArrayType* type = llvm::ArrayType::get(entryType, items.size());
            constructors->eraseFromParent();
            new llvm::GlobalVariable(module, type, false, llvm::GlobalValue::AppendingLinkage, llvm::ConstantArray::get(type, items),
                                     "llvm.global_ctors", nullptr, llvm::GlobalValue::NotThreadLocal, 0);
        }
    }

    runPasses(module, true);
    if (!runConstructors(module, entries, error) || !lowerBuiltins(module, error))
        return false;

    for (llvm::Function& function : module) {
        for (llvm::Instruction& instruction : llvm::instructions(function)) {
            // Address space 4 is clang's generic one: AIR has nothing like it.
            const auto generic = [](llvm::Type* type) {
                if (type->isVectorTy())
                    type = llvm::cast<llvm::VectorType>(type)->getElementType();
                return type->isPointerTy() && type->getPointerAddressSpace() == 4;
            };
            bool unknown = generic(instruction.getType());
            for (llvm::Value* operand : instruction.operands()) {
                if (unknown)
                    break;
                if (!llvm::isa<llvm::Function>(operand))
                    unknown = generic(operand->getType());
            }
            if (!unknown)
                continue;
            std::string text;
            llvm::raw_string_ostream stream(text);
            instruction.print(stream);
            error = "a pointer whose address space the compiler cannot tell remains in " + function.getName().str() + ":" + text;
            return false;
        }
    }

    std::vector<llvm::GlobalVariable*> deviceConstants;
    for (llvm::GlobalVariable& global : module.globals()) {
        if (global.getAddressSpace() == 1 && global.isConstant() && !global.isDeclaration())
            deviceConstants.push_back(&global);
    }
    for (llvm::GlobalVariable* global : deviceConstants) {
        auto* moved = new llvm::GlobalVariable(module, global->getValueType(), true, global->getLinkage(), global->getInitializer(), "",
                                               nullptr, llvm::GlobalValue::NotThreadLocal, 2);
        moved->setAlignment(global->getAlign());
        moved->takeName(global);
        if (!moveUses(global, moved)) {
            error = "the constant '" + moved->getName().str() + "' is used in a way the compiler cannot place";
            return false;
        }
        global->eraseFromParent();
    }

    std::vector<llvm::MemIntrinsic*> copies;
    for (llvm::Function& function : module) {
        for (llvm::Instruction& instruction : llvm::instructions(function)) {
            if (auto* copy = llvm::dyn_cast<llvm::MemIntrinsic>(&instruction))
                copies.push_back(copy);
        }
    }
    for (llvm::MemIntrinsic* copy : copies) {
        llvm::TargetTransformInfo information(module.getDataLayout());
        if (auto* memcpy = llvm::dyn_cast<llvm::MemCpyInst>(copy)) {
            llvm::expandMemCpyAsLoop(memcpy, information);
            copy->eraseFromParent();
        } else if (auto* memmove = llvm::dyn_cast<llvm::MemMoveInst>(copy)) {
            llvm::expandMemMoveAsLoop(memmove, information);
            copy->eraseFromParent();
        } else if (auto* memset = llvm::dyn_cast<llvm::MemSetInst>(copy)) {
            llvm::expandMemSetAsLoop(memset);
            copy->eraseFromParent();
        }
    }

    runPasses(module, false);
    lowerConversions(module);
    if (!lowerIntrinsics(module, options.fastMath, error))
        return false;
    for (llvm::Function& function : module) {
        if (!function.isDeclaration() && !entryFunctions.count(&function)) {
            error = "the function " + function.getName().str() + " could not be inlined into an entry point";
            return false;
        }
    }

    llvm::LLVMContext& context = module.getContext();
    const auto integer = [&](uint64_t value) {
        return llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), value));
    };
    for (const EntryPoint& entry : entries) {
        std::vector<llvm::Metadata*> arguments;
        for (size_t i = 0; i < entry.arguments.size(); i++) {
            std::vector<llvm::Metadata*> operands = {integer(i)};
            operands.insert(operands.end(), entry.arguments[i].begin(), entry.arguments[i].end());
            if (entry.function->getArg(i)->use_empty())
                operands.push_back(llvm::MDString::get(context, "air.arg_unused"));
            arguments.push_back(llvm::MDTuple::get(context, operands));
        }
        std::vector<llvm::Metadata*> node = {
            llvm::ValueAsMetadata::get(entry.function),
            llvm::MDTuple::get(context, entry.outputs),
            llvm::MDTuple::get(context, arguments),
        };
        node.insert(node.end(), entry.attributes.begin(), entry.attributes.end());
        module.getOrInsertNamedMetadata(entryKinds[entry.kind])->addOperand(llvm::MDTuple::get(context, node));
    }

    const int air = options.majorVersion > 2 ? options.minorVersion + 5 : options.majorVersion == 2 ? options.minorVersion / 2 + 2 : 0;
    module.setModuleFlag(llvm::Module::Error, "wchar_size", integer(4));
    module.setModuleFlag(llvm::Module::Max, "air.max_device_buffers", integer(31));
    module.setModuleFlag(llvm::Module::Max, "air.max_constant_buffers", integer(31));
    module.setModuleFlag(llvm::Module::Max, "air.max_threadgroup_buffers", integer(31));
    module.setModuleFlag(llvm::Module::Max, "air.max_textures", integer(128));
    module.setModuleFlag(llvm::Module::Max, "air.max_read_write_textures", integer(8));
    module.setModuleFlag(llvm::Module::Max, "air.max_samplers", integer(16));
    module.getOrInsertNamedMetadata("air.version")->addOperand(llvm::MDTuple::get(context, {integer(2), integer(air), integer(0)}));
    module.getOrInsertNamedMetadata("air.language_version")
        ->addOperand(llvm::MDTuple::get(
            context, {llvm::MDString::get(context, "Metal"), integer(options.majorVersion), integer(options.minorVersion), integer(0)}));
    llvm::NamedMDNode* compileOptions = module.getOrInsertNamedMetadata("air.compile_options");
    compileOptions->addOperand(llvm::MDTuple::get(context, {llvm::MDString::get(context, "air.compile.denorms_disable")}));
    compileOptions->addOperand(llvm::MDTuple::get(
        context, {llvm::MDString::get(context, options.fastMath ? "air.compile.fast_math_enable" : "air.compile.fast_math_disable")}));
    compileOptions->addOperand(llvm::MDTuple::get(context, {llvm::MDString::get(context, "air.compile.framebuffer_fetch_enable")}));
    if (llvm::NamedMDNode* ident = module.getNamedMetadata("llvm.ident"))
        module.eraseNamedMetadata(ident);
    module.getOrInsertNamedMetadata("llvm.ident")
        ->addOperand(llvm::MDTuple::get(context, {llvm::MDString::get(context, "cutefish-reef Metal Shading Language front end")}));
    module.setDataLayout("e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-"
                         "v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024-n8:16:32");
    module.setTargetTriple("air64_v2" + std::to_string(air) + "-apple-macosx13.0.0");
    pruneFunctionConstants(module);

    std::string problems;
    llvm::raw_string_ostream stream(problems);
    if (llvm::verifyModule(module, &stream)) {
        error = "the compiler wrote an invalid module: " + problems;
        return false;
    }
    return true;
}

}
