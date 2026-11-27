// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Keyed archives store no class versions: Apple's NSKeyedUnarchiver answers versionForClassName: with the current
// version of the class, NSNotFound for a class that does not exist (observed on macOS 26). Reef's falls through to
// NSCoder's abstract method.
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

static NSInteger versionForClassName(id self, SEL _cmd, NSString *name) {
    Class c = NSClassFromString(name);
    return c ? [c version] : NSNotFound;
}

__attribute__((constructor)) static void addKeyedVersion(void) {
    class_addMethod([NSKeyedUnarchiver class], @selector(versionForClassName:), (IMP)versionForClassName, "q@:@");
}
