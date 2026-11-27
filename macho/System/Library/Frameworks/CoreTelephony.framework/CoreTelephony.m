// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoreTelephony: the carrier and the network info of a Mac, which has no cellular radio (every answer is empty).

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
@end

@protocol CTTelephonyNetworkInfoDelegate;
@class CTCarrier;

@interface CTCarrier : NSObject
@property (readonly, strong) NSString *carrierName;
@property (readonly, strong) NSString *mobileCountryCode;
@property (readonly, strong) NSString *mobileNetworkCode;
@property (readonly, strong) NSString *isoCountryCode;
@property (readonly) BOOL allowsVOIP;
@end

@implementation CTCarrier
- (id)carrierName { return nil; }
- (id)mobileCountryCode { return nil; }
- (id)mobileNetworkCode { return nil; }
- (id)isoCountryCode { return nil; }
- (BOOL)allowsVOIP { return NO; }
@end

@interface CTTelephonyNetworkInfo : NSObject
{
    __weak id<CTTelephonyNetworkInfoDelegate> _delegate;
    id _notifier;
}
@property (readonly, strong) NSDictionary *serviceSubscriberCellularProviders;
@property (readonly, strong) NSDictionary *serviceCurrentRadioAccessTechnology;
@property (readonly, strong) NSString *dataServiceIdentifier;
@property (weak, nonatomic) id<CTTelephonyNetworkInfoDelegate> delegate;
@property (copy) void (^serviceSubscriberCellularProvidersDidUpdateNotifier)(void);
@property (readonly, strong) CTCarrier *subscriberCellularProvider;
@property (readonly, strong) NSString *currentRadioAccessTechnology;
@end

@implementation CTTelephonyNetworkInfo

- (void)dealloc
{
    [_notifier release];
    [super dealloc];
}

- (id)serviceSubscriberCellularProviders { return nil; }
- (id)serviceCurrentRadioAccessTechnology { return nil; }
- (id)dataServiceIdentifier { return nil; }

- (id<CTTelephonyNetworkInfoDelegate>)delegate { return _delegate; }
- (void)setDelegate:(id<CTTelephonyNetworkInfoDelegate>)delegate { _delegate = delegate; }

- (void (^)(void))serviceSubscriberCellularProvidersDidUpdateNotifier { return _notifier; }

- (void)setServiceSubscriberCellularProvidersDidUpdateNotifier:(void (^)(void))notifier
{
    if (_notifier != notifier) {
        [_notifier release];
        _notifier = [notifier copy];
    }
}

- (id)subscriberCellularProvider { return nil; }
- (id)currentRadioAccessTechnology { return nil; }

@end
