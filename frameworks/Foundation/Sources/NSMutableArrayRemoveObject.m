// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// -[NSMutableArray removeObject:] and removeObject:inRange: keep the object alive until they return. Reef's
// implementation compares the remaining elements with the object after removing (and so releasing) a match: when
// the array held the last reference — removing an element found by enumerating the array, as GarageBand does —
// the comparisons message a freed object. Apple's removes the object safely.
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

static IMP originalRemove, originalRemoveInRange;

static void removeObject(id self, SEL _cmd, id object) {
    if (object) CFRetain((__bridge CFTypeRef)object);   // alive for the duration of the call
    ((void (*)(id, SEL, id))originalRemove)(self, _cmd, object);
    if (object) CFRelease((__bridge CFTypeRef)object);
}

static void removeObjectInRange(id self, SEL _cmd, id object, NSRange range) {
    if (object) CFRetain((__bridge CFTypeRef)object);
    ((void (*)(id, SEL, id, NSRange))originalRemoveInRange)(self, _cmd, object, range);
    if (object) CFRelease((__bridge CFTypeRef)object);
}

__attribute__((constructor)) static void keepRemovedObject(void) {
    Method m = class_getInstanceMethod([NSMutableArray class], @selector(removeObject:));
    if (m) originalRemove = method_setImplementation(m, (IMP)removeObject);
    m = class_getInstanceMethod([NSMutableArray class], @selector(removeObject:inRange:));
    if (m) originalRemoveInRange = method_setImplementation(m, (IMP)removeObjectInRange);
}
