// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OSLog: the log store exists (the local one, or a file that exists) but holds no entries.

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (id)autorelease;
@end

@class NSDate, NSPredicate;

@interface NSDate : NSObject
+ (id)distantPast;
@end

@interface NSURL : NSObject
- (NSString *)path;
@end

@interface NSFileManager : NSObject
+ (id)defaultManager;
- (BOOL)fileExistsAtPath:(NSString *)path;
@end

@interface NSError : NSObject
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end

@interface NSEnumerator : NSObject
@end

extern NSString *const NSCocoaErrorDomain;
extern NSString *const NSURLErrorKey;

@interface OSLogEntry : NSObject <NSSecureCoding>
@property (readonly, copy) NSString *composedMessage;
@property (readonly, copy) NSDate *date;
@property (readonly) NSInteger storeCategory;
@property (class, readonly) BOOL supportsSecureCoding;
@end

@implementation OSLogEntry

+ (BOOL)supportsSecureCoding { return YES; }

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

- (NSString *)composedMessage { return @""; }
- (NSDate *)date { return [NSDate distantPast]; }
- (NSInteger)storeCategory { return 0; }

@end

@interface OSLogEntryLog : OSLogEntry
{
    int _processIdentifier;
    NSInteger _level;
    NSString *_subsystem;
    NSString *_category;
    NSString *_process;
    NSString *_sender;
    NSUInteger _threadIdentifier;
    NSUInteger _activityIdentifier;
}
@property (readonly) NSInteger level;
@property (readonly, copy) NSString *subsystem;
@property (readonly, copy) NSString *category;
@property (readonly, copy) NSString *process;
@property (readonly) int processIdentifier;
@property (readonly, copy) NSString *sender;
@property (readonly) NSUInteger threadIdentifier;
@property (readonly) NSUInteger activityIdentifier;
@end

@implementation OSLogEntryLog
@end

@interface OSLogPosition : NSObject
@end

@implementation OSLogPosition
@end

@interface OSLogEnumerator : NSEnumerator
@end

@implementation OSLogEnumerator

- (id)nextObject { return nil; }

@end

@interface OSLogStore : NSObject
+ (id)localStoreAndReturnError:(NSError **)error;
+ (id)storeWithScope:(NSInteger)scope error:(NSError **)error;
+ (id)storeWithURL:(NSURL *)url error:(NSError **)error;
- (id)entriesEnumeratorWithOptions:(NSUInteger)options position:(OSLogPosition *)position predicate:(NSPredicate *)predicate error:(NSError **)error;
- (id)entriesEnumeratorAndReturnError:(NSError **)error;
- (id)positionWithDate:(NSDate *)date;
- (id)positionWithTimeIntervalSinceEnd:(double)interval;
- (id)positionWithTimeIntervalSinceLatestBoot:(double)interval;
@end

@implementation OSLogStore

+ (id)localStoreAndReturnError:(NSError **)error
{
    return [[[self alloc] init] autorelease];
}

+ (id)storeWithScope:(NSInteger)scope error:(NSError **)error
{
    return [[[self alloc] init] autorelease];
}

+ (id)storeWithURL:(NSURL *)url error:(NSError **)error
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:[url path]])
        return [[[self alloc] init] autorelease];
    if (error)
        *error = [NSError errorWithDomain:NSCocoaErrorDomain code:260 userInfo:@{NSURLErrorKey: url}];
    return nil;
}

- (id)entriesEnumeratorWithOptions:(NSUInteger)options position:(OSLogPosition *)position predicate:(NSPredicate *)predicate error:(NSError **)error
{
    return [[[OSLogEnumerator alloc] init] autorelease];
}

- (id)entriesEnumeratorAndReturnError:(NSError **)error
{
    return [self entriesEnumeratorWithOptions:0 position:nil predicate:nil error:error];
}

- (id)positionWithDate:(NSDate *)date
{
    return [[[OSLogPosition alloc] init] autorelease];
}

- (id)positionWithTimeIntervalSinceEnd:(double)interval
{
    return [[[OSLogPosition alloc] init] autorelease];
}

- (id)positionWithTimeIntervalSinceLatestBoot:(double)interval
{
    return [[[OSLogPosition alloc] init] autorelease];
}

@end
