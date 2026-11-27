// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// LocalAuthenticationEmbeddedUI: an authentication view keeps its context and control size.
#import <Foundation/Foundation.h>
#include <stdio.h>
const char *object_getClassName(id);

@interface LAAuthenticationView : NSObject
@property (readonly) id context;
@property (readonly) unsigned long controlSize;
- (id)initWithContext:(id)context;
- (id)initWithContext:(id)context controlSize:(unsigned long)controlSize;
@end

int main(void)
{
    id context = [NSObject new];
    LAAuthenticationView *a = [[LAAuthenticationView alloc] initWithContext:context];
    LAAuthenticationView *b = [[LAAuthenticationView alloc] initWithContext:context controlSize:2];
    printf("a %d %d %lu\n", a != nil, a.context == context, a.controlSize);
    printf("b %d %d %lu\n", b != nil, b.context == context, b.controlSize);
    printf("class %s\n", object_getClassName(b));
    return 0;
}
