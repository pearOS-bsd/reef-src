// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Peer to peer sessions over Wi-Fi and Bluetooth: peers are made and sessions open, but no peer is ever found.
#import <Foundation/Foundation.h>

@interface MCPeerID : NSObject <NSCopying, NSSecureCoding>
@property (readonly, copy) NSString *displayName;
@end

@implementation MCPeerID
+ (BOOL)supportsSecureCoding { return YES; }
- (instancetype)initWithDisplayName:(NSString *)name {
    if ((self = [super init])) _displayName = [name copy];
    return self;
}
- (id)copyWithZone:(NSZone *)zone { return self; }
- (void)encodeWithCoder:(NSCoder *)coder { [coder encodeObject:_displayName forKey:@"displayName"]; }
- (instancetype)initWithCoder:(NSCoder *)coder { return [self initWithDisplayName:[coder decodeObjectOfClass:NSString.class forKey:@"displayName"]]; }
@end

@interface MCSession : NSObject
@property (weak) id delegate;
@property (readonly) MCPeerID *myPeerID;
@property (readonly) NSArray *securityIdentity;
@property (readonly) NSInteger encryptionPreference;
@property (readonly) NSArray *connectedPeers;
@end

@implementation MCSession
- (instancetype)initWithPeer:(MCPeerID *)peer { return [self initWithPeer:peer securityIdentity:nil encryptionPreference:1]; }
- (instancetype)initWithPeer:(MCPeerID *)peer securityIdentity:(NSArray *)identity encryptionPreference:(NSInteger)preference {
    if ((self = [super init])) {
        _myPeerID = peer;
        _securityIdentity = identity;
        _encryptionPreference = preference;
    }
    return self;
}
- (NSArray *)connectedPeers { return @[]; }
- (BOOL)sendData:(NSData *)data toPeers:(NSArray *)peers withMode:(NSInteger)mode error:(NSError **)error {
    if (error) *error = [NSError errorWithDomain:@"MCErrorDomain" code:2 userInfo:nil];   // MCErrorNotConnected
    return NO;
}
- (void)disconnect {}
- (void)connectPeer:(MCPeerID *)peer withNearbyConnectionData:(NSData *)data {}
- (void)cancelConnectPeer:(MCPeerID *)peer {}
@end

@interface MCNearbyServiceAdvertiser : NSObject
@property (weak) id delegate;
@property (readonly) MCPeerID *myPeerID;
@property (readonly) NSDictionary *discoveryInfo;
@property (readonly) NSString *serviceType;
@end

@implementation MCNearbyServiceAdvertiser
- (instancetype)initWithPeer:(MCPeerID *)peer discoveryInfo:(NSDictionary *)info serviceType:(NSString *)type {
    if ((self = [super init])) {
        _myPeerID = peer;
        _discoveryInfo = [info copy];
        _serviceType = [type copy];
    }
    return self;
}
- (void)startAdvertisingPeer {}
- (void)stopAdvertisingPeer {}
@end

@interface MCNearbyServiceBrowser : NSObject
@property (weak) id delegate;
@property (readonly) MCPeerID *myPeerID;
@property (readonly) NSString *serviceType;
@end

@implementation MCNearbyServiceBrowser
- (instancetype)initWithPeer:(MCPeerID *)peer serviceType:(NSString *)type {
    if ((self = [super init])) {
        _myPeerID = peer;
        _serviceType = [type copy];
    }
    return self;
}
- (void)startBrowsingForPeers {}
- (void)stopBrowsingForPeers {}
- (void)invitePeer:(MCPeerID *)peer toSession:(MCSession *)session withContext:(NSData *)context timeout:(NSTimeInterval)timeout {}
@end
