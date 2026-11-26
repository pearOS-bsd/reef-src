// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Objective-C part of Calculate.framework (private framework of macOS): result, unit and currency classes.
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NSString *CalculateKey NS_EXTENSIBLE_STRING_ENUM;
extern CalculateKey const CalculateKeyAllowEqualSign;
extern CalculateKey const CalculateKeyAllowPartialExpressions;
extern CalculateKey const CalculateKeyAllowStaleCurrencyConversions;
extern CalculateKey const CalculateKeyAssumeDegrees;
extern CalculateKey const CalculateKeyConvertTrivial;
extern CalculateKey const CalculateKeyLocales;
extern CalculateKey const CalculateKeyNumberFormatter;
extern CalculateKey const CalculateKeyScientificNotationFormat;
extern CalculateKey const CalculateKeyVariables;

// Plain C enums (they import into Swift as structs).
typedef enum CalculateEngine { CalculateEngineDefault = 0, CalculateEngineBasic = 1, CalculateEngineScientific = 2, CalculateEngineProgrammer = 3 } CalculateEngine;
typedef enum CalculateFormat { CalculateFormatDefault = 0, CalculateFormatDecimal = 1, CalculateFormatScientific = 2, CalculateFormatEngineering = 3 } CalculateFormat;

@class CalculateUnit, CalculateUnitCategory, CalculateTerm;

@interface CalculateUnit : NSObject
@property (nonatomic, readonly) NSString *name;
@property (nonatomic, readonly) NSString *shortName;
@property (nonatomic, readonly) NSString *unitType;
@property (nonatomic, readonly) double factor;
@property (nonatomic, readonly) double offset;
- (instancetype)initWithName:(NSString *)name shortName:(NSString *)shortName type:(NSString *)type factor:(double)factor offset:(double)offset;
@end

@interface CalculateUnitCategory : NSObject
@property (nonatomic, readonly) NSString *name;
@property (nonatomic, readonly) NSArray<CalculateUnit *> *units;
@property (nonatomic, readonly, nullable) CalculateUnit *preferredFromUnit;
@property (nonatomic, readonly, nullable) CalculateUnit *preferredToUnit;
- (instancetype)initWithName:(NSString *)name units:(NSArray<CalculateUnit *> *)units;
@end

@interface CalculateUnitCollection : NSObject
@property (class, nonatomic, readonly) CalculateUnitCollection *shared;
@property (nonatomic, readonly) NSArray<CalculateUnitCategory *> *categories;
- (nullable CalculateUnitCategory *)findCategoryWithName:(NSString *)name;
- (nullable CalculateUnit *)findUnitWithName:(NSString *)name;
@end

@interface CalculateTerm : NSObject
@property (nonatomic, readonly) double value;
@property (nonatomic, readonly, nullable) CalculateUnit *unit;
@property (nonatomic, readonly) NSString *formattedValue;
- (instancetype)initWithValue:(double)value unit:(nullable CalculateUnit *)unit;
@end

@interface CalculateResult : NSObject
@property (nonatomic, readonly) double value;
@property (nonatomic, readonly) NSArray<CalculateTerm *> *terms;
@property (nonatomic, readonly) NSString *formattedResult;
@property (nonatomic, readonly) NSString *formattedValue;
@property (nonatomic, readonly) BOOL isPartialExpression;
@property (nonatomic, readonly) BOOL isTrivial;
@property (nonatomic, readonly, nullable) CalculateTerm *inputValueAndUnit;
- (instancetype)initWithValue:(double)value;
- (nullable CalculateResult *)convertedTo:(CalculateUnit *)unit;
@end

@interface CalculateCurrencyCache : NSObject
@property (class, nonatomic, readonly) CalculateCurrencyCache *shared;
@property (nonatomic, readonly) BOOL needsRefresh;
@property (nonatomic, readonly, nullable) NSDate *lastRefreshDate;
@property (nonatomic) BOOL synchronous;
- (void)refreshIfNeeded;
@end

NS_ASSUME_NONNULL_END
