// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CoreTelephony: a Mac has no cellular radio (empty answers); the delegate is weak and the notifier is copied.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface CTCarrier : NSObject
@property (readonly) id carrierName, mobileCountryCode, mobileNetworkCode, isoCountryCode;
@property (readonly) BOOL allowsVOIP;
@end

@interface CTTelephonyNetworkInfo : NSObject
@property (readonly) id serviceSubscriberCellularProviders, serviceCurrentRadioAccessTechnology, dataServiceIdentifier;
@property (weak) id delegate;
@property (copy) void (^serviceSubscriberCellularProvidersDidUpdateNotifier)(void);
@property (readonly) id subscriberCellularProvider, currentRadioAccessTechnology;
@end

int main(void)
{
    CTTelephonyNetworkInfo *info = [CTTelephonyNetworkInfo new];
    CTCarrier *carrier = [CTCarrier new];
    printf("carrier %d %d %d %d %d\n", carrier.carrierName == nil, carrier.mobileCountryCode == nil, carrier.mobileNetworkCode == nil,
           carrier.isoCountryCode == nil, carrier.allowsVOIP);
    printf("info %d %d %d %d %d\n", info.serviceSubscriberCellularProviders == nil, info.serviceCurrentRadioAccessTechnology == nil,
           info.dataServiceIdentifier == nil, info.subscriberCellularProvider == nil, info.currentRadioAccessTechnology == nil);
    @autoreleasepool {
        id delegate = [NSObject new];
        info.delegate = delegate;
        printf("delegate %d\n", info.delegate == delegate);
    }
    printf("delegate after %d\n", info.delegate == nil);
    __block int calls = 0;
    info.serviceSubscriberCellularProvidersDidUpdateNotifier = ^{ calls++; };
    info.serviceSubscriberCellularProvidersDidUpdateNotifier();
    info.serviceSubscriberCellularProvidersDidUpdateNotifier = nil;
    printf("notifier %d %d\n", calls, info.serviceSubscriberCellularProvidersDidUpdateNotifier == nil);
    return 0;
}
