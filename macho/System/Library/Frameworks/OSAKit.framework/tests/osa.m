// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// OSAKit: the language, scripts from source, files and compiled data, error information, text storage.
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

@interface NSString (T)
- (const char *)UTF8String;
+ (id)stringWithContentsOfFile:(NSString *)path encoding:(NSUInteger)encoding error:(NSError **)error;
@end
@interface NSURL : NSObject
+ (NSURL *)fileURLWithPath:(NSString *)path;
- (NSString *)path;
@end
@interface NSObject (T)
- (id)copy;
- (id)copyWithZone:(void *)zone;
- (NSUInteger)count;
- (id)objectForKey:(id)key;
@end
@interface NSData : NSObject
+ (id)dataWithBytes:(const void *)bytes length:(NSUInteger)length;
@end

@interface OSALanguage : NSObject
+ (NSArray *)availableLanguages;
+ (OSALanguage *)languageForName:(NSString *)name;
+ (OSALanguage *)languageForScriptDataDescriptor:(id)descriptor;
+ (OSALanguage *)defaultLanguage;
+ (void)setDefaultLanguage:(OSALanguage *)language;
- (id)sharedLanguageInstance;
- (NSString *)name;
- (NSString *)info;
- (NSString *)version;
- (unsigned int)type;
- (unsigned int)subType;
- (unsigned int)manufacturer;
- (unsigned long)features;
- (BOOL)isThreadSafe;
@end
@interface OSALanguageInstance : NSObject
+ (id)languageInstanceWithLanguage:(OSALanguage *)language;
- (id)initWithLanguage:(OSALanguage *)language;
- (OSALanguage *)language;
- (id)defaultTarget;
- (void)setDefaultTarget:(id)target;
- (id)richTextFromDescriptor:(id)descriptor;
@end
@interface OSAScript : NSObject
+ (id)scriptDataDescriptorWithContentsOfURL:(NSURL *)url;
- (id)initWithSource:(NSString *)source;
- (id)initWithSource:(NSString *)source language:(OSALanguage *)language;
- (id)initWithSource:(NSString *)source languageInstance:(OSALanguageInstance *)instance;
- (id)initWithContentsOfURL:(NSURL *)url error:(NSDictionary **)error;
- (id)initWithContentsOfURL:(NSURL *)url language:(OSALanguage *)language error:(NSDictionary **)error;
- (id)initWithCompiledData:(NSData *)data error:(NSDictionary **)error;
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

extern NSString *const OSAScriptErrorMessageKey, *const OSAScriptErrorNumberKey, *const OSAStorageTextType, *const OSAStorageScriptType;

static void code(const char *what, NSDictionary *error)
{
    printf("%s: %s\n", what, error ? "error" : "none");
}

int main(void)
{
    OSALanguage *language = [OSALanguage defaultLanguage];
    printf("language %s %s %s %08x %08x %08x %lx %d\n", [[language name] UTF8String], [[language info] UTF8String], [[language version] UTF8String],
        [language type], [language subType], [language manufacturer], [language features], [language isThreadSafe]);
    printf("same %d list %lu named %d other %d descriptor %d\n", language == [OSALanguage defaultLanguage], (unsigned long)[[OSALanguage availableLanguages] count],
        [OSALanguage languageForName:@"AppleScript"] == language, [OSALanguage languageForName:@"JavaScript"] == nil,
        [OSALanguage languageForScriptDataDescriptor:nil] == language);
    [OSALanguage setDefaultLanguage:nil];
    printf("after set %d\n", [OSALanguage defaultLanguage] == language);

    OSALanguageInstance *instance = [language sharedLanguageInstance];
    OSALanguageInstance *made = [OSALanguageInstance languageInstanceWithLanguage:language];
    printf("instance %d %d %d target %d rich %d\n", instance != nil, made != nil, [instance language] == language, [instance defaultTarget] == nil,
        [instance richTextFromDescriptor:@"x"] != nil);
    [instance setDefaultTarget:@"t"];

    OSAScript *script = [[OSAScript alloc] initWithSource:@"return 1"];
    printf("script %s url %d lang %d inst %d compiled %d\n", [[script source] UTF8String], [script url] == nil, [script language] == language,
        [script languageInstance] != nil, [script isCompiled]);
    [script setLanguage:language];
    [script setLanguageInstance:instance];
    printf("instance kept %d\n", [script languageInstance] == instance);
    NSDictionary *error = (__bridge NSDictionary *)(void *)1;
    BOOL compiled = [script compileAndReturnError:&error];
    printf("compile %d\n", compiled); code("compile", error);
    id result = [script executeAndReturnError:&error];
    printf("execute %d\n", result != nil); code("execute", error);
    id display = (__bridge id)(void *)1;
    result = [script executeAndReturnDisplayValue:&display error:&error];
    printf("display %d %d\n", display == nil, result != nil);
    result = [script executeAppleEvent:nil error:&error];
    result = [script executeHandlerWithName:@"h" arguments:nil error:&error];
    printf("handler %d\n", result != nil);
    [script compileAndReturnError:NULL];
    [script executeAndReturnError:NULL];
    printf("rich %d %d\n", [script richTextSource] != nil, [script richTextFromDescriptor:nil] == nil);

    OSAScript *bad = [[OSAScript alloc] initWithSource:@"this is not ( valid"];
    BOOL ok = [bad compileAndReturnError:&error];
    printf("bad compile %d\n", ok); code("bad", error);
    if (error) printf("keys %d %d\n", [error objectForKey:OSAScriptErrorMessageKey] != nil, [error objectForKey:OSAScriptErrorNumberKey] != nil);
    result = [bad executeAndReturnError:&error]; code("bad execute", error);
    id empty = [[OSAScript alloc] initWithSource:nil];
    printf("empty %s\n", [[empty source] UTF8String]);
    id other = [[OSAScript alloc] initWithSource:@"x" language:language];
    id withInstance = [[OSAScript alloc] initWithSource:@"x" languageInstance:instance];
    printf("variants %d %d\n", [other languageInstance] != nil, [withInstance languageInstance] == instance);

    id copy = [script copy];
    printf("copy %d\n", copy == script);

    NSString *path = @"/tmp/osakit-test.applescript";
    NSURL *url = [NSURL fileURLWithPath:path];
    NSDictionary *info = nil;
    ok = [script writeToURL:url ofType:OSAStorageTextType error:&info];
    printf("write text %d\n", ok); code("write", info);
    if (ok) printf("file %s\n", [[NSString stringWithContentsOfFile:path encoding:4 error:NULL] UTF8String]);
    ok = [script writeToURL:[NSURL fileURLWithPath:@"/nonexistent-folder/x.applescript"] ofType:OSAStorageTextType error:&info];
    printf("write bad %d\n", ok); code("write bad", info);
    if (info) printf("write keys %d %d\n", [info objectForKey:OSAScriptErrorMessageKey] != nil, [info objectForKey:OSAScriptErrorNumberKey] != nil);
    ok = [script writeToURL:url ofType:OSAStorageScriptType error:&info];
    printf("write script %d\n", ok); code("write script", info);
    ok = [script writeToURL:url ofType:OSAStorageTextType usingStorageOptions:0 error:NULL];
    id data = [script compiledDataForType:OSAStorageScriptType usingStorageOptions:0 error:&info];
    printf("data %d\n", data == nil);

    OSAScript *loaded = [[OSAScript alloc] initWithContentsOfURL:url error:&info];
    printf("loaded %d\n", loaded != nil); code("load", info);
    if (loaded) printf("loaded source %d url %d\n", [loaded source] != nil, [[[loaded url] path] isEqual:path]);
    loaded = [[OSAScript alloc] initWithContentsOfURL:[NSURL fileURLWithPath:@"/nonexistent/x.scpt"] error:&info];
    printf("missing %d\n", loaded != nil); code("missing", info);
    loaded = [[OSAScript alloc] initWithContentsOfURL:[NSURL fileURLWithPath:@"/nonexistent/x.scpt"] error:NULL];
    loaded = [[OSAScript alloc] initWithContentsOfURL:[NSURL fileURLWithPath:@"/nonexistent/x.scpt"] language:language error:&info];
    printf("missing language %d\n", loaded != nil);
    loaded = [[OSAScript alloc] initWithCompiledData:[NSData dataWithBytes:"abc" length:3] error:&info];
    printf("data init %d\n", loaded != nil); code("data init", info);
    loaded = [[OSAScript alloc] initWithCompiledData:[NSData dataWithBytes:"abc" length:3] error:NULL];
    printf("descriptor %d\n", [OSAScript scriptDataDescriptorWithContentsOfURL:url] == nil);
    unlink([path UTF8String]);
    return 0;
}
