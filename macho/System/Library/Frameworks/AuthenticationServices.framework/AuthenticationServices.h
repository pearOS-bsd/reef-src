// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AuthenticationServices: declarations shared by the source files.

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@interface NSError : NSObject
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end
@interface NSArray (AuthenticationServices)
+ (id)arrayWithObjects:(const id *)objects count:(NSUInteger)count;
@end
@interface NSSet : NSObject
+ (id)setWithObjects:(id)first, ...;
@end
@interface NSCoder : NSObject
- (id)decodeObjectOfClass:(Class)cls forKey:(NSString *)key;
- (id)decodeObjectOfClasses:(NSSet *)classes forKey:(NSString *)key;
- (void)encodeObject:(id)object forKey:(NSString *)key;
@end
@interface NSString (AuthenticationServices)
- (NSString *)lowercaseString;
- (BOOL)isEqualToString:(NSString *)string;
@end
@interface NSURL : NSObject
- (NSString *)scheme;
- (NSString *)host;
- (NSString *)path;
@end
@class NSData, NSHTTPURLResponse, NSPersonNameComponents;

@interface ASAuthorizationRequest : NSObject <NSCopying, NSSecureCoding>
{
    id _provider;
}
@property (readonly, strong) id provider;
- (id)_initWithProvider:(id)provider;
@end

@interface ASAuthorizationPlatformPublicKeyCredentialAssertionRequest : ASAuthorizationRequest
{
    NSData *_challenge;
    NSString *_relyingPartyIdentifier;
    NSArray *_allowedCredentials;
    NSString *_userVerificationPreference;
}
@property (copy) NSData *challenge;
@property (copy) NSString *relyingPartyIdentifier;
@property (copy) NSArray *allowedCredentials;
@property (copy) NSString *userVerificationPreference;
@end

@interface ASAuthorizationSecurityKeyPublicKeyCredentialAssertionRequest : ASAuthorizationPlatformPublicKeyCredentialAssertionRequest
@end

@interface ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest : ASAuthorizationRequest
{
    NSData *_challenge;
    NSString *_relyingPartyIdentifier;
    NSString *_name;
    NSString *_displayName;
    NSData *_userID;
    NSString *_userVerificationPreference;
}
@property (copy) NSData *challenge;
@property (copy) NSString *relyingPartyIdentifier;
@property (copy) NSString *name;
@property (copy) NSString *displayName;
@property (copy) NSData *userID;
@property (copy) NSString *userVerificationPreference;
@end

@interface ASAuthorizationSecurityKeyPublicKeyCredentialRegistrationRequest : ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest
@end

@interface ASAuthorizationPlatformPublicKeyCredentialProvider : NSObject
{
    NSString *_relyingPartyIdentifier;
}
@property (readonly, copy) NSString *relyingPartyIdentifier;
- (id)initWithRelyingPartyIdentifier:(NSString *)relyingPartyIdentifier;
- (Class)_assertionClass;
- (Class)_registrationClass;
- (id)createCredentialAssertionRequestWithChallenge:(NSData *)challenge;
- (id)createCredentialRegistrationRequestWithChallenge:(NSData *)challenge name:(NSString *)name userID:(NSData *)userID;
@end

@interface ASAuthorizationSecurityKeyPublicKeyCredentialProvider : ASAuthorizationPlatformPublicKeyCredentialProvider
- (id)createCredentialRegistrationRequestWithChallenge:(NSData *)challenge displayName:(NSString *)displayName name:(NSString *)name userID:(NSData *)userID;
@end
