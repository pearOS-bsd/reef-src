// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Old style dependent keys of key-value observing, as in Apple's Foundation (macOS 26, see EVIDENCE.md):
// +setKeys:triggerChangeNotificationsForDependentKey: stores, per class (not inherited), the set of keys
// for a dependent key; +_keysForValuesAffectingValueForKey: answers them without the key paths (keys with
// a dot are left out); +keyPathsForValuesAffectingValueForKey: asks +keyPathsForValuesAffecting<Key> and
// falls back on them. Reef's Foundation has the last one without the fallback: its implementation is
// replaced when this library is loaded.

#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import <os/lock.h>
#import <string.h>
#import <ctype.h>

static os_unfair_lock dependenciesLock = OS_UNFAIR_LOCK_INIT;
static CFMutableDictionaryRef dependenciesByClass;   // class -> (dependent key -> NSSet of keys)

@implementation NSObject (NSDeprecatedKeyValueObservingCustomization)
+ (void)setKeys:(NSArray *)keys triggerChangeNotificationsForDependentKey:(NSString *)dependentKey {
    os_unfair_lock_lock(&dependenciesLock);
    NSSet *set = [[NSSet alloc] initWithArray:keys];
    if (!dependenciesByClass)
        dependenciesByClass = CFDictionaryCreateMutable(NULL, 0, NULL, &kCFTypeDictionaryValueCallBacks);
    CFMutableDictionaryRef byKey = (CFMutableDictionaryRef)CFDictionaryGetValue(dependenciesByClass, (__bridge const void *)self);
    if (!byKey) {
        byKey = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        CFDictionarySetValue(dependenciesByClass, (__bridge const void *)self, byKey);
        CFRelease(byKey);
    }
    CFDictionarySetValue(byKey, (__bridge const void *)dependentKey, (__bridge const void *)set);
    os_unfair_lock_unlock(&dependenciesLock);
}
@end

@interface NSObject (NSKeyValueObservingOldStyleDependencies)
+ (NSSet *)_keysForValuesAffectingValueForKey:(NSString *)key;
@end

@implementation NSObject (NSKeyValueObservingOldStyleDependencies)
+ (NSSet *)_keysForValuesAffectingValueForKey:(NSString *)key {
    os_unfair_lock_lock(&dependenciesLock);
    NSSet *keys = nil;
    if (dependenciesByClass) {
        CFDictionaryRef byKey = CFDictionaryGetValue(dependenciesByClass, (__bridge const void *)[self class]);
        if (byKey)
            keys = (__bridge NSSet *)CFDictionaryGetValue(byKey, (__bridge const void *)key);
    }
    os_unfair_lock_unlock(&dependenciesLock);
    NSMutableSet *result = [NSMutableSet set];
    for (NSString *k in keys)
        if ([k rangeOfString:@"."].length == 0)
            [result addObject:k];
    return result;
}
@end

static NSSet *keyPathsForValuesAffectingValueForKey(Class self, SEL _cmd, NSString *key) {
    const NSUInteger length = [key lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
    char *utf8 = malloc(length + 1);
    [key getCString:utf8 maxLength:length + 1 encoding:NSUTF8StringEncoding];
    if (memchr(utf8, 0, length)) {
        free(utf8);
        @throw [NSException exceptionWithName:NSInvalidArgumentException
                                       reason:@"Attempted to use a key or key path whose UTF-8 representation contains a NUL character"
                                     userInfo:nil];
    }
    if (key.length)
        utf8[0] = (char)toupper((unsigned char)utf8[0]);
    char *name = malloc(strlen(utf8) + sizeof "keyPathsForValuesAffecting");
    snprintf(name, strlen(utf8) + sizeof "keyPathsForValuesAffecting", "keyPathsForValuesAffecting%s", utf8);
    Method method = class_getClassMethod(self, sel_registerName(name));
    free(name);
    free(utf8);
    if (method)
        return ((NSSet * (*)(id, SEL))method_getImplementation(method))(self, method_getName(method));
    return [self _keysForValuesAffectingValueForKey:key];
}

__attribute__((constructor)) static void installKeyValueObservingDependencies(void) {
    Method method = class_getClassMethod([NSObject class], @selector(keyPathsForValuesAffectingValueForKey:));
    if (method)
        method_setImplementation(method, (IMP)keyPathsForValuesAffectingValueForKey);
}
