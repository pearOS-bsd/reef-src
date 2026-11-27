// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Accessibility: the display setting queries (animated images on, nothing else) and AXCustomContent.

#import <Foundation/Foundation.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@interface NSAttributedString : NSObject
- (id)initWithString:(NSString *)string;
- (NSString *)string;
@end

@interface NSCoder : NSObject
- (id)decodeObjectOfClass:(Class)cls forKey:(NSString *)key;
- (NSInteger)decodeIntegerForKey:(NSString *)key;
- (void)encodeObject:(id)object forKey:(NSString *)key;
- (void)encodeInteger:(NSInteger)value forKey:(NSString *)key;
@end

NSString *const AXAnimatedImagesEnabledDidChangeNotification = @"AXAnimatedImagesEnabledDidChangeNotification";
NSString *const AXPrefersHorizontalTextLayoutDidChangeNotification = @"AXPrefersHorizontalTextLayoutDidChangeNotification";
NSString *const AXPrefersNonBlinkingTextInsertionIndicatorDidChangeNotification = @"AXPrefersNonBlinkingTextInsertionIndicatorDidChangeNotification";

BOOL AXAnimatedImagesEnabled(void) { return YES; }
BOOL AXPrefersHorizontalTextLayout(void) { return NO; }
BOOL AXPrefersNonBlinkingTextInsertionIndicator(void) { return NO; }

@interface AXCustomContent : NSObject <NSCopying, NSSecureCoding>
{
    NSAttributedString *_attributedLabel;
    NSAttributedString *_attributedValue;
    NSUInteger _importance;
}
@property (readonly) NSString *label;
@property (readonly) NSString *value;
@property (readonly) NSAttributedString *attributedLabel;
@property (readonly) NSAttributedString *attributedValue;
@property NSUInteger importance;
@property (class, readonly) BOOL supportsSecureCoding;
+ (id)customContentWithAttributedLabel:(NSAttributedString *)label attributedValue:(NSAttributedString *)value;
+ (id)customContentWithLabel:(NSString *)label value:(NSString *)value;
@end

@implementation AXCustomContent

+ (BOOL)supportsSecureCoding { return YES; }

+ (id)customContentWithAttributedLabel:(NSAttributedString *)label attributedValue:(NSAttributedString *)value
{
    AXCustomContent *content = [[[self alloc] init] autorelease];
    content->_attributedLabel = [label copy];
    content->_attributedValue = [value copy];
    return content;
}

+ (id)customContentWithLabel:(NSString *)label value:(NSString *)value
{
    NSAttributedString *attributedLabel = [[[NSAttributedString alloc] initWithString:label ? label : @""] autorelease];
    NSAttributedString *attributedValue = [[[NSAttributedString alloc] initWithString:value ? value : @""] autorelease];
    return [self customContentWithAttributedLabel:attributedLabel attributedValue:attributedValue];
}

- (void)dealloc
{
    [_attributedLabel release];
    [_attributedValue release];
    [super dealloc];
}

- (NSString *)label { return [_attributedLabel string]; }
- (NSString *)value { return [_attributedValue string]; }
- (NSAttributedString *)attributedLabel { return _attributedLabel; }
- (NSAttributedString *)attributedValue { return _attributedValue; }
- (NSUInteger)importance { return _importance; }
- (void)setImportance:(NSUInteger)importance { _importance = importance; }

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super init];
    if (self) {
        _attributedLabel = [[coder decodeObjectOfClass:[NSAttributedString class] forKey:@"label"] copy];
        _attributedValue = [[coder decodeObjectOfClass:[NSAttributedString class] forKey:@"value"] copy];
        _importance = [coder decodeIntegerForKey:@"importance"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_attributedLabel forKey:@"label"];
    [coder encodeObject:_attributedValue forKey:@"value"];
    [coder encodeInteger:_importance forKey:@"importance"];
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

@end
