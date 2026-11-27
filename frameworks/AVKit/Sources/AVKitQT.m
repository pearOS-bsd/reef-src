// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <AppKit/AppKit.h>
#import <CoreMedia/CoreMedia.h>

// Private AVKit pieces QuickTime Player links against.
@interface AVPlayerController : NSObject
@property (strong) id player;
@end
@implementation AVPlayerController
@end
@interface AVPlayerControllerTimeResolver : NSObject
@end
@implementation AVPlayerControllerTimeResolver
@end
@interface AVPlayerControlsViewController : NSViewController
@end
@implementation AVPlayerControlsViewController
@end
@interface AVCaptureView : NSView
@end
@implementation AVCaptureView
@end
@interface AVAudioLevelIndicatorView : NSView
@end
@implementation AVAudioLevelIndicatorView
@end
@interface AVVolumeControlSlider : NSSlider
@end
@implementation AVVolumeControlSlider
@end
@interface AVTouchBarCaptureInputSourceItem : NSObject
@end
@implementation AVTouchBarCaptureInputSourceItem
@end
@interface AVTouchBarRecordingControlsProvider : NSObject
@end
@implementation AVTouchBarRecordingControlsProvider
@end
@interface AVAssetIdentifier : NSObject
@end
@implementation AVAssetIdentifier
@end
@interface AVTimecode : NSObject
@end
@implementation AVTimecode
@end

// Media selection menus: the options of the asset's groups become menu items.
void AVAddAudibleMediaSelectionOptionsToMenu(NSMenu *menu, id asset, id selection) {}
void AVAddLegibleMediaSelectionOptionsToMenu(NSMenu *menu, id asset, id selection) {}
// Slow motion: the normalized range is already the asset's range when there is no slow motion.
CMTimeRange AVAssetSlowMotionTimeRangeFromNormalizedSlowMotionTimeRange(CMTimeRange range, id asset) { return range; }
CMTimeRange AVNormalizedSlowMotionTimeRangeFromAssetSlowMotionTimeRange(CMTimeRange range, id asset) { return range; }
