// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// MediaAccessibility: the fixed caption appearance (colours, opacities, sizes) and the font per style.
#include <stdio.h>

typedef struct CGColor *CGColorRef;
typedef struct CTFontDescriptor *CTFontDescriptorRef;
typedef const void *CFStringRef;
extern CFStringRef kMACaptionAppearanceSettingsChangedNotification;
extern const double *CGColorGetComponents(CGColorRef);
extern unsigned long CGColorGetNumberOfComponents(CGColorRef);
extern CFStringRef CTFontDescriptorCopyAttribute(CTFontDescriptorRef, CFStringRef);
extern long CFStringGetLength(CFStringRef);
extern unsigned char CFStringGetCString(CFStringRef, char *, long, unsigned);
extern CFStringRef kCTFontNameAttribute;
extern void CFRelease(const void *);

int MACaptionAppearanceGetDisplayType(int);
CGColorRef MACaptionAppearanceCopyForegroundColor(int, long *);
CGColorRef MACaptionAppearanceCopyBackgroundColor(int, long *);
CGColorRef MACaptionAppearanceCopyWindowColor(int, long *);
double MACaptionAppearanceGetForegroundOpacity(int, long *);
double MACaptionAppearanceGetBackgroundOpacity(int, long *);
double MACaptionAppearanceGetWindowOpacity(int, long *);
double MACaptionAppearanceGetWindowRoundedCornerRadius(int, long *);
double MACaptionAppearanceGetRelativeCharacterSize(int, long *);
int MACaptionAppearanceGetTextEdgeStyle(int, long *);
CTFontDescriptorRef MACaptionAppearanceCopyFontDescriptorForStyle(int, long *, unsigned);

static void color(const char *what, CGColorRef c, long behavior)
{
    const double *v = CGColorGetComponents(c);
    printf("%s: behavior %ld, %lu components %g %g %g %g\n", what, behavior, CGColorGetNumberOfComponents(c), v[0], v[1], v[2], v[3]);
    CFRelease(c);
}

int main(void)
{
    long b = 9;
    printf("notification length %ld\n", CFStringGetLength(kMACaptionAppearanceSettingsChangedNotification));
    printf("display %d\n", MACaptionAppearanceGetDisplayType(0));
    color("fg", MACaptionAppearanceCopyForegroundColor(0, &b), b);
    color("bg", MACaptionAppearanceCopyBackgroundColor(0, &b), b);
    color("window", MACaptionAppearanceCopyWindowColor(0, 0), 0);
    b = 9; printf("fg opacity %g %ld\n", MACaptionAppearanceGetForegroundOpacity(0, &b), b);
    b = 9; printf("bg opacity %g %ld\n", MACaptionAppearanceGetBackgroundOpacity(0, &b), b);
    b = 9; printf("window opacity %g %ld\n", MACaptionAppearanceGetWindowOpacity(0, &b), b);
    b = 9; printf("radius %g %ld\n", MACaptionAppearanceGetWindowRoundedCornerRadius(0, &b), b);
    b = 9; printf("size %g %ld\n", MACaptionAppearanceGetRelativeCharacterSize(0, &b), b);
    b = 9; printf("edge %d %ld\n", MACaptionAppearanceGetTextEdgeStyle(0, &b), b);
    for (unsigned style = 0; style <= 8; style++) {
        b = 9;
        CTFontDescriptorRef d = MACaptionAppearanceCopyFontDescriptorForStyle(0, &b, style);
        char name[64] = "?";
        if (d) {
            CFStringRef n = CTFontDescriptorCopyAttribute(d, kCTFontNameAttribute);
            if (n) { CFStringGetCString(n, name, sizeof name, 0x08000100); CFRelease(n); }
            CFRelease(d);
        }
        printf("style %u: %s (%ld)\n", style, name, b);
    }
    return 0;
}
