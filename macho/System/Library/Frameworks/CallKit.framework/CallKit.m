// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CallKit: a call (identified by a UUID, never connected) and an observer that never sees one.

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
@end

@interface NSUUID : NSObject
@end

@interface CXCall : NSObject
{
    BOOL _outgoing;
    BOOL _onHold;
    BOOL _hasConnected;
    BOOL _hasEnded;
    NSUUID *_UUID;
}
@property (readonly, copy) NSUUID *UUID;
@property (readonly, getter=isOutgoing) BOOL outgoing;
@property (readonly, getter=isOnHold) BOOL onHold;
@property (readonly) BOOL hasConnected;
@property (readonly) BOOL hasEnded;
- (BOOL)isEqualToCall:(id)call;
@end

@implementation CXCall

- (id)init
{
    self = [super init];
    if (self)
        _UUID = [[NSUUID alloc] init];
    return self;
}

- (void)dealloc
{
    [_UUID release];
    [super dealloc];
}

- (BOOL)isEqualToCall:(id)call
{
    return [_UUID isEqual:[call UUID]];
}

- (BOOL)isEqual:(id)object
{
    if ([object isKindOfClass:[CXCall class]])
        return [self isEqualToCall:object] != 0;
    return NO;
}

- (NSUInteger)hash
{
    return [_UUID hash];
}

@end

@interface CXCallObserver : NSObject
{
    __weak id _delegate;
}
@property (readonly, copy) NSArray *calls;
- (void)setDelegate:(id)delegate queue:(id)queue;
@end

@implementation CXCallObserver

- (id)calls
{
    return [NSArray array];
}

- (void)setDelegate:(id)delegate queue:(id)queue
{
    _delegate = delegate;
}

@end
