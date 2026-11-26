// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The attributes of the Metal Shading Language, kept as annotations: [[buffer(0)]] is annotate("msl:buffer:0").

#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/ParsedAttrInfo.h>
#include <clang/Sema/Lookup.h>
#include <clang/Sema/ParsedAttr.h>
#include <clang/Sema/Sema.h>

#include <optional>
#include <string>
#include <vector>

namespace cutefish::reef::metal::msl {
namespace {

enum class Arguments { None, Integer, Name, NameAndInteger, String };

struct AttributeName {
    const char* name;
    Arguments arguments;
};

const AttributeName attributeNames[] = {
    {"buffer", Arguments::Integer},
    {"texture", Arguments::Integer},
    {"sampler", Arguments::Integer},
    // [[threadgroup(n)]]: the source pass renames it, because threadgroup is a macro.
    {"__msl_tgrp_", Arguments::Integer},
    {"attribute", Arguments::Integer},
    {"color", Arguments::Integer},
    {"index", Arguments::Integer},
    {"raster_order_group", Arguments::Integer},
    {"id", Arguments::Integer},
    {"max_total_threads_per_threadgroup", Arguments::Integer},
    {"user", Arguments::Name},
    {"function_constant", Arguments::Integer},
    {"depth", Arguments::Name},
    {"patch", Arguments::NameAndInteger},
    {"host_name", Arguments::String},
    {"stage_in", Arguments::None},
    {"position", Arguments::None},
    {"vertex_id", Arguments::None},
    {"instance_id", Arguments::None},
    {"base_vertex", Arguments::None},
    {"base_instance", Arguments::None},
    {"front_facing", Arguments::None},
    {"point_coord", Arguments::None},
    {"point_size", Arguments::None},
    {"clip_distance", Arguments::None},
    {"sample_id", Arguments::None},
    {"sample_mask", Arguments::None},
    {"stencil", Arguments::None},
    {"primitive_id", Arguments::None},
    {"render_target_array_index", Arguments::None},
    {"viewport_array_index", Arguments::None},
    {"amplification_id", Arguments::None},
    {"amplification_count", Arguments::None},
    {"barycentric_coord", Arguments::None},
    {"flat", Arguments::None},
    {"center_perspective", Arguments::None},
    {"center_no_perspective", Arguments::None},
    {"centroid_perspective", Arguments::None},
    {"centroid_no_perspective", Arguments::None},
    {"sample_perspective", Arguments::None},
    {"sample_no_perspective", Arguments::None},
    {"invariant", Arguments::None},
    {"early_fragment_tests", Arguments::None},
    {"thread_position_in_grid", Arguments::None},
    {"thread_position_in_threadgroup", Arguments::None},
    {"threadgroup_position_in_grid", Arguments::None},
    {"threads_per_threadgroup", Arguments::None},
    {"threads_per_grid", Arguments::None},
    {"threadgroups_per_grid", Arguments::None},
    {"thread_index_in_threadgroup", Arguments::None},
    {"thread_index_in_simdgroup", Arguments::None},
    {"thread_index_in_quadgroup", Arguments::None},
    {"threads_per_simdgroup", Arguments::None},
    {"thread_execution_width", Arguments::None},
    {"simdgroup_index_in_threadgroup", Arguments::None},
    {"simdgroups_per_threadgroup", Arguments::None},
    {"quadgroup_index_in_threadgroup", Arguments::None},
    {"quadgroups_per_threadgroup", Arguments::None},
    {"vertex", Arguments::None},
    {"fragment", Arguments::None},
    {"kernel", Arguments::None},
    {"visible", Arguments::None},
    {"stitchable", Arguments::None},
};

constexpr char annotationPrefix[] = "msl:";

const AttributeName* findAttribute(llvm::StringRef name)
{
    for (const AttributeName& known : attributeNames) {
        if (name == known.name)
            return &known;
    }
    return nullptr;
}

// A lone name is parsed as an identifier, anything else as an expression.
std::optional<std::string> nameArgument(const clang::ParsedAttr& attribute)
{
    if (attribute.isArgIdent(0))
        return attribute.getArgAsIdent(0)->Ident->getName().str();
    const clang::Expr* expression = attribute.getArgAsExpr(0);
    if (expression) {
        if (const auto* reference = llvm::dyn_cast<clang::DeclRefExpr>(expression->IgnoreParenImpCasts()))
            return reference->getDecl()->getName().str();
    }
    return std::nullopt;
}

std::optional<int64_t> integerArgument(clang::Sema& sema, const clang::ParsedAttr& attribute, unsigned index)
{
    if (attribute.isArgIdent(index)) {
        const clang::IdentifierLoc* name = attribute.getArgAsIdent(index);
        clang::LookupResult result(sema, name->Ident, name->Loc, clang::Sema::LookupOrdinaryName);
        sema.LookupName(result, sema.getCurScope());
        if (!result.isSingleResult())
            return std::nullopt;
        const clang::NamedDecl* found = result.getFoundDecl();
        if (const auto* constant = llvm::dyn_cast<clang::EnumConstantDecl>(found))
            return constant->getInitVal().getExtValue();
        if (const auto* variable = llvm::dyn_cast<clang::VarDecl>(found)) {
            const clang::APValue* value = variable->evaluateValue();
            if (value && value->isInt())
                return value->getInt().getExtValue();
        }
        return std::nullopt;
    }
    const clang::Expr* expression = attribute.getArgAsExpr(index);
    if (!expression || expression->isValueDependent())
        return std::nullopt;
    clang::Expr::EvalResult result;
    if (!expression->EvaluateAsInt(result, sema.Context))
        return std::nullopt;
    return result.Val.getInt().getExtValue();
}

clang::VarDecl* variableArgument(clang::Sema& sema, const clang::ParsedAttr& attribute)
{
    if (attribute.isArgIdent(0)) {
        const clang::IdentifierLoc* name = attribute.getArgAsIdent(0);
        clang::LookupResult result(sema, name->Ident, name->Loc, clang::Sema::LookupOrdinaryName);
        sema.LookupName(result, sema.getCurScope());
        return result.getAsSingle<clang::VarDecl>();
    }
    if (clang::Expr* expression = attribute.getArgAsExpr(0)) {
        if (auto* reference = llvm::dyn_cast<clang::DeclRefExpr>(expression->IgnoreParenImpCasts()))
            return llvm::dyn_cast<clang::VarDecl>(reference->getDecl());
    }
    return nullptr;
}

void report(clang::Sema& sema, clang::SourceLocation location, const std::string& message)
{
    clang::DiagnosticsEngine& diagnostics = sema.getDiagnostics();
    diagnostics.Report(location, diagnostics.getCustomDiagID(clang::DiagnosticsEngine::Error, "%0")) << message;
}

class MslAttributes : public clang::ParsedAttrInfo {
public:
    MslAttributes()
    {
        OptArgs = 15;
        for (const AttributeName& known : attributeNames)
            m_spellings.push_back({clang::AttributeCommonInfo::AS_CXX11, known.name});
        Spellings = m_spellings;
    }

    AttrHandling handleDeclAttribute(clang::Sema& sema, clang::Decl* decl, const clang::ParsedAttr& attribute) const override
    {
        const llvm::StringRef spelling = attribute.getAttrName()->getName();
        const AttributeName* known = findAttribute(spelling);
        if (!known)
            return NotHandled;
        const std::string name = spelling == "__msl_tgrp_" ? "threadgroup" : spelling.str();
        const unsigned count = attribute.getNumArgs();
        std::string annotation = std::string(annotationPrefix) + name;

        const auto fail = [&](const std::string& message) {
            report(sema, attribute.getLoc(), "'" + name + "' attribute " + message);
            return AttributeNotApplied;
        };

        switch (known->arguments) {
        case Arguments::None:
            if (count != 0)
                return fail("takes no arguments");
            break;
        case Arguments::Integer: {
            if (count != 1)
                return fail("takes one integer argument");
            if (name == "function_constant" && llvm::isa<clang::FieldDecl>(decl)) {
                // On a member it names the constant that says whether the member is there.
                clang::VarDecl* constant = variableArgument(sema, attribute);
                if (!constant || constant->hasLocalStorage() || !constant->getType()->isBooleanType())
                    return fail("needs a boolean constant");
                constant->addAttr(clang::UsedAttr::CreateImplicit(sema.Context));
                decl->addAttr(clang::AnnotateAttr::CreateImplicit(
                    sema.Context, std::string(annotationPrefix) + "function_constant_condition:" + constant->getName().str(), nullptr, 0,
                    attribute.getRange()));
                return AttributeApplied;
            }
            const std::optional<int64_t> value = integerArgument(sema, attribute, 0);
            if (!value)
                return fail("needs an integer constant");
            if (*value < 0)
                return fail("needs a value that is not negative");
            annotation += ":" + std::to_string(*value);
            break;
        }
        case Arguments::Name: {
            if (count != 1)
                return fail("takes one name");
            const std::optional<std::string> argument = nameArgument(attribute);
            if (!argument)
                return fail("needs a name");
            annotation += ":" + *argument;
            break;
        }
        case Arguments::NameAndInteger: {
            if (count < 1 || count > 2)
                return fail("takes a name and an optional integer");
            const std::optional<std::string> argument = nameArgument(attribute);
            if (!argument)
                return fail("needs a name");
            annotation += ":" + *argument;
            if (count == 2) {
                const std::optional<int64_t> value = integerArgument(sema, attribute, 1);
                if (!value)
                    return fail("needs an integer constant");
                annotation += ":" + std::to_string(*value);
            }
            break;
        }
        case Arguments::String: {
            if (count != 1)
                return fail("takes one string");
            const auto* text = attribute.isArgExpr(0)
                ? llvm::dyn_cast_or_null<clang::StringLiteral>(attribute.getArgAsExpr(0)->IgnoreParenImpCasts())
                : nullptr;
            if (!text)
                return fail("needs a string literal");
            annotation += ":" + text->getString().str();
            break;
        }
        }

        // A function constant gets its value when the pipeline is made: its definition is not in the program.
        if (name == "function_constant") {
            auto* variable = llvm::dyn_cast<clang::VarDecl>(decl);
            if (variable && variable->hasGlobalStorage() && !variable->getInit())
                variable->setStorageClass(clang::SC_Extern);
        }
        decl->addAttr(clang::AnnotateAttr::CreateImplicit(sema.Context, annotation, nullptr, 0, attribute.getRange()));
        return AttributeApplied;
    }

private:
    std::vector<Spelling> m_spellings;
};

const clang::ParsedAttrInfoRegistry::Add<MslAttributes> registration("metal-shading-language",
                                                                     "The attributes of the Metal Shading Language");

}
}
