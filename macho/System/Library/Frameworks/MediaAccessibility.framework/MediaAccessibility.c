// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MediaAccessibility: caption appearance. There are no settings; every query answers with the same defaults (white text, black background and
// window, a font for the style) and says to use that value.

typedef const void *CFStringRef;
typedef struct CGColorSpace *CGColorSpaceRef;
typedef struct CGColor *CGColorRef;
typedef struct CTFontDescriptor *CTFontDescriptorRef;
typedef double CGFloat;
typedef long MACaptionAppearanceBehavior;
typedef int MACaptionAppearanceDomain;

enum { kMACaptionAppearanceBehaviorUseValue = 1 };

extern CGColorSpaceRef CGColorSpaceCreateDeviceRGB(void);
extern CGColorRef CGColorCreate(CGColorSpaceRef, const CGFloat *);
extern void CGColorSpaceRelease(CGColorSpaceRef);
extern CTFontDescriptorRef CTFontDescriptorCreateWithNameAndSize(CFStringRef, CGFloat);

#define CFSTR(string) ((CFStringRef)__builtin___CFStringMakeConstantString(string))

const CFStringRef kMACaptionAppearanceSettingsChangedNotification = CFSTR("MACaptionAppearanceSettingsChangedNotification");

int MACaptionAppearanceGetDisplayType(MACaptionAppearanceDomain domain)
{
    return 0;
}

static CGColorRef copyColor(MACaptionAppearanceBehavior *behavior, CGFloat red, CGFloat green, CGFloat blue, CGFloat alpha)
{
    if (behavior)
        *behavior = kMACaptionAppearanceBehaviorUseValue;
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGFloat components[4] = { red, green, blue, alpha };
    CGColorRef color = CGColorCreate(space, components);
    CGColorSpaceRelease(space);
    return color;
}

CGColorRef MACaptionAppearanceCopyForegroundColor(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    return copyColor(behavior, 1, 1, 1, 1);
}

CGColorRef MACaptionAppearanceCopyBackgroundColor(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    return copyColor(behavior, 0, 0, 0, 1);
}

CGColorRef MACaptionAppearanceCopyWindowColor(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    return copyColor(behavior, 0, 0, 0, 1);
}

static CGFloat useValue(MACaptionAppearanceBehavior *behavior, CGFloat value)
{
    if (behavior)
        *behavior = kMACaptionAppearanceBehaviorUseValue;
    return value;
}

CGFloat MACaptionAppearanceGetForegroundOpacity(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    return useValue(behavior, 1.0);
}

CGFloat MACaptionAppearanceGetBackgroundOpacity(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    return useValue(behavior, 0.8);
}

CGFloat MACaptionAppearanceGetWindowOpacity(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    return useValue(behavior, 0.0);
}

CGFloat MACaptionAppearanceGetWindowRoundedCornerRadius(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    return useValue(behavior, 0.0);
}

CGFloat MACaptionAppearanceGetRelativeCharacterSize(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    return useValue(behavior, 1.0);
}

int MACaptionAppearanceGetTextEdgeStyle(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior)
{
    if (behavior)
        *behavior = kMACaptionAppearanceBehaviorUseValue;
    return 1;
}

// Styles 1 to 6 (monospaced serif, proportional serif, monospaced sans, proportional sans, casual, cursive); anything else is Helvetica.
CTFontDescriptorRef MACaptionAppearanceCopyFontDescriptorForStyle(MACaptionAppearanceDomain domain, MACaptionAppearanceBehavior *behavior, unsigned style)
{
    if (behavior)
        *behavior = kMACaptionAppearanceBehaviorUseValue;
    CFStringRef name;
    switch (style) {
    case 1: name = CFSTR("Courier"); break;
    case 2: name = CFSTR("Times-Roman"); break;
    case 3: name = CFSTR("Menlo-Regular"); break;
    case 4: name = CFSTR("Helvetica"); break;
    case 5: name = CFSTR("Helvetica"); break;
    case 6: name = CFSTR("Apple Chancery"); break;
    default: name = CFSTR("Helvetica"); break;
    }
    return CTFontDescriptorCreateWithNameAndSize(name, 0.0);
}
