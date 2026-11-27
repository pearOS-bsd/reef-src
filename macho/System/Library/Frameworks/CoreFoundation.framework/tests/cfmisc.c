// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// NSString line and paragraph bounds, NSNumber subclasses (_cfNumberType, _getValue:forType:), NSDictionary0 and NSStream _cfStreamError in Reef's CoreFoundation.
// The same output with the original and with the rebuilt file.
#include <stdio.h>
#include <string.h>

typedef const void *CFStringRef, *CFAllocatorRef;
extern CFAllocatorRef kCFAllocatorDefault;
extern CFStringRef CFStringCreateWithCString(CFAllocatorRef, const char *, unsigned);
extern void *objc_msgSend(void *, void *, ...);
extern void *sel_registerName(const char *);
extern void *objc_getClass(const char *);
extern void *objc_allocateClassPair(void *, const char *, unsigned long);
extern void objc_registerClassPair(void *);
extern unsigned char class_addMethod(void *, void *, void *, const char *);
#define MSG(T, o, n, ...) ((T (*)(void *, void *, ...))objc_msgSend)((void *)(o), sel_registerName(n), ##__VA_ARGS__)
#define SEL(n) sel_registerName(n)
typedef struct { unsigned long location, length; } Range;

static void bounds(const char *text, unsigned long location, unsigned long length) {
    void *string = (void *)CFStringCreateWithCString(kCFAllocatorDefault, text, 0x08000100);
    unsigned long start = 99, end = 99, contents = 99, pstart = 99, pend = 99, pcontents = 99;
    Range range = {location, length};
    ((void (*)(void *, void *, unsigned long *, unsigned long *, unsigned long *, Range))objc_msgSend)(string, SEL("getLineStart:end:contentsEnd:forRange:"), &start, &end, &contents, range);
    ((void (*)(void *, void *, unsigned long *, unsigned long *, unsigned long *, Range))objc_msgSend)(string, SEL("getParagraphStart:end:contentsEnd:forRange:"), &pstart, &pend, &pcontents, range);
    printf("[%lu,%lu] line %lu %lu %lu paragraph %lu %lu %lu\n", location, length, start, end, contents, pstart, pend, pcontents);
}

// A number class of our own: the value and the type are fields of the object.
static double numberValue;
static const char *numberType;
static const char *numberObjCType(void *self, void *cmd) { return numberType; }
static void numberGetValue(void *self, void *cmd, void *out) {
    if (*numberType == 'd') *(double *)out = numberValue;
    else if (*numberType == 'f') *(float *)out = numberValue;
    else if (*numberType == 'c') *(char *)out = numberValue;
    else if (*numberType == 's') *(short *)out = numberValue;
    else if (*numberType == 'i') *(int *)out = numberValue;
    else if (*numberType == 'Q') *(unsigned long long *)out = (unsigned long long)numberValue;
    else *(long long *)out = numberValue;
}

int main(void) {
    bounds("one\ntwo\r\nthree\rfour", 0, 0);
    bounds("one\ntwo\r\nthree\rfour", 5, 0);
    bounds("one\ntwo\r\nthree\rfour", 8, 1);
    bounds("one\ntwo\r\nthree\rfour", 9, 0);
    bounds("one\ntwo\r\nthree\rfour", 4, 12);
    bounds("one\ntwo\r\nthree\rfour", 14, 2);
    bounds("one\ntwo\r\nthree\rfour", 19, 0);
    bounds("one\ntwo\r\nthree\rfour", 3, 0);
    bounds("a\xe2\x80\xa8" "b\xc2\x85" "c\xe2\x80\xa9" "d", 3, 0);
    bounds("a\xe2\x80\xa8" "b\xc2\x85" "c\xe2\x80\xa9" "d", 5, 0);
    bounds("a\xe2\x80\xa8" "b\xc2\x85" "c\xe2\x80\xa9" "d", 0, 7);
    bounds("", 0, 0);
    bounds("\n", 0, 0);
    bounds("\n", 1, 0);
    // numbers
    void *cls = objc_allocateClassPair(objc_getClass("NSNumber"), "MyNumber", 0);
    class_addMethod(cls, SEL("objCType"), numberObjCType, "r*16@0:8");
    class_addMethod(cls, SEL("getValue:"), numberGetValue, "v24@0:8^v16");
    objc_registerClassPair(cls);
    void *number = MSG(void *, MSG(void *, cls, "alloc"), "init");
    const char *types[] = {"c", "s", "i", "l", "q", "Q", "f", "d", "C", "S", "I", "L", "B"};
    double values[] = {-3, 300.75, 100000, -5.5};
    for (unsigned t = 0; t < sizeof types / sizeof *types; t++) {
        numberType = types[t];
        for (unsigned v = 0; v < 4; v++) {
            numberValue = values[v];
            printf("%s %g: cfType %ld", numberType, numberValue, MSG(long, number, "_cfNumberType"));
            for (long ct = 0; ct <= 7; ct++) {
                union { char c; short s; int i; long long q; float f; double d; unsigned char raw[16]; } out;
                memset(&out, 0, sizeof out);
                unsigned char ok = MSG(unsigned char, number, "_getValue:forType:", &out, ct);
                if (!ok) { printf(" t%ld:no", ct); continue; }
                switch (ct) {
                case 1: printf(" t1:%d", out.c); break;
                case 2: printf(" t2:%d", out.s); break;
                case 3: printf(" t3:%d", out.i); break;
                case 4: printf(" t4:%lld", out.q); break;
                case 5: printf(" t5:%g", out.f); break;
                case 6: printf(" t6:%g", out.d); break;
                }
            }
            printf("\n");
        }
    }
    // the empty dictionary singleton
    void *error = MSG(void *, objc_getClass("NSError"), "errorWithDomain:code:userInfo:", CFStringCreateWithCString(kCFAllocatorDefault, "d", 0x08000100), 5L, 0);
    printf("error user info %p\n", MSG(void *, error, "userInfo") ? (void *)1 : (void *)0);
    return 0;
}
