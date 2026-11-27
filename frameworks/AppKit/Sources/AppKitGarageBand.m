// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// AppKit classes and private instance variables that GarageBand uses and Reef's AppKit lacks.
#import <AppKit/AppKit.h>

// The offsets of private instance variables that GarageBand's MAToolKit reads and writes directly, mapped onto
// Reef's AppKit: the columns array of NSTableView; the flag words of NSCell, NSBox and NSButtonCell go to bytes
// that Reef's classes leave unused (the padding after their last instance variable).
const long ReefIvarTableColumns __asm__("_OBJC_IVAR_$_NSTableView._tableColumns") = 480;
const long ReefIvarCellFlags __asm__("_OBJC_IVAR_$_NSCell._cFlags") = 176;
const long ReefIvarBoxFlags __asm__("_OBJC_IVAR_$_NSBox._bFlags") = 428;
const long ReefIvarButtonCellFlags __asm__("_OBJC_IVAR_$_NSButtonCell._bcFlags") = 264;

@interface NSBrowserTableView : NSTableView
@end
@implementation NSBrowserTableView
@end

@interface _NSBrowserScrollView : NSScrollView
@end
@implementation _NSBrowserScrollView
@end

@interface _NSBrowserColumnView : NSScrollView
@end
@implementation _NSBrowserColumnView
@end

// A bitmap made from a CGImage
@interface NSCGImageRep : NSBitmapImageRep
@end
@implementation NSCGImageRep
- (instancetype)initWithCGImage:(CGImageRef)image size:(NSSize)size {
    if ((self = [super initWithCGImage:image]))
        self.size = size;
    return self;
}
@end

// Drawers (deprecated): this one never opens
#pragma clang diagnostic ignored "-Wdeprecated-implementations"
@implementation NSDrawer
- (instancetype)initWithContentSize:(NSSize)size preferredEdge:(NSRectEdge)edge {
    if ((self = [super init])) {
        self.contentSize = size;
        self.preferredEdge = edge;
    }
    return self;
}
- (NSInteger)state { return 0; }   // NSDrawerClosedState
- (NSRectEdge)edge { return self.preferredEdge; }
- (void)open {}
- (void)openOnEdge:(NSRectEdge)edge {}
- (void)close {}
- (void)toggle:(id)sender {}
- (void)open:(id)sender {}
- (void)close:(id)sender {}
@end

@interface NSDrawerWindow : NSWindow
@end
@implementation NSDrawerWindow
@end

// Force Touch preview gestures: none happen without a Force Touch trackpad
@interface NSImmediateActionGestureRecognizer : NSGestureRecognizer
@property (readonly) CGFloat animationProgress;
@end
@implementation NSImmediateActionGestureRecognizer
@end

@implementation NSInputManager
+ (NSInputManager *)currentInputManager { return nil; }
+ (void)cycleToNextInputLanguage:(id)sender {}
+ (void)cycleToNextInputServerInLanguage:(id)sender {}
- (BOOL)wantsToHandleMouseEvents { return NO; }
- (BOOL)wantsToInterpretAllKeystrokes { return NO; }
- (BOOL)handleMouseEvent:(NSEvent *)event { return NO; }
- (NSString *)language { return @"en"; }
- (NSString *)localizedInputManagerName { return nil; }
- (void)markedTextAbandoned:(id)client {}
- (void)markedTextSelectionChanged:(NSRange)range client:(id)client {}
@end

// Multi-touch devices: none
@interface NSTouchDevice : NSObject
@end
@implementation NSTouchDevice
+ (NSArray *)devices { return @[]; }
+ (NSArray *)touchDevices { return @[]; }
+ (instancetype)defaultDevice { return nil; }
@end
