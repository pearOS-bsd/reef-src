// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <AppKit/AppKit.h>

NSAppearanceName const NSAppearanceNameTouchBar = @"NSAppearanceNameTouchBar";

// Shows the standard About panel: options are the About panel's own dictionary.
void NSShowSystemInfoPanel(NSDictionary *options) {
    [NSApp orderFrontStandardAboutPanelWithOptions:options ?: @{}];
}

@interface NSUserInterfaceCompressionOptions () { @public NSSet<NSString *> *_ids; }
@end

@implementation NSUserInterfaceCompressionOptions
+ (BOOL)supportsSecureCoding { return YES; }
static NSUserInterfaceCompressionOptions *Make(NSString *i) { return [[NSUserInterfaceCompressionOptions alloc] initWithIdentifier:i]; }
+ (NSUserInterfaceCompressionOptions *)hideImagesOption { return Make(@"NSUserInterfaceCompressionOptionHideImages"); }
+ (NSUserInterfaceCompressionOptions *)hideTextOption { return Make(@"NSUserInterfaceCompressionOptionHideText"); }
+ (NSUserInterfaceCompressionOptions *)reduceMetricsOption { return Make(@"NSUserInterfaceCompressionOptionReduceMetrics"); }
+ (NSUserInterfaceCompressionOptions *)breakEqualWidthsOption { return Make(@"NSUserInterfaceCompressionOptionBreakEqualWidths"); }
+ (NSUserInterfaceCompressionOptions *)standardOptions {
    return [[self hideImagesOption] optionsByAddingOptions:[[self hideTextOption] optionsByAddingOptions:[[self reduceMetricsOption] optionsByAddingOptions:[self breakEqualWidthsOption]]]];
}
- (instancetype)init { if ((self = [super init])) _ids = [NSSet set]; return self; }
- (instancetype)initWithIdentifier:(NSString *)identifier { if ((self = [super init])) _ids = [NSSet setWithObject:identifier]; return self; }
- (instancetype)initWithCompressionOptions:(NSSet<NSUserInterfaceCompressionOptions *> *)options {
    if ((self = [super init])) {
        NSMutableSet *s = [NSMutableSet set];
        for (NSUserInterfaceCompressionOptions *o in options) [s unionSet:o->_ids];
        _ids = s;
    }
    return self;
}
- (BOOL)isEmpty { return _ids.count == 0; }
- (BOOL)containsOptions:(NSUserInterfaceCompressionOptions *)o { return [o->_ids isSubsetOfSet:_ids]; }
- (BOOL)intersectsOptions:(NSUserInterfaceCompressionOptions *)o { return [_ids intersectsSet:o->_ids]; }
- (NSUserInterfaceCompressionOptions *)optionsByAddingOptions:(NSUserInterfaceCompressionOptions *)o {
    NSUserInterfaceCompressionOptions *r = [NSUserInterfaceCompressionOptions new]; r->_ids = [_ids setByAddingObjectsFromSet:o->_ids]; return r;
}
- (NSUserInterfaceCompressionOptions *)optionsByRemovingOptions:(NSUserInterfaceCompressionOptions *)o {
    NSUserInterfaceCompressionOptions *r = [NSUserInterfaceCompressionOptions new];
    NSMutableSet *s = [_ids mutableCopy]; [s minusSet:o->_ids]; r->_ids = s; return r;
}
- (id)copyWithZone:(NSZone *)z { return self; }
- (void)encodeWithCoder:(NSCoder *)c { [c encodeObject:_ids.allObjects forKey:@"ids"]; }
- (instancetype)initWithCoder:(NSCoder *)c {
    if ((self = [super init])) _ids = [NSSet setWithArray:[c decodeObjectOfClasses:[NSSet setWithObjects:NSArray.class, NSString.class, nil] forKey:@"ids"] ?: @[]];
    return self;
}
@end
