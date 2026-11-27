// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AuthenticationServices: web authentication sessions.

#import "AuthenticationServices.h"
#include <dispatch/dispatch.h>
#include <stdbool.h>

extern NSString *const NSLocalizedDescriptionKey;

typedef long dispatch_once_t;
extern void dispatch_once(dispatch_once_t *predicate, void (^block)(void));
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, void (^block)(void))
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once


@interface ASWebAuthenticationSessionCallback : NSObject
{
    NSString *_scheme;
    NSString *_host;
    NSString *_path;
}
+ (id)callbackWithCustomScheme:(NSString *)scheme;
+ (id)callbackWithHTTPSHost:(NSString *)host path:(NSString *)path;
- (BOOL)matchesURL:(NSURL *)url;
@end

@implementation ASWebAuthenticationSessionCallback

+ (id)callbackWithCustomScheme:(NSString *)scheme
{
    ASWebAuthenticationSessionCallback *callback = [[[self alloc] init] autorelease];
    callback->_scheme = [scheme copy];
    return callback;
}

+ (id)callbackWithHTTPSHost:(NSString *)host path:(NSString *)path
{
    ASWebAuthenticationSessionCallback *callback = [[[self alloc] init] autorelease];
    callback->_scheme = @"https";
    callback->_host = [host copy];
    callback->_path = [path copy];
    return callback;
}

- (void)dealloc
{
    [_scheme release];
    [_host release];
    [_path release];
    [super dealloc];
}

- (BOOL)matchesURL:(NSURL *)url
{
    if (![[[url scheme] lowercaseString] isEqualToString:[_scheme lowercaseString]])
        return NO;
    if (_host && ![[url host] isEqualToString:_host])
        return NO;
    if (_path) {
        bool matches = [[url path] isEqualToString:_path];
        return matches;
    }
    return YES;
}

@end

@interface ASWebAuthenticationSession : NSObject
{
    NSURL *_url;
    void (^_completion)(NSURL *callbackURL, NSError *error);
    id _presentationContextProvider;
    BOOL _prefersEphemeralWebBrowserSession;
    NSDictionary *_additionalHeaderFields;
}
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

@implementation ASWebAuthenticationSession

- (id)initWithURL:(NSURL *)url callbackURLScheme:(NSString *)scheme completionHandler:(void (^)(NSURL *callbackURL, NSError *error))handler
{
    self = [super init];
    if (self) {
        _url = [url copy];
        _completion = [handler copy];
    }
    return self;
}

- (id)initWithURL:(NSURL *)url callback:(ASWebAuthenticationSessionCallback *)callback completionHandler:(void (^)(NSURL *callbackURL, NSError *error))handler
{
    return [self initWithURL:url callbackURLScheme:nil completionHandler:handler];
}

- (void)dealloc
{
    [_url release];
    [_completion release];
    [_additionalHeaderFields release];
    [super dealloc];
}

- (id)presentationContextProvider
{
    return _presentationContextProvider;
}

- (void)setPresentationContextProvider:(id)provider
{
    _presentationContextProvider = provider;
}

- (BOOL)prefersEphemeralWebBrowserSession
{
    return _prefersEphemeralWebBrowserSession;
}

- (void)setPrefersEphemeralWebBrowserSession:(BOOL)prefers
{
    _prefersEphemeralWebBrowserSession = prefers;
}

- (NSDictionary *)additionalHeaderFields
{
    return _additionalHeaderFields;
}

- (void)setAdditionalHeaderFields:(NSDictionary *)fields
{
    [_additionalHeaderFields release];
    _additionalHeaderFields = [fields copy];
}

- (BOOL)canStart
{
    return NO;
}

- (BOOL)start
{
    return NO;
}

- (void)cancel
{
    void (^completion)(NSURL *callbackURL, NSError *error) = _completion;
    _completion = nil;
    if (completion) {
        completion(nil, [NSError errorWithDomain:@"com.apple.AuthenticationServices.WebAuthenticationSession" code:1 userInfo:nil]);
        [completion release];
    }
}

@end

@interface ASWebAuthenticationSessionWebBrowserSessionManager : NSObject
{
    id _sessionHandler;
}
@property (strong) id sessionHandler;
@property (readonly) BOOL wasLaunchedByAuthenticationServices;
+ (id)sharedManager;
@end

@implementation ASWebAuthenticationSessionWebBrowserSessionManager

+ (id)sharedManager
{
    static ASWebAuthenticationSessionWebBrowserSessionManager *manager;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        manager = [[ASWebAuthenticationSessionWebBrowserSessionManager alloc] init];
    });
    return manager;
}

- (id)sessionHandler
{
    return _sessionHandler;
}

- (void)setSessionHandler:(id)handler
{
    [handler retain];
    [_sessionHandler release];
    _sessionHandler = handler;
}

- (BOOL)wasLaunchedByAuthenticationServices
{
    return NO;
}

@end
