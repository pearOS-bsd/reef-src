// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CoreDisplay: the info dictionary of a display and the name inside it.
#include <stdio.h>

typedef const void *CFTypeRef;
extern CFTypeRef CoreDisplay_DisplayCreateInfoDictionary(unsigned);
extern CFTypeRef CFDictionaryGetValue(CFTypeRef, CFTypeRef);
extern long CFDictionaryGetCount(CFTypeRef);
extern CFTypeRef CFStringCreateWithCString(CFTypeRef, const char *, unsigned);
extern unsigned char CFStringGetCString(CFTypeRef, char *, long, unsigned);
extern void CFRelease(CFTypeRef);

int main(void)
{
    CFTypeRef info = CoreDisplay_DisplayCreateInfoDictionary(1);
    printf("count %ld\n", CFDictionaryGetCount(info));
    CFTypeRef names = CFDictionaryGetValue(info, CFStringCreateWithCString(0, "DisplayProductName", 0x08000100));
    printf("names %ld\n", names ? CFDictionaryGetCount(names) : -1);
    char name[64] = "?";
    CFTypeRef value = names ? CFDictionaryGetValue(names, CFStringCreateWithCString(0, "en_US", 0x08000100)) : 0;
    if (value) CFStringGetCString(value, name, sizeof name, 0x08000100);
    printf("name %s\n", name);
    CFRelease(info);
    return 0;
}
