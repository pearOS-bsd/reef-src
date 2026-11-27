// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <AppKit/AppKit.h>

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
