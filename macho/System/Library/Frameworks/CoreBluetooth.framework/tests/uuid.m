// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CoreBluetooth: UUID parsing and printing, the unsupported managers.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSData : NSObject
+ (id)dataWithBytes:(const void *)bytes length:(NSUInteger)length;
- (NSUInteger)length;
- (const void *)bytes;
@end
@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSObject (T)
- (NSString *)description;
@end
@interface NSUUID : NSObject
- (id)initWithUUIDString:(NSString *)string;
@end

@interface CBUUID : NSObject
+ (id)UUIDWithData:(id)data;
+ (id)UUIDWithNSUUID:(id)uuid;
+ (id)UUIDWithString:(id)string;
- (id)data;
- (id)_fullData;
- (NSString *)UUIDString;
@end

@interface CBManager : NSObject
+ (long)authorization;
- (long)state;
@end

@interface CBCentralManager : CBManager
- (id)initWithDelegate:(id)delegate queue:(id)queue;
- (BOOL)isScanning;
- (id)delegate;
- (id)retrievePeripheralsWithIdentifiers:(id)identifiers;
@end

@interface CBPeripheralManager : CBManager
- (id)init;
- (BOOL)isAdvertising;
@end

static void show(const char *what, NSString *s) { printf("%s %s\n", what, s ? [s UTF8String] : "(nil)"); }

int main(void)
{
    CBUUID *u16 = [CBUUID UUIDWithString:@"180D"];
    show("short", [u16 UUIDString]);
    printf("short data %lu full %lu\n", (unsigned long)[[u16 data] length], (unsigned long)[[u16 _fullData] length]);
    CBUUID *u32 = [CBUUID UUIDWithString:@"0000180d"];
    show("32", [u32 UUIDString]);
    CBUUID *full = [CBUUID UUIDWithString:@"0000180D-0000-1000-8000-00805F9B34FB"];
    show("full", [full UUIDString]);
    printf("equal %d %d %d\n", [u16 isEqual:full], [u32 isEqual:full], [u16 isEqual:@"x"]);
    printf("hash %d\n", [u16 hash] == [full hash]);
    show("bad length", [CBUUID UUIDWithString:@"123"] ? @"made" : nil);
    show("bad digits", [CBUUID UUIDWithString:@"zzzz"] ? @"made" : nil);
    show("lower", [[CBUUID UUIDWithString:@"ab-cd"] UUIDString]);
    unsigned char raw[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    show("data", [[CBUUID UUIDWithData:[NSData dataWithBytes:raw length:16]] UUIDString]);
    show("data 2", [[CBUUID UUIDWithData:[NSData dataWithBytes:raw length:2]] description]);
    show("data 3", [CBUUID UUIDWithData:[NSData dataWithBytes:raw length:3]] ? @"made" : nil);
    NSUUID *system = [[NSUUID alloc] initWithUUIDString:@"E621E1F8-C36C-495A-93FC-0C247A3E6E5F"];
    show("nsuuid", [[CBUUID UUIDWithNSUUID:system] UUIDString]);
    printf("copy %d\n", [u16 copy] == u16);

    printf("authorization %ld\n", [CBManager authorization]);
    CBCentralManager *central = [[CBCentralManager alloc] initWithDelegate:nil queue:nil];
    printf("central state %ld scanning %d delegate %d peripherals %lu\n", [central state], [central isScanning],
        [central delegate] == nil, (unsigned long)[[central retrievePeripheralsWithIdentifiers:nil] count]);
    CBPeripheralManager *peripheral = [[CBPeripheralManager alloc] init];
    printf("peripheral state %ld advertising %d\n", [peripheral state], [peripheral isAdvertising]);
    return 0;
}
