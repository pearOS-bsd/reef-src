// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoreHaptics: haptic events, parameters and patterns as plain data (there is no haptic engine).

#import <Foundation/Foundation.h>

NSString *const CHHapticEventTypeHapticTransient = @"HapticTransient";
NSString *const CHHapticEventTypeHapticContinuous = @"HapticContinuous";
NSString *const CHHapticEventTypeAudioContinuous = @"AudioContinuous";
NSString *const CHHapticEventTypeAudioCustom = @"AudioCustom";
NSString *const CHHapticEventParameterIDHapticIntensity = @"HapticIntensity";
NSString *const CHHapticEventParameterIDHapticSharpness = @"HapticSharpness";
NSString *const CHHapticEventParameterIDAttackTime = @"AttackTime";
NSString *const CHHapticEventParameterIDDecayTime = @"DecayTime";
NSString *const CHHapticEventParameterIDReleaseTime = @"ReleaseTime";
NSString *const CHHapticEventParameterIDSustained = @"Sustained";
NSString *const CHHapticDynamicParameterIDHapticIntensityControl = @"HapticIntensityControl";
NSString *const CHHapticDynamicParameterIDHapticSharpnessControl = @"HapticSharpnessControl";
NSString *const CHHapticErrorDomain = @"com.apple.CoreHaptics";

@interface CHHapticEventParameter : NSObject
{
    float _value;
    NSString *_parameterID;
}
@property (readonly, copy) NSString *parameterID;
@property float value;
- (id)initWithParameterID:(NSString *)parameterID value:(float)value;
@end

@implementation CHHapticEventParameter

- (id)initWithParameterID:(NSString *)parameterID value:(float)value
{
    self = [super init];
    if (self) {
        _parameterID = [parameterID copy];
        _value = value;
    }
    return self;
}

@end

@interface CHHapticDynamicParameter : NSObject
{
    float _value;
    NSString *_parameterID;
    double _relativeTime;
}
@property (readonly, copy) NSString *parameterID;
@property float value;
@property double relativeTime;
- (id)initWithParameterID:(NSString *)parameterID value:(float)value relativeTime:(double)relativeTime;
@end

@implementation CHHapticDynamicParameter

- (id)initWithParameterID:(NSString *)parameterID value:(float)value relativeTime:(double)relativeTime
{
    self = [super init];
    if (self) {
        _parameterID = [parameterID copy];
        _value = value;
        _relativeTime = relativeTime;
    }
    return self;
}

@end

@interface CHHapticEvent : NSObject
{
    NSString *_type;
    NSArray *_eventParameters;
    double _relativeTime;
    double _duration;
}
@property (readonly, copy) NSString *type;
@property (readonly, copy) NSArray *eventParameters;
@property double relativeTime;
@property double duration;
- (id)initWithEventType:(NSString *)type parameters:(NSArray *)parameters relativeTime:(double)relativeTime;
- (id)initWithEventType:(NSString *)type parameters:(NSArray *)parameters relativeTime:(double)relativeTime duration:(double)duration;
@end

@implementation CHHapticEvent

- (id)initWithEventType:(NSString *)type parameters:(NSArray *)parameters relativeTime:(double)relativeTime
{
    return [self initWithEventType:type parameters:parameters relativeTime:relativeTime duration:0];
}

- (id)initWithEventType:(NSString *)type parameters:(NSArray *)parameters relativeTime:(double)relativeTime duration:(double)duration
{
    self = [super init];
    if (self) {
        _type = [type copy];
        _eventParameters = [parameters copy] ?: @[];
        _relativeTime = relativeTime;
        _duration = duration;
    }
    return self;
}

@end

@interface CHHapticPattern : NSObject
{
    NSArray *_events;
    NSArray *_parameters;
}
@property (readonly) double duration;
- (id)initWithEvents:(NSArray *)events parameters:(NSArray *)parameters error:(NSError **)error;
@end

@implementation CHHapticPattern

- (id)initWithEvents:(NSArray *)events parameters:(NSArray *)parameters error:(NSError **)error
{
    self = [super init];
    if (self) {
        _events = [events copy] ?: @[];
        _parameters = [parameters copy] ?: @[];
    }
    return self;
}

- (double)duration
{
    double end = 0;
    for (CHHapticEvent *event in _events) {
        double eventEnd = [event relativeTime] + [event duration];
        end = eventEnd > end ? eventEnd : end;
    }
    return end;
}

@end
