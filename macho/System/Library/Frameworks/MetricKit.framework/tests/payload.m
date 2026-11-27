// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// MetricKit: payloads and diagnostics are empty holders (empty JSON and dictionary); the manager is one object that takes subscribers and delivers nothing.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSData : NSObject
- (NSUInteger)length;
- (const void *)bytes;
@end
@interface NSDictionary (Count)
- (NSUInteger)count;
@end

@interface MXMetaData : NSObject
@property (readonly) NSString *applicationBuildVersion, *osVersion, *deviceType, *regionFormat;
+ (BOOL)supportsSecureCoding;
- (NSData *)JSONRepresentation;
- (NSDictionary *)dictionaryRepresentation;
@end
@interface MXCallStackTree : NSObject
+ (BOOL)supportsSecureCoding;
- (NSData *)JSONRepresentation;
@end
@interface MXDiagnostic : NSObject
@property (readonly) id metaData, applicationVersion;
+ (BOOL)supportsSecureCoding;
- (NSData *)JSONRepresentation;
- (NSDictionary *)dictionaryRepresentation;
@end
@interface MXCrashDiagnostic : MXDiagnostic
@property (readonly) id callStackTree, terminationReason, virtualMemoryRegionInfo, exceptionType, exceptionCode, signal, exceptionReason;
@end
@interface MXHangDiagnostic : MXDiagnostic
@property (readonly) id callStackTree, hangDuration;
@end
@interface MXCPUExceptionDiagnostic : MXDiagnostic
@property (readonly) id callStackTree, totalCPUTime, totalSampledTime;
@end
@interface MXDiskWriteExceptionDiagnostic : MXDiagnostic
@property (readonly) id callStackTree, totalWritesCaused;
@end
@interface MXDiagnosticPayload : NSObject
@property (readonly) id cpuExceptionDiagnostics, diskWriteExceptionDiagnostics, hangDiagnostics, crashDiagnostics, timeStampBegin, timeStampEnd;
+ (BOOL)supportsSecureCoding;
- (NSData *)JSONRepresentation;
- (NSDictionary *)dictionaryRepresentation;
@end
@interface MXMetricPayload : NSObject
@property (readonly) id timeStampBegin, timeStampEnd;
+ (BOOL)supportsSecureCoding;
- (NSData *)JSONRepresentation;
- (NSDictionary *)dictionaryRepresentation;
@end
@interface MXMetricManager : NSObject
@property (readonly) NSArray *pastPayloads, *pastDiagnosticPayloads;
+ (MXMetricManager *)sharedManager;
- (void)addSubscriber:(id)subscriber;
- (void)removeSubscriber:(id)subscriber;
@end

static void show(const char *name, NSData *json, NSDictionary *dictionary, BOOL secure)
{
    printf("%s json %lu %.2s dict %lu secure %d\n", name, (unsigned long)[json length], (const char *)[json bytes], (unsigned long)[dictionary count], secure);
}

int main(void)
{
    show("metadata", [[MXMetaData new] JSONRepresentation], [[MXMetaData new] dictionaryRepresentation], [MXMetaData supportsSecureCoding]);
    printf("stack %lu %d\n", (unsigned long)[[[MXCallStackTree new] JSONRepresentation] length], [MXCallStackTree supportsSecureCoding]);
    show("diagnostic", [[MXDiagnostic new] JSONRepresentation], [[MXDiagnostic new] dictionaryRepresentation], [MXDiagnostic supportsSecureCoding]);
    show("diagnostic payload", [[MXDiagnosticPayload new] JSONRepresentation], [[MXDiagnosticPayload new] dictionaryRepresentation], [MXDiagnosticPayload supportsSecureCoding]);
    show("metric payload", [[MXMetricPayload new] JSONRepresentation], [[MXMetricPayload new] dictionaryRepresentation], [MXMetricPayload supportsSecureCoding]);
    MXCrashDiagnostic *crash = [MXCrashDiagnostic new];
    printf("crash %d %d %d %d\n", crash.metaData == nil, crash.callStackTree == nil, crash.terminationReason == nil, crash.exceptionReason == nil);
    printf("others %d %d %d %d\n", [MXHangDiagnostic new].hangDuration == nil, [MXCPUExceptionDiagnostic new].totalCPUTime == nil, [MXDiskWriteExceptionDiagnostic new].totalWritesCaused == nil, [MXMetaData new].osVersion == nil);
    printf("payload %d %d\n", [MXDiagnosticPayload new].crashDiagnostics == nil, [MXMetricPayload new].timeStampBegin == nil);
    MXMetricManager *m = [MXMetricManager sharedManager];
    id subscriber = [NSObject new];
    [m addSubscriber:subscriber];
    [m removeSubscriber:subscriber];
    printf("manager %d %lu %lu\n", m == [MXMetricManager sharedManager], (unsigned long)[m.pastPayloads count], (unsigned long)[m.pastDiagnosticPayloads count]);
    return 0;
}
