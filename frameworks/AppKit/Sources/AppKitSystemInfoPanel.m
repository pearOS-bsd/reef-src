// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <objc/message.h>

// The About panel of AppKit (private): QuickTime Player reaches into its views.
@interface NSSystemInfoPanel : NSObject {
@public
    NSDictionary *_optionsDictionary;
    NSView *copyrightView;
    NSPanel *infoPanel;
}
@end
@implementation NSSystemInfoPanel
@end

// Whether the system draws with the glass material of macOS 26 (Solarium). Reef's AppKit draws its own look: no.
BOOL NSSolariumEnabled(void) { return NO; }

// Autosaving of documents (NSDocumentController private methods): files live in ~/Library/Autosave Information.
static NSMutableSet<NSURL *> *ReefReservedAutosaves;
__attribute__((constructor)) static void ReefAutosaveMethods(void) {
    Class c = NSClassFromString(@"NSDocumentController");
    if (!c) return;
    ReefReservedAutosaves = [NSMutableSet set];
    class_addMethod(c, NSSelectorFromString(@"_autosaveDirectoryURLCreateIfNecessary:"), imp_implementationWithBlock(^NSURL *(id s, BOOL create) {
        NSURL *dir = [NSURL fileURLWithPath:[NSHomeDirectory() stringByAppendingPathComponent:@"Library/Autosave Information"] isDirectory:YES];
        if (create) [[NSFileManager defaultManager] createDirectoryAtURL:dir withIntermediateDirectories:YES attributes:nil error:NULL];
        return dir;
    }), "@@:c");
    class_addMethod(c, NSSelectorFromString(@"_uniqueAutosavedContentsURLWithFileNameExtension:"), imp_implementationWithBlock(^NSURL *(id s, NSString *ext) {
        NSURL *dir = ((NSURL * (*)(id, SEL, BOOL))objc_msgSend)(s, NSSelectorFromString(@"_autosaveDirectoryURLCreateIfNecessary:"), YES);
        NSString *name = [NSUUID UUID].UUIDString;
        return [dir URLByAppendingPathComponent:ext.length ? [name stringByAppendingPathExtension:ext] : name];
    }), "@@:@");
    class_addMethod(c, NSSelectorFromString(@"_reserveAutosavingContentsURL:"), imp_implementationWithBlock(^BOOL(id s, NSURL *url) {
        if ([ReefReservedAutosaves containsObject:url]) return NO;
        [ReefReservedAutosaves addObject:url];
        return YES;
    }), "c@:@");
    class_addMethod(c, NSSelectorFromString(@"_unreserveAutosavingContentsURL:"), imp_implementationWithBlock(^(id s, NSURL *url) {
        [ReefReservedAutosaves removeObject:url];
    }), "v@:@");
}
