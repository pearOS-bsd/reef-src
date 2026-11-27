// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Small functions of Reef's CoreFoundation: the uncaught exception handler slot, forced preferences (none), weak reference flag.
#include "CFInternal.h"
#include <objc/runtime.h>
#include <stdio.h>
#include <stdlib.h>

typedef void NSUncaughtExceptionHandler(void *exception);

extern NSString *const NSGenericException, *const NSInvalidArgumentException, *const NSRangeException, *const NSInternalInconsistencyException;
extern const char *object_getClassName(id object);
extern const char *sel_getName(SEL selector);
extern void objc_setEnumerationMutationHandler(void (*handler)(id));
extern void objc_setExceptionPreprocessor(id (*preprocessor)(id));
extern void objc_setUncaughtExceptionHandler(void (*handler)(id));
extern void reef_report_unsupported(const char *what);

@interface NSException (ReefRaise)
+ (void)raise:(id)name format:(id)format, ...;
- (id)callStackSymbols;
- (void)_recordCallStack;
@end

static NSUncaughtExceptionHandler *uncaughtHandler;

NSUncaughtExceptionHandler *NSGetUncaughtExceptionHandler(void) {
    return uncaughtHandler;
}

void NSSetUncaughtExceptionHandler(NSUncaughtExceptionHandler *handler) {
    uncaughtHandler = handler;
}

Boolean CFPreferencesAppValueIsForced(CFStringRef key, CFStringRef applicationID) {
    return false;
}

void _CFSetWeaklyReferenced(CFTypeRef cf) {
    CFRuntimeBase *base = (CFRuntimeBase *)cf;
    if ((uint64_t)base->_cfinfoa >> 32 != 0) {
        __c11_atomic_fetch_or(&base->_cfinfoa, 0x100000, __ATOMIC_SEQ_CST);
    }
}

// The handler of the Objective-C runtime for a collection changed while it is enumerated.
static void mutatedWhileEnumerating(id collection) {
    [NSException raise:NSGenericException format:CFSTR("*** Collection <%s: %p> was mutated while being enumerated."), object_getClassName(collection), collection];
}

// The exception preprocessor: an exception remembers the stack of the first throw.
static id recordCallStack(id exception) {
    if ([exception isKindOfClass:[NSException class]]) [exception _recordCallStack];
    return exception;
}

static char reportMissingMethodBuffer[0x4000];

// The uncaught exception handler of the runtime: the report of an uncaught exception, as on a Mac, and abort.  A message to an object
// that does not implement the method is reported to the host first.
static void terminate(id exception) {
    char line[2048];
    if (![exception isKindOfClass:[NSException class]]) {
        fprintf(stderr, "*** Terminating app due to uncaught exception of class '%s'\n", object_getClassName(exception));
        abort();
    }
    if (uncaughtHandler) uncaughtHandler(exception);
    CFAllocatorRef allocator = kCFAllocatorSystemDefault;
    NSException *e = (NSException *)exception;
    CFStringRef message = CFStringCreateWithFormat(allocator, NULL, CFSTR("*** Terminating app due to uncaught exception '%@', reason: '%@'"), e.name, e.reason);
    if (CFStringGetCString(message, line, sizeof(line), kCFStringEncodingUTF8)) fprintf(stderr, "%s\n", line);
    CFRelease(message);
    if (CFStringGetCString(CFSTR("*** First throw call stack:"), line, sizeof(line), kCFStringEncodingUTF8)) fprintf(stderr, "%s\n", line);
    CFArrayRef stack = (CFArrayRef)[exception callStackSymbols];
    for (CFIndex idx = 0; idx < CFArrayGetCount(stack); idx++) {
        if (CFStringGetCString((CFStringRef)CFArrayGetValueAtIndex(stack, idx), line, sizeof(line), kCFStringEncodingUTF8)) fprintf(stderr, "%s\n", line);
    }
    fwrite("terminating due to uncaught exception of type NSException\n", 0x3a, 1, stderr);
    CFStringRef name = (CFStringRef)e.name, reason = (CFStringRef)e.reason;
    if (name && reason && CFEqual(name, (CFStringRef)NSInvalidArgumentException) && CFStringFind(reason, CFSTR("unrecognized selector sent to"), 0).location != kCFNotFound) {
        CFMutableStringRef report = CFStringCreateMutable(allocator, 0);
        CFStringAppend(report, reason);
        CFStringAppend(report, CFSTR("\n\nFirst throw call stack:"));
        for (CFIndex idx = 0; idx < CFArrayGetCount(stack); idx++) {
            CFStringAppend(report, CFSTR("\n%s"));
            CFStringAppend(report, (CFStringRef)CFArrayGetValueAtIndex(stack, idx));
        }
        if (!CFStringGetCString(report, reportMissingMethodBuffer, sizeof(reportMissingMethodBuffer), kCFStringEncodingUTF8)) {
            CFRelease(report);
            report = CFStringCreateMutableCopy(allocator, 0, (CFStringRef)e.reason);
            CFStringGetCString(report, reportMissingMethodBuffer, sizeof(reportMissingMethodBuffer), kCFStringEncodingUTF8);
        }
        CFRelease(report);
        reef_report_unsupported(reportMissingMethodBuffer);
    }
    abort();
}

CF_PRIVATE void __CFExceptionInitialize(void) {
    objc_setEnumerationMutationHandler(mutatedWhileEnumerating);
    objc_setExceptionPreprocessor(recordCallStack);
    objc_setUncaughtExceptionHandler(terminate);
}

// The exceptions of the collection classes that cannot go on.
CF_PRIVATE void __reefRaiseImmutable(id object, SEL selector) {
    [NSException raise:NSInternalInconsistencyException format:CFSTR("-[%s %s]: mutating method sent to immutable object"), object_getClassName(object), sel_getName(selector)];
    __builtin_trap();
}

CF_PRIVATE void __reefRaiseNil(id object, SEL selector, const char *what) {
    [NSException raise:NSInvalidArgumentException format:CFSTR("*** -[%s %s]: %s cannot be nil"), object_getClassName(object), sel_getName(selector), what];
    __builtin_trap();
}

CF_PRIVATE void __reefRaiseIndex(id object, SEL selector, unsigned long index, unsigned long count) {
    if (count == 0) [NSException raise:NSRangeException format:CFSTR("*** -[%s %s]: index %lu beyond bounds for empty array"), object_getClassName(object), sel_getName(selector), index];
    [NSException raise:NSRangeException format:CFSTR("*** -[%s %s]: index %lu beyond bounds [0 .. %lu]"), object_getClassName(object), sel_getName(selector), index, count - 1];
    __builtin_trap();
}

// The forward handler of the runtime (NSInvocation based forwarding of unknown messages) is installed here once its entries are written.
static void __CFForwardingInitialize(void) {
}

// Connects the type identifiers of the Core Foundation types to the Objective-C classes of their objects (the class an object of the type
// has as its isa), after the forwarding and the exception handlers of the runtime: every type (1024 slots) is an __NSCFType first.
CF_PRIVATE void __CFBridgeInitialize(void) {
    static const struct { CFTypeID typeID; const char *className; } classes[] = {
        {7, "__NSCFString"}, {19, "__NSCFArray"}, {18, "__NSCFDictionary"}, {17, "__NSCFSet"}, {20, "__NSCFData"}, {22, "__NSCFNumber"},
        {21, "__NSCFBoolean"}, {16, "NSNull"}, {25, "__NSCFCharacterSet"}, {27, "__NSCFError"}, {42, "__NSCFDate"}, {29, "NSURL"},
        {47, "__NSCFTimer"}, {38, "__NSCFInputStream"}, {39, "__NSCFOutputStream"}, {55, "__NSCFLocale"}, {48, "__NSCFTimeZone"},
        {49, "__NSCFCalendar"}, {62, "__NSCFAttributedString"},
    };
    __CFForwardingInitialize();
    __CFExceptionInitialize();
    const void *type = objc_getClass("__NSCFType");
    for (CFTypeID idx = 0; idx < 1024; idx++) _CFRuntimeBridgeTypeToClass(idx, type);
    for (CFIndex idx = 0; idx < sizeof(classes) / sizeof(classes[0]); idx++) _CFRuntimeBridgeTypeToClass(classes[idx].typeID, objc_getClass(classes[idx].className));
}
