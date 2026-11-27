// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LocalAuthentication: LAContext; no biometry and no passcode exist, so a policy can never be evaluated and says why.

#import <Foundation/Foundation.h>

typedef void (^dispatch_block_t)(void);
extern void dispatch_async(void *queue, dispatch_block_t block);
extern void *dispatch_get_global_queue(long identifier, unsigned long flags);

@interface NSObject (ManualRetain)
- (oneway void)release;
@end

@interface NSError : NSObject
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end

extern NSString *const NSLocalizedDescriptionKey;

NSString *const LAErrorDomain = @"com.apple.LocalAuthentication";

@interface LAContext : NSObject
{
    BOOL _invalidated;
    BOOL _interactionNotAllowed;
    NSString *_localizedFallbackTitle;
    NSString *_localizedCancelTitle;
    NSString *_localizedReason;
    double _touchIDAuthenticationAllowableReuseDuration;
}
@property (copy) NSString *localizedFallbackTitle;
@property (copy) NSString *localizedCancelTitle;
@property (copy) NSString *localizedReason;
@property double touchIDAuthenticationAllowableReuseDuration;
@property BOOL interactionNotAllowed;
@property (readonly) NSInteger biometryType;
@property (readonly) NSData *evaluatedPolicyDomainState;
- (BOOL)canEvaluatePolicy:(NSInteger)policy error:(NSError **)error;
- (void)evaluatePolicy:(NSInteger)policy localizedReason:(NSString *)reason reply:(void (^)(BOOL success, NSError *error))reply;
- (void)invalidate;
@end

// The error for a policy: -10 (invalidated context), -6 (biometry policies) or -5 (passcode).
__attribute__((always_inline)) static NSError *errorForPolicy(BOOL invalidated, NSInteger policy)
{
    if (invalidated)
        return [NSError errorWithDomain:@"com.apple.LocalAuthentication" code:-10 userInfo:nil];
    _Bool biometry = policy == 1 || policy == 4;
    NSInteger code = biometry ^ -5;
    NSString *message = biometry ? @"Biometry is not available on this device." : @"Passcode not set.";
    return [NSError errorWithDomain:@"com.apple.LocalAuthentication" code:code userInfo:@{NSLocalizedDescriptionKey: message}];
}

@implementation LAContext

- (void)dealloc
{
    [_localizedFallbackTitle release];
    [_localizedCancelTitle release];
    [_localizedReason release];
    [super dealloc];
}

- (NSInteger)biometryType { return 0; }
- (id)evaluatedPolicyDomainState { return nil; }

- (BOOL)canEvaluatePolicy:(NSInteger)policy error:(NSError **)error
{
    if (error)
        *error = errorForPolicy(_invalidated, policy);
    return NO;
}

- (void)evaluatePolicy:(NSInteger)policy localizedReason:(NSString *)reason reply:(void (^)(BOOL, NSError *))reply
{
    NSError *error = errorForPolicy(_invalidated, policy);
    void (^handler)(BOOL, NSError *) = [reply copy];
    dispatch_async(dispatch_get_global_queue(0x19, 0), ^{
        handler(NO, error);
        [(id)handler release];
    });
}

- (void)invalidate { _invalidated = YES; }

@end
