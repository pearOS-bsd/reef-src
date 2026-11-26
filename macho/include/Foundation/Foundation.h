// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations of Foundation and the geometry types (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stddef.h>
#include <stdint.h>

typedef signed char BOOL;
#define YES ((BOOL)1)
#define NO ((BOOL)0)
#define nil ((id)0)
#define NULL ((void *)0)
typedef long NSInteger;
typedef unsigned long NSUInteger;
typedef double CGFloat;
typedef double NSTimeInterval;
struct _NSZone;
typedef struct _NSZone NSZone;
struct CGPoint { CGFloat x, y; }; typedef struct CGPoint CGPoint;
struct CGSize { CGFloat width, height; }; typedef struct CGSize CGSize;
struct CGRect { CGPoint origin; CGSize size; }; typedef struct CGRect CGRect;
struct CGVector { CGFloat dx, dy; }; typedef struct CGVector CGVector;
struct CGAffineTransform { CGFloat a, b, c, d, tx, ty; }; typedef struct CGAffineTransform CGAffineTransform;
typedef struct _NSRange { NSUInteger location, length; } NSRange;

@protocol NSObject
- (BOOL)isEqual:(id)object;
- (NSUInteger)hash;
- (id)self;
@end
@protocol NSCopying
- (id)copyWithZone:(NSZone *)zone;
@end
@protocol NSMutableCopying
- (id)mutableCopyWithZone:(NSZone *)zone;
@end
@class NSCoder;
@protocol NSCoding
- (void)encodeWithCoder:(NSCoder *)coder;
- (id)initWithCoder:(NSCoder *)coder;
@end
@protocol NSSecureCoding <NSCoding>
+ (BOOL)supportsSecureCoding;
@end

__attribute__((objc_root_class))
@interface NSObject <NSObject>
{
    Class isa;
}
+ (id)alloc;
+ (id)new;
+ (id)allocWithZone:(NSZone *)zone;
- (id)init;
- (void)dealloc;
- (id)copy;
- (id)mutableCopy;
- (Class)class;
- (BOOL)isKindOfClass:(Class)cls;
- (BOOL)respondsToSelector:(SEL)selector;
- (id)performSelector:(SEL)selector;
- (id)valueForKey:(id)key;
- (void)setValue:(id)value forKey:(id)key;
- (void)setValue:(id)value forUndefinedKey:(id)key;
@end

@interface NSArray : NSObject
+ (id)array;
+ (id)arrayWithObjects:(const id *)objects count:(NSUInteger)count;
- (NSUInteger)count;
- (id)objectAtIndex:(NSUInteger)index;
@end
@interface NSDictionary : NSObject
+ (id)dictionary;
+ (id)dictionaryWithObjects:(const id *)objects forKeys:(const id *)keys count:(NSUInteger)count;
- (id)objectForKey:(id)key;
@end
@interface NSNumber : NSObject
+ (NSNumber *)numberWithBool:(BOOL)value;
+ (NSNumber *)numberWithInt:(int)value;
+ (NSNumber *)numberWithInteger:(NSInteger)value;
+ (NSNumber *)numberWithDouble:(double)value;
@end
@interface NSString : NSObject
- (NSUInteger)length;
@end
@class NSData, NSDate, NSURL, NSError, NSSet, NSUUID, NSMutableArray, NSMutableDictionary, NSBundle, NSLocale, NSRunLoop, NSIndexSet, NSCharacterSet, NSTimeZone, NSNotification;
