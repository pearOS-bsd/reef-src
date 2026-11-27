// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// SafariServices: every request is answered at once with no result or the unavailable error; a universal link cannot be created.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSError : NSObject
- (NSInteger)code;
- (NSString *)domain;
@end

@interface SFSafariApplication : NSObject
+ (void)getActiveWindowWithCompletionHandler:(void (^)(id window))handler;
+ (void)getAllWindowsWithCompletionHandler:(void (^)(NSArray *windows))handler;
+ (void)openWindowWithURL:(id)url completionHandler:(void (^)(id window))handler;
+ (void)getHostApplicationWithCompletionHandler:(void (^)(id application))handler;
+ (NSError *)unavailable;
+ (void)dispatchMessageWithName:(NSString *)name toExtensionWithIdentifier:(NSString *)identifier userInfo:(NSDictionary *)userInfo completionHandler:(void (^)(NSError *error))handler;
+ (void)showPreferencesForExtensionWithIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *error))handler;
+ (void)setToolbarItemsNeedUpdate;
@end
@interface SFSafariExtensionManager : NSObject
+ (void)getStateOfSafariExtensionWithIdentifier:(NSString *)identifier completionHandler:(void (^)(id state, NSError *error))handler;
@end
@interface SFUniversalLink : NSObject
@property (readonly) id webpageURL, applicationURL;
@property BOOL enabled;
- (id)initWithWebpageURL:(id)url;
@end

int main(void)
{
    [SFSafariApplication getActiveWindowWithCompletionHandler:^(id w) { printf("active %d\n", w == nil); }];
    [SFSafariApplication getAllWindowsWithCompletionHandler:^(NSArray *w) { printf("all %lu\n", (unsigned long)[w count]); }];
    [SFSafariApplication openWindowWithURL:nil completionHandler:^(id w) { printf("open %d\n", w == nil); }];
    [SFSafariApplication getHostApplicationWithCompletionHandler:^(id a) { printf("host %d\n", a == nil); }];
    [SFSafariApplication dispatchMessageWithName:@"m" toExtensionWithIdentifier:@"x" userInfo:nil completionHandler:^(NSError *e) { printf("dispatch %ld %s\n", (long)[e code], [[e domain] UTF8String]); }];
    [SFSafariApplication showPreferencesForExtensionWithIdentifier:@"x" completionHandler:^(NSError *e) { printf("prefs %ld\n", (long)[e code]); }];
    [SFSafariExtensionManager getStateOfSafariExtensionWithIdentifier:@"x" completionHandler:^(id s, NSError *e) { printf("state %d %ld\n", s == nil, (long)[e code]); }];
    [SFSafariApplication getActiveWindowWithCompletionHandler:nil];
    [SFSafariApplication setToolbarItemsNeedUpdate];
    printf("unavailable %ld\n", (long)[[SFSafariApplication unavailable] code]);
    SFUniversalLink *link = [[SFUniversalLink alloc] initWithWebpageURL:nil];
    printf("link %d\n", link == nil);
    SFUniversalLink *other = [SFUniversalLink new];
    other.enabled = YES;
    printf("props %d %d %d\n", other.webpageURL == nil, other.applicationURL == nil, other.enabled);
    return 0;
}
