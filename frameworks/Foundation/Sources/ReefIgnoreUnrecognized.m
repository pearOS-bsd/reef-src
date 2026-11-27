// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// A bring-up aid, off unless REEF_IGNORE_UNRECOGNIZED is set in the environment: a message that no class
// implements (and that the receiver does not forward) answers zero instead of raising
// NSInvalidArgumentException, and is printed once ("reef: ignored -[Class selector]"). It lets an
// application run past methods Reef does not have yet, so they can be listed; Apple's runtime raises.

#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import <stdlib.h>
#import <string.h>

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
    // the class that key-value observing made for an observed object stands for its superclass
    Class own = cls;
    if (!isClass && !strncmp(class_getName(cls), "NSKVONotifying_", 15)) own = class_getSuperclass(cls);
    // Receivers that can forward the message (they have its signature) keep doing so
    (void)own;
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

// Receivers that answer forwardingTargetForSelector: themselves reach the forwarding with no signature: they get one
// ("@@:", the arguments are not looked at) and NSObject's forwardInvocation: answers zero for those messages.
static IMP originalSignature, originalClassSignature, originalForward, originalClassForward;
static NSMutableSet *faked;

static void printIgnored(id self, SEL sel) {
    Class cls = object_getClass(self);
    BOOL isClass = class_isMetaClass(cls);
    @synchronized ([ReefIgnoredMessageSink class]) {
        static NSMutableSet *printed;
        if (!printed) printed = [NSMutableSet set];
        NSString *key = [NSString stringWithFormat:@"%c[%s %s]", isClass ? '+' : '-', class_getName(isClass ? (Class)self : cls), sel_getName(sel)];
        if (![printed containsObject:key]) {
            [printed addObject:key];
            fprintf(stderr, "reef: ignored %s\n", key.UTF8String);
        }
    }
}

static NSMethodSignature *signatureFor(id self, SEL _cmd, SEL sel, IMP original) {
    NSMethodSignature *s = ((NSMethodSignature * (*)(id, SEL, SEL))original)(self, _cmd, sel);
    if (s) return s;
    @synchronized ([ReefIgnoredMessageSink class]) { [faked addObject:NSStringFromSelector(sel)]; }
    return [NSMethodSignature signatureWithObjCTypes:"@@:"];
}
static NSMethodSignature *instanceSignature(id self, SEL _cmd, SEL sel) { return signatureFor(self, _cmd, sel, originalSignature); }
static NSMethodSignature *classSignature(id self, SEL _cmd, SEL sel) { return signatureFor(self, _cmd, sel, originalClassSignature); }

static void forwardFor(id self, SEL _cmd, NSInvocation *invocation, IMP original) {
    BOOL ours;
    @synchronized ([ReefIgnoredMessageSink class]) { ours = [faked containsObject:NSStringFromSelector(invocation.selector)]; }
    if (!ours) { ((void (*)(id, SEL, NSInvocation *))original)(self, _cmd, invocation); return; }
    printIgnored(self, invocation.selector);
    id zero = nil;
    [invocation setReturnValue:&zero];
}
static void instanceForward(id self, SEL _cmd, NSInvocation *i) { forwardFor(self, _cmd, i, originalForward); }
static void classForward(id self, SEL _cmd, NSInvocation *i) { forwardFor(self, _cmd, i, originalClassForward); }

static void swizzle(Class c, SEL s, IMP replacement, IMP *original) {
    Method m = class_getInstanceMethod(c, s);
    if (m) *original = method_setImplementation(m, replacement);
}

__attribute__((constructor)) static void installIgnoreUnrecognized(void) {
    if (!getenv("REEF_IGNORE_UNRECOGNIZED"))
        return;
    fprintf(stderr, "reef: unrecognized messages answer zero (REEF_IGNORE_UNRECOGNIZED)\n");
    sink = [ReefIgnoredMessageSink new];
    faked = [NSMutableSet set];
    Class root = [NSObject class], rootMeta = object_getClass(root);
    swizzle(root, @selector(methodSignatureForSelector:), (IMP)instanceSignature, &originalSignature);
    swizzle(root, @selector(forwardInvocation:), (IMP)instanceForward, &originalForward);
    // class methods of NSObject (the root metaclass has its own when the runtime defines them)
    Method cm = class_getClassMethod(root, @selector(methodSignatureForSelector:));
    if (cm && method_getImplementation(cm) != (IMP)instanceSignature) originalClassSignature = method_setImplementation(cm, (IMP)classSignature);
    cm = class_getClassMethod(root, @selector(forwardInvocation:));
    if (cm && method_getImplementation(cm) != (IMP)instanceForward) originalClassForward = method_setImplementation(cm, (IMP)classForward);
    (void)rootMeta;
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

// Also part of the aid: attributes set on a range outside the string are left out (and printed) instead of raising.
static IMP originalSetAttributes, originalAddAttributes, originalAddAttribute;
static BOOL rangeFits(NSAttributedString *s, NSRange r, SEL sel) {
    if (r.location <= s.length && r.length <= s.length - r.location) return YES;
    fprintf(stderr, "reef: ignored %s %s outside \"%s\" (%lu)\n", sel_getName(sel), NSStringFromRange(r).UTF8String,
            s.string.UTF8String, (unsigned long)s.length);
    return NO;
}
static void setAttributesRange(NSMutableAttributedString *self, SEL _cmd, NSDictionary *a, NSRange r) {
    if (rangeFits(self, r, _cmd)) ((void (*)(id, SEL, id, NSRange))originalSetAttributes)(self, _cmd, a, r);
}
static void addAttributesRange(NSMutableAttributedString *self, SEL _cmd, NSDictionary *a, NSRange r) {
    if (rangeFits(self, r, _cmd)) ((void (*)(id, SEL, id, NSRange))originalAddAttributes)(self, _cmd, a, r);
}
static void addAttributeRange(NSMutableAttributedString *self, SEL _cmd, NSString *n, id v, NSRange r) {
    if (rangeFits(self, r, _cmd)) ((void (*)(id, SEL, id, id, NSRange))originalAddAttribute)(self, _cmd, n, v, r);
}
__attribute__((constructor)) static void installAttributeRanges(void) {
    if (!getenv("REEF_IGNORE_UNRECOGNIZED")) return;
    Class c = [NSMutableAttributedString class];
    swizzle(c, @selector(setAttributes:range:), (IMP)setAttributesRange, &originalSetAttributes);
    swizzle(c, @selector(addAttributes:range:), (IMP)addAttributesRange, &originalAddAttributes);
    swizzle(c, @selector(addAttribute:value:range:), (IMP)addAttributeRange, &originalAddAttribute);
}
