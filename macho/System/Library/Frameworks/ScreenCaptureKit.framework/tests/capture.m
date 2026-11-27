// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// ScreenCaptureKit: stream configuration defaults and accessors, content filters, shareable content and the failing screenshot calls.
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

struct CGImage;
static CGRect CGRectMake(double x, double y, double width, double height)
{
    CGRect rect = {{x, y}, {width, height}};
    return rect;
}

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSError : NSObject
- (NSString *)domain;
- (NSInteger)code;
- (NSString *)localizedDescription;
@end
@interface NSObject (T)
- (NSUInteger)count;
@end

@interface SCRunningApplication : NSObject
@property (readonly) NSString *bundleIdentifier;
@property (readonly) NSString *applicationName;
@property (readonly) int processID;
@end
@interface SCWindow : NSObject
@property (readonly) unsigned int windowID;
@property (readonly) CGRect frame;
@property (readonly) NSString *title;
@property (readonly) long windowLayer;
@property (readonly) SCRunningApplication *owningApplication;
@property (readonly, getter=isOnScreen) BOOL onScreen;
@property (readonly, getter=isActive) BOOL active;
@end
@interface SCDisplay : NSObject
@property (readonly) unsigned int displayID;
@property (readonly) long width;
@property (readonly) long height;
@property (readonly) CGRect frame;
@end
@interface SCShareableContent : NSObject
@property (readonly) NSArray *windows;
@property (readonly) NSArray *displays;
@property (readonly) NSArray *applications;
+ (void)getShareableContentWithCompletionHandler:(void (^)(SCShareableContent *content, NSError *error))handler;
+ (void)getShareableContentExcludingDesktopWindows:(BOOL)a onScreenWindowsOnly:(BOOL)b completionHandler:(void (^)(SCShareableContent *content, NSError *error))handler;
+ (void)getCurrentProcessShareableContentWithCompletionHandler:(void (^)(SCShareableContent *content, NSError *error))handler;
@end
@interface SCContentFilter : NSObject
@property (readonly) CGRect contentRect;
@property (readonly) float pointPixelScale;
- (id)initWithDesktopIndependentWindow:(SCWindow *)window;
- (id)initWithDisplay:(SCDisplay *)display excludingWindows:(NSArray *)windows;
- (id)initWithDisplay:(SCDisplay *)display includingWindows:(NSArray *)windows;
@end
@interface SCStreamConfiguration : NSObject
@property (nonatomic) unsigned long width;
@property (nonatomic) unsigned long height;
@property (nonatomic) BOOL showsCursor;
@property (nonatomic) BOOL scalesToFit;
@property (nonatomic) BOOL capturesAudio;
@property (nonatomic) unsigned int pixelFormat;
@property (nonatomic) CGRect sourceRect;
@property (nonatomic) CGRect destinationRect;
@property (nonatomic) BOOL ignoreShadowsSingleWindow;
@property (nonatomic) BOOL ignoreShadowsDisplay;
@property (nonatomic) BOOL shouldBeOpaque;
@property (nonatomic) BOOL includeChildWindows;
@property (nonatomic) long captureResolution;
@end
@interface SCScreenshotManager : NSObject
+ (void)captureImageWithFilter:(SCContentFilter *)filter configuration:(SCStreamConfiguration *)configuration completionHandler:(void (^)(struct CGImage *image, NSError *error))handler;
+ (void)captureSampleBufferWithFilter:(SCContentFilter *)filter configuration:(SCStreamConfiguration *)configuration completionHandler:(void (^)(void *sampleBuffer, NSError *error))handler;
+ (void)captureImageInRect:(CGRect)rect completionHandler:(void (^)(struct CGImage *image, NSError *error))handler;
@end
extern NSString *const SCStreamErrorDomain;
extern NSString *const SCStreamFrameInfoStatus;
extern NSString *const SCStreamFrameInfoPresenterOverlayContentRect;

extern void *class_getInstanceVariable(Class cls, const char *name);
extern long ivar_getOffset(void *ivar);

static void setFrame(id object, CGRect frame)
{
    void *ivar = class_getInstanceVariable([object class], "_frame");
    *(CGRect *)((char *)(__bridge void *)object + ivar_getOffset(ivar)) = frame;
}

static volatile int arrived;

static void rect(const char *name, CGRect r)
{
    printf("%s %g %g %g %g\n", name, r.origin.x, r.origin.y, r.size.width, r.size.height);
}

static void wait(int count)
{
    for (int i = 0; i < 100 && arrived < count; i++)
        usleep(20000);
    usleep(20000);
}

int main(void)
{
    SCStreamConfiguration *c = [SCStreamConfiguration new];
    printf("config %lu %lu %d %d %d %08x %ld\n", c.width, c.height, c.showsCursor, c.scalesToFit, c.capturesAudio, c.pixelFormat, c.captureResolution);
    rect("source", c.sourceRect);
    rect("destination", c.destinationRect);
    printf("flags %d %d %d %d\n", c.ignoreShadowsSingleWindow, c.ignoreShadowsDisplay, c.shouldBeOpaque, c.includeChildWindows);
    c.width = 640; c.height = 480; c.showsCursor = NO; c.scalesToFit = YES; c.capturesAudio = YES; c.pixelFormat = 0x34323076; c.captureResolution = 2;
    c.sourceRect = CGRectMake(1, 2, 3, 4); c.destinationRect = (CGRect){{5, 6}, {7, 8}};
    c.ignoreShadowsSingleWindow = YES; c.ignoreShadowsDisplay = YES; c.shouldBeOpaque = YES; c.includeChildWindows = YES;
    printf("config %lu %lu %d %d %d %08x %ld\n", c.width, c.height, c.showsCursor, c.scalesToFit, c.capturesAudio, c.pixelFormat, c.captureResolution);
    rect("source", c.sourceRect);
    rect("destination", c.destinationRect);
    printf("flags %d %d %d %d\n", c.ignoreShadowsSingleWindow, c.ignoreShadowsDisplay, c.shouldBeOpaque, c.includeChildWindows);

    SCWindow *w = [SCWindow new];
    printf("window %u %d %d %ld %d\n", w.windowID, w.onScreen, w.active, w.windowLayer, w.title == nil && w.owningApplication == nil);
    rect("frame", w.frame);
    SCDisplay *d = [SCDisplay new];
    printf("display %u %ld %ld\n", d.displayID, d.width, d.height);
    rect("dframe", d.frame);
    SCRunningApplication *a = [SCRunningApplication new];
    printf("application [%s] [%s] %d\n", [a.bundleIdentifier UTF8String], [a.applicationName UTF8String], a.processID);

    setFrame(w, CGRectMake(10, 20, 300, 200));
    setFrame(d, CGRectMake(0, 0, 1920, 1080));
    rect("frame", w.frame);
    rect("dframe", d.frame);
    SCContentFilter *f1 = [[SCContentFilter alloc] initWithDesktopIndependentWindow:w];
    printf("filter1 %g\n", f1.pointPixelScale);
    rect("rect1", f1.contentRect);
    SCContentFilter *f2 = [[SCContentFilter alloc] initWithDesktopIndependentWindow:nil];
    printf("filter2 %g\n", f2.pointPixelScale);
    rect("rect2", f2.contentRect);
    SCContentFilter *f3 = [[SCContentFilter alloc] initWithDisplay:d excludingWindows:@[w]];
    printf("filter3 %g\n", f3.pointPixelScale);
    rect("rect3", f3.contentRect);
    SCContentFilter *f4 = [[SCContentFilter alloc] initWithDisplay:nil includingWindows:@[w]];
    printf("filter4 %g\n", f4.pointPixelScale);
    rect("rect4", f4.contentRect);

    SCShareableContent *content = [SCShareableContent new];
    printf("content %lu %lu %lu\n", (unsigned long)[content.windows count], (unsigned long)[content.displays count], (unsigned long)[content.applications count]);
    printf("domain %s %s %s\n", [SCStreamErrorDomain UTF8String], [SCStreamFrameInfoStatus UTF8String], [SCStreamFrameInfoPresenterOverlayContentRect UTF8String]);

    [SCShareableContent getShareableContentWithCompletionHandler:^(SCShareableContent *s, NSError *e) {
        printf("shareable %d %s %ld %s\n", s == nil, [e.domain UTF8String], (long)e.code, [e.localizedDescription UTF8String]);
        arrived++;
    }];
    wait(1);
    [SCShareableContent getShareableContentExcludingDesktopWindows:YES onScreenWindowsOnly:YES completionHandler:^(SCShareableContent *s, NSError *e) {
        printf("shareable2 %d %ld\n", s == nil, (long)e.code);
        arrived++;
    }];
    wait(2);
    [SCShareableContent getCurrentProcessShareableContentWithCompletionHandler:^(SCShareableContent *s, NSError *e) {
        printf("current %d %d %lu\n", s != nil, e == nil, (unsigned long)[s.windows count]);
        arrived++;
    }];
    wait(3);
    [SCScreenshotManager captureImageWithFilter:f1 configuration:c completionHandler:^(struct CGImage *i, NSError *e) {
        printf("image %d %ld %s\n", i == NULL, (long)e.code, [e.localizedDescription UTF8String]);
        arrived++;
    }];
    wait(4);
    [SCScreenshotManager captureSampleBufferWithFilter:f1 configuration:c completionHandler:^(void *b, NSError *e) {
        printf("sample %d %ld\n", b == NULL, (long)e.code);
        arrived++;
    }];
    wait(5);
    [SCScreenshotManager captureImageInRect:CGRectMake(0, 0, 10, 10) completionHandler:^(struct CGImage *i, NSError *e) {
        printf("rect %d %s\n", i == NULL, [e.domain UTF8String]);
        arrived++;
    }];
    wait(6);
    [SCShareableContent getShareableContentWithCompletionHandler:nil];
    [SCScreenshotManager captureImageWithFilter:f1 configuration:c completionHandler:nil];
    [SCScreenshotManager captureImageInRect:CGRectMake(0, 0, 10, 10) completionHandler:nil];
    usleep(100000);
    printf("arrived %d\n", arrived);
    return 0;
}
