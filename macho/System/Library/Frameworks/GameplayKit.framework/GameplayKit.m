// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GameplayKit: states and the state machine that moves between them.

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@interface NSMapTable : NSObject
+ (id)strongToStrongObjectsMapTable;
- (id)objectForKey:(id)key;
- (void)setObject:(id)object forKey:(id)key;
- (id)objectEnumerator;
@end

@class GKStateMachine;

@interface GKState : NSObject
{
    __weak GKStateMachine *_stateMachine;
}
@property (weak) GKStateMachine *stateMachine;
+ (id)state;
- (BOOL)isValidNextState:(Class)stateClass;
- (void)didEnterWithPreviousState:(GKState *)previousState;
- (void)updateWithDeltaTime:(double)seconds;
- (void)willExitWithNextState:(GKState *)nextState;
@end

@implementation GKState

+ (id)state
{
    return [[[self alloc] init] autorelease];
}

- (id)init
{
    return [super init];
}

- (BOOL)isValidNextState:(Class)stateClass { return YES; }
- (void)didEnterWithPreviousState:(GKState *)previousState { }
- (void)updateWithDeltaTime:(double)seconds { }
- (void)willExitWithNextState:(GKState *)nextState { }

@end

@interface GKStateMachine : NSObject
{
    NSMapTable *_states;
    GKState *_currentState;
}
@property (readonly) GKState *currentState;
+ (id)stateMachineWithStates:(NSArray *)states;
- (id)initWithStates:(NSArray *)states;
- (void)updateWithDeltaTime:(double)seconds;
- (GKState *)stateForClass:(Class)stateClass;
- (BOOL)canEnterState:(Class)stateClass;
- (BOOL)enterState:(Class)stateClass;
@end

@implementation GKStateMachine

+ (id)stateMachineWithStates:(NSArray *)states
{
    return [[[self alloc] initWithStates:states] autorelease];
}

- (id)initWithStates:(NSArray *)states
{
    self = [super init];
    if (self) {
        _states = [[NSMapTable strongToStrongObjectsMapTable] retain];
        for (GKState *state in states) {
            [state setStateMachine:self];
            [_states setObject:state forKey:[state class]];
        }
    }
    return self;
}

- (void)dealloc
{
    for (GKState *state in [_states objectEnumerator])
        [state setStateMachine:nil];
    [_states release];
    [_currentState release];
    [super dealloc];
}

- (GKState *)currentState { return _currentState; }

- (void)updateWithDeltaTime:(double)seconds
{
    [_currentState updateWithDeltaTime:seconds];
}

- (GKState *)stateForClass:(Class)stateClass
{
    return [_states objectForKey:stateClass];
}

- (BOOL)canEnterState:(Class)stateClass
{
    if (![self stateForClass:stateClass])
        return NO;
    if (!_currentState)
        return YES;
    return [_currentState isValidNextState:stateClass] != 0;
}

- (BOOL)enterState:(Class)stateClass
{
    if (![self canEnterState:stateClass])
        return NO;
    GKState *next = [self stateForClass:stateClass];
    GKState *previous = _currentState;
    [previous willExitWithNextState:next];
    _currentState = [next retain];
    [next didEnterWithPreviousState:previous];
    [previous release];
    return YES;
}

@end
