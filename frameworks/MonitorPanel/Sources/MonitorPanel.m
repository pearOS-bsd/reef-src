// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// The display manager of Displays settings: no external display configuration.
@interface MPDisplayMgr : NSObject
- (NSArray *)displays;
@end
@implementation MPDisplayMgr
- (NSArray *)displays { return @[]; }
@end
