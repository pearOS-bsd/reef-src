// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// AVFoundation's time extensions of NSValue and NSCoder (AVTime.h), which Reef's AVFoundation lacks, and the export
// session's compatibility questions (no export on Reef yet: nothing is compatible).

#import <AVFoundation/AVFoundation.h>

@implementation NSValue (ReefCMTime)
+ (NSValue *)valueWithCMTime:(CMTime)time { return [NSValue valueWithBytes:&time objCType:@encode(CMTime)]; }
- (CMTime)CMTimeValue
{
    CMTime time = kCMTimeInvalid;
    if (!strcmp(self.objCType, @encode(CMTime)))
        [self getValue:&time];
    return time;
}
+ (NSValue *)valueWithCMTimeRange:(CMTimeRange)range { return [NSValue valueWithBytes:&range objCType:@encode(CMTimeRange)]; }
- (CMTimeRange)CMTimeRangeValue
{
    CMTimeRange range = kCMTimeRangeInvalid;
    if (!strcmp(self.objCType, @encode(CMTimeRange)))
        [self getValue:&range];
    return range;
}
+ (NSValue *)valueWithCMTimeMapping:(CMTimeMapping)mapping { return [NSValue valueWithBytes:&mapping objCType:@encode(CMTimeMapping)]; }
- (CMTimeMapping)CMTimeMappingValue
{
    CMTimeMapping mapping = {kCMTimeRangeInvalid, kCMTimeRangeInvalid};
    if (!strcmp(self.objCType, @encode(CMTimeMapping)))
        [self getValue:&mapping];
    return mapping;
}
@end

// The keys and dictionaries are CoreMedia's (CMTimeCopyAsDictionary).
@implementation NSCoder (ReefCMTime)
- (void)encodeCMTime:(CMTime)time forKey:(NSString *)key
{
    CFDictionaryRef d = CMTimeCopyAsDictionary(time, kCFAllocatorDefault);
    [self encodeObject:(__bridge NSDictionary *)d forKey:key];
    if (d)
        CFRelease(d);
}
- (CMTime)decodeCMTimeForKey:(NSString *)key
{
    NSDictionary *d = [self decodeObjectOfClass:[NSDictionary class] forKey:key];
    return d ? CMTimeMakeFromDictionary((__bridge CFDictionaryRef)d) : kCMTimeInvalid;
}
- (void)encodeCMTimeRange:(CMTimeRange)range forKey:(NSString *)key
{
    CFDictionaryRef d = CMTimeRangeCopyAsDictionary(range, kCFAllocatorDefault);
    [self encodeObject:(__bridge NSDictionary *)d forKey:key];
    if (d)
        CFRelease(d);
}
- (CMTimeRange)decodeCMTimeRangeForKey:(NSString *)key
{
    NSDictionary *d = [self decodeObjectOfClass:[NSDictionary class] forKey:key];
    return d ? CMTimeRangeMakeFromDictionary((__bridge CFDictionaryRef)d) : kCMTimeRangeInvalid;
}
- (void)encodeCMTimeMapping:(CMTimeMapping)mapping forKey:(NSString *)key
{
    [self encodeCMTimeRange:mapping.source forKey:[key stringByAppendingString:@".source"]];
    [self encodeCMTimeRange:mapping.target forKey:[key stringByAppendingString:@".target"]];
}
- (CMTimeMapping)decodeCMTimeMappingForKey:(NSString *)key
{
    CMTimeMapping m = {[self decodeCMTimeRangeForKey:[key stringByAppendingString:@".source"]],
                       [self decodeCMTimeRangeForKey:[key stringByAppendingString:@".target"]]};
    return m;
}
@end

@implementation AVAssetExportSession (ReefPlayback)
+ (void)determineCompatibilityOfExportPreset:(NSString *)preset withAsset:(AVAsset *)asset outputFileType:(NSString *)type
                           completionHandler:(void (^)(BOOL))handler
{
    if (handler)
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{ handler(NO); });
}
+ (NSArray<NSString *> *)exportPresetsCompatibleWithAsset:(AVAsset *)asset { return @[]; }
@end
