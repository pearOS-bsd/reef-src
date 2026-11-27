// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Speech: results, transcriptions, requests, the failing recognition task and the recognizer (authorization, delegate, handlers on its queue).
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <unistd.h>

@interface NSString (T)
- (const char *)UTF8String;
@end
@interface NSError : NSObject
- (NSString *)domain;
- (NSInteger)code;
- (NSString *)localizedDescription;
@end
@interface NSURL : NSObject
+ (NSURL *)fileURLWithPath:(NSString *)path;
- (NSString *)path;
@end
@interface NSLocale : NSObject
+ (id)localeWithLocaleIdentifier:(NSString *)identifier;
- (NSString *)localeIdentifier;
@end
@interface NSObject (T)
- (NSUInteger)count;
@end
@interface NSOperationQueue : NSObject
+ (id)mainQueue;
@end
@interface NSSet : NSObject
- (NSUInteger)count;
@end

@interface SFTranscriptionSegment : NSObject <NSCopying>
@property (readonly, copy) NSString *substring;
@property (readonly) NSRange substringRange;
@property (readonly) double timestamp;
@property (readonly) double duration;
@property (readonly) float confidence;
@property (readonly) NSArray *alternativeSubstrings;
+ (BOOL)supportsSecureCoding;
@end
@interface SFTranscription : NSObject <NSCopying>
@property (readonly, copy) NSString *formattedString;
@property (readonly, copy) NSArray *segments;
+ (BOOL)supportsSecureCoding;
@end
@interface SFSpeechRecognitionResult : NSObject <NSCopying>
@property (readonly, copy) SFTranscription *bestTranscription;
@property (readonly, copy) NSArray *transcriptions;
@property (readonly, getter=isFinal) BOOL final;
+ (BOOL)supportsSecureCoding;
@end
@interface SFSpeechRecognitionRequest : NSObject
@property long taskHint;
@property BOOL shouldReportPartialResults;
@property (copy) NSArray *contextualStrings;
@property BOOL requiresOnDeviceRecognition;
@property BOOL addsPunctuation;
@end
@interface SFSpeechURLRecognitionRequest : SFSpeechRecognitionRequest
@property (readonly, copy) NSURL *URL;
- (id)initWithURL:(NSURL *)url;
@end
@interface SFSpeechAudioBufferRecognitionRequest : SFSpeechRecognitionRequest
@property (readonly) id nativeAudioFormat;
- (void)appendAudioPCMBuffer:(id)buffer;
- (void)endAudio;
@end
@interface SFSpeechRecognitionTask : NSObject
@property (readonly) long state;
@property (readonly, getter=isFinishing) BOOL finishing;
@property (readonly, getter=isCancelled) BOOL cancelled;
@property (readonly, copy) NSError *error;
- (void)finish;
- (void)cancel;
@end
@interface SFSpeechRecognizer : NSObject
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
extern NSString *const SFSpeechErrorDomain;

static volatile int arrived;

@interface Delegate : NSObject
@end
@implementation Delegate
- (void)speechRecognitionTask:(SFSpeechRecognitionTask *)task didFinishSuccessfully:(BOOL)success
{
    printf("delegate %d %ld %d\n", task != nil, task.state, success);
    arrived++;
}
@end

@interface Plain : NSObject
@end
@implementation Plain
@end

int main(void)
{
    SFTranscriptionSegment *segment = [SFTranscriptionSegment new];
    printf("segment [%s] %lu %lu %g %g %g %lu copy %d coding %d\n", [segment.substring UTF8String], (unsigned long)segment.substringRange.location, (unsigned long)segment.substringRange.length,
           segment.timestamp, segment.duration, segment.confidence, (unsigned long)[segment.alternativeSubstrings count], [segment copy] == segment, [SFTranscriptionSegment supportsSecureCoding]);
    SFTranscription *transcription = [SFTranscription new];
    printf("transcription [%s] %lu copy %d\n", [transcription.formattedString UTF8String], (unsigned long)[transcription.segments count], [transcription copy] == transcription);
    SFSpeechRecognitionResult *result = [SFSpeechRecognitionResult new];
    printf("result %d %d %lu copy %d\n", result.bestTranscription != nil, result.final, (unsigned long)[result.transcriptions count], [result copy] == result);

    SFSpeechRecognitionRequest *request = [SFSpeechRecognitionRequest new];
    printf("request %ld %d %d %d %d\n", request.taskHint, request.shouldReportPartialResults, request.requiresOnDeviceRecognition, request.addsPunctuation, request.contextualStrings == nil);
    request.taskHint = 2; request.shouldReportPartialResults = YES; request.requiresOnDeviceRecognition = YES; request.addsPunctuation = YES; request.contextualStrings = @[@"a", @"b"];
    printf("request %ld %d %d %d %lu\n", request.taskHint, request.shouldReportPartialResults, request.requiresOnDeviceRecognition, request.addsPunctuation, (unsigned long)[request.contextualStrings count]);
    SFSpeechURLRecognitionRequest *url = [[SFSpeechURLRecognitionRequest alloc] initWithURL:[NSURL fileURLWithPath:@"/tmp/a.wav"]];
    printf("url %s\n", [[url.URL path] UTF8String]);
    SFSpeechAudioBufferRecognitionRequest *audio = [SFSpeechAudioBufferRecognitionRequest new];
    [audio appendAudioPCMBuffer:nil]; [audio endAudio];
    printf("audio %d\n", audio.nativeAudioFormat == nil);

    SFSpeechRecognitionTask *task = [SFSpeechRecognitionTask new];
    printf("task %ld %d %d %s %ld %s\n", task.state, task.finishing, task.cancelled, [task.error.domain UTF8String], (long)task.error.code, [task.error.localizedDescription UTF8String]);
    [task finish]; [task cancel];
    printf("domain %s\n", [SFSpeechErrorDomain UTF8String]);

    printf("class %ld %lu\n", [SFSpeechRecognizer authorizationStatus], (unsigned long)[[SFSpeechRecognizer supportedLocales] count]);
    SFSpeechRecognizer *recognizer = [[SFSpeechRecognizer alloc] initWithLocale:[NSLocale localeWithLocaleIdentifier:@"en_US"]];
    printf("recognizer %s %d %d %ld %d\n", [[recognizer.locale localeIdentifier] UTF8String], recognizer.available, recognizer.supportsOnDeviceRecognition, recognizer.defaultTaskHint, recognizer.queue == [NSOperationQueue mainQueue]);
    recognizer.defaultTaskHint = 3; recognizer.supportsOnDeviceRecognition = YES;
    printf("recognizer %ld %d\n", recognizer.defaultTaskHint, recognizer.supportsOnDeviceRecognition);
    recognizer.queue = nil;
    printf("queue %d\n", recognizer.queue == nil);
    recognizer.queue = [NSOperationQueue new];
    SFSpeechRecognizer *current = [SFSpeechRecognizer new];
    printf("default locale %d\n", current.locale != nil);

    Delegate *delegate = [Delegate new];
    recognizer.delegate = delegate;
    printf("delegate %d\n", recognizer.delegate == delegate);
    [SFSpeechRecognizer requestAuthorization:nil];
    [SFSpeechRecognizer requestAuthorization:^(long status) {
        printf("authorization %ld\n", status);
        arrived++;
    }];
    for (int i = 0; i < 100 && arrived < 1; i++)
        usleep(20000);
    SFSpeechRecognitionTask *t1 = [recognizer recognitionTaskWithRequest:url resultHandler:^(SFSpeechRecognitionResult *r, NSError *e) {
        printf("handler %d %s %ld\n", r == nil, [e.domain UTF8String], (long)e.code);
        arrived++;
    }];
    for (int i = 0; i < 100 && arrived < 2; i++)
        usleep(20000);
    printf("task1 %d %ld\n", t1 != nil, t1.state);
    SFSpeechRecognitionTask *t2 = [recognizer recognitionTaskWithRequest:url resultHandler:nil];
    printf("task2 %d\n", t2 != nil);
    SFSpeechRecognitionTask *t4 = [recognizer recognitionTaskWithRequest:url delegate:[Plain new]];
    printf("task4 %d\n", t4 != nil);
    SFSpeechRecognitionTask *t3 = [recognizer recognitionTaskWithRequest:url delegate:delegate];
    for (int i = 0; i < 100 && arrived < 3; i++)
        usleep(20000);
    printf("task3 %d\n", t3 != nil);
    printf("arrived %d\n", arrived);
    return 0;
}
