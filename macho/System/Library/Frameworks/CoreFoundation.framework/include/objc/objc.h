// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdbool.h>
#ifndef __OBJC__
typedef struct objc_class *Class;
typedef struct objc_object { Class isa; } *id;
typedef struct objc_selector *SEL;
#endif
typedef id (*IMP)(id, SEL, ...);
typedef signed char BOOL;
#define YES ((BOOL)1)
#define NO ((BOOL)0)
#ifndef nil
#define nil ((id)0)
#define Nil ((Class)0)
#endif
#define OBJC_ROOT_CLASS __attribute__((objc_root_class))
typedef unsigned long NSUInteger;
typedef long NSInteger;
typedef struct _NSRange { NSUInteger location; NSUInteger length; } NSRange;
static inline NSRange NSMakeRange(NSUInteger loc, NSUInteger len) { NSRange r = {loc, len}; return r; }
typedef unsigned short unichar;
typedef NSUInteger NSCalendarUnit;
typedef NSUInteger NSStringCompareOptions;
typedef NSInteger NSTimeZoneNameStyle;
#ifdef __OBJC__
@protocol NSObject
- (id)retain;
@end
OBJC_ROOT_CLASS @interface NSObject <NSObject> { Class isa; }
+ (Class)class;
- (id)copy;
- (id)copyWithZone:(void *)zone;
- (BOOL)isKindOfClass:(Class)cls;
- (id)retain;
@end
#endif
#if defined(__OBJC__) && !defined(REEF_NSCLASSES)
@interface NSException : NSObject
@property (readonly, copy) id name;
@property (readonly, copy) id reason;
@property (readonly, copy) id userInfo;
@end
#endif
