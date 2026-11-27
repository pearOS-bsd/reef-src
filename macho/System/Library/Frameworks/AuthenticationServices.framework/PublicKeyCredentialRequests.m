// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AuthenticationServices: the platform and security key public key credential requests.

#import "AuthenticationServices.h"

@implementation ASAuthorizationPlatformPublicKeyCredentialAssertionRequest

- (void)dealloc
{
    [_challenge release];
    [_relyingPartyIdentifier release];
    [_allowedCredentials release];
    [_userVerificationPreference release];
    [super dealloc];
}

@end

@implementation ASAuthorizationSecurityKeyPublicKeyCredentialAssertionRequest
@end

@implementation ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest

- (void)dealloc
{
    [_challenge release];
    [_relyingPartyIdentifier release];
    [_name release];
    [_displayName release];
    [_userID release];
    [_userVerificationPreference release];
    [super dealloc];
}

@end

@implementation ASAuthorizationSecurityKeyPublicKeyCredentialRegistrationRequest
@end
@implementation ASAuthorizationPlatformPublicKeyCredentialProvider

- (id)initWithRelyingPartyIdentifier:(NSString *)relyingPartyIdentifier
{
    self = [super init];
    if (self)
        _relyingPartyIdentifier = [relyingPartyIdentifier copy];
    return self;
}

- (void)dealloc
{
    [_relyingPartyIdentifier release];
    [super dealloc];
}

- (Class)_assertionClass
{
    return [ASAuthorizationPlatformPublicKeyCredentialAssertionRequest class];
}

- (Class)_registrationClass
{
    return [ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest class];
}

- (id)createCredentialAssertionRequestWithChallenge:(NSData *)challenge
{
    ASAuthorizationPlatformPublicKeyCredentialAssertionRequest *request = [[[self _assertionClass] alloc] _initWithProvider:self];
    [request setChallenge:challenge];
    [request setRelyingPartyIdentifier:_relyingPartyIdentifier];
    [request setAllowedCredentials:@[]];
    [request setUserVerificationPreference:@"preferred"];
    return [request autorelease];
}

- (id)createCredentialRegistrationRequestWithChallenge:(NSData *)challenge name:(NSString *)name userID:(NSData *)userID
{
    ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest *request = [[[self _registrationClass] alloc] _initWithProvider:self];
    [request setChallenge:challenge];
    [request setRelyingPartyIdentifier:_relyingPartyIdentifier];
    [request setName:name];
    [request setUserID:userID];
    [request setUserVerificationPreference:@"preferred"];
    return [request autorelease];
}

@end

@implementation ASAuthorizationSecurityKeyPublicKeyCredentialProvider

- (Class)_assertionClass
{
    return [ASAuthorizationSecurityKeyPublicKeyCredentialAssertionRequest class];
}

- (Class)_registrationClass
{
    return [ASAuthorizationSecurityKeyPublicKeyCredentialRegistrationRequest class];
}

- (id)createCredentialRegistrationRequestWithChallenge:(NSData *)challenge displayName:(NSString *)displayName name:(NSString *)name userID:(NSData *)userID
{
    ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest *request = [self createCredentialRegistrationRequestWithChallenge:challenge name:name userID:userID];
    [request setDisplayName:displayName];
    return request;
}

@end

@interface ASAuthorizationPlatformPublicKeyCredentialAssertion : NSObject <NSCopying, NSSecureCoding>
{
    NSData *_rawAuthenticatorData;
    NSData *_userID;
    NSData *_signature;
    NSData *_rawClientDataJSON;
    NSData *_credentialID;
    NSString *_attachment;
}
@property (readonly, copy) NSData *rawAuthenticatorData;
@property (readonly, copy) NSData *userID;
@property (readonly, copy) NSData *signature;
@property (readonly, copy) NSData *rawClientDataJSON;
@property (readonly, copy) NSData *credentialID;
@property (readonly, copy) NSString *attachment;
@end

@implementation ASAuthorizationPlatformPublicKeyCredentialAssertion

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

@end

@interface ASAuthorizationSecurityKeyPublicKeyCredentialAssertion : ASAuthorizationPlatformPublicKeyCredentialAssertion
{
    BOOL _appID;
}
@property (readonly) BOOL appID;
@end

@implementation ASAuthorizationSecurityKeyPublicKeyCredentialAssertion
@end

@interface ASAuthorizationPlatformPublicKeyCredentialRegistration : NSObject <NSCopying, NSSecureCoding>
{
    NSData *_rawAttestationObject;
    NSData *_rawClientDataJSON;
    NSData *_credentialID;
    NSString *_attachment;
}
@property (readonly, copy) NSData *rawAttestationObject;
@property (readonly, copy) NSData *rawClientDataJSON;
@property (readonly, copy) NSData *credentialID;
@property (readonly, copy) NSString *attachment;
@end

@implementation ASAuthorizationPlatformPublicKeyCredentialRegistration

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

@end
