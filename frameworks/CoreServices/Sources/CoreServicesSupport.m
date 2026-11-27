// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// LSApplicationRecord of Launch Services (private API): the record of an installed application.
@interface LSApplicationState : NSObject
@property (readonly, getter=isInstalled) BOOL installed;
@property (readonly, getter=isPlaceholder) BOOL placeholder;
@property (readonly, getter=isRestricted) BOOL restricted;
@end

@implementation LSApplicationState
- (instancetype)initInstalled:(BOOL)installed {
    if ((self = [super init])) _installed = installed;
    return self;
}
@end

@interface LSApplicationRecord : NSObject
@property (readonly, copy) NSString *bundleIdentifier;
@property (readonly) LSApplicationState *applicationState;
- (instancetype)initWithBundleIdentifier:(NSString *)bundleIdentifier allowPlaceholder:(BOOL)allowPlaceholder error:(NSError **)error;
@end

@implementation LSApplicationRecord
- (instancetype)initWithBundleIdentifier:(NSString *)bundleIdentifier allowPlaceholder:(BOOL)allowPlaceholder error:(NSError **)error {
    if ((self = [super init])) {
        _bundleIdentifier = [bundleIdentifier copy];
        // Installed means a bundle with this identifier is known to Launch Services.
        BOOL known = [NSBundle bundleWithIdentifier:bundleIdentifier] != nil;
        _applicationState = [[LSApplicationState alloc] initInstalled:known];
        if (!known && error) *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:-10814 userInfo:nil];
    }
    return self;
}
@end

// Spotlight attribute key (MDItem.h); the value is the name.
const CFStringRef kMDItemTextContent = CFSTR("kMDItemTextContent");

// Launch Services and Spotlight pieces of the recent-documents code (QuickTime Player).
CFStringRef _kLSSharedFileListRecentItemsMaxAmount = CFSTR("com.apple.LSSharedFileList.RecentItemsMaxAmount");
LSSharedFileListItemRef _kLSSharedFileListItemBeforeFirst = (LSSharedFileListItemRef)1;
// Reef's lists compare with its own sentinel and key (the names without the underscore): the app's names get their values.
extern LSSharedFileListItemRef ReefItemBeforeFirst __asm__("_kLSSharedFileListItemBeforeFirst");
extern CFStringRef ReefRecentItemsMaxAmount __asm__("_kLSSharedFileListRecentItemsMaxAmount");
__attribute__((constructor)) static void ReefSharedFileListNames(void)
{
    _kLSSharedFileListItemBeforeFirst = ReefItemBeforeFirst;
    _kLSSharedFileListRecentItemsMaxAmount = ReefRecentItemsMaxAmount;
}
Boolean _CSDeviceSupportsAirDrop(void) { return false; }
void _MDItemMarkAsUsedWithURL(CFURLRef url) {}
