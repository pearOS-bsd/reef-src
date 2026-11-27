// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// AVFAudio classes GarageBand uses that Reef's AVFAudio lacks.
#import <Foundation/Foundation.h>
#import <CoreAudioTypes/CoreAudioTypes.h>
#import <AudioToolbox/AudioComponent.h>
typedef uint32_t AVAudioChannelCount;

@interface AVAudioApplication : NSObject
@end
@implementation AVAudioApplication
+ (instancetype)sharedInstance {
    static AVAudioApplication *shared;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ shared = [AVAudioApplication new]; });
    return shared;
}
- (NSInteger)recordPermission { return 'grnt'; }   // AVAudioApplicationRecordPermissionGranted
+ (void)requestRecordPermissionWithCompletionHandler:(void (^)(BOOL))handler { if (handler) handler(YES); }
- (BOOL)isInputMuted { return NO; }
- (BOOL)setInputMuted:(BOOL)muted error:(NSError **)error { return YES; }
- (BOOL)setInputMuteStateChangeHandler:(id)handler error:(NSError **)error { return YES; }
@end

// A channel layout: the layout tag and its channel count
@interface AVAudioChannelLayout : NSObject <NSSecureCoding>
@end
@implementation AVAudioChannelLayout {
    AudioChannelLayout *_layout;
    size_t _size;
}
+ (BOOL)supportsSecureCoding { return YES; }
+ (instancetype)layoutWithLayoutTag:(AudioChannelLayoutTag)tag { return [[self alloc] initWithLayoutTag:tag]; }
+ (instancetype)layoutWithLayout:(const AudioChannelLayout *)layout { return [[self alloc] initWithLayout:layout]; }
- (instancetype)initWithLayoutTag:(AudioChannelLayoutTag)tag {
    AudioChannelLayout layout = {0};
    layout.mChannelLayoutTag = tag;
    return [self initWithLayout:&layout];
}
- (instancetype)initWithLayout:(const AudioChannelLayout *)layout {
    if (!layout) return nil;
    if ((self = [super init])) {
        UInt32 n = layout->mChannelLayoutTag == kAudioChannelLayoutTag_UseChannelDescriptions ? layout->mNumberChannelDescriptions : 0;
        _size = offsetof(AudioChannelLayout, mChannelDescriptions) + (n ? n : 1) * sizeof(AudioChannelDescription);
        _layout = calloc(1, _size);
        memcpy(_layout, layout, offsetof(AudioChannelLayout, mChannelDescriptions) + n * sizeof(AudioChannelDescription));
    }
    return self;
}
- (void)dealloc { free(_layout); }
- (AudioChannelLayoutTag)layoutTag { return _layout->mChannelLayoutTag; }
- (const AudioChannelLayout *)layout { return _layout; }
- (AVAudioChannelCount)channelCount {
    AudioChannelLayoutTag tag = _layout->mChannelLayoutTag;
    if (tag == kAudioChannelLayoutTag_UseChannelDescriptions) return _layout->mNumberChannelDescriptions;
    if (tag == kAudioChannelLayoutTag_UseChannelBitmap) return (AVAudioChannelCount)__builtin_popcount(_layout->mChannelBitmap);
    return tag & 0xffff;
}
- (BOOL)isEqual:(id)object {
    return [object isKindOfClass:[AVAudioChannelLayout class]] && ((AVAudioChannelLayout *)object)->_size == _size &&
           memcmp(((AVAudioChannelLayout *)object)->_layout, _layout, _size) == 0;
}
- (NSUInteger)hash { return _layout->mChannelLayoutTag; }
- (void)encodeWithCoder:(NSCoder *)coder { [coder encodeObject:[NSData dataWithBytes:_layout length:_size] forKey:@"layout"]; }
- (instancetype)initWithCoder:(NSCoder *)coder {
    NSData *d = [coder decodeObjectOfClass:NSData.class forKey:@"layout"];
    return d.length >= sizeof(AudioChannelLayout) ? [self initWithLayout:d.bytes] : nil;
}
@end

@interface AVAudioFile : NSObject
@end
@implementation AVAudioFile
- (instancetype)initForReading:(NSURL *)url error:(NSError **)error {
    if (error) *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:'typ?' userInfo:nil];   // kAudioFileUnsupportedFileTypeError
    return nil;
}
- (instancetype)initForReading:(NSURL *)url commonFormat:(NSInteger)format interleaved:(BOOL)interleaved error:(NSError **)error {
    return [self initForReading:url error:error];
}
- (instancetype)initForWriting:(NSURL *)url settings:(NSDictionary *)settings error:(NSError **)error {
    return [self initForReading:url error:error];
}
@end

@interface AVAudioSessionPortDescription : NSObject
@property (readonly) NSString *portType;
@property (readonly) NSString *portName;
@property (readonly) NSString *UID;
@property (readonly) BOOL hasHardwareVoiceCallProcessing;
@property (readonly) BOOL isSpatialAudioEnabled;
@property (readonly) NSArray *channels;
@property (readonly) NSArray *dataSources;
@property (readonly) id selectedDataSource;
@property (readonly) id preferredDataSource;
@end
@implementation AVAudioSessionPortDescription
- (BOOL)setPreferredDataSource:(id)dataSource error:(NSError **)error { return YES; }
@end

// The audio units installed: Reef's own effects and instruments only
@interface AVAudioUnitComponentManager : NSObject
@end
@implementation AVAudioUnitComponentManager
+ (instancetype)sharedAudioUnitComponentManager {
    static AVAudioUnitComponentManager *shared;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ shared = [AVAudioUnitComponentManager new]; });
    return shared;
}
- (NSArray *)tagNames { return @[]; }
- (NSArray *)standardLocalizedTagNames { return @[]; }
- (NSArray *)componentsMatchingPredicate:(NSPredicate *)predicate { return @[]; }
- (NSArray *)componentsPassingTest:(id)test { return @[]; }
- (NSArray *)componentsMatchingDescription:(AudioComponentDescription)description { return @[]; }
@end
