// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MetricKit: payload, diagnostic and metadata classes as plain holders; the manager collects subscribers and never delivers anything.

#import <Foundation/Foundation.h>

typedef long dispatch_once_t;
extern void dispatch_once(dispatch_once_t *predicate, void (^block)(void));
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, void (^block)(void))
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once

@interface NSObject (ManualRetain)
- (id)retain;
@end

@interface NSString (Encoding)
- (NSData *)dataUsingEncoding:(NSUInteger)encoding;
@end

@interface NSHashTable : NSObject
+ (id)weakObjectsHashTable;
- (void)addObject:(id)object;
- (void)removeObject:(id)object;
@end

@class NSMeasurement, NSDate;

@interface MXMetaData : NSObject <NSSecureCoding>
{
    NSString *_applicationBuildVersion;
    NSString *_osVersion;
    NSString *_deviceType;
    NSString *_regionFormat;
}
@property (readonly) NSString *applicationBuildVersion;
@property (readonly) NSString *osVersion;
@property (readonly) NSString *deviceType;
@property (readonly) NSString *regionFormat;
@property (class, readonly) BOOL supportsSecureCoding;
- (NSData *)JSONRepresentation;
- (NSDictionary *)dictionaryRepresentation;
@end

@implementation MXMetaData

+ (BOOL)supportsSecureCoding { return YES; }
- (id)initWithCoder:(NSCoder *)coder { return [self init]; }
- (void)encodeWithCoder:(NSCoder *)coder { }
- (NSData *)JSONRepresentation { return [@"{}" dataUsingEncoding:4]; }
- (NSDictionary *)dictionaryRepresentation { return [NSDictionary dictionary]; }

@end

@interface MXCallStackTree : NSObject <NSSecureCoding>
@property (class, readonly) BOOL supportsSecureCoding;
- (NSData *)JSONRepresentation;
@end

@implementation MXCallStackTree

+ (BOOL)supportsSecureCoding { return YES; }
- (id)initWithCoder:(NSCoder *)coder { return [self init]; }
- (void)encodeWithCoder:(NSCoder *)coder { }
- (NSData *)JSONRepresentation { return [@"{}" dataUsingEncoding:4]; }

@end

@interface MXDiagnostic : NSObject <NSSecureCoding>
{
    MXMetaData *_metaData;
    NSString *_applicationVersion;
}
@property (readonly, retain) MXMetaData *metaData;
@property (readonly, retain) NSString *applicationVersion;
@property (class, readonly) BOOL supportsSecureCoding;
- (NSData *)JSONRepresentation;
- (NSDictionary *)dictionaryRepresentation;
@end

@implementation MXDiagnostic

+ (BOOL)supportsSecureCoding { return YES; }
- (id)initWithCoder:(NSCoder *)coder { return [self init]; }
- (void)encodeWithCoder:(NSCoder *)coder { }
- (NSData *)JSONRepresentation { return [@"{}" dataUsingEncoding:4]; }
- (NSDictionary *)dictionaryRepresentation { return [NSDictionary dictionary]; }

@end

@interface MXCrashDiagnostic : MXDiagnostic
{
    MXCallStackTree *_callStackTree;
    NSString *_terminationReason;
    NSString *_virtualMemoryRegionInfo;
    NSNumber *_exceptionType;
    NSNumber *_exceptionCode;
    NSNumber *_signal;
    id _exceptionReason;
}
@property (readonly, retain) MXCallStackTree *callStackTree;
@property (readonly, retain) NSString *terminationReason;
@property (readonly, retain) NSString *virtualMemoryRegionInfo;
@property (readonly, retain) NSNumber *exceptionType;
@property (readonly, retain) NSNumber *exceptionCode;
@property (readonly, retain) NSNumber *signal;
@property (readonly, retain) id exceptionReason;
@end

@implementation MXCrashDiagnostic
@end

@interface MXHangDiagnostic : MXDiagnostic
{
    MXCallStackTree *_callStackTree;
    NSMeasurement *_hangDuration;
}
@property (readonly, retain) MXCallStackTree *callStackTree;
@property (readonly, retain) NSMeasurement *hangDuration;
@end

@implementation MXHangDiagnostic
@end

@interface MXCPUExceptionDiagnostic : MXDiagnostic
{
    MXCallStackTree *_callStackTree;
    NSMeasurement *_totalCPUTime;
    NSMeasurement *_totalSampledTime;
}
@property (readonly, retain) MXCallStackTree *callStackTree;
@property (readonly, retain) NSMeasurement *totalCPUTime;
@property (readonly, retain) NSMeasurement *totalSampledTime;
@end

@implementation MXCPUExceptionDiagnostic
@end

@interface MXDiskWriteExceptionDiagnostic : MXDiagnostic
{
    MXCallStackTree *_callStackTree;
    NSMeasurement *_totalWritesCaused;
}
@property (readonly, retain) MXCallStackTree *callStackTree;
@property (readonly, retain) NSMeasurement *totalWritesCaused;
@end

@implementation MXDiskWriteExceptionDiagnostic
@end

@interface MXDiagnosticPayload : NSObject <NSSecureCoding>
{
    NSArray *_cpuExceptionDiagnostics;
    NSArray *_diskWriteExceptionDiagnostics;
    NSArray *_hangDiagnostics;
    NSArray *_crashDiagnostics;
    NSDate *_timeStampBegin;
    NSDate *_timeStampEnd;
}
@property (readonly) NSArray *cpuExceptionDiagnostics;
@property (readonly) NSArray *diskWriteExceptionDiagnostics;
@property (readonly) NSArray *hangDiagnostics;
@property (readonly) NSArray *crashDiagnostics;
@property (readonly) NSDate *timeStampBegin;
@property (readonly) NSDate *timeStampEnd;
@property (class, readonly) BOOL supportsSecureCoding;
- (NSData *)JSONRepresentation;
- (NSDictionary *)dictionaryRepresentation;
@end

@implementation MXDiagnosticPayload

+ (BOOL)supportsSecureCoding { return YES; }
- (id)initWithCoder:(NSCoder *)coder { return [self init]; }
- (void)encodeWithCoder:(NSCoder *)coder { }
- (NSData *)JSONRepresentation { return [@"{}" dataUsingEncoding:4]; }
- (NSDictionary *)dictionaryRepresentation { return [NSDictionary dictionary]; }

@end

@interface MXMetricPayload : NSObject <NSSecureCoding>
{
    NSDate *_timeStampBegin;
    NSDate *_timeStampEnd;
}
@property (readonly) NSDate *timeStampBegin;
@property (readonly) NSDate *timeStampEnd;
@property (class, readonly) BOOL supportsSecureCoding;
- (NSData *)JSONRepresentation;
- (NSDictionary *)dictionaryRepresentation;
@end

@implementation MXMetricPayload

+ (BOOL)supportsSecureCoding { return YES; }
- (id)initWithCoder:(NSCoder *)coder { return [self init]; }
- (void)encodeWithCoder:(NSCoder *)coder { }
- (NSData *)JSONRepresentation { return [@"{}" dataUsingEncoding:4]; }
- (NSDictionary *)dictionaryRepresentation { return [NSDictionary dictionary]; }

@end

@interface MXMetricManager : NSObject
{
    NSHashTable *_subscribers;
}
@property (readonly) NSArray *pastPayloads;
@property (readonly) NSArray *pastDiagnosticPayloads;
@property (class, readonly) MXMetricManager *sharedManager;
- (void)addSubscriber:(id)subscriber;
- (void)removeSubscriber:(id)subscriber;
@end

@implementation MXMetricManager

+ (MXMetricManager *)sharedManager
{
    static MXMetricManager *manager;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        manager = [[MXMetricManager alloc] init];
    });
    return manager;
}

- (id)init
{
    self = [super init];
    if (self)
        _subscribers = [[NSHashTable weakObjectsHashTable] retain];
    return self;
}

- (NSArray *)pastPayloads { return [NSArray array]; }
- (NSArray *)pastDiagnosticPayloads { return [NSArray array]; }

- (void)addSubscriber:(id)subscriber
{
    @synchronized (self) {
        [_subscribers addObject:subscriber];
    }
}

- (void)removeSubscriber:(id)subscriber
{
    @synchronized (self) {
        [_subscribers removeObject:subscriber];
    }
}

@end
