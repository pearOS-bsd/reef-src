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
