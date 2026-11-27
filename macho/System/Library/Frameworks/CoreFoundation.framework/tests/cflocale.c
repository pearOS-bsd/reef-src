// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Display names of locales, languages, scripts, countries and variants in Reef's CoreFoundation (the locale name tables).
// The same output with the original and with the rebuilt file.
#include <stdio.h>
#include <string.h>

typedef const void *CFStringRef, *CFAllocatorRef, *CFLocaleRef;
extern CFAllocatorRef kCFAllocatorDefault;
extern CFStringRef kCFLocaleIdentifier, kCFLocaleLanguageCode, kCFLocaleCountryCode, kCFLocaleScriptCode, kCFLocaleVariantCode;
extern CFStringRef CFStringCreateWithCString(CFAllocatorRef, const char *, unsigned);
extern CFLocaleRef CFLocaleCreate(CFAllocatorRef, CFStringRef);
extern CFStringRef CFLocaleCopyDisplayNameForPropertyValue(CFLocaleRef, CFStringRef, CFStringRef);
extern unsigned char CFStringGetCString(CFStringRef, char *, long, unsigned);

static void show(const char *where, CFStringRef key, const char *keyName, const char *value) {
    CFLocaleRef locale = CFLocaleCreate(kCFAllocatorDefault, CFStringCreateWithCString(kCFAllocatorDefault, where, 0x08000100));
    CFStringRef name = CFLocaleCopyDisplayNameForPropertyValue(locale, key, CFStringCreateWithCString(kCFAllocatorDefault, value, 0x08000100));
    char buffer[512] = "(null)";
    if (name && !CFStringGetCString(name, buffer, sizeof buffer, 0x08000100)) strcpy(buffer, "(long)");
    printf("%s %s %s = %s\n", where, keyName, value, buffer);
}

int main(void) {
    static const char *const where[] = {"en_US", "fr_FR", "de_DE", "ja_JP", "zh_Hans_CN", "ar_SA", "ru_RU", "pt_BR", "es_419", "he_IL", "sr_Latn_RS", "en_GB@calendar=buddhist"};
    static const char *const ids[] = {"en", "en_US", "fr_CA", "zh_Hant_TW", "zh_Hans", "pt_PT", "sr_Cyrl_RS", "de_AT", "es_MX", "nb_NO", "en_US_POSIX", "ca_ES_VALENCIA", "xx_YY", "haw", "und", "ja_JP", "ko_KR", "ar_EG", "sv_FI", "es_419"};
    for (unsigned w = 0; w < sizeof where / sizeof *where; w++) {
        for (unsigned i = 0; i < sizeof ids / sizeof *ids; i++) show(where[w], kCFLocaleIdentifier, "id", ids[i]);
        show(where[w], kCFLocaleLanguageCode, "lang", "fr"); show(where[w], kCFLocaleLanguageCode, "lang", "yue");
        show(where[w], kCFLocaleCountryCode, "country", "US"); show(where[w], kCFLocaleCountryCode, "country", "TW"); show(where[w], kCFLocaleCountryCode, "country", "HK");
        show(where[w], kCFLocaleScriptCode, "script", "Latn"); show(where[w], kCFLocaleScriptCode, "script", "Hant");
        show(where[w], kCFLocaleVariantCode, "variant", "POSIX");
    }
    return 0;
}
