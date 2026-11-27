// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFStringTokenizer: tokens found by an ICU break iterator over a copy of the string, with the token kind (numeric, non-letters, CJK) derived from the characters.
#include "CFInternal.h"
#include "CFRuntime_Internal.h"
#include <pthread.h>
#include <stdlib.h>
#include <_foundation_unicode/uchar.h>
#include <_foundation_unicode/ubrk.h>

typedef struct __CFStringTokenizer *CFStringTokenizerRef;

enum {
    kTokenNormal = 1,
    kTokenHasNumbersMask = 1 << 3,
    kTokenHasNonLettersMask = 1 << 4,
    kTokenIsCJWordMask = 1 << 5,
};

struct __CFStringTokenizer {
    CFRuntimeBase _base;
    CFOptionFlags _options;
    char _locale[64];
    UChar *_buffer;
    CFIndex _rangeLocation;
    CFIndex _rangeLength;
    UBreakIterator *_breaker;
    CFIndex _tokenLocation;
    CFIndex _tokenLength;
    CFOptionFlags _tokenType;
    int _position;
};

static CFTypeID tokenizerTypeID;
static pthread_once_t tokenizerOnce = PTHREAD_ONCE_INIT;

static void finalizeTokenizer(CFTypeRef cf) {
    struct __CFStringTokenizer *t = (struct __CFStringTokenizer *)cf;
    if (t->_breaker) ubrk_close(t->_breaker);
    free(t->_buffer);
}

static const CFRuntimeClass tokenizerClass = {
    0, "CFStringTokenizer", NULL, NULL, finalizeTokenizer, NULL, NULL, NULL, NULL,
};

static void registerTokenizer(void) {
    tokenizerTypeID = _CFRuntimeRegisterClass(&tokenizerClass);
}

CFTypeID CFStringTokenizerGetTypeID(void) {
    pthread_once(&tokenizerOnce, registerTokenizer);
    return tokenizerTypeID;
}

void CFStringTokenizerSetString(CFStringTokenizerRef tokenizer, CFStringRef string, CFRange range) {
    struct __CFStringTokenizer *t = (struct __CFStringTokenizer *)tokenizer;
    if (!t) return;
    if (t->_breaker) {
        ubrk_close(t->_breaker);
        t->_breaker = NULL;
    }
    free(t->_buffer);
    t->_buffer = NULL;
    t->_rangeLocation = 0;
    t->_rangeLength = 0;
    t->_tokenLocation = -1;
    t->_tokenLength = 0;
    t->_tokenType = 0;
    t->_position = 0;
    if (!string) return;
    CFIndex length = CFStringGetLength(string);
    if (range.location < 0 || range.location > length) range = CFRangeMake(0, 0);
    CFIndex count = range.length;
    if (range.location + range.length > length) count = length - range.location;
    if (range.length < 0) count = length - range.location;
    t->_rangeLocation = range.location;
    t->_rangeLength = count;
    t->_buffer = (UChar *)malloc((count + (count == 0)) * sizeof(UChar));
    CFStringGetCharacters(string, CFRangeMake(range.location, count), t->_buffer);
    unsigned char unit = (unsigned char)(t->_options & 0xff) - 1;
    static const UBreakIteratorType types[3] = { UBRK_SENTENCE, UBRK_SENTENCE, UBRK_LINE };
    UBreakIteratorType type = unit < 3 ? types[unit] : UBRK_WORD;
    UErrorCode status = U_ZERO_ERROR;
    UBreakIterator *breaker = ubrk_open(type, t->_locale, t->_buffer, (int32_t)count, &status);
    t->_breaker = status <= U_ZERO_ERROR ? breaker : NULL;
}

CFStringTokenizerRef CFStringTokenizerCreate(CFAllocatorRef alloc, CFStringRef string, CFRange range, CFOptionFlags options, CFLocaleRef locale) {
    pthread_once(&tokenizerOnce, registerTokenizer);
    struct __CFStringTokenizer *t = (struct __CFStringTokenizer *)_CFRuntimeCreateInstance(alloc, tokenizerTypeID, sizeof(struct __CFStringTokenizer) - sizeof(CFRuntimeBase), NULL);
    if (t) {
        t->_options = options;
        t->_locale[0] = 0;
        if (locale) {
            CFStringRef identifier = CFLocaleGetIdentifier(locale);
            if (identifier) CFStringGetCString(identifier, t->_locale, sizeof(t->_locale), kCFStringEncodingUTF8);
        }
        CFStringTokenizerSetString((CFStringTokenizerRef)t, string, range);
    }
    return (CFStringTokenizerRef)t;
}

// The kind of the token at [start, end) of the buffer.
static CFOptionFlags tokenType(struct __CFStringTokenizer *t, int start, int end) {
    CFOptionFlags type = kTokenNormal;
    for (int i = start; i < end; i++) {
        UChar c = t->_buffer[i];
        if (u_isdigit(c)) type |= kTokenHasNumbersMask;
        else if (!u_isalpha(c)) type |= kTokenHasNonLettersMask;
        if ((unsigned short)(c + 0xb200) < 0x5200) type = (type & ~0u) | kTokenIsCJWordMask;
        if ((unsigned short)(c + 0xcfc0) < 0xc0) type = (type & ~0u) | kTokenIsCJWordMask;
    }
    return type;
}

static Boolean hasAlnum(struct __CFStringTokenizer *t, int start, int end) {
    for (int i = start; i < end; i++) {
        if (u_isalnum(t->_buffer[i])) return true;
    }
    return false;
}

static void setToken(struct __CFStringTokenizer *t, int start, int end) {
    t->_tokenLocation = t->_rangeLocation + start;
    t->_tokenLength = end - start;
    t->_tokenType = tokenType(t, start, end);
}

CFOptionFlags CFStringTokenizerGoToTokenAtIndex(CFStringTokenizerRef tokenizer, CFIndex index) {
    struct __CFStringTokenizer *t = (struct __CFStringTokenizer *)tokenizer;
    if (!t || !t->_breaker) return 0;
    CFIndex offset = index - t->_rangeLocation;
    t->_tokenLocation = -1;
    t->_tokenLength = 0;
    t->_tokenType = 0;
    if (offset < 0 || offset >= t->_rangeLength) {
        t->_position = (int)t->_rangeLength;
        return 0;
    }
    if (!ubrk_isBoundary(t->_breaker, (int32_t)offset)) offset = ubrk_preceding(t->_breaker, (int32_t)offset);
    int start = offset == -1 ? 0 : (int)offset;
    int end = ubrk_following(t->_breaker, start);
    if (end == -1) {
        t->_position = (int)t->_rangeLength;
        return 0;
    }
    t->_position = end;
    if ((t->_options & 0xff) == 0 && ubrk_getRuleStatus(t->_breaker) == 0) {
        if (end <= start || !hasAlnum(t, start, end)) return 0;
    }
    setToken(t, start, end);
    return t->_tokenType;
}

CFOptionFlags CFStringTokenizerAdvanceToNextToken(CFStringTokenizerRef tokenizer) {
    struct __CFStringTokenizer *t = (struct __CFStringTokenizer *)tokenizer;
    if (!t || !t->_breaker) return 0;
    int start = t->_position;
    while (start != -1 && start < t->_rangeLength) {
        int end = ubrk_following(t->_breaker, start);
        if (end == -1) break;
        if ((t->_options & 0xff) == 0 && ubrk_getRuleStatus(t->_breaker) == 0 && !hasAlnum(t, start, end)) {
            start = end;
            if (t->_rangeLength <= end) break;
            continue;
        }
        setToken(t, start, end);
        t->_position = end;
        return t->_tokenType;
    }
    t->_tokenLocation = -1;
    t->_tokenLength = 0;
    t->_tokenType = 0;
    t->_position = (int)t->_rangeLength;
    return 0;
}

CFRange CFStringTokenizerGetCurrentTokenRange(CFStringTokenizerRef tokenizer) {
    struct __CFStringTokenizer *t = (struct __CFStringTokenizer *)tokenizer;
    if (!t || t->_tokenLocation == -1) return CFRangeMake(kCFNotFound, 0);
    return CFRangeMake(t->_tokenLocation, t->_tokenLength);
}

CFTypeRef CFStringTokenizerCopyCurrentTokenAttribute(CFStringTokenizerRef tokenizer, CFOptionFlags attribute) {
    struct __CFStringTokenizer *t = (struct __CFStringTokenizer *)tokenizer;
    if (!t || attribute != 0x20000 || t->_tokenLocation == -1 || !t->_locale[0]) return NULL;
    return CFStringCreateWithCString(NULL, t->_locale, kCFStringEncodingUTF8);
}

CFIndex CFStringTokenizerGetCurrentSubTokens(CFStringTokenizerRef tokenizer, CFRange *ranges, CFIndex maxRangeLength, CFMutableArrayRef derivedSubTokens) {
    return 0;
}

CFStringRef CFStringTokenizerCopyBestStringLanguage(CFStringRef string, CFRange range) {
    if (!string) return NULL;
    CFIndex length = CFStringGetLength(string);
    CFIndex end = range.location + range.length;
    if (range.location < 0 || range.length <= 0 || end > length) return NULL;
    for (CFIndex i = range.location; i < end; i++) {
        UniChar c = CFStringGetCharacterAtIndex(string, i);
        if (!u_isalpha(c)) continue;
        if ((unsigned short)(c + 0xcfc0) < 0xc0) return CFSTR("ja");
        if ((unsigned short)(c + 0xb200) < 0x5200) return CFSTR("zh-Hans");
        if ((unsigned short)(c + 0x5400) < 0x2bb0) return CFSTR("ko");
        if ((c & 0xff00) == 0x400) return CFSTR("ru");
        if ((unsigned short)(c - 0x590) < 0x70) return CFSTR("he");
        if ((c & 0xff00) == 0x600) return CFSTR("ar");
        if ((c & 0xff80) == 0xe00) return CFSTR("th");
        if ((unsigned short)(c - 0x370) < 0x90) return CFSTR("el");
        if (c < 0x250) return CFSTR("en");
        return NULL;
    }
    return NULL;
}
