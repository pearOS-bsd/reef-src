// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// LocalAuthentication: no policy can be evaluated; the errors, the reply on a global queue, invalidation and the copied titles.
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSError : NSObject
- (NSInteger)code;
- (NSString *)domain;
- (NSDictionary *)userInfo;
@end
extern NSString *const NSLocalizedDescriptionKey;
extern NSString *const LAErrorDomain;

@interface LAContext : NSObject
@property (copy) NSString *localizedFallbackTitle, *localizedCancelTitle, *localizedReason;
@property double touchIDAuthenticationAllowableReuseDuration;
@property BOOL interactionNotAllowed;
@property (readonly) NSInteger biometryType;
@property (readonly) id evaluatedPolicyDomainState;
- (BOOL)canEvaluatePolicy:(NSInteger)policy error:(NSError **)error;
- (void)evaluatePolicy:(NSInteger)policy localizedReason:(NSString *)reason reply:(void (^)(BOOL success, NSError *error))reply;
- (void)invalidate;
@end

static void show(const char *what, NSError *e)
{
    printf("%s: %ld %s %s\n", what, (long)[e code], [[e domain] UTF8String], e.userInfo ? [[e.userInfo objectForKey:NSLocalizedDescriptionKey] UTF8String] : "(no info)");
}

int main(void)
{
    LAContext *c = [LAContext new];
    NSError *e = nil;
    printf("can %d\n", [c canEvaluatePolicy:1 error:&e]); show("bio", e);
    printf("can %d\n", [c canEvaluatePolicy:2 error:&e]); show("pass", e);
    printf("can %d\n", [c canEvaluatePolicy:4 error:&e]); show("watch", e);
    printf("can %d\n", [c canEvaluatePolicy:1 error:NULL]);
    printf("domain %s type %ld state %d\n", [LAErrorDomain UTF8String], (long)c.biometryType, c.evaluatedPolicyDomainState == nil);
    __block int done = 0;
    [c evaluatePolicy:2 localizedReason:@"why" reply:^(BOOL ok, NSError *err) { printf("reply %d\n", ok); show("evaluate", err); done = 1; }];
    for (int i = 0; i < 200 && !done; i++) usleep(10000);
    [c invalidate];
    printf("can %d\n", [c canEvaluatePolicy:1 error:&e]); show("invalid", e);
    done = 0;
    [c evaluatePolicy:1 localizedReason:@"why" reply:^(BOOL ok, NSError *err) { printf("reply %d\n", ok); show("invalid evaluate", err); done = 1; }];
    for (int i = 0; i < 200 && !done; i++) usleep(10000);
    c.localizedFallbackTitle = @"fallback"; c.localizedCancelTitle = @"cancel"; c.localizedReason = @"reason";
    c.touchIDAuthenticationAllowableReuseDuration = 2.5; c.interactionNotAllowed = YES;
    printf("props %s %s %s %g %d\n", [c.localizedFallbackTitle UTF8String], [c.localizedCancelTitle UTF8String], [c.localizedReason UTF8String], c.touchIDAuthenticationAllowableReuseDuration, c.interactionNotAllowed);
    return 0;
}
