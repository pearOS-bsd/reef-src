// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// A group of writer inputs that are alternatives to each other (AVMediaSelectionGroup on Apple's side)
@interface AVAssetWriterInputGroup : NSObject
@property (readonly) NSArray *inputs;
@property (readonly) id defaultInput;
@end
@implementation AVAssetWriterInputGroup
+ (instancetype)assetWriterInputGroupWithInputs:(NSArray *)inputs defaultInput:(id)input { return [[self alloc] initWithInputs:inputs defaultInput:input]; }
- (instancetype)initWithInputs:(NSArray *)inputs defaultInput:(id)input {
    if ((self = [super init])) {
        _inputs = [inputs copy];
        _defaultInput = input;
    }
    return self;
}
@end

// The clock a player runs on: NULL is the default (host time) clock
#import <objc/runtime.h>
@interface NSObject (ReefPlayerClock)
@end
static const void *SourceClockKey = &SourceClockKey;
static id playerSourceClock(id self, SEL _cmd) { return objc_getAssociatedObject(self, SourceClockKey); }
static void playerSetSourceClock(id self, SEL _cmd, id clock) { objc_setAssociatedObject(self, SourceClockKey, clock, OBJC_ASSOCIATION_RETAIN); }
static void playerSetWaits(id self, SEL _cmd, BOOL value) { objc_setAssociatedObject(self, &SourceClockKey + 1, @(value), OBJC_ASSOCIATION_RETAIN); }
static BOOL playerWaits(id self, SEL _cmd) { NSNumber *n = objc_getAssociatedObject(self, &SourceClockKey + 1); return n ? n.boolValue : YES; }

__attribute__((constructor)) static void addPlayerClock(void) {
    Class player = objc_getClass("AVPlayer");
    if (!player) return;
    class_addMethod(player, sel_registerName("sourceClock"), (IMP)playerSourceClock, "^{OpaqueCMClock=}@:");
    class_addMethod(player, sel_registerName("setSourceClock:"), (IMP)playerSetSourceClock, "v@:^{OpaqueCMClock=}");
    class_addMethod(player, sel_registerName("automaticallyWaitsToMinimizeStalling"), (IMP)playerWaits, "c@:");
    class_addMethod(player, sel_registerName("setAutomaticallyWaitsToMinimizeStalling:"), (IMP)playerSetWaits, "v@:c");
}
