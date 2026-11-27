// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// AppKit functions GarageBand imports and Reef's AppKit lacks.
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#import <ctype.h>

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

// Private AppKit methods that GarageBand's toolkit (MAToolKit) sends to its cells, views and windows (added only
// where Reef's AppKit has none): defaults that draw nothing extra.
static NSDictionary *cellTextAttributes(NSCell *self, SEL _cmd) {
    NSMutableParagraphStyle *p = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
    p.alignment = self.alignment;
    p.lineBreakMode = self.lineBreakMode;
    // a mutable dictionary, as AppKit's: callers add their own attributes to it
    return [@{NSFontAttributeName: self.font ?: [NSFont systemFontOfSize:[NSFont systemFontSize]],
              NSForegroundColorAttributeName: self.isEnabled ? [NSColor controlTextColor] : [NSColor disabledControlTextColor],
              NSParagraphStyleAttributeName: p} mutableCopy];
}
static void noRectView(id self, SEL _cmd, NSRect r, id v) {}
static void noArg(id self, SEL _cmd, id a) {}
static void noInt(id self, SEL _cmd, long a) {}
static void noBool(id self, SEL _cmd, BOOL a) {}
static void noArgs(id self, SEL _cmd) {}
static BOOL boolNo(id self, SEL _cmd) { return NO; }
static BOOL boolNoArg(id self, SEL _cmd, id a) { return NO; }
static BOOL boolNoRectView(id self, SEL _cmd, NSRect r, id v) { return NO; }
static id nilResult(id self, SEL _cmd) { return nil; }
static id sameArg(id self, SEL _cmd, id a) { return a; }
static NSRect rectSame(id self, SEL _cmd, NSRect r) { return r; }
static NSRect viewVisibleRect(NSView *self, SEL _cmd) { return self.visibleRect; }
static long cellFocusRing(NSCell *self, SEL _cmd) { return self.focusRingType == NSFocusRingTypeDefault ? NSFocusRingTypeExterior : self.focusRingType; }
static NSAttributedString *cellUnformatted(NSCell *self, SEL _cmd, BOOL b) { return self.attributedStringValue; }
static id viewClipAncestor(NSView *self, SEL _cmd) {
    for (NSView *v = self.superview; v; v = v.superview) if ([v isKindOfClass:[NSClipView class]]) return v;
    return nil;
}
static BOOL viewIsAncestor(NSView *self, SEL _cmd, NSView *v) { return [v isDescendantOf:self]; }
static id windowBorderView(NSWindow *self, SEL _cmd) { return self.contentView.superview; }
static id windowParent(NSWindow *self, SEL _cmd) { return self.parentWindow; }

static void add(Class c, const char *name, IMP imp, const char *types) { class_addMethod(c, sel_registerName(name), imp, types); }

__attribute__((constructor)) static void addToolkitMethods(void) {
    Class cell = [NSCell class], view = [NSView class], window = [NSWindow class], app = [NSApplication class];
    add(cell, "_textAttributes", (IMP)cellTextAttributes, "@@:");
    add(cell, "_drawKeyboardFocusRingWithFrame:inView:", (IMP)noRectView, "v@:{CGRect={CGPoint=dd}{CGSize=dd}}@");
    add(cell, "_drawSortIndicatorIfNecessaryWithFrame:inView:", (IMP)noRectView, "v@:{CGRect={CGPoint=dd}{CGSize=dd}}@");
    add(cell, "_usingAlternateHighlightColorWithFrame:inView:", (IMP)boolNoRectView, "c@:{CGRect={CGPoint=dd}{CGSize=dd}}@");
    add(cell, "_effectiveFocusRingType", (IMP)cellFocusRing, "q@:");
    add(cell, "_shouldShowFocusRingInView:", (IMP)boolNoArg, "c@:@");
    add(cell, "_shouldShowHighlightForSort", (IMP)boolNo, "c@:");
    add(cell, "_setMouseDownFlags:", (IMP)noInt, "v@:i");
    add(cell, "_imageRectWithRect:", (IMP)rectSame, "{CGRect={CGPoint=dd}{CGSize=dd}}@:{CGRect={CGPoint=dd}{CGSize=dd}}");
    add(cell, "_updateInvalidatedObjectValue:", (IMP)noArg, "v@:@");
    add(cell, "_unformattedAttributedStringValue:", (IMP)cellUnformatted, "@@:c");
    add(cell, "_isWhite", (IMP)boolNo, "c@:");
    add(cell, "_setVerticallyCentered:", (IMP)noBool, "v@:c");
    add(cell, "_setEditOnSingleClick:", (IMP)noBool, "v@:c");
    add(cell, "_setLineBorderColor:", (IMP)noArg, "v@:@");
    add(view, "_focusRingClipAncestor", (IMP)nilResult, "@@:");
    add(view, "_focusRingVisibleRect", (IMP)viewVisibleRect, "{CGRect={CGPoint=dd}{CGSize=dd}}@:");
    add(view, "_clipViewAncestor", (IMP)viewClipAncestor, "@@:");
    add(view, "_isAncestorOfViewIdenticalTo:", (IMP)viewIsAncestor, "c@:@");
    add(view, "_findLastViewInKeyViewLoopStartingAtView:", (IMP)sameArg, "@@:@");
    add(view, "_setLastFocusRingView:bleedRegion:", (IMP)noArg, "v@:@{CGRect={CGPoint=dd}{CGSize=dd}}");
    add(window, "_borderView", (IMP)windowBorderView, "@@:");
    add(window, "_parentWindow", (IMP)windowParent, "@@:");
    add(window, "_setDefaultKeyViewLoop", (IMP)noArgs, "v@:");
    add(window, "_setDefaultButtonPaused:", (IMP)noBool, "v@:c");
    add(window, "_defaultButtonPaused", (IMP)boolNo, "c@:");
    add(window, "_setDefaultButtonIndicatorNeedsDisplay", (IMP)noArgs, "v@:");
    add(window, "_startLiveResizeAsTopLevel", (IMP)noArgs, "v@:");
    add(window, "_endLiveResizeAsTopLevel", (IMP)noArgs, "v@:");
    add(window, "_getPositionFromServer", (IMP)noArgs, "v@:");
    add(window, "_setCursorForMouseLocation:", (IMP)noArg, "v@:{CGPoint=dd}");
    add(window, "_windowButtonMouseUp:", (IMP)noArg, "v@:@");
    add(window, "_hasForceCapableTouchDevice", (IMP)boolNo, "c@:");
    add(window, "_eventRelativeToWindow:", (IMP)sameArg, "@@:@");
    add(window, "_setLastFocusRingView:bleedRegion:", (IMP)noArg, "v@:@{CGRect={CGPoint=dd}{CGSize=dd}}");
    add(app, "_cycleWindows:", (IMP)noArg, "v@:@");
    add(app, "_cycleWindowsBackwards:", (IMP)noArg, "v@:@");
    add(object_getClass(app), "_isRunningInIB", (IMP)boolNo, "c@:");
    add(app, "_isRunningInIB", (IMP)boolNo, "c@:");
}

static void textViewChangeBackground(NSTextView *self, SEL _cmd, id sender) {
    if ([sender respondsToSelector:@selector(color)]) self.backgroundColor = [sender color];
}
__attribute__((constructor)) static void addTextViewMethods(void) {
    add([NSTextView class], "changeDocumentBackgroundColor:", (IMP)textViewChangeBackground, "v@:@");
}

// Test aid: REEF_KEYS="seconds:key;seconds:key ..." types into the key window that many seconds after launch.
// A key is a character, Space, Return, Escape or Delete, with "cmd+", "shift+", "alt+" or "ctrl+" in front for
// modifiers, and "/milliseconds" behind to hold it (default 120).
static void ReefPostKeyEvent(NSString *spec) {
    NSEventModifierFlags flags = 0;
    double hold = 0.12;
    NSRange slash = [spec rangeOfString:@"/" options:NSBackwardsSearch];
    if (slash.location != NSNotFound && slash.location > 0) {
        hold = [[spec substringFromIndex:slash.location + 1] doubleValue] / 1000.0;
        spec = [spec substringToIndex:slash.location];
    }
    for (;;) {
        if ([spec hasPrefix:@"cmd+"]) { flags |= NSEventModifierFlagCommand; spec = [spec substringFromIndex:4]; }
        else if ([spec hasPrefix:@"shift+"]) { flags |= NSEventModifierFlagShift; spec = [spec substringFromIndex:6]; }
        else if ([spec hasPrefix:@"alt+"]) { flags |= NSEventModifierFlagOption; spec = [spec substringFromIndex:4]; }
        else if ([spec hasPrefix:@"ctrl+"]) { flags |= NSEventModifierFlagControl; spec = [spec substringFromIndex:5]; }
        else break;
    }
    NSString *chars = spec;
    unsigned short code = 0;
    if ([spec isEqualToString:@"Space"]) { chars = @" "; code = 49; }
    else if ([spec isEqualToString:@"Return"]) { chars = @"\r"; code = 36; }
    else if ([spec isEqualToString:@"Escape"]) { chars = @"\x1b"; code = 53; }
    else if ([spec isEqualToString:@"Delete"]) { chars = @"\x7f"; code = 51; }
    else {
        static const char *keys = "asdfhgzxcv?bqweryt123465=97-80]ou[ip?lj'k;\\,/nm.";
        const char *p = strchr(keys, tolower([spec characterAtIndex:0]));
        if (p && spec.length == 1) code = (unsigned short)(p - keys);
    }
    NSWindow *w = NSApp.keyWindow ?: NSApp.mainWindow ?: NSApp.windows.firstObject;
    fprintf(stderr, "key: %s (%s)\n", spec.UTF8String, w.title.UTF8String);
    NSEvent *down = [NSEvent keyEventWithType:NSEventTypeKeyDown location:NSZeroPoint modifierFlags:flags timestamp:NSProcessInfo.processInfo.systemUptime
                                 windowNumber:w.windowNumber context:nil characters:chars charactersIgnoringModifiers:chars isARepeat:NO keyCode:code];
    [NSApp sendEvent:down];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(hold * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSEvent *up = [NSEvent keyEventWithType:NSEventTypeKeyUp location:NSZeroPoint modifierFlags:flags timestamp:NSProcessInfo.processInfo.systemUptime
                                   windowNumber:w.windowNumber context:nil characters:chars charactersIgnoringModifiers:chars isARepeat:NO keyCode:code];
        [NSApp sendEvent:up];
    });
}

__attribute__((constructor)) static void ReefTypeKeys(void) {
    const char *keys = getenv("REEF_KEYS");
    if (!keys) return;
    for (NSString *item in [[NSString stringWithUTF8String:keys] componentsSeparatedByString:@";"]) {
        NSRange colon = [item rangeOfString:@":"];
        if (colon.location == NSNotFound) continue;
        double seconds = [[item substringToIndex:colon.location] doubleValue];
        NSString *spec = [item substringFromIndex:colon.location + 1];
        __block BOOL done = NO;
        ReefOnMain(seconds, ^{
            if (done) return;
            done = YES;
            ReefPostKeyEvent(spec);
        });
    }
}

// NSBrowser's matrix class (deprecated, column views are matrices of that class): kept, NSMatrix by default
static const void *MatrixClassKey = &MatrixClassKey;
static void browserSetMatrixClass(NSBrowser *self, SEL _cmd, Class cls) { objc_setAssociatedObject(self, MatrixClassKey, cls, OBJC_ASSOCIATION_ASSIGN); }
static Class browserMatrixClass(NSBrowser *self, SEL _cmd) { return objc_getAssociatedObject(self, MatrixClassKey) ?: [NSMatrix class]; }
__attribute__((constructor)) static void addBrowserMatrixClass(void) {
    add([NSBrowser class], "setMatrixClass:", (IMP)browserSetMatrixClass, "v@:#");
    add([NSBrowser class], "matrixClass", (IMP)browserMatrixClass, "#@:");
}

// -[NSTextField setPlaceholderAttributedString:] of Reef's AppKit also sends setPlaceholderString:; a subclass whose
// setPlaceholderString: sets an attributed placeholder through super (GarageBand's toolkit) loops forever. When the
// setter is entered again from there, only the cell's placeholder is set.
static IMP originalSetPlaceholderAttributed;
static __thread int settingPlaceholder;
static void textFieldSetPlaceholderAttributed(NSTextField *self, SEL _cmd, NSAttributedString *value) {
    if (settingPlaceholder) {
        if ([self.cell respondsToSelector:@selector(setPlaceholderAttributedString:)]) [(NSTextFieldCell *)self.cell setPlaceholderAttributedString:value];
        return;
    }
    settingPlaceholder++;
    ((void (*)(id, SEL, id))originalSetPlaceholderAttributed)(self, _cmd, value);
    settingPlaceholder--;
}
__attribute__((constructor)) static void guardPlaceholderSetter(void) {
    Method m = class_getInstanceMethod([NSTextField class], @selector(setPlaceholderAttributedString:));
    if (m) originalSetPlaceholderAttributed = method_setImplementation(m, (IMP)textFieldSetPlaceholderAttributed);
}

// The numeric values of a cell come from its stored object value, as AppKit's do, not through -stringValue: Reef's
// NSCell asked [self stringValue], which subclasses that format their number in stringValue (GarageBand's toolkit)
// answer by asking the number again, forever.
static id cellStoredValue(NSCell *self) {
    static Ivar ivar;
    if (!ivar) ivar = class_getInstanceVariable([NSCell class], "_objectValue");
    return ivar ? object_getIvar(self, ivar) : nil;
}
static double cellDoubleValue(NSCell *self, SEL _cmd) {
    id v = cellStoredValue(self);
    return [v respondsToSelector:@selector(doubleValue)] ? [v doubleValue] : 0;
}
static float cellFloatValue(NSCell *self, SEL _cmd) {
    id v = cellStoredValue(self);
    return [v respondsToSelector:@selector(floatValue)] ? [v floatValue] : 0;
}
static int cellIntValue(NSCell *self, SEL _cmd) {
    id v = cellStoredValue(self);
    return [v respondsToSelector:@selector(intValue)] ? [v intValue] : 0;
}
static NSInteger cellIntegerValue(NSCell *self, SEL _cmd) {
    id v = cellStoredValue(self);
    return [v respondsToSelector:@selector(integerValue)] ? [v integerValue] : 0;
}
__attribute__((constructor)) static void cellNumbersFromObjectValue(void) {
    if (!class_getInstanceVariable([NSCell class], "_objectValue")) return;
    method_setImplementation(class_getInstanceMethod([NSCell class], @selector(doubleValue)), (IMP)cellDoubleValue);
    method_setImplementation(class_getInstanceMethod([NSCell class], @selector(floatValue)), (IMP)cellFloatValue);
    method_setImplementation(class_getInstanceMethod([NSCell class], @selector(intValue)), (IMP)cellIntValue);
    method_setImplementation(class_getInstanceMethod([NSCell class], @selector(integerValue)), (IMP)cellIntegerValue);
}

// Setting a number stores it with NSCell's own setObjectValue:, not through the subclass's override: subclasses that
// turn every object value into setDoubleValue: (GarageBand's toolkit) would otherwise loop.
static IMP cellOwnSetObjectValue;
static void cellSetDouble(NSCell *self, SEL _cmd, double v) { ((void (*)(id, SEL, id))cellOwnSetObjectValue)(self, @selector(setObjectValue:), @(v)); }
static void cellSetFloat(NSCell *self, SEL _cmd, float v) { ((void (*)(id, SEL, id))cellOwnSetObjectValue)(self, @selector(setObjectValue:), @(v)); }
static void cellSetInt(NSCell *self, SEL _cmd, int v) { ((void (*)(id, SEL, id))cellOwnSetObjectValue)(self, @selector(setObjectValue:), @(v)); }
static void cellSetInteger(NSCell *self, SEL _cmd, NSInteger v) { ((void (*)(id, SEL, id))cellOwnSetObjectValue)(self, @selector(setObjectValue:), @(v)); }
__attribute__((constructor)) static void cellNumbersToObjectValue(void) {
    cellOwnSetObjectValue = class_getMethodImplementation([NSCell class], @selector(setObjectValue:));
    if (!cellOwnSetObjectValue) return;
    method_setImplementation(class_getInstanceMethod([NSCell class], @selector(setDoubleValue:)), (IMP)cellSetDouble);
    method_setImplementation(class_getInstanceMethod([NSCell class], @selector(setFloatValue:)), (IMP)cellSetFloat);
    method_setImplementation(class_getInstanceMethod([NSCell class], @selector(setIntValue:)), (IMP)cellSetInt);
    method_setImplementation(class_getInstanceMethod([NSCell class], @selector(setIntegerValue:)), (IMP)cellSetInteger);
}
