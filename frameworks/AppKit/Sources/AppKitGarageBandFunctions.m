// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// AppKit functions GarageBand imports and Reef's AppKit lacks.
#import <AppKit/AppKit.h>
#import <objc/runtime.h>

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

// Test aid: REEF_PRESS="seconds:Title;seconds:Title ..." presses the button with that title (in any window) that
// many seconds after launch, also during modal sessions; REEF_PRESS_LIST=seconds prints the buttons of every window.
static NSButton *ReefFindButton(NSView *v, NSString *title) {
    if ([v isKindOfClass:[NSButton class]] && !v.hidden && [((NSButton *)v).title isEqualToString:title]) return (NSButton *)v;
    for (NSView *s in v.subviews) {
        NSButton *b = ReefFindButton(s, title);
        if (b) return b;
    }
    return nil;
}

static void ReefListButtons(NSView *v, int depth) {
    if ([v isKindOfClass:[NSButton class]])
        fprintf(stderr, "  button '%s' %s%s target=%s action=%s\n", ((NSButton *)v).title.UTF8String, NSStringFromRect([v convertRect:v.bounds toView:nil]).UTF8String,
                v.hidden ? " hidden" : "", object_getClassName(((NSButton *)v).target), sel_getName(((NSButton *)v).action));
    for (NSView *s in v.subviews) ReefListButtons(s, depth + 1);
}

static void ReefOnMain(double seconds, void (^block)(void)) {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(seconds * NSEC_PER_SEC)), dispatch_get_global_queue(0, 0), ^{
        CFRunLoopPerformBlock(CFRunLoopGetMain(), kCFRunLoopCommonModes, block);
        CFRunLoopPerformBlock(CFRunLoopGetMain(), (__bridge CFStringRef)NSModalPanelRunLoopMode, block);
        CFRunLoopWakeUp(CFRunLoopGetMain());
    });
}

static void ReefPressLater(NSString *title, double seconds, int tries) {
    ReefOnMain(seconds, ^{
        static NSMutableSet *done;
        if (!done) done = [NSMutableSet set];
        NSString *key = [NSString stringWithFormat:@"%@#%g", title, seconds];
        if ([done containsObject:key]) return;
        [done addObject:key];
        for (NSWindow *w in NSApp.windows) {
            NSButton *b = w.visible ? ReefFindButton(w.contentView, title) : nil;
            if (b) {
                fprintf(stderr, "press: '%s' in '%s'\n", title.UTF8String, w.title.UTF8String);
                [b performClick:nil];
                return;
            }
        }
        if (tries > 0) ReefPressLater(title, 1, tries - 1);
        else fprintf(stderr, "press: no button '%s'\n", title.UTF8String);
    });
}

// The button is looked for once a second for 60 seconds from the given time.
__attribute__((constructor)) static void ReefPressButtons(void) {
    const char *list = getenv("REEF_PRESS_LIST");
    if (list) {
        ReefOnMain(atof(list), ^{
            static BOOL done;
            if (done) return;
            done = YES;
            for (NSWindow *w in NSApp.windows) {
                fprintf(stderr, "window '%s' %s visible=%d key=%d\n", w.title.UTF8String, NSStringFromRect(w.frame).UTF8String, w.visible, w.keyWindow);
                ReefListButtons(w.contentView, 1);
            }
        });
    }
    const char *press = getenv("REEF_PRESS");
    if (!press) return;
    // presses happen in order: each waits for the previous one's window
    double t = 0;
    for (NSString *item in [[NSString stringWithUTF8String:press] componentsSeparatedByString:@";"]) {
        NSRange colon = [item rangeOfString:@":"];
        if (colon.location == NSNotFound) continue;
        t = [[item substringToIndex:colon.location] doubleValue];
        ReefPressLater([item substringFromIndex:colon.location + 1], t, 60);
    }
}

// Test aid: REEF_SEND="seconds:Window title:selector;..." sends the action message to the controller of that window
// (its window controller, else its delegate), with the window as sender.
__attribute__((constructor)) static void ReefSendActions(void) {
    const char *send = getenv("REEF_SEND");
    if (!send) return;
    for (NSString *item in [[NSString stringWithUTF8String:send] componentsSeparatedByString:@";"]) {
        NSArray *parts = [item componentsSeparatedByString:@":"];
        if (parts.count < 3) continue;
        double seconds = [parts[0] doubleValue];
        NSString *title = parts[1];
        NSString *selector = [[parts subarrayWithRange:NSMakeRange(2, parts.count - 2)] componentsJoinedByString:@":"];
        __block BOOL done = NO;
        ReefOnMain(seconds, ^{
            if (done) return;
            done = YES;
            SEL sel = NSSelectorFromString(selector);
            for (NSWindow *w in NSApp.windows) {
                if (![w.title isEqualToString:title]) continue;
                id target = [w.windowController respondsToSelector:sel] ? w.windowController : [w.delegate respondsToSelector:sel] ? w.delegate : nil;
                fprintf(stderr, "send: %s to %s of '%s'\n", selector.UTF8String, object_getClassName(target), title.UTF8String);
                if (target) [NSApp sendAction:sel to:target from:w];
                return;
            }
            fprintf(stderr, "send: no window '%s'\n", title.UTF8String);
        });
    }
}

// Methods of AppKit classes GarageBand sends that Reef's AppKit lacks (added only where missing)
static const void *ActionMaskKey = &ActionMaskKey;
static const void *OneShotKey = &OneShotKey;
static NSInteger cellSendActionOn(NSCell *self, SEL _cmd, NSInteger mask) {
    NSNumber *old = objc_getAssociatedObject(self, ActionMaskKey);
    objc_setAssociatedObject(self, ActionMaskKey, @(mask), OBJC_ASSOCIATION_RETAIN);
    return old ? old.integerValue : NSEventMaskLeftMouseUp;
}
static void windowSetOneShot(NSWindow *self, SEL _cmd, BOOL value) { objc_setAssociatedObject(self, OneShotKey, @(value), OBJC_ASSOCIATION_RETAIN); }
static BOOL windowIsOneShot(NSWindow *self, SEL _cmd) { return [objc_getAssociatedObject(self, OneShotKey) boolValue]; }
static void ignoreBool(id self, SEL _cmd, BOOL value) {}

__attribute__((constructor)) static void addAppKitMethods(void) {
    class_addMethod([NSCell class], @selector(sendActionOn:), (IMP)cellSendActionOn, "q@:q");
    class_addMethod([NSWindow class], @selector(setOneShot:), (IMP)windowSetOneShot, "v@:c");
    class_addMethod([NSWindow class], @selector(isOneShot), (IMP)windowIsOneShot, "c@:");
    class_addMethod([NSWindow class], sel_registerName("setTitlebarAppearsTransparentWhenFullscreen:"), (IMP)ignoreBool, "v@:c");
}

// The contents of a cell (private): the title of a button cell, else the object value
static id cellContents(NSCell *self, SEL _cmd) {
    if ([self isKindOfClass:[NSButtonCell class]]) return ((NSButtonCell *)self).title;
    return self.objectValue;
}
static id cellAltContents(NSCell *self, SEL _cmd) {
    return [self isKindOfClass:[NSButtonCell class]] ? ((NSButtonCell *)self).alternateTitle : nil;
}
static void cellSetContents(NSCell *self, SEL _cmd, id value) {
    if ([self isKindOfClass:[NSButtonCell class]] && [value isKindOfClass:[NSString class]]) ((NSButtonCell *)self).title = value;
    else self.objectValue = value;
}

__attribute__((constructor)) static void addCellContents(void) {
    class_addMethod([NSCell class], sel_registerName("_contents"), (IMP)cellContents, "@@:");
    class_addMethod([NSCell class], sel_registerName("_altContents"), (IMP)cellAltContents, "@@:");
    class_addMethod([NSCell class], sel_registerName("_setContents:"), (IMP)cellSetContents, "v@:@");
}
