// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

// Classes QuickTime Player links against. Capture needs hardware Reef does not have; the others are plain value classes.
@interface AVAssetCollection : NSObject
@end
@implementation AVAssetCollection
@end

@interface AVCaptureAudioFileOutput : NSObject
@end
@implementation AVCaptureAudioFileOutput
@end

@interface AVCaptureAudioPreviewOutput : NSObject
@property (nonatomic) float volume;
@property (nonatomic, copy) NSString *outputDeviceUniqueID;
@end
@implementation AVCaptureAudioPreviewOutput
@end

@interface AVCaptureMovieFileOutput : NSObject
@property (nonatomic, readonly, getter=isRecording) BOOL recording;
@end
@implementation AVCaptureMovieFileOutput
- (BOOL)isRecording { return NO; }
@end

@interface AVMutableAudioMixInputParameters : NSObject
@property (nonatomic) int trackID;
- (void)setVolume:(float)volume atTime:(CMTime)time;
- (void)setVolumeRampFromStartVolume:(float)s toEndVolume:(float)e timeRange:(CMTimeRange)r;
+ (instancetype)audioMixInputParametersWithTrack:(id)track;
@end
@implementation AVMutableAudioMixInputParameters
+ (instancetype)audioMixInputParametersWithTrack:(id)track { return [[self alloc] init]; }
- (void)setVolume:(float)volume atTime:(CMTime)time {}
- (void)setVolumeRampFromStartVolume:(float)s toEndVolume:(float)e timeRange:(CMTimeRange)r {}
@end

@interface AVMutableAudioMix : NSObject
@property (nonatomic, copy) NSArray *inputParameters;
+ (instancetype)audioMix;
@end
@implementation AVMutableAudioMix
+ (instancetype)audioMix { return [[self alloc] init]; }
@end

@interface AVPlayerItemTrack : NSObject
@property (nonatomic, getter=isEnabled) BOOL enabled;
@property (nonatomic, readonly) id assetTrack;
@end
@implementation AVPlayerItemTrack
- (instancetype)init { if ((self = [super init])) _enabled = YES; return self; }
@end

// A time as hours:minutes:seconds ("1:02:03" or "02:03").
@interface AVTimeFormatter : NSFormatter
@end
@implementation AVTimeFormatter
- (NSString *)stringForObjectValue:(id)obj {
    if (![obj respondsToSelector:@selector(doubleValue)]) return nil;
    long s = (long)[obj doubleValue], neg = s < 0;
    if (neg) s = -s;
    NSString *t = s >= 3600 ? [NSString stringWithFormat:@"%ld:%02ld:%02ld", s / 3600, s / 60 % 60, s % 60] : [NSString stringWithFormat:@"%02ld:%02ld", s / 60, s % 60];
    return neg ? [@"-" stringByAppendingString:t] : t;
}
- (BOOL)getObjectValue:(id *)obj forString:(NSString *)string errorDescription:(NSString **)error { *obj = @([string doubleValue]); return YES; }
@end
