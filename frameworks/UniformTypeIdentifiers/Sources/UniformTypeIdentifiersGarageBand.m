// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

// Core types that Reef's framework does not export: made when the library is loaded.
UTType *ReefTypeAIFF __asm__("_UTTypeAIFF");
UTType *ReefTypeWAV __asm__("_UTTypeWAV");

__attribute__((constructor)) static void makeTypes(void) {
    ReefTypeAIFF = [UTType typeWithIdentifier:@"public.aiff-audio"];
    ReefTypeWAV = [UTType typeWithIdentifier:@"com.microsoft.waveform-audio"];
}

// The type of the device the application runs on (private): a Mac
#import <objc/runtime.h>
static NSString *deviceIdentifier(id self, SEL _cmd) { return @"com.apple.mac"; }
static NSString *deviceDescription(id self, SEL _cmd) { return @"Mac"; }

static id typeOfCurrentDevice(id self, SEL _cmd) {
    static UTType *type;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        type = [UTType typeWithIdentifier:@"com.apple.mac"] ?: [UTType typeWithIdentifier:@"public.item"];
        // a subclass made at run time (UTType may not be subclassed in source) that names the device
        Class device = objc_allocateClassPair([type class], "ReefDeviceType", 0);
        if (device) {
            class_addMethod(device, @selector(identifier), (IMP)deviceIdentifier, "@@:");
            class_addMethod(device, @selector(localizedDescription), (IMP)deviceDescription, "@@:");
            objc_registerClassPair(device);
            object_setClass(type, device);
        }
    });
    return type;
}

__attribute__((constructor)) static void addDeviceType(void) {
    class_addMethod(object_getClass([UTType class]), sel_registerName("_typeOfCurrentDevice"), (IMP)typeOfCurrentDevice, "@@:");
}
