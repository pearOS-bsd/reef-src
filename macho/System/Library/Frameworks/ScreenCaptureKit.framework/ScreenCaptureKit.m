// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScreenCaptureKit: shareable content, content filters, stream configuration and screenshots; nothing can be captured,
// the screenshot calls fail with "Screen capture is not permitted."

#import <Foundation/Foundation.h>
#include <dispatch/dispatch.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
@end

@interface NSError : NSObject
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end
@interface NSArray (ScreenCaptureKit)
+ (id)array;
@end

extern NSString *const NSLocalizedDescriptionKey;

static inline CGRect CGRectMake(double x, double y, double width, double height)
{
    CGRect rect = {{x, y}, {width, height}};
    return rect;
}

typedef void (^SCErrorHandler)(NSError *error);

// Reports the error of a capture that is not allowed to the handler, later, on a global queue.
static void failLater(SCErrorHandler completion)
{
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        completion([NSError errorWithDomain:@"com.apple.ScreenCaptureKit.SCStreamErrorDomain" code:-3801 userInfo:@{NSLocalizedDescriptionKey: @"Screen capture is not permitted."}]);
        [completion release];
    });
}

@interface SCRunningApplication : NSObject
{
    int _processID;
}
@property (readonly) NSString *bundleIdentifier;
@property (readonly) NSString *applicationName;
@property (readonly) int processID;
@end

@implementation SCRunningApplication

- (NSString *)bundleIdentifier
{
    return @"";
}

- (NSString *)applicationName
{
    return @"";
}

@synthesize processID = _processID;

@end

@interface SCWindow : NSObject
{
    BOOL _onScreen;
    BOOL _active;
    unsigned int _windowID;
    NSString *_title;
    long _windowLayer;
    SCRunningApplication *_owningApplication;
    CGRect _frame;
}
@property (readonly) unsigned int windowID;
@property (readonly) CGRect frame;
@property (readonly) NSString *title;
@property (readonly) long windowLayer;
@property (readonly) SCRunningApplication *owningApplication;
@property (readonly, getter=isOnScreen) BOOL onScreen;
@property (readonly, getter=isActive) BOOL active;
@end

@implementation SCWindow
@end

@interface SCDisplay : NSObject
{
    unsigned int _displayID;
    long _width;
    long _height;
    CGRect _frame;
}
@property (readonly) unsigned int displayID;
@property (readonly) long width;
@property (readonly) long height;
@property (readonly) CGRect frame;
@end

@implementation SCDisplay
@end

@interface SCShareableContent : NSObject
@property (readonly) NSArray *windows;
@property (readonly) NSArray *displays;
@property (readonly) NSArray *applications;
+ (void)getShareableContentWithCompletionHandler:(void (^)(SCShareableContent *content, NSError *error))handler;
+ (void)getShareableContentExcludingDesktopWindows:(BOOL)excludeDesktopWindows onScreenWindowsOnly:(BOOL)onScreenOnly completionHandler:(void (^)(SCShareableContent *content, NSError *error))handler;
+ (void)getCurrentProcessShareableContentWithCompletionHandler:(void (^)(SCShareableContent *content, NSError *error))handler;
@end

@implementation SCShareableContent

+ (void)getShareableContentWithCompletionHandler:(void (^)(SCShareableContent *content, NSError *error))handler
{
    [self getShareableContentExcludingDesktopWindows:NO onScreenWindowsOnly:NO completionHandler:handler];
}

+ (void)getShareableContentExcludingDesktopWindows:(BOOL)excludeDesktopWindows onScreenWindowsOnly:(BOOL)onScreenOnly completionHandler:(void (^)(SCShareableContent *content, NSError *error))handler
{
    if (handler)
        failLater([^(NSError *error) { handler(nil, error); } copy]);
}

+ (void)getCurrentProcessShareableContentWithCompletionHandler:(void (^)(SCShareableContent *content, NSError *error))handler
{
    if (handler) {
        void (^completion)(SCShareableContent *content, NSError *error) = [handler copy];
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            SCShareableContent *content = [[SCShareableContent alloc] init];
            completion(content, nil);
            [content release];
            [completion release];
        });
    }
}

- (NSArray *)windows
{
    return [NSArray array];
}

- (NSArray *)displays
{
    return [NSArray array];
}

- (NSArray *)applications
{
    return [NSArray array];
}

@end

@interface SCContentFilter : NSObject
{
    float _pointPixelScale;
    CGRect _contentRect;
}
@property (readonly) CGRect contentRect;
@property (readonly) float pointPixelScale;
- (id)initWithDesktopIndependentWindow:(SCWindow *)window;
- (id)initWithDisplay:(SCDisplay *)display excludingWindows:(NSArray *)windows;
- (id)initWithDisplay:(SCDisplay *)display includingWindows:(NSArray *)windows;
@end

@implementation SCContentFilter

- (id)initWithDesktopIndependentWindow:(SCWindow *)window
{
    self = [super init];
    if (self) {
        CGRect *target = &_contentRect;
        *target = window ? [window frame] : CGRectMake(0, 0, 0, 0);
        _pointPixelScale = 1.0f;
    }
    return self;
}

- (id)initWithDisplay:(SCDisplay *)display excludingWindows:(NSArray *)windows
{
    self = [super init];
    if (self) {
        CGRect *target = &_contentRect;
        *target = display ? [display frame] : CGRectMake(0, 0, 0, 0);
        _pointPixelScale = 1.0f;
    }
    return self;
}

- (id)initWithDisplay:(SCDisplay *)display includingWindows:(NSArray *)windows
{
    return [self initWithDisplay:display excludingWindows:[NSArray array]];
}

@end

@interface SCStreamConfiguration : NSObject
{
    BOOL _showsCursor;
    BOOL _scalesToFit;
    BOOL _capturesAudio;
    BOOL _ignoreShadowsSingleWindow;
    BOOL _ignoreShadowsDisplay;
    BOOL _shouldBeOpaque;
    BOOL _includeChildWindows;
    unsigned int _pixelFormat;
    unsigned long _width;
    unsigned long _height;
    long _captureResolution;
    CGRect _sourceRect;
    CGRect _destinationRect;
}
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

@implementation SCStreamConfiguration

- (id)init
{
    self = [super init];
    if (self) {
        _width = 1920;
        _height = 1080;
        _showsCursor = YES;
        _pixelFormat = 0x42475241;      // 'BGRA'
    }
    return self;
}

@end

@interface SCScreenshotManager : NSObject
+ (void)captureImageWithFilter:(SCContentFilter *)filter configuration:(SCStreamConfiguration *)configuration completionHandler:(void (^)(struct CGImage *image, NSError *error))handler;
+ (void)captureSampleBufferWithFilter:(SCContentFilter *)filter configuration:(SCStreamConfiguration *)configuration completionHandler:(void (^)(void *sampleBuffer, NSError *error))handler;
+ (void)captureImageInRect:(CGRect)rect completionHandler:(void (^)(struct CGImage *image, NSError *error))handler;
@end

@implementation SCScreenshotManager

+ (void)captureImageWithFilter:(SCContentFilter *)filter configuration:(SCStreamConfiguration *)configuration completionHandler:(void (^)(struct CGImage *image, NSError *error))handler
{
    if (handler)
        failLater([^(NSError *error) { handler(NULL, error); } copy]);
}

+ (void)captureSampleBufferWithFilter:(SCContentFilter *)filter configuration:(SCStreamConfiguration *)configuration completionHandler:(void (^)(void *sampleBuffer, NSError *error))handler
{
    if (handler)
        failLater([^(NSError *error) { handler(NULL, error); } copy]);
}

+ (void)captureImageInRect:(CGRect)rect completionHandler:(void (^)(struct CGImage *image, NSError *error))handler
{
    if (handler)
        failLater([^(NSError *error) { handler(NULL, error); } copy]);
}

@end

NSString *const SCStreamErrorDomain = @"com.apple.ScreenCaptureKit.SCStreamErrorDomain";
NSString *const SCStreamFrameInfoStatus = @"SCStreamUpdateFrameStatus";
NSString *const SCStreamFrameInfoDisplayTime = @"SCStreamUpdateFrameDisplayTime";
NSString *const SCStreamFrameInfoScaleFactor = @"SCStreamUpdateFrameScaleFactor";
NSString *const SCStreamFrameInfoContentScale = @"SCStreamUpdateFrameContentScale";
NSString *const SCStreamFrameInfoContentRect = @"SCStreamUpdateFrameContentRect";
NSString *const SCStreamFrameInfoBoundingRect = @"SCStreamUpdateFrameBoundingRect";
NSString *const SCStreamFrameInfoScreenRect = @"SCStreamUpdateFrameScreenRect";
NSString *const SCStreamFrameInfoDirtyRects = @"SCStreamUpdateFrameDirtyRects";
NSString *const SCStreamFrameInfoPresenterOverlayContentRect = @"SCStreamUpdateFramePresenterOverlayContentRect";
