// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// AVKit's player view over the playback engine of this tree's AVFoundation wrapper. Reef's AVKit has an AVPlayerView that
// only keeps its properties; here it shows the video of its player (a view drawing the pictures the player layer gets)
// under floating playback controls (play/pause, elapsed and remaining time, a scrubber), as macOS's floating controls
// style does. AVPlayerController is the private model that AVPlayerView and QuickTime Player share (the signatures of
// macOS 26's AVKit: times and rates are doubles), AVPlayerControllerTimeResolver publishes its time.

#import <AppKit/AppKit.h>
#import <AVFoundation/AVFoundation.h>
#import <AVKit/AVKit.h>
#import <QuartzCore/QuartzCore.h>
#import <objc/runtime.h>
#import <objc/message.h>

static double Sec(CMTime t)
{
    return CMTIME_IS_NUMERIC(t) ? CMTimeGetSeconds(t) : 0;
}

static NSString *TimeString(double seconds)
{
    if (isnan(seconds) || seconds < 0)
        seconds = 0;
    long s = lround(floor(seconds));
    return s >= 3600 ? [NSString stringWithFormat:@"%ld:%02ld:%02ld", s / 3600, s / 60 % 60, s % 60]
                     : [NSString stringWithFormat:@"%ld:%02ld", s / 60, s % 60];
}

#pragma mark - AVPlayerController

@interface AVPlayerController : NSObject
- (instancetype)initWithPlayer:(AVPlayer *)player;
@property (nonatomic, strong) AVPlayer *player;
@end

@interface AVPlayerItem (ReefPrivate)
- (BOOL)hasVideo;
- (BOOL)hasEnabledVideo;
- (BOOL)hasEnabledAudio;
@end

static void *kControllerContext = &kControllerContext;

@implementation AVPlayerController {
    AVPlayerItem *_item;
    id _timeObserver;
    id _endObserver;
    BOOL _looping, _scrubbing, _allowsPictureInPicture;
    float _rateBeforeScrubbing;
}

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)key
{
    return NO;
}

+ (NSArray *)derivedKeys
{
    static NSArray *keys;
    if (!keys)
        keys = @[@"playing", @"rate", @"activeRate", @"timeControlStatus", @"status", @"readyToPlay", @"contentDimensions",
                 @"presentationSize", @"contentDuration", @"contentDurationWithinEndTimes", @"maxTime", @"minTime", @"hasEnabledVideo",
                 @"hasVideo", @"hasEnabledAudio", @"canPlay", @"canPause", @"canTogglePlayback", @"canSeek", @"currentTime",
                 @"currentTimeWithinEndTimes", @"error", @"nominalFrameRate", @"isCurrentItemReadyForInspection",
                 @"currentAssetIfReady", @"atMaxTime", @"atMinTime", @"seekableTimeRanges", @"loadedTimeRanges", @"muted", @"volume"];
    return keys;
}

- (void)changed:(NSArray *)keys
{
    for (NSString *k in keys)
        [self willChangeValueForKey:k];
    for (NSString *k in keys.reverseObjectEnumerator)
        [self didChangeValueForKey:k];
}

- (instancetype)init
{
    return [self initWithPlayer:nil];
}

- (instancetype)initWithPlayer:(AVPlayer *)player
{
    if ((self = [super init]))
        self.player = player;
    return self;
}

- (void)dealloc
{
    [self unobservePlayer];
}

- (void)observeItem:(AVPlayerItem *)item
{
    if (_item == item)
        return;
    if (_item) {
        for (NSString *k in @[@"status", @"presentationSize", @"duration", @"tracks", @"hasVideo"])
            [_item removeObserver:self forKeyPath:k context:kControllerContext];
        [[NSNotificationCenter defaultCenter] removeObserver:_endObserver];
        _endObserver = nil;
    }
    _item = item;
    if (_item) {
        for (NSString *k in @[@"status", @"presentationSize", @"duration", @"tracks", @"hasVideo"])
            [_item addObserver:self forKeyPath:k options:0 context:kControllerContext];
        __weak AVPlayerController *weak = self;
        _endObserver = [[NSNotificationCenter defaultCenter] addObserverForName:AVPlayerItemDidPlayToEndTimeNotification object:_item
                                                                          queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification *n) {
            AVPlayerController *me = weak;
            if (me && me->_looping) {
                [me.player seekToTime:kCMTimeZero];
                [me.player play];
            }
        }];
    }
}

- (void)unobservePlayer
{
    if (!_player)
        return;
    [self observeItem:nil];
    for (NSString *k in @[@"rate", @"currentItem", @"volume", @"muted"])
        [_player removeObserver:self forKeyPath:k context:kControllerContext];
    if (_timeObserver)
        [_player removeTimeObserver:_timeObserver];
    _timeObserver = nil;
}

- (void)setPlayer:(AVPlayer *)player
{
    if (_player == player)
        return;
    [self unobservePlayer];
    [self willChangeValueForKey:@"player"];
    [self willChangeValueForKey:@"activePlayer"];
    _player = player;
    [self didChangeValueForKey:@"activePlayer"];
    [self didChangeValueForKey:@"player"];
    if (_player) {
        for (NSString *k in @[@"rate", @"currentItem", @"volume", @"muted"])
            [_player addObserver:self forKeyPath:k options:0 context:kControllerContext];
        [self observeItem:_player.currentItem];
        __weak AVPlayerController *weak = self;
        _timeObserver = [_player addPeriodicTimeObserverForInterval:CMTimeMake(1, 4) queue:dispatch_get_main_queue() usingBlock:^(CMTime t) {
            [weak changed:@[@"currentTime", @"currentTimeWithinEndTimes", @"atMaxTime", @"atMinTime"]];
        }];
    }
    [self changed:[AVPlayerController derivedKeys]];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (context != kControllerContext) {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
        return;
    }
    if (object == _player && [keyPath isEqualToString:@"currentItem"])
        [self observeItem:_player.currentItem];
    [self changed:[AVPlayerController derivedKeys]];
}

- (AVPlayer *)activePlayer { return _player; }
- (AVPlayerItem *)currentItem { return _player.currentItem; }
- (BOOL)isReadyToPlay { return _player.currentItem.status == AVPlayerItemStatusReadyToPlay; }
- (BOOL)readyToPlay { return self.isReadyToPlay; }
- (BOOL)isCurrentItemReadyForInspection { return self.isReadyToPlay; }
- (AVAsset *)currentAssetIfReady { return self.isReadyToPlay ? _player.currentItem.asset : nil; }
- (NSInteger)status { return _player.currentItem ? _player.currentItem.status : 0; }
- (NSError *)error { return _player.currentItem.error ?: _player.error; }

- (BOOL)isPlaying { return _player.rate != 0; }
- (BOOL)playing { return self.isPlaying; }
- (void)setPlaying:(BOOL)playing { if (playing) [_player play]; else [_player pause]; }
- (double)rate { return _player.rate; }
- (double)activeRate { return _player.rate; }
- (void)setRate:(double)rate { _player.rate = rate; }
- (NSInteger)timeControlStatus { return _player.timeControlStatus; }
- (void)play:(id)sender { if (self.isAtMaxTime) [_player seekToTime:kCMTimeZero]; [_player play]; }
- (void)pause:(id)sender { [_player pause]; }
- (void)togglePlayback:(id)sender { if (self.isPlaying) [self pause:sender]; else [self play:sender]; }
- (void)togglePlaybackEvenWhenInBackground:(id)sender { [self togglePlayback:sender]; }
- (void)autoplay:(id)sender { [self play:sender]; }
- (void)playPauseButtonWasPressed:(id)sender { [self togglePlayback:sender]; }
- (BOOL)canPlay { return self.isReadyToPlay; }
- (BOOL)canPause { return self.isReadyToPlay; }
- (BOOL)canTogglePlayback { return self.isReadyToPlay; }
- (BOOL)canSeek { return self.isReadyToPlay; }
- (BOOL)canScanForward { return NO; }
- (BOOL)canScanBackward { return NO; }
- (void)scanForward:(id)sender {}
- (void)scanBackward:(id)sender {}
- (BOOL)isSeeking { return NO; }
- (BOOL)isScrubbing { return _scrubbing; }

- (double)currentTime { return Sec(_player.currentTime); }
- (double)currentTimeWithinEndTimes { return self.currentTime; }
- (double)contentDuration { return self.isReadyToPlay ? Sec(_player.currentItem.duration) : 0; }
- (double)contentDurationWithinEndTimes { return self.contentDuration; }
- (double)minTime { return 0; }
- (double)maxTime { return self.contentDuration; }
- (BOOL)isAtMaxTime { double d = self.contentDuration; return d > 0 && self.currentTime >= d - 0.01; }
- (BOOL)isAtMinTime { return self.currentTime <= 0.01; }
- (NSArray *)seekableTimeRanges { return _player.currentItem.seekableTimeRanges ?: @[]; }
- (NSArray *)loadedTimeRanges { return _player.currentItem.loadedTimeRanges ?: @[]; }

- (void)seekToTime:(double)time
{
    [_player seekToTime:CMTimeMakeWithSeconds(time, 600) toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero];
    [self changed:@[@"currentTime", @"currentTimeWithinEndTimes", @"atMaxTime", @"atMinTime"]];
}
- (void)seekToTime:(double)time toleranceBefore:(double)before toleranceAfter:(double)after { [self seekToTime:time]; }
- (void)seekFrameForward:(id)sender { [_player pause]; [_player.currentItem stepByCount:1]; }
- (void)seekFrameBackward:(id)sender { [_player pause]; [_player.currentItem stepByCount:-1]; }
- (void)stepForward:(id)sender { [self seekFrameForward:sender]; }
- (void)stepBackward:(id)sender { [self seekFrameBackward:sender]; }
- (BOOL)canStepForward { return self.isReadyToPlay; }
- (BOOL)canStepBackward { return self.isReadyToPlay; }
- (BOOL)canSeekChapterForward { return NO; }
- (BOOL)canSeekChapterBackward { return NO; }
- (void)seekChapterForward:(id)sender {}
- (void)seekChapterBackward:(id)sender {}
- (void)beginScrubbing:(id)sender { _scrubbing = YES; _rateBeforeScrubbing = _player.rate; [_player pause]; }
- (void)endScrubbing:(id)sender { _scrubbing = NO; if (_rateBeforeScrubbing != 0) _player.rate = _rateBeforeScrubbing; }
- (void)gotoEndOfSeekableRanges:(id)sender { [self seekToTime:self.contentDuration]; }

- (CGSize)contentDimensions { return self.isReadyToPlay ? _player.currentItem.presentationSize : CGSizeZero; }
- (CGSize)presentationSize { return self.contentDimensions; }
- (BOOL)hasVideo { AVPlayerItem *i = _player.currentItem; return self.isReadyToPlay && [i respondsToSelector:@selector(hasVideo)] && [i hasVideo]; }
- (BOOL)hasEnabledVideo { return self.hasVideo; }
- (BOOL)hasEnabledAudio { AVPlayerItem *i = _player.currentItem; return self.isReadyToPlay && [i respondsToSelector:@selector(hasEnabledAudio)] && [i hasEnabledAudio]; }
- (float)nominalFrameRate { return [_player.currentItem.asset tracksWithMediaType:AVMediaTypeVideo].firstObject.nominalFrameRate; }
- (CGAffineTransform)preferredTransform { return CGAffineTransformIdentity; }
- (NSDictionary *)metadata { return @{}; }
- (NSArray *)availableMetadataFormats { return @[]; }
- (NSArray *)contentChapters { return @[]; }
- (BOOL)hasProtectedContent { return NO; }
- (BOOL)hasShareableContent { return NO; }
- (BOOL)hasStereoVideo { return NO; }
- (BOOL)isComposable { return YES; }
- (BOOL)isCompatibleWithAirPlayVideo { return NO; }
- (BOOL)isExternalPlaybackActive { return NO; }
- (BOOL)isPlayingOnExternalScreen { return NO; }
- (BOOL)isPlayingOnSecondScreen { return NO; }
- (BOOL)hasReadableTimecodes { return NO; }
- (id)timecodeController { return nil; }
- (CMTime)forwardPlaybackEndTime { return _player.currentItem ? _player.currentItem.forwardPlaybackEndTime : kCMTimeInvalid; }
- (void)setForwardPlaybackEndTime:(CMTime)t { _player.currentItem.forwardPlaybackEndTime = t; }
- (CMTime)reversePlaybackEndTime { return _player.currentItem ? _player.currentItem.reversePlaybackEndTime : kCMTimeInvalid; }
- (void)setReversePlaybackEndTime:(CMTime)t { _player.currentItem.reversePlaybackEndTime = t; }
- (BOOL)hasAudioMediaSelectionOptions { return NO; }
- (BOOL)hasLegibleMediaSelectionOptions { return NO; }
- (NSArray *)audioMediaSelectionOptions { return @[]; }
- (NSArray *)legibleMediaSelectionOptions { return @[]; }
- (id)currentAudioMediaSelectionOption { return nil; }
- (id)currentLegibleMediaSelectionOption { return nil; }
- (void)setCurrentAudioMediaSelectionOption:(id)option {}
- (void)setCurrentLegibleMediaSelectionOption:(id)option {}
- (BOOL)allowsPictureInPicturePlayback { return _allowsPictureInPicture; }
- (void)setAllowsPictureInPicturePlayback:(BOOL)b { _allowsPictureInPicture = b; }
- (BOOL)isPictureInPictureSupported { return NO; }
- (BOOL)isLooping { return _looping; }
- (void)setLooping:(BOOL)looping { [self willChangeValueForKey:@"looping"]; _looping = looping; [self didChangeValueForKey:@"looping"]; }
- (double)volume { return _player.volume; }
- (void)setVolume:(double)volume { _player.volume = volume; }
- (BOOL)isMuted { return _player.muted; }
- (void)setMuted:(BOOL)muted { _player.muted = muted; }
- (BOOL)handlesAudioSessionInterruptions { return NO; }
- (void)setHandlesAudioSessionInterruptions:(BOOL)b {}
- (CGSize)maximumVideoResolution { return CGSizeZero; }
- (NSDate *)currentDate { return nil; }

@end

#pragma mark - AVPlayerControllerTimeResolver

@interface AVPlayerControllerTimeResolver : NSObject
@property (nonatomic, strong) AVPlayerController *playerController;
@property (nonatomic) double currentTime;
@property (nonatomic) double currentTimeWithinEndTimes;
@property (nonatomic) double resolution;
@end

static void *kResolverContext = &kResolverContext;

@implementation AVPlayerControllerTimeResolver

+ (BOOL)automaticallyNotifiesObserversOfCurrentTime
{
    return NO;
}

- (void)setPlayerController:(AVPlayerController *)controller
{
    [_playerController removeObserver:self forKeyPath:@"currentTime" context:kResolverContext];
    _playerController = controller;
    [_playerController addObserver:self forKeyPath:@"currentTime" options:0 context:kResolverContext];
    [self update];
}

- (void)dealloc
{
    [_playerController removeObserver:self forKeyPath:@"currentTime" context:kResolverContext];
}

- (void)update
{
    [self willChangeValueForKey:@"currentTime"];
    [self willChangeValueForKey:@"currentTimeWithinEndTimes"];
    _currentTime = _playerController.currentTime;
    _currentTimeWithinEndTimes = _currentTime;
    [self didChangeValueForKey:@"currentTimeWithinEndTimes"];
    [self didChangeValueForKey:@"currentTime"];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (context == kResolverContext)
        [self update];
    else
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

@end

#pragma mark - AVPlayerControlsViewController

@interface AVPlayerControlsViewController : NSViewController
@property (nonatomic, strong) AVPlayerController *playerController;
@property (nonatomic, strong) NSButton *shareButton;
@end

@implementation AVPlayerControlsViewController
- (id)shareController { return nil; }
- (void)loadView { self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 480, 44)]; }
- (void)controlTextDidBeginEditing:(NSNotification *)n {}
- (void)controlTextDidEndEditing:(NSNotification *)n {}
@end

#pragma mark - asset identifiers

@interface AVAssetIdentifier : NSObject <NSCopying>
@property (nonatomic, copy) NSURL *URL;
@end

@implementation AVAssetIdentifier
+ (instancetype)identifierForURL:(NSURL *)url
{
    AVAssetIdentifier *identifier = [self new];
    identifier.URL = url;
    return identifier;
}
+ (instancetype)identifierForAsset:(AVAsset *)asset
{
    return [asset isKindOfClass:[AVURLAsset class]] ? [self identifierForURL:[(AVURLAsset *)asset URL]] : [self new];
}
- (id)copyWithZone:(NSZone *)zone { return self; }
- (BOOL)isValid { return self.URL != nil; }
- (BOOL)matchesAsset:(AVAsset *)asset
{
    return [asset isKindOfClass:[AVURLAsset class]] && [[(AVURLAsset *)asset URL].URLByStandardizingPath isEqual:_URL.URLByStandardizingPath];
}
- (BOOL)isEqual:(id)other { return [other isKindOfClass:[AVAssetIdentifier class]] && [((AVAssetIdentifier *)other).URL isEqual:_URL]; }
- (NSUInteger)hash { return _URL.hash; }
@end

#pragma mark - the player view's parts

// Draws the picture its player layer got last (the player layer itself stays out of the view's layer tree).
@interface ReefVideoView : NSView
@property (nonatomic, strong) AVPlayerLayer *playerLayer;
@end

@implementation ReefVideoView
- (BOOL)isOpaque { return YES; }
- (NSView *)hitTest:(NSPoint)point { return nil; }
- (void)reef_playerLayerDidChangeContents:(CALayer *)layer { [self setNeedsDisplay:YES]; }

// The player layer has the size of the picture on screen, so that the engine decodes at that size.
- (void)updateLayerSize
{
    CGSize size = self.playerLayer.player.currentItem.presentationSize;
    NSRect bounds = self.bounds;
    if (size.width > 0 && size.height > 0) {
        double scale = MIN(bounds.size.width / size.width, bounds.size.height / size.height);
        size = CGSizeMake(size.width * scale, size.height * scale);
    } else {
        size = bounds.size;
    }
    [CATransaction begin];
    [CATransaction setDisableActions:YES];
    self.playerLayer.bounds = CGRectMake(0, 0, size.width, size.height);
    self.playerLayer.contentsScale = self.window ? self.window.backingScaleFactor : 1;
    [CATransaction commit];
}

- (void)setFrameSize:(NSSize)size
{
    [super setFrameSize:size];
    [self updateLayerSize];
}

- (NSRect)videoRect
{
    CGImageRef image = (__bridge CGImageRef)self.playerLayer.contents;
    NSRect bounds = self.bounds;
    if (!image)
        return NSZeroRect;
    CGSize size = self.playerLayer.player.currentItem.presentationSize;
    if (size.width <= 0 || size.height <= 0)
        size = CGSizeMake(CGImageGetWidth(image), CGImageGetHeight(image));
    NSString *gravity = self.playerLayer.videoGravity;
    if ([gravity isEqualToString:AVLayerVideoGravityResize])
        return bounds;
    double scale = [gravity isEqualToString:AVLayerVideoGravityResizeAspectFill]
                       ? MAX(bounds.size.width / size.width, bounds.size.height / size.height)
                       : MIN(bounds.size.width / size.width, bounds.size.height / size.height);
    NSSize fit = NSMakeSize(size.width * scale, size.height * scale);
    return NSMakeRect(NSMidX(bounds) - fit.width / 2, NSMidY(bounds) - fit.height / 2, fit.width, fit.height);
}

- (void)drawRect:(NSRect)dirty
{
    [[NSColor blackColor] setFill];
    NSRectFill(dirty);
    CGImageRef image = (__bridge CGImageRef)self.playerLayer.contents;
    if (!image)
        return;
    if (!CGSizeEqualToSize(self.playerLayer.bounds.size, [self videoRect].size))
        [self updateLayerSize];
    CGContextRef context = [NSGraphicsContext currentContext].CGContext;
    CGContextSaveGState(context);
    CGContextSetInterpolationQuality(context, kCGInterpolationMedium);
    NSRect rect = [self videoRect];
    if (self.isFlipped) {
        CGContextTranslateCTM(context, 0, NSMaxY(rect) + NSMinY(rect));
        CGContextScaleCTM(context, 1, -1);
    }
    CGContextDrawImage(context, rect, image);
    CGContextRestoreGState(context);
}
@end

// A round button drawing a play triangle or pause bars.
@interface ReefGlyphButton : NSView
@property (nonatomic) BOOL showsPause;
@property (nonatomic, weak) id target;
@property (nonatomic) SEL action;
@end

@implementation ReefGlyphButton
- (BOOL)acceptsFirstMouse:(NSEvent *)event { return YES; }
- (void)mouseDown:(NSEvent *)event
{
    if (self.target && self.action)
        ((void (*)(id, SEL, id))objc_msgSend)(self.target, self.action, self);
}
- (void)setShowsPause:(BOOL)showsPause
{
    if (_showsPause != showsPause) {
        _showsPause = showsPause;
        [self setNeedsDisplay:YES];
    }
}
- (void)drawRect:(NSRect)dirty
{
    NSRect b = NSInsetRect(self.bounds, 7, 7);
    [[NSColor colorWithWhite:1 alpha:0.95] setFill];
    if (self.showsPause) {
        double w = b.size.width * 0.3;
        NSRectFill(NSMakeRect(NSMinX(b) + b.size.width * 0.12, NSMinY(b), w, b.size.height));
        NSRectFill(NSMakeRect(NSMaxX(b) - b.size.width * 0.12 - w, NSMinY(b), w, b.size.height));
    } else {
        NSBezierPath *p = [NSBezierPath bezierPath];
        [p moveToPoint:NSMakePoint(NSMinX(b) + b.size.width * 0.15, NSMinY(b))];
        [p lineToPoint:NSMakePoint(NSMinX(b) + b.size.width * 0.15, NSMaxY(b))];
        [p lineToPoint:NSMakePoint(NSMaxX(b), NSMidY(b))];
        [p closePath];
        [p fill];
    }
}
@end

@class ReefPlayerControlsView;
@interface ReefPlayerViewState : NSObject {
@public
    __weak AVPlayerView *view;
    AVPlayer *player;
    AVPlayerController *controller;
    ReefVideoView *videoView;
    AVPlayerLayer *playerLayer;
    ReefPlayerControlsView *controls;
    id timeObserver;
    NSEdgeInsets margin;
    NSInteger layoutMode, controlsStyle, stereoEyes;
    NSUInteger timeDisplayStyle;
    __weak id slowMotionDelegate, delegate;
    id shareController;
    CMTimeRange slowMotionRange;
    double magnification;
    BOOL canHideControls, zoomed, showsTimecodes, allowsMagnification, enableStereo, showsAlternatePreview;
    NSTrackingArea *tracking;
    NSTimer *hideTimer;
    BOOL setUp;
    NSString *gravity;
}
@end
@implementation ReefPlayerViewState
@end

@interface ReefShareController : NSObject
@property (nonatomic, weak) id delegate;
@end
@implementation ReefShareController
@end

static char kViewState;
static void *kViewContext = &kViewContext;

static ReefPlayerViewState *ViewState(AVPlayerView *view)
{
    ReefPlayerViewState *state = objc_getAssociatedObject(view, &kViewState);
    if (!state) {
        state = [ReefPlayerViewState new];
        state->view = view;
        state->controlsStyle = AVPlayerViewControlsStyleInline;
        state->canHideControls = YES;
        state->magnification = 1;
        state->slowMotionRange = kCMTimeRangeInvalid;
        state->gravity = AVLayerVideoGravityResizeAspect;
        state->shareController = [ReefShareController new];
        objc_setAssociatedObject(view, &kViewState, state, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return state;
}

// The floating bar: play/pause, elapsed time, scrubber, remaining time.
@interface ReefPlayerControlsView : NSView
@property (nonatomic, weak) AVPlayerView *playerView;
@property (nonatomic, weak) AVPlayer *player;
- (void)update;
- (void)togglePlayback:(id)sender;
@end

@implementation ReefPlayerControlsView {
    ReefGlyphButton *_play;
    NSTextField *_elapsed, *_remaining;
    NSSlider *_scrubber;
}

static NSTextField *Label(NSRect frame)
{
    NSTextField *label = [[NSTextField alloc] initWithFrame:frame];
    label.bezeled = NO;
    label.drawsBackground = NO;
    label.editable = NO;
    label.selectable = NO;
    label.textColor = [NSColor colorWithWhite:1 alpha:0.9];
    label.font = [NSFont monospacedDigitSystemFontOfSize:12 weight:NSFontWeightMedium];
    label.stringValue = @"0:00";
    return label;
}

- (instancetype)initWithFrame:(NSRect)frame
{
    if ((self = [super initWithFrame:frame])) {
        double w = frame.size.width;
        _play = [[ReefGlyphButton alloc] initWithFrame:NSMakeRect(10, 6, 32, 32)];
        _play.target = self;
        _play.action = @selector(togglePlayback:);
        _elapsed = Label(NSMakeRect(48, 13, 52, 18));
        _elapsed.alignment = NSTextAlignmentRight;
        _remaining = Label(NSMakeRect(w - 62, 13, 56, 18));
        _remaining.autoresizingMask = NSViewMinXMargin;
        _scrubber = [[NSSlider alloc] initWithFrame:NSMakeRect(106, 12, w - 106 - 68, 20)];
        _scrubber.autoresizingMask = NSViewWidthSizable;
        _scrubber.minValue = 0;
        _scrubber.maxValue = 1;
        _scrubber.continuous = YES;
        _scrubber.target = self;
        _scrubber.action = @selector(scrub:);
        for (NSView *v in @[_play, _elapsed, _scrubber, _remaining])
            [self addSubview:v];
    }
    return self;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event { return YES; }

- (void)drawRect:(NSRect)dirty
{
    [[NSColor colorWithWhite:0.12 alpha:0.78] setFill];
    [[NSBezierPath bezierPathWithRoundedRect:self.bounds xRadius:12 yRadius:12] fill];
}

- (void)togglePlayback:(id)sender
{
    AVPlayer *player = self.player;
    if (player.rate != 0) {
        [player pause];
    } else {
        double d = Sec(player.currentItem.duration);
        if (d > 0 && Sec(player.currentTime) >= d - 0.05)
            [player seekToTime:kCMTimeZero];
        [player play];
    }
    [self update];
}

- (void)scrub:(id)sender
{
    AVPlayer *player = self.player;
    [player seekToTime:CMTimeMakeWithSeconds(_scrubber.doubleValue, 600) toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero];
    [self update];
}

- (void)update
{
    AVPlayer *player = self.player;
    double duration = Sec(player.currentItem.duration), now = Sec(player.currentTime);
    if (isnan(duration) || duration < 0)
        duration = 0;
    _play.showsPause = player.rate != 0;
    _elapsed.stringValue = TimeString(now);
    _remaining.stringValue = [@"-" stringByAppendingString:TimeString(duration - now)];
    _scrubber.maxValue = duration > 0 ? duration : 1;
    _scrubber.doubleValue = now;
}
@end

#pragma mark - AVPlayerView

static void LayOut(AVPlayerView *view)
{
    ReefPlayerViewState *s = ViewState(view);
    if (!s->setUp)
        return;
    NSRect b = view.bounds;
    s->videoView.frame = b;
    double width = MIN(b.size.width - 40, 520);
    if (width < 200)
        width = MAX(b.size.width - 8, 120);
    double bottom = s->margin.bottom > 0 ? s->margin.bottom : 18;
    s->controls.frame = NSMakeRect(round(NSMidX(b) - width / 2), NSMinY(b) + bottom, width, 44);
    s->controls.hidden = s->controlsStyle == AVPlayerViewControlsStyleNone || b.size.height < 80;
}

static void ShowControls(AVPlayerView *view, BOOL show)
{
    ReefPlayerViewState *s = ViewState(view);
    [s->hideTimer invalidate];
    s->hideTimer = nil;
    BOOL playing = s->player.rate != 0;
    s->controls.alphaValue = show || !playing || !s->canHideControls ? 1 : 0;
    if (show && playing && s->canHideControls) {
        __weak AVPlayerView *weak = view;
        s->hideTimer = [NSTimer scheduledTimerWithTimeInterval:3 repeats:NO block:^(NSTimer *t) {
            AVPlayerView *v = weak;
            if (v)
                ShowControls(v, NO);
        }];
    }
}

static void SetUpView(AVPlayerView *view)
{
    ReefPlayerViewState *s = ViewState(view);
    if (s->setUp)
        return;
    s->setUp = YES;
    s->playerLayer = [AVPlayerLayer layer];
    s->playerLayer.videoGravity = s->gravity;
    s->videoView = [[ReefVideoView alloc] initWithFrame:view.bounds];
    s->videoView.playerLayer = s->playerLayer;
    s->playerLayer.delegate = (id)s->videoView;
    s->videoView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [view addSubview:s->videoView positioned:NSWindowBelow relativeTo:nil];
    s->controls = [[ReefPlayerControlsView alloc] initWithFrame:NSMakeRect(0, 0, 480, 44)];
    s->controls.playerView = view;
    s->controls.autoresizingMask = NSViewMinXMargin | NSViewMaxXMargin | NSViewMaxYMargin;
    [view addSubview:s->controls];
    LayOut(view);
    if (s->player) {
        s->playerLayer.player = s->player;
        s->controls.player = s->player;
    }
}

static IMP originalInitWithFrame, originalInitWithCoder;

static id PlayerViewInitWithFrame(id self, SEL _cmd, NSRect frame)
{
    self = ((id (*)(id, SEL, NSRect))originalInitWithFrame)(self, _cmd, frame);
    if (self)
        SetUpView(self);
    return self;
}

static id PlayerViewInitWithCoder(id self, SEL _cmd, NSCoder *coder)
{
    self = ((id (*)(id, SEL, NSCoder *))originalInitWithCoder)(self, _cmd, coder);
    if (self)
        SetUpView(self);
    return self;
}

__attribute__((constructor)) static void ReefInstallPlayerView(void)
{
    Class c = objc_getClass("AVPlayerView");
    Method m = class_getInstanceMethod(c, @selector(initWithFrame:));
    originalInitWithFrame = method_setImplementation(m, (IMP)PlayerViewInitWithFrame);
    m = class_getInstanceMethod(c, @selector(initWithCoder:));
    originalInitWithCoder = method_setImplementation(m, (IMP)PlayerViewInitWithCoder);
}

@implementation AVPlayerView (ReefPlayback)

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)key
{
    static NSSet *manual;
    if (!manual)
        manual = [NSSet setWithArray:@[@"player", @"playerController", @"playerLayerFrame", @"readyForDisplay", @"timeDisplayStyle",
                                       @"availableTimeDisplayStyles", @"allowedTimeDisplayStyles", @"slowMotionTimeRange",
                                       @"prefersUnobscuredContent", @"videoBounds"]];
    return [manual containsObject:key] ? NO : [super automaticallyNotifiesObserversForKey:key];
}

- (AVPlayer *)player { return ViewState(self)->player; }

- (void)setPlayer:(AVPlayer *)player
{
    ReefPlayerViewState *s = ViewState(self);
    if (s->player == player)
        return;
    if (s->player) {
        [s->player removeObserver:self forKeyPath:@"rate" context:kViewContext];
        [s->player removeObserver:self forKeyPath:@"currentItem.status" context:kViewContext];
        if (s->timeObserver)
            [s->player removeTimeObserver:s->timeObserver];
        s->timeObserver = nil;
    }
    [self willChangeValueForKey:@"player"];
    [self willChangeValueForKey:@"readyForDisplay"];
    s->player = player;
    [self didChangeValueForKey:@"readyForDisplay"];
    [self didChangeValueForKey:@"player"];
    if (!s->controller || s->controller.player != player) {
        [self willChangeValueForKey:@"playerController"];
        s->controller = player ? [[AVPlayerController alloc] initWithPlayer:player] : nil;
        [self didChangeValueForKey:@"playerController"];
    }
    s->playerLayer.player = player;
    s->controls.player = player;
    if (player) {
        [player addObserver:self forKeyPath:@"rate" options:0 context:kViewContext];
        [player addObserver:self forKeyPath:@"currentItem.status" options:0 context:kViewContext];
        __weak AVPlayerView *weak = self;
        s->timeObserver = [player addPeriodicTimeObserverForInterval:CMTimeMake(1, 10) queue:dispatch_get_main_queue() usingBlock:^(CMTime t) {
            AVPlayerView *v = weak;
            if (v)
                [ViewState(v)->controls update];
        }];
    }
    [s->controls update];
    ShowControls(self, YES);
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (context != kViewContext) {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
        return;
    }
    ReefPlayerViewState *s = ViewState(self);
    [s->controls update];
    if ([keyPath isEqualToString:@"rate"])
        ShowControls(self, NO);
    [self willChangeValueForKey:@"playerLayerFrame"];
    [self didChangeValueForKey:@"playerLayerFrame"];
}

- (id)playerController { return ViewState(self)->controller; }

- (void)setPlayerController:(id)controller
{
    ReefPlayerViewState *s = ViewState(self);
    if (s->controller == controller)
        return;
    [self willChangeValueForKey:@"playerController"];
    s->controller = controller;
    [self didChangeValueForKey:@"playerController"];
    [self setPlayer:[controller player]];
}

- (NSInteger)controlsStyle { return ViewState(self)->controlsStyle; }
- (void)setControlsStyle:(NSInteger)style { ViewState(self)->controlsStyle = style; LayOut(self); }
- (NSString *)videoGravity { return ViewState(self)->gravity; }
- (void)setVideoGravity:(NSString *)gravity
{
    ReefPlayerViewState *s = ViewState(self);
    s->gravity = [gravity copy] ?: AVLayerVideoGravityResizeAspect;
    s->playerLayer.videoGravity = s->gravity;
    [s->videoView setNeedsDisplay:YES];
}
- (BOOL)isReadyForDisplay { return ViewState(self)->playerLayer.isReadyForDisplay; }
- (NSRect)videoBounds { ReefPlayerViewState *s = ViewState(self); return s->videoView ? [s->videoView videoRect] : NSZeroRect; }
- (NSView *)contentOverlayView { return nil; }
- (NSView *)contentView { return ViewState(self)->videoView; }

- (NSEdgeInsets)floatingPlaybackControlsMargin { return ViewState(self)->margin; }
- (void)setFloatingPlaybackControlsMargin:(NSEdgeInsets)margin { ViewState(self)->margin = margin; LayOut(self); }
- (NSInteger)playerLayerLayoutMode { return ViewState(self)->layoutMode; }
- (void)setPlayerLayerLayoutMode:(NSInteger)mode { ViewState(self)->layoutMode = mode; }
- (NSRect)playerLayerFrame { return self.bounds; }
- (NSRect)playerLayerFrameForBoundsSize:(NSSize)size playerLayerLayoutMode:(NSInteger)mode { return NSMakeRect(0, 0, size.width, size.height); }
- (void)setFixedPlayerLayerSize:(NSSize)size {}
- (BOOL)prefersUnobscuredContent { return NO; }
- (id)slowMotionDelegate { return ViewState(self)->slowMotionDelegate; }
- (void)setSlowMotionDelegate:(id)d { ViewState(self)->slowMotionDelegate = d; }
- (CMTimeRange)slowMotionTimeRange { return ViewState(self)->slowMotionRange; }
- (void)setSlowMotionTimeRange:(CMTimeRange)range { ViewState(self)->slowMotionRange = range; }
- (id)shareController { return ViewState(self)->shareController; }
- (id)delegate { return ViewState(self)->delegate; }
- (void)setDelegate:(id)d { ViewState(self)->delegate = d; }
- (NSUInteger)timeDisplayStyle { return ViewState(self)->timeDisplayStyle; }
- (void)setTimeDisplayStyle:(NSUInteger)style { ViewState(self)->timeDisplayStyle = style; }
- (NSArray *)availableTimeDisplayStyles { return @[]; }
- (NSArray *)allowedTimeDisplayStyles { return @[]; }
- (BOOL)showsTimecodes { return ViewState(self)->showsTimecodes; }
- (void)setShowsTimecodes:(BOOL)b { ViewState(self)->showsTimecodes = b; }
- (BOOL)canHideControls { return ViewState(self)->canHideControls; }
- (void)setCanHideControls:(BOOL)b { ViewState(self)->canHideControls = b; ShowControls(self, YES); }
- (BOOL)allowsMagnification { return ViewState(self)->allowsMagnification; }
- (void)setAllowsMagnification:(BOOL)b { ViewState(self)->allowsMagnification = b; }
- (double)magnification { return ViewState(self)->magnification; }
- (void)setMagnification:(double)m { ViewState(self)->magnification = m; }
- (void)setCanZoomHorizontally:(BOOL)b {}
- (void)setCanZoomVertically:(BOOL)b {}
- (BOOL)canZoomHorizontally { return NO; }
- (BOOL)canZoomVertically { return NO; }
- (BOOL)isZoomed { return ViewState(self)->zoomed; }
- (void)setZoomed:(BOOL)b { ViewState(self)->zoomed = b; }
- (void)toggleZoom:(id)sender {}
- (BOOL)isFullScreen { return NO; }
- (id)trimControlsViewController { return nil; }
- (BOOL)canBeginTrimming { return NO; }
- (BOOL)isTrimming { return NO; }
- (void)beginTrimmingWithMaximumDuration:(CMTime)max completionHandler:(void (^)(NSInteger))handler { if (handler) handler(1); }
- (void)beginTrimmingWithCompletionHandler:(void (^)(NSInteger))handler { if (handler) handler(1); }
- (BOOL)showsControlsPane { return NO; }
- (BOOL)showsAlternateMediaTrackPreview { return ViewState(self)->showsAlternatePreview; }
- (void)setShowsAlternateMediaTrackPreview:(BOOL)b { ViewState(self)->showsAlternatePreview = b; }
- (NSArray *)speeds { return @[]; }
- (void)setSpeeds:(NSArray *)speeds {}
- (id)selectedSpeed { return nil; }
- (void)selectSpeed:(id)speed {}
- (id)playbackSpeedCollection { return nil; }
- (BOOL)enableStereoEyesIfAvailable { return ViewState(self)->enableStereo; }
- (void)setEnableStereoEyesIfAvailable:(BOOL)b { ViewState(self)->enableStereo = b; }
- (NSInteger)stereoEyes { return ViewState(self)->stereoEyes; }
- (void)setStereoEyes:(NSInteger)eyes { ViewState(self)->stereoEyes = eyes; }
- (NSMenu *)buildLegibleOptionsMenu { return nil; }
- (NSMenu *)actionPopUpButtonMenu { return nil; }
- (void)showEditControlsViewController:(id)vc withCompletionHandler:(void (^)(void))handler { if (handler) handler(); }
- (void)showEditControlsViewController:(id)vc shouldPausePlayback:(BOOL)pause withCompletionHandler:(void (^)(void))handler { if (handler) handler(); }
- (void)hideEditControlsViewControllerWithCompletionHandler:(void (^)(void))handler { if (handler) handler(); }
- (void)volumeControlViewDidChangeMutedState:(id)sender {}
- (void)volumeControlViewDidChangeVolume:(id)sender {}
- (void)windowWillBeginSheet:(NSNotification *)n {}
- (void)windowDidEndSheet:(NSNotification *)n {}
- (BOOL)updatesNowPlayingInfoCenter { return NO; }
- (void)setUpdatesNowPlayingInfoCenter:(BOOL)b {}
- (BOOL)allowsVideoFrameAnalysis { return NO; }
- (void)setAllowsVideoFrameAnalysis:(BOOL)b {}
- (BOOL)showsFrameSteppingButtons { return NO; }
- (void)setShowsFrameSteppingButtons:(BOOL)b {}
- (BOOL)showsSharingServiceButton { return NO; }
- (void)setShowsSharingServiceButton:(BOOL)b {}
- (BOOL)showsFullScreenToggleButton { return NO; }
- (void)setShowsFullScreenToggleButton:(BOOL)b {}
- (BOOL)allowsPictureInPicturePlayback { return NO; }
- (void)setAllowsPictureInPicturePlayback:(BOOL)b {}

- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)acceptsFirstMouse:(NSEvent *)event { return YES; }
- (BOOL)isOpaque { return YES; }

- (void)setFrameSize:(NSSize)size
{
    [super setFrameSize:size];
    LayOut(self);
    [self willChangeValueForKey:@"playerLayerFrame"];
    [self didChangeValueForKey:@"playerLayerFrame"];
}

- (void)layout
{
    [super layout];
    LayOut(self);
}

- (void)viewDidMoveToWindow
{
    [super viewDidMoveToWindow];
    LayOut(self);
}

- (void)updateTrackingAreas
{
    [super updateTrackingAreas];
    ReefPlayerViewState *s = ViewState(self);
    if (s->tracking)
        [self removeTrackingArea:s->tracking];
    s->tracking = [[NSTrackingArea alloc] initWithRect:self.bounds
                                               options:NSTrackingMouseMoved | NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways | NSTrackingInVisibleRect
                                                 owner:self userInfo:nil];
    [self addTrackingArea:s->tracking];
}

- (void)mouseMoved:(NSEvent *)event { ShowControls(self, YES); }
- (void)mouseEntered:(NSEvent *)event { ShowControls(self, YES); }
- (void)mouseExited:(NSEvent *)event { ShowControls(self, NO); }

- (void)keyDown:(NSEvent *)event
{
    NSString *keys = event.charactersIgnoringModifiers;
    ReefPlayerViewState *s = ViewState(self);
    if ([keys isEqualToString:@" "]) {
        [s->controls togglePlayback:self];
        ShowControls(self, YES);
        return;
    }
    unichar c = keys.length ? [keys characterAtIndex:0] : 0;
    if (c == NSLeftArrowFunctionKey || c == NSRightArrowFunctionKey) {
        [s->player pause];
        [s->player.currentItem stepByCount:c == NSRightArrowFunctionKey ? 1 : -1];
        [s->controls update];
        return;
    }
    [super keyDown:event];
}

@end

#pragma mark - AVAudioView

// AVKit's view for sound only: the playback controls on a dark background, across the view.
@interface AVAudioView : NSView
@property (nonatomic, strong) AVPlayer *player;
@property (nonatomic, strong) id playerController;
@property (nonatomic) BOOL showsTimecodes, showsSharingServiceButton, showsDurationInsteadOfRemainingTime;
@end

@implementation AVAudioView {
    ReefPlayerControlsView *_controls;
    id _timeObserver;
}

- (void)reefSetUp
{
    if (_controls)
        return;
    _controls = [[ReefPlayerControlsView alloc] initWithFrame:NSMakeRect(8, 8, MAX(self.bounds.size.width - 16, 200), 44)];
    _controls.autoresizingMask = NSViewWidthSizable | NSViewMaxYMargin;
    [self addSubview:_controls];
}

- (instancetype)initWithFrame:(NSRect)frame
{
    if ((self = [super initWithFrame:frame]))
        [self reefSetUp];
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    if ((self = [super initWithCoder:coder]))
        [self reefSetUp];
    return self;
}

- (void)dealloc
{
    if (_timeObserver)
        [_player removeTimeObserver:_timeObserver];
}

- (void)drawRect:(NSRect)dirty
{
    [[NSColor colorWithWhite:0.1 alpha:1] setFill];
    NSRectFill(dirty);
}

- (void)setFrameSize:(NSSize)size
{
    [super setFrameSize:size];
    _controls.frame = NSMakeRect(8, MAX((size.height - 44) / 2, 4), MAX(size.width - 16, 120), 44);
}

- (void)setPlayer:(AVPlayer *)player
{
    if (_player == player)
        return;
    if (_timeObserver)
        [_player removeTimeObserver:_timeObserver];
    _timeObserver = nil;
    [self willChangeValueForKey:@"player"];
    _player = player;
    [self didChangeValueForKey:@"player"];
    _controls.player = player;
    if (player) {
        __weak ReefPlayerControlsView *controls = _controls;
        _timeObserver = [player addPeriodicTimeObserverForInterval:CMTimeMake(1, 10) queue:dispatch_get_main_queue() usingBlock:^(CMTime t) {
            [controls update];
        }];
    }
    [_controls update];
}

- (void)setPlayerController:(id)controller
{
    if (_playerController == controller)
        return;
    [self willChangeValueForKey:@"playerController"];
    _playerController = controller;
    [self didChangeValueForKey:@"playerController"];
    self.player = [controller player];
}

- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)isOpaque { return YES; }
- (void)keyDown:(NSEvent *)event
{
    if ([event.charactersIgnoringModifiers isEqualToString:@" "])
        [_controls togglePlayback:self];
    else
        [super keyDown:event];
}
- (BOOL)showsAlternateMediaTrackPreview { return NO; }
- (void)setShowsAlternateMediaTrackPreview:(BOOL)b {}
- (BOOL)canBeginTrimming { return NO; }
- (BOOL)isTrimming { return NO; }
- (BOOL)isEditing { return NO; }
- (BOOL)showsControlsPane { return NO; }
- (CMTimeRange)slowMotionTimeRange { return kCMTimeRangeInvalid; }
- (id)trimControlsViewController { return nil; }
- (id)playbackSpeedCollection { return nil; }
- (NSMenu *)actionPopUpButtonMenu { return nil; }
- (void)setActionPopUpButtonMenu:(NSMenu *)menu {}
- (void)beginTrimmingWithMaximumDuration:(CMTime)max completionHandler:(void (^)(NSInteger))handler { if (handler) handler(1); }
- (void)showEditControlsViewController:(id)vc withCompletionHandler:(void (^)(void))handler { if (handler) handler(); }
- (void)showEditControlsViewController:(id)vc shouldPausePlayback:(BOOL)pause withCompletionHandler:(void (^)(void))handler { if (handler) handler(); }
- (void)hideEditControlsViewControllerWithCompletionHandler:(void (^)(void))handler { if (handler) handler(); }
- (void)volumeControlViewDidChangeMutedState:(id)sender {}
- (void)volumeControlViewDidChangeVolume:(id)sender {}
@end
