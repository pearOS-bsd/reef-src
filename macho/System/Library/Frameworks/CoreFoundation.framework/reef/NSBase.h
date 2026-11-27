// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// What the Objective-C classes of CoreFoundation (NSClasses.m) need besides include/objc/objc.h.
// The Apple SDK is not used; NSObject itself is implemented by libobjc.
#pragma once
#include <objc/objc.h>

@class NSString;
typedef struct { unsigned long state; id *itemsPtr; unsigned long *mutationsPtr; unsigned long extra[5]; } NSFastEnumerationState;
typedef struct _NSZone NSZone;

@protocol NSCopying
- (id)copyWithZone:(NSZone *)zone;
@end
@protocol NSMutableCopying
- (id)mutableCopyWithZone:(NSZone *)zone;
@end
@protocol NSSecureCoding
@end
@protocol NSFastEnumeration
- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id *)buffer count:(NSUInteger)len;
@end
