// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// GameController: the class methods of GCController and the notification names.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface GCController : NSObject
+ (NSArray *)controllers;
+ (id)current;
+ (BOOL)shouldMonitorBackgroundEvents;
+ (void)setShouldMonitorBackgroundEvents:(BOOL)value;
+ (void)startWirelessControllerDiscoveryWithCompletionHandler:(void (^)(void))handler;
+ (void)stopWirelessControllerDiscovery;
@end
extern NSString *const GCControllerDidConnectNotification;
extern NSString *const GCHapticsLocalityLeftTrigger;
extern const float GCHapticDurationInfinite;

int main(void)
{
    printf("controllers %lu current %d\n", (unsigned long)[[GCController controllers] count], [GCController current] == nil);
    printf("monitor %d", [GCController shouldMonitorBackgroundEvents]);
    [GCController setShouldMonitorBackgroundEvents:YES];
    printf(" -> %d\n", [GCController shouldMonitorBackgroundEvents]);
    [GCController startWirelessControllerDiscoveryWithCompletionHandler:nil];
    [GCController stopWirelessControllerDiscovery];
    printf("%lu %lu %f\n", (unsigned long)[GCControllerDidConnectNotification length], (unsigned long)[GCHapticsLocalityLeftTrigger length], GCHapticDurationInfinite);
    return 0;
}
