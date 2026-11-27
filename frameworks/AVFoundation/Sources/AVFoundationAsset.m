// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Assets, their tracks and compositions on the media engine (ReefMedia.h): AVURLAsset loads its file's description
// (duration, tracks, sizes, rates, format descriptions) asynchronously as on macOS, AVAssetTrack answers from it, and
// AVMutableComposition keeps the segments it is given (Reef's original refuses them: "Media editing is not available").

#import "ReefPlayback.h"
#import <CoreMedia/CoreMedia.h>
#import <AudioToolbox/AudioToolbox.h>
#import <objc/runtime.h>
#import <objc/message.h>
#import <pthread.h>

void ReefLog(NSString *format, ...)
{
    static int on = -1;
    if (on < 0)
        on = getenv("REEF_AV_LOG") != NULL;
    if (!on)
        return;
    va_list arguments;
    va_start(arguments, format);
    NSString *line = [[NSString alloc] initWithFormat:format arguments:arguments];
    va_end(arguments);
    fprintf(stderr, "[av] %s\n", line.UTF8String);
}

CMTime ReefTime(double seconds)
{
    if (isnan(seconds))
        return kCMTimeInvalid;
    if (isinf(seconds))
        return seconds > 0 ? kCMTimePositiveInfinity : kCMTimeNegativeInfinity;
    return CMTimeMakeWithSeconds(seconds, 600);
}

static double Seconds(CMTime t)
{
    return CMTIME_IS_NUMERIC(t) ? CMTimeGetSeconds(t) : NAN;
}

@implementation ReefMediaSource
@end

#pragma mark - state

static char kAssetState, kTrackState, kSegments;

// What a file asset knows. `status` is an AVKeyValueStatus for every key.
@interface ReefAssetState : NSObject {
@public
    pthread_mutex_t lock;
    NSInteger status;
    NSError *error;
    ReefMediaInfo info;
    NSArray *tracks;
    NSMutableArray *handlers;
    NSString *path;
}
@end
@implementation ReefAssetState
- (instancetype)init
{
    if ((self = [super init])) {
        pthread_mutex_init(&lock, NULL);
        handlers = [NSMutableArray array];
    }
    return self;
}
@end

// What a track of a file asset is.
@interface ReefTrackState : NSObject {
@public
    ReefMediaStream stream;
    __weak AVAsset *asset;
    CMFormatDescriptionRef format;
    double fileDuration;
}
@end
@implementation ReefTrackState
- (void)dealloc
{
    if (format)
        CFRelease(format);
}
@end

// A part of a composition track: `sourceRange` of `track` at `start`.
@interface ReefSegment : NSObject {
@public
    AVAssetTrack *track;
    CMTimeRange sourceRange;
    CMTime start;
}
@end
@implementation ReefSegment
@end

static ReefAssetState *AssetState(AVAsset *asset, BOOL create)
{
    ReefAssetState *state = objc_getAssociatedObject(asset, &kAssetState);
    if (state || !create || ![asset isKindOfClass:[AVURLAsset class]])
        return state;
    @synchronized(asset) {
        state = objc_getAssociatedObject(asset, &kAssetState);
        if (!state) {
            state = [ReefAssetState new];
            NSURL *url = [(AVURLAsset *)asset URL];
            state->path = url.isFileURL ? url.path : url.absoluteString;
            objc_setAssociatedObject(asset, &kAssetState, state, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
        }
    }
    return state;
}

static ReefTrackState *TrackState(AVAssetTrack *track)
{
    return objc_getAssociatedObject(track, &kTrackState);
}

static NSMutableArray *Segments(AVAssetTrack *track, BOOL create)
{
    NSMutableArray *segments = objc_getAssociatedObject(track, &kSegments);
    if (!segments && create) {
        segments = [NSMutableArray array];
        objc_setAssociatedObject(track, &kSegments, segments, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return segments;
}

static NSString *MediaTypeOf(int type)
{
    switch (type) {
    case ReefMediaVideo: return AVMediaTypeVideo;
    case ReefMediaAudio: return AVMediaTypeAudio;
    default: return AVMediaTypeSubtitle;
    }
}

static AVAssetTrack *MakeTrack(AVAsset *asset, const ReefMediaStream *stream, double fileDuration)
{
    // -init is not in the SDK's interface; the runtime makes the object as Reef's class does
    AVAssetTrack *track = ((id (*)(id, SEL))objc_msgSend)([AVAssetTrack alloc], @selector(init));
    ReefTrackState *state = [ReefTrackState new];
    state->stream = *stream;
    state->asset = asset;
    state->fileDuration = fileDuration;
    if (stream->type == ReefMediaVideo) {
        CMVideoFormatDescriptionCreate(kCFAllocatorDefault, stream->codecType ? stream->codecType : 'avc1', stream->width,
                                       stream->height, NULL, &state->format);
    } else if (stream->type == ReefMediaAudio) {
        AudioStreamBasicDescription description = {0};
        description.mSampleRate = stream->sampleRate;
        description.mFormatID = stream->codecType ? stream->codecType : kAudioFormatMPEG4AAC;
        description.mChannelsPerFrame = stream->channels;
        description.mFramesPerPacket = description.mFormatID == kAudioFormatMPEG4AAC ? 1024 : 0;
        if (description.mFormatID == kAudioFormatLinearPCM) {
            description.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
            description.mBitsPerChannel = 16;
            description.mFramesPerPacket = 1;
            description.mBytesPerFrame = description.mBytesPerPacket = 2 * stream->channels;
        }
        CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &description, 0, NULL, 0, NULL, NULL, &state->format);
    }
    objc_setAssociatedObject(track, &kTrackState, state, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    return track;
}

// Reads the file's description (on the caller's thread); the state is locked.
static void ProbeLocked(AVAsset *asset, ReefAssetState *state)
{
    int error = ReefMediaProbe(state->path.fileSystemRepresentation, &state->info);
    int playable = 0;
    for (int i = 0; i < state->info.streamCount; i++)
        if (state->info.streams[i].type == ReefMediaVideo || state->info.streams[i].type == ReefMediaAudio)
            playable = 1;
    if (error < 0 || !playable) {
        ReefLog(@"%@: cannot be read (%d)", state->path, error);
        state->status = AVKeyValueStatusFailed;
        NSInteger code = error == -2 || error == -1 ? AVErrorFileFormatNotRecognized : AVErrorFileFailedToParse;
        NSString *text = !ReefMediaAvailable() ? @"Media playback is not available." : @"Cannot Open";
        NSString *reason = !ReefMediaAvailable() ? @"The host has no FFmpeg libraries (libavformat 60 or 61)." : @"This media format is not supported.";
        state->error = [NSError errorWithDomain:AVFoundationErrorDomain code:code
                                       userInfo:@{NSLocalizedDescriptionKey: text, NSLocalizedFailureReasonErrorKey: reason}];
        state->tracks = @[];
        return;
    }
    NSMutableArray *tracks = [NSMutableArray array];
    for (int i = 0; i < state->info.streamCount; i++) {
        const ReefMediaStream *s = &state->info.streams[i];
        if (s->attachedPicture)
            continue;
        [tracks addObject:MakeTrack(asset, s, state->info.duration)];
    }
    state->tracks = tracks;
    state->status = AVKeyValueStatusLoaded;
    ReefLog(@"%@: %@, %.3f s, %lu tracks", state->path, @(state->info.formatName), state->info.duration, (unsigned long)tracks.count);
}

BOOL ReefAssetLoadNow(AVAsset *asset)
{
    ReefAssetState *state = AssetState(asset, YES);
    if (!state)
        return NO;
    pthread_mutex_lock(&state->lock);
    if (state->status == AVKeyValueStatusUnknown || state->status == AVKeyValueStatusLoading)
        ProbeLocked(asset, state);
    BOOL ok = state->status == AVKeyValueStatusLoaded;
    pthread_mutex_unlock(&state->lock);
    return ok;
}

static void LoadAsync(AVAsset *asset, ReefAssetState *state, void (^handler)(void))
{
    pthread_mutex_lock(&state->lock);
    BOOL done = state->status == AVKeyValueStatusLoaded || state->status == AVKeyValueStatusFailed;
    BOOL start = state->status == AVKeyValueStatusUnknown;
    if (!done && handler)
        [state->handlers addObject:[handler copy]];
    if (start)
        state->status = AVKeyValueStatusLoading;
    pthread_mutex_unlock(&state->lock);
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    if (done) {
        if (handler)
            dispatch_async(queue, handler);
        return;
    }
    if (!start)
        return;
    dispatch_async(queue, ^{
        pthread_mutex_lock(&state->lock);
        if (state->status == AVKeyValueStatusLoading)
            ProbeLocked(asset, state);
        NSArray *handlers = [state->handlers copy];
        [state->handlers removeAllObjects];
        pthread_mutex_unlock(&state->lock);
        for (void (^h)(void) in handlers)
            h();
    });
}

static NSArray *AssetTracks(AVAsset *asset)
{
    if ([asset isKindOfClass:[AVComposition class]])
        return [asset tracks];
    ReefAssetState *state = AssetState(asset, YES);
    if (!state)
        return @[];
    ReefAssetLoadNow(asset);
    return state->tracks ?: @[];
}

static AVAssetTrack *FirstTrack(NSArray *tracks, NSString *type)
{
    for (AVAssetTrack *t in tracks)
        if ([t.mediaType isEqualToString:type])
            return t;
    return nil;
}

ReefMediaSource *ReefSourceForAsset(AVAsset *asset)
{
    if (!asset)
        return nil;
    ReefMediaSource *source = [ReefMediaSource new];
    if ([asset isKindOfClass:[AVComposition class]]) {
        // a composition plays the file of its first segment, shifted as that segment is (one file only for now)
        NSArray *tracks = asset.tracks;
        AVAssetTrack *main = FirstTrack(tracks, AVMediaTypeVideo) ?: FirstTrack(tracks, AVMediaTypeAudio);
        ReefSegment *first = Segments(main, NO).firstObject;
        AVAsset *file = first ? TrackState(first->track)->asset : nil;
        if (!file || !ReefAssetLoadNow(file))
            return nil;
        source.path = AssetState(file, NO)->path;
        source.offset = Seconds(first->sourceRange.start) - Seconds(first->start);
        source.duration = Seconds(asset.duration);
        source.hasVideo = FirstTrack(tracks, AVMediaTypeVideo) != nil;
        source.hasAudio = FirstTrack(tracks, AVMediaTypeAudio) != nil;
        AVAssetTrack *video = FirstTrack(tracks, AVMediaTypeVideo);
        source.naturalSize = video ? video.naturalSize : CGSizeZero;
        return source;
    }
    ReefAssetState *state = AssetState(asset, YES);
    if (!state || !ReefAssetLoadNow(asset))
        return nil;
    source.path = state->path;
    source.offset = 0;
    source.duration = state->info.duration;
    AVAssetTrack *video = FirstTrack(state->tracks, AVMediaTypeVideo);
    source.hasVideo = video != nil;
    source.hasAudio = FirstTrack(state->tracks, AVMediaTypeAudio) != nil;
    source.naturalSize = video ? video.naturalSize : CGSizeZero;
    return source;
}

#pragma mark - AVAsset

@implementation AVAsset (ReefPlayback)

- (void)loadValuesAsynchronouslyForKeys:(NSArray<NSString *> *)keys completionHandler:(void (^)(void))handler
{
    ReefLog(@"%s %p load %@", object_getClassName(self), self, [keys componentsJoinedByString:@","]);
    ReefAssetState *state = AssetState(self, YES);
    if (!state) {
        if (handler)
            dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), handler);
        return;
    }
    LoadAsync(self, state, handler);
}

- (AVKeyValueStatus)statusOfValueForKey:(NSString *)key error:(NSError *__autoreleasing *)outError
{
    ReefAssetState *state = AssetState(self, YES);
    if (!state)
        return AVKeyValueStatusLoaded;
    pthread_mutex_lock(&state->lock);
    AVKeyValueStatus status = state->status;
    NSError *error = state->error;
    pthread_mutex_unlock(&state->lock);
    if (status == AVKeyValueStatusFailed && outError)
        *outError = error;
    if (status != AVKeyValueStatusLoaded)
        ReefLog(@"%s %p status of %@: %ld", object_getClassName(self), self, key, (long)status);
    return status;
}

- (void)cancelLoading
{
}

- (void)loadTracksWithMediaType:(NSString *)type completionHandler:(void (^)(NSArray *, NSError *))handler
{
    [self loadValuesAsynchronouslyForKeys:@[@"tracks"] completionHandler:^{
        NSError *error = nil;
        [self statusOfValueForKey:@"tracks" error:&error];
        handler([self tracksWithMediaType:type], error);
    }];
}

- (void)loadTrackWithTrackID:(CMPersistentTrackID)trackID completionHandler:(void (^)(AVAssetTrack *, NSError *))handler
{
    [self loadValuesAsynchronouslyForKeys:@[@"tracks"] completionHandler:^{
        NSError *error = nil;
        [self statusOfValueForKey:@"tracks" error:&error];
        handler([self trackWithTrackID:trackID], error);
    }];
}

- (CMTime)duration
{
    ReefAssetState *state = AssetState(self, YES);
    if (!state || !ReefAssetLoadNow(self))
        return kCMTimeZero;
    return ReefTime(state->info.duration);
}

- (NSArray<AVAssetTrack *> *)tracks
{
    return AssetTracks(self);
}

- (NSArray<AVAssetTrack *> *)tracksWithMediaType:(NSString *)type
{
    NSMutableArray *found = [NSMutableArray array];
    for (AVAssetTrack *t in self.tracks)
        if ([t.mediaType isEqualToString:type])
            [found addObject:t];
    return found;
}

- (NSArray<AVAssetTrack *> *)tracksWithMediaCharacteristic:(NSString *)characteristic
{
    NSMutableArray *found = [NSMutableArray array];
    for (AVAssetTrack *t in self.tracks)
        if ([t hasMediaCharacteristic:characteristic])
            [found addObject:t];
    return found;
}

- (AVAssetTrack *)trackWithTrackID:(CMPersistentTrackID)trackID
{
    for (AVAssetTrack *t in self.tracks)
        if (t.trackID == trackID)
            return t;
    return nil;
}

- (BOOL)isPlayable
{
    if ([self isKindOfClass:[AVComposition class]])
        return self.tracks.count > 0;
    return ReefAssetLoadNow(self);
}
- (BOOL)playable { return [self isPlayable]; }
- (BOOL)isReadable { return [self isPlayable]; }
- (BOOL)readable { return [self isPlayable]; }
- (BOOL)isExportable { return NO; }
- (BOOL)exportable { return NO; }
- (BOOL)isComposable { return [self isPlayable]; }
- (BOOL)hasProtectedContent { return NO; }
- (BOOL)providesPreciseDurationAndTiming { return YES; }
- (BOOL)containsFragments { return NO; }
- (BOOL)canContainFragments { return NO; }
- (BOOL)isCompatibleWithAirPlayVideo { return NO; }
- (BOOL)isCompatibleWithSavedPhotosAlbum { return [self isPlayable]; }
- (AVAssetReferenceRestrictions)referenceRestrictions { return AVAssetReferenceRestrictionForbidNone; }

- (CGSize)naturalSize
{
    AVAssetTrack *video = FirstTrack(self.tracks, AVMediaTypeVideo);
    return video ? video.naturalSize : CGSizeZero;
}

- (CGAffineTransform)preferredTransform { return CGAffineTransformIdentity; }
- (float)preferredRate { return 1; }
- (float)preferredVolume { return 1; }
- (CMTimeScale)naturalTimeScale { return 600; }
- (CMTime)overallDurationHint { return self.duration; }
- (CMTime)minimumTimeOffsetFromLive { return kCMTimeInvalid; }
- (NSArray *)metadata { return @[]; }
- (NSArray *)commonMetadata { return @[]; }
- (NSArray *)availableMetadataFormats { return @[]; }
- (NSArray *)metadataForFormat:(NSString *)format { return @[]; }
- (NSArray *)availableChapterLocales { return @[]; }
- (NSArray *)chapterMetadataGroupsWithTitleLocale:(NSLocale *)locale containingItemsWithCommonKeys:(NSArray *)keys { return @[]; }
- (NSArray *)chapterMetadataGroupsBestMatchingPreferredLanguages:(NSArray *)languages { return @[]; }
- (NSArray *)availableMediaCharacteristicsWithMediaSelectionOptions { return @[]; }
- (AVMediaSelectionGroup *)mediaSelectionGroupForMediaCharacteristic:(NSString *)characteristic { return nil; }
- (AVMediaSelection *)preferredMediaSelection { return nil; }
- (NSArray *)allMediaSelections { return @[]; }
- (NSArray *)trackGroups { return @[]; }
- (NSArray *)alternateTrackGroups { return @[]; }
- (id)audioAlternatesTrackGroup { return nil; }
- (NSDictionary *)trackReferences { return @{}; }
- (NSArray *)availableVideoDynamicRanges { return @[]; }
- (AVMetadataItem *)creationDate { return nil; }
- (NSString *)lyrics { return nil; }

@end

#pragma mark - AVURLAsset

@implementation AVURLAsset (ReefPlayback)
- (NSURL *)resolvedURL { return self.URL; }
- (id)assetCache { return nil; }
- (NSDictionary *)mediaExtensionProperties { return nil; }
- (BOOL)mayRequireContentKeysForMediaDataProcessing { return NO; }
- (id)httpSessionIdentifier { return nil; }
+ (NSArray *)audiovisualTypes
{
    return @[@"com.apple.quicktime-movie", @"public.mpeg-4", @"com.apple.m4v-video", @"com.apple.m4a-audio", @"public.mp3",
             @"com.microsoft.waveform-audio", @"public.aiff-audio", @"public.aifc-audio", @"org.matroska.mkv", @"org.webmproject.webm",
             @"public.avi", @"public.mpeg", @"public.mpeg-2-transport-stream", @"public.3gpp", @"org.xiph.flac", @"org.xiph.ogg-audio"];
}
+ (BOOL)isPlayableExtendedMIMEType:(NSString *)type { return ReefMediaAvailable(); }
@end

#pragma mark - AVAssetTrack

@implementation AVAssetTrack (ReefPlayback)

// A composition track answers from the track of its first segment.
static AVAssetTrack *SourceTrack(AVAssetTrack *track)
{
    if (TrackState(track))
        return track;
    ReefSegment *first = Segments(track, NO).firstObject;
    return first ? first->track : nil;
}

- (AVAsset *)asset
{
    ReefTrackState *state = TrackState(self);
    return state ? state->asset : nil;
}

- (CMPersistentTrackID)trackID
{
    ReefTrackState *state = TrackState(self);
    return state ? state->stream.index + 1 : 0;
}

- (NSString *)mediaType
{
    ReefTrackState *state = TrackState(self);
    return state ? MediaTypeOf(state->stream.type) : AVMediaTypeVideo;
}

- (NSArray *)formatDescriptions
{
    ReefTrackState *state = TrackState(SourceTrack(self));
    return state && state->format ? @[(__bridge id)state->format] : @[];
}

- (BOOL)isPlayable { return YES; }
- (BOOL)isDecodable { return YES; }
- (BOOL)isEnabled { return YES; }
- (BOOL)isSelfContained { return YES; }
- (BOOL)hasProtectedContent { return NO; }
- (BOOL)requiresFrameReordering { return NO; }

- (long long)totalSampleDataLength
{
    ReefTrackState *state = TrackState(SourceTrack(self));
    return state ? (long long)(state->stream.bitRate / 8 * state->stream.duration) : 0;
}

- (CMTimeRange)timeRange
{
    ReefTrackState *state = TrackState(self);
    if (state)
        return CMTimeRangeMake(kCMTimeZero, ReefTime(state->stream.duration > 0 ? state->stream.duration : state->fileDuration));
    CMTime end = kCMTimeZero;
    for (ReefSegment *s in Segments(self, NO))
        end = CMTimeMaximum(end, CMTimeAdd(s->start, s->sourceRange.duration));
    return CMTimeRangeMake(kCMTimeZero, end);
}

- (CMTimeScale)naturalTimeScale
{
    ReefTrackState *state = TrackState(SourceTrack(self));
    return state ? state->stream.timeScale : 600;
}

- (float)estimatedDataRate
{
    ReefTrackState *state = TrackState(SourceTrack(self));
    return state ? (float)state->stream.bitRate : 0;
}

- (NSString *)languageCode
{
    ReefTrackState *state = TrackState(SourceTrack(self));
    return state && state->stream.language[0] && strcmp(state->stream.language, "und") ? @(state->stream.language) : nil;
}

- (NSString *)extendedLanguageTag { return self.languageCode; }

- (CGSize)naturalSize
{
    ReefTrackState *state = TrackState(SourceTrack(self));
    if (!state || state->stream.type != ReefMediaVideo)
        return CGSizeZero;
    return CGSizeMake(round(state->stream.width * state->stream.pixelAspect), state->stream.height);
}

- (CGSize)dimensions { return self.naturalSize; }

- (CGAffineTransform)preferredTransform { return CGAffineTransformIdentity; }
- (float)preferredVolume { return 1; }

- (float)nominalFrameRate
{
    ReefTrackState *state = TrackState(SourceTrack(self));
    return state ? (float)state->stream.frameRate : 0;
}

- (CMTime)minFrameDuration
{
    float rate = self.nominalFrameRate;
    return rate > 0 ? CMTimeMake(600 / rate > 0 ? (int64_t)lround(600 / rate) : 20, 600) : kCMTimeInvalid;
}

- (BOOL)hasMediaCharacteristic:(NSString *)characteristic
{
    NSString *type = self.mediaType;
    if ([characteristic isEqualToString:AVMediaCharacteristicVisual])
        return [type isEqualToString:AVMediaTypeVideo];
    if ([characteristic isEqualToString:AVMediaCharacteristicAudible])
        return [type isEqualToString:AVMediaTypeAudio];
    if ([characteristic isEqualToString:AVMediaCharacteristicLegible])
        return [type isEqualToString:AVMediaTypeSubtitle];
    return NO;
}

- (NSArray *)segments { return @[]; }
- (NSArray *)metadata { return @[]; }
- (NSArray *)commonMetadata { return @[]; }
- (NSArray *)availableMetadataFormats { return @[]; }
- (NSArray *)metadataForFormat:(NSString *)format { return @[]; }
- (NSArray *)availableTrackAssociationTypes { return @[]; }
- (NSArray *)associatedTracksOfType:(NSString *)type { return @[]; }
- (id)layer { return nil; }
- (BOOL)canProvideSampleCursors { return NO; }
- (BOOL)hasAudioSampleDependencies { return NO; }

- (void)loadValuesAsynchronouslyForKeys:(NSArray<NSString *> *)keys completionHandler:(void (^)(void))handler
{
    ReefLog(@"track %d load %@", self.trackID, [keys componentsJoinedByString:@","]);
    if (handler)
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), handler);
}

- (AVKeyValueStatus)statusOfValueForKey:(NSString *)key error:(NSError *__autoreleasing *)outError
{
    return AVKeyValueStatusLoaded;
}

@end

#pragma mark - compositions

@implementation AVCompositionTrack (ReefPlayback)
- (CMTimeRange)timeRange
{
    CMTime end = kCMTimeZero;
    for (ReefSegment *s in Segments(self, NO))
        end = CMTimeMaximum(end, CMTimeAdd(s->start, s->sourceRange.duration));
    return CMTimeRangeMake(kCMTimeZero, end);
}
- (NSArray *)segments { return @[]; }
@end

@implementation AVMutableCompositionTrack (ReefPlayback)

- (BOOL)insertTimeRange:(CMTimeRange)range ofTrack:(AVAssetTrack *)track atTime:(CMTime)start error:(NSError *__autoreleasing *)outError
{
    AVAssetTrack *source = SourceTrack(track);
    if (!source || !CMTIMERANGE_IS_VALID(range)) {
        if (outError)
            *outError = [NSError errorWithDomain:AVFoundationErrorDomain code:AVErrorInvalidSourceMedia userInfo:nil];
        return NO;
    }
    // a range of a composition track becomes ranges of its own sources
    if (source != track) {
        ReefSegment *first = Segments(track, NO).firstObject;
        range.start = CMTimeAdd(range.start, CMTimeSubtract(first->sourceRange.start, first->start));
    }
    NSMutableArray *segments = Segments(self, YES);
    CMTime shift = range.duration;
    for (ReefSegment *s in segments)
        if (CMTimeCompare(s->start, start) >= 0)
            s->start = CMTimeAdd(s->start, shift);
    ReefSegment *segment = [ReefSegment new];
    segment->track = source;
    segment->sourceRange = range;
    segment->start = start;
    [segments addObject:segment];
    [segments sortUsingComparator:^NSComparisonResult(ReefSegment *a, ReefSegment *b) { return CMTimeCompare(a->start, b->start); }];
    ReefLog(@"composition track %d: %.3f s of track %d at %.3f", self.trackID, Seconds(range.duration), source.trackID, Seconds(start));
    return YES;
}

- (BOOL)insertTimeRanges:(NSArray<NSValue *> *)ranges ofTracks:(NSArray<AVAssetTrack *> *)tracks atTime:(CMTime)start error:(NSError *__autoreleasing *)outError
{
    for (NSUInteger i = 0; i < ranges.count && i < tracks.count; i++) {
        CMTimeRange range = ranges[i].CMTimeRangeValue;
        if (![self insertTimeRange:range ofTrack:tracks[i] atTime:start error:outError])
            return NO;
        start = CMTimeAdd(start, range.duration);
    }
    return YES;
}

- (void)insertEmptyTimeRange:(CMTimeRange)range
{
    for (ReefSegment *s in Segments(self, NO))
        if (CMTimeCompare(s->start, range.start) >= 0)
            s->start = CMTimeAdd(s->start, range.duration);
}

- (void)removeTimeRange:(CMTimeRange)range
{
    NSMutableArray *segments = Segments(self, NO);
    NSMutableArray *kept = [NSMutableArray array];
    CMTime cut = range.duration, end = CMTimeRangeGetEnd(range);
    for (ReefSegment *s in segments) {
        CMTime sEnd = CMTimeAdd(s->start, s->sourceRange.duration);
        if (CMTimeCompare(sEnd, range.start) <= 0) {
            [kept addObject:s];
        } else if (CMTimeCompare(s->start, end) >= 0) {
            s->start = CMTimeSubtract(s->start, cut);
            [kept addObject:s];
        } else {
            // overlapping: keep the parts before and after the range
            if (CMTimeCompare(s->start, range.start) < 0) {
                ReefSegment *head = [ReefSegment new];
                head->track = s->track;
                head->start = s->start;
                head->sourceRange = CMTimeRangeMake(s->sourceRange.start, CMTimeSubtract(range.start, s->start));
                [kept addObject:head];
            }
            if (CMTimeCompare(sEnd, end) > 0) {
                ReefSegment *tail = [ReefSegment new];
                tail->track = s->track;
                CMTime skipped = CMTimeSubtract(end, s->start);
                tail->sourceRange = CMTimeRangeMake(CMTimeAdd(s->sourceRange.start, skipped), CMTimeSubtract(sEnd, end));
                tail->start = range.start;
                [kept addObject:tail];
            }
        }
    }
    [segments setArray:kept];
}

- (void)scaleTimeRange:(CMTimeRange)range toDuration:(CMTime)duration
{
}

- (BOOL)validateTrackSegments:(NSArray *)segments error:(NSError *__autoreleasing *)outError
{
    return YES;
}

@end

@implementation AVMutableComposition (ReefPlayback)

- (BOOL)insertTimeRange:(CMTimeRange)range ofAsset:(AVAsset *)asset atTime:(CMTime)start error:(NSError *__autoreleasing *)outError
{
    if (![asset isKindOfClass:[AVComposition class]] && !ReefAssetLoadNow(asset)) {
        if (outError)
            *outError = [NSError errorWithDomain:AVFoundationErrorDomain code:AVErrorInvalidSourceMedia userInfo:nil];
        return NO;
    }
    for (AVAssetTrack *track in asset.tracks) {
        AVMutableCompositionTrack *target = nil;
        for (AVMutableCompositionTrack *t in [self tracksWithMediaType:track.mediaType])
            if (!target)
                target = t;
        if (!target)
            target = [self addMutableTrackWithMediaType:track.mediaType preferredTrackID:kCMPersistentTrackID_Invalid];
        if (![target insertTimeRange:range ofTrack:track atTime:start error:outError])
            return NO;
    }
    return YES;
}

- (void)insertEmptyTimeRange:(CMTimeRange)range
{
    for (AVMutableCompositionTrack *t in self.tracks)
        [t insertEmptyTimeRange:range];
}

- (void)removeTimeRange:(CMTimeRange)range
{
    for (AVMutableCompositionTrack *t in self.tracks)
        [t removeTimeRange:range];
}

- (CMTime)duration
{
    CMTime end = kCMTimeZero;
    for (AVAssetTrack *t in self.tracks)
        end = CMTimeMaximum(end, CMTimeRangeGetEnd(t.timeRange));
    return end;
}

- (id)copyWithZone:(NSZone *)zone { return self; }
- (id)mutableCopyWithZone:(NSZone *)zone { return self; }

@end
