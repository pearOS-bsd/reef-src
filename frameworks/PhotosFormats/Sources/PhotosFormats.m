// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

// Photos library formats (slow motion, video adjustments): no library.
@interface PFSlowMotionUtilities : NSObject
@end
@implementation PFSlowMotionUtilities
@end
// Slow motion of a video: none unless the video is a high frame rate one (as Photos defines it); no adjustments file.
@interface PFVideoAdjustments : NSObject
@property (nonatomic) CMTimeRange slowMotionTimeRange;
@property (nonatomic) float slowMotionRate;
@end
@implementation PFVideoAdjustments
+ (NSURL *)defaultVideoAdjustmentsURLForVideoURL:(NSURL *)url { return nil; }
+ (float)defaultSlowMotionRateForNominalFrameRate:(float)rate { return rate > 60 ? 30 / rate : 1; }
+ (CMTimeRange)defaultSlowMotionTimeRangeForDuration:(CMTime)duration { return kCMTimeRangeZero; }
- (instancetype)initWithSlowMotionTimeRange:(CMTimeRange)range rate:(float)rate
{
    if ((self = [super init])) {
        _slowMotionTimeRange = range;
        _slowMotionRate = rate;
    }
    return self;
}
- (BOOL)hasSlowMotionAdjustments { return NO; }
- (BOOL)isSlowMotion { return NO; }
@end
@interface PFVideoAVObjectBuilder : NSObject
@end
@implementation PFVideoAVObjectBuilder
@end
