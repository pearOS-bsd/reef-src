// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "web.h"

#include <ctime>

namespace cutefish::reef::web {

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
    std::string text(WKStringGetMaximumUTF8CStringSize(string), '\0');
    const size_t size = WKStringGetUTF8CString(string, text.data(), text.size());
    text.resize(size > 0 ? size - 1 : 0);
    return text;
}

std::string takeUrl(WKURLRef url)
{
    if (!url)
        return {};
    WKStringRef string = WKURLCopyString(url);
    std::string text = toString(string);
    if (string)
        WKRelease(string);
    WKRelease(url);
    return text;
}

std::string takeString(WKStringRef string)
{
    if (!string)
        return {};
    std::string text = toString(string);
    WKRelease(string);
    return text;
}

} // namespace cutefish::reef::web
