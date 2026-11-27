// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoreFoundation: the display names of locales and the list patterns, read from ICU's locale data with Reef's own names on top.
//
// The tables (___reefLocaleNamesData and its index, ListPatterns) are the original's data and are cut out of the original file by
// tools/macho-extract-data.py when the file is built (target.conf data.CFLocaleData).  A record is three strings and a value, all
// terminated by a zero byte: locale, table, key, name; the index holds the offsets of the records, sorted by the three strings.
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <_foundation_unicode/uenum.h>
#include <_foundation_unicode/uldnames.h>
#include <_foundation_unicode/uloc.h>
#include <_foundation_unicode/ures.h>
#include <_foundation_unicode/ustring.h>
#include <_foundation_unicode/utypes.h>
#include "CFInternal.h"

// A text of up to 512 UTF-16 units and its length.
typedef struct { UChar text[512]; int32_t length; } ReefText;
// "{0} (…{1}…)": the text before, between and after the two arguments of a pattern.
typedef struct { ReefText before, middle, after; } ReefPattern;
// The separator and the pattern of the display locale and the brackets: bracket[0] and bracket[1] inside the names become bracket[2] and bracket[3].
typedef struct { ReefPattern separator, pattern; UChar bracket[4]; } ReefPatterns;
// Names a locale has in the tables: the records, their index and the number of records.
typedef struct { const char *data; const uint32_t *index; unsigned long count; } ReefOverrides;

extern const char reefLocaleNamesData[] __asm__("___reefLocaleNamesData");
extern const uint32_t reefLocaleNamesIndex[] __asm__("___reefLocaleNamesIndex");
extern const char reefListPatternsData[] __asm__("___reefListPatternsData");
extern const uint32_t reefListPatternsIndex[] __asm__("___reefListPatternsIndex");

static const ReefOverrides reefNameOverrides __asm__("___reefNameOverrides") = {reefLocaleNamesData, reefLocaleNamesIndex, 0x18cd};
static const ReefOverrides reefListOverrides __asm__("___reefListOverrides") = {reefListPatternsData, reefListPatternsIndex, 0x5a};

static const UChar reefPatternParseZero[] __asm__("___reefPatternParse.zero") = {'{', '0', '}', 0};
static const UChar reefPatternParseOne[] __asm__("___reefPatternParse.one") = {'{', '1', '}', 0};
static const UChar reefDialectNameEquals[] __asm__("___reefDialectName.equals") = {'=', 0};
static const UChar reefPlainNameEquals[] __asm__("___reefPlainName.equals") = {'=', 0};

// Finds the text of `key` in the locale's `table` (inside `subtable` when there is one): first in the records of `overrides` and then in the ICU
// resource bundle of the package, going up through the parents of the locale.  Without `fallBack` the search stops before the root locale.
static bool __reefFind(const char *package, const ReefOverrides *overrides, const char *locale, const char *table, const char *subtable, const char *key,
                       bool fallBack, ReefText *out) {
    char name[0x9d];
    UErrorCode status = U_ZERO_ERROR;
    uloc_getBaseName(locale, name, sizeof name, &status);
    if (status > U_ZERO_ERROR || status == U_STRING_NOT_TERMINATED_WARNING || name[0] == '\0') strcpy(name, "root");
    const char *second = subtable ? subtable : table;
    for (int step = 0; step < 16; step++) {
        status = U_ZERO_ERROR;
        UResourceBundle *bundle = ures_openDirect(package, name, &status);
        if (status > U_ZERO_ERROR) bundle = NULL;
        else {
            const char *actual = ures_getLocaleByType(bundle, ULOC_ACTUAL_LOCALE, &status);
            if (actual && status <= U_ZERO_ERROR && *actual) strlcpy(name, actual, sizeof name);
        }
        if (!fallBack && strcmp(name, "root") == 0) {
            if (bundle) ures_close(bundle);
            return false;
        }
        bool found = false, listed = false;
        const char *value = NULL;
        if (overrides) {
            unsigned long low = 0, high = overrides->count;
            while (low < high) {
                unsigned long middle = (low + high) >> 1;
                const char *record = overrides->data + overrides->index[middle];
                const char *wanted[3] = {name, second, key};
                int order = 0;
                for (int i = 0; i < 3; i++) {
                    order = strcmp(record, wanted[i]);
                    record += strlen(record) + 1;
                    if (order) break;
                }
                if (order == 0) { listed = true; value = record; break; }
                if (order < 0) low = middle + 1; else high = middle;
            }
        }
        if (listed) {
            // an empty name in the tables says: this locale has none, look in its parent
            if (*value) {
                UErrorCode converted = U_ZERO_ERROR;
                int32_t length = 0;
                u_strFromUTF8(out->text, 0x200, &length, value, -1, &converted);
                out->length = converted > U_ZERO_ERROR ? 0 : (length > 0x200 ? 0x200 : length);
                found = true;
            }
        } else if (bundle) {
            UErrorCode icu = U_ZERO_ERROR;
            UResourceBundle *outer = ures_getByKey(bundle, table, NULL, &icu);
            UResourceBundle *inner = subtable ? ures_getByKey(outer, subtable, NULL, &icu) : NULL;
            int32_t length = 0;
            const UChar *string = ures_getStringByKey(inner ? inner : outer, key, &length, &icu);
            if (string && icu < U_ZERO_ERROR + 1) {
                found = true;
                int32_t n = length > 0x1ff ? 0x200 : length;
                u_memcpy(out->text, string, n);
                out->length = n;
            }
            if (inner) ures_close(inner);
            if (outer) ures_close(outer);
        }
        char parent[0x9d] = {0};
        if (bundle) {
            UErrorCode icu = U_ZERO_ERROR;
            int32_t length = 0;
            const UChar *string = ures_getStringByKey(bundle, "%%Parent", &length, &icu);
            if (length < 0x9d && icu < U_ZERO_ERROR + 1 && string) {
                u_UCharsToChars(string, parent, length);
                parent[length] = '\0';
            }
            ures_close(bundle);
        }
        if (found) return true;
        if (strcmp(name, "root") == 0) return false;
        if (parent[0] == '\0') {
            strlcpy(parent, name, sizeof parent);
            char *underscore = strrchr(parent, '_');
            if (underscore) *underscore = '\0'; else strcpy(parent, "root");
        }
        strlcpy(name, parent, sizeof name);
    }
    return false;
}

// The language a deprecated or alternate code stands for, from the "metadata" alias table.
static bool __reefCanonicalLanguage(const char *code, char *canonical) {
    UErrorCode status = U_ZERO_ERROR;
    UResourceBundle *metadata = ures_openDirect(NULL, "metadata", &status);
    UResourceBundle *alias = ures_getByKey(metadata, "alias", NULL, &status);
    UResourceBundle *languages = ures_getByKey(alias, "language", NULL, &status);
    UResourceBundle *entry = ures_getByKey(languages, code, NULL, &status);
    int32_t length = 0;
    const UChar *replacement = ures_getStringByKey(entry, "replacement", &length, &status);
    bool ok = length - 1U < 0x9c && replacement && status < U_ZERO_ERROR + 1;
    if (ok) {
        u_UCharsToChars(replacement, canonical, length);
        canonical[length] = '\0';
    }
    if (entry) ures_close(entry);
    if (languages) ures_close(languages);
    if (alias) ures_close(alias);
    if (metadata) ures_close(metadata);
    return ok;
}

// The short name of a region (Countries%short), the long one when there is no short one, the code itself when there is neither.
static void __reefRegionShortName(const char *displayLocale, const char *region, ReefText *out) {
    if (!(region[0] == 'P' && region[1] == 'S' && region[2] == '\0') &&
        __reefFind("ICUDATA-region", &reefNameOverrides, displayLocale, "Countries%short", NULL, region, true, out)) return;
    if (!__reefFind("ICUDATA-region", &reefNameOverrides, displayLocale, "Countries", NULL, region, true, out)) {
        size_t n = strlen(region);
        int32_t length = n < 0x200 ? (int32_t)n : 0x200;
        u_charsToUChars(region, out->text, length);
        out->length = length;
    }
}

// Splits "{0} … {1}" into the text before the first argument, between the two and after the second.
static void __reefPatternParse(const ReefText *source, ReefPattern *out) {
    UChar copy[0x208];
    u_memcpy(copy, source->text, source->length);
    copy[source->length] = 0;
    UChar *zero = u_strstr(copy, reefPatternParseZero);
    UChar *one = u_strstr(copy, reefPatternParseOne);
    out->after.length = 0;
    out->middle.length = 0;
    out->before.length = 0;
    if (!zero || !one || one < zero) {
        int32_t n = source->length < 0x200 ? source->length : 0x200;
        u_memcpy(out->middle.text, source->text, n);
        out->middle.length = n;
        return;
    }
    int32_t n = (int32_t)(zero - copy);
    if (n > 0x1ff) n = 0x200;
    u_memcpy(out->before.text, copy, n);
    out->before.length = n;
    n = (int32_t)(one - zero) - 3;
    if (n > 0x1ff) n = 0x200;
    u_memcpy(out->middle.text, zero + 3, n);
    out->middle.length = n;
    n = source->length - (int32_t)(one + 3 - copy);
    if (n > 0x1ff) n = 0x200;
    u_memcpy(out->after.text, one + 3, n);
    out->after.length = n;
}

// before a middle b after, each part cut to what fits in 512 units.
static void __reefPatternApply(const ReefPattern *pattern, const ReefText *a, const ReefText *b, ReefText *out) {
    ReefText result;
    result.length = 0;
    const struct { const UChar *text; int32_t length; } parts[5] = {
        {pattern->before.text, pattern->before.length}, {a->text, a->length}, {pattern->middle.text, pattern->middle.length},
        {b->text, b->length}, {pattern->after.text, pattern->after.length}};
    for (int i = 0; i < 5; i++) {
        int32_t room = 0x200 - result.length;
        int32_t n = parts[i].length <= room ? parts[i].length : room;
        if (i == 0 && n > 0x1ff) n = 0x200;
        u_memcpy(result.text + result.length, parts[i].text, n);
        result.length += n;
    }
    memcpy(out, &result, sizeof result);
}

// The separator ("{0}, {1}") and the pattern ("{0} ({1})") of the display locale, and the brackets for names inside names.
static void __reefDisplayPatterns(const char *displayLocale, ReefPatterns *patterns) {
    ReefText text;
    if (!__reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "localeDisplayPattern", NULL, "separator", true, &text) || text.length == 0) {
        u_charsToUChars("{0}, {1}", text.text, 8);
        text.length = 8;
    }
    __reefPatternParse(&text, &patterns->separator);
    if (!__reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "localeDisplayPattern", NULL, "pattern", true, &text) || text.length == 0) {
        u_charsToUChars("{0} ({1})", text.text, 9);
        text.length = 9;
    }
    __reefPatternParse(&text, &patterns->pattern);
    // names inside names get square brackets, full width ones when the pattern's own brackets are full width
    static const UChar wide[4] = {0xff08, 0xff09, 0xff3b, 0xff3d}, narrow[4] = {'(', ')', '[', ']'};
    memcpy(patterns->bracket, u_memchr(text.text, 0xff08, text.length) ? wide : narrow, sizeof patterns->bracket);
}

static void __reefBrackets(const ReefPatterns *patterns, ReefText *text) {
    for (int32_t i = 0; i < text->length; i++) {
        if (text->text[i] == patterns->bracket[0]) text->text[i] = patterns->bracket[2];
        else if (text->text[i] == patterns->bracket[1]) text->text[i] = patterns->bracket[3];
    }
}

// Part of a locale identifier, with the length capped to 0x9d; the empty language of "und..." and "root" are named.
// kind: 0 language, 1 script alone, 2 script, 3 region, 4 variant.
static bool __reefComponent(unsigned kind, const char *locale, const char *displayLocale, ReefText *out, bool *defaulted) {
    char code[0x9d + 3];
    UErrorCode status = U_ZERO_ERROR;
    int32_t length;
    if (kind > 4) {
        out->length = 0;
        return false;
    }
    switch (kind) {
    case 0:
        length = uloc_getLanguage(locale, code, 0x9d, &status);
        if (status == U_STRING_NOT_TERMINATED_WARNING || status > U_ZERO_ERROR) length = 0;
        code[length] = '\0';
        if (length == 0) {
            if (strncasecmp(locale, "und", 3) == 0 && memchr("-_@", locale[3], 4)) {
                memcpy(code, "und", 4);
                length = 3;
            } else if (strcasecmp(locale, "root") == 0) {
                memcpy(code, "root", 5);
                length = 4;
            }
        }
        break;
    case 3:
        length = uloc_getCountry(locale, code, 0x9d, &status);
        goto sanitised;
    case 4:
        length = uloc_getVariant(locale, code, 0x9d, &status);
        goto sanitised;
    default:
        length = uloc_getScript(locale, code, 0x9d, &status);
    sanitised:
        if (status == U_STRING_NOT_TERMINATED_WARNING || status > U_ZERO_ERROR) length = 0;
        code[length] = '\0';
    }
    out->length = 0;
    if (length == 0) return false;
    bool found;
    switch (kind) {
    case 0:
        if (strtol(code, NULL, 10) != 0) { found = false; break; }
        found = __reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Languages", NULL, code, true, out);
        if (!found) {
            char canonical[0x9d + 3];
            if (__reefCanonicalLanguage(code, canonical))
                found = __reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Languages", NULL, canonical, true, out);
        }
        break;
    case 4:
        found = __reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Variants", NULL, code, true, out);
        break;
    case 2:
        found = __reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Scripts", NULL, code, true, out);
        break;
    case 3:
        found = __reefFind("ICUDATA-region", &reefNameOverrides, displayLocale, "Countries", NULL, code, true, out);
        break;
    default:
        found = __reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Scripts%stand-alone", NULL, code, true, out) ||
                __reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Scripts", NULL, code, true, out);
    }
    if (!found) {
        // no name: the code itself, and the caller is told the result is a default
        size_t n = strlen(code);
        int32_t copied = n < 0x200 ? (int32_t)n : 0x200;
        u_charsToUChars(code, out->text, copied);
        out->length = copied;
        *defaulted = true;
    }
    return true;
}

// The ICU way to hand a text back: the length is returned, the status tells when it was cut or not terminated.
static int32_t __reefReturn(const ReefText *text, UChar *result, int32_t capacity, UErrorCode *status) {
    int32_t n = text->length < capacity ? text->length : capacity;
    u_memcpy(result, text->text, n);
    if (text->length < capacity) result[text->length] = 0;
    else if (text->length == capacity) *status = U_STRING_NOT_TERMINATED_WARNING;
    else *status = U_BUFFER_OVERFLOW_ERROR;
    return text->length;
}

static int32_t __reefLocaleDisplayComponent(unsigned kind, const char *locale, const char *displayLocale, UChar *result, int32_t capacity, UErrorCode *status) {
    if (*status > U_ZERO_ERROR) return 0;
    ReefText text;
    bool defaulted = false;
    __reefComponent(kind, locale, displayLocale, &text, &defaulted);
    if (defaulted && *status < U_ZERO_ERROR + 1) *status = U_USING_DEFAULT_WARNING;
    return __reefReturn(&text, result, capacity, status);
}

int32_t __reefLocaleDisplayLanguage(const char *locale, const char *displayLocale, UChar *result, int32_t capacity, UErrorCode *status) {
    return __reefLocaleDisplayComponent(0, locale, displayLocale, result, capacity, status);
}
int32_t __reefLocaleDisplayScript(const char *locale, const char *displayLocale, UChar *result, int32_t capacity, UErrorCode *status) {
    return __reefLocaleDisplayComponent(1, locale, displayLocale, result, capacity, status);
}
int32_t __reefLocaleDisplayCountry(const char *locale, const char *displayLocale, UChar *result, int32_t capacity, UErrorCode *status) {
    return __reefLocaleDisplayComponent(3, locale, displayLocale, result, capacity, status);
}
int32_t __reefLocaleDisplayVariant(const char *locale, const char *displayLocale, UChar *result, int32_t capacity, UErrorCode *status) {
    return __reefLocaleDisplayComponent(4, locale, displayLocale, result, capacity, status);
}

// Puts `add` after `text`: the first one alone, the others joined by the separator pattern.
static void __reefJoin(const ReefPatterns *patterns, ReefText *text, const ReefText *add) {
    if (text->length == 0) memcpy(text, add, sizeof *text);
    else __reefPatternApply(&patterns->separator, text, add, text);
}

static void __reefPart(const char *code, ReefText *out) {
    size_t n = strlen(code);
    int32_t length = n < 0x200 ? (int32_t)n : 0x200;
    u_charsToUChars(code, out->text, length);
    out->length = length;
}

static bool __reefSameAscii(const ReefText *text, const char *string) {
    size_t n = strlen(string);
    if ((size_t)text->length != n) return false;
    for (size_t i = 0; i < n; i++) if (text->text[i] != (unsigned char)string[i]) return false;
    return true;
}

static char *__reefCodePart(int32_t (*get)(const char *, char *, int32_t, UErrorCode *), const char *locale, char *buffer, int32_t *length) {
    UErrorCode status = U_ZERO_ERROR;
    int32_t n = get(locale, buffer, 0x9d, &status);
    if (status == U_STRING_NOT_TERMINATED_WARNING || status > U_ZERO_ERROR) n = 0;
    buffer[n] = '\0';
    *length = n;
    return buffer;
}

static bool __reefIsOneOf(const char *language, const char *const *list, int count) {
    for (int i = 0; i < count; i++) if (strcmp(language, list[i]) == 0) return true;
    return false;
}

// The whole name of a locale: language, then script, region, variant and keywords in the brackets of the display locale's pattern ("English (Canada)").
// Chinese and a few other languages have names of their own for their script and region combinations.
int32_t __reefLocaleDisplayName(const char *locale, const char *displayLocale, UChar *result, int32_t capacity, UErrorCode *status) {
    if (*status > U_ZERO_ERROR) return 0;
    ReefText name;
    bool defaulted = false;
    char languageCode[0x9d + 3], scriptCode[0x9d + 3], regionCode[0x9d + 3], variantCode[0x9d + 3];
    int32_t languageLength, scriptLength, regionLength, variantLength;
    __reefCodePart(uloc_getLanguage, locale, languageCode, &languageLength);
    if (languageLength == 0) {
        if (strncasecmp(locale, "und", 3) == 0 && memchr("-_@", locale[3], 4)) strcpy(languageCode, "und");
        else if (strcasecmp(locale, "root") == 0) strcpy(languageCode, "root");
    }
    __reefCodePart(uloc_getScript, locale, scriptCode, &scriptLength);
    __reefCodePart(uloc_getCountry, locale, regionCode, &regionLength);
    // a script and one of these languages, or one of these regions: the names of the combination
    static const char *const scriptLanguages[] = {"zh", "yue", "ks", "pa", "ur"};
    bool combined = false;
    if (scriptLength > 0 && __reefIsOneOf(languageCode, scriptLanguages, 5)) combined = true;
    else if (regionLength > 0 && regionCode[2] == '\0' &&
             (!strcmp(regionCode, "CN") || !strcmp(regionCode, "HK") || !strcmp(regionCode, "MO") || !strcmp(regionCode, "TW"))) combined = true;

    ReefPatterns patterns;
    ReefText details;
    ReefText piece;
    if (!combined) {
        __reefDisplayPatterns(displayLocale, &patterns);
        ReefText language;
        __reefComponent(0, locale, displayLocale, &language, &defaulted);
        details.length = 0;
        // script, region and variant
        for (unsigned kind = 2; kind <= 4; kind++) {
            if (__reefComponent(kind, locale, displayLocale, &piece, &defaulted) && piece.length > 0) {
                __reefBrackets(&patterns, &piece);
                __reefJoin(&patterns, &details, &piece);
            }
        }
        // keywords: calendar=gregorian and the like, named by ICU
        UErrorCode icu = U_ZERO_ERROR;
        UEnumeration *keywords = uloc_openKeywords(locale, &icu);
        if (keywords) {
            while (icu < U_ZERO_ERROR + 1) {
                const char *keyword = uenum_next(keywords, NULL, &icu);
                if (!keyword) break;
                UErrorCode keyStatus = U_ZERO_ERROR, valueStatus = U_ZERO_ERROR;
                int32_t keyLength = uloc_getDisplayKeyword(keyword, displayLocale, piece.text, 0x200, &keyStatus);
                if (keyStatus > U_ZERO_ERROR) keyLength = 0;
                ReefText value;
                int32_t valueLength = uloc_getDisplayKeywordValue(locale, keyword, displayLocale, value.text, 0x200, &valueStatus);
                if (valueStatus > U_ZERO_ERROR) valueLength = 0;
                if (keyStatus == U_USING_DEFAULT_WARNING || valueStatus == U_USING_DEFAULT_WARNING) defaulted = true;
                if (keyLength > 0 && valueLength > 0) {
                    int32_t n = 0x200 - keyLength > 0 ? 1 : 0x200 - keyLength;
                    u_memcpy(piece.text + keyLength, reefPlainNameEquals, n);
                    keyLength += n;
                }
                int32_t room = 0x200 - keyLength;
                int32_t n = valueLength <= room ? valueLength : room;
                u_memcpy(piece.text + keyLength, value.text, n);
                keyLength += n;
                piece.length = keyLength;
                if (keyLength > 0) {
                    __reefBrackets(&patterns, &piece);
                    __reefJoin(&patterns, &details, &piece);
                }
            }
            uenum_close(keywords);
        }
        if (language.length < 1 || details.length < 1) memcpy(&piece, language.length < 1 ? &details : &language, sizeof piece);
        else __reefPatternApply(&patterns.pattern, &language, &details, &piece);
        if (defaulted && *status < U_ZERO_ERROR + 1) *status = U_USING_DEFAULT_WARNING;
        return __reefReturn(&piece, result, capacity, status);
    }

    // The combined names.
    __reefCodePart(uloc_getVariant, locale, variantCode, &variantLength);
    bool needScript = scriptLength > 0, needRegion = regionLength > 0;
    if (languageCode[0] == '\0') {
        if (scriptLength < 1 && regionLength > 0 && variantLength < 1) {
            __reefRegionShortName(displayLocale, regionCode, &piece);
            return __reefReturn(&piece, result, capacity, status);
        }
        strcpy(languageCode, "root");
    }
    static const char *const named[] = {"apw", "cho", "cic", "ur", "pa", "ks", "yue", "zh"};
    ReefText language;
    language.length = 0;
    char key[0x1d7];
    bool done = false;
    if (__reefIsOneOf(languageCode, named, 8)) {
        if (scriptLength > 0 && regionLength > 0) {
            snprintf(key, sizeof key, "%s_%s_%s", languageCode, scriptCode, regionCode);
            if (__reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Languages", NULL, key, true, &language)) {
                needScript = needRegion = false;
                done = true;
            }
        }
        if (!done && scriptLength > 0) {
            snprintf(key, sizeof key, "%s_%s", languageCode, scriptCode);
            if (__reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Languages", NULL, key, true, &language)) {
                needScript = false;
                done = true;
            }
        }
        if (!done && regionLength > 0) {
            snprintf(key, sizeof key, "%s_%s", languageCode, regionCode);
            if (__reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Languages", NULL, key, true, &language)) {
                needRegion = false;
                done = true;
            }
        }
        if (done && language.length == 0) {
            // found, but empty: the name of the language alone
            needScript = scriptLength > 0;
            needRegion = regionLength > 0;
            done = false;
        }
    }
    if (!done) {
        bool found = __reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Languages", NULL, languageCode, true, &language);
        if (!found) {
            char canonical[0x9d + 3];
            if (!strchr(languageCode, '_') && __reefCanonicalLanguage(languageCode, canonical))
                found = __reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Languages", NULL, canonical, true, &language);
            if (!found) __reefPart(languageCode, &language);
        }
    }
    __reefDisplayPatterns(displayLocale, &patterns);
    details.length = 0;
    if (needScript) {
        if (!__reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Scripts", NULL, scriptCode, true, &piece)) __reefPart(scriptCode, &piece);
        __reefJoin(&patterns, &details, &piece);
    }
    if (needRegion) {
        __reefRegionShortName(displayLocale, regionCode, &piece);
        __reefJoin(&patterns, &details, &piece);
    }
    if (variantLength > 0) {
        if (!__reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "Variants", NULL, variantCode, true, &piece)) __reefPart(variantCode, &piece);
        __reefJoin(&patterns, &details, &piece);
    }
    if (details.length > 0) __reefBrackets(&patterns, &details);
    UErrorCode icu = U_ZERO_ERROR;
    UEnumeration *keywords = uloc_openKeywords(locale, &icu);
    if (keywords) {
        ULocaleDisplayNames *names = uldn_open(displayLocale, ULDN_STANDARD_NAMES, &icu);
        if (names && icu < U_ZERO_ERROR + 1) {
            while (icu < U_ZERO_ERROR + 1) {
                const char *keyword = uenum_next(keywords, NULL, &icu);
                if (!keyword) break;
                UErrorCode status2 = U_ZERO_ERROR;
                char value[100];
                int32_t n = uloc_getKeywordValue(locale, keyword, value, 99, &status2);
                if (status2 > U_ZERO_ERROR) break;
                value[n] = '\0';
                ReefText keyName, valueName;
                status2 = U_ZERO_ERROR;
                keyName.length = uldn_keyDisplayName(names, keyword, keyName.text, 0x200, &status2);
                if (status2 > U_ZERO_ERROR) keyName.length = 0;
                status2 = U_ZERO_ERROR;
                valueName.length = uldn_keyValueDisplayName(names, keyword, value, valueName.text, 0x200, &status2);
                if (status2 > U_ZERO_ERROR) valueName.length = 0;
                if (keyName.length > 0) __reefBrackets(&patterns, &keyName);
                if (valueName.length > 0) __reefBrackets(&patterns, &valueName);
                if (!__reefSameAscii(&valueName, value)) {
                    // the value has a name: that is what is shown
                    __reefJoin(&patterns, &details, &valueName);
                } else if (__reefSameAscii(&keyName, keyword)) {
                    // neither has: key=value
                    piece = keyName;
                    int32_t equals = 0x200 - piece.length > 0 ? 1 : 0x200 - piece.length;
                    u_memcpy(piece.text + piece.length, reefDialectNameEquals, equals);
                    piece.length += equals;
                    int32_t room = 0x200 - piece.length;
                    int32_t m = valueName.length <= room ? valueName.length : room;
                    u_memcpy(piece.text + piece.length, valueName.text, m);
                    piece.length += m;
                    __reefJoin(&patterns, &details, &piece);
                } else {
                    // a key with a name and a value without: the key and type pattern of the display locale
                    ReefText text;
                    ReefPattern keyType;
                    if (!__reefFind("ICUDATA-lang", &reefNameOverrides, displayLocale, "localeDisplayPattern", NULL, "keyTypePattern", true, &text)) {
                        u_charsToUChars("{0}={1}", text.text, 7);
                        text.length = 7;
                    }
                    __reefPatternParse(&text, &keyType);
                    __reefPatternApply(&keyType, &keyName, &valueName, &piece);
                    __reefJoin(&patterns, &details, &piece);
                }
            }
        }
        if (names) uldn_close(names);
        uenum_close(keywords);
    }
    if (details.length == 0) {
        memcpy(&piece, &language, sizeof piece);
    } else {
        __reefPatternApply(&patterns.pattern, &language, &details, &piece);
        // "A (B) (C)" is "A (B, C)": the closing bracket and the opening one are taken out and the parts joined by the separator
        UChar probe[3];
        int32_t probeLength;
        if (patterns.bracket[0] == 0xff08) {
            probe[0] = 0xff09; probe[1] = 0xff08;
            probeLength = 2;
        } else {
            probe[0] = ')'; probe[1] = ' '; probe[2] = '(';
            probeLength = 3;
        }
        for (int32_t at = 0; probeLength + at <= piece.length; at++) {
            if (u_memcmp(piece.text + at, probe, probeLength) == 0) {
                ReefText head, tail;
                head.length = at > 0x1ff ? 0x200 : at;
                u_memcpy(head.text, piece.text, head.length);
                int32_t rest = piece.length - (probeLength + at);
                tail.length = rest > 0x1ff ? 0x200 : rest;
                u_memcpy(tail.text, piece.text + at + probeLength, tail.length);
                if (head.length == 0) memcpy(&head, &tail, sizeof head);
                else __reefPatternApply(&patterns.separator, &head, &tail, &head);
                memcpy(&piece, &head, sizeof piece);
                break;
            }
        }
    }
    return __reefReturn(&piece, result, capacity, status);
}

// The pattern of a list ("{0}, {1}") for `key` (2, start, middle, end) of the list type, with the names of the tables when `useTables`.
// The narrow and short types fall back to the short ones and to the standard one before the parents of the locale are asked.
static bool __reefListPattern(const char *locale, const char *type, const char *key, bool useTables, ReefText *out) {
    const ReefOverrides *tables = useTables ? &reefListOverrides : NULL;
    for (;;) {
        const char *next;
        bool last = false;
        if (strcmp(type, "standard-narrow") == 0) next = "standard-short";
        else if (strcmp(type, "standard-short") == 0) next = "standard";
        else if (strcmp(type, "or-narrow") == 0) next = "or-short";
        else if (strcmp(type, "or-short") == 0) next = "or";
        else { next = NULL; last = true; }
        bool found = __reefFind(NULL, tables, locale, "listPattern", type, key, last, out);
        if (last || found) return found;
        type = next;
    }
}

// The patterns of the list type when the tables change one of them from what ICU has: false when they do not (ICU's own are right).
bool _CFLocaleCopyListPatterns(const char *locale, const char *type, CFStringRef patterns[4]) {
    static const char *const keys[4] = {"2", "start", "middle", "end"};
    ReefText with[4], without;
    bool differs = false;
    for (int i = 0; i < 4; i++) {
        if (!__reefListPattern(locale, type, keys[i], true, &with[i])) return false;
        if (!__reefListPattern(locale, type, keys[i], false, &without)) return false;
        if (with[i].length != without.length || u_memcmp(without.text, with[i].text, with[i].length) != 0) differs = true;
    }
    if (!differs) return false;
    for (int i = 0; i < 4; i++) patterns[i] = CFStringCreateWithCharacters(kCFAllocatorSystemDefault, with[i].text, with[i].length);
    return true;
}
