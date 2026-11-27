// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AuthenticationServices: authorization requests, single sign-on, Apple ID credentials and the controller (no sign-in extension handles a request).

#import "AuthenticationServices.h"
#include <dispatch/dispatch.h>
#include <stdbool.h>

extern NSString *const NSLocalizedDescriptionKey;

@implementation ASAuthorizationRequest

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)_initWithProvider:(id)provider
{
    self = [super init];
    if (self)
        _provider = [provider retain];
    return self;
}

- (void)dealloc
{
    [_provider release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self _initWithProvider:nil];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

@end

@interface ASAuthorizationOpenIDRequest : ASAuthorizationRequest
{
    NSArray *_requestedScopes;
    NSString *_state;
    NSString *_nonce;
    NSString *_requestedOperation;
}
@property (copy) NSArray *requestedScopes;
@property (copy) NSString *state;
@property (copy) NSString *nonce;
@property (copy) NSString *requestedOperation;
@end

@implementation ASAuthorizationOpenIDRequest

- (id)_initWithProvider:(id)provider
{
    self = [super _initWithProvider:provider];
    if (self)
        _requestedOperation = [@"implicit" copy];
    return self;
}

- (void)dealloc
{
    [_requestedScopes release];
    [_state release];
    [_nonce release];
    [_requestedOperation release];
    [super dealloc];
}

@end

@interface ASAuthorizationSingleSignOnRequest : ASAuthorizationOpenIDRequest
{
    BOOL _userInterfaceEnabled;
    NSArray *_authorizationOptions;
}
@property (copy) NSArray *authorizationOptions;
@property (getter=isUserInterfaceEnabled) BOOL userInterfaceEnabled;
@end

@implementation ASAuthorizationSingleSignOnRequest

- (id)_initWithProvider:(id)provider
{
    self = [super _initWithProvider:provider];
    if (self) {
        _authorizationOptions = [[NSArray alloc] init];
        _userInterfaceEnabled = YES;
    }
    return self;
}

- (void)dealloc
{
    [_authorizationOptions release];
    [super dealloc];
}

@end

@interface ASAuthorizationSingleSignOnProvider : NSObject
{
    NSURL *_url;
}
@property (readonly) NSURL *url;
@property (readonly) BOOL canPerformAuthorization;
+ (id)authorizationProviderWithIdentityProviderURL:(NSURL *)url;
- (id)createRequest;
@end

@implementation ASAuthorizationSingleSignOnProvider

+ (id)authorizationProviderWithIdentityProviderURL:(NSURL *)url
{
    ASAuthorizationSingleSignOnProvider *provider = [[[self alloc] init] autorelease];
    provider->_url = [url copy];
    return provider;
}

- (void)dealloc
{
    [_url release];
    [super dealloc];
}

- (BOOL)canPerformAuthorization
{
    return NO;
}

- (id)createRequest
{
    return [[[ASAuthorizationSingleSignOnRequest alloc] _initWithProvider:self] autorelease];
}

@synthesize url = _url;

@end

@interface ASAuthorizationSingleSignOnCredential : NSObject <NSCopying, NSSecureCoding>
{
    NSArray *_authorizedScopes;
    NSArray *_privateKeys;
    NSString *_state;
    NSData *_accessToken;
    NSData *_identityToken;
    NSHTTPURLResponse *_authenticatedResponse;
}
@property (readonly, copy) NSString *state;
@property (readonly, copy) NSData *accessToken;
@property (readonly, copy) NSData *identityToken;
@property (readonly, copy) NSArray *authorizedScopes;
@property (readonly, copy) NSHTTPURLResponse *authenticatedResponse;
@property (readonly, copy) NSArray *privateKeys;
@end

@implementation ASAuthorizationSingleSignOnCredential

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (NSArray *)authorizedScopes
{
    return _authorizedScopes ?: @[];
}

- (NSArray *)privateKeys
{
    return _privateKeys ?: @[];
}

- (void)dealloc
{
    [_state release];
    [_accessToken release];
    [_identityToken release];
    [_authorizedScopes release];
    [_authenticatedResponse release];
    [_privateKeys release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

@end

@interface ASAuthorizationAppleIDCredential : NSObject <NSCopying, NSSecureCoding>
{
    NSArray *_authorizedScopes;
    NSString *_user;
    NSString *_state;
    NSData *_authorizationCode;
    NSData *_identityToken;
    NSString *_email;
    NSPersonNameComponents *_fullName;
    long _realUserStatus;
    long _userAgeRange;
}
@property (readonly, copy) NSString *user;
@property (readonly, copy) NSString *state;
@property (readonly, copy) NSArray *authorizedScopes;
@property (readonly, copy) NSData *authorizationCode;
@property (readonly, copy) NSData *identityToken;
@property (readonly, copy) NSString *email;
@property (readonly, copy) NSPersonNameComponents *fullName;
@property (readonly) long realUserStatus;
@property (readonly) long userAgeRange;
@end

@implementation ASAuthorizationAppleIDCredential

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (NSArray *)authorizedScopes
{
    return _authorizedScopes ?: @[];
}

- (void)dealloc
{
    [_user release];
    [_state release];
    [_authorizedScopes release];
    [_authorizationCode release];
    [_identityToken release];
    [_email release];
    [_fullName release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

@end

@interface ASAuthorizationController : NSObject
{
    NSArray *_authorizationRequests;
    id _delegate;
    id _presentationContextProvider;
}
@property (readonly, copy) NSArray *authorizationRequests;
@property id delegate;
@property id presentationContextProvider;
- (id)initWithAuthorizationRequests:(NSArray *)requests;
- (NSArray *)customAuthorizationMethods;
- (void)setCustomAuthorizationMethods:(NSArray *)methods;
- (void)performRequests;
- (void)performAutoFillAssistedRequests;
- (void)performRequestsWithOptions:(unsigned long)options;
- (void)cancel;
@end

@implementation ASAuthorizationController

- (id)initWithAuthorizationRequests:(NSArray *)requests
{
    self = [super init];
    if (self)
        _authorizationRequests = [requests copy];
    return self;
}

- (void)dealloc
{
    [_authorizationRequests release];
    [super dealloc];
}

- (NSArray *)customAuthorizationMethods
{
    return @[];
}

- (void)setCustomAuthorizationMethods:(NSArray *)methods
{
}

- (void)performRequests
{
    [self retain];
    dispatch_async(dispatch_get_main_queue(), ^{
        id delegate = [self delegate];
        if ([delegate respondsToSelector:@selector(authorizationController:didCompleteWithError:)]) {
            NSError *error = [NSError errorWithDomain:@"com.apple.AuthenticationServices.AuthorizationError" code:1003 userInfo:@{NSLocalizedDescriptionKey: @"No sign-in extension handles this request."}];
            [delegate authorizationController:self didCompleteWithError:error];
        }
        [self release];
    });
}

- (void)performAutoFillAssistedRequests
{
    [self performRequests];
}

- (void)performRequestsWithOptions:(unsigned long)options
{
    [self performRequests];
}

- (void)cancel
{
}

@end

NSString *const ASAuthorizationErrorDomain = @"com.apple.AuthenticationServices.AuthorizationError";
NSString *const ASAuthorizationOperationImplicit = @"implicit";
NSString *const ASAuthorizationOperationLogin = @"login";
NSString *const ASAuthorizationOperationRefresh = @"refresh";
NSString *const ASAuthorizationOperationLogout = @"logout";
NSString *const ASWebAuthenticationSessionErrorDomain = @"com.apple.AuthenticationServices.WebAuthenticationSession";
NSString *const ASAuthorizationPublicKeyCredentialUserVerificationPreferenceRequired = @"required";
NSString *const ASAuthorizationPublicKeyCredentialUserVerificationPreferencePreferred = @"preferred";
NSString *const ASAuthorizationPublicKeyCredentialUserVerificationPreferenceDiscouraged = @"discouraged";
NSString *const ASAuthorizationSecurityKeyPublicKeyCredentialDescriptorTransportUSB = @"usb";
NSString *const ASAuthorizationSecurityKeyPublicKeyCredentialDescriptorTransportNFC = @"nfc";
NSString *const ASAuthorizationSecurityKeyPublicKeyCredentialDescriptorTransportBluetooth = @"ble";
NSString *const ASAuthorizationPublicKeyCredentialAttestationKindNone = @"none";
NSString *const ASAuthorizationPublicKeyCredentialAttestationKindDirect = @"direct";
NSString *const ASAuthorizationPublicKeyCredentialAttestationKindIndirect = @"indirect";
NSString *const ASAuthorizationPublicKeyCredentialAttestationKindEnterprise = @"enterprise";
