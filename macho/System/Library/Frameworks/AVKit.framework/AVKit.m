// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AVKit.

#import <Foundation/Foundation.h>

@class AVPlayer;

@interface NSView : NSObject
{
    char _padding[176];
}
@end


@interface AVPlayerView : NSView
{
    BOOL _showsFrameSteppingButtons;
    BOOL _showsSharingServiceButton;
    BOOL _showsFullScreenToggleButton;
    BOOL _showsTimecodes;
    BOOL _allowsPictureInPicturePlayback;
    AVPlayer * _player;
    long _controlsStyle;
    NSString * _videoGravity;
}
@property (strong) AVPlayer * player;
@property long controlsStyle;
@property (copy) NSString * videoGravity;
@property (readonly, getter=isReadyForDisplay) BOOL readyForDisplay;
@property (readonly) CGRect videoBounds;
@property BOOL showsFrameSteppingButtons;
@property BOOL showsSharingServiceButton;
@property BOOL showsFullScreenToggleButton;
@property BOOL showsTimecodes;
@property BOOL allowsPictureInPicturePlayback;
- (id)initWithFrame:(CGRect)a0;
- (id)initWithCoder:(id)a0;
- (void)dealloc;
@end

@implementation AVPlayerView
- (id)initWithFrame:(CGRect)a0
{
    // unresolved
    return nil;
}

- (id)initWithCoder:(id)a0
{
    // unresolved
    return nil;
}

- (void)dealloc
{
    // unresolved

}

@end


