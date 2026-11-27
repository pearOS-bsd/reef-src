// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// ExceptionHandling: the handler object keeps its masks and delegate; its uncaught exception handler asks the delegate and calls the previous handler.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSException : NSObject
+ (id)exceptionWithName:(id)name reason:(id)reason userInfo:(id)info;
@end
typedef void NSUncaughtExceptionHandler(NSException *exception);
extern NSUncaughtExceptionHandler *NSGetUncaughtExceptionHandler(void);
extern void NSSetUncaughtExceptionHandler(NSUncaughtExceptionHandler *handler);

@interface NSExceptionHandler : NSObject
+ (id)defaultExceptionHandler;
- (void)setExceptionHandlingMask:(NSUInteger)mask;
- (NSUInteger)exceptionHandlingMask;
- (void)setExceptionHangingMask:(NSUInteger)mask;
- (NSUInteger)exceptionHangingMask;
- (void)setDelegate:(id)delegate;
- (id)delegate;
@end

@interface NSString (U)
- (const char *)UTF8String;
@end
extern NSString *const NSUncaughtSystemExceptionException, *const NSUncaughtRuntimeErrorException, *const NSStackTraceKey;

static void previous(NSException *exception) { printf("previous handler\n"); }

@interface Delegate : NSObject
@end
@implementation Delegate
- (BOOL)exceptionHandler:(id)sender shouldLogException:(NSException *)exception mask:(NSUInteger)mask
{
    printf("delegate asked, mask %lu\n", (unsigned long)mask);
    return NO;
}
@end

int main(void)
{
    NSExceptionHandler *handler = [NSExceptionHandler defaultExceptionHandler];
    printf("same %d\n", handler == [NSExceptionHandler defaultExceptionHandler]);
    printf("names %s %s %s\n", [NSUncaughtSystemExceptionException UTF8String], [NSUncaughtRuntimeErrorException UTF8String], [NSStackTraceKey UTF8String]);
    NSSetUncaughtExceptionHandler(previous);
    printf("mask %lu\n", (unsigned long)[handler exceptionHandlingMask]);
    Delegate *delegate = [Delegate new];
    [handler setDelegate:delegate];
    [handler setExceptionHangingMask:5];
    [handler setExceptionHandlingMask:1];
    printf("masks %lu %lu delegate %d\n", (unsigned long)[handler exceptionHandlingMask], (unsigned long)[handler exceptionHangingMask], [handler delegate] == delegate);
    NSUncaughtExceptionHandler *installed = NSGetUncaughtExceptionHandler();
    printf("installed %d\n", installed != previous);
    installed([NSException exceptionWithName:@"Test" reason:@"reason" userInfo:nil]);
    [handler setExceptionHandlingMask:0];
    installed([NSException exceptionWithName:@"Test" reason:@"reason" userInfo:nil]);
    return 0;
}
