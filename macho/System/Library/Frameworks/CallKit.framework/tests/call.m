// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CallKit: a call has a UUID of its own and equals only itself; the observer has no calls.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSUUID : NSObject
@end

@interface CXCall : NSObject
@property (readonly, copy) NSUUID *UUID;
@property (readonly, getter=isOutgoing) BOOL outgoing;
@property (readonly, getter=isOnHold) BOOL onHold;
@property (readonly) BOOL hasConnected;
@property (readonly) BOOL hasEnded;
- (BOOL)isEqualToCall:(id)call;
@end

@interface CXCallObserver : NSObject
@property (readonly, copy) NSArray *calls;
- (void)setDelegate:(id)delegate queue:(id)queue;
@end

int main(void)
{
    CXCall *a = [CXCall new], *b = [CXCall new];
    printf("uuid %d %d\n", a.UUID != nil, [a.UUID isEqual:a.UUID]);
    printf("flags %d %d %d %d\n", a.isOutgoing, a.isOnHold, a.hasConnected, a.hasEnded);
    printf("equal %d %d %d %d\n", [a isEqual:a], [a isEqualToCall:a], [a isEqual:b], [a isEqual:@"x"]);
    printf("hash %d\n", [a hash] == [a.UUID hash]);
    CXCallObserver *observer = [CXCallObserver new];
    [observer setDelegate:a queue:nil];
    printf("calls %lu\n", (unsigned long)[observer.calls count]);
    return 0;
}
