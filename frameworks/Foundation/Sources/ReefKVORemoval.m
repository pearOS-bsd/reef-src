// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Removing a key-value observer that is not registered raises on macOS, a programming error that apps never reach there.
// Reef's key-value observing reports a change of a to-many property as a change of the whole array (kind setting, the
// old and new arrays) where macOS reports the inserted, removed or replaced elements; apps that keep observers on the
// elements in step with those changes (QuickTime Player's media clips) can then remove an observer from an element it
// never got. Such a removal is ignored here instead of ending the app.
// macOS also keeps an observer alive while it is being notified; Reef's does not, so an observer that removes itself and
// is released from its own notification (a block-invoking observer whose block lets go of it) would be freed while its
// code still runs. A removed observer is therefore kept until the autorelease pool of the current run loop pass drains.

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

static IMP originalRemoveWithContext, originalRemove;

static BOOL IsNotRegistered(NSException *e)
{
    return [e.name isEqualToString:NSRangeException] && [e.reason rangeOfString:@"not registered"].location != NSNotFound;
}

static void RemoveWithContext(id self, SEL _cmd, NSObject *observer, NSString *keyPath, void *context)
{
    if (observer)
        CFAutorelease(CFRetain((__bridge CFTypeRef)observer));
    @try {
        ((void (*)(id, SEL, id, id, void *))originalRemoveWithContext)(self, _cmd, observer, keyPath, context);
    } @catch (NSException *e) {
        if (!IsNotRegistered(e))
            @throw;
    }
}

static void Remove(id self, SEL _cmd, NSObject *observer, NSString *keyPath)
{
    if (observer)
        CFAutorelease(CFRetain((__bridge CFTypeRef)observer));
    @try {
        ((void (*)(id, SEL, id, id))originalRemove)(self, _cmd, observer, keyPath);
    } @catch (NSException *e) {
        if (!IsNotRegistered(e))
            @throw;
    }
}

__attribute__((constructor)) static void ReefInstallLenientRemoval(void)
{
    Class c = [NSObject class];
    Method m = class_getInstanceMethod(c, @selector(removeObserver:forKeyPath:context:));
    if (m)
        originalRemoveWithContext = method_setImplementation(m, (IMP)RemoveWithContext);
    m = class_getInstanceMethod(c, @selector(removeObserver:forKeyPath:));
    if (m)
        originalRemove = method_setImplementation(m, (IMP)Remove);
}
