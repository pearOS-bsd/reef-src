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

static NSString *Superscript(NSString *digits) {
    NSString *map[] = {@"⁰", @"¹", @"²", @"³", @"⁴", @"⁵", @"⁶", @"⁷", @"⁸", @"⁹"};
    NSMutableString *out = [NSMutableString string];
    for (NSUInteger i = 0; i < digits.length; i++) {
        unichar c = [digits characterAtIndex:i];
        if (c == '-') [out appendString:@"⁻"];
        else if (c >= '0' && c <= '9') [out appendString:map[c - '0']];
    }
    return out;
}

// Default formatting of Apple's engine as observed: grouping, at least 3 fraction digits (3 significant digits
// below 1), scientific beyond 10 integer digits.
static NSString *CalcFormatDecimal(NSDecimalNumber *number, NSNumberFormatter *formatter) {
    if (formatter) {
        NSString *s = [formatter stringFromNumber:number];
        if (s) return s;
    }
    double v = number.doubleValue;
    if (v == 0) return @"0";
    double a = fabs(v);
    int exponent = (int)floor(log10(a));
    if (exponent >= 10 || exponent < -9) {
        int digits = exponent >= 100 || exponent <= -100 ? 4 : 3;
        double m = v / pow(10, exponent);
        NSString *mant = [NSString stringWithFormat:@"%.*f", digits, m];
        if ([mant hasPrefix:@"10."] || [mant hasPrefix:@"-10."]) { exponent++; mant = [NSString stringWithFormat:@"%.*f", digits, v / pow(10, exponent)]; }
        while ([mant hasSuffix:@"0"]) mant = [mant substringToIndex:mant.length - 1];
        if ([mant hasSuffix:@"."]) mant = [mant substringToIndex:mant.length - 1];
        return [NSString stringWithFormat:@"%@×10%@", mant, Superscript([NSString stringWithFormat:@"%d", exponent])];
    }
    NSInteger fraction = MAX(3, -exponent + 2);
    NSNumberFormatter *f = [NSNumberFormatter new];
    f.locale = [NSLocale localeWithLocaleIdentifier:@"en_US"];
    f.numberStyle = NSNumberFormatterDecimalStyle;
    f.usesGroupingSeparator = YES;
    f.minimumFractionDigits = 0;
    f.maximumFractionDigits = fraction;
    f.roundingMode = NSNumberFormatterRoundHalfUp;
    return [f stringFromNumber:number];
}

static NSString *CalcFormat(double v) { return CalcFormatDecimal([NSDecimalNumber numberWithDouble:v], nil); }

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

@implementation CalculateResult {
    NSNumberFormatter *_formatter;
}
- (instancetype)initWithValue:(double)value { return [self initWithDecimal:[NSDecimalNumber numberWithDouble:value] formatter:nil]; }
- (instancetype)initWithDecimal:(NSDecimalNumber *)decimal formatter:(NSNumberFormatter *)formatter {
    if ((self = [super init])) { _decimal = decimal; _value = decimal.doubleValue; _formatter = formatter; }
    return self;
}
- (NSArray<CalculateTerm *> *)terms { return @[[[CalculateTerm alloc] initWithValue:_value unit:nil]]; }
- (NSString *)formattedValue { return CalcFormatDecimal(_decimal, _formatter); }
- (NSString *)formattedResult { return CalcFormatDecimal(_decimal, _formatter); }
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
