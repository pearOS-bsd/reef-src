// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Reads JSON.

#include "web.h"

#include <cstdlib>
#include <cstring>

namespace cutefish::reef::web {

static unsigned parseHex(const char*& at)
{
    unsigned code = 0;
    for (int i = 0; i < 4 && *at; i++) {
        const char c = *at++;
        code <<= 4;
        if (c >= '0' && c <= '9')
            code |= unsigned(c - '0');
        else if (c >= 'a' && c <= 'f')
            code |= unsigned(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
            code |= unsigned(c - 'A' + 10);
    }
    return code;
}

static void appendUtf8(std::string& text, unsigned code)
{
    if (code < 0x80) {
        text += char(code);
    } else if (code < 0x800) {
        text += char(0xc0 | (code >> 6));
        text += char(0x80 | (code & 0x3f));
    } else if (code < 0x10000) {
        text += char(0xe0 | (code >> 12));
        text += char(0x80 | ((code >> 6) & 0x3f));
        text += char(0x80 | (code & 0x3f));
    } else {
        text += char(0xf0 | (code >> 18));
        text += char(0x80 | ((code >> 12) & 0x3f));
        text += char(0x80 | ((code >> 6) & 0x3f));
        text += char(0x80 | (code & 0x3f));
    }
}

static std::string parseString(const char*& at)
{
    std::string text;
    at++;
    while (*at && *at != '"') {
        const char c = *at++;
        if (c != '\\') {
            text += c;
            continue;
        }
        const char escape = *at ? *at++ : '\0';
        switch (escape) {
        case 'b':
            text += '\b';
            break;
        case 'f':
            text += '\f';
            break;
        case 'n':
            text += '\n';
            break;
        case 'r':
            text += '\r';
            break;
        case 't':
            text += '\t';
            break;
        case 'u': {
            unsigned code = parseHex(at);
            if (code >= 0xd800 && code < 0xdc00 && at[0] == '\\' && at[1] == 'u') {
                at += 2;
                code = 0x10000 + ((code - 0xd800) << 10) + (parseHex(at) - 0xdc00);
            }
            appendUtf8(text, code);
            break;
        }
        default:
            text += escape;
            break;
        }
    }
    if (*at == '"')
        at++;
    return text;
}

static void skipSpace(const char*& at)
{
    while (*at == ' ' || *at == '\t' || *at == '\n' || *at == '\r')
        at++;
}

// Null for null and for what is not JSON.
WKTypeRef parseJson(const char*& at)
{
    skipSpace(at);
    if (*at == '"') {
        const std::string text = parseString(at);
        return WKStringCreateWithUTF8CString(text.c_str());
    }
    if (*at == '{') {
        at++;
        WKMutableDictionaryRef dictionary = WKMutableDictionaryCreate();
        skipSpace(at);
        while (*at && *at != '}') {
            skipSpace(at);
            if (*at != '"')
                break;
            const std::string name = parseString(at);
            skipSpace(at);
            if (*at == ':')
                at++;
            WKTypeRef value = parseJson(at);
            WKStringRef key = WKStringCreateWithUTF8CString(name.c_str());
            WKDictionarySetItem(dictionary, key, value);
            WKRelease(key);
            if (value)
                WKRelease(value);
            skipSpace(at);
            if (*at == ',')
                at++;
        }
        if (*at == '}')
            at++;
        return dictionary;
    }
    if (*at == '[') {
        at++;
        WKMutableArrayRef array = WKMutableArrayCreate();
        skipSpace(at);
        while (*at && *at != ']') {
            if (WKTypeRef value = parseJson(at)) {
                WKArrayAppendItem(array, value);
                WKRelease(value);
            }
            skipSpace(at);
            if (*at == ',')
                at++;
            skipSpace(at);
        }
        if (*at == ']')
            at++;
        return array;
    }
    if (strncmp(at, "true", 4) == 0) {
        at += 4;
        return WKBooleanCreate(true);
    }
    if (strncmp(at, "false", 5) == 0) {
        at += 5;
        return WKBooleanCreate(false);
    }
    if (strncmp(at, "null", 4) == 0) {
        at += 4;
        return nullptr;
    }
    char* end = nullptr;
    const double number = strtod(at, &end);
    if (end == at) {
        at++;
        return nullptr;
    }
    at = end;
    return WKDoubleCreate(number);
}

} // namespace cutefish::reef::web
