// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// QuickLookThumbnailing: thumbnail requests, representations and the generator that asks QuickLook for the
// image of a file, falling back to the Finder icon, and delivers the result on a concurrent queue.

#import <Foundation/Foundation.h>
#include <math.h>
#include <stdbool.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

// Dispatch queues are Objective-C objects (OS_OBJECT_USE_OBJC), which the libdispatch header of the build does not declare.
@protocol OS_dispatch_queue
@end
typedef NSObject<OS_dispatch_queue> *queue_t;
extern const struct dispatch_queue_attr_s _dispatch_queue_attr_concurrent;
extern queue_t dispatch_queue_create(const char *label, const struct dispatch_queue_attr_s *attr);
extern void dispatch_async(queue_t queue, void (^block)(void)) __attribute__((nothrow));
typedef long dispatch_once_t;
extern void dispatch_once(dispatch_once_t *predicate, void (^block)(void));
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, void (^block)(void))
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once

static inline CGRect CGRectMake(double x, double y, double width, double height)
{
    CGRect r = { { x, y }, { width, height } };
    return r;
}
typedef struct CGImage *CGImageRef;
typedef struct CGColorSpace *CGColorSpaceRef;
typedef struct CGContext *CGContextRef;
typedef struct CGImageDestination *CGImageDestinationRef;
typedef const struct __CFAllocator *CFAllocatorRef;
typedef const struct __CFDictionary *CFDictionaryRef;
typedef const struct __CFURL *CFURLRef;
typedef const struct __CFString *CFStringRef;
typedef const void *CFTypeRef;
extern const CFAllocatorRef kCFAllocatorDefault;
extern void CFRelease(CFTypeRef object);
extern CGImageRef CGImageRetain(CGImageRef image);
extern void CGImageRelease(CGImageRef image);
extern size_t CGImageGetWidth(CGImageRef image);
extern size_t CGImageGetHeight(CGImageRef image);
extern CGColorSpaceRef CGColorSpaceCreateDeviceRGB(void);
extern void CGColorSpaceRelease(CGColorSpaceRef space);
extern CGContextRef CGBitmapContextCreate(void *data, size_t width, size_t height, size_t bitsPerComponent, size_t bytesPerRow, CGColorSpaceRef space, unsigned int info);
extern CGImageRef CGBitmapContextCreateImage(CGContextRef context);
extern void CGContextRelease(CGContextRef context);
extern void CGContextSetInterpolationQuality(CGContextRef context, int quality);
extern void CGContextDrawImage(CGContextRef context, CGRect rect, CGImageRef image);
extern CGImageDestinationRef CGImageDestinationCreateWithURL(CFURLRef url, CFStringRef type, size_t count, CFDictionaryRef options);
extern void CGImageDestinationAddImage(CGImageDestinationRef destination, CGImageRef image, CFDictionaryRef properties);
extern bool CGImageDestinationFinalize(CGImageDestinationRef destination);
extern CGImageRef QLThumbnailImageCreate(CFAllocatorRef allocator, CFURLRef url, CGSize maxThumbnailSize, CFDictionaryRef options);
extern NSString *const kQLThumbnailOptionIconModeKey;
extern NSString *const kQLThumbnailOptionScaleFactorKey;
extern NSString *const NSImageHintCTM;
extern long lround(double x);

@interface NSURL : NSObject
- (BOOL)isFileURL;
- (NSString *)path;
@end
@interface NSError : NSObject
+ (NSError *)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end
@interface NSDictionary (QL)
+ (id)dictionaryWithObjects:(const id *)objects forKeys:(const id *)keys count:(NSUInteger)count;
@end
@interface NSValue : NSObject
+ (NSValue *)valueWithNonretainedObject:(id)object;
@end
@interface NSMutableSet : NSObject
- (BOOL)containsObject:(id)object;
- (void)addObject:(id)object;
- (void)removeObject:(id)object;
@end
@interface NSCoder : NSObject
- (id)decodeObjectOfClass:(Class)aClass forKey:(NSString *)key;
- (double)decodeDoubleForKey:(NSString *)key;
- (NSInteger)decodeIntegerForKey:(NSString *)key;
- (BOOL)decodeBoolForKey:(NSString *)key;
- (void)encodeObject:(id)object forKey:(NSString *)key;
- (void)encodeDouble:(double)value forKey:(NSString *)key;
- (void)encodeInteger:(NSInteger)value forKey:(NSString *)key;
- (void)encodeBool:(BOOL)value forKey:(NSString *)key;
@end
@interface NSFileManager : NSObject
+ (NSFileManager *)defaultManager;
- (BOOL)fileExistsAtPath:(NSString *)path;
@end
@interface NSAffineTransform : NSObject
+ (NSAffineTransform *)transform;
- (void)scaleBy:(double)scale;
@end
@interface NSImage : NSObject
- (id)initWithCGImage:(CGImageRef)image size:(CGSize)size;
- (CGImageRef)CGImageForProposedRect:(CGRect *)rect context:(id)context hints:(NSDictionary *)hints;
@end
@interface NSWorkspace : NSObject
+ (NSWorkspace *)sharedWorkspace;
- (NSImage *)iconForFile:(NSString *)path;
@end

NSString *const QLThumbnailErrorDomain = @"QLThumbnailErrorDomain";

@interface QLThumbnailGenerationRequest : NSObject <NSCopying, NSSecureCoding>
{
    BOOL _iconMode;
    double _minimumDimension;
    NSURL *_fileURL;
    double _scale;
    unsigned long _representationTypes;
    CGSize _size;
}
@property (nonatomic) double minimumDimension;
@property (nonatomic) BOOL iconMode;
@property (readonly, nonatomic) NSURL *fileURL;
@property (readonly, nonatomic) CGSize size;
@property (readonly, nonatomic) double scale;
@property (readonly, nonatomic) unsigned long representationTypes;
- (id)initWithFileAtURL:(NSURL *)url size:(CGSize)size scale:(double)scale representationTypes:(unsigned long)types;
@end

@implementation QLThumbnailGenerationRequest
+ (BOOL)supportsSecureCoding { return YES; }

- (id)initWithFileAtURL:(NSURL *)url size:(CGSize)size scale:(double)scale representationTypes:(unsigned long)types
{
    if ((self = [super init])) {
        _fileURL = [url copy];
        _size = size;
        _scale = scale > 0 ? scale : 1.0;
        _representationTypes = types;
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)coder
{
    NSURL *url = [coder decodeObjectOfClass:[NSURL class] forKey:@"fileURL"];
    double width = [coder decodeDoubleForKey:@"width"];
    double height = [coder decodeDoubleForKey:@"height"];
    double scale = [coder decodeDoubleForKey:@"scale"];
    self = [self initWithFileAtURL:url size:CGRectMake(0, 0, width, height).size scale:scale representationTypes:[coder decodeIntegerForKey:@"representationTypes"]];
    if (self) {
        _minimumDimension = [coder decodeDoubleForKey:@"minimumDimension"];
        _iconMode = [coder decodeBoolForKey:@"iconMode"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_fileURL forKey:@"fileURL"];
    [coder encodeDouble:_size.width forKey:@"width"];
    [coder encodeDouble:_size.height forKey:@"height"];
    [coder encodeDouble:_scale forKey:@"scale"];
    [coder encodeInteger:_representationTypes forKey:@"representationTypes"];
    [coder encodeDouble:_minimumDimension forKey:@"minimumDimension"];
    [coder encodeBool:_iconMode forKey:@"iconMode"];
}

- (id)copyWithZone:(NSZone *)zone
{
    QLThumbnailGenerationRequest *copy = [[[self class] allocWithZone:zone] initWithFileAtURL:_fileURL size:_size scale:_scale representationTypes:_representationTypes];
    [copy setMinimumDimension:_minimumDimension];
    [copy setIconMode:_iconMode];
    return copy;
}

- (void)dealloc
{
    [_fileURL release];
    [super dealloc];
}
@end

@interface QLThumbnailRepresentation : NSObject
{
    CGImageRef _image;
    double _scale;
    NSImage *_nsImage;
    long _type;
    CGRect _contentRect;
}
@property (readonly) long type;
@property (readonly) CGImageRef CGImage;
@property (readonly) NSImage *NSImage;
@property (readonly) CGRect contentRect;
- (id)initWithImage:(CGImageRef)image scale:(double)scale type:(long)type;
@end

@implementation QLThumbnailRepresentation
- (id)initWithImage:(CGImageRef)image scale:(double)scale type:(long)type
{
    if ((self = [super init])) {
        _image = CGImageRetain(image);
        _scale = scale;
        _type = type;
        _contentRect = CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image));
    }
    return self;
}

- (void)dealloc
{
    CGImageRelease(_image);
    [_nsImage release];
    [super dealloc];
}

- (CGImageRef)CGImage { return _image; }

- (NSImage *)NSImage
{
    @synchronized (self) {
        if (!_nsImage) {
            CGSize size = { CGImageGetWidth(_image) / _scale, CGImageGetHeight(_image) / _scale };
            _nsImage = [[NSImage alloc] initWithCGImage:_image size:size];
        }
        return _nsImage;
    }
}
@end

@interface QLThumbnailGenerator : NSObject
{
    queue_t _queue;
    NSMutableSet *_cancelled;
}
+ (QLThumbnailGenerator *)sharedGenerator;
- (BOOL)takeCancellationOf:(id)request;
- (QLThumbnailRepresentation *)makeIconFor:(QLThumbnailGenerationRequest *)request;
- (QLThumbnailRepresentation *)makeRepresentationFor:(QLThumbnailGenerationRequest *)request;
- (void)generateBestRepresentationForRequest:(QLThumbnailGenerationRequest *)request completionHandler:(void (^)(QLThumbnailRepresentation *, NSError *))handler;
- (void)generateRepresentationsForRequest:(QLThumbnailGenerationRequest *)request updateHandler:(void (^)(QLThumbnailRepresentation *, long, NSError *))handler;
- (void)cancelRequest:(id)request;
- (void)saveBestRepresentationForRequest:(QLThumbnailGenerationRequest *)request toFileAtURL:(NSURL *)url withContentType:(NSString *)type completionHandler:(void (^)(NSError *))handler;
@end

@implementation QLThumbnailGenerator
+ (QLThumbnailGenerator *)sharedGenerator
{
    static QLThumbnailGenerator *generator;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        generator = [[QLThumbnailGenerator alloc] init];
    });
    return generator;
}

- (id)init
{
    if ((self = [super init])) {
        _queue = dispatch_queue_create("com.apple.quicklook.thumbnailing", &_dispatch_queue_attr_concurrent);
        _cancelled = [[NSMutableSet alloc] init];
    }
    return self;
}

- (BOOL)takeCancellationOf:(id)request
{
    @synchronized (_cancelled) {
        NSValue *key = [NSValue valueWithNonretainedObject:request];
        BOOL cancelled = [_cancelled containsObject:key];
        [_cancelled removeObject:key];
        return cancelled;
    }
}

// The icon of the file as a representation of the requested size, or nil.
- (QLThumbnailRepresentation *)makeIconFor:(QLThumbnailGenerationRequest *)request
{
    QLThumbnailRepresentation *result = nil;
    if (([request representationTypes] & 1) && [[request fileURL] isFileURL]
        && [[NSFileManager defaultManager] fileExistsAtPath:[[request fileURL] path]]) {
        NSImage *icon = [[NSWorkspace sharedWorkspace] iconForFile:[[request fileURL] path]];
        double width = [request size].width;
        double height = [request size].height;
        double dimension = width < height ? width : height;
        if (icon && !(dimension <= 0)) {
            CGRect rect = CGRectMake(0, 0, dimension, dimension);
            NSAffineTransform *transform = [NSAffineTransform transform];
            [transform scaleBy:[request scale]];
            NSDictionary *hints = @{ NSImageHintCTM: transform };
            CGImageRef image = [icon CGImageForProposedRect:&rect context:nil hints:hints];
            size_t pixels = lround([request scale] * dimension);
            if (image && pixels) {
                CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
                CGContextRef context = CGBitmapContextCreate(NULL, pixels, pixels, 8, pixels * 4, space, 0x2002);
                CGColorSpaceRelease(space);
                if (context) {
                    CGContextSetInterpolationQuality(context, 3);
                    CGContextDrawImage(context, CGRectMake(0, 0, pixels, pixels), image);
                    CGImageRef scaled = CGBitmapContextCreateImage(context);
                    CGContextRelease(context);
                    if (scaled) {
                        result = [[[QLThumbnailRepresentation alloc] initWithImage:scaled scale:[request scale] type:0] autorelease];
                        CGImageRelease(scaled);
                    }
                }
            }
        }
    }
    return result;
}

- (QLThumbnailRepresentation *)makeRepresentationFor:(QLThumbnailGenerationRequest *)request
{
    if (!([request representationTypes] & 6))
        return [self makeIconFor:request];
    double width = [request size].width * [request scale];
    double height = [request size].height * [request scale];
    NSDictionary *options = @{
        kQLThumbnailOptionIconModeKey: [NSNumber numberWithBool:[request iconMode]],
        kQLThumbnailOptionScaleFactorKey: [NSNumber numberWithDouble:[request scale]],
    };
    CGImageRef image = QLThumbnailImageCreate(kCFAllocatorDefault, (CFURLRef)[request fileURL], CGRectMake(0, 0, width, height).size, (CFDictionaryRef)options);
    if (image) {
        bool tooSmall = false;
        if ([request minimumDimension] > 0) {
            size_t imageWidth = CGImageGetWidth(image);
            size_t imageHeight = CGImageGetHeight(image);
            double shortest = imageWidth < imageHeight ? imageWidth : imageHeight;
            tooSmall = [request minimumDimension] * [request scale] > shortest;
        }
        if (!tooSmall) {
            QLThumbnailRepresentation *representation = [[[QLThumbnailRepresentation alloc] initWithImage:image scale:[request scale] type:2] autorelease];
            CGImageRelease(image);
            return representation;
        }
        CGImageRelease(image);
    }
    return [self makeIconFor:request];
}

- (void)generateBestRepresentationForRequest:(QLThumbnailGenerationRequest *)request completionHandler:(void (^)(QLThumbnailRepresentation *, NSError *))completionHandler
{
    void (^handler)(QLThumbnailRepresentation *, NSError *) = [completionHandler copy];
    QLThumbnailGenerationRequest *copy = [request copy];
    [self retain];
    dispatch_async(_queue, ^{
        @autoreleasepool {
            if ([self takeCancellationOf:request]) {
                handler(nil, [NSError errorWithDomain:QLThumbnailErrorDomain code:5 userInfo:nil]);
            } else {
                QLThumbnailRepresentation *representation = [self makeRepresentationFor:copy];
                handler(representation, representation ? nil : [NSError errorWithDomain:QLThumbnailErrorDomain code:0 userInfo:nil]);
            }
        }
        [copy release];
        [handler release];
        [self release];
    });
}

- (void)generateRepresentationsForRequest:(QLThumbnailGenerationRequest *)request updateHandler:(void (^)(QLThumbnailRepresentation *, long, NSError *))updateHandler
{
    if (!updateHandler)
        return;
    void (^handler)(QLThumbnailRepresentation *, long, NSError *) = [updateHandler copy];
    QLThumbnailGenerationRequest *copy = [request copy];
    [self retain];
    dispatch_async(_queue, ^{
        @autoreleasepool {
            if ([self takeCancellationOf:request]) {
                handler(nil, 2, [NSError errorWithDomain:QLThumbnailErrorDomain code:5 userInfo:nil]);
            } else {
                bool pending = true;
                if ([copy representationTypes] & 6) {
                    QLThumbnailRepresentation *icon = [self makeIconFor:copy];
                    if (icon) {
                        pending = false;
                        handler(icon, 0, nil);
                    }
                }
                QLThumbnailRepresentation *representation = [self makeRepresentationFor:copy];
                if (representation) {
                    if ([representation type] || pending)
                        handler(representation, [representation type], nil);
                } else if (pending) {
                    handler(nil, 2, [NSError errorWithDomain:QLThumbnailErrorDomain code:0 userInfo:nil]);
                }
            }
        }
        [copy release];
        [handler release];
        [self release];
    });
}

- (void)cancelRequest:(id)request
{
    @synchronized (_cancelled) {
        [_cancelled addObject:[NSValue valueWithNonretainedObject:request]];
    }
}

- (void)saveBestRepresentationForRequest:(QLThumbnailGenerationRequest *)request toFileAtURL:(NSURL *)url withContentType:(NSString *)contentType completionHandler:(void (^)(NSError *))completionHandler
{
    void (^handler)(NSError *) = [completionHandler copy];
    NSURL *destinationURL = [url copy];
    NSString *type = [contentType copy];
    [self generateBestRepresentationForRequest:request completionHandler:^(QLThumbnailRepresentation *representation, NSError *error) {
        if (representation) {
            CGImageDestinationRef destination = CGImageDestinationCreateWithURL((CFURLRef)destinationURL, (CFStringRef)type, 1, NULL);
            bool written = false;
            if (destination) {
                CGImageDestinationAddImage(destination, [representation CGImage], NULL);
                written = CGImageDestinationFinalize(destination);
                CFRelease(destination);
            }
            error = written ? nil : [NSError errorWithDomain:QLThumbnailErrorDomain code:1 userInfo:nil];
        }
        if (handler)
            handler(error);
        [handler release];
        [destinationURL release];
        [type release];
    }];
}
@end
