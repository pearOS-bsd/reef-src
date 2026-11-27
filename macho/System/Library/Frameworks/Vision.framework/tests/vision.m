// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Vision: rectangle helpers, request defaults and copies, barcode symbologies, observations and the failing image request handler.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSUUID : NSObject
@end
@interface NSIndexSet : NSObject
- (NSUInteger)count;
@end
@interface NSError : NSObject
- (NSString *)domain;
- (NSInteger)code;
- (NSString *)localizedDescription;
@end
@interface NSURL : NSObject
+ (NSURL *)fileURLWithPath:(NSString *)path;
@end
@interface NSData : NSObject
+ (id)data;
@end
@interface NSObject (T)
- (NSUInteger)count;
@end
@interface NSArray (T)
+ (id)arrayWithObjects:(const id *)objects count:(NSUInteger)count;
@end

static CGRect CGRectMake(double x, double y, double width, double height)
{
    CGRect rect = {{x, y}, {width, height}};
    return rect;
}

static CGPoint CGPointMake(double x, double y)
{
    CGPoint point = {x, y};
    return point;
}

extern CGRect VNImageRectForNormalizedRect(CGRect rect, size_t width, size_t height);
extern CGRect VNNormalizedRectForImageRect(CGRect rect, size_t width, size_t height);
extern CGPoint VNImagePointForNormalizedPoint(CGPoint point, size_t width, size_t height);
extern NSString *const VNErrorDomain;
extern NSString *const VNBarcodeSymbologyQR;
extern NSString *const VNBarcodeSymbologyITF14;

@interface VNObservation : NSObject <NSCopying>
@property (readonly, copy) NSUUID *uuid;
@property (readonly) float confidence;
+ (BOOL)supportsSecureCoding;
@end
@interface VNDetectedObjectObservation : VNObservation
@property (readonly) CGRect boundingBox;
@end
@interface VNSaliencyImageObservation : VNObservation
@property (readonly) NSArray *salientObjects;
@end
@interface VNInstanceMaskObservation : VNObservation
@property (readonly, copy) NSIndexSet *allInstances;
@property (readonly) void *instanceMask;
@end
@interface VNBarcodeObservation : VNDetectedObjectObservation
@property (readonly) NSString *symbology;
@property (readonly) NSString *payloadStringValue;
@property (readonly) NSData *payloadData;
@end
@interface VNRequest : NSObject <NSCopying>
@property (readonly, copy) NSArray *results;
@property (readonly, copy) void (^completionHandler)(id, NSError *);
@property unsigned long revision;
@property BOOL preferBackgroundProcessing;
@property BOOL usesCPUOnly;
- (id)initWithCompletionHandler:(void (^)(id, NSError *))handler;
- (void)cancel;
@end
@interface VNImageBasedRequest : VNRequest
@property CGRect regionOfInterest;
@end
@interface VNDetectFaceRectanglesRequest : VNImageBasedRequest
@end
@interface VNDetectBarcodesRequest : VNImageBasedRequest
@property (copy) NSArray *symbologies;
+ (NSArray *)supportedSymbologies;
- (NSArray *)supportedSymbologiesAndReturnError:(NSError **)error;
@end
@interface VNImageRequestHandler : NSObject
- (id)initWithURL:(NSURL *)url options:(NSDictionary *)options;
- (id)initWithData:(NSData *)data options:(NSDictionary *)options;
- (BOOL)performRequests:(NSArray *)requests error:(NSError **)error;
@end

static void rect(const char *name, CGRect r)
{
    printf("%s %g %g %g %g\n", name, r.origin.x, r.origin.y, r.size.width, r.size.height);
}

int main(void)
{
    rect("image", VNImageRectForNormalizedRect(CGRectMake(0.25, 0.5, 0.5, 0.125), 640, 480));
    rect("norm", VNNormalizedRectForImageRect(CGRectMake(160, 240, 320, 60), 640, 480));
    rect("zero", VNNormalizedRectForImageRect(CGRectMake(160, 240, 320, 60), 0, 480));
    rect("zero2", VNNormalizedRectForImageRect(CGRectMake(160, 240, 320, 60), 640, 0));
    CGPoint p = VNImagePointForNormalizedPoint(CGPointMake(0.5, 0.25), 800, 600);
    printf("point %g %g\n", p.x, p.y);
    printf("domain %s qr %s itf %s\n", [VNErrorDomain UTF8String], [VNBarcodeSymbologyQR UTF8String], [VNBarcodeSymbologyITF14 UTF8String]);

    VNObservation *o = [VNObservation new];
    VNObservation *o2 = [VNObservation new];
    printf("observation %d %g %d copy %d coder %d\n", o.uuid != nil, o.confidence, [o.uuid isEqual:o2.uuid], [o copy] == o, [VNObservation supportsSecureCoding]);
    VNDetectedObjectObservation *d = [VNDetectedObjectObservation new];
    rect("box", d.boundingBox);
    printf("salient %d\n", [VNSaliencyImageObservation new].salientObjects == nil);
    VNInstanceMaskObservation *m = [VNInstanceMaskObservation new];
    printf("mask %d %lu\n", m.instanceMask == NULL, (unsigned long)[m.allInstances count]);
    VNBarcodeObservation *b = [VNBarcodeObservation new];
    printf("barcode %s %d %d\n", [b.symbology UTF8String], b.payloadStringValue == nil, b.payloadData == nil);

    VNRequest *r = [VNRequest new];
    printf("request %lu %d %d %d %d\n", r.revision, r.preferBackgroundProcessing, r.usesCPUOnly, r.completionHandler == nil, r.results == nil);
    r.revision = 3; r.preferBackgroundProcessing = YES; r.usesCPUOnly = YES;
    VNRequest *c = [r copy];
    printf("copy %d %lu %d %d\n", c != r, c.revision, c.preferBackgroundProcessing, c.usesCPUOnly);
    [r cancel];
    VNDetectFaceRectanglesRequest *face = [[VNDetectFaceRectanglesRequest alloc] initWithCompletionHandler:^(id q, NSError *e) { }];
    rect("roi", face.regionOfInterest);
    face.regionOfInterest = CGRectMake(0.1, 0.2, 0.3, 0.4);
    VNDetectFaceRectanglesRequest *fc = [face copy];
    printf("face %d %lu %d\n", [fc isKindOfClass:[VNDetectFaceRectanglesRequest class]], fc.revision, fc.completionHandler != nil);
    rect("roi2", face.regionOfInterest);
    rect("roi3", fc.regionOfInterest);

    VNDetectBarcodesRequest *bc = [VNDetectBarcodesRequest new];
    printf("symbologies %lu %d\n", (unsigned long)[bc.symbologies count], [bc.symbologies isEqual:[VNDetectBarcodesRequest supportedSymbologies]]);
    for (NSString *s in [bc supportedSymbologiesAndReturnError:NULL])
        printf(" %s", [s UTF8String]);
    printf("\n");
    bc.symbologies = @[VNBarcodeSymbologyQR];
    printf("set %lu %d\n", (unsigned long)[bc.symbologies count], bc.results == nil);

    __block int calls = 0;
    VNRequest *a1 = [[VNRequest alloc] initWithCompletionHandler:^(id q, NSError *e) {
        calls++;
        printf("handler %d %s %ld %s\n", q != nil, [[e domain] UTF8String], (long)[e code], [[e localizedDescription] UTF8String]);
    }];
    VNRequest *a2 = [VNRequest new];
    VNImageRequestHandler *h = [[VNImageRequestHandler alloc] initWithURL:[NSURL fileURLWithPath:@"/tmp/x.png"] options:nil];
    NSError *error = nil;
    BOOL ok = [h performRequests:@[a1, a2, a1] error:&error];
    printf("perform %d %d %d %ld\n", ok, calls, error != nil, (long)[error code]);
    printf("noerror %d\n", [[[VNImageRequestHandler alloc] initWithData:[NSData data] options:nil] performRequests:@[] error:NULL]);
    return 0;
}
