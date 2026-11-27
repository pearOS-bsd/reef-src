// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// FinderSync: the default controller is one object, keeps a copy of its directory URLs and answers nothing else.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSSet : NSObject
+ (id)set;
+ (id)setWithObject:(id)object;
- (NSUInteger)count;
@end

@interface FIFinderSyncController : NSObject
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

int main(void)
{
    FIFinderSyncController *c = [FIFinderSyncController defaultController];
    printf("same %d\n", c == [FIFinderSyncController defaultController]);
    printf("empty %lu\n", (unsigned long)[c.directoryURLs count]);
    NSSet *set = [NSSet setWithObject:[NSObject new]];
    c.directoryURLs = set;
    printf("set %lu %d\n", (unsigned long)[c.directoryURLs count], c.directoryURLs == set);
    c.directoryURLs = nil;
    printf("cleared %lu\n", (unsigned long)[c.directoryURLs count]);
    [c setBadgeImage:nil label:nil forBadgeIdentifier:nil];
    [c setBadgeIdentifier:nil forURL:nil];
    [FIFinderSyncController showExtensionManagementInterface];
    printf("enabled %d %d %d %d\n", [FIFinderSyncController isExtensionEnabled], [c targetedURL] == nil, [c selectedItemURLs] == nil, [c lastUsedToolbarItem] == nil);
    return 0;
}
