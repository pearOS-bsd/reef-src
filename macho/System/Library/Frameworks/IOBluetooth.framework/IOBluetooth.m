// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// IOBluetooth: no Bluetooth controller is present, so there are no devices, no controller and no inquiry results.

#import <Foundation/Foundation.h>

@interface NSData : NSObject
@end

@interface IOBluetoothHostController : NSObject
+ (id)defaultController;
@end

@implementation IOBluetoothHostController

+ (id)defaultController { return nil; }

@end

@interface IOBluetoothDevice : NSObject
+ (NSArray *)pairedDevices;
+ (id)deviceWithAddressString:(NSString *)address;
@end

@implementation IOBluetoothDevice

+ (NSArray *)pairedDevices { return @[]; }
+ (id)deviceWithAddressString:(NSString *)address { return nil; }

@end

@interface IOBluetoothDeviceInquiry : NSObject
{
    unsigned char _inquiryLength;
    BOOL _updateNewDeviceNames;
    __weak id _delegate;
}
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

@implementation IOBluetoothDeviceInquiry

+ (id)inquiryWithDelegate:(id)delegate
{
    return [[self alloc] initWithDelegate:delegate];
}

- (id)initWithDelegate:(id)delegate
{
    self = [super init];
    if (self)
        _delegate = delegate;
    return self;
}

- (int)start { return 0xe00002c0; }
- (int)stop { return 0xe00002e2; }
- (NSArray *)foundDevices { return @[]; }
- (void)clearFoundDevices { }

@end

@interface IOBluetoothL2CAPChannel : NSObject
@end

@implementation IOBluetoothL2CAPChannel
@end

@interface IOBluetoothRFCOMMChannel : NSObject
@end

@implementation IOBluetoothRFCOMMChannel
@end

@interface IOBluetoothSDPServiceRecord : NSObject
@end

@implementation IOBluetoothSDPServiceRecord
@end

@interface IOBluetoothSDPUUID : NSData
@end

@implementation IOBluetoothSDPUUID
@end

void IOBluetoothPreferenceSetControllerPowerState(int state)
{
}

int IOBluetoothPreferenceGetControllerPowerState(void)
{
    return 0;
}
