// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Two differences of Reef's key-value observing from macOS that apps run into:
// - macOS keeps an observer alive while it is being notified; Reef's does not, so an observer that removes itself and is
//   released from its own notification (a block-invoking observer whose block lets go of it) is freed while its code
//   still runs. Every class that registers as an observer gets its observeValueForKeyPath:ofObject:change:context:
//   wrapped so that the observer is retained for the duration of the call, as on macOS.
// - Removing an observer that is not registered raises on macOS, a programming error that apps never reach there. Reef
//   reports a change of a to-many property as a change of the whole array (kind setting, the old and new arrays) where
//   macOS reports the inserted, removed or replaced elements; apps that keep observers on the elements in step with
//   those changes (QuickTime Player's media clips) can then remove an observer from an element it never got. Such a
//   removal is ignored here instead of ending the app.

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

static IMP originalRemoveWithContext, originalRemove, originalAdd;

static BOOL IsNotRegistered(NSException *e)
{
    return [e.name isEqualToString:NSRangeException] && [e.reason rangeOfString:@"not registered"].location != NSNotFound;
}

static void RemoveWithContext(id self, SEL _cmd, NSObject *observer, NSString *keyPath, void *context)
{
    @try {
        ((void (*)(id, SEL, id, id, void *))originalRemoveWithContext)(self, _cmd, observer, keyPath, context);
    } @catch (NSException *e) {
        if (!IsNotRegistered(e))
            @throw;
    }
}

static void Remove(id self, SEL _cmd, NSObject *observer, NSString *keyPath)
{
    @try {
        ((void (*)(id, SEL, id, id))originalRemove)(self, _cmd, observer, keyPath);
    } @catch (NSException *e) {
        if (!IsNotRegistered(e))
            @throw;
    }
}

typedef void (*ObserveFunction)(id, SEL, NSString *, id, NSDictionary *, void *);

// The observer's class answers its notifications with the observer retained (once per class).
static void KeepObserversAlive(Class c)
{
    static NSMutableSet *wrapped;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ wrapped = [NSMutableSet set]; });
    SEL sel = @selector(observeValueForKeyPath:ofObject:change:context:);
    @synchronized(wrapped) {
        if ([wrapped containsObject:c])
            return;
        [wrapped addObject:c];
        Method m = class_getInstanceMethod(c, sel);
        if (!m)
            return;
        ObserveFunction original = (ObserveFunction)method_getImplementation(m);
        IMP wrapper = imp_implementationWithBlock(^(id observer, NSString *keyPath, id object, NSDictionary *change, void *context) {
            CFTypeRef kept = CFRetain((__bridge CFTypeRef)observer);
            original(observer, sel, keyPath, object, change, context);
            CFRelease(kept);
        });
        class_replaceMethod(c, sel, wrapper, method_getTypeEncoding(m));
    }
}

static void Add(id self, SEL _cmd, NSObject *observer, NSString *keyPath, NSKeyValueObservingOptions options, void *context)
{
    if (observer)
        KeepObserversAlive(object_getClass(observer));
    ((void (*)(id, SEL, id, id, NSKeyValueObservingOptions, void *))originalAdd)(self, _cmd, observer, keyPath, options, context);
}

__attribute__((constructor)) static void ReefInstallKVOCompatibility(void)
{
    Class c = [NSObject class];
    Method m = class_getInstanceMethod(c, @selector(removeObserver:forKeyPath:context:));
    if (m)
        originalRemoveWithContext = method_setImplementation(m, (IMP)RemoveWithContext);
    m = class_getInstanceMethod(c, @selector(removeObserver:forKeyPath:));
    if (m)
        originalRemove = method_setImplementation(m, (IMP)Remove);
    m = class_getInstanceMethod(c, @selector(addObserver:forKeyPath:options:context:));
    if (m)
        originalAdd = method_setImplementation(m, (IMP)Add);
}
