// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoreDisplay: the display's info dictionary holds one name, "Built-in Display", under the locale en_US.

typedef const void *CFTypeRef;
typedef const struct __CFAllocator *CFAllocatorRef;
typedef const struct __CFString *CFStringRef;
typedef struct __CFDictionary *CFMutableDictionaryRef;
typedef const struct __CFDictionary *CFDictionaryRef;
typedef struct { long version; void *a, *b, *c, *d; } CFDictionaryKeyCallBacks;
typedef struct { long version; void *a, *b, *c; } CFDictionaryValueCallBacks;

extern const CFAllocatorRef kCFAllocatorDefault;
extern const CFDictionaryKeyCallBacks kCFTypeDictionaryKeyCallBacks;
extern const CFDictionaryValueCallBacks kCFTypeDictionaryValueCallBacks;
extern CFMutableDictionaryRef CFDictionaryCreateMutable(CFAllocatorRef, long, const CFDictionaryKeyCallBacks *, const CFDictionaryValueCallBacks *);
extern void CFDictionarySetValue(CFMutableDictionaryRef, const void *, const void *);
extern void CFRelease(CFTypeRef);

#define CFSTR(string) ((CFStringRef)__builtin___CFStringMakeConstantString(string))

CFDictionaryRef CoreDisplay_DisplayCreateInfoDictionary(unsigned display)
{
    CFAllocatorRef allocator = kCFAllocatorDefault;
    CFMutableDictionaryRef names = CFDictionaryCreateMutable(allocator, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(names, CFSTR("en_US"), CFSTR("Built-in Display"));
    CFMutableDictionaryRef info = CFDictionaryCreateMutable(allocator, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(info, CFSTR("DisplayProductName"), names);
    CFRelease(names);
    return info;
}
