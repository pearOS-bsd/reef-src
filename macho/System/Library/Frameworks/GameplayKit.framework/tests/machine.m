// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// GameplayKit: a state machine enters states in the allowed order, tells them about it and updates the current one.
#import <Foundation/Foundation.h>
#include <stdio.h>
const char *object_getClassName(id);

@interface GKStateMachine : NSObject
@property (readonly) id currentState;
+ (id)stateMachineWithStates:(NSArray *)states;
- (id)initWithStates:(NSArray *)states;
- (void)updateWithDeltaTime:(double)seconds;
- (id)stateForClass:(Class)stateClass;
- (BOOL)canEnterState:(Class)stateClass;
- (BOOL)enterState:(Class)stateClass;
@end

@interface GKState : NSObject
@property (weak) GKStateMachine *stateMachine;
+ (id)state;
- (BOOL)isValidNextState:(Class)stateClass;
@end

@interface Idle : GKState
@end
@interface Running : GKState
@end
@interface Done : GKState
@end

@implementation Idle
- (BOOL)isValidNextState:(Class)c { return c == [Running class]; }
- (void)didEnterWithPreviousState:(id)p { printf("idle entered from %s\n", p ? object_getClassName(p) : "nothing"); }
- (void)willExitWithNextState:(id)n { printf("idle exits to %s\n", object_getClassName(n)); }
@end
@implementation Running
- (BOOL)isValidNextState:(Class)c { return c != [Running class]; }
- (void)didEnterWithPreviousState:(id)p { printf("running entered from %s\n", object_getClassName(p)); }
- (void)updateWithDeltaTime:(double)t { printf("running update %g\n", t); }
- (void)willExitWithNextState:(id)n { printf("running exits to %s\n", object_getClassName(n)); }
@end
@implementation Done
@end

int main(void)
{
    GKStateMachine *m = [GKStateMachine stateMachineWithStates:@[[Idle state], [Running state], [Done state]]];
    printf("start %d %d %d\n", m.currentState == nil, [m canEnterState:[Idle class]], [m canEnterState:[NSObject class]]);
    printf("enter idle %d\n", [m enterState:[Idle class]]);
    printf("enter done %d\n", [m enterState:[Done class]]);
    printf("enter running %d\n", [m enterState:[Running class]]);
    [m updateWithDeltaTime:0.5];
    printf("state %s machine %d\n", object_getClassName(m.currentState), [[m stateForClass:[Done class]] stateMachine] == m);
    printf("enter running again %d\n", [m enterState:[Running class]]);
    printf("enter done %d\n", [m enterState:[Done class]]);
    printf("enter unknown %d\n", [m enterState:[NSObject class]]);
    id done = [m stateForClass:[Done class]];
    m = nil;
    printf("after release %d\n", [done stateMachine] == nil);
    return 0;
}
