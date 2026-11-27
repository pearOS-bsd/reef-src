// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// A group of writer inputs that are alternatives to each other (AVMediaSelectionGroup on Apple's side)
@interface AVAssetWriterInputGroup : NSObject
@property (readonly) NSArray *inputs;
@property (readonly) id defaultInput;
@end
@implementation AVAssetWriterInputGroup
+ (instancetype)assetWriterInputGroupWithInputs:(NSArray *)inputs defaultInput:(id)input { return [[self alloc] initWithInputs:inputs defaultInput:input]; }
- (instancetype)initWithInputs:(NSArray *)inputs defaultInput:(id)input {
    if ((self = [super init])) {
        _inputs = [inputs copy];
        _defaultInput = input;
    }
    return self;
}
@end
