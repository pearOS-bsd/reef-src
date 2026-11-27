// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Behaviour of Reef's own CoreFoundation code: the same output with the original and with the rebuilt file.
#include <stdio.h>
#include <string.h>

typedef const void *CFTypeRef, *CFAllocatorRef, *CFStringRef, *CFArrayRef, *CFDictionaryRef, *CFDataRef;
typedef long CFIndex;
typedef unsigned char Boolean;
typedef struct { CFIndex location, length; } CFRange;
typedef struct __CFStringTokenizer *CFStringTokenizerRef;
typedef const struct __CFNotificationCenter *CFNotificationCenterRef;
extern CFAllocatorRef kCFAllocatorDefault;
extern CFStringRef CFStringCreateWithCString(CFAllocatorRef, const char *, unsigned);
extern CFIndex CFStringGetLength(CFStringRef);
extern Boolean CFStringGetCString(CFStringRef, char *, CFIndex, unsigned);
extern void CFRelease(CFTypeRef);
extern CFStringRef CFXMLCreateStringByEscapingEntities(CFAllocatorRef, CFStringRef, CFDictionaryRef);
extern CFStringRef CFXMLCreateStringByUnescapingEntities(CFAllocatorRef, CFStringRef, CFDictionaryRef);
extern CFStringTokenizerRef CFStringTokenizerCreate(CFAllocatorRef, CFStringRef, CFRange, unsigned long, const void *);
extern unsigned long CFStringTokenizerAdvanceToNextToken(CFStringTokenizerRef);
extern CFRange CFStringTokenizerGetCurrentTokenRange(CFStringTokenizerRef);
extern CFStringRef CFStringTokenizerCopyBestStringLanguage(CFStringRef, CFRange);
extern CFNotificationCenterRef CFNotificationCenterGetLocalCenter(void);
extern CFNotificationCenterRef CFNotificationCenterGetDarwinNotifyCenter(void);
extern void CFNotificationCenterAddObserver(CFNotificationCenterRef, const void *, void (*)(CFNotificationCenterRef, void *, CFStringRef, const void *, CFDictionaryRef), CFStringRef, const void *, long);
extern void CFNotificationCenterPostNotification(CFNotificationCenterRef, CFStringRef, const void *, CFDictionaryRef, Boolean);
extern void CFNotificationCenterRemoveEveryObserver(CFNotificationCenterRef, const void *);
extern void CFStreamCreateBoundPair(CFAllocatorRef, void **, void **, CFIndex);
extern Boolean CFReadStreamOpen(void *);
extern Boolean CFWriteStreamOpen(void *);
extern CFIndex CFWriteStreamWrite(void *, const unsigned char *, CFIndex);
extern CFIndex CFReadStreamRead(void *, unsigned char *, CFIndex);
extern void CFWriteStreamClose(void *);

#define UTF8 0x08000100

static void show(const char *label, CFStringRef s) {
    char buffer[256] = "(null)";
    if (s) CFStringGetCString(s, buffer, sizeof(buffer), UTF8);
    printf("%s: %s\n", label, buffer);
    if (s) CFRelease(s);
}

static int received;

static void observer(CFNotificationCenterRef center, void *who, CFStringRef name, const void *object, CFDictionaryRef info) {
    char buffer[64];
    CFStringGetCString(name, buffer, sizeof(buffer), UTF8);
    printf("notification %s %s %d\n", buffer, (const char *)who, object == NULL);
    received++;
}

int main(void) {
    CFStringRef text = CFStringCreateWithCString(kCFAllocatorDefault, "a < b & \"c\" > 'd'", UTF8);
    CFStringRef escaped = CFXMLCreateStringByEscapingEntities(kCFAllocatorDefault, text, NULL);
    show("escaped", escaped);
    CFStringRef source = CFStringCreateWithCString(kCFAllocatorDefault, "x &lt; y &amp; &#65;&#x42; &unknown;", UTF8);
    show("unescaped", CFXMLCreateStringByUnescapingEntities(kCFAllocatorDefault, source, NULL));
    CFRelease(source);
    CFRelease(text);

    CFStringRef sentence = CFStringCreateWithCString(kCFAllocatorDefault, "Hello, wide world 42!", UTF8);
    CFRange all = { 0, CFStringGetLength(sentence) };
    CFStringTokenizerRef tokenizer = CFStringTokenizerCreate(kCFAllocatorDefault, sentence, all, 0, NULL);
    unsigned long type;
    while ((type = CFStringTokenizerAdvanceToNextToken(tokenizer)) != 0) {
        CFRange r = CFStringTokenizerGetCurrentTokenRange(tokenizer);
        printf("token %ld+%ld type %lx\n", r.location, r.length, type);
    }
    show("language", (CFStringRef)CFStringTokenizerCopyBestStringLanguage(sentence, all));
    CFRelease(tokenizer);
    CFRelease(sentence);

    CFNotificationCenterRef center = CFNotificationCenterGetDarwinNotifyCenter();
    CFStringRef name = CFStringCreateWithCString(kCFAllocatorDefault, "org.pearos.test", UTF8);
    CFNotificationCenterAddObserver(center, "me", observer, name, NULL, 0);
    CFNotificationCenterPostNotification(center, name, NULL, NULL, 1);
    CFNotificationCenterRemoveEveryObserver(center, "me");
    CFNotificationCenterPostNotification(center, name, NULL, NULL, 1);
    printf("received %d\n", received);
    CFRelease(name);

    void *reader = NULL, *writer = NULL;
    CFStreamCreateBoundPair(kCFAllocatorDefault, &reader, &writer, 64);
    CFReadStreamOpen(reader);
    CFWriteStreamOpen(writer);
    CFIndex written = CFWriteStreamWrite(writer, (const unsigned char *)"through the pair", 16);
    CFWriteStreamClose(writer);
    unsigned char buffer[64];
    CFIndex got = CFReadStreamRead(reader, buffer, sizeof(buffer));
    printf("pair %ld %ld %.*s\n", written, got, (int)got, buffer);
    CFRelease(reader);
    CFRelease(writer);
    return 0;
}
