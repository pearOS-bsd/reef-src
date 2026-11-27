// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// OSLog: a store opens for an existing path only and holds no entries.
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSURL : NSObject
+ (id)fileURLWithPath:(NSString *)path;
@end
@interface NSError : NSObject
- (NSInteger)code;
- (NSString *)domain;
@end
@interface NSDate : NSObject
- (double)timeIntervalSinceReferenceDate;
@end
@interface NSEnumerator : NSObject
- (id)nextObject;
@end

@interface OSLogEntry : NSObject
@property (readonly, copy) NSString *composedMessage;
@property (readonly, copy) NSDate *date;
@property (readonly) NSInteger storeCategory;
- (id)initWithCoder:(id)coder;
- (void)encodeWithCoder:(id)coder;
@end
@interface OSLogEntryLog : OSLogEntry
@property (readonly) NSInteger level;
@property (readonly, copy) NSString *subsystem, *category, *process, *sender;
@property (readonly) int processIdentifier;
@property (readonly) NSUInteger threadIdentifier, activityIdentifier;
@end
@interface OSLogStore : NSObject
+ (id)localStoreAndReturnError:(NSError **)error;
+ (id)storeWithScope:(NSInteger)scope error:(NSError **)error;
+ (id)storeWithURL:(NSURL *)url error:(NSError **)error;
- (id)entriesEnumeratorWithOptions:(NSUInteger)options position:(id)position predicate:(id)predicate error:(NSError **)error;
- (id)entriesEnumeratorAndReturnError:(NSError **)error;
- (id)positionWithDate:(id)date;
- (id)positionWithTimeIntervalSinceEnd:(double)interval;
- (id)positionWithTimeIntervalSinceLatestBoot:(double)interval;
@end

int main(void)
{
    NSError *e = nil;
    OSLogStore *local = [OSLogStore localStoreAndReturnError:&e];
    printf("local %d %d\n", local != nil, e == nil);
    printf("scope %d\n", [OSLogStore storeWithScope:1 error:&e] != nil);
    OSLogStore *missing = [OSLogStore storeWithURL:[NSURL fileURLWithPath:@"/nonexistent/store.logarchive"] error:&e];
    printf("missing %d %ld %s\n", missing == nil, (long)[e code], [[e domain] UTF8String]);
    printf("missing without error %d\n", [OSLogStore storeWithURL:[NSURL fileURLWithPath:@"/nonexistent"] error:NULL] == nil);
    OSLogStore *root = [OSLogStore storeWithURL:[NSURL fileURLWithPath:@"/"] error:&e];
    printf("root %d\n", root != nil);
    NSEnumerator *en = [local entriesEnumeratorAndReturnError:&e];
    printf("enumerator %d %d\n", en != nil, [en nextObject] == nil);
    printf("positions %d %d %d\n", [local positionWithDate:nil] != nil, [local positionWithTimeIntervalSinceEnd:-5.0] != nil, [local positionWithTimeIntervalSinceLatestBoot:5.0] != nil);
    OSLogEntryLog *entry = [OSLogEntryLog new];
    printf("entry %lu %d %ld\n", (unsigned long)[entry.composedMessage length], [entry.date timeIntervalSinceReferenceDate] < 0, (long)entry.storeCategory);
    printf("log %ld %d %d %d %d %d %lu %lu\n", (long)entry.level, entry.subsystem == nil, entry.category == nil, entry.process == nil, entry.sender == nil, entry.processIdentifier,
           (unsigned long)entry.threadIdentifier, (unsigned long)entry.activityIdentifier);
    return 0;
}
