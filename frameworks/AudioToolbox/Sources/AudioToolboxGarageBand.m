// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// AudioToolbox classes GarageBand uses that Reef's AudioToolbox lacks.
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioComponent.h>

@interface AUAudioUnitPreset : NSObject <NSSecureCoding>
@property NSInteger number;
@property (copy) NSString *name;
@end
@implementation AUAudioUnitPreset
+ (BOOL)supportsSecureCoding { return YES; }
- (void)encodeWithCoder:(NSCoder *)coder {
    [coder encodeInteger:_number forKey:@"number"];
    [coder encodeObject:_name forKey:@"name"];
}
- (instancetype)initWithCoder:(NSCoder *)coder {
    if ((self = [super init])) {
        _number = [coder decodeIntegerForKey:@"number"];
        _name = [coder decodeObjectOfClass:NSString.class forKey:@"name"];
    }
    return self;
}
@end

// Version 3 audio units: no extension provides one here, so none can be instantiated.
@interface AUAudioUnit : NSObject
@property (readonly) AudioComponentDescription componentDescription;
@end
@implementation AUAudioUnit
+ (void)instantiateWithComponentDescription:(AudioComponentDescription)description options:(UInt32)options
                         completionHandler:(void (^)(AUAudioUnit *, NSError *))handler {
    if (handler) handler(nil, [NSError errorWithDomain:NSOSStatusErrorDomain code:-66749 userInfo:nil]);
}
+ (void)registerSubclass:(Class)cls asComponentDescription:(AudioComponentDescription)description name:(NSString *)name version:(UInt32)version {}
- (instancetype)initWithComponentDescription:(AudioComponentDescription)description error:(NSError **)error {
    return [self initWithComponentDescription:description options:0 error:error];
}
- (instancetype)initWithComponentDescription:(AudioComponentDescription)description options:(UInt32)options error:(NSError **)error {
    if ((self = [super init])) _componentDescription = description;
    return self;
}
- (NSArray *)factoryPresets { return @[]; }
- (NSArray *)userPresets { return @[]; }
- (BOOL)allocateRenderResourcesAndReturnError:(NSError **)error { return YES; }
- (void)deallocateRenderResources {}
- (BOOL)renderResourcesAllocated { return NO; }
- (NSString *)audioUnitName { return nil; }
- (NSString *)manufacturerName { return nil; }
- (NSString *)audioUnitShortName { return nil; }
- (id)parameterTree { return nil; }
- (NSArray *)channelCapabilities { return nil; }
- (NSTimeInterval)latency { return 0; }
- (NSTimeInterval)tailTime { return 0; }
- (NSUInteger)maximumFramesToRender { return 4096; }
- (void)setMaximumFramesToRender:(NSUInteger)frames {}
- (BOOL)providesUserInterface { return NO; }
- (BOOL)isMusicDeviceOrEffect { return NO; }
- (NSInteger)virtualMIDICableCount { return 0; }
- (NSDictionary *)fullState { return @{}; }
- (void)setFullState:(NSDictionary *)state {}
- (NSDictionary *)fullStateForDocument { return @{}; }
- (void)setFullStateForDocument:(NSDictionary *)state {}
- (void)requestViewControllerWithCompletionHandler:(void (^)(id))handler { if (handler) handler(nil); }
@end
