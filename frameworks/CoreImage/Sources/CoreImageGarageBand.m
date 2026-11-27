// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// The convenience constructor of the source-atop compositing filter, where Reef's CoreImage has none
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import <objc/message.h>

static id sourceAtopCompositingFilter(id self, SEL _cmd) {
    return ((id (*)(id, SEL, NSString *))objc_msgSend)(self, sel_registerName("filterWithName:"), @"CISourceAtopCompositing");
}

__attribute__((constructor)) static void addCompositingFilters(void) {
    Class filter = objc_getClass("CIFilter");
    if (filter) class_addMethod(object_getClass(filter), sel_registerName("sourceAtopCompositingFilter"), (IMP)sourceAtopCompositingFilter, "@@:");
}
