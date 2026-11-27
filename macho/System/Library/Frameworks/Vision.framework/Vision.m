// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Vision: requests, observations and the image request handler; the handler reports that image analysis is not available.

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
@end

extern const CGRect CGRectZero;
extern NSString *const NSLocalizedDescriptionKey;

static inline CGRect CGRectMake(double x, double y, double width, double height)
{
    CGRect rect = {{x, y}, {width, height}};
    return rect;
}

static inline CGPoint CGPointMake(double x, double y)
{
    CGPoint point = {x, y};
    return point;
}

NSString *const VNErrorDomain = @"com.apple.vis";

typedef void (^VNRequestCompletionHandler)(id request, NSError *error);

CGRect VNImageRectForNormalizedRect(CGRect rect, size_t width, size_t height)
{
    return CGRectMake(rect.origin.x * width, rect.origin.y * height, rect.size.width * width, rect.size.height * height);
}

CGRect VNNormalizedRectForImageRect(CGRect rect, size_t width, size_t height)
{
    if ((width != 0) & (height != 0))
        return CGRectMake(rect.origin.x / width, rect.origin.y / height, rect.size.width / width, rect.size.height / height);
    return CGRectZero;
}

CGPoint VNImagePointForNormalizedPoint(CGPoint point, size_t width, size_t height)
{
    return CGPointMake(point.x * width, point.y * height);
}

@interface VNObservation : NSObject <NSCopying, NSSecureCoding>
{
    float _confidence;
    NSUUID *_uuid;
}
@property (readonly, copy) NSUUID *uuid;
@property (readonly) float confidence;
@end

@implementation VNObservation

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)init
{
    self = [super init];
    if (self)
        _uuid = [[NSUUID alloc] init];
    return self;
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

- (void)dealloc
{
    [_uuid release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

@end

@interface VNDetectedObjectObservation : VNObservation
{
    CGRect _boundingBox;
}
@property (readonly) CGRect boundingBox;
@end

@implementation VNDetectedObjectObservation
@end

@interface VNRectangleObservation : VNDetectedObjectObservation
@end

@implementation VNRectangleObservation
@end

@interface VNPixelBufferObservation : VNObservation
@end

@implementation VNPixelBufferObservation
@end

@interface VNSaliencyImageObservation : VNPixelBufferObservation
{
    NSArray *_salientObjects;
}
@property (readonly) NSArray *salientObjects;
@end

@implementation VNSaliencyImageObservation
@synthesize salientObjects = _salientObjects;
@end

@interface VNInstanceMaskObservation : VNObservation
@property (readonly) struct __CVBuffer *instanceMask;
@property (readonly, copy) NSIndexSet *allInstances;
@end

@implementation VNInstanceMaskObservation

- (struct __CVBuffer *)instanceMask
{
    return 0;
}

- (NSIndexSet *)allInstances
{
    return [NSIndexSet indexSet];
}

@end

@interface VNRequest : NSObject <NSCopying>
{
    BOOL _preferBackgroundProcessing;
    BOOL _usesCPUOnly;
    VNRequestCompletionHandler _completionHandler;
    unsigned long _revision;
}
@property (readonly, copy) NSArray *results;
@property (readonly, copy) VNRequestCompletionHandler completionHandler;
@property unsigned long revision;
@property BOOL preferBackgroundProcessing;
@property BOOL usesCPUOnly;
- (id)initWithCompletionHandler:(VNRequestCompletionHandler)completionHandler;
- (void)cancel;
@end

@implementation VNRequest
@synthesize revision = _revision;
@synthesize preferBackgroundProcessing = _preferBackgroundProcessing;
@synthesize usesCPUOnly = _usesCPUOnly;

- (id)init
{
    return [self initWithCompletionHandler:nil];
}

- (id)initWithCompletionHandler:(VNRequestCompletionHandler)completionHandler
{
    self = [super init];
    if (self) {
        _completionHandler = [completionHandler copy];
        _revision = 1;
    }
    return self;
}

- (void)dealloc
{
    [_completionHandler release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    VNRequest *copy = [[[self class] allocWithZone:zone] initWithCompletionHandler:_completionHandler];
    [copy setRevision:_revision];
    [copy setPreferBackgroundProcessing:_preferBackgroundProcessing];
    [copy setUsesCPUOnly:_usesCPUOnly];
    return copy;
}

- (NSArray *)results
{
    return nil;
}

- (void)cancel
{
}

@end

@interface VNImageBasedRequest : VNRequest
{
    CGRect _regionOfInterest;
}
@property CGRect regionOfInterest;
@end

@implementation VNImageBasedRequest
@synthesize regionOfInterest = _regionOfInterest;

- (id)initWithCompletionHandler:(VNRequestCompletionHandler)completionHandler
{
    self = [super initWithCompletionHandler:completionHandler];
    if (self)
        _regionOfInterest = CGRectMake(0, 0, 1, 1);
    return self;
}

@end

@interface VNGenerateObjectnessBasedSaliencyImageRequest : VNImageBasedRequest
@end

@implementation VNGenerateObjectnessBasedSaliencyImageRequest
@end

@interface VNGenerateAttentionBasedSaliencyImageRequest : VNImageBasedRequest
@end

@implementation VNGenerateAttentionBasedSaliencyImageRequest
@end

@interface VNGenerateForegroundInstanceMaskRequest : VNImageBasedRequest
@end

@implementation VNGenerateForegroundInstanceMaskRequest
@end

@interface VNGeneratePersonInstanceMaskRequest : VNImageBasedRequest
@end

@implementation VNGeneratePersonInstanceMaskRequest
@end

@interface VNDetectFaceRectanglesRequest : VNImageBasedRequest
@end

@implementation VNDetectFaceRectanglesRequest
@end

@interface VNDetectFaceLandmarksRequest : VNImageBasedRequest
@end

@implementation VNDetectFaceLandmarksRequest
@end

@interface VNBarcodeObservation : VNRectangleObservation
@property (readonly, copy) NSString *symbology;
@property (readonly) NSString *payloadStringValue;
@property (readonly) NSData *payloadData;
@end

@implementation VNBarcodeObservation

- (NSString *)symbology
{
    return @"VNBarcodeSymbologyQR";
}

- (NSString *)payloadStringValue
{
    return nil;
}

- (NSData *)payloadData
{
    return nil;
}

@end

@interface VNDetectBarcodesRequest : VNImageBasedRequest
{
    NSArray *_symbologies;
}
@property (copy) NSArray *symbologies;
@property (readonly, copy) NSArray *results;
+ (NSArray *)supportedSymbologies;
- (NSArray *)supportedSymbologiesAndReturnError:(NSError **)error;
@end

@implementation VNDetectBarcodesRequest

+ (NSArray *)supportedSymbologies
{
    return @[@"VNBarcodeSymbologyAztec", @"VNBarcodeSymbologyCode128", @"VNBarcodeSymbologyDataMatrix", @"VNBarcodeSymbologyEAN13", @"VNBarcodeSymbologyPDF417", @"VNBarcodeSymbologyQR"];
}

- (NSArray *)supportedSymbologiesAndReturnError:(NSError **)error
{
    return [[self class] supportedSymbologies];
}

- (void)dealloc
{
    [_symbologies release];
    [super dealloc];
}

- (NSArray *)symbologies
{
    if (_symbologies)
        return _symbologies;
    return [[self class] supportedSymbologies];
}

- (NSArray *)results
{
    return nil;
}

@end

@interface VNImageRequestHandler : NSObject
- (id)initWithCGImage:(struct CGImage *)image options:(NSDictionary *)options;
- (id)initWithCGImage:(struct CGImage *)image orientation:(unsigned int)orientation options:(NSDictionary *)options;
- (id)initWithURL:(NSURL *)url options:(NSDictionary *)options;
- (id)initWithData:(NSData *)data options:(NSDictionary *)options;
- (BOOL)performRequests:(NSArray *)requests error:(NSError **)error;
@end

@implementation VNImageRequestHandler

- (id)initWithCGImage:(struct CGImage *)image options:(NSDictionary *)options
{
    return [super init];
}

- (id)initWithCGImage:(struct CGImage *)image orientation:(unsigned int)orientation options:(NSDictionary *)options
{
    return [super init];
}

- (id)initWithURL:(NSURL *)url options:(NSDictionary *)options
{
    return [super init];
}

- (id)initWithData:(NSData *)data options:(NSDictionary *)options
{
    return [super init];
}

- (BOOL)performRequests:(NSArray *)requests error:(NSError **)error
{
    NSError *failure = [NSError errorWithDomain:VNErrorDomain code:(NSInteger)19 userInfo:@{NSLocalizedDescriptionKey: @"Image analysis is not available."}];
    for (VNRequest *request in requests) {
        if ([request completionHandler])
            [request completionHandler](request, failure);
    }
    if (error)
        *error = failure;
    return NO;
}

@end

NSString *const VNBarcodeSymbologyAztec = @"VNBarcodeSymbologyAztec";
NSString *const VNBarcodeSymbologyCode128 = @"VNBarcodeSymbologyCode128";
NSString *const VNBarcodeSymbologyDataMatrix = @"VNBarcodeSymbologyDataMatrix";
NSString *const VNBarcodeSymbologyEAN13 = @"VNBarcodeSymbologyEAN13";
NSString *const VNBarcodeSymbologyPDF417 = @"VNBarcodeSymbologyPDF417";
NSString *const VNBarcodeSymbologyQR = @"VNBarcodeSymbologyQR";
NSString *const VNBarcodeSymbologyCode39 = @"VNBarcodeSymbologyCode39";
NSString *const VNBarcodeSymbologyCode39Checksum = @"VNBarcodeSymbologyCode39Checksum";
NSString *const VNBarcodeSymbologyCode39FullASCII = @"VNBarcodeSymbologyCode39FullASCII";
NSString *const VNBarcodeSymbologyCode39FullASCIIChecksum = @"VNBarcodeSymbologyCode39FullASCIIChecksum";
NSString *const VNBarcodeSymbologyCode93 = @"VNBarcodeSymbologyCode93";
NSString *const VNBarcodeSymbologyCode93i = @"VNBarcodeSymbologyCode93i";
NSString *const VNBarcodeSymbologyEAN8 = @"VNBarcodeSymbologyEAN8";
NSString *const VNBarcodeSymbologyI2of5 = @"VNBarcodeSymbologyI2of5";
NSString *const VNBarcodeSymbologyI2of5Checksum = @"VNBarcodeSymbologyI2of5Checksum";
NSString *const VNBarcodeSymbologyITF14 = @"VNBarcodeSymbologyITF14";
NSString *const VNBarcodeSymbologyUPCE = @"VNBarcodeSymbologyUPCE";
