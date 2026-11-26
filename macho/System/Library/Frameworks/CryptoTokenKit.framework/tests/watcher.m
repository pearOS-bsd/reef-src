// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CryptoTokenKit: a token watcher reports no tokens.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface TKTokenWatcher : NSObject
@property (readonly) NSArray *tokenIDs;
- (id)initWithInsertionHandler:(void (^)(id))handler;
- (void)setInsertionHandler:(void (^)(id))handler;
- (void)addRemovalHandler:(void (^)(id))handler forTokenID:(id)tokenID;
- (id)tokenInfoForTokenID:(id)tokenID;
@end

int main(void)
{
    TKTokenWatcher *watcher = [[TKTokenWatcher alloc] initWithInsertionHandler:^(id tokenID) {}];
    [watcher setInsertionHandler:nil];
    [watcher addRemovalHandler:nil forTokenID:nil];
    printf("watcher %d tokens %lu info %d\n", watcher != nil, (unsigned long)[watcher.tokenIDs count], [watcher tokenInfoForTokenID:nil] == nil);
    return 0;
}
