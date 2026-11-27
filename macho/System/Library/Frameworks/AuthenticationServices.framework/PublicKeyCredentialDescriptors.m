// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AuthenticationServices: public key credential descriptors.

#import "AuthenticationServices.h"
#include <dispatch/dispatch.h>
#include <stdbool.h>

extern NSString *const NSLocalizedDescriptionKey;

NSArray *ASAuthorizationAllSupportedPublicKeyCredentialDescriptorTransports(void)
{
    return @[@"usb", @"nfc", @"ble"];
}

@interface ASAuthorizationPlatformPublicKeyCredentialDescriptor : NSObject <NSCopying, NSSecureCoding>
{
    NSData *_credentialID;
}
@property (copy) NSData *credentialID;
- (id)initWithCredentialID:(NSData *)credentialID;
@end

@implementation ASAuthorizationPlatformPublicKeyCredentialDescriptor

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)initWithCredentialID:(NSData *)credentialID
{
    self = [super init];
    if (self)
        _credentialID = [credentialID copy];
    return self;
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self initWithCredentialID:[coder decodeObjectOfClass:[NSData class] forKey:@"credentialID"]];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_credentialID forKey:@"credentialID"];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (void)dealloc
{
    [_credentialID release];
    [super dealloc];
}

@end

@interface ASAuthorizationSecurityKeyPublicKeyCredentialDescriptor : NSObject <NSCopying, NSSecureCoding>
{
    NSData *_credentialID;
    NSArray *_transports;
}
@property (copy) NSData *credentialID;
@property (copy) NSArray *transports;
- (id)initWithCredentialID:(NSData *)credentialID transports:(NSArray *)transports;
@end

@implementation ASAuthorizationSecurityKeyPublicKeyCredentialDescriptor

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)initWithCredentialID:(NSData *)credentialID transports:(NSArray *)transports
{
    self = [super init];
    if (self) {
        _credentialID = [credentialID copy];
        _transports = [transports copy];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)coder
{
    NSSet *classes = [NSSet setWithObjects:[NSArray class], [NSString class], nil];
    NSData *credentialID = [coder decodeObjectOfClass:[NSData class] forKey:@"credentialID"];
    return [self initWithCredentialID:credentialID transports:[coder decodeObjectOfClasses:classes forKey:@"transports"]];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_credentialID forKey:@"credentialID"];
    [coder encodeObject:_transports forKey:@"transports"];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (void)dealloc
{
    [_credentialID release];
    [_transports release];
    [super dealloc];
}

@end
