// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// A bring-up aid, off unless REEF_IGNORE_UNRECOGNIZED is set in the environment: a message that no class
// implements (and that the receiver does not forward) answers zero instead of raising
// NSInvalidArgumentException, and is printed once ("reef: ignored -[Class selector]"). It lets an
// application run past methods Reef does not have yet, so they can be listed; Apple's runtime raises.

#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import <stdlib.h>

// Zero in every result register
__asm__(".text\n.p2align 4\n_ReefZeroResult:\n xorl %eax, %eax\n xorl %edx, %edx\n xorps %xmm0, %xmm0\n xorps %xmm1, %xmm1\n retq\n");
extern void ReefZeroResult(void);

@interface ReefIgnoredMessageSink : NSObject
@end

@implementation ReefIgnoredMessageSink
+ (BOOL)resolveInstanceMethod:(SEL)sel {
    class_addMethod(self, sel, (IMP)ReefZeroResult, "@@:");
    return YES;
}
@end

static id sink;
static IMP originalForwardingTarget, originalClassForwardingTarget;
static IMP baseForwardInvocation;

static id targetFor(id self, SEL sel, BOOL isClass) {
    Class cls = object_getClass(self);
    isClass = class_isMetaClass(cls);
    // Receivers that forward on their own keep doing so
    if (!isClass && class_getMethodImplementation(cls, @selector(forwardInvocation:)) != baseForwardInvocation)
        return nil;
    if (!isClass && [self methodSignatureForSelector:sel])
        return nil;
    static NSMutableSet *printed;
    @synchronized ([ReefIgnoredMessageSink class]) {
        if (!printed) printed = [NSMutableSet set];
        NSString *key = [NSString stringWithFormat:@"%c[%s %s]", isClass ? '+' : '-', class_getName(isClass ? (Class)self : cls), sel_getName(sel)];
        if (![printed containsObject:key]) {
            [printed addObject:key];
            fprintf(stderr, "reef: ignored %s\n", key.UTF8String);
        }
    }
    return sink;
}

static id forwardingTarget(id self, SEL _cmd, SEL sel) {
    id target = originalForwardingTarget ? ((id (*)(id, SEL, SEL))originalForwardingTarget)(self, _cmd, sel) : nil;
    return target ? target : targetFor(self, sel, NO);
}

static id classForwardingTarget(id self, SEL _cmd, SEL sel) {
    id target = originalClassForwardingTarget ? ((id (*)(id, SEL, SEL))originalClassForwardingTarget)(self, _cmd, sel) : nil;
    return target ? target : targetFor(self, sel, YES);
}

__attribute__((constructor)) static void installIgnoreUnrecognized(void) {
    if (!getenv("REEF_IGNORE_UNRECOGNIZED"))
        return;
    fprintf(stderr, "reef: unrecognized messages answer zero (REEF_IGNORE_UNRECOGNIZED)\n");
    sink = [ReefIgnoredMessageSink new];
    baseForwardInvocation = class_getMethodImplementation([NSObject class], @selector(forwardInvocation:));
    SEL s = @selector(forwardingTargetForSelector:);
    Method m = class_getInstanceMethod([NSObject class], s);
    if (m) {
        originalForwardingTarget = method_getImplementation(m);
        method_setImplementation(m, (IMP)forwardingTarget);
    } else
        class_addMethod([NSObject class], s, (IMP)forwardingTarget, "@@::");
    Class meta = object_getClass([NSObject class]);
    m = class_getInstanceMethod(meta, s);
    if (m && class_getMethodImplementation(meta, s) != (IMP)forwardingTarget) {
        originalClassForwardingTarget = method_getImplementation(m);
        method_setImplementation(m, (IMP)classForwardingTarget);
    } else if (!m)
        class_addMethod(meta, s, (IMP)classForwardingTarget, "@@::");
}
