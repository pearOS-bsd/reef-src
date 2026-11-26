// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Declarations of the Foundation classes that CoreFoundation messages (written by tools/macho-gen-cf-objc.py from the CF_OBJC_* calls).
#pragma once
@class NSArray, NSMutableArray, NSAttributedString, NSMutableAttributedString, NSCalendar, NSCharacterSet, NSMutableCharacterSet, NSData, NSMutableData, NSDate, NSDictionary, NSMutableDictionary, NSError, NSLocale, NSNumber, NSTimer, NSSet, NSMutableSet, NSInputStream, NSOutputStream, NSString, NSMutableString, NSTimeZone, NSURL;
@interface NSArray : NSObject
- (CFIndex)count;
- (void)getObjects:(id *)a0 range:(NSRange)a1;
@end
@interface NSMutableArray : NSArray
- (void)addObject:(id)a0;
- (void)setObject:(id)a0 atIndex:(NSUInteger)a1;
- (void)insertObject:(id)a0 atIndex:(NSUInteger)a1;
- (void)exchangeObjectAtIndex:(NSUInteger)a0 withObjectAtIndex:(NSUInteger)a1;
- (void)removeObjectAtIndex:(NSUInteger)a0;
- (void)removeAllObjects;
- (void)replaceObjectsInRange:(NSRange)a0 withObjects:(id *)a1 count:(NSUInteger)a2;
- (id)isKindOfClass:(id)a0;
@end
@interface NSAttributedString : NSObject
- (CFStringRef)string;
- (CFIndex)length;
- (CFDictionaryRef)attributesAtIndex:(NSUInteger)a0 effectiveRange:(NSRange *)a1;
- (CFTypeRef)attribute:(NSString *)a0 atIndex:(NSUInteger)a1 effectiveRange:(NSRange *)a2;
- (CFDictionaryRef)attributesAtIndex:(NSUInteger)a0 longestEffectiveRange:(NSRange *)a1 inRange:(NSRange)a2;
- (CFTypeRef)attribute:(NSString *)a0 atIndex:(NSUInteger)a1 longestEffectiveRange:(NSRange *)a2 inRange:(NSRange)a3;
@end
@interface NSMutableAttributedString : NSAttributedString
- (CFMutableStringRef)mutableString;
- (void)replaceCharactersInRange:(NSRange)a0 withString:(NSString *)a1;
- (void)setAttributes:(NSDictionary *)a0 range:(NSRange)a1;
- (void)addAttributes:(NSDictionary *)a0 range:(NSRange)a1;
- (void)addAttribute:(NSString *)a0 value:(id)a1 range:(NSRange)a2;
- (void)removeAttribute:(NSString *)a0 range:(NSRange)a1;
- (void)replaceCharactersInRange:(NSRange)a0 withAttributedString:(NSAttributedString *)a1;
- (void)beginEditing;
- (void)endEditing;
@end
@interface NSCalendar : NSObject
- (CFStringRef)calendarIdentifier;
- (CFLocaleRef)_copyLocale;
- (void)setLocale:(NSLocale *)a0;
- (CFTimeZoneRef)_copyTimeZone;
- (void)setTimeZone:(NSTimeZone *)a0;
- (CFIndex)firstWeekday;
- (void)setFirstWeekday:(NSUInteger)a0;
- (CFIndex)minimumDaysInFirstWeek;
- (void)setMinimumDaysInFirstWeek:(NSUInteger)a0;
- (CFDateRef)_copyGregorianStartDate;
- (void)_setGregorianStartDate:(NSDate *)a0;
- (CFRange)_minimumRangeOfUnit:(NSCalendarUnit)a0;
- (CFRange)_maximumRangeOfUnit:(NSCalendarUnit)a0;
- (BOOL)_composeAbsoluteTime:(id)a0 atp:(const unsigned char *)a1 componentDesc:(id)a2;
- (BOOL)_decomposeAbsoluteTime:(id)a0 at:(const unsigned char *)a1 componentDesc:(id)a2;
- (BOOL)_addComponents:(id)a0 atp:(id)a1 options:(const unsigned char *)a2 componentDesc:(id)a3;
- (BOOL)_diffComponents:(id)a0 startingAT:(id)a1 resultAT:(id)a2 options:(const unsigned char *)a3 componentDesc:(id)a4;
- (BOOL)_rangeOfUnit:(NSCalendarUnit)a0 startTime:(CFAbsoluteTime *)a1 interval:(CFTimeInterval *)a2 forAT:(CFAbsoluteTime)a3;
- (CFRange)_rangeOfUnit:(NSCalendarUnit)a0 inUnit:(NSCalendarUnit)a1 forAT:(CFAbsoluteTime)a2;
- (CFIndex)_ordinalityOfUnit:(NSCalendarUnit)a0 inUnit:(NSCalendarUnit)a1 forAT:(CFAbsoluteTime)a2;
@end
@interface NSCharacterSet : NSObject
- (CFCharacterSetRef)invertedSet;
- (CFMutableCharacterSetRef)mutableCopy;
- (BOOL)longCharacterIsMember:(UTF32Char)a0;
- (CFCharacterSetRef)_expandedCFCharacterSet;
- (BOOL)hasMemberInPlane:(uint8_t)a0;
- (CFDataRef)_retainedBitmapRepresentation;
@end
@interface NSMutableCharacterSet : NSCharacterSet
- (void)addCharactersInRange:(NSRange)a0;
- (void)removeCharactersInRange:(NSRange)a0;
- (void)addCharactersInString:(NSString *)a0;
- (void)removeCharactersInString:(NSString *)a0;
- (void)formUnionWithCharacterSet:(NSCharacterSet *)a0;
- (void)formIntersectionWithCharacterSet:(NSCharacterSet *)a0;
- (void)invert;
@end
@interface NSData : NSObject
- (CFDataRef)copy;
- (CFIndex)length;
- (const uint8_t *)bytes;
- (void)getBytes:(void *)a0 range:(NSRange)a1;
@end
@interface NSMutableData : NSData
- (uint8_t *)mutableBytes;
- (void)setLength:(NSUInteger)a0;
- (void)increaseLengthBy:(NSUInteger)a0;
- (void)appendBytes:(const void *)a0 length:(NSUInteger)a1;
- (void)replaceBytesInRange:(NSRange)a0 withBytes:(void *)a1 length:(long)a2;
@end
@interface NSDate : NSObject
- (CFTimeInterval)timeIntervalSinceReferenceDate;
- (CFTimeInterval)timeIntervalSinceDate:(NSDate *)a0;
- (CFComparisonResult)compare:(NSDate *)a0;
@end
@interface NSDictionary : NSObject
- (CFIndex)count;
- (CFIndex)countForKey:(id)a0;
- (char)containsKey:(id)a0;
- (void const *)objectForKey:(id)a0;
- (BOOL)__getValue:(id *)a0 forKey:(id)a1;
- (CFIndex)countForObject:(id)a0;
- (char)containsObject:(id)a0;
- (void)getObjects:(id *)a0 andKeys:(id *)a1;
- (void)__apply:(id)a0 context:(void *)a1;
- (void)enumerateKeysAndObjectsWithOptions:(long)a0 usingBlock:(id)a1;
@end
@interface NSMutableDictionary : NSDictionary
- (void)__addObject:(id)a0 forKey:(id)a1;
- (void)replaceObject:(id)a0 forKey:(id)a1;
- (void)__setObject:(id)a0 forKey:(id)a1;
- (void)removeObjectForKey:(id)a0;
- (void)removeAllObjects;
@end
@interface NSError : NSObject
- (CFDictionaryRef)userInfo;
- (CFStringRef)domain;
- (CFIndex)code;
- (id)localizedDescription;
- (id)localizedFailureReason;
- (id)localizedRecoverySuggestion;
@end
@interface NSLocale : NSObject
- (BOOL)_doesNotRequireSpecialCaseHandling;
- (void)_setDoesNotRequireSpecialCaseHandling;
- (CFDictionaryRef)_prefs;
- (CFLocaleRef)copy;
- (CFStringRef)localeIdentifier;
- (CFTypeRef)objectForKey:(id)a0;
- (CFStringRef)_copyDisplayNameForKey:(id)a0 value:(id)a1;
@end
@interface NSNumber : NSObject
- (BOOL)boolValue;
- (CFNumberType)_cfNumberType;
- (BOOL)_getValue:(void *)a0 forType:(CFNumberType)a1;
- (CFComparisonResult)compare:(NSNumber *)a0;
- (CFComparisonResult)_reverseCompare:(NSNumber *)a0;
@end
@interface NSTimer : NSObject
- (CFAbsoluteTime)_cffireTime;
- (CFTimeInterval)timeInterval;
- (void)invalidate;
- (BOOL)isValid;
- (CFTimeInterval)tolerance;
- (void)setTolerance:(id)a0;
@end
@interface NSSet : NSObject
- (CFIndex)count;
- (CFIndex)countForObject:(id)a0;
- (char)containsObject:(id)a0;
- (const void *)member:(id)a0;
- (BOOL)__getValue:(id *)a0 forObj:(id)a1;
- (void)getObjects:(id *)a0;
- (void)__applyValues:(id)a0 context:(void *)a1;
@end
@interface NSMutableSet : NSSet
- (void)addObject:(id)a0;
- (void)replaceObject:(id)a0;
- (void)setObject:(id)a0;
- (void)removeObject:(id)a0;
- (void)removeAllObjects;
@end
@interface NSInputStream : NSObject
- (CFStreamStatus)streamStatus;
- (CFStreamError)_cfStreamError;
- (CFErrorRef)streamError;
- (void)close;
- (BOOL)hasBytesAvailable;
- (CFIndex)read:(uint8_t *)a0 maxLength:(NSUInteger)a1;
- (CFTypeRef)propertyForKey:(NSString *)a0;
- (BOOL)setProperty:(id)a0 forKey:(NSString *)a1;
- (id)open;
- (id)getBuffer:(id)a0 length:(NSUInteger *)a1;
@end
@interface NSOutputStream : NSObject
- (CFStreamStatus)streamStatus;
- (CFStreamError)_cfStreamError;
- (CFErrorRef)streamError;
- (void)close;
- (BOOL)hasSpaceAvailable;
- (CFIndex)write:(const uint8_t *)a0 maxLength:(NSUInteger)a1;
- (CFTypeRef)propertyForKey:(NSString *)a0;
- (BOOL)setProperty:(id)a0 forKey:(NSString *)a1;
- (id)open;
@end
@interface NSString : NSObject
- (BOOL)_encodingCantBeStoredInEightBitCFString;
- (CFIndex)length;
- (UniChar)characterAtIndex:(NSUInteger)a0;
- (void)getCharacters:(unichar *)a0 range:(NSRange)a1;
- (const char *)_fastCStringContents:(id)a0;
- (const UniChar *)_fastCharacterContents;
- (BOOL)_getCString:(id)a0 maxLength:(NSUInteger)a1 encoding:(id)a2;
- (void)getLineStart:(NSUInteger *)a0 end:(NSUInteger *)a1 contentsEnd:(NSUInteger *)a2 forRange:(NSRange)a3;
- (void)getParagraphStart:(NSUInteger *)a0 end:(NSUInteger *)a1 contentsEnd:(NSUInteger *)a2 forRange:(NSRange)a3;
- (CFStringEncoding)_smallestEncodingInCFStringEncoding;
- (CFStringEncoding)_fastestEncodingInCFStringEncoding;
@end
@interface NSMutableString : NSString
- (void)insertString:(NSString *)a0 atIndex:(NSUInteger)a1;
- (void)deleteCharactersInRange:(NSRange)a0;
- (void)replaceCharactersInRange:(NSRange)a0 withString:(NSString *)a1;
- (void)setString:(NSString *)a0;
- (void)appendString:(NSString *)a0;
- (void)appendCharacters:(id)a0 length:(NSUInteger)a1;
- (void)_cfAppendCString:(const unsigned char *)a0 length:(NSInteger)a1;
- (CFIndex)replaceOccurrencesOfString:(NSString *)a0 withString:(NSString *)a1 options:(NSStringCompareOptions)a2 range:(NSRange)a3;
- (void)_cfPad:(id)a0 length:(uint32_t)a1 padIndex:(uint32_t)a2;
- (void)_cfTrim:(id)a0;
- (void)_cfTrimWS;
- (void)_cfLowercase:(const void *)a0;
- (void)_cfUppercase:(const void *)a0;
- (void)_cfCapitalize:(const void *)a0;
- (void)_cfNormalize:(id)a0;
@end
@interface NSTimeZone : NSObject
- (CFStringRef)name;
- (CFDataRef)data;
- (CFTimeInterval)_daylightSavingTimeOffsetForAbsoluteTime:(CFAbsoluteTime)a0;
- (CFTimeInterval)_nextDaylightSavingTimeTransitionAfterAbsoluteTime:(CFAbsoluteTime)a0;
- (CFStringRef)localizedName:(NSTimeZoneNameStyle)a0 locale:(NSLocale *)a1;
@end
@interface NSURL : NSObject
- (CFURLRef)_cfurl;
- (CFStringRef)relativeString;
- (CFURLRef)baseURL;
- (id)absoluteURL;
- (id)scheme;
- (id)host;
- (id)port;
- (id)user;
- (id)password;
- (id)query;
- (id)fragment;
- (id)isFileReferenceURL;
@end
