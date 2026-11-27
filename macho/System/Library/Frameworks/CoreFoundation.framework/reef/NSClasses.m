// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoreFoundation.

#import "NSBase.h"

// The info word of a Core Foundation object (the second word): the lowest bits tell mutable from immutable collections and strings.
#define REEF_INFO(object) (__c11_atomic_load((_Atomic(unsigned long) *)((char *)(object) + 8), __ATOMIC_RELAXED))

@class NSDictionary, NSString, CacheEntry, ReefFrame, ReefSignature;

extern long reefCall_CFArrayGetCount(long) __asm__("_CFArrayGetCount");
extern long reefCall_CFArrayGetTypeID(void) __asm__("_CFArrayGetTypeID");
extern long reefCall_CFAttributedStringBeginEditing(long) __asm__("_CFAttributedStringBeginEditing");
extern long reefCall_CFAttributedStringEndEditing(long) __asm__("_CFAttributedStringEndEditing");
extern long reefCall_CFAttributedStringGetLength(long) __asm__("_CFAttributedStringGetLength");
extern long reefCall_CFAttributedStringGetString(long) __asm__("_CFAttributedStringGetString");
extern long reefCall_CFCalendarGetTypeID(void) __asm__("_CFCalendarGetTypeID");
extern long reefCall_CFCharacterSetAddCharactersInString(long, long) __asm__("_CFCharacterSetAddCharactersInString");
extern long reefCall_CFCharacterSetGetTypeID(void) __asm__("_CFCharacterSetGetTypeID");
extern long reefCall_CFCharacterSetIntersect(long, long) __asm__("_CFCharacterSetIntersect");
extern long reefCall_CFCharacterSetInvert(long) __asm__("_CFCharacterSetInvert");
extern long reefCall_CFCharacterSetRemoveCharactersInString(long, long) __asm__("_CFCharacterSetRemoveCharactersInString");
extern long reefCall_CFCharacterSetUnion(long, long) __asm__("_CFCharacterSetUnion");
extern long reefCall_CFDataAppendBytes(long, long, long) __asm__("_CFDataAppendBytes");
extern long reefCall_CFDataGetBytePtr(long) __asm__("_CFDataGetBytePtr");
extern long reefCall_CFDataGetLength(long) __asm__("_CFDataGetLength");
extern long reefCall_CFDataGetMutableBytePtr(long) __asm__("_CFDataGetMutableBytePtr");
extern long reefCall_CFDataGetTypeID(void) __asm__("_CFDataGetTypeID");
extern long reefCall_CFDataIncreaseLength(long, long) __asm__("_CFDataIncreaseLength");
extern long reefCall_CFDataSetLength(long, long) __asm__("_CFDataSetLength");
extern long reefCall_CFDateGetTypeID(void) __asm__("_CFDateGetTypeID");
extern long reefCall_CFDictionaryGetCount(long) __asm__("_CFDictionaryGetCount");
extern long reefCall_CFDictionaryGetCountOfKey(long, long) __asm__("_CFDictionaryGetCountOfKey");
extern long reefCall_CFDictionaryGetCountOfValue(long, long) __asm__("_CFDictionaryGetCountOfValue");
extern long reefCall_CFDictionaryGetKeysAndValues(long, long, long) __asm__("_CFDictionaryGetKeysAndValues");
extern long reefCall_CFDictionaryGetTypeID(void) __asm__("_CFDictionaryGetTypeID");
extern long reefCall_CFErrorGetCode(long) __asm__("_CFErrorGetCode");
extern long reefCall_CFErrorGetDomain(long) __asm__("_CFErrorGetDomain");
extern long reefCall_CFErrorGetTypeID(void) __asm__("_CFErrorGetTypeID");
extern long reefCall_CFGetRetainCount(long) __asm__("_CFGetRetainCount");
extern long reefCall_CFGetTypeID(long) __asm__("_CFGetTypeID");
extern long reefCall_CFHash(long) __asm__("_CFHash");
extern long reefCall_CFLocaleGetIdentifier(long) __asm__("_CFLocaleGetIdentifier");
extern long reefCall_CFLocaleGetTypeID(void) __asm__("_CFLocaleGetTypeID");
extern long reefCall_CFLocaleGetValue(long, long) __asm__("_CFLocaleGetValue");
extern long reefCall_CFNumberCompare(long, long, long) __asm__("_CFNumberCompare");
extern long reefCall_CFNumberGetTypeID(void) __asm__("_CFNumberGetTypeID");
extern long reefCall_CFReadStreamClose(long) __asm__("_CFReadStreamClose");
extern long reefCall_CFReadStreamGetError(long) __asm__("_CFReadStreamGetError");
extern long reefCall_CFReadStreamGetStatus(long) __asm__("_CFReadStreamGetStatus");
extern long reefCall_CFReadStreamGetTypeID(void) __asm__("_CFReadStreamGetTypeID");
extern long reefCall_CFReadStreamOpen(long) __asm__("_CFReadStreamOpen");
extern long reefCall_CFReadStreamRead(long, long, long) __asm__("_CFReadStreamRead");
extern long reefCall_CFRelease(long) __asm__("_CFRelease");
extern long reefCall_CFRetain(long) __asm__("_CFRetain");
extern long reefCall_CFRunLoopTimerGetTypeID(void) __asm__("_CFRunLoopTimerGetTypeID");
extern long reefCall_CFRunLoopTimerInvalidate(long) __asm__("_CFRunLoopTimerInvalidate");
extern long reefCall_CFSetGetCount(long) __asm__("_CFSetGetCount");
extern long reefCall_CFSetGetCountOfValue(long, long) __asm__("_CFSetGetCountOfValue");
extern long reefCall_CFSetGetTypeID(void) __asm__("_CFSetGetTypeID");
extern long reefCall_CFSetGetValues(long, long) __asm__("_CFSetGetValues");
extern long reefCall_CFStringAppend(long, long) __asm__("_CFStringAppend");
extern long reefCall_CFStringGetCharacterAtIndex(long, long) __asm__("_CFStringGetCharacterAtIndex");
extern long reefCall_CFStringGetLength(long) __asm__("_CFStringGetLength");
extern long reefCall_CFStringGetTypeID(void) __asm__("_CFStringGetTypeID");
extern long reefCall_CFStringReplaceAll(long, long) __asm__("_CFStringReplaceAll");
extern long reefCall_CFTimeZoneGetData(long) __asm__("_CFTimeZoneGetData");
extern long reefCall_CFTimeZoneGetName(long) __asm__("_CFTimeZoneGetName");
extern long reefCall_CFTimeZoneGetTypeID(void) __asm__("_CFTimeZoneGetTypeID");
extern long reefCall_CFURLGetBaseURL(long) __asm__("_CFURLGetBaseURL");
extern long reefCall_CFURLGetString(long) __asm__("_CFURLGetString");
extern long reefCall_CFWriteStreamClose(long) __asm__("_CFWriteStreamClose");
extern long reefCall_CFWriteStreamGetError(long) __asm__("_CFWriteStreamGetError");
extern long reefCall_CFWriteStreamGetStatus(long) __asm__("_CFWriteStreamGetStatus");
extern long reefCall_CFWriteStreamGetTypeID(void) __asm__("_CFWriteStreamGetTypeID");
extern long reefCall_CFWriteStreamOpen(long) __asm__("_CFWriteStreamOpen");
extern long reefCall_CFWriteStreamWrite(long, long, long) __asm__("_CFWriteStreamWrite");
extern long reefCall__CFNumberGetType2(long) __asm__("__CFNumberGetType2");
extern long reefCall__CFSetWeaklyReferenced(long) __asm__("__CFSetWeaklyReferenced");
extern long reefCall__CFIsDeallocating(long) __asm__("__CFIsDeallocating");
extern long reefCall__CFTryRetain(long) __asm__("__CFTryRetain");
extern long reefCall_CFEqual(long, long) __asm__("_CFEqual");
@interface NSException : NSObject
{
    NSString * _name;
    NSString * _reason;
    NSDictionary * _userInfo;
    struct __CFArray * _callStack;
}
+ (id)exceptionWithName:(id)a0 reason:(id)a1 userInfo:(id)a2;
+ (void)raise:(id)a0 format:(id)a1, ...;
+ (void)raise:(id)a0 format:(id)a1 arguments:(void *)a2;
- (id)initWithName:(id)a0 reason:(id)a1 userInfo:(id)a2;
- (void)dealloc;
- (void)raise;
- (id)name;
- (id)reason;
- (id)userInfo;
- (void)_recordCallStack;
- (id)callStackReturnAddresses;
- (id)callStackSymbols;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)description;
@end
extern void __reefRaiseImmutable(id, SEL) __attribute__((noreturn));
extern void __reefRaiseNil(id, SEL, const char *) __attribute__((noreturn));
extern void __reefRaiseIndex(id, SEL, unsigned long, unsigned long) __attribute__((noreturn));
extern unsigned long _CFArrayFastEnumeration(id, NSFastEnumerationState *, id *, unsigned long);
extern unsigned long _CFDictionaryFastEnumeration(id, NSFastEnumerationState *, id *, unsigned long);
extern unsigned long _CFSetFastEnumeration(id, NSFastEnumerationState *, id *, unsigned long);
extern id objc_autorelease(id);
extern id objc_retain(id);
extern void objc_release(id);
extern long _CFNumberGetType2(CFNumberRef);
extern const char *object_getClassName(id);
extern NSString *const NSRangeException;


@interface __NSCFType : NSObject
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (id)description;
@end

@implementation __NSCFType
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (id)description
{
    id result = (id)CFCopyDescription((CFTypeRef)self);
    return result ? objc_autorelease(result) : nil;
}

@end

@interface NSEnumerator : NSObject
- (id)nextObject;
- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2;
@end

@implementation NSEnumerator
- (id)nextObject
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2
{
    // unresolved
    return 0;
}

@end

@interface __NSCFSnapshotEnumerator : NSEnumerator
{
    struct __CFArray * _objects;
    long _next;
}
- (id)initWithSnapshot:(struct __CFArray *)a0;
- (void)dealloc;
- (id)nextObject;
@end

@implementation __NSCFSnapshotEnumerator
- (id)initWithSnapshot:(struct __CFArray *)a0
{
    // unresolved
    return nil;
}

- (void)dealloc
{
    // unresolved

}

- (id)nextObject
{
    // unresolved
    return nil;
}

@end

@interface NSNull : NSObject
+ (id)null;
+ (id)allocWithZone:(struct _NSZone *)a0;
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (id)description;
@end

@implementation NSNull
+ (id)null
{
    // unresolved
    return nil;
}

+ (id)allocWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (id)description
{
    return (id)CFSTR("<null>");
}

@end

@interface NSCache : NSObject
{
    void * _lock;
    struct __CFDictionary * _entries;
    struct CacheEntry * _newest;
    struct CacheEntry * _oldest;
    unsigned long _totalCost;
    unsigned long _totalCostLimit;
    unsigned long _countLimit;
    BOOL _evictsObjectsWithDiscardedContent;
    NSString * _name;
    id _delegate;
}
- (id)init;
- (void)dealloc;
- (id)objectForKey:(id)a0;
- (void)setObject:(id)a0 forKey:(id)a1;
- (void)setObject:(id)a0 forKey:(id)a1 cost:(unsigned long)a2;
- (void)removeObjectForKey:(id)a0;
- (void)removeAllObjects;
- (unsigned long)totalCostLimit;
- (void)setTotalCostLimit:(unsigned long)a0;
- (unsigned long)countLimit;
- (void)setCountLimit:(unsigned long)a0;
- (BOOL)evictsObjectsWithDiscardedContent;
- (void)setEvictsObjectsWithDiscardedContent:(BOOL)a0;
- (id)name;
- (void)setName:(id)a0;
- (id)delegate;
- (void)setDelegate:(id)a0;
@end

@implementation NSCache
- (id)init
{
    // unresolved
    return nil;
}

- (void)dealloc
{
    // unresolved

}

- (id)objectForKey:(id)a0
{
    // unresolved
    return nil;
}

- (void)setObject:(id)a0 forKey:(id)a1
{
    // unresolved

}

- (void)setObject:(id)a0 forKey:(id)a1 cost:(unsigned long)a2
{
    // unresolved

}

- (void)removeObjectForKey:(id)a0
{
    // unresolved

}

- (void)removeAllObjects
{
    // unresolved

}

- (unsigned long)totalCostLimit
{
    return _totalCostLimit;
}

- (void)setTotalCostLimit:(unsigned long)a0
{
    // unresolved

}

- (unsigned long)countLimit
{
    return _countLimit;
}

- (void)setCountLimit:(unsigned long)a0
{
    // unresolved

}

- (BOOL)evictsObjectsWithDiscardedContent
{
    return _evictsObjectsWithDiscardedContent;
}

- (void)setEvictsObjectsWithDiscardedContent:(BOOL)a0
{
    _evictsObjectsWithDiscardedContent = a0;
}

- (id)name
{
    // unresolved
    return nil;
}

- (void)setName:(id)a0
{
    // unresolved

}

- (id)delegate
{
    return _delegate;
}

- (void)setDelegate:(id)a0
{
    // unresolved

}

@end

@interface NSArray : NSObject
- (unsigned long)count;
- (id)objectAtIndex:(unsigned long)a0;
- (void)getObjects:(id *)a0 range:(NSRange)a1;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)mutableCopyWithZone:(struct _NSZone *)a0;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2;
@end

@implementation NSArray
- (unsigned long)count
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)objectAtIndex:(unsigned long)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (void)getObjects:(id *)a0 range:(NSRange)a1
{
    // unresolved

}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)mutableCopyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    return [self count];
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFArrayGetTypeID();
}

- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2
{
    // unresolved
    return 0;
}

@end

@interface NSMutableArray : NSArray
- (void)insertObject:(id)a0 atIndex:(unsigned long)a1;
- (void)removeObjectAtIndex:(unsigned long)a0;
- (void)replaceObjectAtIndex:(unsigned long)a0 withObject:(id)a1;
- (void)addObject:(id)a0;
- (void)removeLastObject;
- (void)setObject:(id)a0 atIndex:(unsigned long)a1;
- (void)exchangeObjectAtIndex:(unsigned long)a0 withObjectAtIndex:(unsigned long)a1;
- (void)removeAllObjects;
- (void)replaceObjectsInRange:(NSRange)a0 withObjects:(id *)a1 count:(unsigned long)a2;
@end

@implementation NSMutableArray
- (void)insertObject:(id)a0 atIndex:(unsigned long)a1
{
    // unresolved

}

- (void)removeObjectAtIndex:(unsigned long)a0
{
    // unresolved

}

- (void)replaceObjectAtIndex:(unsigned long)a0 withObject:(id)a1
{
    // unresolved

}

- (void)addObject:(id)a0
{
    // unresolved

}

- (void)removeLastObject
{
    // unresolved

}

- (void)setObject:(id)a0 atIndex:(unsigned long)a1
{
    // unresolved

}

- (void)exchangeObjectAtIndex:(unsigned long)a0 withObjectAtIndex:(unsigned long)a1
{
    // unresolved

}

- (void)removeAllObjects
{
    // unresolved

}

- (void)replaceObjectsInRange:(NSRange)a0 withObjects:(id *)a1 count:(unsigned long)a2
{
    // unresolved

}

@end

@interface __NSCFArray : NSMutableArray
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (unsigned long)count;
- (id)objectAtIndex:(unsigned long)a0;
- (void)getObjects:(id *)a0 range:(NSRange)a1;
- (void)insertObject:(id)a0 atIndex:(unsigned long)a1;
- (void)removeObjectAtIndex:(unsigned long)a0;
- (void)replaceObjectAtIndex:(unsigned long)a0 withObject:(id)a1;
- (void)addObject:(id)a0;
- (void)setObject:(id)a0 atIndex:(unsigned long)a1;
- (void)exchangeObjectAtIndex:(unsigned long)a0 withObjectAtIndex:(unsigned long)a1;
- (void)removeAllObjects;
- (void)replaceObjectsInRange:(NSRange)a0 withObjects:(id *)a1 count:(unsigned long)a2;
- (void)removeLastObject;
- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSCFArray
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [(REEF_INFO(self) & 3 ? [NSMutableArray class] : [NSArray class]) class];
}

- (unsigned long)count
{
    return (unsigned long)reefCall_CFArrayGetCount(self);
}

- (id)objectAtIndex:(unsigned long)a0
{
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count <= a0) __reefRaiseIndex(self, _cmd, a0, count);
    return (id)CFArrayGetValueAtIndex((CFArrayRef)self, a0);
}

- (void)getObjects:(id *)a0 range:(NSRange)a1
{
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count < a1.location || count - a1.location < a1.length) {
        [NSException raise:NSRangeException format:CFSTR("*** -[%s getObjects:range:]: range {%lu, %lu} extends beyond bounds [0 .. %ld]"), object_getClassName(self), a1.location, a1.length, count - 1];
    }
    CFArrayGetValues((CFArrayRef)self, CFRangeMake(a1.location, a1.length), (const void **)a0);
}

- (void)insertObject:(id)a0 atIndex:(unsigned long)a1
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    if (a0 == 0) __reefRaiseNil(self, _cmd, "object");
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count < a1) __reefRaiseIndex(self, _cmd, a1, count + 1);
    CFArrayInsertValueAtIndex((CFMutableArrayRef)self, a1, a0);
}

- (void)removeObjectAtIndex:(unsigned long)a0
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count <= a0) __reefRaiseIndex(self, _cmd, a0, count);
    CFArrayRemoveValueAtIndex((CFMutableArrayRef)self, a0);
}

- (void)replaceObjectAtIndex:(unsigned long)a0 withObject:(id)a1
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    if (a1 == 0) __reefRaiseNil(self, _cmd, "object");
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count <= a0) __reefRaiseIndex(self, _cmd, a0, count);
    CFArraySetValueAtIndex((CFMutableArrayRef)self, a0, a1);
}

- (void)addObject:(id)a0
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    if (a0 == 0) __reefRaiseNil(self, _cmd, "object");
    CFArrayAppendValue((CFMutableArrayRef)self, a0);
}

- (void)setObject:(id)a0 atIndex:(unsigned long)a1
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    if (a0 == 0) __reefRaiseNil(self, _cmd, "object");
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count < a1) __reefRaiseIndex(self, _cmd, a1, count + 1);
    CFArraySetValueAtIndex((CFMutableArrayRef)self, a1, a0);
}

- (void)exchangeObjectAtIndex:(unsigned long)a0 withObjectAtIndex:(unsigned long)a1
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count <= a0) __reefRaiseIndex(self, _cmd, a0, count);
    if (count <= a1) __reefRaiseIndex(self, _cmd, a1, count);
    CFArrayExchangeValuesAtIndices((CFMutableArrayRef)self, a0, a1);
}

- (void)removeAllObjects
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    CFArrayRemoveAllValues((CFMutableArrayRef)self);
}

- (void)replaceObjectsInRange:(NSRange)a0 withObjects:(id *)a1 count:(unsigned long)a2
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count < a0.location || count - a0.location < a0.length) {
        [NSException raise:NSRangeException format:CFSTR("*** -[%s replaceObjectsInRange:withObjects:count:]: range {%lu, %lu} extends beyond bounds [0 .. %ld]"), object_getClassName(self), a0.location, a0.length, count - 1];
    }
    for (unsigned long idx = 0; idx < a2; idx++) {
        if (a1[idx] == 0) __reefRaiseNil(self, _cmd, "object");
    }
    CFArrayReplaceValues((CFMutableArrayRef)self, CFRangeMake(a0.location, a0.length), (const void **)a1, a2);
}

- (void)removeLastObject
{
    if (!(REEF_INFO(self) & 3)) __reefRaiseImmutable(self, _cmd);
    unsigned long count = CFArrayGetCount((CFArrayRef)self);
    if (count != 0) CFArrayRemoveValueAtIndex((CFMutableArrayRef)self, count - 1);
}

- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2
{
    if (a0->state == 0) a0->mutationsPtr = (unsigned long *)&a0->extra[0];
    return _CFArrayFastEnumeration(self, a0, a1, a2);
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return (id)CFArrayCreateCopy(kCFAllocatorSystemDefault, (CFArrayRef)self);
}

@end

@interface NSDictionary : NSObject
- (unsigned long)count;
- (id)objectForKey:(id)a0;
- (id)keyEnumerator;
- (void)getObjects:(id *)a0 andKeys:(id *)a1;
- (BOOL)containsKey:(id)a0;
- (unsigned long)countForKey:(id)a0;
- (unsigned long)countForObject:(id)a0;
- (BOOL)containsObject:(id)a0;
- (BOOL)__getValue:(id *)a0 forKey:(id)a1;
- (void)__apply:(void * *)a0 context:(void *)a1;
- (void)enumerateKeysAndObjectsWithOptions:(unsigned long)a0 usingBlock:(void (^)(void))a1;
- (id)_cfMutableCopy;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)mutableCopyWithZone:(struct _NSZone *)a0;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2;
@end

@implementation NSDictionary
- (unsigned long)count
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)objectForKey:(id)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)keyEnumerator
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (void)getObjects:(id *)a0 andKeys:(id *)a1
{
    // unresolved

}

- (BOOL)containsKey:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)countForKey:(id)a0
{
    // unresolved
    return 0;
}

- (unsigned long)countForObject:(id)a0
{
    // unresolved
    return 0;
}

- (BOOL)containsObject:(id)a0
{
    // unresolved
    return NO;
}

- (BOOL)__getValue:(id *)a0 forKey:(id)a1
{
    // unresolved
    return NO;
}

- (void)__apply:(void * *)a0 context:(void *)a1
{
    // unresolved

}

- (void)enumerateKeysAndObjectsWithOptions:(unsigned long)a0 usingBlock:(void (^)(void))a1
{
    // unresolved

}

- (id)_cfMutableCopy
{
    // unresolved
    return nil;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)mutableCopyWithZone:(struct _NSZone *)a0
{
    return [self _cfMutableCopy];
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    return [self count];
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFDictionaryGetTypeID();
}

- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2
{
    // unresolved
    return 0;
}

@end

@interface NSMutableDictionary : NSDictionary
- (void)setObject:(id)a0 forKey:(id)a1;
- (void)removeObjectForKey:(id)a0;
- (void)removeAllObjects;
- (void)replaceObject:(id)a0 forKey:(id)a1;
- (void)__addObject:(id)a0 forKey:(id)a1;
- (void)__setObject:(id)a0 forKey:(id)a1;
@end

@implementation NSMutableDictionary
- (void)setObject:(id)a0 forKey:(id)a1
{
    // unresolved

}

- (void)removeObjectForKey:(id)a0
{
    // unresolved

}

- (void)removeAllObjects
{
    // unresolved

}

- (void)replaceObject:(id)a0 forKey:(id)a1
{
    // unresolved

}

- (void)__addObject:(id)a0 forKey:(id)a1
{
    // unresolved

}

- (void)__setObject:(id)a0 forKey:(id)a1
{
    [self setObject:a0 forKey:a1];
}

@end

@interface __NSCFDictionary : NSMutableDictionary
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (unsigned long)count;
- (id)objectForKey:(id)a0;
- (id)keyEnumerator;
- (void)getObjects:(id *)a0 andKeys:(id *)a1;
- (BOOL)containsKey:(id)a0;
- (BOOL)containsObject:(id)a0;
- (unsigned long)countForKey:(id)a0;
- (unsigned long)countForObject:(id)a0;
- (void)setObject:(id)a0 forKey:(id)a1;
- (void)__setObject:(id)a0 forKey:(id)a1;
- (void)__addObject:(id)a0 forKey:(id)a1;
- (void)replaceObject:(id)a0 forKey:(id)a1;
- (void)removeObjectForKey:(id)a0;
- (void)removeAllObjects;
- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)mutableCopyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSCFDictionary
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [(REEF_INFO(self) & 0x40 ? [NSDictionary class] : [NSMutableDictionary class]) class];
}

- (unsigned long)count
{
    return (unsigned long)reefCall_CFDictionaryGetCount(self);
}

- (id)objectForKey:(id)a0
{
    if (a0 == 0) return 0;
    return (id)CFDictionaryGetValue((CFDictionaryRef)self, a0);
}

- (id)keyEnumerator
{
    CFIndex count = CFDictionaryGetCount((CFDictionaryRef)self);
    const void **items = malloc(count * sizeof(void *) + 1);
    CFDictionaryGetKeysAndValues((CFDictionaryRef)self, items, NULL);
    CFArrayRef snapshot = CFArrayCreate(kCFAllocatorSystemDefault, items, count, &kCFTypeArrayCallBacks);
    id enumerator = [[__NSCFSnapshotEnumerator alloc] initWithSnapshot:(id)snapshot];
    CFRelease(snapshot);
    id result = objc_autorelease(enumerator);
    free(items);
    return result;
}

- (void)getObjects:(id *)a0 andKeys:(id *)a1
{
    reefCall_CFDictionaryGetKeysAndValues(self, a1, a0);
}

- (BOOL)containsKey:(id)a0
{
    return (BOOL)CFDictionaryContainsKey((CFDictionaryRef)self, a0);
}

- (BOOL)containsObject:(id)a0
{
    if (a0 == 0) return 0;
    return CFDictionaryContainsValue((CFDictionaryRef)self, a0) != 0;
}

- (unsigned long)countForKey:(id)a0
{
    return (unsigned long)reefCall_CFDictionaryGetCountOfKey(self, a0);
}

- (unsigned long)countForObject:(id)a0
{
    return (unsigned long)reefCall_CFDictionaryGetCountOfValue(self, a0);
}

- (void)setObject:(id)a0 forKey:(id)a1
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    if (a0 == 0) __reefRaiseNil(self, _cmd, "object");
    if (a1 == 0) __reefRaiseNil(self, _cmd, "key");
    id key = [a1 copy];
    CFDictionarySetValue((CFMutableDictionaryRef)self, key, a0);
    objc_release(key);
}

- (void)__setObject:(id)a0 forKey:(id)a1
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    CFDictionarySetValue((CFMutableDictionaryRef)self, a1, a0);
}

- (void)__addObject:(id)a0 forKey:(id)a1
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    CFDictionaryAddValue((CFMutableDictionaryRef)self, a1, a0);
}

- (void)replaceObject:(id)a0 forKey:(id)a1
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    CFDictionaryReplaceValue((CFMutableDictionaryRef)self, a1, a0);
}

- (void)removeObjectForKey:(id)a0
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    if (a0 == 0) __reefRaiseNil(self, _cmd, "key");
    CFDictionaryRemoveValue((CFMutableDictionaryRef)self, a0);
}

- (void)removeAllObjects
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    CFDictionaryRemoveAllValues((CFMutableDictionaryRef)self);
}

- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2
{
    if (a0->state == 0) a0->mutationsPtr = (unsigned long *)&a0->extra[0];
    return _CFDictionaryFastEnumeration(self, a0, a1, a2);
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return (id)CFDictionaryCreateCopy(kCFAllocatorSystemDefault, (CFDictionaryRef)self);
}

- (id)mutableCopyWithZone:(struct _NSZone *)a0
{
    return (id)CFDictionaryCreateMutableCopy(kCFAllocatorSystemDefault, 0, (CFDictionaryRef)self);
}

@end

@interface NSSet : NSObject
- (unsigned long)count;
- (id)member:(id)a0;
- (id)objectEnumerator;
- (BOOL)containsObject:(id)a0;
- (unsigned long)countForObject:(id)a0;
- (void)getObjects:(id *)a0;
- (BOOL)__getValue:(id *)a0 forObj:(id)a1;
- (void)__applyValues:(void * *)a0 context:(void *)a1;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)mutableCopyWithZone:(struct _NSZone *)a0;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2;
@end

@implementation NSSet
- (unsigned long)count
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)member:(id)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)objectEnumerator
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (BOOL)containsObject:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)countForObject:(id)a0
{
    // unresolved
    return 0;
}

- (void)getObjects:(id *)a0
{
    // unresolved

}

- (BOOL)__getValue:(id *)a0 forObj:(id)a1
{
    // unresolved
    return NO;
}

- (void)__applyValues:(void * *)a0 context:(void *)a1
{
    // unresolved

}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)mutableCopyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    return [self count];
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFSetGetTypeID();
}

- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2
{
    // unresolved
    return 0;
}

@end

@interface NSMutableSet : NSSet
- (void)addObject:(id)a0;
- (void)removeObject:(id)a0;
- (void)removeAllObjects;
- (void)replaceObject:(id)a0;
- (void)setObject:(id)a0;
@end

@implementation NSMutableSet
- (void)addObject:(id)a0
{
    // unresolved

}

- (void)removeObject:(id)a0
{
    // unresolved

}

- (void)removeAllObjects
{
    // unresolved

}

- (void)replaceObject:(id)a0
{
    // unresolved

}

- (void)setObject:(id)a0
{
    // unresolved

}

@end

@interface __NSCFSet : NSMutableSet
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (unsigned long)count;
- (id)member:(id)a0;
- (id)objectEnumerator;
- (void)getObjects:(id *)a0;
- (BOOL)containsObject:(id)a0;
- (unsigned long)countForObject:(id)a0;
- (void)addObject:(id)a0;
- (void)removeObject:(id)a0;
- (void)removeAllObjects;
- (void)replaceObject:(id)a0;
- (void)setObject:(id)a0;
- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSCFSet
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [(REEF_INFO(self) & 0x40 ? [NSSet class] : [NSMutableSet class]) class];
}

- (unsigned long)count
{
    return (unsigned long)reefCall_CFSetGetCount(self);
}

- (id)member:(id)a0
{
    const void *value = NULL;
    if (a0 == 0 || !CFSetGetValueIfPresent((CFSetRef)self, a0, &value)) return 0;
    return (id)value;
}

- (id)objectEnumerator
{
    CFIndex count = CFSetGetCount((CFSetRef)self);
    const void **items = malloc(count * sizeof(void *) + 1);
    CFSetGetValues((CFSetRef)self, items);
    CFArrayRef snapshot = CFArrayCreate(kCFAllocatorSystemDefault, items, count, &kCFTypeArrayCallBacks);
    id enumerator = [[__NSCFSnapshotEnumerator alloc] initWithSnapshot:(id)snapshot];
    CFRelease(snapshot);
    id result = objc_autorelease(enumerator);
    free(items);
    return result;
}

- (void)getObjects:(id *)a0
{
    reefCall_CFSetGetValues(self, a0);
}

- (BOOL)containsObject:(id)a0
{
    if (a0 == 0) return 0;
    return CFSetContainsValue((CFSetRef)self, a0) != 0;
}

- (unsigned long)countForObject:(id)a0
{
    return (unsigned long)reefCall_CFSetGetCountOfValue(self, a0);
}

- (void)addObject:(id)a0
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    if (a0 == 0) __reefRaiseNil(self, _cmd, "object");
    CFSetAddValue((CFMutableSetRef)self, a0);
}

- (void)removeObject:(id)a0
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    if (a0 == 0) __reefRaiseNil(self, _cmd, "object");
    CFSetRemoveValue((CFMutableSetRef)self, a0);
}

- (void)removeAllObjects
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    CFSetRemoveAllValues((CFMutableSetRef)self);
}

- (void)replaceObject:(id)a0
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    CFSetReplaceValue((CFMutableSetRef)self, a0);
}

- (void)setObject:(id)a0
{
    if (REEF_INFO(self) & 0x40) __reefRaiseImmutable(self, _cmd);
    CFSetSetValue((CFMutableSetRef)self, a0);
}

- (unsigned long)countByEnumeratingWithState:(NSFastEnumerationState *)a0 objects:(id *)a1 count:(unsigned long)a2
{
    if (a0->state == 0) a0->mutationsPtr = (unsigned long *)&a0->extra[0];
    return _CFSetFastEnumeration(self, a0, a1, a2);
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return (id)CFSetCreateCopy(kCFAllocatorSystemDefault, (CFSetRef)self);
}

@end

@interface __NSArray0 : NSArray
- (id)retain;
- (void)release;
- (id)autorelease;
- (unsigned long)retainCount;
- (unsigned long)count;
- (id)objectAtIndex:(unsigned long)a0;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSArray0
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)autorelease
{
    return self;
}

- (unsigned long)retainCount
{
    return 0xffffffffffffffff;
}

- (unsigned long)count
{
    return 0;
}

- (id)objectAtIndex:(unsigned long)a0
{
    // unresolved
    return nil;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return self;
}

@end

@interface __NSDictionary0 : NSDictionary
- (id)retain;
- (void)release;
- (id)autorelease;
- (unsigned long)retainCount;
- (unsigned long)count;
- (id)objectForKey:(id)a0;
- (id)keyEnumerator;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSDictionary0
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)autorelease
{
    return self;
}

- (unsigned long)retainCount
{
    return 0xffffffffffffffff;
}

- (unsigned long)count
{
    return 0;
}

- (id)objectForKey:(id)a0
{
    return nil;
}

- (id)keyEnumerator
{
    // unresolved
    return nil;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return self;
}

@end

@interface NSConstantArray : NSArray
{
    unsigned long _count;
    id * _objects;
}
- (id)retain;
- (void)release;
- (id)autorelease;
- (unsigned long)retainCount;
- (unsigned long)count;
- (id)objectAtIndex:(unsigned long)a0;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation NSConstantArray
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)autorelease
{
    return self;
}

- (unsigned long)retainCount
{
    return 0xffffffffffffffff;
}

- (unsigned long)count
{
    // unresolved
    return 0;
}

- (id)objectAtIndex:(unsigned long)a0
{
    // unresolved
    return nil;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return self;
}

@end

@interface NSConstantDictionary : NSDictionary
{
    unsigned long _options;
    unsigned long _count;
    id * _keys;
    id * _objects;
}
- (id)retain;
- (void)release;
- (id)autorelease;
- (unsigned long)retainCount;
- (unsigned long)count;
- (id)objectForKey:(id)a0;
- (id)keyEnumerator;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation NSConstantDictionary
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)autorelease
{
    return self;
}

- (unsigned long)retainCount
{
    return 0xffffffffffffffff;
}

- (unsigned long)count
{
    return _count;
}

- (id)objectForKey:(id)a0
{
    // unresolved
    return nil;
}

- (id)keyEnumerator
{
    // unresolved
    return nil;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

@end

@interface NSUserDefaults : NSObject
{
    id _suite;
    id _registration;
    id _volatile;
}

@end

@implementation NSUserDefaults
@end

@interface NSDate : NSObject
- (double)timeIntervalSinceReferenceDate;
- (double)timeIntervalSinceDate:(id)a0;
- (long)compare:(id)a0;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (id)copyWithZone:(struct _NSZone *)a0;
- (unsigned long)_cfTypeID;
@end

@implementation NSDate
- (double)timeIntervalSinceReferenceDate
{
    // unresolved
    return 0;
}

- (double)timeIntervalSinceDate:(id)a0
{
    // unresolved
    return 0;
}

- (long)compare:(id)a0
{
    // unresolved
    return 0;
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    // unresolved
    return 0;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFDateGetTypeID();
}

@end

@interface __NSCFDate : NSDate
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (double)timeIntervalSinceReferenceDate;
@end

@implementation __NSCFDate
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [NSDate class];
}

- (double)timeIntervalSinceReferenceDate
{
    return CFDateGetAbsoluteTime((CFDateRef)self);
}

@end

@interface NSURL : NSObject
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (struct __CFURL *)_cfurl;
- (struct __CFString *)_reefPath;
- (id)baseURL;
- (id)absoluteURL;
- (id)relativeString;
- (id)scheme;
- (id)host;
- (id)port;
- (id)user;
- (id)password;
- (id)query;
- (id)fragment;
- (BOOL)isFileReferenceURL;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)description;
@end

@implementation NSURL
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (struct __CFURL *)_cfurl
{
    // unresolved
    return 0;
}

- (struct __CFString *)_reefPath
{
    // unresolved
    return 0;
}

- (id)baseURL
{
    return (id)reefCall_CFURLGetBaseURL(self);
}

- (id)absoluteURL
{
    // unresolved
    return nil;
}

- (id)relativeString
{
    return (id)reefCall_CFURLGetString(self);
}

- (id)scheme
{
    // unresolved
    return nil;
}

- (id)host
{
    // unresolved
    return nil;
}

- (id)port
{
    // unresolved
    return nil;
}

- (id)user
{
    // unresolved
    return nil;
}

- (id)password
{
    // unresolved
    return nil;
}

- (id)query
{
    // unresolved
    return nil;
}

- (id)fragment
{
    // unresolved
    return nil;
}

- (BOOL)isFileReferenceURL
{
    return NO;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)description
{
    // unresolved
    return nil;
}

@end


@implementation NSException
+ (id)exceptionWithName:(id)a0 reason:(id)a1 userInfo:(id)a2
{
    // unresolved
    return nil;
}

+ (void)raise:(id)a0 format:(id)a1, ...
{
    // unresolved

}

+ (void)raise:(id)a0 format:(id)a1 arguments:(void *)a2
{
    // unresolved

}

- (id)initWithName:(id)a0 reason:(id)a1 userInfo:(id)a2
{
    // unresolved
    return nil;
}

- (void)dealloc
{
    // unresolved

}

- (void)raise
{
    // unresolved

}

- (id)name
{
    // unresolved
    return nil;
}

- (id)reason
{
    return _reason;
}

- (id)userInfo
{
    return _userInfo;
}

- (void)_recordCallStack
{
    // unresolved

}

- (id)callStackReturnAddresses
{
    // unresolved
    return nil;
}

- (id)callStackSymbols
{
    // unresolved
    return nil;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)description
{
    // unresolved
    return nil;
}

@end

@interface NSLocale : NSObject
- (id)objectForKey:(id)a0;
- (id)displayNameForKey:(id)a0 value:(id)a1;
- (id)localeIdentifier;
- (struct __CFString *)_copyDisplayNameForKey:(id)a0 value:(id)a1;
- (struct __CFDictionary *)_prefs;
- (BOOL)_doesNotRequireSpecialCaseHandling;
- (void)_setDoesNotRequireSpecialCaseHandling;
- (id)copyWithZone:(struct _NSZone *)a0;
- (unsigned long)_cfTypeID;
@end

@implementation NSLocale
- (id)objectForKey:(id)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)displayNameForKey:(id)a0 value:(id)a1
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)localeIdentifier
{
    // unresolved
    return nil;
}

- (struct __CFString *)_copyDisplayNameForKey:(id)a0 value:(id)a1
{
    // unresolved
    return 0;
}

- (struct __CFDictionary *)_prefs
{
    return 0;
}

- (BOOL)_doesNotRequireSpecialCaseHandling
{
    return NO;
}

- (void)_setDoesNotRequireSpecialCaseHandling
{

}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFLocaleGetTypeID();
}

@end

@interface __NSCFLocale : NSLocale
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (id)objectForKey:(id)a0;
- (id)displayNameForKey:(id)a0 value:(id)a1;
- (id)localeIdentifier;
@end

@implementation __NSCFLocale
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [NSLocale class];
}

- (id)objectForKey:(id)a0
{
    return (id)reefCall_CFLocaleGetValue(self, a0);
}

- (id)displayNameForKey:(id)a0 value:(id)a1
{
    id result = (id)CFLocaleCopyDisplayNameForPropertyValue((CFLocaleRef)self, (CFStringRef)a0, (CFStringRef)a1);
    return result ? objc_autorelease(result) : nil;
}

- (id)localeIdentifier
{
    return (id)reefCall_CFLocaleGetIdentifier(self);
}

@end

@interface NSTimeZone : NSObject
- (id)name;
- (id)data;
- (long)secondsFromGMTForDate:(id)a0;
- (id)abbreviationForDate:(id)a0;
- (BOOL)isDaylightSavingTimeForDate:(id)a0;
- (double)daylightSavingTimeOffsetForDate:(id)a0;
- (id)nextDaylightSavingTimeTransitionAfterDate:(id)a0;
- (id)localizedName:(long)a0 locale:(id)a1;
- (double)_daylightSavingTimeOffsetForAbsoluteTime:(double)a0;
- (double)_nextDaylightSavingTimeTransitionAfterAbsoluteTime:(double)a0;
- (id)copyWithZone:(struct _NSZone *)a0;
- (unsigned long)_cfTypeID;
@end

@implementation NSTimeZone
- (id)name
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)data
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (long)secondsFromGMTForDate:(id)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (id)abbreviationForDate:(id)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (BOOL)isDaylightSavingTimeForDate:(id)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (double)daylightSavingTimeOffsetForDate:(id)a0
{
    // unresolved
    return 0;
}

- (id)nextDaylightSavingTimeTransitionAfterDate:(id)a0
{
    return nil;
}

- (id)localizedName:(long)a0 locale:(id)a1
{
    return nil;
}

- (double)_daylightSavingTimeOffsetForAbsoluteTime:(double)a0
{
    // unresolved
    return 0;
}

- (double)_nextDaylightSavingTimeTransitionAfterAbsoluteTime:(double)a0
{
    // unresolved
    return 0;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFTimeZoneGetTypeID();
}

@end

@interface __NSCFTimeZone : NSTimeZone
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (id)name;
- (id)data;
- (long)secondsFromGMTForDate:(id)a0;
- (id)abbreviationForDate:(id)a0;
- (BOOL)isDaylightSavingTimeForDate:(id)a0;
- (double)daylightSavingTimeOffsetForDate:(id)a0;
- (id)nextDaylightSavingTimeTransitionAfterDate:(id)a0;
- (id)localizedName:(long)a0 locale:(id)a1;
@end

@implementation __NSCFTimeZone
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [NSTimeZone class];
}

- (id)name
{
    return (id)reefCall_CFTimeZoneGetName(self);
}

- (id)data
{
    return (id)reefCall_CFTimeZoneGetData(self);
}

- (long)secondsFromGMTForDate:(id)a0
{
    return (long)CFTimeZoneGetSecondsFromGMT((CFTimeZoneRef)self, CFDateGetAbsoluteTime((CFDateRef)a0));
}

- (id)abbreviationForDate:(id)a0
{
    id result = (id)CFTimeZoneCopyAbbreviation((CFTimeZoneRef)self, CFDateGetAbsoluteTime((CFDateRef)a0));
    return result ? objc_autorelease(result) : nil;
}

- (BOOL)isDaylightSavingTimeForDate:(id)a0
{
    return CFTimeZoneIsDaylightSavingTime((CFTimeZoneRef)self, CFDateGetAbsoluteTime((CFDateRef)a0));
}

- (double)daylightSavingTimeOffsetForDate:(id)a0
{
    return CFTimeZoneGetDaylightSavingTimeOffset((CFTimeZoneRef)self, CFDateGetAbsoluteTime((CFDateRef)a0));
}

- (id)nextDaylightSavingTimeTransitionAfterDate:(id)a0
{
    CFAbsoluteTime next = CFTimeZoneGetNextDaylightSavingTimeTransition((CFTimeZoneRef)self, CFDateGetAbsoluteTime((CFDateRef)a0));
    if (next == 0) return nil;
    id result = (id)CFDateCreate(kCFAllocatorSystemDefault, next);
    return result ? objc_autorelease(result) : nil;
}

- (id)localizedName:(long)a0 locale:(id)a1
{
    id result = (id)CFTimeZoneCopyLocalizedName((CFTimeZoneRef)self, (CFTimeZoneNameStyle)a0, (CFLocaleRef)a1);
    return result ? objc_autorelease(result) : nil;
}

@end

@interface NSCalendar : NSObject
- (id)copyWithZone:(struct _NSZone *)a0;
- (unsigned long)_cfTypeID;
@end

@implementation NSCalendar
- (id)copyWithZone:(struct _NSZone *)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFCalendarGetTypeID();
}

@end

@interface __NSCFCalendar : NSCalendar
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSCFCalendar
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [NSCalendar class];
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    CFAllocatorRef allocator = kCFAllocatorSystemDefault;
    CFCalendarRef copy = CFCalendarCreateWithIdentifier(allocator, CFCalendarGetIdentifier((CFCalendarRef)self));
    CFLocaleRef locale = CFCalendarCopyLocale((CFCalendarRef)self);
    CFTimeZoneRef timeZone = CFCalendarCopyTimeZone((CFCalendarRef)self);
    CFCalendarSetLocale(copy, locale);
    CFCalendarSetTimeZone(copy, timeZone);
    CFCalendarSetFirstWeekday(copy, CFCalendarGetFirstWeekday((CFCalendarRef)self));
    CFCalendarSetMinimumDaysInFirstWeek(copy, CFCalendarGetMinimumDaysInFirstWeek((CFCalendarRef)self));
    CFRelease(locale);
    CFRelease(timeZone);
    return (id)copy;
}

@end

@interface NSRunLoop : NSObject
{
    struct __CFRunLoop * _rl;
}
+ (id)runLoopFor:(struct __CFRunLoop *)a0;
+ (id)currentRunLoop;
+ (id)mainRunLoop;
- (id)currentMode;
- (struct __CFRunLoop *)getCFRunLoop;
- (void)addTimer:(id)a0 forMode:(id)a1;
- (void)addPort:(id)a0 forMode:(id)a1;
- (void)removePort:(id)a0 forMode:(id)a1;
- (id)limitDateForMode:(id)a0;
- (void)acceptInputForMode:(id)a0 beforeDate:(id)a1;
- (void)run;
- (void)runUntilDate:(id)a0;
- (BOOL)runMode:(id)a0 beforeDate:(id)a1;
- (void)performInModes:(id)a0 block:(void (^)(void))a1;
- (void)performBlock:(void (^)(void))a0;
- (id)description;
@end

@implementation NSRunLoop
+ (id)runLoopFor:(struct __CFRunLoop *)a0
{
    // unresolved
    return nil;
}

+ (id)currentRunLoop
{
    // unresolved
    return nil;
}

+ (id)mainRunLoop
{
    // unresolved
    return nil;
}

- (id)currentMode
{
    // unresolved
    return nil;
}

- (struct __CFRunLoop *)getCFRunLoop
{
    // unresolved
    return 0;
}

- (void)addTimer:(id)a0 forMode:(id)a1
{
    // unresolved

}

- (void)addPort:(id)a0 forMode:(id)a1
{
    // unresolved

}

- (void)removePort:(id)a0 forMode:(id)a1
{
    // unresolved

}

- (id)limitDateForMode:(id)a0
{
    // unresolved
    return nil;
}

- (void)acceptInputForMode:(id)a0 beforeDate:(id)a1
{
    // unresolved

}

- (void)run
{
    // unresolved

}

- (void)runUntilDate:(id)a0
{
    // unresolved

}

- (BOOL)runMode:(id)a0 beforeDate:(id)a1
{
    // unresolved
    return NO;
}

- (void)performInModes:(id)a0 block:(void (^)(void))a1
{
    // unresolved

}

- (void)performBlock:(void (^)(void))a0
{
    // unresolved

}

- (id)description
{
    // unresolved
    return nil;
}

@end

@interface NSStream : NSObject
+ (void)getStreamsToHostWithName:(id)a0 port:(long)a1 inputStream:(id *)a2 outputStream:(id *)a3;
+ (void)getStreamsToHost:(id)a0 port:(long)a1 inputStream:(id *)a2 outputStream:(id *)a3;
+ (void)getBoundStreamsWithBufferSize:(unsigned long)a0 inputStream:(id *)a1 outputStream:(id *)a2;
- (void)open;
- (void)close;
- (id)delegate;
- (void)setDelegate:(id)a0;
- (id)propertyForKey:(id)a0;
- (BOOL)setProperty:(id)a0 forKey:(id)a1;
- (void)scheduleInRunLoop:(id)a0 forMode:(id)a1;
- (void)removeFromRunLoop:(id)a0 forMode:(id)a1;
- (unsigned long)streamStatus;
- (id)streamError;
- (void *)_cfStreamError;
@end

@implementation NSStream
+ (void)getStreamsToHostWithName:(id)a0 port:(long)a1 inputStream:(id *)a2 outputStream:(id *)a3
{
    // unresolved

}

+ (void)getStreamsToHost:(id)a0 port:(long)a1 inputStream:(id *)a2 outputStream:(id *)a3
{
    // unresolved

}

+ (void)getBoundStreamsWithBufferSize:(unsigned long)a0 inputStream:(id *)a1 outputStream:(id *)a2
{
    // unresolved

}

- (void)open
{
    // unresolved

}

- (void)close
{
    // unresolved

}

- (id)delegate
{
    // unresolved
    return nil;
}

- (void)setDelegate:(id)a0
{
    // unresolved

}

- (id)propertyForKey:(id)a0
{
    return nil;
}

- (BOOL)setProperty:(id)a0 forKey:(id)a1
{
    return NO;
}

- (void)scheduleInRunLoop:(id)a0 forMode:(id)a1
{

}

- (void)removeFromRunLoop:(id)a0 forMode:(id)a1
{

}

- (unsigned long)streamStatus
{
    return 0;
}

- (id)streamError
{
    return nil;
}

- (void *)_cfStreamError
{
    // unresolved
    return (void *){0};
}

@end

@interface NSInputStream : NSStream
+ (id)allocWithZone:(struct _NSZone *)a0;
+ (id)inputStreamWithData:(id)a0;
+ (id)inputStreamWithFileAtPath:(id)a0;
+ (id)inputStreamWithURL:(id)a0;
- (id)initWithData:(id)a0;
- (id)initWithURL:(id)a0;
- (id)initWithFileAtPath:(id)a0;
- (long)read:(char *)a0 maxLength:(unsigned long)a1;
- (BOOL)getBuffer:(char * *)a0 length:(unsigned long *)a1;
- (BOOL)hasBytesAvailable;
- (unsigned long)_cfTypeID;
@end

@implementation NSInputStream
+ (id)allocWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

+ (id)inputStreamWithData:(id)a0
{
    // unresolved
    return nil;
}

+ (id)inputStreamWithFileAtPath:(id)a0
{
    // unresolved
    return nil;
}

+ (id)inputStreamWithURL:(id)a0
{
    // unresolved
    return nil;
}

- (id)initWithData:(id)a0
{
    // unresolved
    return nil;
}

- (id)initWithURL:(id)a0
{
    // unresolved
    return nil;
}

- (id)initWithFileAtPath:(id)a0
{
    // unresolved
    return nil;
}

- (long)read:(char *)a0 maxLength:(unsigned long)a1
{
    // unresolved
    return 0;
}

- (BOOL)getBuffer:(char * *)a0 length:(unsigned long *)a1
{
    return NO;
}

- (BOOL)hasBytesAvailable
{
    return NO;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFReadStreamGetTypeID();
}

@end

@interface NSOutputStream : NSStream
+ (id)allocWithZone:(struct _NSZone *)a0;
+ (id)outputStreamToMemory;
+ (id)outputStreamToBuffer:(char *)a0 capacity:(unsigned long)a1;
+ (id)outputStreamToFileAtPath:(id)a0 append:(BOOL)a1;
+ (id)outputStreamWithURL:(id)a0 append:(BOOL)a1;
- (id)initToMemory;
- (id)initToBuffer:(char *)a0 capacity:(unsigned long)a1;
- (id)initWithURL:(id)a0 append:(BOOL)a1;
- (id)initToFileAtPath:(id)a0 append:(BOOL)a1;
- (long)write:(char *)a0 maxLength:(unsigned long)a1;
- (BOOL)hasSpaceAvailable;
- (unsigned long)_cfTypeID;
@end

@implementation NSOutputStream
+ (id)allocWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

+ (id)outputStreamToMemory
{
    // unresolved
    return nil;
}

+ (id)outputStreamToBuffer:(char *)a0 capacity:(unsigned long)a1
{
    // unresolved
    return nil;
}

+ (id)outputStreamToFileAtPath:(id)a0 append:(BOOL)a1
{
    // unresolved
    return nil;
}

+ (id)outputStreamWithURL:(id)a0 append:(BOOL)a1
{
    // unresolved
    return nil;
}

- (id)initToMemory
{
    // unresolved
    return nil;
}

- (id)initToBuffer:(char *)a0 capacity:(unsigned long)a1
{
    // unresolved
    return nil;
}

- (id)initWithURL:(id)a0 append:(BOOL)a1
{
    // unresolved
    return nil;
}

- (id)initToFileAtPath:(id)a0 append:(BOOL)a1
{
    // unresolved
    return nil;
}

- (long)write:(char *)a0 maxLength:(unsigned long)a1
{
    // unresolved
    return 0;
}

- (BOOL)hasSpaceAvailable
{
    return NO;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFWriteStreamGetTypeID();
}

@end

@interface __NSPlaceholderInputStream : NSInputStream
- (id)retain;
- (void)release;
- (id)initWithData:(id)a0;
- (id)initWithURL:(id)a0;
- (id)initWithFileAtPath:(id)a0;
@end

@implementation __NSPlaceholderInputStream
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)initWithData:(id)a0
{
    // unresolved
    return nil;
}

- (id)initWithURL:(id)a0
{
    // unresolved
    return nil;
}

- (id)initWithFileAtPath:(id)a0
{
    // unresolved
    return nil;
}

@end

@interface __NSPlaceholderOutputStream : NSOutputStream
- (id)retain;
- (void)release;
- (id)initToMemory;
- (id)initToBuffer:(char *)a0 capacity:(unsigned long)a1;
- (id)initWithURL:(id)a0 append:(BOOL)a1;
- (id)initToFileAtPath:(id)a0 append:(BOOL)a1;
@end

@implementation __NSPlaceholderOutputStream
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)initToMemory
{
    // unresolved
    return nil;
}

- (id)initToBuffer:(char *)a0 capacity:(unsigned long)a1
{
    // unresolved
    return nil;
}

- (id)initWithURL:(id)a0 append:(BOOL)a1
{
    // unresolved
    return nil;
}

- (id)initToFileAtPath:(id)a0 append:(BOOL)a1
{
    // unresolved
    return nil;
}

@end

@interface __NSCFInputStream : NSInputStream
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (void)open;
- (void)close;
- (void)setDelegate:(id)a0;
- (id)propertyForKey:(id)a0;
- (BOOL)setProperty:(id)a0 forKey:(id)a1;
- (void)scheduleInRunLoop:(id)a0 forMode:(id)a1;
- (void)removeFromRunLoop:(id)a0 forMode:(id)a1;
- (unsigned long)streamStatus;
- (id)streamError;
- (void *)_cfStreamError;
- (long)read:(char *)a0 maxLength:(unsigned long)a1;
- (BOOL)getBuffer:(char * *)a0 length:(unsigned long *)a1;
- (BOOL)hasBytesAvailable;
@end

@implementation __NSCFInputStream
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (void)open
{
    reefCall_CFReadStreamOpen(self);
}

- (void)close
{
    reefCall_CFReadStreamClose(self);
}

- (void)setDelegate:(id)a0
{
    // unresolved

}

- (id)propertyForKey:(id)a0
{
    // unresolved
    return nil;
}

- (BOOL)setProperty:(id)a0 forKey:(id)a1
{
    // unresolved
    return NO;
}

- (void)scheduleInRunLoop:(id)a0 forMode:(id)a1
{
    // unresolved

}

- (void)removeFromRunLoop:(id)a0 forMode:(id)a1
{
    // unresolved

}

- (unsigned long)streamStatus
{
    return (unsigned long)reefCall_CFReadStreamGetStatus(self);
}

- (id)streamError
{
    // unresolved
    return nil;
}

- (void *)_cfStreamError
{
    return (void *)reefCall_CFReadStreamGetError(self);
}

- (long)read:(char *)a0 maxLength:(unsigned long)a1
{
    return (long)reefCall_CFReadStreamRead(self, a0, a1);
}

- (BOOL)getBuffer:(char * *)a0 length:(unsigned long *)a1
{
    // unresolved
    return NO;
}

- (BOOL)hasBytesAvailable
{
    // unresolved
    return NO;
}

@end

@interface __NSCFOutputStream : NSOutputStream
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (void)open;
- (void)close;
- (void)setDelegate:(id)a0;
- (id)propertyForKey:(id)a0;
- (BOOL)setProperty:(id)a0 forKey:(id)a1;
- (void)scheduleInRunLoop:(id)a0 forMode:(id)a1;
- (void)removeFromRunLoop:(id)a0 forMode:(id)a1;
- (unsigned long)streamStatus;
- (id)streamError;
- (void *)_cfStreamError;
- (long)write:(char *)a0 maxLength:(unsigned long)a1;
- (BOOL)hasSpaceAvailable;
@end

@implementation __NSCFOutputStream
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (void)open
{
    reefCall_CFWriteStreamOpen(self);
}

- (void)close
{
    reefCall_CFWriteStreamClose(self);
}

- (void)setDelegate:(id)a0
{
    // unresolved

}

- (id)propertyForKey:(id)a0
{
    // unresolved
    return nil;
}

- (BOOL)setProperty:(id)a0 forKey:(id)a1
{
    // unresolved
    return NO;
}

- (void)scheduleInRunLoop:(id)a0 forMode:(id)a1
{
    // unresolved

}

- (void)removeFromRunLoop:(id)a0 forMode:(id)a1
{
    // unresolved

}

- (unsigned long)streamStatus
{
    return (unsigned long)reefCall_CFWriteStreamGetStatus(self);
}

- (id)streamError
{
    // unresolved
    return nil;
}

- (void *)_cfStreamError
{
    return (void *)reefCall_CFWriteStreamGetError(self);
}

- (long)write:(char *)a0 maxLength:(unsigned long)a1
{
    return (long)reefCall_CFWriteStreamWrite(self, a0, a1);
}

- (BOOL)hasSpaceAvailable
{
    // unresolved
    return NO;
}

@end

@interface NSString : NSObject
- (unsigned long)length;
- (unsigned short)characterAtIndex:(unsigned long)a0;
- (void)getCharacters:(unsigned short *)a0 range:(NSRange)a1;
- (id)description;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)mutableCopyWithZone:(struct _NSZone *)a0;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (unsigned short *)_fastCharacterContents;
- (char *)_fastCStringContents:(BOOL)a0;
- (unsigned int)_fastestEncodingInCFStringEncoding;
- (unsigned int)_smallestEncodingInCFStringEncoding;
- (BOOL)_encodingCantBeStoredInEightBitCFString;
- (BOOL)_getCString:(char *)a0 maxLength:(unsigned long)a1 encoding:(unsigned int)a2;
- (struct __CFString *)_createSubstringWithRange:(NSRange)a0;
- (void)getLineStart:(unsigned long *)a0 end:(unsigned long *)a1 contentsEnd:(unsigned long *)a2 forRange:(NSRange)a3;
- (void)getParagraphStart:(unsigned long *)a0 end:(unsigned long *)a1 contentsEnd:(unsigned long *)a2 forRange:(NSRange)a3;
@end

@implementation NSString
- (unsigned long)length
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (unsigned short)characterAtIndex:(unsigned long)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (void)getCharacters:(unsigned short *)a0 range:(NSRange)a1
{
    // unresolved

}

- (id)description
{
    return self;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)mutableCopyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    // unresolved
    return 0;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFStringGetTypeID();
}

- (unsigned short *)_fastCharacterContents
{
    return 0;
}

- (char *)_fastCStringContents:(BOOL)a0
{
    return 0;
}

- (unsigned int)_fastestEncodingInCFStringEncoding
{
    return 0x100;
}

- (unsigned int)_smallestEncodingInCFStringEncoding
{
    // unresolved
    return 0;
}

- (BOOL)_encodingCantBeStoredInEightBitCFString
{
    // unresolved
    return NO;
}

- (BOOL)_getCString:(char *)a0 maxLength:(unsigned long)a1 encoding:(unsigned int)a2
{
    // unresolved
    return NO;
}

- (struct __CFString *)_createSubstringWithRange:(NSRange)a0
{
    // unresolved
    return 0;
}

- (void)getLineStart:(unsigned long *)a0 end:(unsigned long *)a1 contentsEnd:(unsigned long *)a2 forRange:(NSRange)a3
{
    // unresolved

}

- (void)getParagraphStart:(unsigned long *)a0 end:(unsigned long *)a1 contentsEnd:(unsigned long *)a2 forRange:(NSRange)a3
{
    // unresolved

}

@end

@interface NSMutableString : NSString
- (void)replaceCharactersInRange:(NSRange)a0 withString:(id)a1;
- (void)appendString:(id)a0;
- (void)insertString:(id)a0 atIndex:(unsigned long)a1;
- (void)deleteCharactersInRange:(NSRange)a0;
- (void)setString:(id)a0;
- (void)appendCharacters:(unsigned short *)a0 length:(unsigned long)a1;
- (void)_cfAppendCString:(char *)a0 length:(long)a1;
- (unsigned long)replaceOccurrencesOfString:(id)a0 withString:(id)a1 options:(unsigned long)a2 range:(NSRange)a3;
- (void)_cfLowercase:(void *)a0;
- (void)_cfUppercase:(void *)a0;
- (void)_cfCapitalize:(void *)a0;
- (void)_cfNormalize:(long)a0;
- (void)_cfPad:(struct __CFString *)a0 length:(unsigned int)a1 padIndex:(unsigned int)a2;
- (void)_cfTrim:(struct __CFString *)a0;
- (void)_cfTrimWS;
@end

@implementation NSMutableString
- (void)replaceCharactersInRange:(NSRange)a0 withString:(id)a1
{
    // unresolved

}

- (void)appendString:(id)a0
{
    // unresolved

}

- (void)insertString:(id)a0 atIndex:(unsigned long)a1
{
    // unresolved

}

- (void)deleteCharactersInRange:(NSRange)a0
{
    // unresolved

}

- (void)setString:(id)a0
{
    // unresolved

}

- (void)appendCharacters:(unsigned short *)a0 length:(unsigned long)a1
{
    // unresolved

}

- (void)_cfAppendCString:(char *)a0 length:(long)a1
{
    // unresolved

}

- (unsigned long)replaceOccurrencesOfString:(id)a0 withString:(id)a1 options:(unsigned long)a2 range:(NSRange)a3
{
    // unresolved
    return 0;
}

- (void)_cfLowercase:(void *)a0
{
    // unresolved

}

- (void)_cfUppercase:(void *)a0
{
    // unresolved

}

- (void)_cfCapitalize:(void *)a0
{
    // unresolved

}

- (void)_cfNormalize:(long)a0
{
    // unresolved

}

- (void)_cfPad:(struct __CFString *)a0 length:(unsigned int)a1 padIndex:(unsigned int)a2
{
    // unresolved

}

- (void)_cfTrim:(struct __CFString *)a0
{
    // unresolved

}

- (void)_cfTrimWS
{
    // unresolved

}

@end

@interface __NSCFString : NSMutableString
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (unsigned long)length;
- (unsigned short)characterAtIndex:(unsigned long)a0;
- (void)getCharacters:(unsigned short *)a0 range:(NSRange)a1;
- (void)replaceCharactersInRange:(NSRange)a0 withString:(id)a1;
- (void)appendString:(id)a0;
- (void)setString:(id)a0;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSCFString
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [(REEF_INFO(self) & 1 ? [NSMutableString class] : [NSString class]) class];
}

- (unsigned long)length
{
    return (unsigned long)reefCall_CFStringGetLength(self);
}

- (unsigned short)characterAtIndex:(unsigned long)a0
{
    return (unsigned short)reefCall_CFStringGetCharacterAtIndex(self, a0);
}

- (void)getCharacters:(unsigned short *)a0 range:(NSRange)a1
{
    CFStringGetCharacters((CFStringRef)self, CFRangeMake(a1.location, a1.length), a0);
}

- (void)replaceCharactersInRange:(NSRange)a0 withString:(id)a1
{
    CFStringReplace((CFMutableStringRef)self, CFRangeMake(a0.location, a0.length), (CFStringRef)a1);
}

- (void)appendString:(id)a0
{
    reefCall_CFStringAppend(self, a0);
}

- (void)setString:(id)a0
{
    reefCall_CFStringReplaceAll(self, a0);
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return (id)CFStringCreateCopy(kCFAllocatorSystemDefault, (CFStringRef)self);
}

@end

@interface __NSCFConstantString : __NSCFString
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSCFConstantString
- (id)retain
{
    return self;
}

- (void)release
{

}

- (unsigned long)retainCount
{
    return 0xffffffffffffffff;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return self;
}

@end

@interface __NSCFAttributedString : NSObject
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)mutableCopyWithZone:(struct _NSZone *)a0;
- (id)string;
- (unsigned long)length;
- (id)attributesAtIndex:(unsigned long)a0 effectiveRange:(NSRange *)a1;
- (id)attribute:(id)a0 atIndex:(unsigned long)a1 effectiveRange:(NSRange *)a2;
- (id)attributesAtIndex:(unsigned long)a0 longestEffectiveRange:(NSRange *)a1 inRange:(NSRange)a2;
- (id)attribute:(id)a0 atIndex:(unsigned long)a1 longestEffectiveRange:(NSRange *)a2 inRange:(NSRange)a3;
- (struct __CFAttributedString *)_createAttributedSubstringWithRange:(NSRange)a0;
- (void)replaceCharactersInRange:(NSRange)a0 withString:(id)a1;
- (void)replaceCharactersInRange:(NSRange)a0 withAttributedString:(id)a1;
- (void)setAttributes:(id)a0 range:(NSRange)a1;
- (void)addAttributes:(id)a0 range:(NSRange)a1;
- (void)addAttribute:(id)a0 value:(id)a1 range:(NSRange)a2;
- (void)removeAttribute:(id)a0 range:(NSRange)a1;
- (void)beginEditing;
- (void)endEditing;
- (void)_beginHoldingEdits;
- (void)_endHoldingEdits;
@end

@implementation __NSCFAttributedString
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (Class)classForCoder
{
    // unresolved
    return nil;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)mutableCopyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)string
{
    return (id)reefCall_CFAttributedStringGetString(self);
}

- (unsigned long)length
{
    return (unsigned long)reefCall_CFAttributedStringGetLength(self);
}

- (id)attributesAtIndex:(unsigned long)a0 effectiveRange:(NSRange *)a1
{
    // unresolved
    return nil;
}

- (id)attribute:(id)a0 atIndex:(unsigned long)a1 effectiveRange:(NSRange *)a2
{
    // unresolved
    return nil;
}

- (id)attributesAtIndex:(unsigned long)a0 longestEffectiveRange:(NSRange *)a1 inRange:(NSRange)a2
{
    // unresolved
    return nil;
}

- (id)attribute:(id)a0 atIndex:(unsigned long)a1 longestEffectiveRange:(NSRange *)a2 inRange:(NSRange)a3
{
    // unresolved
    return nil;
}

- (struct __CFAttributedString *)_createAttributedSubstringWithRange:(NSRange)a0
{
    // unresolved
    return 0;
}

- (void)replaceCharactersInRange:(NSRange)a0 withString:(id)a1
{
    // unresolved

}

- (void)replaceCharactersInRange:(NSRange)a0 withAttributedString:(id)a1
{
    // unresolved

}

- (void)setAttributes:(id)a0 range:(NSRange)a1
{
    // unresolved

}

- (void)addAttributes:(id)a0 range:(NSRange)a1
{
    // unresolved

}

- (void)addAttribute:(id)a0 value:(id)a1 range:(NSRange)a2
{
    // unresolved

}

- (void)removeAttribute:(id)a0 range:(NSRange)a1
{
    // unresolved

}

- (void)beginEditing
{
    reefCall_CFAttributedStringBeginEditing(self);
}

- (void)endEditing
{
    reefCall_CFAttributedStringEndEditing(self);
}

- (void)_beginHoldingEdits
{

}

- (void)_endHoldingEdits
{
    // unresolved

}

@end

@interface NSTimer : NSObject
- (void)invalidate;
- (BOOL)isValid;
- (double)timeInterval;
- (double)tolerance;
- (void)setTolerance:(double)a0;
- (double)_cffireTime;
- (id)fireDate;
- (void)setFireDate:(id)a0;
- (id)userInfo;
- (void)fire;
- (unsigned long)_cfTypeID;
@end

@implementation NSTimer
- (void)invalidate
{
    // unresolved

}

- (BOOL)isValid
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (double)timeInterval
{
    // unresolved
    return 0;
}

- (double)tolerance
{
    // unresolved
    return 0;
}

- (void)setTolerance:(double)a0
{

}

- (double)_cffireTime
{
    // unresolved
    return 0;
}

- (id)fireDate
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (void)setFireDate:(id)a0
{
    // unresolved

}

- (id)userInfo
{
    return nil;
}

- (void)fire
{

}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFRunLoopTimerGetTypeID();
}

@end

@interface __NSCFTimer : NSTimer
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (void)invalidate;
- (BOOL)isValid;
- (double)timeInterval;
- (double)tolerance;
- (void)setTolerance:(double)a0;
- (double)_cffireTime;
- (id)fireDate;
- (void)setFireDate:(id)a0;
- (id)userInfo;
- (void)fire;
@end

@implementation __NSCFTimer
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (void)invalidate
{
    reefCall_CFRunLoopTimerInvalidate(self);
}

- (BOOL)isValid
{
    return CFRunLoopTimerIsValid((CFRunLoopTimerRef)self);
}

- (double)timeInterval
{
    return CFRunLoopTimerGetInterval((CFRunLoopTimerRef)self);
}

- (double)tolerance
{
    return CFRunLoopTimerGetTolerance((CFRunLoopTimerRef)self);
}

- (void)setTolerance:(double)a0
{
    CFRunLoopTimerSetTolerance((CFRunLoopTimerRef)self, a0);
}

- (double)_cffireTime
{
    return CFRunLoopTimerGetNextFireDate((CFRunLoopTimerRef)self);
}

- (id)fireDate
{
    id date = (id)CFDateCreate(kCFAllocatorSystemDefault, CFRunLoopTimerGetNextFireDate((CFRunLoopTimerRef)self));
    return date ? objc_autorelease(date) : nil;
}

- (void)setFireDate:(id)a0
{
    CFRunLoopTimerSetNextFireDate((CFRunLoopTimerRef)self, [a0 timeIntervalSinceReferenceDate]);
}

- (id)userInfo
{
    CFRunLoopTimerContext context = {0};
    CFRunLoopTimerGetContext((CFRunLoopTimerRef)self, &context);
    id info = (id)context.info;
    if ([info respondsToSelector:@selector(_reefUserInfo)]) return [info _reefUserInfo];
    return nil;
}

- (void)fire
{
    if (CFRunLoopTimerIsValid((CFRunLoopTimerRef)self)) {
        CFRunLoopTimerContext context = {0};
        CFRunLoopTimerGetContext((CFRunLoopTimerRef)self, &context);
        id info = objc_retain((id)context.info);
        if ([info respondsToSelector:@selector(_reefFireTimer:)]) [info _reefFireTimer:self];
        objc_release(info);
        if (!(CFRunLoopTimerGetInterval((CFRunLoopTimerRef)self) > 0)) CFRunLoopTimerInvalidate((CFRunLoopTimerRef)self);
    }
}

@end

@interface NSData : NSObject
- (unsigned long)length;
- (void *)bytes;
- (void)getBytes:(void *)a0 range:(NSRange)a1;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)mutableCopyWithZone:(struct _NSZone *)a0;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
@end

@implementation NSData
- (unsigned long)length
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (void *)bytes
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (void)getBytes:(void *)a0 range:(NSRange)a1
{
    // unresolved

}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)mutableCopyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    // unresolved
    return 0;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFDataGetTypeID();
}

@end

@interface NSMutableData : NSData
- (void *)mutableBytes;
- (void)setLength:(unsigned long)a0;
- (void *)bytes;
- (void)increaseLengthBy:(unsigned long)a0;
- (void)appendBytes:(void *)a0 length:(unsigned long)a1;
- (void)replaceBytesInRange:(NSRange)a0 withBytes:(void *)a1 length:(unsigned long)a2;
@end

@implementation NSMutableData
- (void *)mutableBytes
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (void)setLength:(unsigned long)a0
{
    // unresolved

}

- (void *)bytes
{
    return [self mutableBytes];
}

- (void)increaseLengthBy:(unsigned long)a0
{
    // unresolved

}

- (void)appendBytes:(void *)a0 length:(unsigned long)a1
{
    // unresolved

}

- (void)replaceBytesInRange:(NSRange)a0 withBytes:(void *)a1 length:(unsigned long)a2
{
    // unresolved

}

@end

@interface __NSCFData : NSMutableData
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (unsigned long)length;
- (void *)bytes;
- (void)getBytes:(void *)a0 range:(NSRange)a1;
- (void *)mutableBytes;
- (void)setLength:(unsigned long)a0;
- (void)increaseLengthBy:(unsigned long)a0;
- (void)appendBytes:(void *)a0 length:(unsigned long)a1;
- (void)replaceBytesInRange:(NSRange)a0 withBytes:(void *)a1 length:(unsigned long)a2;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSCFData
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [(REEF_INFO(self) & 1 ? [NSMutableData class] : [NSData class]) class];
}

- (unsigned long)length
{
    return (unsigned long)reefCall_CFDataGetLength(self);
}

- (void *)bytes
{
    return (void *)reefCall_CFDataGetBytePtr(self);
}

- (void)getBytes:(void *)a0 range:(NSRange)a1
{
    CFDataGetBytes((CFDataRef)self, CFRangeMake(a1.location, a1.length), a0);
}

- (void *)mutableBytes
{
    return (void *)reefCall_CFDataGetMutableBytePtr(self);
}

- (void)setLength:(unsigned long)a0
{
    reefCall_CFDataSetLength(self, a0);
}

- (void)increaseLengthBy:(unsigned long)a0
{
    reefCall_CFDataIncreaseLength(self, a0);
}

- (void)appendBytes:(void *)a0 length:(unsigned long)a1
{
    reefCall_CFDataAppendBytes(self, a0, a1);
}

- (void)replaceBytesInRange:(NSRange)a0 withBytes:(void *)a1 length:(unsigned long)a2
{
    CFDataReplaceBytes((CFMutableDataRef)self, CFRangeMake(a0.location, a0.length), a1, a2);
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return (id)CFDataCreateCopy(kCFAllocatorSystemDefault, (CFDataRef)self);
}

@end

@interface NSValue : NSObject
- (char *)objCType;
- (void)getValue:(void *)a0;
@end

@implementation NSValue
- (char *)objCType
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (void)getValue:(void *)a0
{
    // unresolved

}

@end

@interface NSNumber : NSValue
- (BOOL)charValue;
- (int)intValue;
- (long)longValue;
- (long)longLongValue;
- (long)integerValue;
- (unsigned int)unsignedIntValue;
- (unsigned long)unsignedLongLongValue;
- (unsigned long)unsignedIntegerValue;
- (float)floatValue;
- (double)doubleValue;
- (BOOL)boolValue;
- (id)description;
- (long)_cfNumberType;
- (BOOL)_getValue:(void *)a0 forType:(long)a1;
- (long)compare:(id)a0;
- (long)_reverseCompare:(id)a0;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (id)copyWithZone:(struct _NSZone *)a0;
- (unsigned long)_cfTypeID;
@end

@implementation NSNumber
- (BOOL)charValue
{
    // unresolved
    return NO;
}

- (int)intValue
{
    // unresolved
    return 0;
}

- (long)longValue
{
    // unresolved
    return 0;
}

- (long)longLongValue
{
    // unresolved
    return 0;
}

- (long)integerValue
{
    // unresolved
    return 0;
}

- (unsigned int)unsignedIntValue
{
    // unresolved
    return 0;
}

- (unsigned long)unsignedLongLongValue
{
    // unresolved
    return 0;
}

- (unsigned long)unsignedIntegerValue
{
    return [self unsignedLongLongValue];
}

- (float)floatValue
{
    // unresolved
    return 0;
}

- (double)doubleValue
{
    // unresolved
    return 0;
}

- (BOOL)boolValue
{
    // unresolved
    return NO;
}

- (id)description
{
    // unresolved
    return nil;
}

- (long)_cfNumberType
{
    // unresolved
    return 0;
}

- (BOOL)_getValue:(void *)a0 forType:(long)a1
{
    // unresolved
    return NO;
}

- (long)compare:(id)a0
{
    // unresolved
    return 0;
}

- (long)_reverseCompare:(id)a0
{
    // unresolved
    return 0;
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    // unresolved
    return 0;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFNumberGetTypeID();
}

@end

@interface __NSCFNumber : NSNumber
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (Class)classForCoder;
- (char *)objCType;
- (void)getValue:(void *)a0;
- (long)longLongValue;
- (double)doubleValue;
- (long)_cfNumberType;
- (BOOL)_getValue:(void *)a0 forType:(long)a1;
- (long)compare:(id)a0;
- (id)description;
@end

@implementation __NSCFNumber
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (Class)classForCoder
{
    return [NSNumber class];
}

- (char *)objCType
{
    switch (_CFNumberGetType2((CFNumberRef)self)) {
    case 1: case 7: return "c";
    case 2: case 8: return "s";
    case 3: case 9: return "i";
    case 5: case 12: return "f";
    case 6: case 13: case 16: return "d";
    case 17: return "Q";
    default: return "q";
    }
}

- (void)getValue:(void *)a0
{
    CFNumberType type = _CFNumberGetType2((CFNumberRef)self);
    if (type == 17) {
        unsigned long long wide[2] = {0, 0};
        CFNumberGetValue((CFNumberRef)self, 17, wide);
        *(unsigned long long *)a0 = wide[1];
        return;
    }
    if ((type & ~4L) == 10 || type == 15) type = 4;
    CFNumberGetValue((CFNumberRef)self, type, a0);
}

- (long)longLongValue
{
    long long value = 0;
    CFNumberGetValue((CFNumberRef)self, kCFNumberLongLongType, &value);
    return value;
}

- (double)doubleValue
{
    double value = 0;
    CFNumberGetValue((CFNumberRef)self, kCFNumberDoubleType, &value);
    return value;
}

- (long)_cfNumberType
{
    return (long)reefCall__CFNumberGetType2(self);
}

- (BOOL)_getValue:(void *)a0 forType:(long)a1
{
    return CFNumberGetValue((CFNumberRef)self, a1, a0);
}

- (long)compare:(id)a0
{
    return (long)reefCall_CFNumberCompare(self, a0, a0);
}

- (id)description
{
    // unresolved
    return nil;
}

@end

@interface __NSCFBoolean : NSNumber
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (Class)classForCoder;
- (char *)objCType;
- (void)getValue:(void *)a0;
- (BOOL)boolValue;
- (long)_cfNumberType;
- (id)description;
@end

@implementation __NSCFBoolean
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (Class)classForCoder
{
    return [NSNumber class];
}

- (char *)objCType
{
    return "c";
}

- (void)getValue:(void *)a0
{
    *(char *)a0 = CFBooleanGetValue((CFBooleanRef)self);
}

- (BOOL)boolValue
{
    return CFBooleanGetValue((CFBooleanRef)self);
}

- (long)_cfNumberType
{
    return 7;
}

- (id)description
{
    return (id)(CFBooleanGetValue((CFBooleanRef)self) ? CFSTR("1") : CFSTR("0"));
}

@end

@interface NSCharacterSet : NSObject
- (BOOL)characterIsMember:(unsigned short)a0;
- (BOOL)longCharacterIsMember:(unsigned int)a0;
- (BOOL)hasMemberInPlane:(unsigned char)a0;
- (id)invertedSet;
- (struct __CFCharacterSet *)_expandedCFCharacterSet;
- (struct __CFData *)_retainedBitmapRepresentation;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)mutableCopyWithZone:(struct _NSZone *)a0;
- (unsigned long)_cfTypeID;
@end

@implementation NSCharacterSet
- (BOOL)characterIsMember:(unsigned short)a0
{
    [self doesNotRecognizeSelector:_cmd];
    return 0;
}

- (BOOL)longCharacterIsMember:(unsigned int)a0
{
    // unresolved
    return NO;
}

- (BOOL)hasMemberInPlane:(unsigned char)a0
{
    // unresolved
    return NO;
}

- (id)invertedSet
{
    // unresolved
    return nil;
}

- (struct __CFCharacterSet *)_expandedCFCharacterSet
{
    // unresolved
    return 0;
}

- (struct __CFData *)_retainedBitmapRepresentation
{
    // unresolved
    return 0;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (id)mutableCopyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFCharacterSetGetTypeID();
}

@end

@interface NSMutableCharacterSet : NSCharacterSet
- (void)addCharactersInRange:(NSRange)a0;
- (void)removeCharactersInRange:(NSRange)a0;
- (void)addCharactersInString:(id)a0;
- (void)removeCharactersInString:(id)a0;
- (void)formUnionWithCharacterSet:(id)a0;
- (void)formIntersectionWithCharacterSet:(id)a0;
- (void)invert;
@end

@implementation NSMutableCharacterSet
- (void)addCharactersInRange:(NSRange)a0
{
    // unresolved

}

- (void)removeCharactersInRange:(NSRange)a0
{
    // unresolved

}

- (void)addCharactersInString:(id)a0
{
    // unresolved

}

- (void)removeCharactersInString:(id)a0
{
    // unresolved

}

- (void)formUnionWithCharacterSet:(id)a0
{
    // unresolved

}

- (void)formIntersectionWithCharacterSet:(id)a0
{
    // unresolved

}

- (void)invert
{
    // unresolved

}

@end

@interface __NSCFCharacterSet : NSMutableCharacterSet
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (BOOL)characterIsMember:(unsigned short)a0;
- (BOOL)longCharacterIsMember:(unsigned int)a0;
- (BOOL)hasMemberInPlane:(unsigned char)a0;
- (id)invertedSet;
- (struct __CFCharacterSet *)_expandedCFCharacterSet;
- (struct __CFData *)_retainedBitmapRepresentation;
- (void)addCharactersInRange:(NSRange)a0;
- (void)removeCharactersInRange:(NSRange)a0;
- (void)addCharactersInString:(id)a0;
- (void)removeCharactersInString:(id)a0;
- (void)formUnionWithCharacterSet:(id)a0;
- (void)formIntersectionWithCharacterSet:(id)a0;
- (void)invert;
- (id)copyWithZone:(struct _NSZone *)a0;
@end

@implementation __NSCFCharacterSet
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [(REEF_INFO(self) & 1 ? [NSMutableCharacterSet class] : [NSCharacterSet class]) class];
}

- (BOOL)characterIsMember:(unsigned short)a0
{
    return CFCharacterSetIsCharacterMember((CFCharacterSetRef)self, a0);
}

- (BOOL)longCharacterIsMember:(unsigned int)a0
{
    return CFCharacterSetIsLongCharacterMember((CFCharacterSetRef)self, a0);
}

- (BOOL)hasMemberInPlane:(unsigned char)a0
{
    return CFCharacterSetHasMemberInPlane((CFCharacterSetRef)self, a0);
}

- (id)invertedSet
{
    id result = (id)CFCharacterSetCreateInvertedSet(kCFAllocatorSystemDefault, (CFCharacterSetRef)self);
    return result ? objc_autorelease(result) : nil;
}

- (struct __CFCharacterSet *)_expandedCFCharacterSet
{
    return self;
}

- (struct __CFData *)_retainedBitmapRepresentation
{
    return (id)CFCharacterSetCreateBitmapRepresentation(kCFAllocatorSystemDefault, (CFCharacterSetRef)self);
}

- (void)addCharactersInRange:(NSRange)a0
{
    CFCharacterSetAddCharactersInRange((CFMutableCharacterSetRef)self, CFRangeMake(a0.location, a0.length));
}

- (void)removeCharactersInRange:(NSRange)a0
{
    CFCharacterSetRemoveCharactersInRange((CFMutableCharacterSetRef)self, CFRangeMake(a0.location, a0.length));
}

- (void)addCharactersInString:(id)a0
{
    reefCall_CFCharacterSetAddCharactersInString(self, a0);
}

- (void)removeCharactersInString:(id)a0
{
    reefCall_CFCharacterSetRemoveCharactersInString(self, a0);
}

- (void)formUnionWithCharacterSet:(id)a0
{
    reefCall_CFCharacterSetUnion(self, a0);
}

- (void)formIntersectionWithCharacterSet:(id)a0
{
    reefCall_CFCharacterSetIntersect(self, a0);
}

- (void)invert
{
    reefCall_CFCharacterSetInvert(self);
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return (id)CFCharacterSetCreateCopy(kCFAllocatorSystemDefault, (CFCharacterSetRef)self);
}

@end

@interface NSError : NSObject
{
    void * _reserved;
    long _code;
    NSString * _domain;
    NSDictionary * _userInfo;
}
+ (id)errorWithDomain:(id)a0 code:(long)a1 userInfo:(id)a2;
- (id)initWithDomain:(id)a0 code:(long)a1 userInfo:(id)a2;
- (void)dealloc;
- (id)domain;
- (long)code;
- (id)userInfo;
- (id)localizedDescription;
- (id)localizedFailureReason;
- (id)localizedRecoverySuggestion;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (id)copyWithZone:(struct _NSZone *)a0;
- (unsigned long)_cfTypeID;
@end

@implementation NSError
+ (id)errorWithDomain:(id)a0 code:(long)a1 userInfo:(id)a2
{
    // unresolved
    return nil;
}

- (id)initWithDomain:(id)a0 code:(long)a1 userInfo:(id)a2
{
    // unresolved
    return nil;
}

- (void)dealloc
{
    // unresolved

}

- (id)domain
{
    return _domain;
}

- (long)code
{
    return _code;
}

- (id)userInfo
{
    // unresolved
    return nil;
}

- (id)localizedDescription
{
    // unresolved
    return nil;
}

- (id)localizedFailureReason
{
    // unresolved
    return nil;
}

- (id)localizedRecoverySuggestion
{
    // unresolved
    return nil;
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    // unresolved
    return 0;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    // unresolved
    return nil;
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFErrorGetTypeID();
}

@end

@interface __NSCFError : NSError
- (id)retain;
- (void)release;
- (unsigned long)retainCount;
- (BOOL)allowsWeakReference;
- (BOOL)retainWeakReference;
- (void)_setWeaklyReferenced;
- (unsigned long)hash;
- (unsigned long)_cfTypeID;
- (BOOL)isEqual:(id)a0;
- (Class)classForCoder;
- (id)copyWithZone:(struct _NSZone *)a0;
- (id)domain;
- (long)code;
- (id)userInfo;
- (id)localizedDescription;
- (id)localizedFailureReason;
- (id)localizedRecoverySuggestion;
@end

@implementation __NSCFError
- (id)retain
{
    return (id)reefCall_CFRetain(self);
}

- (void)release
{
    reefCall_CFRelease(self);
}

- (unsigned long)retainCount
{
    return (unsigned long)reefCall_CFGetRetainCount(self);
}

- (BOOL)allowsWeakReference
{
    return !(unsigned char)reefCall__CFIsDeallocating((long)self);
}

- (BOOL)retainWeakReference
{
    return reefCall__CFTryRetain((long)self) != 0;
}

- (void)_setWeaklyReferenced
{
    reefCall__CFSetWeaklyReferenced(self);
}

- (unsigned long)hash
{
    return (unsigned long)reefCall_CFHash(self);
}

- (unsigned long)_cfTypeID
{
    return (unsigned long)reefCall_CFGetTypeID(self);
}

- (BOOL)isEqual:(id)a0
{
    if (a0 == 0) return 0;
    return (BOOL)reefCall_CFEqual((long)self, (long)a0);
}

- (Class)classForCoder
{
    return [NSError class];
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return objc_retain(self);
}

- (id)domain
{
    return (id)reefCall_CFErrorGetDomain(self);
}

- (long)code
{
    return (long)reefCall_CFErrorGetCode(self);
}

- (id)userInfo
{
    id result = (id)CFErrorCopyUserInfo((CFErrorRef)self);
    return result ? objc_autorelease(result) : nil;
}

- (id)localizedDescription
{
    id result = (id)CFErrorCopyDescription((CFErrorRef)self);
    return result ? objc_autorelease(result) : nil;
}

- (id)localizedFailureReason
{
    id result = (id)CFErrorCopyFailureReason((CFErrorRef)self);
    return result ? objc_autorelease(result) : nil;
}

- (id)localizedRecoverySuggestion
{
    id result = (id)CFErrorCopyRecoverySuggestion((CFErrorRef)self);
    return result ? objc_autorelease(result) : nil;
}

@end

@interface NSConstantIntegerNumber : NSNumber
{
    char * _encoding;
    long _value;
}
- (id)retain;
- (void)release;
- (id)autorelease;
- (unsigned long)retainCount;
- (id)copyWithZone:(struct _NSZone *)a0;
- (char *)objCType;
- (void)getValue:(void *)a0;
@end

@implementation NSConstantIntegerNumber
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)autorelease
{
    return self;
}

- (unsigned long)retainCount
{
    return 0xffffffffffffffff;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return self;
}

- (char *)objCType
{
    // unresolved
    return 0;
}

- (void)getValue:(void *)a0
{
    // unresolved

}

@end

@interface NSConstantFloatNumber : NSNumber
{
    float _value;
}
- (id)retain;
- (void)release;
- (id)autorelease;
- (unsigned long)retainCount;
- (id)copyWithZone:(struct _NSZone *)a0;
- (char *)objCType;
- (void)getValue:(void *)a0;
@end

@implementation NSConstantFloatNumber
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)autorelease
{
    return self;
}

- (unsigned long)retainCount
{
    return 0xffffffffffffffff;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return self;
}

- (char *)objCType
{
    // unresolved
    return 0;
}

- (void)getValue:(void *)a0
{
    // unresolved

}

@end

@interface NSConstantDoubleNumber : NSNumber
{
    double _value;
}
- (id)retain;
- (void)release;
- (id)autorelease;
- (unsigned long)retainCount;
- (id)copyWithZone:(struct _NSZone *)a0;
- (char *)objCType;
- (void)getValue:(void *)a0;
@end

@implementation NSConstantDoubleNumber
- (id)retain
{
    return self;
}

- (void)release
{

}

- (id)autorelease
{
    return self;
}

- (unsigned long)retainCount
{
    return 0xffffffffffffffff;
}

- (id)copyWithZone:(struct _NSZone *)a0
{
    return self;
}

- (char *)objCType
{
    // unresolved
    return 0;
}

- (void)getValue:(void *)a0
{
    // unresolved

}

@end

@interface NSMethodSignature : NSObject
{
    struct ReefSignature * _signature;
}
+ (id)signatureWithObjCTypes:(char *)a0;
- (void)dealloc;
- (struct ReefSignature *)_reefSignature;
- (unsigned long)numberOfArguments;
- (char *)getArgumentTypeAtIndex:(unsigned long)a0;
- (unsigned long)frameLength;
- (BOOL)isOneway;
- (char *)methodReturnType;
- (unsigned long)methodReturnLength;
- (BOOL)isEqual:(id)a0;
- (unsigned long)hash;
- (id)description;
@end

@implementation NSMethodSignature
+ (id)signatureWithObjCTypes:(char *)a0
{
    // unresolved
    return nil;
}

- (void)dealloc
{
    // unresolved

}

- (struct ReefSignature *)_reefSignature
{
    // unresolved
    return 0;
}

- (unsigned long)numberOfArguments
{
    // unresolved
    return 0;
}

- (char *)getArgumentTypeAtIndex:(unsigned long)a0
{
    // unresolved
    return 0;
}

- (unsigned long)frameLength
{
    // unresolved
    return 0;
}

- (BOOL)isOneway
{
    // unresolved
    return NO;
}

- (char *)methodReturnType
{
    // unresolved
    return 0;
}

- (unsigned long)methodReturnLength
{
    // unresolved
    return 0;
}

- (BOOL)isEqual:(id)a0
{
    // unresolved
    return NO;
}

- (unsigned long)hash
{
    // unresolved
    return 0;
}

- (id)description
{
    // unresolved
    return nil;
}

@end

@interface NSInvocation : NSObject
{
    NSMethodSignature * _signature;
    struct ReefFrame * _frame;
    void * _result;
    void * * _indirect;
    BOOL _retained;
}
+ (id)invocationWithMethodSignature:(id)a0;
- (id)_initWithMethodSignature:(id)a0 frame:(struct ReefFrame *)a1;
- (void)dealloc;
- (id)methodSignature;
- (void)_checkIndex:(long)a0;
- (void)getArgument:(void *)a0 atIndex:(long)a1;
- (void)setArgument:(void *)a0 atIndex:(long)a1;
- (id)target;
- (void)setTarget:(id)a0;
- (SEL)selector;
- (void)setSelector:(SEL)a0;
- (void)getReturnValue:(void *)a0;
- (void)setReturnValue:(void *)a0;
- (BOOL)argumentsRetained;
- (void)retainArguments;
- (void)invoke;
- (void)invokeWithTarget:(id)a0;
- (void)invokeUsingIMP:(void * *)a0;
- (void)_invokeUsingIMP:(void * *)a0;
- (void)_storeResultInFrame:(struct ReefFrame *)a0;
- (id)description;
@end

@implementation NSInvocation
+ (id)invocationWithMethodSignature:(id)a0
{
    // unresolved
    return nil;
}

- (id)_initWithMethodSignature:(id)a0 frame:(struct ReefFrame *)a1
{
    // unresolved
    return nil;
}

- (void)dealloc
{
    // unresolved

}

- (id)methodSignature
{
    // unresolved
    return nil;
}

- (void)_checkIndex:(long)a0
{
    // unresolved

}

- (void)getArgument:(void *)a0 atIndex:(long)a1
{
    // unresolved

}

- (void)setArgument:(void *)a0 atIndex:(long)a1
{
    // unresolved

}

- (id)target
{
    // unresolved
    return nil;
}

- (void)setTarget:(id)a0
{
    // unresolved

}

- (SEL)selector
{
    // unresolved
    return 0;
}

- (void)setSelector:(SEL)a0
{
    // unresolved

}

- (void)getReturnValue:(void *)a0
{
    // unresolved

}

- (void)setReturnValue:(void *)a0
{
    // unresolved

}

- (BOOL)argumentsRetained
{
    return _retained;
}

- (void)retainArguments
{
    // unresolved

}

- (void)invoke
{
    // unresolved

}

- (void)invokeWithTarget:(id)a0
{
    // unresolved

}

- (void)invokeUsingIMP:(void * *)a0
{
    [self _invokeUsingIMP:a0];
}

- (void)_invokeUsingIMP:(void * *)a0
{
    // unresolved

}

- (void)_storeResultInFrame:(struct ReefFrame *)a0
{
    // unresolved

}

- (id)description
{
    // unresolved
    return nil;
}

@end

NSString *const reefConstant_NSDefaultRunLoopMode __asm__("_NSDefaultRunLoopMode") = @"kCFRunLoopDefaultMode";
NSString *const reefConstant_NSRunLoopCommonModes __asm__("_NSRunLoopCommonModes") = @"kCFRunLoopCommonModes";
NSString *const reefConstant_kCFStreamPropertyShouldCloseNativeSocket __asm__("_kCFStreamPropertyShouldCloseNativeSocket") = @"kCFStreamPropertyShouldCloseNativeSocket";
NSString *const reefConstant_kCFStreamPropertyAutoErrorOnSystemChange __asm__("_kCFStreamPropertyAutoErrorOnSystemChange") = @"kCFStreamPropertyAutoErrorOnSystemChange";
NSString *const reefConstant_NSStreamSOCKSProxyConfigurationKey __asm__("_NSStreamSOCKSProxyConfigurationKey") = @"kCFStreamPropertySOCKSProxy";
NSString *const reefConstant_kCFStreamPropertySOCKSProxy __asm__("_kCFStreamPropertySOCKSProxy") = @"kCFStreamPropertySOCKSProxy";
NSString *const reefConstant_NSStreamSOCKSProxyHostKey __asm__("_NSStreamSOCKSProxyHostKey") = @"SOCKSProxy";
NSString *const reefConstant_kCFStreamPropertySOCKSProxyHost __asm__("_kCFStreamPropertySOCKSProxyHost") = @"SOCKSProxy";
NSString *const reefConstant_NSStreamSOCKSProxyPortKey __asm__("_NSStreamSOCKSProxyPortKey") = @"SOCKSPort";
NSString *const reefConstant_kCFStreamPropertySOCKSProxyPort __asm__("_kCFStreamPropertySOCKSProxyPort") = @"SOCKSPort";
NSString *const reefConstant_NSStreamSOCKSProxyVersionKey __asm__("_NSStreamSOCKSProxyVersionKey") = @"kCFStreamPropertySOCKSVersion";
NSString *const reefConstant_kCFStreamPropertySOCKSVersion __asm__("_kCFStreamPropertySOCKSVersion") = @"kCFStreamPropertySOCKSVersion";
NSString *const reefConstant_NSStreamSOCKSProxyVersion4 __asm__("_NSStreamSOCKSProxyVersion4") = @"kCFStreamSocketSOCKSVersion4";
NSString *const reefConstant_kCFStreamSocketSOCKSVersion4 __asm__("_kCFStreamSocketSOCKSVersion4") = @"kCFStreamSocketSOCKSVersion4";
NSString *const reefConstant_NSStreamSOCKSProxyVersion5 __asm__("_NSStreamSOCKSProxyVersion5") = @"kCFStreamSocketSOCKSVersion5";
NSString *const reefConstant_kCFStreamSocketSOCKSVersion5 __asm__("_kCFStreamSocketSOCKSVersion5") = @"kCFStreamSocketSOCKSVersion5";
NSString *const reefConstant_NSStreamSOCKSProxyUserKey __asm__("_NSStreamSOCKSProxyUserKey") = @"kCFStreamPropertySOCKSUser";
NSString *const reefConstant_kCFStreamPropertySOCKSUser __asm__("_kCFStreamPropertySOCKSUser") = @"kCFStreamPropertySOCKSUser";
NSString *const reefConstant_NSStreamSOCKSProxyPasswordKey __asm__("_NSStreamSOCKSProxyPasswordKey") = @"kCFStreamPropertySOCKSPassword";
NSString *const reefConstant_kCFStreamPropertySOCKSPassword __asm__("_kCFStreamPropertySOCKSPassword") = @"kCFStreamPropertySOCKSPassword";
NSString *const reefConstant_NSStreamSocketSecurityLevelKey __asm__("_NSStreamSocketSecurityLevelKey") = @"kCFStreamPropertySocketSecurityLevel";
NSString *const reefConstant_kCFStreamPropertySocketSecurityLevel __asm__("_kCFStreamPropertySocketSecurityLevel") = @"kCFStreamPropertySocketSecurityLevel";
NSString *const reefConstant_NSStreamSocketSecurityLevelNone __asm__("_NSStreamSocketSecurityLevelNone") = @"kCFStreamSocketSecurityLevelNone";
NSString *const reefConstant_kCFStreamSocketSecurityLevelNone __asm__("_kCFStreamSocketSecurityLevelNone") = @"kCFStreamSocketSecurityLevelNone";
NSString *const reefConstant_NSStreamSocketSecurityLevelSSLv2 __asm__("_NSStreamSocketSecurityLevelSSLv2") = @"kCFStreamSocketSecurityLevelSSLv2";
NSString *const reefConstant_kCFStreamSocketSecurityLevelSSLv2 __asm__("_kCFStreamSocketSecurityLevelSSLv2") = @"kCFStreamSocketSecurityLevelSSLv2";
NSString *const reefConstant_NSStreamSocketSecurityLevelSSLv3 __asm__("_NSStreamSocketSecurityLevelSSLv3") = @"kCFStreamSocketSecurityLevelSSLv3";
NSString *const reefConstant_kCFStreamSocketSecurityLevelSSLv3 __asm__("_kCFStreamSocketSecurityLevelSSLv3") = @"kCFStreamSocketSecurityLevelSSLv3";
NSString *const reefConstant_NSStreamSocketSecurityLevelTLSv1 __asm__("_NSStreamSocketSecurityLevelTLSv1") = @"kCFStreamSocketSecurityLevelTLSv1";
NSString *const reefConstant_kCFStreamSocketSecurityLevelTLSv1 __asm__("_kCFStreamSocketSecurityLevelTLSv1") = @"kCFStreamSocketSecurityLevelTLSv1";
NSString *const reefConstant_NSStreamSocketSecurityLevelNegotiatedSSL __asm__("_NSStreamSocketSecurityLevelNegotiatedSSL") = @"kCFStreamSocketSecurityLevelNegotiatedSSL";
NSString *const reefConstant_kCFStreamSocketSecurityLevelNegotiatedSSL __asm__("_kCFStreamSocketSecurityLevelNegotiatedSSL") = @"kCFStreamSocketSecurityLevelNegotiatedSSL";
NSString *const reefConstant_NSGenericException __asm__("_NSGenericException") = @"NSGenericException";
NSString *const reefConstant_NSRangeException __asm__("_NSRangeException") = @"NSRangeException";
NSString *const reefConstant_NSInvalidArgumentException __asm__("_NSInvalidArgumentException") = @"NSInvalidArgumentException";
NSString *const reefConstant_NSInternalInconsistencyException __asm__("_NSInternalInconsistencyException") = @"NSInternalInconsistencyException";
NSString *const reefConstant_NSMallocException __asm__("_NSMallocException") = @"NSMallocException";
NSString *const reefConstant_NSObjectInaccessibleException __asm__("_NSObjectInaccessibleException") = @"NSObjectInaccessibleException";
NSString *const reefConstant_NSObjectNotAvailableException __asm__("_NSObjectNotAvailableException") = @"NSObjectNotAvailableException";
NSString *const reefConstant_NSDestinationInvalidException __asm__("_NSDestinationInvalidException") = @"NSDestinationInvalidException";
NSString *const reefConstant_NSPortTimeoutException __asm__("_NSPortTimeoutException") = @"NSPortTimeoutException";
NSString *const reefConstant_NSInvalidSendPortException __asm__("_NSInvalidSendPortException") = @"NSInvalidSendPortException";
NSString *const reefConstant_NSInvalidReceivePortException __asm__("_NSInvalidReceivePortException") = @"NSInvalidReceivePortException";
NSString *const reefConstant_NSPortSendException __asm__("_NSPortSendException") = @"NSPortSendException";
NSString *const reefConstant_NSPortReceiveException __asm__("_NSPortReceiveException") = @"NSPortReceiveException";
NSString *const reefConstant_NSCharacterConversionException __asm__("_NSCharacterConversionException") = @"NSCharacterConversionException";
NSString *const reefConstant_NSFileHandleOperationException __asm__("_NSFileHandleOperationException") = @"NSFileHandleOperationException";
NSString *const reefConstant_NSStreamSocketSSLErrorDomain __asm__("_NSStreamSocketSSLErrorDomain") = @"NSStreamSocketSSLErrorDomain";
NSString *const reefConstant_NSStreamSOCKSErrorDomain __asm__("_NSStreamSOCKSErrorDomain") = @"NSStreamSOCKSErrorDomain";
NSString *const reefConstant_NSStreamNetworkServiceType __asm__("_NSStreamNetworkServiceType") = @"kCFStreamNetworkServiceType";
NSString *const reefConstant_NSStreamNetworkServiceTypeVoIP __asm__("_NSStreamNetworkServiceTypeVoIP") = @"kCFStreamNetworkServiceTypeVoIP";
NSString *const reefConstant_NSStreamNetworkServiceTypeVideo __asm__("_NSStreamNetworkServiceTypeVideo") = @"kCFStreamNetworkServiceTypeVideo";
NSString *const reefConstant_NSStreamNetworkServiceTypeBackground __asm__("_NSStreamNetworkServiceTypeBackground") = @"kCFStreamNetworkServiceTypeBackground";
NSString *const reefConstant_NSStreamNetworkServiceTypeVoice __asm__("_NSStreamNetworkServiceTypeVoice") = @"kCFStreamNetworkServiceTypeVoice";
NSString *const reefConstant_NSStreamNetworkServiceTypeCallSignaling __asm__("_NSStreamNetworkServiceTypeCallSignaling") = @"kCFStreamNetworkServiceTypeCallSignaling";
NSString *const reefConstant_NSURLNameKey __asm__("_NSURLNameKey") = @"NSURLNameKey";
NSString *const reefConstant_kCFURLNameKey __asm__("_kCFURLNameKey") = @"NSURLNameKey";
NSString *const reefConstant_NSURLLocalizedNameKey __asm__("_NSURLLocalizedNameKey") = @"NSURLLocalizedNameKey";
NSString *const reefConstant_kCFURLLocalizedNameKey __asm__("_kCFURLLocalizedNameKey") = @"NSURLLocalizedNameKey";
NSString *const reefConstant_NSURLIsRegularFileKey __asm__("_NSURLIsRegularFileKey") = @"NSURLIsRegularFileKey";
NSString *const reefConstant_kCFURLIsRegularFileKey __asm__("_kCFURLIsRegularFileKey") = @"NSURLIsRegularFileKey";
NSString *const reefConstant_NSURLIsDirectoryKey __asm__("_NSURLIsDirectoryKey") = @"NSURLIsDirectoryKey";
NSString *const reefConstant_kCFURLIsDirectoryKey __asm__("_kCFURLIsDirectoryKey") = @"NSURLIsDirectoryKey";
NSString *const reefConstant_NSURLIsSymbolicLinkKey __asm__("_NSURLIsSymbolicLinkKey") = @"NSURLIsSymbolicLinkKey";
NSString *const reefConstant_kCFURLIsSymbolicLinkKey __asm__("_kCFURLIsSymbolicLinkKey") = @"NSURLIsSymbolicLinkKey";
NSString *const reefConstant_NSURLIsVolumeKey __asm__("_NSURLIsVolumeKey") = @"NSURLIsVolumeKey";
NSString *const reefConstant_kCFURLIsVolumeKey __asm__("_kCFURLIsVolumeKey") = @"NSURLIsVolumeKey";
NSString *const reefConstant_NSURLIsPackageKey __asm__("_NSURLIsPackageKey") = @"NSURLIsPackageKey";
NSString *const reefConstant_kCFURLIsPackageKey __asm__("_kCFURLIsPackageKey") = @"NSURLIsPackageKey";
NSString *const reefConstant_NSURLIsApplicationKey __asm__("_NSURLIsApplicationKey") = @"NSURLIsApplicationKey";
NSString *const reefConstant_kCFURLIsApplicationKey __asm__("_kCFURLIsApplicationKey") = @"NSURLIsApplicationKey";
NSString *const reefConstant_NSURLApplicationIsScriptableKey __asm__("_NSURLApplicationIsScriptableKey") = @"NSURLApplicationIsScriptableKey";
NSString *const reefConstant_kCFURLApplicationIsScriptableKey __asm__("_kCFURLApplicationIsScriptableKey") = @"NSURLApplicationIsScriptableKey";
NSString *const reefConstant_NSURLIsSystemImmutableKey __asm__("_NSURLIsSystemImmutableKey") = @"NSURLIsSystemImmutableKey";
NSString *const reefConstant_kCFURLIsSystemImmutableKey __asm__("_kCFURLIsSystemImmutableKey") = @"NSURLIsSystemImmutableKey";
NSString *const reefConstant_NSURLIsUserImmutableKey __asm__("_NSURLIsUserImmutableKey") = @"NSURLIsUserImmutableKey";
NSString *const reefConstant_kCFURLIsUserImmutableKey __asm__("_kCFURLIsUserImmutableKey") = @"NSURLIsUserImmutableKey";
NSString *const reefConstant_NSURLIsHiddenKey __asm__("_NSURLIsHiddenKey") = @"NSURLIsHiddenKey";
NSString *const reefConstant_kCFURLIsHiddenKey __asm__("_kCFURLIsHiddenKey") = @"NSURLIsHiddenKey";
NSString *const reefConstant_NSURLHasHiddenExtensionKey __asm__("_NSURLHasHiddenExtensionKey") = @"NSURLHasHiddenExtensionKey";
NSString *const reefConstant_kCFURLHasHiddenExtensionKey __asm__("_kCFURLHasHiddenExtensionKey") = @"NSURLHasHiddenExtensionKey";
NSString *const reefConstant_NSURLCreationDateKey __asm__("_NSURLCreationDateKey") = @"NSURLCreationDateKey";
NSString *const reefConstant_kCFURLCreationDateKey __asm__("_kCFURLCreationDateKey") = @"NSURLCreationDateKey";
NSString *const reefConstant_NSURLContentAccessDateKey __asm__("_NSURLContentAccessDateKey") = @"NSURLContentAccessDateKey";
NSString *const reefConstant_kCFURLContentAccessDateKey __asm__("_kCFURLContentAccessDateKey") = @"NSURLContentAccessDateKey";
NSString *const reefConstant_NSURLContentModificationDateKey __asm__("_NSURLContentModificationDateKey") = @"NSURLContentModificationDateKey";
NSString *const reefConstant_kCFURLContentModificationDateKey __asm__("_kCFURLContentModificationDateKey") = @"NSURLContentModificationDateKey";
NSString *const reefConstant_NSURLAttributeModificationDateKey __asm__("_NSURLAttributeModificationDateKey") = @"NSURLAttributeModificationDateKey";
NSString *const reefConstant_kCFURLAttributeModificationDateKey __asm__("_kCFURLAttributeModificationDateKey") = @"NSURLAttributeModificationDateKey";
NSString *const reefConstant_NSURLLinkCountKey __asm__("_NSURLLinkCountKey") = @"NSURLLinkCountKey";
NSString *const reefConstant_kCFURLLinkCountKey __asm__("_kCFURLLinkCountKey") = @"NSURLLinkCountKey";
NSString *const reefConstant_NSURLParentDirectoryURLKey __asm__("_NSURLParentDirectoryURLKey") = @"NSURLParentDirectoryURLKey";
NSString *const reefConstant_kCFURLParentDirectoryURLKey __asm__("_kCFURLParentDirectoryURLKey") = @"NSURLParentDirectoryURLKey";
NSString *const reefConstant_NSURLVolumeURLKey __asm__("_NSURLVolumeURLKey") = @"NSURLVolumeURLKey";
NSString *const reefConstant_kCFURLVolumeURLKey __asm__("_kCFURLVolumeURLKey") = @"NSURLVolumeURLKey";
NSString *const reefConstant_NSURLTypeIdentifierKey __asm__("_NSURLTypeIdentifierKey") = @"NSURLTypeIdentifierKey";
NSString *const reefConstant_kCFURLTypeIdentifierKey __asm__("_kCFURLTypeIdentifierKey") = @"NSURLTypeIdentifierKey";
NSString *const reefConstant_NSURLLocalizedTypeDescriptionKey __asm__("_NSURLLocalizedTypeDescriptionKey") = @"NSURLLocalizedTypeDescriptionKey";
NSString *const reefConstant_kCFURLLocalizedTypeDescriptionKey __asm__("_kCFURLLocalizedTypeDescriptionKey") = @"NSURLLocalizedTypeDescriptionKey";
NSString *const reefConstant_NSURLLabelNumberKey __asm__("_NSURLLabelNumberKey") = @"NSURLLabelNumberKey";
NSString *const reefConstant_kCFURLLabelNumberKey __asm__("_kCFURLLabelNumberKey") = @"NSURLLabelNumberKey";
NSString *const reefConstant_NSURLLabelColorKey __asm__("_NSURLLabelColorKey") = @"NSURLLabelColorKey";
NSString *const reefConstant_kCFURLLabelColorKey __asm__("_kCFURLLabelColorKey") = @"NSURLLabelColorKey";
NSString *const reefConstant_NSURLLocalizedLabelKey __asm__("_NSURLLocalizedLabelKey") = @"NSURLLocalizedLabelKey";
NSString *const reefConstant_kCFURLLocalizedLabelKey __asm__("_kCFURLLocalizedLabelKey") = @"NSURLLocalizedLabelKey";
NSString *const reefConstant_NSURLEffectiveIconKey __asm__("_NSURLEffectiveIconKey") = @"NSURLEffectiveIconKey";
NSString *const reefConstant_kCFURLEffectiveIconKey __asm__("_kCFURLEffectiveIconKey") = @"NSURLEffectiveIconKey";
NSString *const reefConstant_NSURLCustomIconKey __asm__("_NSURLCustomIconKey") = @"NSURLCustomIconKey";
NSString *const reefConstant_kCFURLCustomIconKey __asm__("_kCFURLCustomIconKey") = @"NSURLCustomIconKey";
NSString *const reefConstant_NSURLFileResourceIdentifierKey __asm__("_NSURLFileResourceIdentifierKey") = @"NSURLFileResourceIdentifierKey";
NSString *const reefConstant_kCFURLFileResourceIdentifierKey __asm__("_kCFURLFileResourceIdentifierKey") = @"NSURLFileResourceIdentifierKey";
NSString *const reefConstant_NSURLVolumeIdentifierKey __asm__("_NSURLVolumeIdentifierKey") = @"NSURLVolumeIdentifierKey";
NSString *const reefConstant_kCFURLVolumeIdentifierKey __asm__("_kCFURLVolumeIdentifierKey") = @"NSURLVolumeIdentifierKey";
NSString *const reefConstant_NSURLPreferredIOBlockSizeKey __asm__("_NSURLPreferredIOBlockSizeKey") = @"NSURLPreferredIOBlockSizeKey";
NSString *const reefConstant_kCFURLPreferredIOBlockSizeKey __asm__("_kCFURLPreferredIOBlockSizeKey") = @"NSURLPreferredIOBlockSizeKey";
NSString *const reefConstant_NSURLIsReadableKey __asm__("_NSURLIsReadableKey") = @"NSURLIsReadableKey";
NSString *const reefConstant_kCFURLIsReadableKey __asm__("_kCFURLIsReadableKey") = @"NSURLIsReadableKey";
NSString *const reefConstant_NSURLIsWritableKey __asm__("_NSURLIsWritableKey") = @"NSURLIsWritableKey";
NSString *const reefConstant_kCFURLIsWritableKey __asm__("_kCFURLIsWritableKey") = @"NSURLIsWritableKey";
NSString *const reefConstant_NSURLIsExecutableKey __asm__("_NSURLIsExecutableKey") = @"NSURLIsExecutableKey";
NSString *const reefConstant_kCFURLIsExecutableKey __asm__("_kCFURLIsExecutableKey") = @"NSURLIsExecutableKey";
NSString *const reefConstant_NSURLFileSecurityKey __asm__("_NSURLFileSecurityKey") = @"NSURLFileSecurityKey";
NSString *const reefConstant_kCFURLFileSecurityKey __asm__("_kCFURLFileSecurityKey") = @"NSURLFileSecurityKey";
NSString *const reefConstant_NSURLIsExcludedFromBackupKey __asm__("_NSURLIsExcludedFromBackupKey") = @"NSURLIsExcludedFromBackupKey";
NSString *const reefConstant_kCFURLIsExcludedFromBackupKey __asm__("_kCFURLIsExcludedFromBackupKey") = @"NSURLIsExcludedFromBackupKey";
NSString *const reefConstant_NSURLTagNamesKey __asm__("_NSURLTagNamesKey") = @"NSURLTagNamesKey";
NSString *const reefConstant_kCFURLTagNamesKey __asm__("_kCFURLTagNamesKey") = @"NSURLTagNamesKey";
NSString *const reefConstant_NSURLPathKey __asm__("_NSURLPathKey") = @"_NSURLPathKey";
NSString *const reefConstant_kCFURLPathKey __asm__("_kCFURLPathKey") = @"_NSURLPathKey";
NSString *const reefConstant_NSURLCanonicalPathKey __asm__("_NSURLCanonicalPathKey") = @"_NSURLCanonicalPathKey";
NSString *const reefConstant_kCFURLCanonicalPathKey __asm__("_kCFURLCanonicalPathKey") = @"_NSURLCanonicalPathKey";
NSString *const reefConstant_NSURLIsMountTriggerKey __asm__("_NSURLIsMountTriggerKey") = @"NSURLIsMountTriggerKey";
NSString *const reefConstant_kCFURLIsMountTriggerKey __asm__("_kCFURLIsMountTriggerKey") = @"NSURLIsMountTriggerKey";
NSString *const reefConstant_NSURLGenerationIdentifierKey __asm__("_NSURLGenerationIdentifierKey") = @"NSURLGenerationIdentifierKey";
NSString *const reefConstant_kCFURLGenerationIdentifierKey __asm__("_kCFURLGenerationIdentifierKey") = @"NSURLGenerationIdentifierKey";
NSString *const reefConstant_NSURLDocumentIdentifierKey __asm__("_NSURLDocumentIdentifierKey") = @"NSURLDocumentIdentifierKey";
NSString *const reefConstant_kCFURLDocumentIdentifierKey __asm__("_kCFURLDocumentIdentifierKey") = @"NSURLDocumentIdentifierKey";
NSString *const reefConstant_NSURLAddedToDirectoryDateKey __asm__("_NSURLAddedToDirectoryDateKey") = @"NSURLAddedToDirectoryDateKey";
NSString *const reefConstant_kCFURLAddedToDirectoryDateKey __asm__("_kCFURLAddedToDirectoryDateKey") = @"NSURLAddedToDirectoryDateKey";
NSString *const reefConstant_NSURLQuarantinePropertiesKey __asm__("_NSURLQuarantinePropertiesKey") = @"NSURLQuarantinePropertiesKey";
NSString *const reefConstant_kCFURLQuarantinePropertiesKey __asm__("_kCFURLQuarantinePropertiesKey") = @"NSURLQuarantinePropertiesKey";
NSString *const reefConstant_NSURLFileResourceTypeKey __asm__("_NSURLFileResourceTypeKey") = @"NSURLFileResourceTypeKey";
NSString *const reefConstant_kCFURLFileResourceTypeKey __asm__("_kCFURLFileResourceTypeKey") = @"NSURLFileResourceTypeKey";
NSString *const reefConstant_NSURLFileResourceTypeNamedPipe __asm__("_NSURLFileResourceTypeNamedPipe") = @"NSFileTypeNamedPipe";
NSString *const reefConstant_kCFURLFileResourceTypeNamedPipe __asm__("_kCFURLFileResourceTypeNamedPipe") = @"NSFileTypeNamedPipe";
NSString *const reefConstant_NSURLFileResourceTypeCharacterSpecial __asm__("_NSURLFileResourceTypeCharacterSpecial") = @"NSFileTypeCharacterSpecial";
NSString *const reefConstant_kCFURLFileResourceTypeCharacterSpecial __asm__("_kCFURLFileResourceTypeCharacterSpecial") = @"NSFileTypeCharacterSpecial";
NSString *const reefConstant_NSURLFileResourceTypeDirectory __asm__("_NSURLFileResourceTypeDirectory") = @"NSFileTypeDirectory";
NSString *const reefConstant_kCFURLFileResourceTypeDirectory __asm__("_kCFURLFileResourceTypeDirectory") = @"NSFileTypeDirectory";
NSString *const reefConstant_NSURLFileResourceTypeBlockSpecial __asm__("_NSURLFileResourceTypeBlockSpecial") = @"NSFileTypeBlockSpecial";
NSString *const reefConstant_kCFURLFileResourceTypeBlockSpecial __asm__("_kCFURLFileResourceTypeBlockSpecial") = @"NSFileTypeBlockSpecial";
NSString *const reefConstant_NSURLFileResourceTypeRegular __asm__("_NSURLFileResourceTypeRegular") = @"NSFileTypeRegular";
NSString *const reefConstant_kCFURLFileResourceTypeRegular __asm__("_kCFURLFileResourceTypeRegular") = @"NSFileTypeRegular";
NSString *const reefConstant_NSURLFileResourceTypeSymbolicLink __asm__("_NSURLFileResourceTypeSymbolicLink") = @"NSFileTypeSymbolicLink";
NSString *const reefConstant_kCFURLFileResourceTypeSymbolicLink __asm__("_kCFURLFileResourceTypeSymbolicLink") = @"NSFileTypeSymbolicLink";
NSString *const reefConstant_NSURLFileResourceTypeSocket __asm__("_NSURLFileResourceTypeSocket") = @"NSFileTypeSocket";
NSString *const reefConstant_kCFURLFileResourceTypeSocket __asm__("_kCFURLFileResourceTypeSocket") = @"NSFileTypeSocket";
NSString *const reefConstant_NSURLFileResourceTypeUnknown __asm__("_NSURLFileResourceTypeUnknown") = @"NSFileTypeUnknown";
NSString *const reefConstant_kCFURLFileResourceTypeUnknown __asm__("_kCFURLFileResourceTypeUnknown") = @"NSFileTypeUnknown";
NSString *const reefConstant_NSURLFileSizeKey __asm__("_NSURLFileSizeKey") = @"NSURLFileSizeKey";
NSString *const reefConstant_kCFURLFileSizeKey __asm__("_kCFURLFileSizeKey") = @"NSURLFileSizeKey";
NSString *const reefConstant_NSURLFileAllocatedSizeKey __asm__("_NSURLFileAllocatedSizeKey") = @"NSURLFileAllocatedSizeKey";
NSString *const reefConstant_kCFURLFileAllocatedSizeKey __asm__("_kCFURLFileAllocatedSizeKey") = @"NSURLFileAllocatedSizeKey";
NSString *const reefConstant_NSURLTotalFileSizeKey __asm__("_NSURLTotalFileSizeKey") = @"NSURLTotalFileSizeKey";
NSString *const reefConstant_kCFURLTotalFileSizeKey __asm__("_kCFURLTotalFileSizeKey") = @"NSURLTotalFileSizeKey";
NSString *const reefConstant_NSURLTotalFileAllocatedSizeKey __asm__("_NSURLTotalFileAllocatedSizeKey") = @"NSURLTotalFileAllocatedSizeKey";
NSString *const reefConstant_kCFURLTotalFileAllocatedSizeKey __asm__("_kCFURLTotalFileAllocatedSizeKey") = @"NSURLTotalFileAllocatedSizeKey";
NSString *const reefConstant_NSURLIsAliasFileKey __asm__("_NSURLIsAliasFileKey") = @"NSURLIsAliasFileKey";
NSString *const reefConstant_kCFURLIsAliasFileKey __asm__("_kCFURLIsAliasFileKey") = @"NSURLIsAliasFileKey";
NSString *const reefConstant_NSURLFileProtectionKey __asm__("_NSURLFileProtectionKey") = @"NSURLFileProtectionKey";
NSString *const reefConstant_kCFURLFileProtectionKey __asm__("_kCFURLFileProtectionKey") = @"NSURLFileProtectionKey";
NSString *const reefConstant_NSURLFileProtectionNone __asm__("_NSURLFileProtectionNone") = @"NSFileProtectionNone";
NSString *const reefConstant_kCFURLFileProtectionNone __asm__("_kCFURLFileProtectionNone") = @"NSFileProtectionNone";
NSString *const reefConstant_NSURLFileProtectionComplete __asm__("_NSURLFileProtectionComplete") = @"NSFileProtectionComplete";
NSString *const reefConstant_kCFURLFileProtectionComplete __asm__("_kCFURLFileProtectionComplete") = @"NSFileProtectionComplete";
NSString *const reefConstant_NSURLFileProtectionCompleteUnlessOpen __asm__("_NSURLFileProtectionCompleteUnlessOpen") = @"NSFileProtectionCompleteUnlessOpen";
NSString *const reefConstant_kCFURLFileProtectionCompleteUnlessOpen __asm__("_kCFURLFileProtectionCompleteUnlessOpen") = @"NSFileProtectionCompleteUnlessOpen";
NSString *const reefConstant_NSURLFileProtectionCompleteUntilFirstUserAuthentication __asm__("_NSURLFileProtectionCompleteUntilFirstUserAuthentication") = @"NSFileProtectionCompleteUntilFirstUserAuthentication";
NSString *const reefConstant_kCFURLFileProtectionCompleteUntilFirstUserAuthentication __asm__("_kCFURLFileProtectionCompleteUntilFirstUserAuthentication") = @"NSFileProtectionCompleteUntilFirstUserAuthentication";
NSString *const reefConstant_NSURLVolumeLocalizedFormatDescriptionKey __asm__("_NSURLVolumeLocalizedFormatDescriptionKey") = @"NSURLVolumeLocalizedFormatDescriptionKey";
NSString *const reefConstant_kCFURLVolumeLocalizedFormatDescriptionKey __asm__("_kCFURLVolumeLocalizedFormatDescriptionKey") = @"NSURLVolumeLocalizedFormatDescriptionKey";
NSString *const reefConstant_NSURLVolumeTotalCapacityKey __asm__("_NSURLVolumeTotalCapacityKey") = @"NSURLVolumeTotalCapacityKey";
NSString *const reefConstant_kCFURLVolumeTotalCapacityKey __asm__("_kCFURLVolumeTotalCapacityKey") = @"NSURLVolumeTotalCapacityKey";
NSString *const reefConstant_NSURLVolumeAvailableCapacityKey __asm__("_NSURLVolumeAvailableCapacityKey") = @"NSURLVolumeAvailableCapacityKey";
NSString *const reefConstant_kCFURLVolumeAvailableCapacityKey __asm__("_kCFURLVolumeAvailableCapacityKey") = @"NSURLVolumeAvailableCapacityKey";
NSString *const reefConstant_NSURLVolumeAvailableCapacityForImportantUsageKey __asm__("_NSURLVolumeAvailableCapacityForImportantUsageKey") = @"NSURLVolumeAvailableCapacityForImportantUsageKey";
NSString *const reefConstant_kCFURLVolumeAvailableCapacityForImportantUsageKey __asm__("_kCFURLVolumeAvailableCapacityForImportantUsageKey") = @"NSURLVolumeAvailableCapacityForImportantUsageKey";
NSString *const reefConstant_NSURLVolumeAvailableCapacityForOpportunisticUsageKey __asm__("_NSURLVolumeAvailableCapacityForOpportunisticUsageKey") = @"NSURLVolumeAvailableCapacityForOpportunisticUsageKey";
NSString *const reefConstant_kCFURLVolumeAvailableCapacityForOpportunisticUsageKey __asm__("_kCFURLVolumeAvailableCapacityForOpportunisticUsageKey") = @"NSURLVolumeAvailableCapacityForOpportunisticUsageKey";
NSString *const reefConstant_NSURLVolumeResourceCountKey __asm__("_NSURLVolumeResourceCountKey") = @"NSURLVolumeResourceCountKey";
NSString *const reefConstant_kCFURLVolumeResourceCountKey __asm__("_kCFURLVolumeResourceCountKey") = @"NSURLVolumeResourceCountKey";
NSString *const reefConstant_NSURLVolumeSupportsPersistentIDsKey __asm__("_NSURLVolumeSupportsPersistentIDsKey") = @"NSURLVolumeSupportsPersistentIDsKey";
NSString *const reefConstant_kCFURLVolumeSupportsPersistentIDsKey __asm__("_kCFURLVolumeSupportsPersistentIDsKey") = @"NSURLVolumeSupportsPersistentIDsKey";
NSString *const reefConstant_NSURLVolumeSupportsSymbolicLinksKey __asm__("_NSURLVolumeSupportsSymbolicLinksKey") = @"NSURLVolumeSupportsSymbolicLinksKey";
NSString *const reefConstant_kCFURLVolumeSupportsSymbolicLinksKey __asm__("_kCFURLVolumeSupportsSymbolicLinksKey") = @"NSURLVolumeSupportsSymbolicLinksKey";
NSString *const reefConstant_NSURLVolumeSupportsHardLinksKey __asm__("_NSURLVolumeSupportsHardLinksKey") = @"NSURLVolumeSupportsHardLinksKey";
NSString *const reefConstant_kCFURLVolumeSupportsHardLinksKey __asm__("_kCFURLVolumeSupportsHardLinksKey") = @"NSURLVolumeSupportsHardLinksKey";
NSString *const reefConstant_NSURLVolumeSupportsJournalingKey __asm__("_NSURLVolumeSupportsJournalingKey") = @"NSURLVolumeSupportsJournalingKey";
NSString *const reefConstant_kCFURLVolumeSupportsJournalingKey __asm__("_kCFURLVolumeSupportsJournalingKey") = @"NSURLVolumeSupportsJournalingKey";
NSString *const reefConstant_NSURLVolumeIsJournalingKey __asm__("_NSURLVolumeIsJournalingKey") = @"NSURLVolumeIsJournalingKey";
NSString *const reefConstant_kCFURLVolumeIsJournalingKey __asm__("_kCFURLVolumeIsJournalingKey") = @"NSURLVolumeIsJournalingKey";
NSString *const reefConstant_NSURLVolumeSupportsSparseFilesKey __asm__("_NSURLVolumeSupportsSparseFilesKey") = @"NSURLVolumeSupportsSparseFilesKey";
NSString *const reefConstant_kCFURLVolumeSupportsSparseFilesKey __asm__("_kCFURLVolumeSupportsSparseFilesKey") = @"NSURLVolumeSupportsSparseFilesKey";
NSString *const reefConstant_NSURLVolumeSupportsZeroRunsKey __asm__("_NSURLVolumeSupportsZeroRunsKey") = @"NSURLVolumeSupportsZeroRunsKey";
NSString *const reefConstant_kCFURLVolumeSupportsZeroRunsKey __asm__("_kCFURLVolumeSupportsZeroRunsKey") = @"NSURLVolumeSupportsZeroRunsKey";
NSString *const reefConstant_NSURLVolumeSupportsCaseSensitiveNamesKey __asm__("_NSURLVolumeSupportsCaseSensitiveNamesKey") = @"NSURLVolumeSupportsCaseSensitiveNamesKey";
NSString *const reefConstant_kCFURLVolumeSupportsCaseSensitiveNamesKey __asm__("_kCFURLVolumeSupportsCaseSensitiveNamesKey") = @"NSURLVolumeSupportsCaseSensitiveNamesKey";
NSString *const reefConstant_NSURLVolumeSupportsCasePreservedNamesKey __asm__("_NSURLVolumeSupportsCasePreservedNamesKey") = @"NSURLVolumeSupportsCasePreservedNamesKey";
NSString *const reefConstant_kCFURLVolumeSupportsCasePreservedNamesKey __asm__("_kCFURLVolumeSupportsCasePreservedNamesKey") = @"NSURLVolumeSupportsCasePreservedNamesKey";
NSString *const reefConstant_NSURLVolumeSupportsRootDirectoryDatesKey __asm__("_NSURLVolumeSupportsRootDirectoryDatesKey") = @"NSURLVolumeSupportsRootDirectoryDatesKey";
NSString *const reefConstant_kCFURLVolumeSupportsRootDirectoryDatesKey __asm__("_kCFURLVolumeSupportsRootDirectoryDatesKey") = @"NSURLVolumeSupportsRootDirectoryDatesKey";
NSString *const reefConstant_NSURLVolumeSupportsVolumeSizesKey __asm__("_NSURLVolumeSupportsVolumeSizesKey") = @"NSURLVolumeSupportsVolumeSizesKey";
NSString *const reefConstant_kCFURLVolumeSupportsVolumeSizesKey __asm__("_kCFURLVolumeSupportsVolumeSizesKey") = @"NSURLVolumeSupportsVolumeSizesKey";
NSString *const reefConstant_NSURLVolumeSupportsRenamingKey __asm__("_NSURLVolumeSupportsRenamingKey") = @"NSURLVolumeSupportsRenamingKey";
NSString *const reefConstant_kCFURLVolumeSupportsRenamingKey __asm__("_kCFURLVolumeSupportsRenamingKey") = @"NSURLVolumeSupportsRenamingKey";
NSString *const reefConstant_NSURLVolumeSupportsAdvisoryFileLockingKey __asm__("_NSURLVolumeSupportsAdvisoryFileLockingKey") = @"NSURLVolumeSupportsAdvisoryFileLockingKey";
NSString *const reefConstant_kCFURLVolumeSupportsAdvisoryFileLockingKey __asm__("_kCFURLVolumeSupportsAdvisoryFileLockingKey") = @"NSURLVolumeSupportsAdvisoryFileLockingKey";
NSString *const reefConstant_NSURLVolumeSupportsExtendedSecurityKey __asm__("_NSURLVolumeSupportsExtendedSecurityKey") = @"NSURLVolumeSupportsExtendedSecurityKey";
NSString *const reefConstant_kCFURLVolumeSupportsExtendedSecurityKey __asm__("_kCFURLVolumeSupportsExtendedSecurityKey") = @"NSURLVolumeSupportsExtendedSecurityKey";
NSString *const reefConstant_NSURLVolumeIsBrowsableKey __asm__("_NSURLVolumeIsBrowsableKey") = @"NSURLVolumeIsBrowsableKey";
NSString *const reefConstant_kCFURLVolumeIsBrowsableKey __asm__("_kCFURLVolumeIsBrowsableKey") = @"NSURLVolumeIsBrowsableKey";
NSString *const reefConstant_NSURLVolumeMaximumFileSizeKey __asm__("_NSURLVolumeMaximumFileSizeKey") = @"NSURLVolumeMaximumFileSizeKey";
NSString *const reefConstant_kCFURLVolumeMaximumFileSizeKey __asm__("_kCFURLVolumeMaximumFileSizeKey") = @"NSURLVolumeMaximumFileSizeKey";
NSString *const reefConstant_NSURLVolumeIsEjectableKey __asm__("_NSURLVolumeIsEjectableKey") = @"NSURLVolumeIsEjectableKey";
NSString *const reefConstant_kCFURLVolumeIsEjectableKey __asm__("_kCFURLVolumeIsEjectableKey") = @"NSURLVolumeIsEjectableKey";
NSString *const reefConstant_NSURLVolumeIsRemovableKey __asm__("_NSURLVolumeIsRemovableKey") = @"NSURLVolumeIsRemovableKey";
NSString *const reefConstant_kCFURLVolumeIsRemovableKey __asm__("_kCFURLVolumeIsRemovableKey") = @"NSURLVolumeIsRemovableKey";
NSString *const reefConstant_NSURLVolumeIsInternalKey __asm__("_NSURLVolumeIsInternalKey") = @"NSURLVolumeIsInternalKey";
NSString *const reefConstant_kCFURLVolumeIsInternalKey __asm__("_kCFURLVolumeIsInternalKey") = @"NSURLVolumeIsInternalKey";
NSString *const reefConstant_NSURLVolumeIsAutomountedKey __asm__("_NSURLVolumeIsAutomountedKey") = @"NSURLVolumeIsAutomountedKey";
NSString *const reefConstant_kCFURLVolumeIsAutomountedKey __asm__("_kCFURLVolumeIsAutomountedKey") = @"NSURLVolumeIsAutomountedKey";
NSString *const reefConstant_NSURLVolumeSupportsBrowsingKey __asm__("_NSURLVolumeSupportsBrowsingKey") = @"NSURLVolumeSupportsBrowsingKey";
NSString *const reefConstant_kCFURLVolumeSupportsBrowsingKey __asm__("_kCFURLVolumeSupportsBrowsingKey") = @"NSURLVolumeSupportsBrowsingKey";
NSString *const reefConstant_NSURLVolumeIsLocalKey __asm__("_NSURLVolumeIsLocalKey") = @"NSURLVolumeIsLocalKey";
NSString *const reefConstant_kCFURLVolumeIsLocalKey __asm__("_kCFURLVolumeIsLocalKey") = @"NSURLVolumeIsLocalKey";
NSString *const reefConstant_NSURLVolumeIsReadOnlyKey __asm__("_NSURLVolumeIsReadOnlyKey") = @"NSURLVolumeIsReadOnlyKey";
NSString *const reefConstant_kCFURLVolumeIsReadOnlyKey __asm__("_kCFURLVolumeIsReadOnlyKey") = @"NSURLVolumeIsReadOnlyKey";
NSString *const reefConstant_NSURLVolumeCreationDateKey __asm__("_NSURLVolumeCreationDateKey") = @"NSURLVolumeCreationDateKey";
NSString *const reefConstant_kCFURLVolumeCreationDateKey __asm__("_kCFURLVolumeCreationDateKey") = @"NSURLVolumeCreationDateKey";
NSString *const reefConstant_NSURLVolumeURLForRemountingKey __asm__("_NSURLVolumeURLForRemountingKey") = @"NSURLVolumeURLForRemountingKey";
NSString *const reefConstant_kCFURLVolumeURLForRemountingKey __asm__("_kCFURLVolumeURLForRemountingKey") = @"NSURLVolumeURLForRemountingKey";
NSString *const reefConstant_NSURLVolumeUUIDStringKey __asm__("_NSURLVolumeUUIDStringKey") = @"NSURLVolumeUUIDStringKey";
NSString *const reefConstant_kCFURLVolumeUUIDStringKey __asm__("_kCFURLVolumeUUIDStringKey") = @"NSURLVolumeUUIDStringKey";
NSString *const reefConstant_NSURLVolumeNameKey __asm__("_NSURLVolumeNameKey") = @"NSURLVolumeNameKey";
NSString *const reefConstant_kCFURLVolumeNameKey __asm__("_kCFURLVolumeNameKey") = @"NSURLVolumeNameKey";
NSString *const reefConstant_NSURLVolumeLocalizedNameKey __asm__("_NSURLVolumeLocalizedNameKey") = @"NSURLVolumeLocalizedNameKey";
NSString *const reefConstant_kCFURLVolumeLocalizedNameKey __asm__("_kCFURLVolumeLocalizedNameKey") = @"NSURLVolumeLocalizedNameKey";
NSString *const reefConstant_NSURLVolumeIsEncryptedKey __asm__("_NSURLVolumeIsEncryptedKey") = @"NSURLVolumeIsEncryptedKey";
NSString *const reefConstant_kCFURLVolumeIsEncryptedKey __asm__("_kCFURLVolumeIsEncryptedKey") = @"NSURLVolumeIsEncryptedKey";
NSString *const reefConstant_NSURLVolumeIsRootFileSystemKey __asm__("_NSURLVolumeIsRootFileSystemKey") = @"NSURLVolumeIsRootFileSystemKey";
NSString *const reefConstant_kCFURLVolumeIsRootFileSystemKey __asm__("_kCFURLVolumeIsRootFileSystemKey") = @"NSURLVolumeIsRootFileSystemKey";
NSString *const reefConstant_NSURLVolumeSupportsCompressionKey __asm__("_NSURLVolumeSupportsCompressionKey") = @"NSURLVolumeSupportsCompressionKey";
NSString *const reefConstant_kCFURLVolumeSupportsCompressionKey __asm__("_kCFURLVolumeSupportsCompressionKey") = @"NSURLVolumeSupportsCompressionKey";
NSString *const reefConstant_NSURLVolumeSupportsFileCloningKey __asm__("_NSURLVolumeSupportsFileCloningKey") = @"NSURLVolumeSupportsFileCloningKey";
NSString *const reefConstant_kCFURLVolumeSupportsFileCloningKey __asm__("_kCFURLVolumeSupportsFileCloningKey") = @"NSURLVolumeSupportsFileCloningKey";
NSString *const reefConstant_NSURLVolumeSupportsSwapRenamingKey __asm__("_NSURLVolumeSupportsSwapRenamingKey") = @"NSURLVolumeSupportsSwapRenamingKey";
NSString *const reefConstant_kCFURLVolumeSupportsSwapRenamingKey __asm__("_kCFURLVolumeSupportsSwapRenamingKey") = @"NSURLVolumeSupportsSwapRenamingKey";
NSString *const reefConstant_NSURLVolumeSupportsExclusiveRenamingKey __asm__("_NSURLVolumeSupportsExclusiveRenamingKey") = @"NSURLVolumeSupportsExclusiveRenamingKey";
NSString *const reefConstant_kCFURLVolumeSupportsExclusiveRenamingKey __asm__("_kCFURLVolumeSupportsExclusiveRenamingKey") = @"NSURLVolumeSupportsExclusiveRenamingKey";
NSString *const reefConstant_NSURLVolumeSupportsImmutableFilesKey __asm__("_NSURLVolumeSupportsImmutableFilesKey") = @"NSURLVolumeSupportsImmutableFilesKey";
NSString *const reefConstant_kCFURLVolumeSupportsImmutableFilesKey __asm__("_kCFURLVolumeSupportsImmutableFilesKey") = @"NSURLVolumeSupportsImmutableFilesKey";
NSString *const reefConstant_NSURLVolumeSupportsAccessPermissionsKey __asm__("_NSURLVolumeSupportsAccessPermissionsKey") = @"NSURLVolumeSupportsAccessPermissionsKey";
NSString *const reefConstant_kCFURLVolumeSupportsAccessPermissionsKey __asm__("_kCFURLVolumeSupportsAccessPermissionsKey") = @"NSURLVolumeSupportsAccessPermissionsKey";
NSString *const reefConstant_NSURLVolumeSupportsFileProtectionKey __asm__("_NSURLVolumeSupportsFileProtectionKey") = @"NSURLVolumeSupportsFileProtectionKey";
NSString *const reefConstant_kCFURLVolumeSupportsFileProtectionKey __asm__("_kCFURLVolumeSupportsFileProtectionKey") = @"NSURLVolumeSupportsFileProtectionKey";
NSString *const reefConstant_NSURLIsUbiquitousItemKey __asm__("_NSURLIsUbiquitousItemKey") = @"NSURLIsUbiquitousItemKey";
NSString *const reefConstant_kCFURLIsUbiquitousItemKey __asm__("_kCFURLIsUbiquitousItemKey") = @"NSURLIsUbiquitousItemKey";
NSString *const reefConstant_NSURLUbiquitousItemHasUnresolvedConflictsKey __asm__("_NSURLUbiquitousItemHasUnresolvedConflictsKey") = @"NSURLUbiquitousItemHasUnresolvedConflictsKey";
NSString *const reefConstant_kCFURLUbiquitousItemHasUnresolvedConflictsKey __asm__("_kCFURLUbiquitousItemHasUnresolvedConflictsKey") = @"NSURLUbiquitousItemHasUnresolvedConflictsKey";
NSString *const reefConstant_NSURLUbiquitousItemIsDownloadedKey __asm__("_NSURLUbiquitousItemIsDownloadedKey") = @"NSURLUbiquitousItemIsDownloadedKey";
NSString *const reefConstant_kCFURLUbiquitousItemIsDownloadedKey __asm__("_kCFURLUbiquitousItemIsDownloadedKey") = @"NSURLUbiquitousItemIsDownloadedKey";
NSString *const reefConstant_NSURLUbiquitousItemDownloadingStatusKey __asm__("_NSURLUbiquitousItemDownloadingStatusKey") = @"NSURLUbiquitousItemDownloadingStatusKey";
NSString *const reefConstant_kCFURLUbiquitousItemDownloadingStatusKey __asm__("_kCFURLUbiquitousItemDownloadingStatusKey") = @"NSURLUbiquitousItemDownloadingStatusKey";
NSString *const reefConstant_NSURLUbiquitousItemIsDownloadingKey __asm__("_NSURLUbiquitousItemIsDownloadingKey") = @"NSURLUbiquitousItemIsDownloadingKey";
NSString *const reefConstant_kCFURLUbiquitousItemIsDownloadingKey __asm__("_kCFURLUbiquitousItemIsDownloadingKey") = @"NSURLUbiquitousItemIsDownloadingKey";
NSString *const reefConstant_NSURLUbiquitousItemIsUploadedKey __asm__("_NSURLUbiquitousItemIsUploadedKey") = @"NSURLUbiquitousItemIsUploadedKey";
NSString *const reefConstant_kCFURLUbiquitousItemIsUploadedKey __asm__("_kCFURLUbiquitousItemIsUploadedKey") = @"NSURLUbiquitousItemIsUploadedKey";
NSString *const reefConstant_NSURLUbiquitousItemIsUploadingKey __asm__("_NSURLUbiquitousItemIsUploadingKey") = @"NSURLUbiquitousItemIsUploadingKey";
NSString *const reefConstant_kCFURLUbiquitousItemIsUploadingKey __asm__("_kCFURLUbiquitousItemIsUploadingKey") = @"NSURLUbiquitousItemIsUploadingKey";
NSString *const reefConstant_NSURLUbiquitousItemPercentDownloadedKey __asm__("_NSURLUbiquitousItemPercentDownloadedKey") = @"NSURLUbiquitousItemPercentDownloadedKey";
NSString *const reefConstant_kCFURLUbiquitousItemPercentDownloadedKey __asm__("_kCFURLUbiquitousItemPercentDownloadedKey") = @"NSURLUbiquitousItemPercentDownloadedKey";
NSString *const reefConstant_NSURLUbiquitousItemPercentUploadedKey __asm__("_NSURLUbiquitousItemPercentUploadedKey") = @"NSURLUbiquitousItemPercentUploadedKey";
NSString *const reefConstant_kCFURLUbiquitousItemPercentUploadedKey __asm__("_kCFURLUbiquitousItemPercentUploadedKey") = @"NSURLUbiquitousItemPercentUploadedKey";
NSString *const reefConstant_NSURLUbiquitousItemDownloadingErrorKey __asm__("_NSURLUbiquitousItemDownloadingErrorKey") = @"NSURLUbiquitousItemDownloadingErrorKey";
NSString *const reefConstant_kCFURLUbiquitousItemDownloadingErrorKey __asm__("_kCFURLUbiquitousItemDownloadingErrorKey") = @"NSURLUbiquitousItemDownloadingErrorKey";
NSString *const reefConstant_NSURLUbiquitousItemUploadingErrorKey __asm__("_NSURLUbiquitousItemUploadingErrorKey") = @"NSURLUbiquitousItemUploadingErrorKey";
NSString *const reefConstant_kCFURLUbiquitousItemUploadingErrorKey __asm__("_kCFURLUbiquitousItemUploadingErrorKey") = @"NSURLUbiquitousItemUploadingErrorKey";
NSString *const reefConstant_NSURLUbiquitousItemIsExcludedFromSyncKey __asm__("_NSURLUbiquitousItemIsExcludedFromSyncKey") = @"NSURLUbiquitousItemIsExcludedFromSyncKey";
NSString *const reefConstant_kCFURLUbiquitousItemIsExcludedFromSyncKey __asm__("_kCFURLUbiquitousItemIsExcludedFromSyncKey") = @"NSURLUbiquitousItemIsExcludedFromSyncKey";
NSString *const reefConstant_NSURLUbiquitousItemDownloadingStatusNotDownloaded __asm__("_NSURLUbiquitousItemDownloadingStatusNotDownloaded") = @"NSURLUbiquitousItemDownloadingStatusNotDownloaded";
NSString *const reefConstant_kCFURLUbiquitousItemDownloadingStatusNotDownloaded __asm__("_kCFURLUbiquitousItemDownloadingStatusNotDownloaded") = @"NSURLUbiquitousItemDownloadingStatusNotDownloaded";
NSString *const reefConstant_NSURLUbiquitousItemDownloadingStatusDownloaded __asm__("_NSURLUbiquitousItemDownloadingStatusDownloaded") = @"NSURLUbiquitousItemDownloadingStatusDownloaded";
NSString *const reefConstant_kCFURLUbiquitousItemDownloadingStatusDownloaded __asm__("_kCFURLUbiquitousItemDownloadingStatusDownloaded") = @"NSURLUbiquitousItemDownloadingStatusDownloaded";
NSString *const reefConstant_NSURLUbiquitousItemDownloadingStatusCurrent __asm__("_NSURLUbiquitousItemDownloadingStatusCurrent") = @"NSURLUbiquitousItemDownloadingStatusCurrent";
NSString *const reefConstant_kCFURLUbiquitousItemDownloadingStatusCurrent __asm__("_kCFURLUbiquitousItemDownloadingStatusCurrent") = @"NSURLUbiquitousItemDownloadingStatusCurrent";
NSString *const reefConstant_NSURLContentTypeKey __asm__("_NSURLContentTypeKey") = @"NSURLContentTypeKey";
NSString *const reefConstant_kCFURLContentTypeKey __asm__("_kCFURLContentTypeKey") = @"NSURLContentTypeKey";
NSString *const reefConstant_NSURLKeysOfUnsetValuesKey __asm__("_NSURLKeysOfUnsetValuesKey") = @"NSURLKeysOfUnsetValuesKey";
NSString *const reefConstant_kCFURLKeysOfUnsetValuesKey __asm__("_kCFURLKeysOfUnsetValuesKey") = @"NSURLKeysOfUnsetValuesKey";
NSString *const reefConstant_NSURLVolumeTypeNameKey __asm__("_NSURLVolumeTypeNameKey") = @"NSURLVolumeTypeNameKey";
NSString *const reefConstant_kCFURLVolumeTypeNameKey __asm__("_kCFURLVolumeTypeNameKey") = @"NSURLVolumeTypeNameKey";

// The compiler's name for the class of the constant strings is the class itself (the object of a constant string has it as its isa).
__asm__(".globl ___CFConstantStringClassReference\n\t.set ___CFConstantStringClassReference, _OBJC_CLASS_$___NSCFConstantString");
