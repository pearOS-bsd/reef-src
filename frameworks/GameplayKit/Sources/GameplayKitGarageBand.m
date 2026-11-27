// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// An R-tree of elements by their bounding boxes; a flat list searched in order gives the same answers.
#import <Foundation/Foundation.h>
#import <simd/simd.h>

@interface GKRTree : NSObject
@property (readonly) NSUInteger queryReserve;
@end

@interface GKRTreeEntry : NSObject
@property (strong) id element;
@property simd_float2 min, max;
@end
@implementation GKRTreeEntry
@end

@implementation GKRTree {
    NSMutableArray<GKRTreeEntry *> *_entries;
}
+ (instancetype)treeWithMaxNumberOfChildren:(NSUInteger)n { return [[self alloc] initWithMaxNumberOfChildren:n]; }
- (instancetype)initWithMaxNumberOfChildren:(NSUInteger)n {
    if ((self = [super init])) _entries = [NSMutableArray array];
    return self;
}
- (void)addElement:(id)element boundingRectMin:(simd_float2)min boundingRectMax:(simd_float2)max splitStrategy:(NSInteger)strategy {
    GKRTreeEntry *e = [GKRTreeEntry new];
    e.element = element; e.min = min; e.max = max;
    [_entries addObject:e];
}
- (void)removeElement:(id)element boundingRectMin:(simd_float2)min boundingRectMax:(simd_float2)max {
    for (NSUInteger i = 0; i < _entries.count; i++) {
        GKRTreeEntry *e = _entries[i];
        if (e.element == element && simd_equal(e.min, min) && simd_equal(e.max, max)) { [_entries removeObjectAtIndex:i]; return; }
    }
}
- (NSArray *)elementsInBoundingRectMin:(simd_float2)min rectMax:(simd_float2)max {
    NSMutableArray *found = [NSMutableArray array];
    for (GKRTreeEntry *e in _entries)
        if (e.min.x <= max.x && e.max.x >= min.x && e.min.y <= max.y && e.max.y >= min.y)
            [found addObject:e.element];
    return found;
}
@end
