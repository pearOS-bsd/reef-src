// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// FileProvider: domains and item versions are plain value classes; the manager has no provider extensions to talk to, so
// every request answers with a "not supported" error on a background queue.

#import <Foundation/Foundation.h>
#include <dispatch/dispatch.h>
#include <stdbool.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@interface NSString (FileProvider)
- (NSString *)stringByAppendingPathComponent:(NSString *)component;
- (NSUInteger)hash;
- (BOOL)isEqualToString:(NSString *)other;
@end
@interface NSURL : NSObject
+ (NSURL *)fileURLWithPath:(NSString *)path isDirectory:(BOOL)directory;
@end
@interface NSError : NSObject
+ (NSError *)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end
@interface NSUUID : NSObject
+ (NSUUID *)UUID;
- (NSString *)UUIDString;
@end
@interface NSBundle : NSObject
+ (NSBundle *)mainBundle;
- (NSString *)bundleIdentifier;
@end
@interface NSData : NSObject
- (id)initWithBytes:(const void *)bytes length:(NSUInteger)length;
@end
extern NSString *NSHomeDirectory(void);

typedef long dispatch_once_t;
extern void dispatch_once(dispatch_once_t *predicate, void (^block)(void));
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, void (^block)(void))
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once

NSString *const NSFileProviderRootContainerItemIdentifier = @"NSFileProviderRootContainerItemIdentifier";
NSString *const NSFileProviderWorkingSetContainerItemIdentifier = @"NSFileProviderWorkingSetContainerItemIdentifier";
NSString *const NSFileProviderTrashContainerItemIdentifier = @"NSFileProviderTrashContainerItemIdentifier";
NSString *const NSFileProviderErrorDomain = @"NSFileProviderErrorDomain";

NSData *NSFileProviderInitialPageSortedByDate;
NSData *NSFileProviderInitialPageSortedByName;

static __attribute__((constructor)) void makePages(void)
{
    static const unsigned char byDate[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    static const unsigned char byName[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    NSFileProviderInitialPageSortedByDate = [[NSData alloc] initWithBytes:byDate length:8];
    NSFileProviderInitialPageSortedByName = [[NSData alloc] initWithBytes:byName length:8];
}

enum {
    NSFileProviderErrorProviderNotFound = -2001,
    NSFileProviderErrorServerUnreachable = -2013,
};

// Calls the completion handler with the error on a background queue.
static void answer(NSError *error, void (^completion)(NSError *))
{
    if (completion) {
        void (^handler)(NSError *) = [completion copy];
        dispatch_async(dispatch_get_global_queue(0x15, 0), ^{
            handler(error);
            [handler release];
        });
    }
}

@interface NSFileProviderDomain : NSObject
{
    BOOL _disconnected;
    BOOL _hidden;
    BOOL _supportsSyncingTrash;
    NSString *_identifier;
    NSString *_displayName;
}
@property (readonly, copy) NSString *identifier;
@property (readonly, copy) NSString *displayName;
@property (readonly, getter=isDisconnected) BOOL disconnected;
@property (readonly) BOOL userEnabled;
@property (readonly, getter=isHidden) BOOL hidden;
@property (readonly, getter=isReplicated) BOOL replicated;
@property BOOL supportsSyncingTrash;
- (id)initWithIdentifier:(NSString *)identifier displayName:(NSString *)displayName;
- (id)initWithDisplayName:(NSString *)displayName;
@end

@implementation NSFileProviderDomain
- (id)initWithIdentifier:(NSString *)identifier displayName:(NSString *)displayName
{
    if ((self = [super init])) {
        _identifier = [identifier copy];
        _displayName = [displayName copy];
    }
    return self;
}

- (id)initWithDisplayName:(NSString *)displayName
{
    return [self initWithIdentifier:[[NSUUID UUID] UUIDString] displayName:displayName];
}

- (void)dealloc
{
    [_identifier release];
    [_displayName release];
    [super dealloc];
}

- (BOOL)userEnabled { return YES; }
- (BOOL)isReplicated { return YES; }

- (BOOL)isEqual:(id)object
{
    bool equal = false;
    if ([object isKindOfClass:[NSFileProviderDomain class]])
        equal = [_identifier isEqualToString:[object identifier]];
    return equal;
}

- (NSUInteger)hash { return [_identifier hash]; }
@end

@interface NSFileProviderItemVersion : NSObject
{
    NSData *_contentVersion;
    NSData *_metadataVersion;
}
@property (readonly) NSData *contentVersion;
@property (readonly) NSData *metadataVersion;
- (id)initWithContentVersion:(NSData *)contentVersion metadataVersion:(NSData *)metadataVersion;
@end

@implementation NSFileProviderItemVersion
- (id)initWithContentVersion:(NSData *)contentVersion metadataVersion:(NSData *)metadataVersion
{
    if ((self = [super init])) {
        _contentVersion = [contentVersion copy];
        _metadataVersion = [metadataVersion copy];
    }
    return self;
}

- (void)dealloc
{
    [_contentVersion release];
    [_metadataVersion release];
    [super dealloc];
}
@end

@interface NSFileProviderManager : NSObject
{
    NSFileProviderDomain *_domain;
}
@property (readonly) NSURL *documentStorageURL;
@property (readonly, copy) NSString *providerIdentifier;
@property (class, readonly) NSFileProviderManager *defaultManager;
+ (NSFileProviderManager *)defaultManager;
+ (NSFileProviderManager *)managerForDomain:(NSFileProviderDomain *)domain;
+ (void)getDomainsWithCompletionHandler:(void (^)(NSArray *, NSError *))completionHandler;
+ (void)addDomain:(NSFileProviderDomain *)domain completionHandler:(void (^)(NSError *))completionHandler;
+ (void)removeDomain:(NSFileProviderDomain *)domain completionHandler:(void (^)(NSError *))completionHandler;
+ (void)removeDomain:(NSFileProviderDomain *)domain mode:(long)mode completionHandler:(void (^)(NSURL *, NSError *))completionHandler;
+ (void)removeAllDomainsWithCompletionHandler:(void (^)(NSError *))completionHandler;
- (NSURL *)temporaryDirectoryURLWithError:(NSError **)error;
- (void)signalEnumeratorForContainerItemIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *))completionHandler;
- (void)reconnectWithCompletionHandler:(void (^)(NSError *))completionHandler;
- (void)disconnectWithReason:(NSString *)reason options:(unsigned long)options completionHandler:(void (^)(NSError *))completionHandler;
- (void)evictItemWithIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *))completionHandler;
- (void)waitForChangesOnItemsBelowItemWithIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *))completionHandler;
- (void)waitForStabilizationWithCompletionHandler:(void (^)(NSError *))completionHandler;
- (void)signalErrorResolved:(NSError *)error completionHandler:(void (^)(NSError *))completionHandler;
- (void)getUserVisibleURLForItemIdentifier:(NSString *)identifier completionHandler:(void (^)(NSURL *, NSError *))completionHandler;
@end

#define SERVER_UNREACHABLE [NSError errorWithDomain:NSFileProviderErrorDomain code:NSFileProviderErrorServerUnreachable userInfo:nil]

@implementation NSFileProviderManager
+ (NSFileProviderManager *)defaultManager
{
    static dispatch_once_t once;
    static NSFileProviderManager *manager;
    dispatch_once(&once, ^{
        manager = [[NSFileProviderManager alloc] init];
    });
    return manager;
}

+ (NSFileProviderManager *)managerForDomain:(NSFileProviderDomain *)domain
{
    NSFileProviderManager *manager = [[[self alloc] init] autorelease];
    manager->_domain = [domain retain];
    return manager;
}

+ (void)getDomainsWithCompletionHandler:(void (^)(NSArray *, NSError *))completionHandler
{
    void (^handler)(NSArray *, NSError *) = [completionHandler copy];
    dispatch_async(dispatch_get_global_queue(0x15, 0), ^{
        handler(@[], nil);
        [handler release];
    });
}

+ (void)addDomain:(NSFileProviderDomain *)domain completionHandler:(void (^)(NSError *))completionHandler
{
    answer([NSError errorWithDomain:NSFileProviderErrorDomain code:NSFileProviderErrorProviderNotFound userInfo:nil], completionHandler);
}

+ (void)removeDomain:(NSFileProviderDomain *)domain completionHandler:(void (^)(NSError *))completionHandler
{
    answer(SERVER_UNREACHABLE, completionHandler);
}

+ (void)removeDomain:(NSFileProviderDomain *)domain mode:(long)mode completionHandler:(void (^)(NSURL *, NSError *))completionHandler
{
    void (^handler)(NSURL *, NSError *) = [completionHandler copy];
    dispatch_async(dispatch_get_global_queue(0x15, 0), ^{
        handler(nil, SERVER_UNREACHABLE);
        [handler release];
    });
}

+ (void)removeAllDomainsWithCompletionHandler:(void (^)(NSError *))completionHandler
{
    answer(nil, completionHandler);
}

- (void)dealloc
{
    [_domain release];
    [super dealloc];
}

- (NSURL *)documentStorageURL
{
    NSString *path = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/CloudStorage"];
    return [NSURL fileURLWithPath:path isDirectory:YES];
}

- (NSString *)providerIdentifier
{
    NSString *identifier = [[NSBundle mainBundle] bundleIdentifier];
    return identifier ? identifier : @"";
}

- (NSURL *)temporaryDirectoryURLWithError:(NSError **)error
{
    if (error)
        *error = SERVER_UNREACHABLE;
    return nil;
}

- (void)signalEnumeratorForContainerItemIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *))completionHandler
{
    answer(SERVER_UNREACHABLE, completionHandler);
}

- (void)reconnectWithCompletionHandler:(void (^)(NSError *))completionHandler
{
    answer(SERVER_UNREACHABLE, completionHandler);
}

- (void)disconnectWithReason:(NSString *)reason options:(unsigned long)options completionHandler:(void (^)(NSError *))completionHandler
{
    answer(SERVER_UNREACHABLE, completionHandler);
}

- (void)evictItemWithIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *))completionHandler
{
    answer(SERVER_UNREACHABLE, completionHandler);
}

- (void)waitForChangesOnItemsBelowItemWithIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *))completionHandler
{
    answer(SERVER_UNREACHABLE, completionHandler);
}

- (void)waitForStabilizationWithCompletionHandler:(void (^)(NSError *))completionHandler
{
    answer(SERVER_UNREACHABLE, completionHandler);
}

- (void)signalErrorResolved:(NSError *)error completionHandler:(void (^)(NSError *))completionHandler
{
    answer(SERVER_UNREACHABLE, completionHandler);
}

- (void)getUserVisibleURLForItemIdentifier:(NSString *)identifier completionHandler:(void (^)(NSURL *, NSError *))completionHandler
{
    void (^handler)(NSURL *, NSError *) = [completionHandler copy];
    dispatch_async(dispatch_get_global_queue(0x15, 0), ^{
        handler(nil, SERVER_UNREACHABLE);
        [handler release];
    });
}
@end
