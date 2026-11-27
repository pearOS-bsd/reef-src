// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// Arbitration of audio routing between apps and devices (macOS 11 and later, for AirPods switching): one app, no contenders.
@interface AVAudioRoutingArbiter : NSObject
+ (AVAudioRoutingArbiter *)sharedRoutingArbiter;
- (void)beginArbitrationWithCategory:(NSInteger)category completionHandler:(void (^)(BOOL defaultDeviceChanged, NSError *error))handler;
- (void)leaveArbitration;
@end
@implementation AVAudioRoutingArbiter
+ (AVAudioRoutingArbiter *)sharedRoutingArbiter {
    static AVAudioRoutingArbiter *a;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ a = [[AVAudioRoutingArbiter alloc] init]; });
    return a;
}
- (void)beginArbitrationWithCategory:(NSInteger)category completionHandler:(void (^)(BOOL, NSError *))handler { if (handler) handler(NO, nil); }
- (void)leaveArbitration {}
@end
