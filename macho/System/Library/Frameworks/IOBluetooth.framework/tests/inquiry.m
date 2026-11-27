// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// IOBluetooth: there is no controller; an inquiry can neither start nor stop and finds nothing.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface IOBluetoothHostController : NSObject
+ (id)defaultController;
@end
@interface IOBluetoothDevice : NSObject
+ (NSArray *)pairedDevices;
+ (id)deviceWithAddressString:(NSString *)address;
@end
@interface IOBluetoothDeviceInquiry : NSObject
@property (weak) id delegate;
@property unsigned char inquiryLength;
@property BOOL updateNewDeviceNames;
+ (id)inquiryWithDelegate:(id)delegate;
- (id)initWithDelegate:(id)delegate;
- (int)start;
- (int)stop;
- (NSArray *)foundDevices;
- (void)clearFoundDevices;
@end
void IOBluetoothPreferenceSetControllerPowerState(int state);
int IOBluetoothPreferenceGetControllerPowerState(void);

int main(void)
{
    printf("controller %d paired %lu device %d\n", [IOBluetoothHostController defaultController] == nil, (unsigned long)[[IOBluetoothDevice pairedDevices] count],
           [IOBluetoothDevice deviceWithAddressString:@"00-11-22-33-44-55"] == nil);
    id delegate = [NSObject new];
    IOBluetoothDeviceInquiry *inquiry = [IOBluetoothDeviceInquiry inquiryWithDelegate:delegate];
    printf("inquiry %d delegate %d\n", inquiry != nil, inquiry.delegate == delegate);
    inquiry.inquiryLength = 7; inquiry.updateNewDeviceNames = YES;
    printf("props %d %d\n", inquiry.inquiryLength, inquiry.updateNewDeviceNames);
    printf("start %#x stop %#x found %lu\n", [inquiry start], [inquiry stop], (unsigned long)[[inquiry foundDevices] count]);
    [inquiry clearFoundDevices];
    IOBluetoothPreferenceSetControllerPowerState(1);
    printf("power %d\n", IOBluetoothPreferenceGetControllerPowerState());
    return 0;
}
