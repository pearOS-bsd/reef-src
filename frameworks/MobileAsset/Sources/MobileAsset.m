// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// Downloadable system assets: the catalogue is not reachable, queries find nothing.
NSString *const ASAttributeContentVersion = @"_ContentVersion";
@interface MAAsset : NSObject
@end
@implementation MAAsset
@end
@interface MADownloadOptions : NSObject
@property BOOL allowsCellularAccess;
@end
@implementation MADownloadOptions
@end
@interface MAAssetQuery : NSObject
- (instancetype)initWithType:(NSString *)type;
- (NSInteger)queryMetaDataSync;
- (void)queryMetaData:(void (^)(NSInteger result))completion;
- (NSArray *)results;
@end
@implementation MAAssetQuery
- (instancetype)initWithType:(NSString *)type { return [super init]; }
- (NSInteger)queryMetaDataSync { return 0; }
- (void)queryMetaData:(void (^)(NSInteger))completion { if (completion) completion(0); }
- (NSArray *)results { return @[]; }
@end
