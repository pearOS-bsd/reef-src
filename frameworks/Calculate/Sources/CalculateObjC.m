// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <CalculateObjC.h>

CalculateKey const CalculateKeyAllowEqualSign = @"CalculateKeyAllowEqualSign";
CalculateKey const CalculateKeyAllowPartialExpressions = @"CalculateKeyAllowPartialExpressions";
CalculateKey const CalculateKeyAllowStaleCurrencyConversions = @"CalculateKeyAllowStaleCurrencyConversions";
CalculateKey const CalculateKeyAssumeDegrees = @"CalculateKeyAssumeDegrees";
CalculateKey const CalculateKeyConvertTrivial = @"CalculateKeyConvertTrivial";
CalculateKey const CalculateKeyLocales = @"CalculateKeyLocales";
CalculateKey const CalculateKeyNumberFormatter = @"CalculateKeyNumberFormatter";
CalculateKey const CalculateKeyScientificNotationFormat = @"CalculateKeyScientificNotationFormat";
CalculateKey const CalculateKeyVariables = @"CalculateKeyVariables";

static NSString *CalcFormat(double v) {
    if (v == (long long)v && v < 1e15 && v > -1e15) return [NSString stringWithFormat:@"%lld", (long long)v];
    NSNumberFormatter *f = [NSNumberFormatter new];
    f.maximumFractionDigits = 9;
    f.minimumFractionDigits = 0;
    f.numberStyle = NSNumberFormatterDecimalStyle;
    f.usesGroupingSeparator = NO;
    return [f stringFromNumber:@(v)];
}

@implementation CalculateUnit
- (instancetype)initWithName:(NSString *)name shortName:(NSString *)shortName type:(NSString *)type factor:(double)factor offset:(double)offset {
    if ((self = [super init])) { _name = [name copy]; _shortName = [shortName copy]; _unitType = [type copy]; _factor = factor; _offset = offset; }
    return self;
}
@end

@implementation CalculateUnitCategory
- (instancetype)initWithName:(NSString *)name units:(NSArray<CalculateUnit *> *)units {
    if ((self = [super init])) { _name = [name copy]; _units = [units copy]; }
    return self;
}
- (CalculateUnit *)preferredFromUnit { return self.units.firstObject; }
- (CalculateUnit *)preferredToUnit { return self.units.count > 1 ? self.units[1] : self.units.firstObject; }
@end

@implementation CalculateUnitCollection {
    NSArray<CalculateUnitCategory *> *_categories;
}
+ (CalculateUnitCollection *)shared {
    static CalculateUnitCollection *s;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ s = [CalculateUnitCollection new]; });
    return s;
}
- (instancetype)init {
    if ((self = [super init])) {
        CalculateUnit *m = [[CalculateUnit alloc] initWithName:@"meter" shortName:@"m" type:@"length" factor:1 offset:0];
        CalculateUnit *km = [[CalculateUnit alloc] initWithName:@"kilometer" shortName:@"km" type:@"length" factor:1000 offset:0];
        CalculateUnit *mi = [[CalculateUnit alloc] initWithName:@"mile" shortName:@"mi" type:@"length" factor:1609.344 offset:0];
        _categories = @[[[CalculateUnitCategory alloc] initWithName:@"Length" units:@[m, km, mi]]];
    }
    return self;
}
- (NSArray<CalculateUnitCategory *> *)categories { return _categories; }
- (CalculateUnitCategory *)findCategoryWithName:(NSString *)name {
    for (CalculateUnitCategory *c in _categories) if ([c.name caseInsensitiveCompare:name] == NSOrderedSame) return c;
    return nil;
}
- (CalculateUnit *)findUnitWithName:(NSString *)name {
    for (CalculateUnitCategory *c in _categories)
        for (CalculateUnit *u in c.units)
            if ([u.name caseInsensitiveCompare:name] == NSOrderedSame || [u.shortName isEqualToString:name]) return u;
    return nil;
}
@end

@implementation CalculateTerm
- (instancetype)initWithValue:(double)value unit:(CalculateUnit *)unit {
    if ((self = [super init])) { _value = value; _unit = unit; }
    return self;
}
- (NSString *)formattedValue { return CalcFormat(_value); }
@end

@implementation CalculateResult
- (instancetype)initWithValue:(double)value { if ((self = [super init])) _value = value; return self; }
- (NSArray<CalculateTerm *> *)terms { return @[[[CalculateTerm alloc] initWithValue:_value unit:nil]]; }
- (NSString *)formattedValue { return CalcFormat(_value); }
- (NSString *)formattedResult { return CalcFormat(_value); }
- (BOOL)isPartialExpression { return NO; }
- (BOOL)isTrivial { return YES; }
- (CalculateTerm *)inputValueAndUnit { return nil; }
- (CalculateResult *)convertedTo:(CalculateUnit *)unit { return nil; }
@end

@implementation CalculateCurrencyCache
+ (CalculateCurrencyCache *)shared {
    static CalculateCurrencyCache *s;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ s = [CalculateCurrencyCache new]; });
    return s;
}
- (BOOL)needsRefresh { return NO; }
- (NSDate *)lastRefreshDate { return nil; }
- (void)refreshIfNeeded {}
@end
