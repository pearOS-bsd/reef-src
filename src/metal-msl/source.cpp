// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Passes over the text of a program: what of the Metal Shading Language is not C++ and no macro can mend.

#include "frontend.h"

#include <clang/Basic/LangOptions.h>
#include <clang/Lex/Lexer.h>
#include <clang/Lex/Token.h>

#include <cctype>
#include <cstdio>
#include <vector>

namespace cutefish::reef::metal::msl {

namespace {

struct Qualifier {
    const char* name;
    const char* replacement;
};

// Address spaces after the parameters of a member function, as overloads that lose to the unqualified one: the same length,
// so that columns in errors stay right.
const Qualifier qualifiers[] = {
    {"device", "__mqd_"},
    {"constant", "__mqc___"},
    {"threadgroup", "__mqtg_____"},
    {"thread", "__mqt_"},
};

bool is(const clang::Token& token, llvm::StringRef name)
{
    return token.is(clang::tok::raw_identifier) && token.getRawIdentifier() == name;
}

// What before a brace makes it the body of a function: the end of the parameters, or a qualifier after them.
bool endsDeclarator(const clang::Token& token)
{
    if (token.is(clang::tok::r_paren))
        return true;
    for (const char* name : {"const", "volatile", "noexcept", "mutable", "constant", "device", "threadgroup", "thread"}) {
        if (is(token, name))
            return true;
    }
    return false;
}

}

bool relaxConstants(std::string& source, const std::string& errors)
{
    bool changed = false;
    for (size_t start = 0; start < errors.size();) {
        size_t end = errors.find('\n', start);
        if (end == std::string::npos)
            end = errors.size();
        const std::string line = errors.substr(start, end - start);
        start = end + 1;
        if (line.compare(0, 15, "program_source:") != 0)
            continue;
        if (line.find("must be initialized by a constant expression") == std::string::npos
            || line.find("constexpr variable") == std::string::npos)
            continue;
        unsigned row = 0;
        unsigned column = 0;
        if (std::sscanf(line.c_str() + 15, "%u:%u", &row, &column) != 2 || !row || !column)
            continue;
        size_t offset = 0;
        bool found = true;
        for (unsigned current = 1; current != row; current++) {
            if (offset >= source.size()) {
                found = false;
                break;
            }
            const size_t next = source.find('\n', offset);
            if (next == std::string::npos) {
                found = false;
                break;
            }
            offset = next + 1;
        }
        if (!found)
            continue;
        // constexpr is before the name of the variable, in the same declaration.
        for (size_t at = offset + column - 1; at-- > 0;) {
            const char c = source[at];
            if (c == ';' || c == '{' || c == '}')
                break;
            if (source.compare(at, 9, "constexpr") != 0)
                continue;
            if (at && std::isalnum(static_cast<unsigned char>(source[at - 1])))
                continue;
            const char after = source[at + 9];
            if (std::isalnum(static_cast<unsigned char>(after)) || after == '_')
                continue;
            source.replace(at, 9, "const    ");
            changed = true;
            break;
        }
    }
    return changed;
}

std::string prepareSource(const std::string& source)
{
    clang::LangOptions options;
    options.CPlusPlus = 1;
    options.CPlusPlus11 = 1;
    options.CPlusPlus14 = 1;
    options.CPlusPlus17 = 1;
    clang::Lexer lexer(clang::SourceLocation::getFromRawEncoding(1), options, source.data(), source.data(), source.data() + source.size());
    std::vector<clang::Token> tokens;
    clang::Token token;
    while (true) {
        const bool end = lexer.LexFromRawLexer(token);
        if (token.is(clang::tok::eof))
            break;
        tokens.push_back(token);
        if (end)
            break;
    }

    std::string text = source;
    const auto replace = [&](const clang::Token& word, const char* replacement) {
        text.replace(word.getLocation().getRawEncoding() - 1, word.getLength(), replacement);
    };
    int attributes = 0;
    // For each open brace: whether it opens the body of a function.
    std::vector<bool> bodies;
    // The parenthesis that closes the parameters of a function-like macro: no member function ends there.
    size_t macroParameters = size_t(-1);
    for (size_t i = 0; i < tokens.size(); i++) {
        const clang::Token& word = tokens[i];
        const clang::Token* before = i ? &tokens[i - 1] : nullptr;
        if (word.is(clang::tok::hash)) {
            if (word.isAtStartOfLine() && i + 3 < tokens.size() && is(tokens[i + 1], "define") && tokens[i + 2].is(clang::tok::raw_identifier)
                && tokens[i + 3].is(clang::tok::l_paren) && !tokens[i + 3].hasLeadingSpace()) {
                for (size_t j = i + 4; j < tokens.size() && !tokens[j].isAtStartOfLine(); j++) {
                    if (tokens[j].is(clang::tok::r_paren)) {
                        macroParameters = j;
                        break;
                    }
                }
            }
            continue;
        }
        if (word.is(clang::tok::l_square)) {
            if (i + 1 < tokens.size() && tokens[i + 1].is(clang::tok::l_square)) {
                attributes++;
                i++;
            }
            continue;
        }
        if (attributes > 0) {
            if (word.is(clang::tok::r_square)) {
                if (i + 1 < tokens.size() && tokens[i + 1].is(clang::tok::r_square)) {
                    attributes--;
                    i++;
                }
            } else if (is(word, "threadgroup")) {
                replace(word, "__msl_tgrp_");
            }
            continue;
        }
        if (word.is(clang::tok::l_brace)) {
            bool body = !bodies.empty() && bodies.back();
            if (before && endsDeclarator(*before))
                body = true;
            bodies.push_back(body);
            continue;
        }
        if (word.is(clang::tok::r_brace)) {
            if (!bodies.empty())
                bodies.pop_back();
            continue;
        }
        if (word.is(clang::tok::raw_identifier)) {
            // A threadgroup variable declared in a function body.
            if (is(word, "threadgroup") && !bodies.empty() && bodies.back() && before
                && (before->is(clang::tok::l_brace) || before->is(clang::tok::r_brace) || before->is(clang::tok::semi)))
                replace(word, "__msl_tgl__");
            continue;
        }
        if (!word.is(clang::tok::r_paren) || i == macroParameters)
            continue;
        // The qualifiers of a member function: const and volatile stay, an address space becomes an overload.
        for (size_t j = i + 1; j < tokens.size(); j++) {
            const clang::Token& next = tokens[j];
            if (is(next, "const") || is(next, "volatile"))
                continue;
            const Qualifier* found = nullptr;
            for (const Qualifier& qualifier : qualifiers) {
                if (is(next, qualifier.name))
                    found = &qualifier;
            }
            if (!found)
                break;
            replace(next, found->replacement);
        }
    }
    return text;
}

}
