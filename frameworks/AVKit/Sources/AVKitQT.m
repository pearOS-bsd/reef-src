// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <AppKit/AppKit.h>
#import <CoreMedia/CoreMedia.h>

// Private AVKit pieces QuickTime Player links against (the player controller, its time resolver, the controls view
// controller and asset identifiers are in AVKitPlayback.m).
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
