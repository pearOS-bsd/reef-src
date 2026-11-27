// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Head tracking of headphones: no supported headphones are connected.
#import <Foundation/Foundation.h>

@interface CMHeadphoneMotionManager : NSObject
@property (weak) id delegate;
@property (readonly, getter=isDeviceMotionAvailable) BOOL deviceMotionAvailable;
@property (readonly, getter=isDeviceMotionActive) BOOL deviceMotionActive;
@property (readonly) id deviceMotion;
@end

@implementation CMHeadphoneMotionManager
+ (NSInteger)authorizationStatus { return 3; }   // CMAuthorizationStatusAuthorized
- (BOOL)isDeviceMotionAvailable { return NO; }
- (BOOL)isDeviceMotionActive { return NO; }
- (id)deviceMotion { return nil; }
- (void)startDeviceMotionUpdates {}
- (void)startDeviceMotionUpdatesToQueue:(NSOperationQueue *)queue withHandler:(id)handler {}
- (void)stopDeviceMotionUpdates {}
- (BOOL)isConnectionStatusActive { return NO; }
- (void)startConnectionStatusUpdates {}
- (void)stopConnectionStatusUpdates {}
@end
