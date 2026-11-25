// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "web.h"

#include <cmath>
#include <cstdio>
#include <ctime>

namespace cutefish::reef::web {

static void appendString(std::string& json, const std::string& text)
{
    json += '"';
    for (const char c : text) {
        switch (c) {
        case '"':
            json += "\\\"";
            break;
        case '\\':
            json += "\\\\";
            break;
        case '\n':
            json += "\\n";
            break;
        case '\r':
            json += "\\r";
            break;
        case '\t':
            json += "\\t";
            break;
        default:
            if (static_cast<unsigned char>(c) < 0x20) {
                char escape[8];
                snprintf(escape, sizeof escape, "\\u%04x", c);
                json += escape;
            } else
                json += c;
            break;
        }
    }
    json += '"';
}

WKStringRef makeString(const char* text) { return WKStringCreateWithUTF8CString(text ? text : ""); }

WKURLRef makeUrl(const char* text) { return text && *text ? WKURLCreateWithUTF8CString(text) : nullptr; }

double now()
{
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return double(time.tv_sec) + double(time.tv_nsec) / 1e9;
}

std::string toString(WKStringRef string)
{
    if (!string)
        return {};
    const size_t capacity = WKStringGetMaximumUTF8CStringSize(string);
    std::string text(capacity, '\0');
    const size_t size = WKStringGetUTF8CString(string, text.data(), capacity);
    text.resize(size > 0 ? size - 1 : 0);
    return text;
}

std::string takeString(WKStringRef string)
{
    std::string text = toString(string);
    if (string)
        WKRelease(string);
    return text;
}

std::string takeUrl(WKURLRef url)
{
    if (!url)
        return {};
    std::string text = takeString(WKURLCopyString(url));
    WKRelease(url);
    return text;
}

void appendJson(std::string& json, WKTypeRef value)
{
    if (!value) {
        json += "null";
        return;
    }
    const WKTypeID type = WKGetTypeID(value);
    if (type == WKStringGetTypeID()) {
        appendString(json, toString(static_cast<WKStringRef>(value)));
    } else if (type == WKDoubleGetTypeID()) {
        const double number = WKDoubleGetValue(static_cast<WKDoubleRef>(value));
        if (std::isfinite(number)) {
            char text[32];
            snprintf(text, sizeof text, "%.17g", number);
            json += text;
        } else
            json += "null";
    } else if (type == WKUInt64GetTypeID()) {
        json += std::to_string(WKUInt64GetValue(static_cast<WKUInt64Ref>(value)));
    } else if (type == WKBooleanGetTypeID()) {
        json += WKBooleanGetValue(static_cast<WKBooleanRef>(value)) ? "true" : "false";
    } else if (type == WKArrayGetTypeID()) {
        const WKArrayRef array = static_cast<WKArrayRef>(value);
        json += '[';
        const size_t count = WKArrayGetSize(array);
        for (size_t i = 0; i < count; i++) {
            if (i)
                json += ',';
            appendJson(json, WKArrayGetItemAtIndex(array, i));
        }
        json += ']';
    } else if (type == WKDictionaryGetTypeID()) {
        const WKDictionaryRef dictionary = static_cast<WKDictionaryRef>(value);
        WKArrayRef keys = WKDictionaryCopyKeys(dictionary);
        json += '{';
        const size_t count = WKArrayGetSize(keys);
        for (size_t i = 0; i < count; i++) {
            const WKStringRef key = static_cast<WKStringRef>(WKArrayGetItemAtIndex(keys, i));
            if (i)
                json += ',';
            appendString(json, toString(key));
            json += ':';
            appendJson(json, WKDictionaryGetItemForKey(dictionary, key));
        }
        json += '}';
        WKRelease(keys);
    } else if (type == WKURLGetTypeID()) {
        appendString(json, takeString(WKURLCopyString(static_cast<WKURLRef>(value))));
    } else
        json += "null";
}

} // namespace cutefish::reef::web
