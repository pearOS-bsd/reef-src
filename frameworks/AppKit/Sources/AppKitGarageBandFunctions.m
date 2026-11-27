// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// AppKit functions GarageBand imports and Reef's AppKit lacks.
#import <AppKit/AppKit.h>

void NSAccessibilityUnregisterUniqueIdForUIElement(id element) {}

// Classic Mac 'TEXT' + 'styl' data: the plain text, in the system font
NSAttributedString *NSAttributedStringFromMacSimpleTextData(NSData *text, NSData *style) {
    if (!text) return nil;
    NSString *s = [[NSString alloc] initWithData:text encoding:NSMacOSRomanStringEncoding];
    return s ? [[NSAttributedString alloc] initWithString:s] : nil;
}

// Private text cell helpers: the size a cell's text needs, and editing through the field editor
NSSize _NSTextCellSize(NSCell *cell, NSSize size) {
    return [cell respondsToSelector:@selector(cellSizeForBounds:)] ? [cell cellSizeForBounds:NSMakeRect(0, 0, size.width, size.height)] : NSZeroSize;
}
void _NSEditTextCell(NSCell *cell, NSRect frame, NSView *view, NSText *editor, id delegate, NSEvent *event) {
    [cell editWithFrame:frame inView:view editor:editor delegate:delegate event:event];
}

// The window server connection of the application (private)
@interface NSApplication (ReefContextID)
- (int)contextID;
@end
@implementation NSApplication (ReefContextID)
- (int)contextID { return getpid(); }
@end
