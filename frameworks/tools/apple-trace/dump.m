// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
#import <AppKit/AppKit.h>
static void dump(NSView *v, int d) {
    fprintf(stderr, "%*s%s frame=%s\n", d*2, "", object_getClassName(v), NSStringFromRect(v.frame).UTF8String);
    for (NSView *s in v.subviews) dump(s, d+1);
}
__attribute__((constructor)) static void init(void) {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 4 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        for (NSWindow *w in NSApp.windows) {
            fprintf(stderr, "WINDOW %s %s styleMask=%lx\n", object_getClassName(w), NSStringFromRect(w.frame).UTF8String, (unsigned long)w.styleMask);
            fprintf(stderr, " contentMin=%s contentMax=%s contentLayoutRect=%s\n", NSStringFromSize(w.contentMinSize).UTF8String, NSStringFromSize(w.contentMaxSize).UTF8String, NSStringFromRect(w.contentLayoutRect).UTF8String);
            NSView *c = w.contentView;
            fprintf(stderr, " fitting=%s intrinsic=%s\n", NSStringFromSize(c.fittingSize).UTF8String, NSStringFromSize(c.intrinsicContentSize).UTF8String);
            dump(c, 1);
        }
    });
}
