// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Speech: recognizer, requests, tasks and results; recognition is not available, a task carries an error saying so.

#import <Foundation/Foundation.h>
#include <dispatch/dispatch.h>

@interface NSObject (ManualRetain)
- (oneway void)release;
- (id)retain;
- (id)autorelease;
@end

@interface NSError : NSObject
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo;
@end
@interface NSLocale : NSObject
+ (id)currentLocale;
@end
@interface NSSet : NSObject
+ (id)set;
@end
@interface NSOperationQueue : NSObject
+ (id)mainQueue;
- (void)addOperationWithBlock:(void (^)(void))block;
@end
@interface NSArray (Speech)
+ (id)array;
@end
@class AVAudioFormat;

extern NSString *const NSLocalizedDescriptionKey;

@protocol SFSpeechRecognizerDelegate <NSObject>
@optional
- (void)speechRecognitionTask:(id)task didFinishSuccessfully:(BOOL)success;
@end

@interface SFTranscriptionSegment : NSObject <NSCopying, NSSecureCoding>
{
    float _confidence;
    double _timestamp;
    double _duration;
    NSRange _substringRange;
}
@property (readonly, copy) NSString *substring;
@property (readonly) NSRange substringRange;
@property (readonly) double timestamp;
@property (readonly) double duration;
@property (readonly) float confidence;
@property (readonly) NSArray *alternativeSubstrings;
@end

@implementation SFTranscriptionSegment

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (NSString *)substring
{
    return @"";
}

- (NSArray *)alternativeSubstrings
{
    return [NSArray array];
}

@end

@interface SFTranscription : NSObject <NSCopying, NSSecureCoding>
@property (readonly, copy) NSString *formattedString;
@property (readonly, copy) NSArray *segments;
@end

@implementation SFTranscription

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (NSString *)formattedString
{
    return @"";
}

- (NSArray *)segments
{
    return [NSArray array];
}

@end

@interface SFSpeechRecognitionResult : NSObject <NSCopying, NSSecureCoding>
{
    BOOL _final;
}
@property (readonly, copy) SFTranscription *bestTranscription;
@property (readonly, copy) NSArray *transcriptions;
@property (readonly, getter=isFinal) BOOL final;
@end

@implementation SFSpeechRecognitionResult

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (id)initWithCoder:(NSCoder *)coder
{
    return [self init];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
}

- (id)copyWithZone:(NSZone *)zone
{
    return [self retain];
}

- (SFTranscription *)bestTranscription
{
    return [[[SFTranscription alloc] init] autorelease];
}

- (NSArray *)transcriptions
{
    return [NSArray array];
}

@end

@interface SFSpeechRecognitionRequest : NSObject
{
    BOOL _shouldReportPartialResults;
    BOOL _requiresOnDeviceRecognition;
    BOOL _addsPunctuation;
    long _taskHint;
    NSArray *_contextualStrings;
}
@property long taskHint;
@property BOOL shouldReportPartialResults;
@property (copy) NSArray *contextualStrings;
@property BOOL requiresOnDeviceRecognition;
@property BOOL addsPunctuation;
@end

@implementation SFSpeechRecognitionRequest

- (void)dealloc
{
    [_contextualStrings release];
    [super dealloc];
}

@end

@interface SFSpeechURLRecognitionRequest : SFSpeechRecognitionRequest
{
    NSURL *_URL;
}
@property (readonly, copy) NSURL *URL;
- (id)initWithURL:(NSURL *)url;
@end

@implementation SFSpeechURLRecognitionRequest

- (id)initWithURL:(NSURL *)url
{
    self = [super init];
    if (self)
        _URL = [url copy];
    return self;
}

- (void)dealloc
{
    [_URL release];
    [super dealloc];
}

@end

@interface SFSpeechAudioBufferRecognitionRequest : SFSpeechRecognitionRequest
@property (readonly) AVAudioFormat *nativeAudioFormat;
- (void)appendAudioPCMBuffer:(id)buffer;
- (void)appendAudioSampleBuffer:(void *)buffer;
- (void)endAudio;
@end

@implementation SFSpeechAudioBufferRecognitionRequest

- (AVAudioFormat *)nativeAudioFormat
{
    return nil;
}

- (void)appendAudioPCMBuffer:(id)buffer
{
}

- (void)appendAudioSampleBuffer:(void *)buffer
{
}

- (void)endAudio
{
}

@end

@interface SFSpeechRecognitionTask : NSObject
{
    NSError *_error;
}
@property (readonly) long state;
@property (readonly, getter=isFinishing) BOOL finishing;
@property (readonly, getter=isCancelled) BOOL cancelled;
@property (readonly, copy) NSError *error;
- (void)finish;
- (void)cancel;
@end

@implementation SFSpeechRecognitionTask

- (id)init
{
    self = [super init];
    if (self)
        _error = [[NSError errorWithDomain:@"SFSpeechErrorDomain" code:1 userInfo:@{NSLocalizedDescriptionKey: @"Speech recognition is not available."}] retain];
    return self;
}

- (void)dealloc
{
    [_error release];
    [super dealloc];
}

- (long)state
{
    return 4;
}

- (BOOL)isFinishing
{
    return NO;
}

- (BOOL)isCancelled
{
    return NO;
}

- (NSError *)error
{
    return _error;
}

- (void)finish
{
}

- (void)cancel
{
}

@end

@interface SFSpeechRecognizer : NSObject
{
    NSLocale *_locale;
    __weak id _delegate;
    long _defaultTaskHint;
    NSOperationQueue *_queue;
}
@property (readonly, getter=isAvailable) BOOL available;
@property (readonly, copy) NSLocale *locale;
@property (nonatomic) BOOL supportsOnDeviceRecognition;
@property (weak) id delegate;
@property long defaultTaskHint;
@property (strong) NSOperationQueue *queue;
+ (NSSet *)supportedLocales;
+ (long)authorizationStatus;
+ (void)requestAuthorization:(void (^)(long status))handler;
- (id)initWithLocale:(NSLocale *)locale;
- (SFSpeechRecognitionTask *)recognitionTaskWithRequest:(SFSpeechRecognitionRequest *)request resultHandler:(void (^)(SFSpeechRecognitionResult *result, NSError *error))handler;
- (SFSpeechRecognitionTask *)recognitionTaskWithRequest:(SFSpeechRecognitionRequest *)request delegate:(id)delegate;
@end

@implementation SFSpeechRecognizer

+ (NSSet *)supportedLocales
{
    return [NSSet set];
}

+ (long)authorizationStatus
{
    return 1;
}

+ (void)requestAuthorization:(void (^)(long status))handler
{
    if (handler) {
        void (^copy)(long status) = [handler copy];
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            copy(1);
            [copy release];
        });
    }
}

- (id)init
{
    return [self initWithLocale:[NSLocale currentLocale]];
}

- (id)initWithLocale:(NSLocale *)locale
{
    self = [super init];
    if (self) {
        _locale = [locale copy];
        _queue = [[NSOperationQueue mainQueue] retain];
    }
    return self;
}

- (void)dealloc
{
    [_locale release];
    [_queue release];
    [super dealloc];
}

- (BOOL)isAvailable
{
    return NO;
}

- (BOOL)supportsOnDeviceRecognition
{
    return NO;
}

- (void)setSupportsOnDeviceRecognition:(BOOL)supports
{
}

- (SFSpeechRecognitionTask *)recognitionTaskWithRequest:(SFSpeechRecognitionRequest *)request resultHandler:(void (^)(SFSpeechRecognitionResult *result, NSError *error))handler
{
    SFSpeechRecognitionTask *task = [[[SFSpeechRecognitionTask alloc] init] autorelease];
    if (handler) {
        void (^completion)(SFSpeechRecognitionResult *result, NSError *error) = [handler copy];
        NSError *error = [[task error] retain];
        [_queue addOperationWithBlock:^{
            completion(nil, error);
            [error release];
            [completion release];
        }];
    }
    return task;
}

- (SFSpeechRecognitionTask *)recognitionTaskWithRequest:(SFSpeechRecognitionRequest *)request delegate:(id)delegate
{
    SFSpeechRecognitionTask *task = [[[SFSpeechRecognitionTask alloc] init] autorelease];
    if ([delegate respondsToSelector:@selector(speechRecognitionTask:didFinishSuccessfully:)]) {
        [delegate retain];
        [task retain];
        [_queue addOperationWithBlock:^{
            [delegate speechRecognitionTask:task didFinishSuccessfully:NO];
            [task release];
            [delegate release];
        }];
    }
    return task;
}

@end

NSString *const SFSpeechErrorDomain = @"SFSpeechErrorDomain";
