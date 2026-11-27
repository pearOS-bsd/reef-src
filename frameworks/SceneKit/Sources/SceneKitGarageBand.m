// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@interface SCNGeometry : NSObject
@end

@interface SCNBox : SCNGeometry
@property CGFloat width, height, length, chamferRadius;
@end
@implementation SCNBox
+ (instancetype)boxWithWidth:(CGFloat)w height:(CGFloat)h length:(CGFloat)l chamferRadius:(CGFloat)r {
    SCNBox *b = [self new];
    b.width = w; b.height = h; b.length = l; b.chamferRadius = r;
    return b;
}
@end

@interface SCNSphere : SCNGeometry
@property CGFloat radius;
@property (getter=isGeodesic) BOOL geodesic;
@property NSInteger segmentCount;
@end
@implementation SCNSphere
+ (instancetype)sphereWithRadius:(CGFloat)radius {
    SCNSphere *s = [self new];
    s.radius = radius; s.segmentCount = 48;
    return s;
}
@end
