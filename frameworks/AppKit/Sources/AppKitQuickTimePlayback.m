// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Private AppKit methods QuickTime Player calls while it shows a movie (signatures of macOS 26's AppKit; contextID is in
// AppKitGarageBandFunctions.m).

#import <AppKit/AppKit.h>
#import <objc/runtime.h>

static char kGestureMask, kTitlebarAlpha, kHostsLayers;

@implementation NSApplication (ReefQuickTime)
// The gesture events the application wants (trackpad gestures): kept, Reef has no such events.
- (unsigned long long)gestureEventMask { return [objc_getAssociatedObject(self, &kGestureMask) unsignedLongLongValue]; }
- (void)setGestureEventMask:(unsigned long long)mask { objc_setAssociatedObject(self, &kGestureMask, @(mask), OBJC_ASSOCIATION_RETAIN_NONATOMIC); }
@end

@implementation NSWindow (ReefQuickTime)
- (unsigned long long)gestureEventMask { return [objc_getAssociatedObject(self, &kGestureMask) unsignedLongLongValue]; }
- (void)setGestureEventMask:(unsigned long long)mask { objc_setAssociatedObject(self, &kGestureMask, @(mask), OBJC_ASSOCIATION_RETAIN_NONATOMIC); }
- (BOOL)canHostLayersInWindowServer { return [objc_getAssociatedObject(self, &kHostsLayers) boolValue]; }
- (void)setCanHostLayersInWindowServer:(BOOL)b { objc_setAssociatedObject(self, &kHostsLayers, @(b), OBJC_ASSOCIATION_RETAIN_NONATOMIC); }
- (double)titlebarAlphaValue
{
    NSNumber *alpha = objc_getAssociatedObject(self, &kTitlebarAlpha);
    return alpha ? alpha.doubleValue : 1;
}
- (void)setTitlebarAlphaValue:(double)alpha { objc_setAssociatedObject(self, &kTitlebarAlpha, @(alpha), OBJC_ASSOCIATION_RETAIN_NONATOMIC); }
// The height of the title bar: the frame above the content layout rectangle.
- (double)titlebarHeight
{
    NSRect frame = self.frame, layout = self.contentLayoutRect;
    double height = NSHeight(frame) - NSMaxY(layout);
    return height > 0 ? height : 28;
}
@end

@implementation NSScreen (ReefQuickTime)
// The size of the screen in device pixels.
- (CGSize)devicePixelCounts
{
    NSRect frame = self.frame;
    double scale = self.backingScaleFactor > 0 ? self.backingScaleFactor : 1;
    return CGSizeMake(frame.size.width * scale, frame.size.height * scale);
}
@end
