// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "bundle.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sys/stat.h>

namespace cutefish::reef {

// The position after `token`, which may come after white space; npos when it is not there.
static size_t after(const std::string& text, size_t at, const std::string& token)
{
    if (at >= text.size())
        return std::string::npos;
    while (text[at] == ' ' || text[at] == '\t' || text[at] == '\n' || text[at] == '\r') {
        if (++at >= text.size())
            return std::string::npos;
    }
    if (text.compare(at, token.size(), token) != 0)
        return std::string::npos;
    return at + token.size();
}

std::string withoutTrailingSlashes(std::string path)
{
    while (path.size() > 1 && path.back() == '/')
        path.pop_back();
    return path;
}

static uint64_t bigEndian(const unsigned char* bytes, size_t count)
{
    uint64_t value = 0;
    for (size_t i = 0; i < count; i++)
        value = value << 8 | bytes[i];
    return value;
}

struct BinaryPlist;
static std::string objectString(const BinaryPlist& plist, uint64_t index, bool& ok);

// A binary property list: the trailer, and where the objects are.
struct BinaryPlist
{
    explicit BinaryPlist(const std::string& text)
        : data(reinterpret_cast<const unsigned char*>(text.data()))
        , size(text.size())
    {
        if (size < 40)
            return;
        const unsigned char* trailer = data + size - 32;
        offsetSize = trailer[6];
        referenceSize = trailer[7];
        objectCount = bigEndian(trailer + 8, 8);
        top = bigEndian(trailer + 16, 8);
        offsetTable = bigEndian(trailer + 24, 8);
        if (offsetSize - 1 > 7 || referenceSize - 1 > 7 || top >= objectCount || offsetTable > size)
            return;
        valid = objectCount <= (size - offsetTable) / offsetSize;
    }

    uint64_t offset(uint64_t index) const
    {
        if (index >= objectCount)
            return size;
        return bigEndian(data + offsetTable + index * offsetSize, offsetSize);
    }

    // The count in the marker of the object, or in the integer after it; `at` moves to the contents.
    uint64_t count(uint8_t marker, uint64_t& at) const
    {
        uint64_t value = marker & 0xf;
        if (value != 0xf)
            return value;
        value = UINT64_MAX;
        if (at < offsetTable && (data[at] & 0xf0) == 0x10) {
            const uint64_t length = uint64_t(1) << (data[at] & 0xf);
            if (length <= 8 && at + 1 + length <= offsetTable) {
                value = bigEndian(data + at + 1, length);
                at += 1 + length;
            }
        }
        return value;
    }

    // The references of the keys and values of a dictionary object.
    std::vector<std::pair<uint64_t, uint64_t>> dictionary(uint64_t index) const
    {
        std::vector<std::pair<uint64_t, uint64_t>> entries;
        const uint64_t at0 = offset(index);
        if (at0 >= offsetTable || data[at0] >> 4 != 0xd)
            return entries;
        uint64_t at = at0 + 1;
        const uint64_t length = count(data[at0], at);
        if (length > (offsetTable - at) / (referenceSize * 2) || length == 0)
            return entries;
        const unsigned char* keys = data + at;
        const unsigned char* values = keys + referenceSize * length;
        for (uint64_t i = 0; i < length; i++)
            entries.emplace_back(bigEndian(keys + i * referenceSize, referenceSize), bigEndian(values + i * referenceSize, referenceSize));
        return entries;
    }

    // The value of a key of the top dictionary.
    uint64_t value(const std::string& key) const
    {
        uint64_t found = objectCount;
        if (!valid)
            return found;
        for (const auto& [name, object] : dictionary(top)) {
            bool ok;
            if (objectString(*this, name, ok) == key && ok) {
                found = object;
                break;
            }
        }
        return found;
    }

    const unsigned char* data;
    size_t size;
    uint64_t offsetSize = 0;
    uint64_t referenceSize = 0;
    uint64_t objectCount = 0;
    uint64_t top = 0;
    uint64_t offsetTable = 0;
    bool valid = false;
};

static void appendUtf8(std::string& text, uint32_t c)
{
    if (c < 0x80) {
        text += char(c);
    } else if (c < 0x800) {
        text += char(0xc0 | c >> 6);
        text += char(0x80 | (c & 0x3f));
    } else if (c < 0x10000) {
        text += char(0xe0 | c >> 12);
        text += char(0x80 | (c >> 6 & 0x3f));
        text += char(0x80 | (c & 0x3f));
    } else {
        text += char(0xf0 | c >> 18);
        text += char(0x80 | (c >> 12 & 0x3f));
        text += char(0x80 | (c >> 6 & 0x3f));
        text += char(0x80 | (c & 0x3f));
    }
}

// An ASCII or UTF-16 string object.
static std::string objectString(const BinaryPlist& plist, uint64_t index, bool& ok)
{
    ok = false;
    const uint64_t offset = plist.offset(index);
    if (offset >= plist.offsetTable)
        return {};
    const uint8_t marker = plist.data[offset];
    uint64_t at = offset + 1;
    const uint64_t count = plist.count(marker, at);
    std::string text;
    if (marker >> 4 == 5) {
        if (plist.offsetTable - at < count)
            return {};
        text.assign(reinterpret_cast<const char*>(plist.data + at), count);
    } else if (marker >> 4 == 6) {
        if ((plist.offsetTable - at) / 2 < count)
            return {};
        for (uint64_t i = 0; i < count; i++) {
            uint32_t c = uint32_t(plist.data[at + i * 2]) << 8 | plist.data[at + i * 2 + 1];
            if (c >= 0xd800 && c < 0xdc00 && i + 1 < count) {
                const uint32_t low = uint32_t(plist.data[at + i * 2 + 2]) << 8 | plist.data[at + i * 2 + 3];
                if (low >= 0xdc00 && low < 0xe000) {
                    c = 0x10000 + ((c - 0xd800) << 10) + (low - 0xdc00);
                    i++;
                }
            }
            appendUtf8(text, c);
        }
    } else {
        return {};
    }
    ok = true;
    return text;
}

static const struct
{
    const char* name;
    char value;
} entities[] = {{"&amp;", '&'}, {"&lt;", '<'}, {"&gt;", '>'}, {"&quot;", '"'}, {"&apos;", '\''}};

static std::string unescape(const std::string& text, size_t from, size_t to)
{
    std::string plain;
    while (from < to) {
        bool found = false;
        for (const auto& entity : entities) {
            const size_t length = strlen(entity.name);
            if (text.compare(from, length, entity.name) == 0) {
                plain += entity.value;
                from += length;
                found = true;
                break;
            }
        }
        if (!found)
            plain += text[from++];
    }
    return plain;
}

static std::string readInfo(const std::string& bundle)
{
    std::ifstream file(bundle + "/Contents/Info.plist", std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

std::vector<std::pair<std::string, std::string>> infoDictionary(const std::string& bundle, const std::string& key)
{
    const std::string text = readInfo(bundle);
    std::vector<std::pair<std::string, std::string>> pairs;
    if (text.compare(0, 8, "bplist00") == 0) {
        const BinaryPlist plist(text);
        if (!plist.valid)
            return pairs;
        for (const auto& [name, value] : plist.dictionary(plist.value(key))) {
            bool nameOk;
            bool valueOk;
            std::string first = objectString(plist, name, nameOk);
            std::string second = objectString(plist, value, valueOk);
            if (nameOk && valueOk)
                pairs.emplace_back(std::move(first), std::move(second));
        }
        return pairs;
    }
    const std::string needle = "<key>" + key + "</key>";
    const size_t found = text.find(needle);
    if (found == std::string::npos)
        return pairs;
    size_t at = after(text, found + needle.size(), "<dict>");
    if (at == std::string::npos || at >= text.size())
        return pairs;
    const size_t end = text.find("</dict>", at);
    if (end == std::string::npos)
        return pairs;
    for (;;) {
        const size_t name = after(text, at, "<key>");
        if (name >= end || name >= text.size())
            return pairs;
        const size_t nameEnd = text.find("</key>", name);
        if (nameEnd == std::string::npos || nameEnd > end)
            return pairs;
        const size_t value = after(text, nameEnd + 6, "<string>");
        if (value >= end || value >= text.size())
            return pairs;
        const size_t valueEnd = text.find("</string>", value);
        if (valueEnd == std::string::npos || valueEnd > end)
            return pairs;
        pairs.emplace_back(unescape(text, name, nameEnd), unescape(text, value, valueEnd));
        at = valueEnd + 9;
    }
}

std::string infoString(const std::string& bundle, const std::string& key)
{
    const std::string text = readInfo(bundle);
    if (text.compare(0, 8, "bplist00") == 0) {
        const BinaryPlist plist(text);
        bool ok;
        const std::string value = objectString(plist, plist.value(key), ok);
        if (!ok)
            return {};
        return value;
    }
    const std::string needle = "<key>" + key + "</key>";
    const size_t found = text.find(needle);
    if (found == std::string::npos)
        return {};
    const size_t at = after(text, found + needle.size(), "<string>");
    if (at == std::string::npos || at >= text.size())
        return {};
    const size_t end = text.find("</string>", at);
    if (end == std::string::npos)
        return {};
    return unescape(text, at, end);
}

std::string bundleExecutable(const std::string& path)
{
    struct stat status;
    if (stat(path.c_str(), &status) != 0 || !S_ISDIR(status.st_mode))
        return {};
    const std::string bundle = withoutTrailingSlashes(path);
    std::string name = infoString(bundle, "CFBundleExecutable");
    if (name.empty() || name.find('/') != std::string::npos) {
        // Without the key: the name of the bundle.
        name = bundle.substr(bundle.rfind('/') + 1);
        static const std::string suffix = ".app";
        if (name.size() > suffix.size() && name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
            name.resize(name.size() - suffix.size());
    }
    const std::string executable = bundle + "/Contents/MacOS/" + name;
    if (stat(executable.c_str(), &status) != 0 || !S_ISREG(status.st_mode))
        return {};
    return executable;
}

} // namespace cutefish::reef
