// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFXML: escaping and unescaping of the five predefined entities and character references in XML text.
#include "CFInternal.h"

CFStringRef CFXMLCreateStringByEscapingEntities(CFAllocatorRef allocator, CFStringRef string, CFDictionaryRef entitiesDictionary) {
    if (!string) return NULL;
    CFMutableStringRef result = CFStringCreateMutable(allocator, 0);
    CFIndex length = CFStringGetLength(string);
    for (CFIndex i = 0; i < length; i++) {
        UniChar c = CFStringGetCharacterAtIndex(string, i);
        switch (c) {
        case '"': CFStringAppend(result, CFSTR("&quot;")); break;
        case '&': CFStringAppend(result, CFSTR("&amp;")); break;
        case '\'': CFStringAppend(result, CFSTR("&apos;")); break;
        case '<': CFStringAppend(result, CFSTR("&lt;")); break;
        case '>': CFStringAppend(result, CFSTR("&gt;")); break;
        default: CFStringAppendCharacters(result, &c, 1);
        }
    }
    return result;
}

static const struct { const char *name; UniChar value; } entities[] = {
    { "amp;", '&' }, { "lt;", '<' }, { "gt;", '>' }, { "quot;", '"' }, { "apos;", '\'' },
};

// Whether the text at index is the entity name (up to and with the semicolon).
static Boolean hasEntity(CFStringRef string, CFIndex length, CFIndex index, const char *name) {
    for (CFIndex k = 0; name[k]; k++) {
        if (index + k >= length || CFStringGetCharacterAtIndex(string, index + k) != (UniChar)name[k]) return false;
    }
    return true;
}

CFStringRef CFXMLCreateStringByUnescapingEntities(CFAllocatorRef allocator, CFStringRef string, CFDictionaryRef entitiesDictionary) {
    if (!string) return NULL;
    CFMutableStringRef result = CFStringCreateMutable(allocator, 0);
    CFIndex length = CFStringGetLength(string);
    CFIndex i = 0;
    while (i < length) {
        UniChar c = CFStringGetCharacterAtIndex(string, i);
        if (c != '&') {
            CFStringAppendCharacters(result, &c, 1);
            i++;
            continue;
        }
        Boolean done = false;
        for (unsigned k = 0; k < sizeof(entities) / sizeof(entities[0]) && !done; k++) {
            if (hasEntity(string, length, i + 1, entities[k].name)) {
                UniChar value = entities[k].value;
                CFStringAppendCharacters(result, &value, 1);
                i += 1 + (CFIndex)strlen(entities[k].name);
                done = true;
            }
        }
        if (done) continue;
        if (i + 2 < length && CFStringGetCharacterAtIndex(string, i + 1) == '#') {
            CFIndex j = i + 2;
            Boolean hex = false;
            UniChar marker = CFStringGetCharacterAtIndex(string, j);
            if (marker == 'x' || marker == 'X') {
                hex = true;
                j++;
            }
            unsigned long code = 0;
            CFIndex digits = 0;
            for (; j < length; j++, digits++) {
                UniChar d = CFStringGetCharacterAtIndex(string, j);
                int value;
                if (d >= '0' && d <= '9') value = d - '0';
                else if (hex && d >= 'a' && d <= 'f') value = d - 'a' + 10;
                else if (hex && d >= 'A' && d <= 'F') value = d - 'A' + 10;
                else break;
                code = code * (hex ? 16 : 10) + value;
            }
            if (digits > 0 && j < length && CFStringGetCharacterAtIndex(string, j) == ';' && code < 0x110000) {
                UniChar units[2];
                CFIndex count = 1;
                if (code < 0x10000) {
                    units[0] = (UniChar)code;
                } else {
                    units[0] = (UniChar)(((code - 0x10000) >> 10) + 0xd800);
                    units[1] = (UniChar)((code & 0x3ff) | 0xdc00);
                    count = 2;
                }
                CFStringAppendCharacters(result, units, count);
                i = j + 1;
                continue;
            }
        } else if (entitiesDictionary) {
            CFIndex j = i + 1;
            while (j < length && CFStringGetCharacterAtIndex(string, j) != ';') j++;
            if (j < length) {
                CFStringRef name = CFStringCreateWithSubstring(allocator, string, CFRangeMake(i + 1, j - i - 1));
                CFStringRef value = (CFStringRef)CFDictionaryGetValue(entitiesDictionary, name);
                CFRelease(name);
                if (value) {
                    CFStringAppend(result, value);
                    i = j + 1;
                    continue;
                }
            }
        }
        CFStringAppendCharacters(result, &c, 1);
        i++;
    }
    return result;
}
