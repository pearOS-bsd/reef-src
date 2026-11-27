// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// AVKit: a player view starts with inline controls and the aspect gravity, keeps its settings and has no video.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSString (T)
- (const char *)UTF8String;
@end

@interface AVPlayerView : NSObject
- (id)initWithFrame:(CGRect)frame;
@property (strong) id player;
@property long controlsStyle;
@property (copy) NSString *videoGravity;
@property (readonly, getter=isReadyForDisplay) BOOL readyForDisplay;
@property (readonly) CGRect videoBounds;
@property BOOL showsFrameSteppingButtons, showsSharingServiceButton, showsFullScreenToggleButton, showsTimecodes, allowsPictureInPicturePlayback;
@end

int main(void)
{
    CGRect frame = {{1, 2}, {300, 200}};
    AVPlayerView *v = [[AVPlayerView alloc] initWithFrame:frame];
    printf("initial %ld %s %d %d\n", v.controlsStyle, [v.videoGravity UTF8String], v.showsFullScreenToggleButton, v.isReadyForDisplay);
    CGRect b = v.videoBounds;
    printf("bounds %g %g %g %g\n", b.origin.x, b.origin.y, b.size.width, b.size.height);
    id player = [NSObject new];
    v.player = player; v.controlsStyle = 3; v.videoGravity = @"AVLayerVideoGravityResizeAspectFill";
    v.showsFrameSteppingButtons = YES; v.showsSharingServiceButton = YES; v.showsFullScreenToggleButton = YES; v.showsTimecodes = YES; v.allowsPictureInPicturePlayback = YES;
    printf("set %d %ld %s %d%d%d%d%d\n", v.player == player, v.controlsStyle, [v.videoGravity UTF8String], v.showsFrameSteppingButtons,
           v.showsSharingServiceButton, v.showsFullScreenToggleButton, v.showsTimecodes, v.allowsPictureInPicturePlayback);
    return 0;
}
