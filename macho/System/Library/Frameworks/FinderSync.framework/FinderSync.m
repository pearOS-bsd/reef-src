// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FinderSync.

#import <Foundation/Foundation.h>

@class NSImage;
@protocol FIFinderSyncProtocol
@end


@interface FIFinderSync : NSObject <FIFinderSyncProtocol>
@property (readonly, copy) NSString * toolbarItemName;
@property (readonly, copy) NSImage * toolbarItemImage;
@property (readonly, copy) NSString * toolbarItemToolTip;

@end

@implementation FIFinderSync
@end

@interface FIFinderSyncController : NSObject
{
    NSSet * _directoryURLs;
}
@property (copy) NSSet * directoryURLs;
+ (id)defaultController;
+ (BOOL)isExtensionEnabled;
+ (void)showExtensionManagementInterface;
- (void)dealloc;
- (void)setBadgeImage:(id)a0 label:(id)a1 forBadgeIdentifier:(id)a2;
- (void)setBadgeIdentifier:(id)a0 forURL:(id)a1;
- (id)targetedURL;
- (id)selectedItemURLs;
- (id)lastUsedToolbarItem;
@end

@implementation FIFinderSyncController
+ (id)defaultController
{
    // unresolved
    return nil;
}

+ (BOOL)isExtensionEnabled
{
    return NO;
}

+ (void)showExtensionManagementInterface
{

}

- (void)dealloc
{
    // unresolved

}

- (void)setBadgeImage:(id)a0 label:(id)a1 forBadgeIdentifier:(id)a2
{

}

- (void)setBadgeIdentifier:(id)a0 forURL:(id)a1
{

}

- (id)targetedURL
{
    return nil;
}

- (id)selectedItemURLs
{
    return nil;
}

- (id)lastUsedToolbarItem
{
    return nil;
}

@end


