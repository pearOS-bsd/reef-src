// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FinderSync: the extension controller of Finder Sync; Finder has no such extensions here, so everything but the watched directories is empty.

#import <Foundation/Foundation.h>

typedef long dispatch_once_t;
extern void dispatch_once(dispatch_once_t *predicate, void (^block)(void));
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, void (^block)(void))
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@class NSImage, NSMenu, NSURL;

@interface NSSet : NSObject
+ (id)set;
@end

@protocol FIFinderSyncProtocol
@optional
- (NSMenu *)menuForMenuKind:(NSUInteger)menuKind;
- (void)beginObservingDirectoryAtURL:(NSURL *)url;
- (void)endObservingDirectoryAtURL:(NSURL *)url;
- (void)requestBadgeIdentifierForURL:(NSURL *)url;
@property (readonly, copy) NSString *toolbarItemName;
@property (readonly, copy) NSImage *toolbarItemImage;
@property (readonly, copy) NSString *toolbarItemToolTip;
@end

@interface FIFinderSync : NSObject <FIFinderSyncProtocol>
@end

@implementation FIFinderSync
@end

@interface FIFinderSyncController : NSObject
{
    NSSet *_directoryURLs;
}
@property (copy) NSSet *directoryURLs;
+ (id)defaultController;
+ (BOOL)isExtensionEnabled;
+ (void)showExtensionManagementInterface;
- (void)setBadgeImage:(id)image label:(id)label forBadgeIdentifier:(id)identifier;
- (void)setBadgeIdentifier:(id)identifier forURL:(id)url;
- (id)targetedURL;
- (id)selectedItemURLs;
- (id)lastUsedToolbarItem;
@end

@implementation FIFinderSyncController

+ (id)defaultController
{
    static FIFinderSyncController *controller;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        controller = [[FIFinderSyncController alloc] init];
    });
    return controller;
}

+ (BOOL)isExtensionEnabled { return NO; }
+ (void)showExtensionManagementInterface { }

- (void)dealloc
{
    [_directoryURLs release];
    [super dealloc];
}

- (NSSet *)directoryURLs
{
    @synchronized (self) {
        NSSet *urls = [[_directoryURLs retain] autorelease];
        return urls ? urls : [NSSet set];
    }
}

- (void)setDirectoryURLs:(NSSet *)urls
{
    @synchronized (self) {
        NSSet *old = _directoryURLs;
        _directoryURLs = [urls copy];
        [old release];
    }
}

- (void)setBadgeImage:(id)image label:(id)label forBadgeIdentifier:(id)identifier { }
- (void)setBadgeIdentifier:(id)identifier forURL:(id)url { }
- (id)targetedURL { return nil; }
- (id)selectedItemURLs { return nil; }
- (id)lastUsedToolbarItem { return nil; }

@end
