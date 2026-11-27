// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// HIToolbox functions GarageBand imports and Reef's Carbon lacks, answered from AppKit's state.
#import <AppKit/AppKit.h>

Boolean CheckEventQueueForUserCancel(void) { return false; }
UInt32 GetCurrentButtonState(void) { return (UInt32)[NSEvent pressedMouseButtons]; }
UInt32 GetCurrentEventButtonState(void) { return (UInt32)[NSEvent pressedMouseButtons]; }
OSStatus GetMenuTrackingData(void *menu, void *data) { return -5620; }   // menuNotFoundErr: no menu is being tracked

// kHICoordSpace72DPIGlobal and kHICoordSpaceScreenPixel: origin at the top left of the main screen
OSStatus HIGetMousePosition(UInt32 space, void *object, CGPoint *point) {
    NSPoint p = [NSEvent mouseLocation];
    CGFloat height = NSScreen.screens.count ? NSScreen.screens[0].frame.size.height : 0;
    if (point) *point = CGPointMake(p.x, height - p.y);
    return 0;
}

OSStatus RunCurrentEventLoop(double seconds) {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, seconds, true);
    return -9875;   // eventLoopTimedOutErr
}
