// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Private CoreGraphics (window server) functions GarageBand imports and Reef's CoreGraphics lacks.
#include <CoreFoundation/CoreFoundation.h>
#include <stdint.h>

typedef struct CGContext *CGContextRef;
typedef const void *CGStyleRef;

void CGContextSetStyle(CGContextRef context, CGStyleRef style) {}
CGStyleRef CGStyleCreateFocusRingWithColor(const void *focusRing, const void *color) { return NULL; }
void CGStyleRelease(CGStyleRef style) { if (style) CFRelease(style); }
int32_t CGSGetSymbolicHotKeyRepresentationList(void *list) { return 1000; }   // kCGErrorFailure
Boolean CGSIsSymbolicHotKeyEnabled(int32_t hotKey) { return false; }
int32_t CGSSetConnectionProperty(int32_t connection, int32_t target, CFStringRef key, CFTypeRef value) { return 0; }
int32_t CGShieldingWindowLevel(void) { return 2147483628; }
CFArrayRef CGWindowListCreate(uint32_t option, uint32_t relativeToWindow) { return CFArrayCreate(NULL, NULL, 0, NULL); }
