// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <AppKit/AppKit.h>

NSAppearanceName const NSAppearanceNameTouchBar = @"NSAppearanceNameTouchBar";

// Shows the standard About panel: options are the About panel's own dictionary.
void NSShowSystemInfoPanel(NSDictionary *options) {
    [NSApp orderFrontStandardAboutPanelWithOptions:options ?: @{}];
}

@interface NSUserInterfaceCompressionOptions () { @public NSSet<NSString *> *_ids; }
@end

@implementation NSUserInterfaceCompressionOptions
+ (BOOL)supportsSecureCoding { return YES; }
static NSUserInterfaceCompressionOptions *Make(NSString *i) { return [[NSUserInterfaceCompressionOptions alloc] initWithIdentifier:i]; }
+ (NSUserInterfaceCompressionOptions *)hideImagesOption { return Make(@"NSUserInterfaceCompressionOptionHideImages"); }
+ (NSUserInterfaceCompressionOptions *)hideTextOption { return Make(@"NSUserInterfaceCompressionOptionHideText"); }
+ (NSUserInterfaceCompressionOptions *)reduceMetricsOption { return Make(@"NSUserInterfaceCompressionOptionReduceMetrics"); }
+ (NSUserInterfaceCompressionOptions *)breakEqualWidthsOption { return Make(@"NSUserInterfaceCompressionOptionBreakEqualWidths"); }
+ (NSUserInterfaceCompressionOptions *)standardOptions {
    return [[self hideImagesOption] optionsByAddingOptions:[[self hideTextOption] optionsByAddingOptions:[[self reduceMetricsOption] optionsByAddingOptions:[self breakEqualWidthsOption]]]];
}
- (instancetype)init { if ((self = [super init])) _ids = [NSSet set]; return self; }
- (instancetype)initWithIdentifier:(NSString *)identifier { if ((self = [super init])) _ids = [NSSet setWithObject:identifier]; return self; }
- (instancetype)initWithCompressionOptions:(NSSet<NSUserInterfaceCompressionOptions *> *)options {
    if ((self = [super init])) {
        NSMutableSet *s = [NSMutableSet set];
        for (NSUserInterfaceCompressionOptions *o in options) [s unionSet:o->_ids];
        _ids = s;
    }
    return self;
}
- (BOOL)isEmpty { return _ids.count == 0; }
- (BOOL)containsOptions:(NSUserInterfaceCompressionOptions *)o { return [o->_ids isSubsetOfSet:_ids]; }
- (BOOL)intersectsOptions:(NSUserInterfaceCompressionOptions *)o { return [_ids intersectsSet:o->_ids]; }
- (NSUserInterfaceCompressionOptions *)optionsByAddingOptions:(NSUserInterfaceCompressionOptions *)o {
    NSUserInterfaceCompressionOptions *r = [NSUserInterfaceCompressionOptions new]; r->_ids = [_ids setByAddingObjectsFromSet:o->_ids]; return r;
}
- (NSUserInterfaceCompressionOptions *)optionsByRemovingOptions:(NSUserInterfaceCompressionOptions *)o {
    NSUserInterfaceCompressionOptions *r = [NSUserInterfaceCompressionOptions new];
    NSMutableSet *s = [_ids mutableCopy]; [s minusSet:o->_ids]; r->_ids = s; return r;
}
- (id)copyWithZone:(NSZone *)z { return self; }
- (void)encodeWithCoder:(NSCoder *)c { [c encodeObject:_ids.allObjects forKey:@"ids"]; }
- (instancetype)initWithCoder:(NSCoder *)c {
    if ((self = [super init])) _ids = [NSSet setWithArray:[c decodeObjectOfClasses:[NSSet setWithObjects:NSArray.class, NSString.class, nil] forKey:@"ids"] ?: @[]];
    return self;
}
@end

// macOS 26: an appearance that carries a tint colour. Reef's AppKit has no tinted appearances: the
// appearance itself is returned.
@interface NSAppearance (ReefTint)
- (NSAppearance *)appearanceByApplyingTintColor:(NSColor *)color;
@end

@implementation NSAppearance (ReefTint)
- (NSAppearance *)appearanceByApplyingTintColor:(NSColor *)color { return self; }
@end

// Debug aid: REEF_DUMP_VIEWS=<seconds> prints the view tree of every window once.
static void DumpView(NSView *v, int depth) {
    fprintf(stderr, "%*s%s frame=%s hidden=%d alpha=%.2f\n", depth * 2, "", object_getClassName(v),
            NSStringFromRect(v.frame).UTF8String, v.hidden, v.alphaValue);
    for (NSView *s in v.subviews) DumpView(s, depth + 1);
}

__attribute__((constructor)) static void ReefDumpViews(void) {
    const char *t = getenv("REEF_DUMP_VIEWS");
    if (!t) return;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(atof(t) * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        for (NSWindow *w in NSApp.windows) {
            fprintf(stderr, "WINDOW %s %s\n", object_getClassName(w), NSStringFromRect(w.frame).UTF8String);
            DumpView(w.contentView, 1);
            fprintf(stderr, "contentSize=%s min=%s max=%s mask=%lu autoresize=%lu fitting=%s intrinsic=%s styleMask=%lx\n", NSStringFromSize(w.contentView.superview ? w.contentView.superview.frame.size : w.contentLayoutRect.size).UTF8String,
                    NSStringFromSize(w.contentMinSize).UTF8String, NSStringFromSize(w.contentMaxSize).UTF8String, (unsigned long)w.contentView.autoresizingMask, (unsigned long)w.contentView.autoresizingMask,
                    NSStringFromSize(w.contentView.fittingSize).UTF8String, NSStringFromSize(w.contentView.intrinsicContentSize).UTF8String, (unsigned long)w.styleMask);
            if (getenv("REEF_FIX_FRAME")) {
                NSRect r = w.contentView.frame; r.size.width = w.contentLayoutRect.size.width;
                fprintf(stderr, "fix width -> %s\n", NSStringFromRect(r).UTF8String);
                w.contentView.frame = r;
            }
        }
    });
}

// Reef bug: a SwiftUI window whose content has a flexible width can come up with a hosting view of width 0
// (fittingSize is NaN), and the layout never recovers (Calculator). While the content view is that narrow, give it
// the width of the window's content area.
static void ReefFixWindows(void) {
    for (NSWindow *w in NSApp.windows) {
        NSView *v = w.contentView;
        if (!v || ![NSStringFromClass(v.class) containsString:@"NSHostingView"]) continue;
        CGFloat want = w.contentLayoutRect.size.width;
        if (want > 1 && !(v.frame.size.width > 1)) {
            NSRect r = v.frame; r.size.width = want;
            v.frame = r;
        }
    }
}

__attribute__((constructor)) static void ReefFixHostingWidth(void) {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSTimer scheduledTimerWithTimeInterval:0.25 repeats:YES block:^(NSTimer *t) { ReefFixWindows(); }];
    });
}

// Test aid: REEF_INJECT_KEYS="1 2 + 3 =" types the characters into the key window, one every
// REEF_INJECT_INTERVAL seconds (default 0.6), starting REEF_INJECT_DELAY seconds after launch (default 9).
// Names: Return, Escape, Delete (backspace).
static void ReefPostKey(NSString *chars, unsigned short code) {
    NSWindow *w = NSApp.keyWindow ?: NSApp.mainWindow ?: NSApp.windows.firstObject;
    if (!w) { fprintf(stderr, "inject: no window\n"); return; }
    for (NSNumber *down in @[@YES, @NO]) {
        NSEvent *e = [NSEvent keyEventWithType:down.boolValue ? NSEventTypeKeyDown : NSEventTypeKeyUp location:NSZeroPoint modifierFlags:0
                                     timestamp:NSProcessInfo.processInfo.systemUptime windowNumber:w.windowNumber context:nil
                                    characters:chars charactersIgnoringModifiers:chars isARepeat:NO keyCode:code];
        [NSApp sendEvent:e];
    }
}

// REEF_INJECT_CLICKS="x,y x,y ...": left clicks at points of the content area (from its top left).
static void ReefPostClick(double x, double yFromTop) {
    NSWindow *w = NSApp.keyWindow ?: NSApp.mainWindow ?: NSApp.windows.firstObject;
    if (!w) return;
    NSRect content = w.contentView.frame;
    NSPoint p = NSMakePoint(x, content.size.height - yFromTop);
    for (NSNumber *down in @[@YES, @NO]) {
        NSEvent *e = [NSEvent mouseEventWithType:down.boolValue ? NSEventTypeLeftMouseDown : NSEventTypeLeftMouseUp location:p modifierFlags:0
                                       timestamp:NSProcessInfo.processInfo.systemUptime windowNumber:w.windowNumber context:nil eventNumber:0 clickCount:1 pressure:down.boolValue ? 1.0 : 0.0];
        [NSApp sendEvent:e];
    }
}

__attribute__((constructor)) static void ReefInjectClicks(void) {
    const char *pts = getenv("REEF_INJECT_CLICKS");
    if (!pts) return;
    double delay = getenv("REEF_INJECT_DELAY") ? atof(getenv("REEF_INJECT_DELAY")) : 9;
    double interval = getenv("REEF_INJECT_INTERVAL") ? atof(getenv("REEF_INJECT_INTERVAL")) : 0.6;
    NSArray *list = [[NSString stringWithUTF8String:pts] componentsSeparatedByString:@" "];
    for (NSUInteger i = 0; i < list.count; i++) {
        NSArray *xy = [list[i] componentsSeparatedByString:@","];
        if (xy.count != 2) continue;
        double x = [xy[0] doubleValue], y = [xy[1] doubleValue];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)((delay + interval * i) * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            fprintf(stderr, "inject: click %g,%g\n", x, y);
            ReefPostClick(x, y);
        });
    }
}

__attribute__((constructor)) static void ReefInjectKeys(void) {
    const char *keys = getenv("REEF_INJECT_KEYS");
    if (!keys) return;
    double delay = getenv("REEF_INJECT_DELAY") ? atof(getenv("REEF_INJECT_DELAY")) : 9;
    double interval = getenv("REEF_INJECT_INTERVAL") ? atof(getenv("REEF_INJECT_INTERVAL")) : 0.6;
    NSArray *list = [[NSString stringWithUTF8String:keys] componentsSeparatedByString:@" "];
    for (NSUInteger i = 0; i < list.count; i++) {
        NSString *k = list[i];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)((delay + interval * i) * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            fprintf(stderr, "inject: %s\n", k.UTF8String);
            if ([k isEqualToString:@"Return"]) ReefPostKey(@"\r", 36);
            else if ([k isEqualToString:@"Escape"]) ReefPostKey(@"\x1b", 53);
            else if ([k isEqualToString:@"Delete"]) ReefPostKey(@"\x7f", 51);
            else ReefPostKey(k, 0);
        });
    }
}

// Debug aid: REEF_LOG_EVENTS=1 prints the events that reach the application.
__attribute__((constructor)) static void ReefLogEvents(void) {
    if (!getenv("REEF_LOG_EVENTS")) return;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskAny handler:^NSEvent *(NSEvent *e) {
            if (e.type != NSEventTypeMouseMoved || 1) fprintf(stderr, "event: type=%ld loc=%s window=%ld chars=%s\n", (long)e.type, NSStringFromPoint(e.locationInWindow).UTF8String, (long)e.windowNumber, e.type == NSEventTypeKeyDown ? e.characters.UTF8String : "");
            return e;
        }];
    });
}
