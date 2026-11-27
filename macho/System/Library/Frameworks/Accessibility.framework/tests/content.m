// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Accessibility: the setting queries and custom content (labels, values, copying, archiving).
#import <Foundation/Foundation.h>
#include <stdio.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSAttributedString : NSObject
- (id)initWithString:(NSString *)string;
- (NSString *)string;
@end
@interface NSData : NSObject
@end
@interface NSKeyedArchiver : NSObject
+ (NSData *)archivedDataWithRootObject:(id)object requiringSecureCoding:(BOOL)secure error:(NSError **)error;
@end
@interface NSKeyedUnarchiver : NSObject
+ (id)unarchivedObjectOfClass:(Class)cls fromData:(NSData *)data error:(NSError **)error;
@end
@interface NSError : NSObject
@end

extern NSString *const AXAnimatedImagesEnabledDidChangeNotification, *const AXPrefersHorizontalTextLayoutDidChangeNotification, *const AXPrefersNonBlinkingTextInsertionIndicatorDidChangeNotification;
BOOL AXAnimatedImagesEnabled(void);
BOOL AXPrefersHorizontalTextLayout(void);
BOOL AXPrefersNonBlinkingTextInsertionIndicator(void);

@interface AXCustomContent : NSObject <NSCopying>
@property (readonly) NSString *label, *value;
@property (readonly) NSAttributedString *attributedLabel, *attributedValue;
@property NSUInteger importance;
+ (BOOL)supportsSecureCoding;
+ (id)customContentWithAttributedLabel:(NSAttributedString *)label attributedValue:(NSAttributedString *)value;
+ (id)customContentWithLabel:(NSString *)label value:(NSString *)value;
@end

int main(void)
{
    printf("settings %d %d %d\n", AXAnimatedImagesEnabled(), AXPrefersHorizontalTextLayout(), AXPrefersNonBlinkingTextInsertionIndicator());
    printf("names %s %s %s\n", [AXAnimatedImagesEnabledDidChangeNotification UTF8String], [AXPrefersHorizontalTextLayoutDidChangeNotification UTF8String],
           [AXPrefersNonBlinkingTextInsertionIndicatorDidChangeNotification UTF8String]);
    AXCustomContent *c = [AXCustomContent customContentWithLabel:@"Name" value:@"Value"];
    c.importance = 1;
    printf("content %s %s %lu %d\n", [c.label UTF8String], [c.value UTF8String], (unsigned long)c.importance, [AXCustomContent supportsSecureCoding]);
    AXCustomContent *empty = [AXCustomContent customContentWithLabel:nil value:nil];
    printf("empty '%s' '%s'\n", [empty.label UTF8String], [empty.value UTF8String]);
    AXCustomContent *attributed = [AXCustomContent customContentWithAttributedLabel:[[NSAttributedString alloc] initWithString:@"A"] attributedValue:[[NSAttributedString alloc] initWithString:@"B"]];
    printf("attributed %s %s copy %d\n", [attributed.label UTF8String], [attributed.value UTF8String], [attributed copy] == attributed);
    NSError *e = nil;
    NSData *data = [NSKeyedArchiver archivedDataWithRootObject:c requiringSecureCoding:YES error:&e];
    AXCustomContent *back = [NSKeyedUnarchiver unarchivedObjectOfClass:[AXCustomContent class] fromData:data error:&e];
    printf("archive %d %s %s %lu\n", data != nil, [back.label UTF8String], [back.value UTF8String], (unsigned long)back.importance);
    return 0;
}
