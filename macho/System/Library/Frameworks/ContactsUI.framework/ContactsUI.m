// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContactsUI: CNContactPicker, a popover that shows "No Contacts" (there is no address book to pick from).

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@protocol NSPopoverDelegate
@end

@protocol CNContactPickerDelegate
@optional
- (void)contactPickerWillClose:(id)picker;
- (void)contactPickerDidClose:(id)picker;
@end

@interface NSBundle : NSObject
+ (NSBundle *)bundleForClass:(Class)cls;
- (NSString *)localizedStringForKey:(NSString *)key value:(NSString *)value table:(NSString *)table;
@end

@interface NSFont : NSObject
+ (CGFloat)systemFontSize;
+ (NSFont *)systemFontOfSize:(CGFloat)size weight:(CGFloat)weight;
@end
@interface NSColor : NSObject
+ (NSColor *)secondaryLabelColor;
@end
@interface NSLayoutAnchor : NSObject
- (id)constraintEqualToAnchor:(NSLayoutAnchor *)anchor;
@end
@interface NSLayoutConstraint : NSObject
+ (void)activateConstraints:(NSArray *)constraints;
@end
@interface NSView : NSObject
- (id)initWithFrame:(CGRect)frame;
- (void)addSubview:(NSView *)view;
- (void)setTranslatesAutoresizingMaskIntoConstraints:(BOOL)flag;
@property (readonly) NSLayoutAnchor *centerXAnchor, *centerYAnchor;
@end
@interface NSTextField : NSView
+ (id)labelWithString:(NSString *)string;
- (void)setFont:(NSFont *)font;
- (void)setTextColor:(NSColor *)color;
@end
@interface NSViewController : NSObject
- (void)setView:(NSView *)view;
@end
@interface NSPopover : NSObject
- (void)setBehavior:(NSInteger)behavior;
- (void)setContentViewController:(NSViewController *)controller;
- (void)setDelegate:(id)delegate;
- (void)showRelativeToRect:(CGRect)rect ofView:(NSView *)view preferredEdge:(NSUInteger)edge;
- (void)close;
@end

extern const CGFloat NSFontWeightSemibold;

@interface CNContactPicker : NSObject <NSPopoverDelegate>
{
    NSArray *_displayedKeys;
    NSPopover *_popover;
    __weak id _delegate;
}
@property (copy) NSArray *displayedKeys;
@property (weak) id delegate;
- (void)showRelativeToRect:(CGRect)rect ofView:(NSView *)view preferredEdge:(NSUInteger)edge;
- (void)close;
@end

@implementation CNContactPicker

- (void)dealloc
{
    [_popover setDelegate:nil];
    [_popover close];
    [_popover release];
    [_displayedKeys release];
    [super dealloc];
}

- (NSArray *)displayedKeys
{
    return [[_displayedKeys retain] autorelease];
}

- (void)setDisplayedKeys:(NSArray *)keys
{
    NSArray *old = _displayedKeys;
    _displayedKeys = [keys copy];
    [old release];
}

- (void)showRelativeToRect:(CGRect)rect ofView:(NSView *)view preferredEdge:(NSUInteger)edge
{
    if (!_popover) {
        _popover = [[NSPopover alloc] init];
        [_popover setBehavior:1];
        NSView *content = [[[NSView alloc] initWithFrame:(CGRect){{0, 0}, {280, 360}}] autorelease];
        NSTextField *label = [NSTextField labelWithString:[[NSBundle bundleForClass:[CNContactPicker class]] localizedStringForKey:@"No Contacts" value:@"" table:nil]];
        [label setFont:[NSFont systemFontOfSize:[NSFont systemFontSize] weight:NSFontWeightSemibold]];
        [label setTextColor:[NSColor secondaryLabelColor]];
        [label setTranslatesAutoresizingMaskIntoConstraints:NO];
        [content addSubview:label];
        [NSLayoutConstraint activateConstraints:@[[[label centerXAnchor] constraintEqualToAnchor:[content centerXAnchor]],
                                                  [[label centerYAnchor] constraintEqualToAnchor:[content centerYAnchor]]]];
        NSViewController *controller = [[[NSViewController alloc] init] autorelease];
        [controller setView:content];
        [_popover setContentViewController:controller];
        [_popover setDelegate:self];
    }
    [_popover showRelativeToRect:rect ofView:view preferredEdge:edge];
}

- (void)close
{
    [_popover close];
}

- (void)popoverWillClose:(id)notification
{
    id delegate = [self delegate];
    if ([delegate respondsToSelector:@selector(contactPickerWillClose:)])
        [delegate contactPickerWillClose:self];
}

- (void)popoverDidClose:(id)notification
{
    id delegate = [self delegate];
    if ([delegate respondsToSelector:@selector(contactPickerDidClose:)])
        [delegate contactPickerDidClose:self];
}

@end
