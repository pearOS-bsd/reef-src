// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// FileProvider: domains, item versions, the pages, and the manager answering "unreachable" on a background queue.
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSError : NSObject
- (NSInteger)code;
- (NSString *)domain;
@end
@interface NSURL : NSObject
- (NSString *)path;
@end
@interface NSData : NSObject
+ (id)dataWithBytes:(const void *)bytes length:(NSUInteger)length;
- (NSUInteger)length;
@end
@interface NSObject (T)
- (id)copy;
@end
@interface NSArray (T)
- (NSUInteger)count;
@end

extern NSString *const NSFileProviderErrorDomain;
extern NSString *const NSFileProviderRootContainerItemIdentifier;
extern NSData *NSFileProviderInitialPageSortedByDate;
extern NSData *NSFileProviderInitialPageSortedByName;

@interface NSFileProviderDomain : NSObject
- (id)initWithIdentifier:(NSString *)identifier displayName:(NSString *)displayName;
- (id)initWithDisplayName:(NSString *)displayName;
- (NSString *)identifier;
- (NSString *)displayName;
- (BOOL)userEnabled;
- (BOOL)isReplicated;
- (BOOL)isHidden;
- (BOOL)isDisconnected;
- (BOOL)supportsSyncingTrash;
- (void)setSupportsSyncingTrash:(BOOL)value;
@end

@interface NSFileProviderItemVersion : NSObject
- (id)initWithContentVersion:(NSData *)content metadataVersion:(NSData *)metadata;
- (NSData *)contentVersion;
- (NSData *)metadataVersion;
@end

@interface NSFileProviderManager : NSObject
+ (NSFileProviderManager *)defaultManager;
+ (NSFileProviderManager *)managerForDomain:(NSFileProviderDomain *)domain;
+ (void)getDomainsWithCompletionHandler:(void (^)(NSArray *, NSError *))handler;
+ (void)addDomain:(NSFileProviderDomain *)domain completionHandler:(void (^)(NSError *))handler;
+ (void)removeDomain:(NSFileProviderDomain *)domain mode:(long)mode completionHandler:(void (^)(NSURL *, NSError *))handler;
+ (void)removeAllDomainsWithCompletionHandler:(void (^)(NSError *))handler;
- (NSURL *)documentStorageURL;
- (NSString *)providerIdentifier;
- (NSURL *)temporaryDirectoryURLWithError:(NSError **)error;
- (void)evictItemWithIdentifier:(NSString *)identifier completionHandler:(void (^)(NSError *))handler;
- (void)getUserVisibleURLForItemIdentifier:(NSString *)identifier completionHandler:(void (^)(NSURL *, NSError *))handler;
@end

int main(void)
{
    NSFileProviderDomain *a = [[NSFileProviderDomain alloc] initWithIdentifier:@"id.one" displayName:@"One"];
    NSFileProviderDomain *b = [[NSFileProviderDomain alloc] initWithIdentifier:@"id.one" displayName:@"Other"];
    NSFileProviderDomain *c = [[NSFileProviderDomain alloc] initWithDisplayName:@"Generated"];
    printf("domain %s %s enabled %d replicated %d hidden %d disconnected %d trash %d\n", [[a identifier] UTF8String], [[a displayName] UTF8String],
        [a userEnabled], [a isReplicated], [a isHidden], [a isDisconnected], [a supportsSyncingTrash]);
    [a setSupportsSyncingTrash:YES];
    printf("trash %d equal %d %d %d hash %d generated %d\n", [a supportsSyncingTrash], [a isEqual:b], [a isEqual:c], [a isEqual:@"id.one"],
        [a hash] == [b hash], [[c identifier] length] == 36);

    NSData *one = [NSData dataWithBytes:"1" length:1];
    NSFileProviderItemVersion *version = [[NSFileProviderItemVersion alloc] initWithContentVersion:one metadataVersion:nil];
    printf("version %lu %d\n", (unsigned long)[[version contentVersion] length], [version metadataVersion] == nil);
    printf("pages %lu %lu %d\n", (unsigned long)[NSFileProviderInitialPageSortedByDate length], (unsigned long)[NSFileProviderInitialPageSortedByName length],
        NSFileProviderInitialPageSortedByDate != NSFileProviderInitialPageSortedByName);
    printf("root %s error domain %s\n", [NSFileProviderRootContainerItemIdentifier UTF8String], [NSFileProviderErrorDomain UTF8String]);

    NSFileProviderManager *manager = [NSFileProviderManager defaultManager];
    printf("default %d same %d\n", manager != nil, manager == [NSFileProviderManager defaultManager]);
    printf("for domain %d provider %s\n", [NSFileProviderManager managerForDomain:a] != nil, [[manager providerIdentifier] length] >= 0 ? "ok" : "");
    printf("storage %d\n", [[[manager documentStorageURL] path] length] > 0);
    NSError *error = nil;
    NSURL *temporary = [manager temporaryDirectoryURLWithError:&error];
    printf("temporary %d %ld\n", temporary == nil, (long)[error code]);

    [NSFileProviderManager getDomainsWithCompletionHandler:^(NSArray *domains, NSError *e) { printf("domains %lu %d\n", (unsigned long)[domains count], e == nil); }];
    [NSFileProviderManager addDomain:a completionHandler:^(NSError *e) { printf("add %ld %s\n", (long)[e code], [[e domain] UTF8String]); }];
    [NSFileProviderManager removeDomain:a mode:0 completionHandler:^(NSURL *u, NSError *e) { printf("remove %d %ld\n", u == nil, (long)[e code]); }];
    [NSFileProviderManager removeAllDomainsWithCompletionHandler:^(NSError *e) { printf("remove all %d\n", e == nil); }];
    [manager evictItemWithIdentifier:@"x" completionHandler:^(NSError *e) { printf("evict %ld\n", (long)[e code]); }];
    [manager getUserVisibleURLForItemIdentifier:@"x" completionHandler:^(NSURL *u, NSError *e) { printf("visible %d %ld\n", u == nil, (long)[e code]); }];
    [manager evictItemWithIdentifier:@"x" completionHandler:nil];
    usleep(500000);
    return 0;
}
