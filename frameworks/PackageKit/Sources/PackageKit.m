// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Installer receipts: none of Apple's packages is installed here.
#import <Foundation/Foundation.h>

@interface PKReceipt : NSObject
@end

@implementation PKReceipt
+ (NSArray *)receiptsOnVolumeAtPath:(NSString *)path { return @[]; }
+ (NSArray *)receiptsWithIdentifierPrefix:(NSString *)prefix volume:(NSString *)volume { return @[]; }
+ (instancetype)receiptWithIdentifier:(NSString *)identifier volume:(NSString *)volume { return nil; }
- (NSString *)packageIdentifier { return nil; }
- (NSString *)packageVersion { return nil; }
- (NSDate *)installDate { return nil; }
- (NSString *)installPrefixPath { return nil; }
@end
