// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// AuthenticationServices: requests and providers, credentials, the controller, web authentication sessions and the public key credential classes.
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSError : NSObject
- (NSString *)domain;
- (NSInteger)code;
- (NSString *)localizedDescription;
@end
@interface NSURL : NSObject
+ (NSURL *)URLWithString:(NSString *)string;
@end
@interface NSData : NSObject
+ (id)dataWithBytes:(const void *)bytes length:(NSUInteger)length;
- (NSUInteger)length;
@end
@interface NSObject (T)
- (NSUInteger)count;
- (id)objectAtIndex:(NSUInteger)index;
- (id)copy;
- (id)objectForKey:(id)key;
@end

extern void *class_getInstanceVariable(Class cls, const char *name);
extern long ivar_getOffset(void *ivar);
extern const char *class_getName(Class cls);

static void setIvar(id object, const char *name, id value)
{
    void *ivar = class_getInstanceVariable([object class], name);
    *(void **)((char *)(__bridge void *)object + ivar_getOffset(ivar)) = (__bridge_retained void *)value;
}

@interface ASAuthorizationRequest : NSObject <NSCopying>
@property (readonly, strong) id provider;
- (id)_initWithProvider:(id)provider;
@end
@interface ASAuthorizationOpenIDRequest : ASAuthorizationRequest
@property (copy) NSArray *requestedScopes;
@property (copy) NSString *state;
@property (copy) NSString *nonce;
@property (copy) NSString *requestedOperation;
@end
@interface ASAuthorizationSingleSignOnRequest : ASAuthorizationOpenIDRequest
@property (copy) NSArray *authorizationOptions;
@property (getter=isUserInterfaceEnabled) BOOL userInterfaceEnabled;
@end
@interface ASAuthorizationSingleSignOnProvider : NSObject
@property (readonly) NSURL *url;
@property (readonly) BOOL canPerformAuthorization;
+ (id)authorizationProviderWithIdentityProviderURL:(NSURL *)url;
- (id)createRequest;
@end
@interface ASAuthorizationSingleSignOnCredential : NSObject <NSCopying>
@property (readonly, copy) NSString *state;
@property (readonly, copy) NSData *accessToken;
@property (readonly, copy) NSData *identityToken;
@property (readonly, copy) NSArray *authorizedScopes;
@property (readonly, copy) id authenticatedResponse;
@property (readonly, copy) NSArray *privateKeys;
+ (BOOL)supportsSecureCoding;
@end
@interface ASAuthorizationAppleIDCredential : NSObject <NSCopying>
@property (readonly, copy) NSString *user;
@property (readonly, copy) NSString *state;
@property (readonly, copy) NSArray *authorizedScopes;
@property (readonly, copy) NSData *authorizationCode;
@property (readonly, copy) NSData *identityToken;
@property (readonly, copy) NSString *email;
@property (readonly, copy) id fullName;
@property (readonly) long realUserStatus;
@property (readonly) long userAgeRange;
@end
@interface ASAuthorizationController : NSObject
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
@interface ASWebAuthenticationSessionCallback : NSObject
+ (id)callbackWithCustomScheme:(NSString *)scheme;
+ (id)callbackWithHTTPSHost:(NSString *)host path:(NSString *)path;
- (BOOL)matchesURL:(NSURL *)url;
@end
@interface ASWebAuthenticationSession : NSObject
- (id)initWithURL:(NSURL *)url callbackURLScheme:(NSString *)scheme completionHandler:(void (^)(NSURL *callbackURL, NSError *error))handler;
- (id)initWithURL:(NSURL *)url callback:(ASWebAuthenticationSessionCallback *)callback completionHandler:(void (^)(NSURL *callbackURL, NSError *error))handler;
- (id)presentationContextProvider;
- (void)setPresentationContextProvider:(id)provider;
- (BOOL)prefersEphemeralWebBrowserSession;
- (void)setPrefersEphemeralWebBrowserSession:(BOOL)prefers;
- (NSDictionary *)additionalHeaderFields;
- (void)setAdditionalHeaderFields:(NSDictionary *)fields;
- (BOOL)canStart;
- (BOOL)start;
- (void)cancel;
@end
@interface ASWebAuthenticationSessionWebBrowserSessionManager : NSObject
@property (strong) id sessionHandler;
@property (readonly) BOOL wasLaunchedByAuthenticationServices;
+ (id)sharedManager;
@end
@interface ASAuthorizationPlatformPublicKeyCredentialDescriptor : NSObject <NSCopying>
@property (copy) NSData *credentialID;
- (id)initWithCredentialID:(NSData *)credentialID;
@end
@interface ASAuthorizationSecurityKeyPublicKeyCredentialDescriptor : NSObject <NSCopying>
@property (copy) NSData *credentialID;
@property (copy) NSArray *transports;
- (id)initWithCredentialID:(NSData *)credentialID transports:(NSArray *)transports;
@end
@interface ASAuthorizationPlatformPublicKeyCredentialAssertionRequest : ASAuthorizationRequest
@property (copy) NSData *challenge;
@property (copy) NSString *relyingPartyIdentifier;
@property (copy) NSArray *allowedCredentials;
@property (copy) NSString *userVerificationPreference;
@end
@interface ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest : ASAuthorizationRequest
@property (copy) NSData *challenge;
@property (copy) NSString *relyingPartyIdentifier;
@property (copy) NSString *name;
@property (copy) NSString *displayName;
@property (copy) NSData *userID;
@property (copy) NSString *userVerificationPreference;
@end
@interface ASAuthorizationPlatformPublicKeyCredentialProvider : NSObject
@property (readonly, copy) NSString *relyingPartyIdentifier;
- (id)initWithRelyingPartyIdentifier:(NSString *)relyingPartyIdentifier;
- (id)createCredentialAssertionRequestWithChallenge:(NSData *)challenge;
- (id)createCredentialRegistrationRequestWithChallenge:(NSData *)challenge name:(NSString *)name userID:(NSData *)userID;
@end
@interface ASAuthorizationSecurityKeyPublicKeyCredentialProvider : ASAuthorizationPlatformPublicKeyCredentialProvider
- (id)createCredentialRegistrationRequestWithChallenge:(NSData *)challenge displayName:(NSString *)displayName name:(NSString *)name userID:(NSData *)userID;
@end
@interface ASAuthorizationPlatformPublicKeyCredentialAssertion : NSObject <NSCopying>
@property (readonly, copy) NSData *signature;
+ (BOOL)supportsSecureCoding;
@end
@interface ASAuthorizationSecurityKeyPublicKeyCredentialAssertion : ASAuthorizationPlatformPublicKeyCredentialAssertion
@property (readonly) BOOL appID;
@end
@interface ASAuthorizationPlatformPublicKeyCredentialRegistration : NSObject <NSCopying>
@property (readonly, copy) NSString *attachment;
+ (BOOL)supportsSecureCoding;
@end
extern NSString *const ASAuthorizationErrorDomain;
extern NSString *const ASAuthorizationOperationImplicit;
extern NSString *const ASAuthorizationOperationLogout;
extern NSString *const ASWebAuthenticationSessionErrorDomain;
extern NSString *const ASAuthorizationPublicKeyCredentialUserVerificationPreferencePreferred;
extern NSString *const ASAuthorizationSecurityKeyPublicKeyCredentialDescriptorTransportNFC;
extern NSString *const ASAuthorizationPublicKeyCredentialAttestationKindEnterprise;

@interface Delegate : NSObject
@end
static volatile int arrived;
@implementation Delegate
- (void)authorizationController:(id)controller didCompleteWithError:(NSError *)error
{
    printf("delegate %d %s %ld %s\n", controller != nil, [error.domain UTF8String], (long)error.code, [error.localizedDescription UTF8String]);
    arrived++;
}
@end
@interface Plain : NSObject
@end
@implementation Plain
@end

int main(void)
{
    ASAuthorizationOpenIDRequest *request = [[ASAuthorizationOpenIDRequest alloc] _initWithProvider:@"provider"];
    printf("request %s %s %lu\n", [request.provider UTF8String], [request.requestedOperation UTF8String], (unsigned long)[request.requestedScopes count]);
    request.requestedScopes = @[@"a", @"b"]; request.state = @"s"; request.nonce = @"n"; request.requestedOperation = ASAuthorizationOperationLogout;
    printf("request %lu %s %s %s %d\n", (unsigned long)[request.requestedScopes count], [request.state UTF8String], [request.nonce UTF8String], [request.requestedOperation UTF8String], [request copy] == request);
    ASAuthorizationRequest *plain = [[ASAuthorizationRequest alloc] _initWithProvider:nil];
    printf("plain %d\n", plain.provider == nil);

    ASAuthorizationSingleSignOnProvider *sso = [ASAuthorizationSingleSignOnProvider authorizationProviderWithIdentityProviderURL:[NSURL URLWithString:@"https://idp.example/"]];
    printf("sso %d %d\n", sso.url != nil, sso.canPerformAuthorization);
    ASAuthorizationSingleSignOnRequest *ssoRequest = [sso createRequest];
    printf("sso request %d %lu %s %d\n", ssoRequest.provider == sso, (unsigned long)[ssoRequest.authorizationOptions count], [ssoRequest.requestedOperation UTF8String], ssoRequest.userInterfaceEnabled);
    ssoRequest.userInterfaceEnabled = NO; ssoRequest.authorizationOptions = @[@"x"];
    printf("sso request %lu %d\n", (unsigned long)[ssoRequest.authorizationOptions count], ssoRequest.userInterfaceEnabled);

    ASAuthorizationSingleSignOnCredential *credential = [ASAuthorizationSingleSignOnCredential new];
    printf("credential %lu %lu %d %d %d %d %d\n", (unsigned long)[credential.authorizedScopes count], (unsigned long)[credential.privateKeys count], credential.state == nil, credential.accessToken == nil, credential.identityToken == nil, credential.authenticatedResponse == nil, [credential copy] == credential);
    setIvar(credential, "_authorizedScopes", @[@"openid"]); setIvar(credential, "_state", @"state"); setIvar(credential, "_accessToken", [NSData dataWithBytes:"ab" length:2]);
    printf("credential %lu %s %lu %d\n", (unsigned long)[credential.authorizedScopes count], [credential.state UTF8String], (unsigned long)[credential.accessToken length], [ASAuthorizationSingleSignOnCredential supportsSecureCoding]);
    setIvar(credential, "_authorizedScopes", nil); setIvar(credential, "_state", nil); setIvar(credential, "_accessToken", nil);

    ASAuthorizationAppleIDCredential *apple = [ASAuthorizationAppleIDCredential new];
    printf("apple %lu %d %d %ld %ld %d\n", (unsigned long)[apple.authorizedScopes count], apple.user == nil, apple.email == nil, apple.realUserStatus, apple.userAgeRange, [apple copy] == apple);
    setIvar(apple, "_user", @"user"); setIvar(apple, "_email", @"a@b");
    printf("apple %s %s\n", [apple.user UTF8String], [apple.email UTF8String]);
    setIvar(apple, "_user", nil); setIvar(apple, "_email", nil);

    ASAuthorizationController *controller = [[ASAuthorizationController alloc] initWithAuthorizationRequests:@[request, plain]];
    printf("controller %lu %lu %d\n", (unsigned long)[controller.authorizationRequests count], (unsigned long)[[controller customAuthorizationMethods] count], controller.delegate == nil);
    [controller setCustomAuthorizationMethods:@[@"x"]];
    [controller cancel];
    Delegate *delegate = [Delegate new];
    controller.delegate = delegate; controller.presentationContextProvider = @"window";
    printf("controller %d %s\n", controller.delegate == delegate, [controller.presentationContextProvider UTF8String]);
    controller.delegate = [Plain new];
    [controller performRequests];
    usleep(100000);
    printf("arrived %d\n", arrived);

    ASWebAuthenticationSessionCallback *scheme = [ASWebAuthenticationSessionCallback callbackWithCustomScheme:@"MyApp"];
    ASWebAuthenticationSessionCallback *https = [ASWebAuthenticationSessionCallback callbackWithHTTPSHost:@"example.com" path:@"/cb"];
    ASWebAuthenticationSessionCallback *hostOnly = [ASWebAuthenticationSessionCallback callbackWithHTTPSHost:@"example.com" path:nil];
    printf("callbacks %d %d %d | %d %d %d %d | %d %d\n", [scheme matchesURL:[NSURL URLWithString:@"myapp://x"]], [scheme matchesURL:[NSURL URLWithString:@"other://x"]], [scheme matchesURL:[NSURL URLWithString:@"MYAPP:/path"]],
           [https matchesURL:[NSURL URLWithString:@"https://example.com/cb"]], [https matchesURL:[NSURL URLWithString:@"https://example.com/other"]], [https matchesURL:[NSURL URLWithString:@"https://other.com/cb"]], [https matchesURL:[NSURL URLWithString:@"http://example.com/cb"]],
           [hostOnly matchesURL:[NSURL URLWithString:@"https://example.com/anything"]], [hostOnly matchesURL:[NSURL URLWithString:@"https://nope.com/"]]);

    __block int completed = 0;
    ASWebAuthenticationSession *session = [[ASWebAuthenticationSession alloc] initWithURL:[NSURL URLWithString:@"https://idp.example/auth"] callbackURLScheme:@"myapp" completionHandler:^(NSURL *u, NSError *e) {
        printf("session %d %s %ld\n", u == nil, [e.domain UTF8String], (long)e.code);
        completed++;
    }];
    printf("session %d %d %d %d\n", [session canStart], [session start], [session prefersEphemeralWebBrowserSession], [session additionalHeaderFields] == nil);
    [session setPrefersEphemeralWebBrowserSession:YES]; [session setAdditionalHeaderFields:@{@"a": @"b"}]; [session setPresentationContextProvider:@"ctx"];
    printf("session %d %lu %s\n", [session prefersEphemeralWebBrowserSession], (unsigned long)[[session additionalHeaderFields] count], [[session presentationContextProvider] UTF8String]);
    [session cancel]; [session cancel];
    printf("completed %d\n", completed);
    ASWebAuthenticationSession *second = [[ASWebAuthenticationSession alloc] initWithURL:nil callback:https completionHandler:nil];
    [second cancel];
    printf("second %d\n", second != nil);
    ASWebAuthenticationSessionWebBrowserSessionManager *manager = [ASWebAuthenticationSessionWebBrowserSessionManager sharedManager];
    manager.sessionHandler = @"handler";
    printf("manager %d %s %d\n", manager == [ASWebAuthenticationSessionWebBrowserSessionManager sharedManager], [manager.sessionHandler UTF8String], manager.wasLaunchedByAuthenticationServices);

    NSData *id1 = [NSData dataWithBytes:"id" length:2];
    ASAuthorizationPlatformPublicKeyCredentialDescriptor *platform = [[ASAuthorizationPlatformPublicKeyCredentialDescriptor alloc] initWithCredentialID:id1];
    printf("descriptor %lu %d\n", (unsigned long)[platform.credentialID length], [platform copy] == platform);
    ASAuthorizationSecurityKeyPublicKeyCredentialDescriptor *key = [[ASAuthorizationSecurityKeyPublicKeyCredentialDescriptor alloc] initWithCredentialID:id1 transports:@[ASAuthorizationSecurityKeyPublicKeyCredentialDescriptorTransportNFC]];
    printf("key descriptor %lu %lu %d\n", (unsigned long)[key.credentialID length], (unsigned long)[key.transports count], [key copy] == key);
    key.credentialID = nil; key.transports = @[@"usb", @"nfc"];
    printf("key descriptor %d %lu\n", key.credentialID == nil, (unsigned long)[key.transports count]);

    ASAuthorizationPlatformPublicKeyCredentialProvider *provider = [[ASAuthorizationPlatformPublicKeyCredentialProvider alloc] initWithRelyingPartyIdentifier:@"example.com"];
    ASAuthorizationPlatformPublicKeyCredentialAssertionRequest *assertion = [provider createCredentialAssertionRequestWithChallenge:id1];
    printf("assertion %s %s %lu %s %d %d\n", class_getName([assertion class]), [assertion.relyingPartyIdentifier UTF8String], (unsigned long)[assertion.allowedCredentials count], [assertion.userVerificationPreference UTF8String], assertion.provider == provider, [assertion.challenge length] == 2);
    ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest *registration = [provider createCredentialRegistrationRequestWithChallenge:id1 name:@"alice" userID:id1];
    printf("registration %s %s %s %d %d %s\n", class_getName([registration class]), [registration.relyingPartyIdentifier UTF8String], [registration.name UTF8String], registration.displayName == nil, [registration.userID length] == 2, [registration.userVerificationPreference UTF8String]);
    ASAuthorizationSecurityKeyPublicKeyCredentialProvider *keyProvider = [[ASAuthorizationSecurityKeyPublicKeyCredentialProvider alloc] initWithRelyingPartyIdentifier:@"key.example"];
    ASAuthorizationPlatformPublicKeyCredentialAssertionRequest *keyAssertion = [keyProvider createCredentialAssertionRequestWithChallenge:id1];
    ASAuthorizationPlatformPublicKeyCredentialRegistrationRequest *keyRegistration = [keyProvider createCredentialRegistrationRequestWithChallenge:id1 displayName:@"Alice" name:@"alice" userID:id1];
    printf("key %s %s %s %s %s\n", class_getName([keyAssertion class]), class_getName([keyRegistration class]), [keyRegistration.displayName UTF8String], [keyRegistration.name UTF8String], [keyProvider.relyingPartyIdentifier UTF8String]);
    ASAuthorizationPlatformPublicKeyCredentialAssertion *assertionResult = [ASAuthorizationPlatformPublicKeyCredentialAssertion new];
    printf("result %d %d %d\n", assertionResult.signature == nil, [assertionResult copy] == assertionResult, [ASAuthorizationPlatformPublicKeyCredentialAssertion supportsSecureCoding]);
    printf("result %d\n", [ASAuthorizationSecurityKeyPublicKeyCredentialAssertion new].appID);
    ASAuthorizationPlatformPublicKeyCredentialRegistration *registrationResult = [ASAuthorizationPlatformPublicKeyCredentialRegistration new];
    printf("result %d %d %d\n", registrationResult.attachment == nil, [registrationResult copy] == registrationResult, [ASAuthorizationPlatformPublicKeyCredentialRegistration supportsSecureCoding]);
    printf("constants %s %s %s %s %s\n", [ASAuthorizationErrorDomain UTF8String], [ASAuthorizationOperationImplicit UTF8String], [ASWebAuthenticationSessionErrorDomain UTF8String],
           [ASAuthorizationPublicKeyCredentialUserVerificationPreferencePreferred UTF8String], [ASAuthorizationPublicKeyCredentialAttestationKindEnterprise UTF8String]);
    return 0;
}
