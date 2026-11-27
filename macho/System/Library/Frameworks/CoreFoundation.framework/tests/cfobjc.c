// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// The Objective-C side of Reef's CoreFoundation: messages to Core Foundation objects (toll-free bridging through the __NSCF* classes).
// The same output with the original and with the rebuilt file.
#include <stdio.h>
#include <string.h>

typedef const void *CFTypeRef, *CFAllocatorRef, *CFStringRef, *CFArrayRef, *CFDictionaryRef, *CFDataRef, *CFNumberRef, *CFSetRef;
typedef void *CFMutableArrayRef, *CFMutableStringRef, *CFMutableDictionaryRef, *CFMutableSetRef, *CFMutableDataRef;
typedef long CFIndex;
typedef unsigned char Boolean;
extern CFAllocatorRef kCFAllocatorDefault;
extern const char kCFTypeArrayCallBacks[], kCFTypeDictionaryKeyCallBacks[], kCFTypeDictionaryValueCallBacks[], kCFTypeSetCallBacks[];
extern CFStringRef CFStringCreateWithCString(CFAllocatorRef, const char *, unsigned);
extern CFMutableStringRef CFStringCreateMutable(CFAllocatorRef, CFIndex);
extern CFMutableArrayRef CFArrayCreateMutable(CFAllocatorRef, CFIndex, const void *);
extern CFMutableDictionaryRef CFDictionaryCreateMutable(CFAllocatorRef, CFIndex, const void *, const void *);
extern CFMutableSetRef CFSetCreateMutable(CFAllocatorRef, CFIndex, const void *);
extern CFMutableDataRef CFDataCreateMutable(CFAllocatorRef, CFIndex);
extern CFDataRef CFDataCreate(CFAllocatorRef, const unsigned char *, CFIndex);
extern CFNumberRef CFNumberCreate(CFAllocatorRef, long, const void *);
extern void CFRelease(CFTypeRef);
extern CFIndex CFArrayGetCount(CFArrayRef);
extern void CFArrayAppendValue(CFMutableArrayRef, const void *);
extern void *objc_msgSend(void *, void *, ...);
extern void *sel_registerName(const char *);
extern void *objc_getClass(const char *);
#define MSG(T, o, n, ...) ((T (*)(void *, void *, ...))objc_msgSend)((void *)(o), sel_registerName(n), ##__VA_ARGS__)
typedef struct { unsigned long location, length; } Range;

static void show(const char *what, void *string) {
    char buffer[200] = "(nil)";
    if (string) {
        extern Boolean CFStringGetCString(CFStringRef, char *, CFIndex, unsigned);
        CFStringGetCString(string, buffer, sizeof buffer, 0x08000100);
    }
    printf("%s: %s\n", what, buffer);
}

int main(void) {
    CFAllocatorRef a = kCFAllocatorDefault;
    // array
    void *array = CFArrayCreateMutable(a, 0, kCFTypeArrayCallBacks);
    CFStringRef x = CFStringCreateWithCString(a, "x", 0x08000100), y = CFStringCreateWithCString(a, "y", 0x08000100), z = CFStringCreateWithCString(a, "z", 0x08000100);
    MSG(void, array, "addObject:", x);
    MSG(void, array, "addObject:", y);
    MSG(void, array, "insertObject:atIndex:", z, 1UL);
    printf("array count %lu\n", MSG(unsigned long, array, "count"));
    show("array[0]", MSG(void *, array, "objectAtIndex:", 0UL));
    show("array[1]", MSG(void *, array, "objectAtIndex:", 1UL));
    show("array[2]", MSG(void *, array, "objectAtIndex:", 2UL));
    MSG(void, array, "exchangeObjectAtIndex:withObjectAtIndex:", 0UL, 2UL);
    show("exchanged[0]", MSG(void *, array, "objectAtIndex:", 0UL));
    MSG(void, array, "replaceObjectAtIndex:withObject:", 1UL, x);
    show("replaced[1]", MSG(void *, array, "objectAtIndex:", 1UL));
    MSG(void, array, "removeObjectAtIndex:", 0UL);
    MSG(void, array, "removeLastObject");
    printf("array count %lu\n", MSG(unsigned long, array, "count"));
    void *copy = MSG(void *, array, "copy");
    MSG(void, array, "removeAllObjects");
    printf("copy count %lu, array count %lu, equal %d\n", MSG(unsigned long, copy, "count"), MSG(unsigned long, array, "count"), (int)MSG(char, copy, "isEqual:", array));
    void *objects[2] = {0, 0};
    MSG(void, copy, "getObjects:range:", objects, (Range){0, 1});
    show("objects[0]", objects[0]);
    CFRelease(copy);
    CFRelease(array);
    // string
    void *string = CFStringCreateMutable(a, 0);
    MSG(void, string, "appendString:", x);
    MSG(void, string, "appendString:", y);
    printf("string length %lu, char[1] %c\n", MSG(unsigned long, string, "length"), (int)MSG(unsigned short, string, "characterAtIndex:", 1UL));
    MSG(void, string, "replaceCharactersInRange:withString:", (Range){0, 1}, z);
    show("string", string);
    unsigned short chars[4] = {0};
    MSG(void, string, "getCharacters:range:", chars, (Range){0, 2});
    printf("chars %c%c\n", chars[0], chars[1]);
    void *stringCopy = MSG(void *, string, "copy");
    show("string copy", stringCopy);
    printf("hash equal %d, isEqual %d\n", MSG(unsigned long, string, "hash") == MSG(unsigned long, stringCopy, "hash"), (int)MSG(char, string, "isEqual:", stringCopy));
    CFRelease(stringCopy);
    CFRelease(string);
    // dictionary
    void *dict = CFDictionaryCreateMutable(a, 0, kCFTypeDictionaryKeyCallBacks, kCFTypeDictionaryValueCallBacks);
    MSG(void, dict, "setObject:forKey:", x, y);
    MSG(void, dict, "setObject:forKey:", z, x);
    printf("dict count %lu\n", MSG(unsigned long, dict, "count"));
    show("dict[y]", MSG(void *, dict, "objectForKey:", y));
    show("dict[x]", MSG(void *, dict, "objectForKey:", x));
    printf("nil key %p, containsKey %d, containsObject %d\n", MSG(void *, dict, "objectForKey:", 0), (int)MSG(char, dict, "containsKey:", x), (int)MSG(char, dict, "containsObject:", z));
    MSG(void, dict, "removeObjectForKey:", x);
    printf("dict count %lu\n", MSG(unsigned long, dict, "count"));
    void *keys = MSG(void *, dict, "keyEnumerator");
    int seen = 0;
    while (MSG(void *, keys, "nextObject")) seen++;
    printf("enumerated %d keys\n", seen);
    void *dictCopy = MSG(void *, dict, "mutableCopy");
    MSG(void, dict, "removeAllObjects");
    printf("dict copy count %lu, dict count %lu\n", MSG(unsigned long, dictCopy, "count"), MSG(unsigned long, dict, "count"));
    CFRelease(dictCopy);
    CFRelease(dict);
    // set
    void *set = CFSetCreateMutable(a, 0, kCFTypeSetCallBacks);
    MSG(void, set, "addObject:", x);
    MSG(void, set, "addObject:", y);
    MSG(void, set, "addObject:", x);
    printf("set count %lu, contains x %d, contains z %d\n", MSG(unsigned long, set, "count"), (int)MSG(char, set, "containsObject:", x), (int)MSG(char, set, "containsObject:", z));
    show("member", MSG(void *, set, "member:", y));
    MSG(void, set, "removeObject:", x);
    printf("set count %lu\n", MSG(unsigned long, set, "count"));
    CFRelease(set);
    // data
    unsigned char bytes[4] = {1, 2, 3, 4};
    void *data = CFDataCreateMutable(a, 0);
    MSG(void, data, "appendBytes:length:", bytes, 4UL);
    unsigned char out[4] = {0};
    MSG(void, data, "getBytes:range:", out, (Range){1, 2});
    printf("data length %lu bytes %d %d\n", MSG(unsigned long, data, "length"), out[0], out[1]);
    MSG(void, data, "replaceBytesInRange:withBytes:length:", (Range){0, 1}, bytes + 3, 1UL);
    MSG(void, data, "getBytes:range:", out, (Range){0, 1});
    printf("replaced %d\n", out[0]);
    CFRelease(data);
    // numbers
    int i = 300; double d = 2.5; long long q = 1LL << 40;
    CFNumberRef n1 = CFNumberCreate(a, 9, &i), n2 = CFNumberCreate(a, 13, &d), n3 = CFNumberCreate(a, 11, &q);
    printf("int: %lld %s, double: %g %s, long long: %lld %s\n", MSG(long long, n1, "longLongValue"), (const char *)MSG(void *, n1, "objCType"),
           MSG(double, n2, "doubleValue"), (const char *)MSG(void *, n2, "objCType"), MSG(long long, n3, "longLongValue"), (const char *)MSG(void *, n3, "objCType"));
    int back = 0; MSG(void, n1, "getValue:", &back);
    printf("getValue %d\n", back);
    CFRelease(n1); CFRelease(n2); CFRelease(n3);
    CFRelease(x); CFRelease(y); CFRelease(z);
    printf("null %d\n", MSG(void *, objc_getClass("NSNull"), "null") != 0);
    void *loop = MSG(void *, objc_getClass("NSRunLoop"), "currentRunLoop");
    printf("run loop same %d, main differs or same %d\n", MSG(void *, objc_getClass("NSRunLoop"), "currentRunLoop") == loop, loop != 0);
    // streams
    void *source = 0, *sink = 0;
    MSG(void, objc_getClass("NSStream"), "getBoundStreamsWithBufferSize:inputStream:outputStream:", 64UL, &source, &sink);
    MSG(void, source, "open");
    MSG(void, sink, "open");
    printf("space %d\n", (int)MSG(char, sink, "hasSpaceAvailable"));
    long wrote = MSG(long, sink, "write:maxLength:", "hello", 5UL);
    char text[8] = {0};
    long got = MSG(long, source, "read:maxLength:", text, 8UL);
    printf("wrote %ld, read %ld %s\n", wrote, got, text);
    // errors
    CFStringRef domain = CFStringCreateWithCString(a, "TestDomain", 0x08000100);
    void *error = MSG(void *, objc_getClass("NSError"), "errorWithDomain:code:userInfo:", domain, 42L, 0);
    void *error2 = MSG(void *, objc_getClass("NSError"), "errorWithDomain:code:userInfo:", domain, 42L, 0);
    printf("error code %ld, equal %d, hash equal %d\n", MSG(long, error, "code"), (int)MSG(char, error, "isEqual:", error2), MSG(unsigned long, error, "hash") == MSG(unsigned long, error2, "hash"));
    show("domain", MSG(void *, error, "domain"));
    show("description", MSG(void *, error, "localizedDescription"));
    CFRelease(domain);
    printf("(end)\n");
    return 0;
}
