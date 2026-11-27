// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// MetalPerformanceShaders: no kernel can be made from a device; a kernel made with new keeps its label and settings and copies to itself.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSString (T)
- (const char *)UTF8String;
@end

BOOL MPSSupportsMTLDevice(id device);

@interface MPSKernel : NSObject <NSCopying>
@property (readonly) id device;
@property (copy) NSString *label;
- (id)initWithDevice:(id)device;
@end
@interface MPSUnaryImageKernel : MPSKernel
@property NSUInteger edgeMode;
- (BOOL)encodeToCommandBuffer:(id)commandBuffer inPlaceTexture:(id *)texture fallbackCopyAllocator:(id)allocator;
- (void)encodeToCommandBuffer:(id)commandBuffer sourceTexture:(id)source destinationTexture:(id)destination;
@end
@interface MPSImageConvolution : MPSUnaryImageKernel
@property (readonly) NSUInteger kernelHeight, kernelWidth;
@property float bias;
- (id)initWithDevice:(id)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height weights:(const float *)weights;
@end
@interface MPSImageBox : MPSUnaryImageKernel
@property (readonly) NSUInteger kernelHeight, kernelWidth;
- (id)initWithDevice:(id)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height;
@end
@interface MPSImageAreaMax : MPSUnaryImageKernel
@property (readonly) NSUInteger kernelHeight, kernelWidth;
- (id)initWithDevice:(id)device kernelWidth:(NSUInteger)width kernelHeight:(NSUInteger)height;
@end
@interface MPSImageAreaMin : MPSImageAreaMax
@end

int main(void)
{
    printf("supports %d\n", MPSSupportsMTLDevice(nil));
    printf("init %d %d %d %d\n", [[MPSKernel alloc] initWithDevice:nil] == nil, [[MPSImageBox alloc] initWithDevice:nil kernelWidth:3 kernelHeight:5] == nil,
           [[MPSImageAreaMin alloc] initWithDevice:nil kernelWidth:3 kernelHeight:5] == nil, [[MPSImageConvolution alloc] initWithDevice:nil kernelWidth:3 kernelHeight:5 weights:NULL] == nil);
    MPSImageConvolution *k = [MPSImageConvolution new];
    k.label = @"blur"; k.edgeMode = 2; k.bias = 0.5f;
    printf("kernel %s %lu %g %lu %lu %d\n", [k.label UTF8String], (unsigned long)k.edgeMode, k.bias, (unsigned long)k.kernelWidth, (unsigned long)k.kernelHeight, k.device == nil);
    printf("encode %d\n", [k encodeToCommandBuffer:nil inPlaceTexture:NULL fallbackCopyAllocator:nil]);
    [k encodeToCommandBuffer:nil sourceTexture:nil destinationTexture:nil];
    printf("copy %d\n", [k copy] == k);
    return 0;
}
