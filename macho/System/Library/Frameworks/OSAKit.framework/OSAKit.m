// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// OSAKit: the one language, AppleScript, and scripts that wrap an NSAppleScript; error information is
// renamed from the NSAppleScriptError keys to the OSAScriptError keys.

#import <Foundation/Foundation.h>
#include <dispatch/dispatch.h>
#include <stdbool.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@interface NSString (OSAKit)
- (BOOL)writeToURL:(NSURL *)url atomically:(BOOL)atomically encoding:(NSUInteger)encoding error:(NSError **)error;
@end
@interface NSError : NSObject
- (NSInteger)code;
- (NSString *)localizedDescription;
@end
@interface NSAttributedString : NSObject
- (id)initWithString:(NSString *)string;
@end
@interface NSMutableDictionary : NSDictionary
+ (id)dictionary;
- (void)setObject:(id)object forKeyedSubscript:(id)key;
@end
@interface NSDictionary (OSAKit)
+ (id)dictionaryWithObjects:(const id *)objects forKeys:(const id *)keys count:(NSUInteger)count;
- (id)objectForKeyedSubscript:(id)key;
- (void)enumerateKeysAndObjectsUsingBlock:(void (^)(id key, id object, BOOL *stop))block;
@end
@interface NSArray (OSAKit)
+ (id)arrayWithObjects:(const id *)objects count:(NSUInteger)count;
@end
@interface NSAppleScript : NSObject
- (id)initWithSource:(NSString *)source;
- (id)initWithContentsOfURL:(NSURL *)url error:(NSDictionary **)error;
- (NSString *)source;
- (BOOL)isCompiled;
- (BOOL)compileAndReturnError:(NSDictionary **)error;
- (id)executeAndReturnError:(NSDictionary **)error;
@end
@interface NSData : NSObject
@end
@interface NSObject (OSAKit)
- (id)data;
- (id)stringValue;
@end

extern NSString *const NSAppleScriptErrorMessage;
extern NSString *const NSAppleScriptErrorBriefMessage;
extern NSString *const NSAppleScriptErrorNumber;
extern NSString *const NSAppleScriptErrorAppName;
extern NSString *const NSAppleScriptErrorRange;

typedef long dispatch_once_t;
extern void dispatch_once(dispatch_once_t *predicate, void (^block)(void));
// The SDK's inline fast path in front of the library function.
static inline void _dispatch_once(dispatch_once_t *predicate, void (^block)(void))
{
    if (__builtin_expect(*predicate, ~0l) != ~0l)
        dispatch_once(predicate, block);
}
#define dispatch_once _dispatch_once

@class OSALanguageInstance;

@interface OSALanguage : NSObject
{
    NSString *_name;
}
+ (id)availableLanguages;
+ (id)languageForName:(NSString *)name;
+ (id)languageForScriptDataDescriptor:(id)descriptor;
+ (id)defaultLanguage;
+ (void)setDefaultLanguage:(OSALanguage *)language;
- (OSALanguageInstance *)sharedLanguageInstance;
- (NSString *)name;
- (NSString *)info;
- (NSString *)version;
- (unsigned int)type;
- (unsigned int)subType;
- (unsigned int)manufacturer;
- (unsigned long)features;
- (BOOL)isThreadSafe;
@end

@implementation OSALanguage
+ (id)availableLanguages
{
    return @[[self defaultLanguage]];
}

+ (id)languageForName:(NSString *)name
{
    if ([name isEqual:@"AppleScript"])
        return [self defaultLanguage];
    return nil;
}

+ (id)languageForScriptDataDescriptor:(id)descriptor
{
    return [self defaultLanguage];
}

+ (id)defaultLanguage
{
    static OSALanguage *language;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        language = [[OSALanguage alloc] init];
        language->_name = @"AppleScript";
    });
    return language;
}

+ (void)setDefaultLanguage:(OSALanguage *)language
{
}

- (OSALanguageInstance *)sharedLanguageInstance
{
    return [[[OSALanguageInstance alloc] init] autorelease];
}

- (NSString *)name { return _name; }
- (NSString *)info { return @"AppleScript"; }
- (NSString *)version { return @"2.8"; }
- (unsigned int)type { return 'ascr'; }
- (unsigned int)subType { return 'ascr'; }
- (unsigned int)manufacturer { return 'aapl'; }
- (unsigned long)features { return 0x1e; }
- (BOOL)isThreadSafe { return NO; }
@end

@interface OSALanguageInstance : NSObject
+ (id)languageInstanceWithLanguage:(OSALanguage *)language;
- (id)initWithLanguage:(OSALanguage *)language;
- (OSALanguage *)language;
- (id)defaultTarget;
- (void)setDefaultTarget:(id)target;
- (id)richTextFromDescriptor:(id)descriptor;
@end

@implementation OSALanguageInstance
+ (id)languageInstanceWithLanguage:(OSALanguage *)language
{
    return [[[self alloc] initWithLanguage:language] autorelease];
}

- (id)initWithLanguage:(OSALanguage *)language
{
    return [self init];
}

- (OSALanguage *)language { return [OSALanguage defaultLanguage]; }
- (id)defaultTarget { return nil; }
- (void)setDefaultTarget:(id)target {}
- (id)richTextFromDescriptor:(id)descriptor { return descriptor; }
@end

// The error dictionary of an NSAppleScript with its keys renamed to the OSAScriptError keys.
static NSDictionary *osaErrorInfo(NSDictionary *info)
{
    if (!info)
        return nil;
    NSMutableDictionary *result = [NSMutableDictionary dictionary];
    NSDictionary *names = @{
        NSAppleScriptErrorMessage: @"OSAScriptErrorMessageKey",
        NSAppleScriptErrorBriefMessage: @"OSAScriptErrorBriefMessageKey",
        NSAppleScriptErrorNumber: @"OSAScriptErrorNumberKey",
        NSAppleScriptErrorAppName: @"OSAScriptErrorAppNameKey",
        NSAppleScriptErrorRange: @"OSAScriptErrorRangeKey",
    };
    [info enumerateKeysAndObjectsUsingBlock:^(NSString *key, id object, BOOL *stop) {
        result[names[key] ?: key] = object;
    }];
    return result;
}

@interface OSAScript : NSObject <NSCopying>
{
    NSAppleScript *_script;
    NSURL *_url;
    OSALanguageInstance *_instance;
}
+ (id)scriptDataDescriptorWithContentsOfURL:(NSURL *)url;
- (id)initWithSource:(NSString *)source;
- (id)initWithSource:(NSString *)source language:(OSALanguage *)language;
- (id)initWithSource:(NSString *)source languageInstance:(OSALanguageInstance *)instance;
- (id)initWithSource:(NSString *)source fromURL:(NSURL *)url languageInstance:(OSALanguageInstance *)instance usingStorageOptions:(unsigned long)options;
- (id)initWithContentsOfURL:(NSURL *)url error:(NSDictionary **)error;
- (id)initWithContentsOfURL:(NSURL *)url language:(OSALanguage *)language error:(NSDictionary **)error;
- (id)initWithContentsOfURL:(NSURL *)url languageInstance:(OSALanguageInstance *)instance usingStorageOptions:(unsigned long)options error:(NSDictionary **)error;
- (id)initWithCompiledData:(NSData *)data error:(NSDictionary **)error;
- (id)initWithCompiledData:(NSData *)data fromURL:(NSURL *)url usingStorageOptions:(unsigned long)options error:(NSDictionary **)error;
- (id)initWithScriptDataDescriptor:(id)descriptor fromURL:(NSURL *)url languageInstance:(OSALanguageInstance *)instance usingStorageOptions:(unsigned long)options error:(NSDictionary **)error;
- (NSString *)source;
- (NSURL *)url;
- (OSALanguage *)language;
- (void)setLanguage:(OSALanguage *)language;
- (OSALanguageInstance *)languageInstance;
- (void)setLanguageInstance:(OSALanguageInstance *)instance;
- (BOOL)isCompiled;
- (BOOL)compileAndReturnError:(NSDictionary **)error;
- (id)executeAndReturnError:(NSDictionary **)error;
- (id)executeAppleEvent:(id)event error:(NSDictionary **)error;
- (id)executeAndReturnDisplayValue:(id *)displayValue error:(NSDictionary **)error;
- (id)executeHandlerWithName:(NSString *)name arguments:(NSArray *)arguments error:(NSDictionary **)error;
- (id)richTextSource;
- (id)richTextFromDescriptor:(id)descriptor;
- (BOOL)writeToURL:(NSURL *)url ofType:(NSString *)type error:(NSDictionary **)error;
- (BOOL)writeToURL:(NSURL *)url ofType:(NSString *)type usingStorageOptions:(unsigned long)options error:(NSDictionary **)error;
- (id)compiledDataForType:(NSString *)type usingStorageOptions:(unsigned long)options error:(NSDictionary **)error;
@end

@implementation OSAScript
+ (id)scriptDataDescriptorWithContentsOfURL:(NSURL *)url { return nil; }

- (id)initWithSource:(NSString *)source
{
    return [self initWithSource:source fromURL:nil languageInstance:nil usingStorageOptions:0];
}

- (id)initWithSource:(NSString *)source language:(OSALanguage *)language
{
    return [self initWithSource:source fromURL:nil languageInstance:nil usingStorageOptions:0];
}

- (id)initWithSource:(NSString *)source languageInstance:(OSALanguageInstance *)instance
{
    return [self initWithSource:source fromURL:nil languageInstance:instance usingStorageOptions:0];
}

- (id)initWithSource:(NSString *)source fromURL:(NSURL *)url languageInstance:(OSALanguageInstance *)instance usingStorageOptions:(unsigned long)options
{
    if ((self = [super init])) {
        _script = [[NSAppleScript alloc] initWithSource:source ?: @""];
        _url = [url copy];
        _instance = [instance retain];
    }
    return self;
}

- (id)initWithContentsOfURL:(NSURL *)url error:(NSDictionary **)error
{
    return [self initWithContentsOfURL:url languageInstance:nil usingStorageOptions:0 error:error];
}

- (id)initWithContentsOfURL:(NSURL *)url language:(OSALanguage *)language error:(NSDictionary **)error
{
    return [self initWithContentsOfURL:url languageInstance:nil usingStorageOptions:0 error:error];
}

- (id)initWithContentsOfURL:(NSURL *)url languageInstance:(OSALanguageInstance *)instance usingStorageOptions:(unsigned long)options error:(NSDictionary **)error
{
    NSDictionary *info = nil;
    NSAppleScript *script = [[[NSAppleScript alloc] initWithContentsOfURL:url error:&info] autorelease];
    if (!script) {
        if (error)
            *error = osaErrorInfo(info);
        [self release];
        return nil;
    }
    if ((self = [super init])) {
        _script = [script retain];
        _url = [url copy];
        _instance = [instance retain];
    }
    return self;
}

- (id)initWithCompiledData:(NSData *)data error:(NSDictionary **)error
{
    return [self initWithCompiledData:data fromURL:nil usingStorageOptions:0 error:error];
}

- (id)initWithCompiledData:(NSData *)data fromURL:(NSURL *)url usingStorageOptions:(unsigned long)options error:(NSDictionary **)error
{
    NSDictionary *info = nil;
    [[[[NSAppleScript alloc] initWithSource:@""] autorelease] compileAndReturnError:&info];
    if (error)
        *error = osaErrorInfo(info);
    [self release];
    return nil;
}

- (id)initWithScriptDataDescriptor:(id)descriptor fromURL:(NSURL *)url languageInstance:(OSALanguageInstance *)instance usingStorageOptions:(unsigned long)options error:(NSDictionary **)error
{
    return [self initWithCompiledData:[descriptor data] fromURL:url usingStorageOptions:options error:error];
}

- (void)dealloc
{
    [_script release];
    [_url release];
    [_instance release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone { return [self retain]; }
- (NSString *)source { return [_script source]; }
- (NSURL *)url { return _url; }
- (OSALanguage *)language { return [OSALanguage defaultLanguage]; }
- (void)setLanguage:(OSALanguage *)language {}

- (OSALanguageInstance *)languageInstance
{
    return _instance ?: [[OSALanguage defaultLanguage] sharedLanguageInstance];
}

- (void)setLanguageInstance:(OSALanguageInstance *)instance
{
    [_instance release];
    _instance = [instance retain];
}

- (BOOL)isCompiled { return [_script isCompiled]; }

- (BOOL)compileAndReturnError:(NSDictionary **)error
{
    NSDictionary *info = nil;
    BOOL compiled = [_script compileAndReturnError:&info];
    if (error)
        *error = osaErrorInfo(info);
    return compiled;
}

- (id)executeAndReturnError:(NSDictionary **)error
{
    NSDictionary *info = nil;
    id result = [_script executeAndReturnError:&info];
    if (error)
        *error = osaErrorInfo(info);
    return result;
}

- (id)executeAppleEvent:(id)event error:(NSDictionary **)error
{
    return [self executeAndReturnError:error];
}

- (id)executeAndReturnDisplayValue:(id *)displayValue error:(NSDictionary **)error
{
    if (displayValue)
        *displayValue = nil;
    return [self executeAndReturnError:error];
}

- (id)executeHandlerWithName:(NSString *)name arguments:(NSArray *)arguments error:(NSDictionary **)error
{
    return [self executeAndReturnError:error];
}

- (id)richTextSource
{
    NSString *source = [self source];
    if (source)
        return [[[NSAttributedString alloc] initWithString:source] autorelease];
    return nil;
}

- (id)richTextFromDescriptor:(id)descriptor
{
    NSString *string = [descriptor stringValue];
    if (string)
        return [[[NSAttributedString alloc] initWithString:string] autorelease];
    return nil;
}

- (BOOL)writeToURL:(NSURL *)url ofType:(NSString *)type error:(NSDictionary **)error
{
    return [self writeToURL:url ofType:type usingStorageOptions:0 error:error];
}

- (BOOL)writeToURL:(NSURL *)url ofType:(NSString *)type usingStorageOptions:(unsigned long)options error:(NSDictionary **)error
{
    if (![type isEqual:@"com.apple.applescript.text"])
        return [self compileAndReturnError:error];
    NSError *failure = nil;
    if ([[self source] writeToURL:url atomically:YES encoding:4 error:&failure])
        return YES;
    if (error) {
        *error = @{
            @"OSAScriptErrorMessageKey": [failure localizedDescription] ?: @"",
            @"OSAScriptErrorNumberKey": [NSNumber numberWithInteger:[failure code]],
        };
    }
    return NO;
}

- (id)compiledDataForType:(NSString *)type usingStorageOptions:(unsigned long)options error:(NSDictionary **)error
{
    [self compileAndReturnError:error];
    return nil;
}
@end

NSString *const OSAScriptErrorMessageKey = @"OSAScriptErrorMessageKey";
NSString *const OSAScriptErrorBriefMessageKey = @"OSAScriptErrorBriefMessageKey";
NSString *const OSAScriptErrorNumberKey = @"OSAScriptErrorNumberKey";
NSString *const OSAScriptErrorPartialResultKey = @"OSAScriptErrorPartialResultKey";
NSString *const OSAScriptErrorOffendingObjectKey = @"OSAScriptErrorOffendingObjectKey";
NSString *const OSAScriptErrorExpectedTypeKey = @"OSAScriptErrorExpectedTypeKey";
NSString *const OSAScriptErrorAppAddressKey = @"OSAScriptErrorAppAddressKey";
NSString *const OSAScriptErrorAppNameKey = @"OSAScriptErrorAppNameKey";
NSString *const OSAScriptErrorRangeKey = @"OSAScriptErrorRangeKey";
NSString *const OSAScriptErrorMessage = @"OSAScriptErrorMessage";
NSString *const OSAScriptErrorBriefMessage = @"OSAScriptErrorBriefMessage";
NSString *const OSAScriptErrorNumber = @"OSAScriptErrorNumber";
NSString *const OSAScriptErrorPartialResult = @"OSAScriptErrorPartialResult";
NSString *const OSAScriptErrorOffendingObject = @"OSAScriptErrorOffendingObject";
NSString *const OSAScriptErrorExpectedType = @"OSAScriptErrorExpectedType";
NSString *const OSAScriptErrorAppAddress = @"OSAScriptErrorAppAddress";
NSString *const OSAScriptErrorAppName = @"OSAScriptErrorAppName";
NSString *const OSAScriptErrorRange = @"OSAScriptErrorRange";
NSString *const OSAStorageScriptType = @"com.apple.applescript.script";
NSString *const OSAStorageScriptBundleType = @"com.apple.applescript.script-bundle";
NSString *const OSAStorageApplicationType = @"com.apple.application-bundle";
NSString *const OSAStorageApplicationBundleType = @"com.apple.application-bundle";
NSString *const OSAStorageTextType = @"com.apple.applescript.text";
