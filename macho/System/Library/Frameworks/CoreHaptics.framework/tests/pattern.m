// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CoreHaptics: events, parameters and patterns keep their data; a pattern lasts until its last event ends.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
extern NSString *const CHHapticEventTypeHapticTransient, *const CHHapticEventTypeHapticContinuous, *const CHHapticEventTypeAudioContinuous, *const CHHapticEventTypeAudioCustom;
extern NSString *const CHHapticEventParameterIDHapticIntensity, *const CHHapticEventParameterIDHapticSharpness, *const CHHapticEventParameterIDAttackTime;
extern NSString *const CHHapticEventParameterIDDecayTime, *const CHHapticEventParameterIDReleaseTime, *const CHHapticEventParameterIDSustained;
extern NSString *const CHHapticDynamicParameterIDHapticIntensityControl, *const CHHapticDynamicParameterIDHapticSharpnessControl, *const CHHapticErrorDomain;

@interface CHHapticEventParameter : NSObject
@property (readonly, copy) NSString *parameterID;
@property float value;
- (id)initWithParameterID:(NSString *)parameterID value:(float)value;
@end
@interface CHHapticDynamicParameter : NSObject
@property (readonly, copy) NSString *parameterID;
@property float value;
@property double relativeTime;
- (id)initWithParameterID:(NSString *)parameterID value:(float)value relativeTime:(double)relativeTime;
@end
@interface CHHapticEvent : NSObject
@property (readonly, copy) NSString *type;
@property (readonly, copy) NSArray *eventParameters;
@property double relativeTime, duration;
- (id)initWithEventType:(NSString *)type parameters:(NSArray *)parameters relativeTime:(double)relativeTime;
- (id)initWithEventType:(NSString *)type parameters:(NSArray *)parameters relativeTime:(double)relativeTime duration:(double)duration;
@end
@interface CHHapticPattern : NSObject
@property (readonly) double duration;
- (id)initWithEvents:(NSArray *)events parameters:(NSArray *)parameters error:(NSError **)error;
@end

int main(void)
{
    printf("names %s %s %s %s\n", [CHHapticEventTypeHapticTransient UTF8String], [CHHapticEventTypeHapticContinuous UTF8String], [CHHapticEventTypeAudioContinuous UTF8String], [CHHapticEventTypeAudioCustom UTF8String]);
    printf("ids %s %s %s %s %s %s\n", [CHHapticEventParameterIDHapticIntensity UTF8String], [CHHapticEventParameterIDHapticSharpness UTF8String], [CHHapticEventParameterIDAttackTime UTF8String],
           [CHHapticEventParameterIDDecayTime UTF8String], [CHHapticEventParameterIDReleaseTime UTF8String], [CHHapticEventParameterIDSustained UTF8String]);
    printf("dynamic %s %s %s\n", [CHHapticDynamicParameterIDHapticIntensityControl UTF8String], [CHHapticDynamicParameterIDHapticSharpnessControl UTF8String], [CHHapticErrorDomain UTF8String]);
    CHHapticEventParameter *p = [[CHHapticEventParameter alloc] initWithParameterID:CHHapticEventParameterIDHapticIntensity value:0.5f];
    p.value = 0.75f;
    printf("parameter %s %g\n", [p.parameterID UTF8String], p.value);
    CHHapticDynamicParameter *d = [[CHHapticDynamicParameter alloc] initWithParameterID:CHHapticDynamicParameterIDHapticSharpnessControl value:0.25f relativeTime:1.5];
    d.relativeTime = 2.5;
    printf("dynamic %s %g %g\n", [d.parameterID UTF8String], d.value, d.relativeTime);
    CHHapticEvent *a = [[CHHapticEvent alloc] initWithEventType:CHHapticEventTypeHapticTransient parameters:@[p] relativeTime:1.0];
    CHHapticEvent *b = [[CHHapticEvent alloc] initWithEventType:CHHapticEventTypeHapticContinuous parameters:nil relativeTime:0.5 duration:3.0];
    printf("events %s %lu %g %g | %s %lu %g %g\n", [a.type UTF8String], (unsigned long)[a.eventParameters count], a.relativeTime, a.duration, [b.type UTF8String], (unsigned long)[b.eventParameters count], b.relativeTime, b.duration);
    b.duration = 4.0; b.relativeTime = 0.25;
    NSError *e = nil;
    CHHapticPattern *pattern = [[CHHapticPattern alloc] initWithEvents:@[a, b] parameters:@[d] error:&e];
    printf("pattern %g\n", pattern.duration);
    CHHapticPattern *empty = [[CHHapticPattern alloc] initWithEvents:nil parameters:nil error:&e];
    printf("empty %g\n", empty.duration);
    return 0;
}
