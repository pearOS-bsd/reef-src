// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CryptoTokenKit.

#import <Foundation/Foundation.h>



@interface TKTokenWatcherTokenInfo : NSObject
{
    NSString * _tokenID;
    NSString * _localizedDriverName;
    NSString * _slotName;
}
@property (readonly, copy) NSString * tokenID;
@property (readonly, copy) NSString * localizedDriverName;
@property (readonly, copy) NSString * slotName;

@end

@implementation TKTokenWatcherTokenInfo
@end

@interface TKTokenWatcher : NSObject
@property (readonly) NSArray *tokenIDs;
- (id)initWithInsertionHandler:(void (^)(void))a0;
- (void)setInsertionHandler:(void (^)(void))a0;
- (void)addRemovalHandler:(void (^)(void))a0 forTokenID:(id)a1;
- (id)tokenInfoForTokenID:(id)a0;
@end

@implementation TKTokenWatcher
- (id)initWithInsertionHandler:(void (^)(void))a0
{
    return [self init];
}

- (id)tokenIDs
{
    return [NSArray array];
}

- (void)setInsertionHandler:(void (^)(void))a0
{

}

- (void)addRemovalHandler:(void (^)(void))a0 forTokenID:(id)a1
{

}

- (id)tokenInfoForTokenID:(id)a0
{
    return nil;
}

@end


