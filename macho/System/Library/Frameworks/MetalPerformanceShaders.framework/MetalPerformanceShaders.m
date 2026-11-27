// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MetalPerformanceShaders: the image kernels exist as objects but have no GPU to run on (a kernel cannot be created from a device).

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
@end

@protocol MTLDevice;

BOOL MPSSupportsMTLDevice(id<MTLDevice> device)
{
    return NO;
}

@interface MPSKernel : NSObject <NSCopying>
{
    id<MTLDevice> _device;
    NSString *_label;
}
@property (readonly, strong, nonatomic) id<MTLDevice> device;
@property (copy) NSString *label;
- (id)initWithDevice:(id<MTLDevice>)device;
@end

@implementation MPSKernel

- (id)initWithDevice:(id<MTLDevice>)device
{
    [self release];
    return nil;
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (void)dealloc
{
    [(id)_device release];
    [_label release];
    [super dealloc];
}

@end

@interface MPSUnaryImageKernel : MPSKernel
{
    NSUInteger _edgeMode;
}
@property (nonatomic) NSUInteger edgeMode;
- (BOOL)encodeToCommandBuffer:(id)commandBuffer inPlaceTexture:(id *)texture fallbackCopyAllocator:(id)allocator;
- (void)encodeToCommandBuffer:(id)commandBuffer sourceTexture:(id)source destinationTexture:(id)destination;
@end

@implementation MPSUnaryImageKernel

- (BOOL)encodeToCommandBuffer:(id)commandBuffer inPlaceTexture:(id *)texture fallbackCopyAllocator:(id)allocator
{
    return NO;
}

- (void)encodeToCommandBuffer:(id)commandBuffer sourceTexture:(id)source destinationTexture:(id)destination
{
}

@end

@interface MPSImageConvolution : MPSUnaryImageKernel
{
    float _bias;
    NSUInteger _kernelHeight;
    NSUInteger _kernelWidth;
}
@property (readonly, nonatomic) NSUInteger kernelHeight;
@property (readonly, nonatomic) NSUInteger kernelWidth;
@property (nonatomic) float bias;
- (id)initWithDevice:(id<MTLDevice>)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height weights:(const float *)weights;
@end

@implementation MPSImageConvolution

- (id)initWithDevice:(id<MTLDevice>)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height weights:(const float *)weights
{
    self = [super initWithDevice:device];
    if (self) {
        _kernelWidth = width;
        _kernelHeight = height;
    }
    return self;
}

@end

@interface MPSImageBox : MPSUnaryImageKernel
{
    NSUInteger _kernelHeight;
    NSUInteger _kernelWidth;
}
@property (readonly, nonatomic) NSUInteger kernelHeight;
@property (readonly, nonatomic) NSUInteger kernelWidth;
- (id)initWithDevice:(id<MTLDevice>)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height;
@end

@implementation MPSImageBox

- (id)initWithDevice:(id<MTLDevice>)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height
{
    self = [super initWithDevice:device];
    if (self) {
        _kernelWidth = width;
        _kernelHeight = height;
    }
    return self;
}

@end

@interface MPSImageAreaMax : MPSUnaryImageKernel
{
    NSUInteger _kernelHeight;
    NSUInteger _kernelWidth;
}
@property (readonly, nonatomic) NSUInteger kernelHeight;
@property (readonly, nonatomic) NSUInteger kernelWidth;
- (id)initWithDevice:(id<MTLDevice>)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height;
@end

@implementation MPSImageAreaMax

- (id)initWithDevice:(id<MTLDevice>)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height
{
    self = [super initWithDevice:device];
    if (self) {
        _kernelWidth = width;
        _kernelHeight = height;
    }
    return self;
}

@end

@interface MPSImageAreaMin : MPSImageAreaMax
@end

@implementation MPSImageAreaMin
@end
