// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CoreBluetooth: there is no Bluetooth stack, so the managers report an unsupported state; CBUUID is real.

#import <Foundation/Foundation.h>
#include <stdbool.h>
#include <string.h>
typedef unsigned char uuid_t[16];

// Dispatch queues are Objective-C objects (OS_OBJECT_USE_OBJC), which the libdispatch header of the build does not declare.
@protocol OS_dispatch_queue
@end
typedef NSObject<OS_dispatch_queue> *dispatch_queue_t;
extern struct dispatch_queue_s _dispatch_main_q;
static inline __attribute__((always_inline)) dispatch_queue_t dispatch_get_main_queue(void) { return (__bridge dispatch_queue_t)(void *)&_dispatch_main_q; }
void dispatch_async(dispatch_queue_t queue, void (^block)(void)) __attribute__((nothrow));
typedef id (*IMP)(id, SEL, ...);
static inline NSRange NSMakeRange(NSUInteger location, NSUInteger length) { NSRange r = { location, length }; return r; }

@interface NSData : NSObject
+ (id)dataWithBytes:(const void *)bytes length:(NSUInteger)length;
- (NSUInteger)length;
- (const void *)bytes __attribute__((objc_returns_inner_pointer));
- (id)copy;
- (BOOL)isEqualToData:(NSData *)other;
@end
@interface NSMutableData : NSData
+ (id)dataWithLength:(NSUInteger)length;
- (void *)mutableBytes __attribute__((objc_returns_inner_pointer));
@end
@interface NSMutableString : NSString
+ (id)string;
- (void)appendString:(NSString *)string;
- (void)appendFormat:(NSString *)format, ...;
@end
@interface NSString (CBUUID)
- (NSString *)stringByReplacingOccurrencesOfString:(NSString *)target withString:(NSString *)replacement;
- (NSString *)substringWithRange:(NSRange)range;
@end
@interface NSObject (CBMethod)
- (IMP)methodForSelector:(SEL)selector;
@end
@interface NSScanner : NSObject
+ (id)scannerWithString:(NSString *)string;
- (BOOL)scanHexInt:(unsigned *)value;
@end
@interface NSUUID : NSObject
- (void)getUUIDBytes:(unsigned char *)bytes;
@end

@interface CBManager : NSObject
@property (readonly) long state;
@property (class, readonly) long authorization;
- (void)_reportStateTo:(id)target selector:(SEL)selector queue:(dispatch_queue_t)queue;
@end

@implementation CBManager
+ (long)authorization { return 3; }
- (long)state { return 2; }
- (void)_reportStateTo:(id)target selector:(SEL)selector queue:(dispatch_queue_t)queue
{
    if ([target respondsToSelector:selector]) {
        __weak id weakTarget = target;
        dispatch_async(queue ?: dispatch_get_main_queue(), ^{
            id strongTarget = weakTarget;
            if (strongTarget) {
                IMP imp = [strongTarget methodForSelector:selector];
                ((void (*)(id, SEL, id))imp)(strongTarget, selector, self);
            }
        });
    }
}
@end

@interface CBCentralManager : CBManager
{
    __weak id _delegate;
}
@property (weak) id delegate;
@property (readonly) BOOL isScanning;
- (id)init;
- (id)initWithDelegate:(id)delegate queue:(id)queue;
- (id)initWithDelegate:(id)delegate queue:(id)queue options:(id)options;
- (void)scanForPeripheralsWithServices:(id)services options:(id)options;
- (void)stopScan;
- (id)retrievePeripheralsWithIdentifiers:(id)identifiers;
- (id)retrieveConnectedPeripheralsWithServices:(id)services;
- (void)connectPeripheral:(id)peripheral options:(id)options;
- (void)cancelPeripheralConnection:(id)peripheral;
@end

@implementation CBCentralManager
- (id)init { return [self initWithDelegate:nil queue:nil options:nil]; }
- (id)initWithDelegate:(id)delegate queue:(id)queue { return [self initWithDelegate:delegate queue:queue options:nil]; }
- (id)initWithDelegate:(id)delegate queue:(id)queue options:(id)options
{
    if ((self = [super init])) {
        _delegate = delegate;
        [self _reportStateTo:delegate selector:@selector(centralManagerDidUpdateState:) queue:queue];
    }
    return self;
}
- (BOOL)isScanning { return NO; }
- (void)scanForPeripheralsWithServices:(id)services options:(id)options { }
- (void)stopScan { }
- (id)retrievePeripheralsWithIdentifiers:(id)identifiers { return @[]; }
- (id)retrieveConnectedPeripheralsWithServices:(id)services { return @[]; }
- (void)connectPeripheral:(id)peripheral options:(id)options { }
- (void)cancelPeripheralConnection:(id)peripheral { }
@end

@interface CBPeripheralManager : CBManager
{
    __weak id _delegate;
}
@property (weak) id delegate;
@property (readonly) BOOL isAdvertising;
- (id)init;
- (id)initWithDelegate:(id)delegate queue:(id)queue;
- (id)initWithDelegate:(id)delegate queue:(id)queue options:(id)options;
- (void)startAdvertising:(id)data;
- (void)stopAdvertising;
- (void)addService:(id)service;
- (void)removeAllServices;
@end

@implementation CBPeripheralManager
- (id)init { return [self initWithDelegate:nil queue:nil options:nil]; }
- (id)initWithDelegate:(id)delegate queue:(id)queue { return [self initWithDelegate:delegate queue:queue options:nil]; }
- (id)initWithDelegate:(id)delegate queue:(id)queue options:(id)options
{
    if ((self = [super init])) {
        _delegate = delegate;
        [self _reportStateTo:delegate selector:@selector(peripheralManagerDidUpdateState:) queue:queue];
    }
    return self;
}
- (BOOL)isAdvertising { return NO; }
- (void)startAdvertising:(id)data { }
- (void)stopAdvertising { }
- (void)addService:(id)service { }
- (void)removeAllServices { }
@end

static const uint8_t baseUUID[16] __attribute__((aligned(16))) = { 0, 0, 0, 0, 0, 0, 0x10, 0, 0x80, 0, 0, 0x80, 0x5f, 0x9b, 0x34, 0xfb };

@interface CBUUID : NSObject <NSCopying>
{
    NSData *_data;
}
@property (readonly, nonatomic) NSData *data;
@property (readonly) NSString *UUIDString;
+ (id)UUIDWithData:(id)data;
+ (id)UUIDWithNSUUID:(id)uuid;
+ (id)UUIDWithString:(id)string;
- (id)_fullData;
@end

@implementation CBUUID
+ (id)UUIDWithData:(id)data
{
    if ([data length] != 2 && [data length] != 4 && [data length] != 16)
        return nil;
    CBUUID *uuid = [[self alloc] init];
    uuid->_data = [data copy];
    return uuid;
}

+ (id)UUIDWithNSUUID:(id)uuid
{
    uuid_t bytes;
    [uuid getUUIDBytes:bytes];
    return [self UUIDWithData:[NSData dataWithBytes:bytes length:16]];
}

+ (id)UUIDWithString:(id)string
{
    NSString *digits = [string stringByReplacingOccurrencesOfString:@"-" withString:@""];
    if ([digits length] != 4 && [digits length] != 8 && [digits length] != 32)
        return nil;
    NSMutableData *data = [NSMutableData dataWithLength:[digits length] / 2];
    uint8_t *bytes = [data mutableBytes];
    for (NSUInteger i = 0; i < [digits length] / 2; i++) {
        unsigned value = 0;
        NSScanner *scanner = [NSScanner scannerWithString:[digits substringWithRange:NSMakeRange(i * 2, 2)]];
        if (![scanner scanHexInt:&value])
            return nil;
        bytes[i] = (uint8_t)value;
    }
    return [self UUIDWithData:data];
}

- (id)copyWithZone:(NSZone *)zone { return self; }
- (NSData *)data { return _data; }

- (id)_fullData
{
    if ([_data length] == 16)
        return _data;
    uint8_t full[16];
    memcpy(full, baseUUID, 16);
    memcpy(full + 4 - [_data length], [_data bytes], [_data length]);
    return [NSData dataWithBytes:full length:16];
}

- (NSString *)UUIDString
{
    const uint8_t *bytes = [_data bytes];
    NSMutableString *string = [NSMutableString string];
    for (NSUInteger i = 0; i < [_data length]; i++) {
        if ([_data length] == 16) {
            switch (i) {
            case 4: case 6: case 8: case 10:
                [string appendString:@"-"];
            }
        }
        [string appendFormat:@"%02X", bytes[i]];
    }
    return string;
}

- (BOOL)isEqual:(id)object
{
    bool equal = false;
    if ([object isKindOfClass:[CBUUID class]])
        equal = [[self _fullData] isEqualToData:[object _fullData]];
    return equal;
}
- (NSUInteger)hash { return [[self _fullData] hash]; }
- (NSString *)description { return [self UUIDString]; }
@end

NSString *const CBAdvertisementDataLocalNameKey = @"kCBAdvDataLocalName";
NSString *const CBAdvertisementDataManufacturerDataKey = @"kCBAdvDataManufacturerData";
NSString *const CBAdvertisementDataOverflowServiceUUIDsKey = @"kCBAdvDataOverflowServiceUUIDs";
NSString *const CBAdvertisementDataServiceDataKey = @"kCBAdvDataServiceData";
NSString *const CBAdvertisementDataServiceUUIDsKey = @"kCBAdvDataServiceUUIDs";
NSString *const CBAdvertisementDataTxPowerLevelKey = @"kCBAdvDataTxPowerLevel";
NSString *const CBAdvertisementDataIsConnectable = @"kCBAdvDataIsConnectable";
NSString *const CBCentralManagerScanOptionAllowDuplicatesKey = @"kCBScanOptionAllowDuplicates";
NSString *const CBCentralManagerOptionShowPowerAlertKey = @"kCBInitOptionShowPowerAlert";
