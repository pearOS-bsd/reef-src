// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ExceptionHandling: the exception handler object; an uncaught exception is logged and handed on to the handler that was installed before.

#import <Foundation/Foundation.h>

typedef long dispatch_once_t;
extern void dispatch_once(dispatch_once_t *predicate, void (^block)(void));
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, void (^block)(void))
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once

@interface NSException : NSObject
- (NSString *)name;
- (NSString *)reason;
- (NSArray *)callStackSymbols;
@end

extern void NSLog(NSString *format, ...);
typedef void NSUncaughtExceptionHandler(NSException *exception);
extern NSUncaughtExceptionHandler *NSGetUncaughtExceptionHandler(void);
extern void NSSetUncaughtExceptionHandler(NSUncaughtExceptionHandler *handler);

NSString *const NSUncaughtSystemExceptionException = @"NSUncaughtSystemExceptionException";
NSString *const NSUncaughtRuntimeErrorException = @"NSUncaughtRuntimeErrorException";
NSString *const NSStackTraceKey = @"NSStackTraceKey";

@protocol NSExceptionHandlerDelegate
- (BOOL)exceptionHandler:(id)sender shouldLogException:(NSException *)exception mask:(NSUInteger)mask;
@end

@interface NSExceptionHandler : NSObject
{
    NSUInteger _handlingMask;
    NSUInteger _hangingMask;
    id _delegate;
}
+ (id)defaultExceptionHandler;
- (void)setExceptionHandlingMask:(NSUInteger)mask;
- (NSUInteger)exceptionHandlingMask;
- (void)setExceptionHangingMask:(NSUInteger)mask;
- (NSUInteger)exceptionHangingMask;
- (void)setDelegate:(id)delegate;
- (id)delegate;
@end

static NSUncaughtExceptionHandler *previousHandler;

static void handleUncaughtException(NSException *exception)
{
    NSExceptionHandler *handler = [NSExceptionHandler defaultExceptionHandler];
    if ([handler exceptionHandlingMask] & 1) {
        id delegate = [handler delegate];
        BOOL log = YES;
        if ([delegate respondsToSelector:@selector(exceptionHandler:shouldLogException:mask:)])
            log = [delegate exceptionHandler:handler shouldLogException:exception mask:1];
        if (log)
            NSLog(@"Uncaught exception %@: %@\n%@", [exception name], [exception reason], [exception callStackSymbols]);
    }
    if (previousHandler)
        previousHandler(exception);
}

@implementation NSExceptionHandler

+ (id)defaultExceptionHandler
{
    static NSExceptionHandler *handler;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        handler = [[NSExceptionHandler alloc] init];
    });
    return handler;
}

- (void)setExceptionHandlingMask:(NSUInteger)mask
{
    @synchronized (self) {
        NSUInteger old = _handlingMask;
        _handlingMask = mask;
        if ((mask & 3) && !(old & 3)) {
            NSUncaughtExceptionHandler *current = NSGetUncaughtExceptionHandler();
            if (current != handleUncaughtException) {
                previousHandler = current;
                NSSetUncaughtExceptionHandler(handleUncaughtException);
            }
        }
    }
}

- (NSUInteger)exceptionHandlingMask
{
    @synchronized (self) {
        return _handlingMask;
    }
}

- (void)setExceptionHangingMask:(NSUInteger)mask { _hangingMask = mask; }
- (NSUInteger)exceptionHangingMask { return _hangingMask; }
- (void)setDelegate:(id)delegate { _delegate = delegate; }
- (id)delegate { return _delegate; }

@end

void NSExceptionHandlerResume(void)
{
}
