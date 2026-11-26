// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GameController.

#import <Foundation/Foundation.h>
#import <dispatch/dispatch.h>


@interface GCController : NSObject
@property (class) BOOL shouldMonitorBackgroundEvents;
+ (id)controllers;
+ (id)current;
+ (void)startWirelessControllerDiscoveryWithCompletionHandler:(void (^)(void))a0;
+ (void)stopWirelessControllerDiscovery;
@end

static BOOL monitorsBackground;

@implementation GCController
+ (id)controllers
{
    return @[];
}

+ (id)current
{
    return nil;
}

+ (BOOL)shouldMonitorBackgroundEvents
{
    return monitorsBackground;
}

+ (void)setShouldMonitorBackgroundEvents:(BOOL)a0
{
    monitorsBackground = a0;
}

+ (void)startWirelessControllerDiscoveryWithCompletionHandler:(void (^)(void))a0
{
    if (a0)
        dispatch_async(dispatch_get_main_queue(), a0);
}

+ (void)stopWirelessControllerDiscovery
{

}

@end

@interface GCPhysicalInputProfile : NSObject

@end

@implementation GCPhysicalInputProfile
@end

@interface GCExtendedGamepad : GCPhysicalInputProfile

@end

@implementation GCExtendedGamepad
@end

@interface GCDualShockGamepad : GCExtendedGamepad

@end

@implementation GCDualShockGamepad
@end

@interface GCDualSenseGamepad : GCExtendedGamepad

@end

@implementation GCDualSenseGamepad
@end

@interface GCXboxGamepad : GCExtendedGamepad

@end

@implementation GCXboxGamepad
@end

const float GCHapticDurationInfinite = __builtin_inff();
NSString *const GCControllerDidConnectNotification = @"GCControllerDidConnectNotification";
NSString *const GCControllerDidDisconnectNotification = @"GCControllerDidDisconnectNotification";
NSString *const GCControllerDidBecomeCurrentNotification = @"GCControllerDidBecomeCurrentNotification";
NSString *const GCControllerDidStopBeingCurrentNotification = @"GCControllerDidStopBeingCurrentNotification";
NSString *const GCHapticsLocalityDefault = @"Default";
NSString *const GCHapticsLocalityAll = @"All";
NSString *const GCHapticsLocalityHandles = @"Handles";
NSString *const GCHapticsLocalityLeftHandle = @"Left Handle";
NSString *const GCHapticsLocalityRightHandle = @"Right Handle";
NSString *const GCHapticsLocalityTriggers = @"Triggers";
NSString *const GCHapticsLocalityLeftTrigger = @"Left Trigger";
NSString *const GCHapticsLocalityRightTrigger = @"Right Trigger";
