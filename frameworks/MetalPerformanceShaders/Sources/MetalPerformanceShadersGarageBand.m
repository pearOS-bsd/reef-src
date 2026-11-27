// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

@interface MPSUnaryImageKernel : NSObject
- (instancetype)initWithDevice:(id)device;
@end

// A Gaussian blur kernel; encoding draws nothing (the caller keeps its source texture).
@interface MPSImageGaussianBlur : MPSUnaryImageKernel
@property (readonly) float sigma;
@end
@implementation MPSImageGaussianBlur
- (instancetype)initWithDevice:(id)device sigma:(float)sigma {
    if ((self = [super initWithDevice:device])) _sigma = sigma;
    return self;
}
@end
