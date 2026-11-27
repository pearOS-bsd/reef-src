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
        // REEF_DUMP_FILE=path: apps that take over stderr (crash reporters) still show the dump
        if (getenv("REEF_DUMP_FILE")) freopen(getenv("REEF_DUMP_FILE"), "a", stderr);
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

// Test aid: runs a block on the main thread after a delay, also while a modal panel or a tracking loop runs
// (dispatch_after on the main queue waits for the default run loop mode).
@interface ReefInjectRunner : NSObject
+ (void)run:(void (^)(void))block after:(double)seconds;
@end
@implementation ReefInjectRunner
+ (void)call:(void (^)(void))block { block(); }
+ (void)run:(void (^)(void))block after:(double)seconds {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(seconds * NSEC_PER_SEC)), dispatch_get_global_queue(0, 0), ^{
        [self performSelectorOnMainThread:@selector(call:) withObject:[block copy] waitUntilDone:NO
                                    modes:@[NSRunLoopCommonModes, NSModalPanelRunLoopMode, NSEventTrackingRunLoopMode, NSDefaultRunLoopMode]];
    });
}
@end

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
    // The release is queued before the press is delivered: a control tracks the mouse inside its mouseDown: and reads the release from the queue.
    NSEvent *up = [NSEvent mouseEventWithType:NSEventTypeLeftMouseUp location:p modifierFlags:0
                                    timestamp:NSProcessInfo.processInfo.systemUptime windowNumber:w.windowNumber context:nil eventNumber:0 clickCount:1 pressure:0.0];
    NSEvent *down = [NSEvent mouseEventWithType:NSEventTypeLeftMouseDown location:p modifierFlags:0
                                      timestamp:NSProcessInfo.processInfo.systemUptime windowNumber:w.windowNumber context:nil eventNumber:0 clickCount:1 pressure:1.0];
    [NSApp postEvent:up atStart:NO];
    [NSApp sendEvent:down];
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
        [ReefInjectRunner run:^{
            fprintf(stderr, "inject: click %g,%g\n", x, y);
            ReefPostClick(x, y);
        } after:delay + interval * i];
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
        [ReefInjectRunner run:^{
            fprintf(stderr, "inject: %s\n", k.UTF8String);
            if ([k isEqualToString:@"Return"]) ReefPostKey(@"\r", 36);
            else if ([k isEqualToString:@"Escape"]) ReefPostKey(@"\x1b", 53);
            else if ([k isEqualToString:@"Delete"]) ReefPostKey(@"\x7f", 51);
            else ReefPostKey(k, 0);
        } after:delay + interval * i];
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

// Option key of NSImage's bitmap conversion (observed on macOS 26: the value is the name).
NSBitmapImageRepPropertyKey NSImageDitherTransparency = @"NSImageDitherTransparency";

// Debug aid: REEF_LOG_EXCEPTIONS=path appends the reason and the whole call stack of an uncaught exception to the file
// (the report dialog of the loader shows only the first lines).
static NSUncaughtExceptionHandler *ReefPreviousHandler;
static void ReefLogException(NSException *e) {
    FILE *f = fopen(getenv("REEF_LOG_EXCEPTIONS"), "a");
    if (f) {
        fprintf(f, "%s: %s\n", e.name.UTF8String, e.reason.UTF8String);
        for (NSString *s in e.callStackSymbols) fprintf(f, "  %s\n", s.UTF8String);
        fclose(f);
    }
    if (ReefPreviousHandler) ReefPreviousHandler(e);
}

__attribute__((constructor)) static void ReefInstallExceptionLog(void) {
    if (!getenv("REEF_LOG_EXCEPTIONS")) return;
    ReefPreviousHandler = NSGetUncaughtExceptionHandler();
    NSSetUncaughtExceptionHandler(ReefLogException);
}

// Reef's binding code applies value transformers (NSNegateBoolean ...) to the binding markers (NSNoSelectionMarker,
// NSMultipleValuesMarker, NSNotApplicableMarker); AppKit never does. Until that code skips markers, a marker reads as NO / 0.
#import <objc/runtime.h>
__attribute__((constructor)) static void ReefMarkerNumbers(void) {
    Class c = NSClassFromString(@"AKMarker");
    if (!c) return;
    class_addMethod(c, @selector(boolValue), imp_implementationWithBlock(^BOOL(id s) { return NO; }), "c@:");
    class_addMethod(c, @selector(intValue), imp_implementationWithBlock(^int(id s) { return 0; }), "i@:");
    class_addMethod(c, @selector(integerValue), imp_implementationWithBlock(^NSInteger(id s) { return 0; }), "q@:");
    class_addMethod(c, @selector(doubleValue), imp_implementationWithBlock(^double(id s) { return 0; }), "d@:");
    class_addMethod(c, @selector(floatValue), imp_implementationWithBlock(^float(id s) { return 0; }), "f@:");
    class_addMethod(c, @selector(longValue), imp_implementationWithBlock(^long(id s) { return 0; }), "q@:");
    class_addMethod(c, @selector(longLongValue), imp_implementationWithBlock(^long long(id s) { return 0; }), "q@:");
    class_addMethod(c, @selector(unsignedIntegerValue), imp_implementationWithBlock(^NSUInteger(id s) { return 0; }), "Q@:");
    class_addMethod(c, @selector(stringValue), imp_implementationWithBlock(^id(id s) { return @""; }), "@@:");
}

// NSMatrix -setToolTip:forCell: (Reef's NSMatrix lacks it): remembered per cell, not shown.
__attribute__((constructor)) static void ReefMatrixToolTips(void) {
    Class c = NSClassFromString(@"NSMatrix");
    if (!c) return;
    class_addMethod(c, @selector(setToolTip:forCell:), imp_implementationWithBlock(^(id s, NSString *tip, id cell) {
        objc_setAssociatedObject(cell, "reefToolTip", tip, OBJC_ASSOCIATION_COPY_NONATOMIC);
    }), "v@:@@");
}
