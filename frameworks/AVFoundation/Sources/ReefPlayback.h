// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// What the playback sources of the AVFoundation wrapper share. Reef's AVFoundation has the classes (AVAsset, AVURLAsset,
// AVAssetTrack, AVPlayerItem, AVPlayer, AVPlayerLayer, AVMutableComposition ...) but no media engine: their loading and
// playing answer "Media playback is not available". The categories of this wrapper give those classes the engine of
// ReefMedia.h; what they keep per object lives in an associated state object.

#pragma once

#import <AVFoundation/AVFoundation.h>
#import "ReefMedia.h"

NS_ASSUME_NONNULL_BEGIN

// What an item plays: a file and the part of it (file time = item time + offset).
@interface ReefMediaSource : NSObject
@property (nonatomic, copy) NSString *path;
@property (nonatomic) double offset;
@property (nonatomic) double duration;
@property (nonatomic) BOOL hasVideo, hasAudio;
@property (nonatomic) CGSize naturalSize;
@end

// The source of an asset (a file asset or a composition made of one file), nil when it cannot play. Loads the asset.
ReefMediaSource *_Nullable ReefSourceForAsset(AVAsset *asset);
// Loads a file asset now (what an unloaded property does on macOS); YES when it can be read.
BOOL ReefAssetLoadNow(AVAsset *asset);

CMTime ReefTime(double seconds);
// Prints when REEF_AV_LOG is set.
void ReefLog(NSString *format, ...) NS_FORMAT_FUNCTION(1, 2);

NS_ASSUME_NONNULL_END
