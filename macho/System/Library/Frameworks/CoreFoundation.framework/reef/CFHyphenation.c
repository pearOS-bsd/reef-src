// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Hyphenation of CFString words with the hyphen library of the host (libhyphen.so.0, reached through reef_host_symbol) and its dictionaries
// in /usr/share/hyphen.
#include "CFInternal.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern void *reef_host_symbol(const char *library, const char *symbol);

typedef void *(*HyphenLoad)(const char *path);
typedef int (*HyphenHyphenate)(void *dict, const char *word, int wordSize, char *hyphens, char *hyphenatedWord, char ***rep, int **pos, int **cut, int lhmin, int rhmin, int clhmin, int crhmin);

struct dictionary {
    char name[32];
    void *handle;
    CFStringEncoding encoding;
};

static HyphenLoad hyphenLoad;
static HyphenHyphenate hyphenHyphenate;
static pthread_once_t libraryOnce = PTHREAD_ONCE_INIT;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static struct dictionary loaded[16];
static int loadedCount;

static void loadLibrary(void) {
    hyphenLoad = (HyphenLoad)reef_host_symbol("libhyphen.so.0", "hnj_hyphen_load");
    hyphenHyphenate = (HyphenHyphenate)reef_host_symbol("libhyphen.so.0", "hnj_hyphen_hyphenate3");
}

// The dictionary of a locale, loaded on first use, or NULL.
static struct dictionary *dictionaryFor(CFLocaleRef locale) {
    pthread_once(&libraryOnce, loadLibrary);
    if (!hyphenLoad || !hyphenHyphenate || !locale) return NULL;
    char identifier[64] = {0};
    CFStringRef string = CFLocaleGetIdentifier(locale);
    if (!string || !CFStringGetCString(string, identifier, sizeof(identifier), kCFStringEncodingUTF8) || !identifier[0]) return NULL;
    char language[16] = {0}, region[16] = {0};
    sscanf(identifier, "%15[a-zA-Z]", language);
    for (char *separator = strpbrk(identifier, "_-"); separator; separator = strpbrk(separator + 1, "_-")) {
        char part[16] = {0};
        sscanf(separator + 1, "%15[a-zA-Z0-9]", part);
        if (strlen(part) == 2 && part[0] >= 'A' && part[0] <= 'Z') {
            snprintf(region, sizeof(region), "%s", part);
            break;
        }
    }
    if (!language[0]) return NULL;
    char path[256], key[32];
    if (region[0]) {
        snprintf(path, sizeof(path), "/usr/share/hyphen/hyph_%s_%s.dic", language, region);
        snprintf(key, sizeof(key), "%s_%s", language, region);
        if (access(path, R_OK) != 0) region[0] = 0;
    }
    if (!region[0]) {
        snprintf(path, sizeof(path), "/usr/share/hyphen/hyph_%s.dic", language);
        snprintf(key, sizeof(key), "%s", language);
        if (access(path, R_OK) != 0) return NULL;
    }
    pthread_mutex_lock(&lock);
    struct dictionary *found = NULL;
    for (int i = 0; i < loadedCount; i++) {
        if (strcmp(loaded[i].name, key) == 0) {
            found = &loaded[i];
            break;
        }
    }
    if (!found && loadedCount < 16) {
        void *handle = hyphenLoad(path);
        if (handle) {
            char line[32] = {0};
            FILE *file = fopen(path, "r");
            if (file) {
                if (fgets(line, sizeof(line), file)) line[strcspn(line, " \r\n")] = 0;
                fclose(file);
            }
            found = &loaded[loadedCount++];
            snprintf(found->name, sizeof(found->name), "%s", key);
            found->handle = handle;
            found->encoding = strcmp(line, "UTF-8") == 0 ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1;
        }
    }
    pthread_mutex_unlock(&lock);
    return found;
}

Boolean CFStringIsHyphenationAvailableForLocale(CFLocaleRef locale) {
    return dictionaryFor(locale) != NULL;
}

CFIndex CFStringGetHyphenationLocationBeforeIndex(CFStringRef string, CFIndex location, CFRange limitRange, CFOptionFlags options, CFLocaleRef locale, UTF32Char *character) {
    if (!(limitRange.location < location && string && location <= limitRange.location + limitRange.length)) return kCFNotFound;
    if (location > CFStringGetLength(string)) return kCFNotFound;
    struct dictionary *dictionary = dictionaryFor(locale);
    if (!dictionary) return kCFNotFound;
    CFIndex end = limitRange.location + limitRange.length;
    CFCharacterSetRef letters = CFCharacterSetGetPredefined(kCFCharacterSetLetter);
    CFIndex start = location;
    while (start > limitRange.location && CFCharacterSetIsCharacterMember(letters, CFStringGetCharacterAtIndex(string, start - 1))) start--;
    CFIndex stop = location;
    while (stop < end && CFCharacterSetIsCharacterMember(letters, CFStringGetCharacterAtIndex(string, stop))) stop++;
    CFIndex length = stop - start;
    if (length < 3 || length > 100) return kCFNotFound;
    CFMutableStringRef word = CFStringCreateMutable(kCFAllocatorDefault, 0);
    CFStringRef piece = CFStringCreateWithSubstring(kCFAllocatorDefault, string, CFRangeMake(start, length));
    CFStringAppend(word, piece);
    CFRelease(piece);
    CFStringLowercase(word, locale);
    if (CFStringGetLength(word) != length) {
        CFRelease(word);
        return kCFNotFound;
    }
    char bytes[0x200];
    int offsets[128] = {0};
    int used = 0;
    for (CFIndex i = 0; i < length; i++) {
        CFIndex count = 0;
        if (CFStringGetBytes(word, CFRangeMake(i, 1), dictionary->encoding, 0, false, (UInt8 *)bytes + used, sizeof(bytes) - 1 - used, &count) != 1) {
            CFRelease(word);
            return kCFNotFound;
        }
        used += (int)count;
        offsets[i + 1] = used;
    }
    CFRelease(word);
    bytes[used] = 0;
    char hyphens[0x200];
    char **rep = NULL;
    int *pos = NULL, *cut = NULL;
    pthread_mutex_lock(&lock);
    int status = hyphenHyphenate(dictionary->handle, bytes, used, hyphens, NULL, &rep, &pos, &cut, 2, 2, 2, 2);
    pthread_mutex_unlock(&lock);
    if (rep) {
        for (int i = 0; i < used; i++) free(rep[i]);
        free(rep);
        free(pos);
        free(cut);
    }
    if (status != 0) return kCFNotFound;
    CFIndex result = location;
    CFIndex index = location - start - 1;
    for (;;) {
        if (index < 1) return kCFNotFound;
        int offset = offsets[index];
        index--;
        result--;
        if (hyphens[offset - 1] & 1) break;
    }
    if (character) *character = '-';
    return result;
}
