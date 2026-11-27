// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <QuartzCore/QuartzCore.h>

// Constraint based layout of layers (CAConstraint, CAConstraintLayoutManager): constraints are kept; layers keep their frames.
@implementation CAConstraint
+ (instancetype)constraintWithAttribute:(CAConstraintAttribute)attr relativeTo:(NSString *)srcId attribute:(CAConstraintAttribute)srcAttr scale:(CGFloat)m offset:(CGFloat)c {
    return [[self alloc] initWithAttribute:attr relativeTo:srcId attribute:srcAttr scale:m offset:c];
}
+ (instancetype)constraintWithAttribute:(CAConstraintAttribute)attr relativeTo:(NSString *)srcId attribute:(CAConstraintAttribute)srcAttr offset:(CGFloat)c {
    return [[self alloc] initWithAttribute:attr relativeTo:srcId attribute:srcAttr scale:1 offset:c];
}
+ (instancetype)constraintWithAttribute:(CAConstraintAttribute)attr relativeTo:(NSString *)srcId attribute:(CAConstraintAttribute)srcAttr {
    return [[self alloc] initWithAttribute:attr relativeTo:srcId attribute:srcAttr scale:1 offset:0];
}
- (instancetype)initWithAttribute:(CAConstraintAttribute)attr relativeTo:(NSString *)srcId attribute:(CAConstraintAttribute)srcAttr scale:(CGFloat)m offset:(CGFloat)c {
    if ((self = [super init])) { _attribute = attr; _sourceName = [srcId copy]; _sourceAttribute = srcAttr; _scale = m; _offset = c; }
    return self;
}
+ (BOOL)supportsSecureCoding { return YES; }
- (void)encodeWithCoder:(NSCoder *)c {}
- (instancetype)initWithCoder:(NSCoder *)c { return [super init]; }
@end

@implementation CAConstraintLayoutManager
+ (instancetype)layoutManager { return [[self alloc] init]; }
- (void)layoutSublayersOfLayer:(CALayer *)layer {}
@end

@implementation CATiledLayer
+ (CFTimeInterval)fadeDuration { return 0.25; }
- (size_t)levelsOfDetail { return 1; }
- (void)setLevelsOfDetail:(size_t)n {}
- (size_t)levelsOfDetailBias { return 0; }
- (void)setLevelsOfDetailBias:(size_t)n {}
- (CGSize)tileSize { return CGSizeMake(256, 256); }
- (void)setTileSize:(CGSize)s {}
@end
