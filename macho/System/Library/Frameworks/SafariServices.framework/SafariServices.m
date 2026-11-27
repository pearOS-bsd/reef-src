// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SafariServices: there is no Safari, so every request is answered at once, with no result or with the "unavailable" error.

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
@end

@class NSURL;

@interface NSError : NSObject
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end

@interface SFSafariApplication : NSObject
+ (void)getActiveWindowWithCompletionHandler:(void (^)(id window))handler;
+ (void)getAllWindowsWithCompletionHandler:(void (^)(NSArray *windows))handler;
+ (void)openWindowWithURL:(NSURL *)url completionHandler:(void (^)(id window))handler;
+ (void)getHostApplicationWithCompletionHandler:(void (^)(id application))handler;
+ (NSError *)unavailable;
+ (void)dispatchMessageWithName:(NSString *)name toExtensionWithIdentifier:(NSString *)identifier userInfo:(NSDictionary *)userInfo completionHandler:(void (^)(NSError *error))handler;
+ (void)showPreferencesForExtensionWithIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *error))handler;
+ (void)setToolbarItemsNeedUpdate;
@end

@implementation SFSafariApplication

+ (void)getActiveWindowWithCompletionHandler:(void (^)(id))handler
{
    if (handler)
        handler(nil);
}

+ (void)getAllWindowsWithCompletionHandler:(void (^)(NSArray *))handler
{
    if (handler)
        handler(@[]);
}

+ (void)openWindowWithURL:(NSURL *)url completionHandler:(void (^)(id))handler
{
    if (handler)
        handler(nil);
}

+ (void)getHostApplicationWithCompletionHandler:(void (^)(id))handler
{
    if (handler)
        handler(nil);
}

+ (NSError *)unavailable
{
    return [NSError errorWithDomain:@"SFErrorDomain" code:1 userInfo:nil];
}

+ (void)dispatchMessageWithName:(NSString *)name toExtensionWithIdentifier:(NSString *)identifier userInfo:(NSDictionary *)userInfo completionHandler:(void (^)(NSError *))handler
{
    if (handler)
        handler([self unavailable]);
}

+ (void)showPreferencesForExtensionWithIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *))handler
{
    if (handler)
        handler([self unavailable]);
}

+ (void)setToolbarItemsNeedUpdate
{
}

@end

@interface SFSafariExtensionHandler : NSObject
@end

@implementation SFSafariExtensionHandler
@end

@interface SFSafariExtensionManager : NSObject
+ (void)getStateOfSafariExtensionWithIdentifier:(NSString *)identifier completionHandler:(void (^)(id state, NSError *error))handler;
@end

@implementation SFSafariExtensionManager

+ (void)getStateOfSafariExtensionWithIdentifier:(NSString *)identifier completionHandler:(void (^)(id, NSError *))handler
{
    if (handler)
        handler(nil, [SFSafariApplication unavailable]);
}

@end

@interface SFUniversalLink : NSObject
@property (readonly) NSURL *webpageURL;
@property (readonly) NSURL *applicationURL;
@property BOOL enabled;
- (id)initWithWebpageURL:(NSURL *)url;
@end

@implementation SFUniversalLink

- (id)initWithWebpageURL:(NSURL *)url
{
    [self release];
    return nil;
}

- (NSURL *)webpageURL { return nil; }
- (NSURL *)applicationURL { return nil; }
- (BOOL)enabled { return NO; }
- (void)setEnabled:(BOOL)enabled { }

@end
