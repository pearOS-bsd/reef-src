// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AVKit: AVPlayerView, a view that keeps its player and settings and shows no video.

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
@end

@class AVPlayer;

extern NSString *const AVLayerVideoGravityResizeAspect;
extern const CGRect NSZeroRect;

@interface NSView : NSObject
{
    char _padding[176];
}
- (id)initWithFrame:(CGRect)frame;
- (id)initWithCoder:(id)coder;
@end

@interface AVPlayerView : NSView
{
    BOOL _showsFrameSteppingButtons;
    BOOL _showsSharingServiceButton;
    BOOL _showsFullScreenToggleButton;
    BOOL _showsTimecodes;
    BOOL _allowsPictureInPicturePlayback;
    AVPlayer *_player;
    long _controlsStyle;
    NSString *_videoGravity;
}
@property (strong) AVPlayer *player;
@property long controlsStyle;
@property (copy) NSString *videoGravity;
@property (readonly, getter=isReadyForDisplay) BOOL readyForDisplay;
@property (readonly) CGRect videoBounds;
@property BOOL showsFrameSteppingButtons;
@property BOOL showsSharingServiceButton;
@property BOOL showsFullScreenToggleButton;
@property BOOL showsTimecodes;
@property BOOL allowsPictureInPicturePlayback;
@end

@implementation AVPlayerView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _controlsStyle = 1;
        _videoGravity = [AVLayerVideoGravityResizeAspect copy];
        _showsFullScreenToggleButton = NO;
    }
    return self;
}

- (id)initWithCoder:(id)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        _controlsStyle = 1;
        _videoGravity = [AVLayerVideoGravityResizeAspect copy];
    }
    return self;
}

- (void)dealloc
{
    [_player release];
    [_videoGravity release];
    [super dealloc];
}

- (BOOL)isReadyForDisplay { return NO; }
- (CGRect)videoBounds { return NSZeroRect; }

@end
