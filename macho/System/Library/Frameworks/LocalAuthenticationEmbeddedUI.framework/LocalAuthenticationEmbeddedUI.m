// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LocalAuthenticationEmbeddedUI: LAAuthenticationView, a view that only keeps its context and control size.

#import <Foundation/Foundation.h>

@class LAContext;

extern const CGRect NSZeroRect;

@interface NSView : NSObject
{
    char _padding[176];
}
- (id)initWithFrame:(CGRect)frame;
@end

@interface LAAuthenticationView : NSView
{
    LAContext *_context;
    unsigned long _controlSize;
}
@property (readonly) LAContext *context;
@property (readonly) unsigned long controlSize;
- (id)initWithContext:(id)context;
- (id)initWithContext:(id)context controlSize:(unsigned long)controlSize;
@end

@implementation LAAuthenticationView

- (id)initWithContext:(id)context
{
    return [self initWithContext:context controlSize:0];
}

- (id)initWithContext:(id)context controlSize:(unsigned long)controlSize
{
    self = [super initWithFrame:NSZeroRect];
    if (self) {
        _context = context;
        _controlSize = controlSize;
    }
    return self;
}

@end
