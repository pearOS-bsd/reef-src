// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Playback on the media engine (ReefMedia.h). Every AVPlayerItem owns an engine: a thread decodes ahead into a queue of
// pictures and a ring of sound, a thread writes the sound to the host's audio output (the output of Reef's AppKit host,
// PulseAudio or a paced null sink), and the clock is the host's monotonic time scaled by the rate. AVPlayer drives the
// rate and the time observers; on the main thread a 60 Hz timer gives each AVPlayerLayer the picture of the current time
// (as its contents). Key-value notifications are posted on the main thread, as AVFoundation does.

#import "ReefPlayback.h"
#import <QuartzCore/QuartzCore.h>
#import <objc/runtime.h>
#import <objc/message.h>
#import <pthread.h>
#import <time.h>
#import <unistd.h>

extern void *reef_host_symbol(const char *libraries, const char *name);

static double Now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

static double Seconds(CMTime t)
{
    return CMTIME_IS_NUMERIC(t) ? CMTimeGetSeconds(t) : NAN;
}

static void OnMain(dispatch_block_t block)
{
    if ([NSThread isMainThread])
        block();
    else
        dispatch_async(dispatch_get_main_queue(), block);
}

#pragma mark - host audio

// The audio output of Reef's AppKit host (interface version 24): create, write (blocks while the output is full),
// latency (nanoseconds), flush, destroy.
static struct {
    void *(*create)(const char *, double, int, int);
    int (*write)(void *, const float *, int);
    uint64_t (*latency)(void *);
    void (*flush)(void *);
    void (*destroy)(void *);
} HostAudio;

static int HostAudioAvailable(void)
{
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        const void *(*interface)(int) = reef_host_symbol("@root/host/libreef-appkit-host.so", "reef_appkit_host_interface");
        void *const *table = interface ? (void *const *)interface(24) : NULL;
        if (!table)
            return;
        *(void **)&HostAudio.create = table[1 + 216];
        *(void **)&HostAudio.write = table[1 + 217];
        *(void **)&HostAudio.latency = table[1 + 218];
        *(void **)&HostAudio.flush = table[1 + 219];
        *(void **)&HostAudio.destroy = table[1 + 220];
    });
    return HostAudio.create != NULL;
}

#pragma mark - engine

enum { MaxFrames = 24, FullFrames = 8 };

@class ReefPlaybackEngine;
@protocol ReefEngineOwner
- (void)engineDidOpen:(ReefPlaybackEngine *)engine;
- (void)engineDidFail:(ReefPlaybackEngine *)engine;
@end

@interface ReefPlaybackEngine : NSObject {
@public
    pthread_mutex_t lock;
    pthread_cond_t changed;
    ReefMediaSource *source;
    ReefMediaReader *reader;
    pthread_t decoder, writer;
    BOOL decoding, writing, stop;
    // decoded pictures, oldest first
    ReefVideoFrame *frames[MaxFrames];
    int frameCount;
    // decoded sound
    float *ring;
    int ringCapacity, ringStart, ringCount; // in sample frames
    double ringTime;                        // time of the oldest sound in the ring
    int sampleRate, channels;
    BOOL eof;
    // seeking
    BOOL seekPending;
    double seekTarget;
    unsigned generation;
    double dropBefore;
    // the clock
    double position, rate, anchor;
    // the picture on show (given to the layers)
    BOOL hasShown;
    double shownTime, shownDuration;
    BOOL wantsPicture; // show the picture of `position` even when paused
    // sound
    void *output;
    BOOL flushOutput;
    long long written; // sample frames given to the output
    float volume;
    BOOL muted;
    __weak id<ReefEngineOwner> owner;
    BOOL open;
}
@end

static void *DecodeThread(void *argument);
static void *WriteThread(void *argument);

@implementation ReefPlaybackEngine

- (instancetype)initWithSource:(ReefMediaSource *)aSource owner:(id<ReefEngineOwner>)anOwner
{
    if ((self = [super init])) {
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&changed, NULL);
        source = aSource;
        owner = anOwner;
        volume = 1;
        wantsPicture = YES;
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
            [self openReader];
        });
    }
    return self;
}

- (void)openReader
{
    ReefMediaReader *r = ReefMediaReaderOpen(source.path.fileSystemRepresentation, 1, 1, 0, 0);
    if (!r) {
        ReefLog(@"engine: cannot open %@", source.path);
        id<ReefEngineOwner> o = owner;
        OnMain(^{ [o engineDidFail:self]; });
        return;
    }
    pthread_mutex_lock(&lock);
    if (stop) {
        pthread_mutex_unlock(&lock);
        ReefMediaReaderClose(r);
        return;
    }
    reader = r;
    if (ReefMediaReaderHasAudio(r)) {
        sampleRate = ReefMediaReaderSampleRate(r);
        channels = ReefMediaReaderChannels(r);
        ringCapacity = sampleRate * 2;
        ring = calloc((size_t)ringCapacity * channels, sizeof(float));
    }
    if (source.offset > 0.001 || position > 0) {
        seekPending = YES;
        seekTarget = position;
    }
    decoding = pthread_create(&decoder, NULL, DecodeThread, (__bridge void *)self) == 0;
    if (ring && HostAudioAvailable()) {
        output = HostAudio.create("Reef AVPlayer", sampleRate, channels, 1024);
        if (output)
            writing = pthread_create(&writer, NULL, WriteThread, (__bridge void *)self) == 0;
    }
    open = YES;
    pthread_mutex_unlock(&lock);
    ReefLog(@"engine: %@ video %d audio %d (%d Hz, %d ch) output %p", source.path.lastPathComponent, ReefMediaReaderHasVideo(r),
            ReefMediaReaderHasAudio(r), sampleRate, channels, output);
    id<ReefEngineOwner> o = owner;
    OnMain(^{ [o engineDidOpen:self]; });
}

- (void)close
{
    pthread_mutex_lock(&lock);
    stop = YES;
    pthread_cond_broadcast(&changed);
    pthread_mutex_unlock(&lock);
    if (decoding)
        pthread_join(decoder, NULL);
    if (writing)
        pthread_join(writer, NULL);
    decoding = writing = NO;
    if (output)
        HostAudio.destroy(output);
    output = NULL;
    for (int i = 0; i < frameCount; i++)
        ReefVideoFrameFree(frames[i]);
    frameCount = 0;
    if (reader)
        ReefMediaReaderClose(reader);
    reader = NULL;
    free(ring);
    ring = NULL;
}

- (void)dealloc
{
    [self close];
}

// The time of the clock, in item time; the lock is held.
static double ClockLocked(ReefPlaybackEngine *e)
{
    double t = e->position;
    if (e->rate != 0)
        t += (Now() - e->anchor) * e->rate;
    double end = e->source.duration;
    if (end > 0 && t > end)
        t = end;
    return t < 0 ? 0 : t;
}

- (double)currentTime
{
    pthread_mutex_lock(&lock);
    double t = ClockLocked(self);
    pthread_mutex_unlock(&lock);
    return t;
}

- (void)setRate:(double)newRate
{
    pthread_mutex_lock(&lock);
    position = ClockLocked(self);
    anchor = Now();
    if (newRate == 0 && rate != 0)
        flushOutput = YES;
    rate = newRate;
    pthread_cond_broadcast(&changed);
    pthread_mutex_unlock(&lock);
}

- (void)seekTo:(double)t
{
    double end = source.duration;
    if (end > 0 && t > end)
        t = end;
    if (t < 0)
        t = 0;
    pthread_mutex_lock(&lock);
    position = t;
    anchor = Now();
    seekPending = YES;
    seekTarget = t;
    generation++;
    for (int i = 0; i < frameCount; i++)
        ReefVideoFrameFree(frames[i]);
    frameCount = 0;
    ringStart = ringCount = 0;
    eof = NO;
    flushOutput = YES;
    wantsPicture = YES;
    pthread_cond_broadcast(&changed);
    pthread_mutex_unlock(&lock);
}

- (void)setVolume:(float)v muted:(BOOL)m
{
    pthread_mutex_lock(&lock);
    volume = v;
    muted = m;
    pthread_mutex_unlock(&lock);
}

// The picture to show now (the caller owns it), or NULL when the one on show stays; *ended when the clock reached the
// end of everything.
- (ReefVideoFrame *)takePicture:(BOOL *)ended
{
    *ended = NO;
    pthread_mutex_lock(&lock);
    double t = ClockLocked(self);
    ReefVideoFrame *pick = NULL;
    int used = 0;
    // the newest picture whose time has come (when paused after a seek: the first one at or after the position)
    for (int i = 0; i < frameCount; i++) {
        if (frames[i]->time <= t + 0.004 || (wantsPicture && i == 0 && rate == 0)) {
            pick = frames[i];
            used = i + 1;
            if (rate == 0 && wantsPicture)
                break;
        } else {
            break;
        }
    }
    if (pick) {
        for (int i = 0; i < used - 1; i++)
            ReefVideoFrameFree(frames[i]);
        memmove(frames, frames + used, sizeof(*frames) * (frameCount - used));
        frameCount -= used;
        hasShown = YES;
        shownTime = pick->time;
        shownDuration = pick->duration;
        wantsPicture = NO;
        pthread_cond_broadcast(&changed);
    }
    double end = source.duration;
    // the end: the clock reached the duration, or everything decoded has been shown and heard
    double lastMedia = MAX(hasShown ? shownTime + shownDuration : 0, ring ? ringTime : 0);
    if (rate > 0 && ((end > 0 && t >= end - 0.001) || (eof && frameCount == 0 && ringCount == 0 && t >= lastMedia)))
        *ended = YES;
    pthread_mutex_unlock(&lock);
    return pick;
}

static BOOL FullLocked(ReefPlaybackEngine *e)
{
    if (e->frameCount >= MaxFrames)
        return YES;
    BOOL video = ReefMediaReaderHasVideo(e->reader), audio = e->ring != NULL;
    BOOL framesFull = !video || e->frameCount >= FullFrames;
    BOOL soundFull = !audio || e->ringCount >= e->ringCapacity - e->sampleRate / 4;
    return framesFull && soundFull;
}

static void AddSoundLocked(ReefPlaybackEngine *e, ReefAudioChunk *chunk)
{
    int skip = 0, frames = chunk->frames;
    double t = chunk->time - e->source.offset;
    if (t < e->dropBefore) {
        skip = (int)((e->dropBefore - t) * e->sampleRate);
        if (skip >= frames)
            return;
        t += (double)skip / e->sampleRate;
    }
    if (e->ringCount == 0)
        e->ringTime = t;
    int room = e->ringCapacity - e->ringCount;
    int n = frames - skip < room ? frames - skip : room;
    for (int i = 0; i < n; i++) {
        int at = (e->ringStart + e->ringCount + i) % e->ringCapacity;
        memcpy(e->ring + (size_t)at * e->channels, chunk->samples + (size_t)(skip + i) * chunk->channels, sizeof(float) * e->channels);
    }
    e->ringCount += n;
}

static void *DecodeThread(void *argument)
{
    ReefPlaybackEngine *e = (__bridge ReefPlaybackEngine *)argument;
    pthread_setname_np("com.apple.avfoundation.decode");
    pthread_mutex_lock(&e->lock);
    while (!e->stop) {
        if (e->seekPending) {
            double target = e->seekTarget + e->source.offset;
            e->seekPending = NO;
            e->dropBefore = e->seekTarget;
            pthread_mutex_unlock(&e->lock);
            ReefMediaReaderSeek(e->reader, target);
            pthread_mutex_lock(&e->lock);
            continue;
        }
        if (e->eof || FullLocked(e)) {
            pthread_cond_wait(&e->changed, &e->lock);
            continue;
        }
        unsigned generation = e->generation;
        pthread_mutex_unlock(&e->lock);
        ReefVideoFrame *frame = NULL;
        ReefAudioChunk *chunk = NULL;
        int got = ReefMediaReaderNext(e->reader, &frame, &chunk);
        pthread_mutex_lock(&e->lock);
        if (generation != e->generation || e->seekPending) {
            ReefVideoFrameFree(frame);
            ReefAudioChunkFree(chunk);
            continue;
        }
        if (got <= 0) {
            e->eof = YES;
            pthread_cond_broadcast(&e->changed);
            continue;
        }
        if (frame) {
            frame->time -= e->source.offset;
            // a picture that ends before the target of a seek is not shown
            if (frame->time + frame->duration <= e->dropBefore + 0.0005 && !(e->frameCount == 0 && frame->time >= e->dropBefore - frame->duration))
                ReefVideoFrameFree(frame);
            else
                e->frames[e->frameCount++] = frame;
        }
        if (chunk) {
            if (e->ring)
                AddSoundLocked(e, chunk);
            ReefAudioChunkFree(chunk);
        }
        pthread_cond_broadcast(&e->changed);
    }
    pthread_mutex_unlock(&e->lock);
    return NULL;
}

static void *WriteThread(void *argument)
{
    ReefPlaybackEngine *e = (__bridge ReefPlaybackEngine *)argument;
    pthread_setname_np("com.apple.avfoundation.audio");
    enum { Chunk = 1024 };
    float *buffer = malloc(sizeof(float) * Chunk * e->channels);
    pthread_mutex_lock(&e->lock);
    while (!e->stop) {
        if (e->flushOutput) {
            e->flushOutput = NO;
            pthread_mutex_unlock(&e->lock);
            HostAudio.flush(e->output);
            pthread_mutex_lock(&e->lock);
            continue;
        }
        double t = ClockLocked(e);
        if (e->rate <= 0 || e->ringCount == 0) {
            if (e->rate > 0 && e->eof) {
                pthread_mutex_unlock(&e->lock);
                usleep(10000);
                pthread_mutex_lock(&e->lock);
            } else {
                pthread_cond_wait(&e->changed, &e->lock);
            }
            continue;
        }
        // at another rate than 1 the sound is not played (it would not keep up with the pictures)
        if (e->rate != 1 && e->ringTime < t) {
            int late = (int)((t - e->ringTime) * e->sampleRate);
            if (late > e->ringCount)
                late = e->ringCount;
            e->ringStart = (e->ringStart + late) % e->ringCapacity;
            e->ringCount -= late;
            e->ringTime += (double)late / e->sampleRate;
            pthread_cond_broadcast(&e->changed);
            pthread_mutex_unlock(&e->lock);
            usleep(10000);
            pthread_mutex_lock(&e->lock);
            continue;
        }
        // sound that is late (after a stall) is dropped; sound that is early waits
        if (e->ringTime < t - 0.25) {
            int late = (int)((t - e->ringTime) * e->sampleRate);
            if (late > e->ringCount)
                late = e->ringCount;
            e->ringStart = (e->ringStart + late) % e->ringCapacity;
            e->ringCount -= late;
            e->ringTime += (double)late / e->sampleRate;
            pthread_cond_broadcast(&e->changed);
            continue;
        }
        int n = e->ringCount < Chunk ? e->ringCount : Chunk;
        float gain = e->muted ? 0 : e->volume;
        for (int i = 0; i < n; i++) {
            int at = (e->ringStart + i) % e->ringCapacity;
            for (int c = 0; c < e->channels; c++)
                buffer[i * e->channels + c] = e->ring[(size_t)at * e->channels + c] * gain;
        }
        e->ringStart = (e->ringStart + n) % e->ringCapacity;
        e->ringCount -= n;
        e->ringTime += (double)n / e->sampleRate;
        pthread_cond_broadcast(&e->changed);
        e->written += n;
        pthread_mutex_unlock(&e->lock);
        HostAudio.write(e->output, buffer, n);
        pthread_mutex_lock(&e->lock);
    }
    pthread_mutex_unlock(&e->lock);
    free(buffer);
    return NULL;
}

@end

#pragma mark - the picture as an image

static void ReleasePixels(void *info, const void *data, size_t size)
{
    ReefVideoFrameFree(info);
}

static CGImageRef CreateImage(ReefVideoFrame *frame)
{
    CGDataProviderRef provider = CGDataProviderCreateWithData(frame, frame->pixels, frame->bytesPerRow * frame->height, ReleasePixels);
    static CGColorSpaceRef space;
    if (!space)
        space = CGColorSpaceCreateDeviceRGB();
    // opaque BGRA as the host's native premultiplied ARGB32 (no conversion when it is drawn)
    CGImageRef image = CGImageCreate(frame->width, frame->height, 8, 32, frame->bytesPerRow, space,
                                     kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst, provider, NULL, false,
                                     kCGRenderingIntentDefault);
    CGDataProviderRelease(provider);
    return image;
}

#pragma mark - item state

static char kItemState, kPlayerState, kLayerState;

@interface ReefTimeObserver : NSObject {
@public
    double interval;           // periodic: seconds; boundary: 0
    NSArray<NSNumber *> *times; // boundary times
    dispatch_queue_t queue;
    void (^periodic)(CMTime);
    void (^boundary)(void);
    double next;
    BOOL removed;
}
@end
@implementation ReefTimeObserver
@end

@interface ReefItemState : NSObject <ReefEngineOwner> {
@public
    __weak AVPlayerItem *item;
    AVAsset *asset;
    ReefPlaybackEngine *engine;
    AVPlayerItemStatus status;
    NSError *error;
    ReefMediaSource *source;
    NSArray *tracks;
    CMTime forwardEnd, reverseEnd;
    __weak AVPlayer *player;
    NSMutableArray *seekHandlers;
    id delegate;
    AVAudioMix *audioMix;
    AVVideoComposition *videoComposition;
    NSString *pitchAlgorithm;
    NSMutableArray *outputs;
    BOOL reachedEnd;
}
@end

@interface ReefPlayerState : NSObject {
@public
    __weak AVPlayer *player;
    AVPlayerItem *item;
    float rate, volume, defaultRate;
    BOOL muted;
    AVPlayerActionAtItemEnd actionAtItemEnd;
    NSMutableArray<ReefTimeObserver *> *observers;
    NSHashTable *layers;
    double lastTime;
    BOOL automaticallyWaits, preventsSleep, appliesCriteria;
    NSString *multichannelStrategy;
}
@end

@interface ReefLayerState : NSObject {
@public
    AVPlayer *player;
    NSString *gravity;
    BOOL ready;
}
@end
@implementation ReefLayerState
@end

static ReefItemState *SetUpItem(AVPlayerItem *item, AVAsset *asset);

// Never nil for an item (one made without the initializers below gets an empty state).
static ReefItemState *ItemState(AVPlayerItem *item)
{
    if (!item)
        return nil;
    ReefItemState *state = objc_getAssociatedObject(item, &kItemState);
    return state ?: SetUpItem(item, nil);
}

static ReefPlayerState *PlayerState(AVPlayer *player)
{
    if (!player)
        return nil;
    ReefPlayerState *state = objc_getAssociatedObject(player, &kPlayerState);
    if (!state) {
        state = [ReefPlayerState new];
        state->player = player;
        objc_setAssociatedObject(player, &kPlayerState, state, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return state;
}

static ReefLayerState *LayerState(CALayer *layer)
{
    ReefLayerState *state = objc_getAssociatedObject(layer, &kLayerState);
    if (!state) {
        state = [ReefLayerState new];
        state->gravity = AVLayerVideoGravityResizeAspect;
        objc_setAssociatedObject(layer, &kLayerState, state, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return state;
}

static void Notify(id object, NSString *key, dispatch_block_t change)
{
    [object willChangeValueForKey:key];
    if (change)
        change();
    [object didChangeValueForKey:key];
}

#pragma mark - the display timer

// One timer on the main run loop (common modes) ticks every active player 60 times a second.
@interface ReefDisplayTimer : NSObject
+ (void)addPlayer:(AVPlayer *)player;
@end

@interface AVPlayer (ReefPlaybackTick)
- (BOOL)reef_tick;
@end

@implementation ReefDisplayTimer {
    NSTimer *timer;
    NSHashTable *players;
}

+ (ReefDisplayTimer *)shared
{
    static ReefDisplayTimer *shared;
    static dispatch_once_t once;
    dispatch_once(&once, ^{ shared = [ReefDisplayTimer new]; });
    return shared;
}

+ (void)addPlayer:(AVPlayer *)player
{
    OnMain(^{ [[self shared] add:player]; });
}

- (void)add:(AVPlayer *)player
{
    if (!players)
        players = [NSHashTable weakObjectsHashTable];
    [players addObject:player];
    if (!timer) {
        timer = [NSTimer timerWithTimeInterval:1 / 60.0 target:self selector:@selector(tick:) userInfo:nil repeats:YES];
        [[NSRunLoop mainRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];
    }
}

- (void)tick:(NSTimer *)t
{
    for (AVPlayer *player in [players allObjects])
        if (![player reef_tick])
            [players removeObject:player];
    if (!players.count) {
        [timer invalidate];
        timer = nil;
    }
}
@end

@implementation ReefPlayerState
@end

#pragma mark - AVPlayerItem

@implementation ReefItemState

- (void)engineDidOpen:(ReefPlaybackEngine *)e
{
    AVPlayerItem *it = item;
    if (!it || e != engine)
        return;
    NSMutableArray *itemTracks = [NSMutableArray array];
    Class trackClass = NSClassFromString(@"AVPlayerItemTrack");
    for (AVAssetTrack *t in asset.tracks) {
        id itemTrack = [trackClass new];
        [itemTrack setValue:t forKey:@"assetTrack"];
        [itemTracks addObject:itemTrack];
    }
    Notify(it, @"tracks", ^{ self->tracks = itemTracks; });
    Notify(it, @"duration", nil);
    Notify(it, @"presentationSize", nil);
    Notify(it, @"loadedTimeRanges", nil);
    Notify(it, @"seekableTimeRanges", nil);
    Notify(it, @"playbackLikelyToKeepUp", nil);
    Notify(it, @"playbackBufferFull", nil);
    Notify(it, @"hasVideo", nil);
    Notify(it, @"hasEnabledVideo", nil);
    Notify(it, @"hasEnabledAudio", nil);
    Notify(it, @"status", ^{ self->status = AVPlayerItemStatusReadyToPlay; });
    ReefLog(@"item ready: %.3f s, %@", source.duration, NSStringFromSize(NSSizeFromCGSize(source.naturalSize)));
    AVPlayer *p = player;
    if (p)
        [ReefDisplayTimer addPlayer:p];
}

- (void)engineDidFail:(ReefPlaybackEngine *)e
{
    AVPlayerItem *it = item;
    if (!it)
        return;
    error = [NSError errorWithDomain:AVFoundationErrorDomain code:AVErrorFileFailedToParse
                            userInfo:@{NSLocalizedDescriptionKey: @"Cannot Open", NSLocalizedFailureReasonErrorKey: @"This media cannot be played."}];
    Notify(it, @"error", nil);
    Notify(it, @"status", ^{ self->status = AVPlayerItemStatusFailed; });
    [[NSNotificationCenter defaultCenter] postNotificationName:AVPlayerItemFailedToPlayToEndTimeNotification object:it
                                                      userInfo:@{AVPlayerItemFailedToPlayToEndTimeErrorKey: error}];
}

- (void)start
{
    if (engine || !asset)
        return;
    AVAsset *a = asset;
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
        ReefMediaSource *s = ReefSourceForAsset(a);
        OnMain(^{
            if (self->engine)
                return;
            if (!s) {
                self->error = nil;
                [self engineDidFail:nil];
                return;
            }
            self->source = s;
            self->engine = [[ReefPlaybackEngine alloc] initWithSource:s owner:self];
        });
    });
}

- (void)dealloc
{
    [engine close];
}

@end

static ReefItemState *SetUpItem(AVPlayerItem *item, AVAsset *asset)
{
    ReefLog(@"item %p for %s %p", item, asset ? object_getClassName(asset) : "nothing", asset);
    ReefItemState *state = [ReefItemState new];
    state->item = item;
    state->asset = asset;
    state->forwardEnd = kCMTimeInvalid;
    state->reverseEnd = kCMTimeInvalid;
    state->seekHandlers = [NSMutableArray array];
    state->outputs = [NSMutableArray array];
    state->pitchAlgorithm = AVAudioTimePitchAlgorithmSpectral;
    objc_setAssociatedObject(item, &kItemState, state, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [state start];
    return state;
}

@implementation AVPlayerItem (ReefPlayback)

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)key
{
    return NO;
}

+ (instancetype)playerItemWithURL:(NSURL *)url { return [[self alloc] initWithURL:url]; }
+ (instancetype)playerItemWithAsset:(AVAsset *)asset { return [[self alloc] initWithAsset:asset]; }
+ (instancetype)playerItemWithAsset:(AVAsset *)asset automaticallyLoadedAssetKeys:(NSArray *)keys
{
    return [[self alloc] initWithAsset:asset automaticallyLoadedAssetKeys:keys];
}

- (instancetype)initWithURL:(NSURL *)url
{
    return [self initWithAsset:[AVURLAsset URLAssetWithURL:url options:nil]];
}

- (instancetype)initWithAsset:(AVAsset *)asset
{
    return [self initWithAsset:asset automaticallyLoadedAssetKeys:nil];
}

- (instancetype)initWithAsset:(AVAsset *)asset automaticallyLoadedAssetKeys:(NSArray *)keys
{
    struct objc_super s = {self, [NSObject class]};
    self = ((id (*)(struct objc_super *, SEL))objc_msgSendSuper)(&s, @selector(init));
    if (self)
        SetUpItem(self, asset);
    return self;
}

- (id)copyWithZone:(NSZone *)zone
{
    return [[AVPlayerItem alloc] initWithAsset:ItemState(self)->asset];
}

- (AVAsset *)asset { return ItemState(self)->asset; }
- (NSArray *)automaticallyLoadedAssetKeys { return @[]; }
- (AVPlayerItemStatus)status { return ItemState(self)->status; }
- (NSError *)error { return ItemState(self)->error; }
- (NSArray *)tracks { return ItemState(self)->tracks ?: @[]; }
- (AVPlayer *)_player { return ItemState(self)->player; }
- (NSURL *)_URL { AVAsset *a = ItemState(self)->asset; return [a isKindOfClass:[AVURLAsset class]] ? [(AVURLAsset *)a URL] : nil; }

- (CMTime)duration
{
    ReefItemState *state = ItemState(self);
    return state->status == AVPlayerItemStatusReadyToPlay ? ReefTime(state->source.duration) : kCMTimeIndefinite;
}
- (CMTime)_duration { return self.duration; }

- (CGSize)presentationSize
{
    ReefItemState *state = ItemState(self);
    return state->status == AVPlayerItemStatusReadyToPlay ? state->source.naturalSize : CGSizeZero;
}

- (BOOL)hasVideo { ReefItemState *s = ItemState(self); return s->status == AVPlayerItemStatusReadyToPlay && s->source.hasVideo; }
- (BOOL)hasEnabledVideo { return self.hasVideo; }
- (BOOL)hasEnabledAudio { ReefItemState *s = ItemState(self); return s->status == AVPlayerItemStatusReadyToPlay && s->source.hasAudio; }
- (BOOL)isAuthorizationRequiredForPlayback { return NO; }
- (BOOL)isApplicationAuthorizedForPlayback { return YES; }
- (BOOL)isContentAuthorizedForPlayback { return YES; }
- (NSInteger)contentAuthorizationRequestStatus { return 0; }
- (void)requestContentAuthorizationAsynchronouslyWithTimeoutInterval:(NSTimeInterval)t completionHandler:(void (^)(void))handler
{
    if (handler)
        dispatch_async(dispatch_get_main_queue(), handler);
}
- (void)cancelContentAuthorizationRequest {}
- (id)_applicationAuthorizationChallenge { return nil; }
- (id)_applicationAuthorizationChallengeProvider { return nil; }
- (void)_setApplicationAuthorizationChallengeResponse:(id)response {}

- (CMTime)currentTime
{
    ReefItemState *state = ItemState(self);
    return ReefTime(state->engine ? [state->engine currentTime] : 0);
}

- (NSDate *)currentDate { return nil; }

- (NSArray *)loadedTimeRanges
{
    ReefItemState *state = ItemState(self);
    if (state->status != AVPlayerItemStatusReadyToPlay)
        return @[];
    return @[[NSValue valueWithCMTimeRange:CMTimeRangeMake(kCMTimeZero, ReefTime(state->source.duration))]];
}
- (NSArray *)seekableTimeRanges { return self.loadedTimeRanges; }
- (BOOL)isPlaybackLikelyToKeepUp { return ItemState(self)->status == AVPlayerItemStatusReadyToPlay; }
- (BOOL)isPlaybackBufferFull { return self.isPlaybackLikelyToKeepUp; }
- (BOOL)isPlaybackBufferEmpty { return NO; }
- (BOOL)canPlayFastForward { return YES; }
- (BOOL)canPlaySlowForward { return YES; }
- (BOOL)canPlayReverse { return NO; }
- (BOOL)canPlayFastReverse { return NO; }
- (BOOL)canPlaySlowReverse { return NO; }
- (BOOL)canStepForward { return YES; }
- (BOOL)canStepBackward { return YES; }
- (BOOL)canUseNetworkResourcesForLiveStreamingWhilePaused { return NO; }
- (NSTimeInterval)preferredForwardBufferDuration { return 0; }
- (void)setPreferredForwardBufferDuration:(NSTimeInterval)d {}
- (double)preferredPeakBitRate { return 0; }
- (void)setPreferredPeakBitRate:(double)r {}
- (CMTimebaseRef)timebase { return NULL; }
- (NSArray *)timedMetadata { return nil; }
- (CMTime)forwardPlaybackEndTime { return ItemState(self)->forwardEnd; }
- (void)setForwardPlaybackEndTime:(CMTime)t { ReefItemState *s = ItemState(self); Notify(self, @"forwardPlaybackEndTime", ^{ s->forwardEnd = t; }); }
- (CMTime)reversePlaybackEndTime { return ItemState(self)->reverseEnd; }
- (void)setReversePlaybackEndTime:(CMTime)t { ReefItemState *s = ItemState(self); Notify(self, @"reversePlaybackEndTime", ^{ s->reverseEnd = t; }); }
- (AVAudioMix *)audioMix { return ItemState(self)->audioMix; }
- (void)setAudioMix:(AVAudioMix *)mix { ItemState(self)->audioMix = mix; }
- (AVVideoComposition *)videoComposition { return ItemState(self)->videoComposition; }
- (void)setVideoComposition:(AVVideoComposition *)c { ItemState(self)->videoComposition = c; }
- (NSString *)audioTimePitchAlgorithm { return ItemState(self)->pitchAlgorithm; }
- (void)setAudioTimePitchAlgorithm:(NSString *)a { ItemState(self)->pitchAlgorithm = a; }
- (NSArray *)outputs { return [ItemState(self)->outputs copy]; }
- (void)addOutput:(id)output { [ItemState(self)->outputs addObject:output]; }
- (void)removeOutput:(id)output { [ItemState(self)->outputs removeObject:output]; }
- (id)delegate { return ItemState(self)->delegate; }
- (void)setDelegate:(id)d { ItemState(self)->delegate = d; }
- (void)setShouldOptimizeForExclusivePlayback:(BOOL)b {}
- (BOOL)appliesPerFrameHDRDisplayMetadata { return NO; }
- (void)setAppliesPerFrameHDRDisplayMetadata:(BOOL)b {}
- (BOOL)seekingWaitsForVideoCompositionRendering { return NO; }
- (void)setSeekingWaitsForVideoCompositionRendering:(BOOL)b {}
- (BOOL)startsOnFirstEligibleVariant { return NO; }
- (void)setStartsOnFirstEligibleVariant:(BOOL)b {}
- (NSString *)identifier { return nil; }
- (AVMediaSelection *)currentMediaSelection { return nil; }
- (void)selectMediaOption:(id)option inMediaSelectionGroup:(id)group {}
- (void)selectMediaOptionAutomaticallyInMediaSelectionGroup:(id)group {}
- (id)accessLog { return nil; }
- (id)errorLog { return nil; }

- (void)seekToTime:(CMTime)time { [self seekToTime:time toleranceBefore:kCMTimePositiveInfinity toleranceAfter:kCMTimePositiveInfinity completionHandler:nil]; }
- (void)seekToTime:(CMTime)time completionHandler:(void (^)(BOOL))handler
{
    [self seekToTime:time toleranceBefore:kCMTimePositiveInfinity toleranceAfter:kCMTimePositiveInfinity completionHandler:handler];
}
- (void)seekToTime:(CMTime)time toleranceBefore:(CMTime)before toleranceAfter:(CMTime)after
{
    [self seekToTime:time toleranceBefore:before toleranceAfter:after completionHandler:nil];
}
- (void)seekToTime:(CMTime)time toleranceBefore:(CMTime)before toleranceAfter:(CMTime)after completionHandler:(void (^)(BOOL))handler
{
    ReefItemState *state = ItemState(self);
    double t = Seconds(time);
    if (!isnan(t) && state->engine) {
        [state->engine seekTo:t];
        state->reachedEnd = NO;
        AVPlayer *p = state->player;
        if (p)
            [ReefDisplayTimer addPlayer:p];
    }
    if (handler)
        dispatch_async(dispatch_get_main_queue(), ^{ handler(state->engine != nil); });
}
- (BOOL)seekToDate:(NSDate *)date { return NO; }
- (BOOL)seekToDate:(NSDate *)date completionHandler:(void (^)(BOOL))handler { if (handler) handler(NO); return NO; }
- (void)cancelPendingSeeks {}
- (void)stepByCount:(NSInteger)count
{
    ReefItemState *state = ItemState(self);
    AVAssetTrack *video = [state->asset tracksWithMediaType:AVMediaTypeVideo].firstObject;
    double rate = video.nominalFrameRate > 0 ? video.nominalFrameRate : 30;
    double t = Seconds(self.currentTime) + count / rate;
    [self seekToTime:ReefTime(t) toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero completionHandler:nil];
}

@end

#pragma mark - AVPlayer

@implementation AVPlayer (ReefPlayback)

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)key
{
    return NO;
}

+ (instancetype)playerWithURL:(NSURL *)url { return [[self alloc] initWithURL:url]; }
+ (instancetype)playerWithPlayerItem:(AVPlayerItem *)item { return [[self alloc] initWithPlayerItem:item]; }

- (instancetype)init
{
    struct objc_super s = {self, [NSObject class]};
    self = ((id (*)(struct objc_super *, SEL))objc_msgSendSuper)(&s, @selector(init));
    if (self) {
        ReefPlayerState *state = PlayerState(self);
        state->volume = 1;
        state->defaultRate = 1;
        state->actionAtItemEnd = AVPlayerActionAtItemEndPause;
        state->observers = [NSMutableArray array];
        state->layers = [NSHashTable weakObjectsHashTable];
        state->automaticallyWaits = YES;
    }
    return self;
}

- (instancetype)initWithURL:(NSURL *)url
{
    return [self initWithPlayerItem:[AVPlayerItem playerItemWithURL:url]];
}

- (instancetype)initWithPlayerItem:(AVPlayerItem *)item
{
    if ((self = [self init]))
        [self replaceCurrentItemWithPlayerItem:item];
    return self;
}

- (AVPlayerStatus)status { return AVPlayerStatusReadyToPlay; }
- (NSError *)error { return nil; }
- (AVPlayerItem *)currentItem { return PlayerState(self)->item; }

- (void)replaceCurrentItemWithPlayerItem:(AVPlayerItem *)item
{
    ReefPlayerState *state = PlayerState(self);
    if (state->item == item)
        return;
    ReefItemState *old = ItemState(state->item);
    if (old) {
        [old->engine setRate:0];
        old->player = nil;
    }
    Notify(self, @"currentItem", ^{ state->item = item; });
    ReefItemState *now = ItemState(item);
    if (now) {
        now->player = self;
        [now->engine setRate:state->rate];
        [now->engine setVolume:state->volume muted:state->muted];
    }
    for (CALayer *layer in [state->layers allObjects]) {
        [CATransaction begin];
        [CATransaction setDisableActions:YES];
        layer.contents = nil;
        [CATransaction commit];
    }
    [ReefDisplayTimer addPlayer:self];
}

- (float)rate { return PlayerState(self)->rate; }

- (void)setRate:(float)rate
{
    ReefPlayerState *state = PlayerState(self);
    if (state->rate == rate)
        return;
    ReefItemState *item = ItemState(state->item);
    // playing again at the end starts from the beginning, as AVPlayer does after the item ended
    if (rate != 0 && item && item->reachedEnd) {
        item->reachedEnd = NO;
        [item->engine seekTo:0];
    }
    [self willChangeValueForKey:@"timeControlStatus"];
    Notify(self, @"rate", ^{ state->rate = rate; });
    if (item)
        [item->engine setRate:rate];
    [self didChangeValueForKey:@"timeControlStatus"];
    ReefLog(@"player rate %.2f at %.3f", rate, item && item->engine ? [item->engine currentTime] : 0.0);
    [self reef_fireTimeJump];
    [ReefDisplayTimer addPlayer:self];
}

- (void)play { [self setRate:PlayerState(self)->defaultRate ?: 1]; }
- (void)pause { [self setRate:0]; }
- (void)playImmediatelyAtRate:(float)rate { [self setRate:rate]; }
- (float)defaultRate { return PlayerState(self)->defaultRate; }
- (void)setDefaultRate:(float)r { PlayerState(self)->defaultRate = r; }
- (void)setRate:(float)rate time:(CMTime)itemTime atHostTime:(CMTime)hostClockTime
{
    if (CMTIME_IS_NUMERIC(itemTime))
        [self seekToTime:itemTime];
    [self setRate:rate];
}

- (AVPlayerTimeControlStatus)timeControlStatus
{
    return PlayerState(self)->rate != 0 ? AVPlayerTimeControlStatusPlaying : AVPlayerTimeControlStatusPaused;
}
- (NSString *)reasonForWaitingToPlay { return nil; }
- (BOOL)automaticallyWaitsToMinimizeStalling { return PlayerState(self)->automaticallyWaits; }
- (void)setAutomaticallyWaitsToMinimizeStalling:(BOOL)b { PlayerState(self)->automaticallyWaits = b; }

- (CMTime)currentTime { return self.currentItem ? self.currentItem.currentTime : kCMTimeZero; }

- (void)seekToTime:(CMTime)time { [self.currentItem seekToTime:time]; [self reef_fireTimeJump]; }
- (void)seekToTime:(CMTime)time completionHandler:(void (^)(BOOL))handler
{
    [self.currentItem seekToTime:time completionHandler:handler];
    [self reef_fireTimeJump];
}
- (void)seekToTime:(CMTime)time toleranceBefore:(CMTime)before toleranceAfter:(CMTime)after
{
    [self.currentItem seekToTime:time toleranceBefore:before toleranceAfter:after];
    [self reef_fireTimeJump];
}
- (void)seekToTime:(CMTime)time toleranceBefore:(CMTime)before toleranceAfter:(CMTime)after completionHandler:(void (^)(BOOL))handler
{
    [self.currentItem seekToTime:time toleranceBefore:before toleranceAfter:after completionHandler:handler];
    [self reef_fireTimeJump];
}
- (void)seekToDate:(NSDate *)date {}
- (void)seekToDate:(NSDate *)date completionHandler:(void (^)(BOOL))handler { if (handler) handler(NO); }

- (float)volume { return PlayerState(self)->volume; }
- (void)setVolume:(float)volume
{
    ReefPlayerState *state = PlayerState(self);
    Notify(self, @"volume", ^{ state->volume = volume; });
    ReefItemState *item = ItemState(state->item);
    if (item)
        [item->engine setVolume:volume muted:state->muted];
}
- (BOOL)isMuted { return PlayerState(self)->muted; }
- (void)setMuted:(BOOL)muted
{
    ReefPlayerState *state = PlayerState(self);
    Notify(self, @"muted", ^{ state->muted = muted; });
    ReefItemState *item = ItemState(state->item);
    if (item)
        [item->engine setVolume:state->volume muted:muted];
}
- (AVPlayerActionAtItemEnd)actionAtItemEnd { return PlayerState(self)->actionAtItemEnd; }
- (void)setActionAtItemEnd:(AVPlayerActionAtItemEnd)a { PlayerState(self)->actionAtItemEnd = a; }
- (BOOL)isExternalPlaybackActive { return NO; }
- (BOOL)allowsExternalPlayback { return NO; }
- (void)setAllowsExternalPlayback:(BOOL)b {}
- (BOOL)usesExternalPlaybackWhileExternalScreenIsActive { return NO; }
- (void)setUsesExternalPlaybackWhileExternalScreenIsActive:(BOOL)b {}
- (BOOL)isOutputObscuredDueToInsufficientExternalProtection { return NO; }
- (BOOL)preventsDisplaySleepDuringVideoPlayback { return PlayerState(self)->preventsSleep; }
- (void)setPreventsDisplaySleepDuringVideoPlayback:(BOOL)b { PlayerState(self)->preventsSleep = b; }
- (BOOL)appliesMediaSelectionCriteriaAutomatically { return PlayerState(self)->appliesCriteria; }
- (void)setAppliesMediaSelectionCriteriaAutomatically:(BOOL)b { PlayerState(self)->appliesCriteria = b; }
- (void)setMediaSelectionCriteria:(id)criteria forMediaCharacteristic:(NSString *)c {}
- (id)mediaSelectionCriteriaForMediaCharacteristic:(NSString *)c { return nil; }
- (void)setMultichannelAudioStrategy:(NSString *)s { PlayerState(self)->multichannelStrategy = s; }
- (NSString *)audioOutputDeviceUniqueID { return nil; }
- (void)setAudioOutputDeviceUniqueID:(NSString *)i {}
- (NSInteger)_externalProtectionStatus { return 0; }
- (void)_setDisplaysUsedForPlayback:(NSArray *)displays {}
- (id)sourceClock { return nil; }
- (void)setSourceClock:(id)c {}
- (id)masterClock { return nil; }
- (void)setMasterClock:(id)c {}
- (void)prerollAtRate:(float)rate completionHandler:(void (^)(BOOL))handler { if (handler) dispatch_async(dispatch_get_main_queue(), ^{ handler(YES); }); }
- (void)cancelPendingPrerolls {}

- (id)addPeriodicTimeObserverForInterval:(CMTime)interval queue:(dispatch_queue_t)queue usingBlock:(void (^)(CMTime))block
{
    ReefTimeObserver *o = [ReefTimeObserver new];
    o->interval = Seconds(interval);
    if (isnan(o->interval) || o->interval <= 0)
        o->interval = 1;
    o->queue = queue ?: dispatch_get_main_queue();
    o->periodic = [block copy];
    o->next = Seconds(self.currentTime) + o->interval;
    [PlayerState(self)->observers addObject:o];
    // the block is called once right away, as AVPlayer does when the observer is added
    CMTime now = self.currentTime;
    dispatch_async(o->queue, ^{ if (!o->removed) block(now); });
    return o;
}

- (id)addBoundaryTimeObserverForTimes:(NSArray<NSValue *> *)times queue:(dispatch_queue_t)queue usingBlock:(void (^)(void))block
{
    ReefTimeObserver *o = [ReefTimeObserver new];
    NSMutableArray *seconds = [NSMutableArray array];
    for (NSValue *v in times)
        [seconds addObject:@(Seconds(v.CMTimeValue))];
    o->times = seconds;
    o->queue = queue ?: dispatch_get_main_queue();
    o->boundary = [block copy];
    [PlayerState(self)->observers addObject:o];
    return o;
}

- (void)removeTimeObserver:(id)observer
{
    if ([observer isKindOfClass:[ReefTimeObserver class]]) {
        ((ReefTimeObserver *)observer)->removed = YES;
        [PlayerState(self)->observers removeObject:observer];
    }
}

// Periodic observers are told whenever the time jumps or the rate changes.
- (void)reef_fireTimeJump
{
    ReefPlayerState *state = PlayerState(self);
    CMTime now = self.currentTime;
    double t = Seconds(now);
    for (ReefTimeObserver *o in [state->observers copy]) {
        if (!o->periodic)
            continue;
        o->next = (floor(t / o->interval) + 1) * o->interval;
        void (^block)(CMTime) = o->periodic;
        dispatch_async(o->queue, ^{ if (!o->removed) block(now); });
    }
    state->lastTime = t;
}

// One tick of the display timer: the layers get the picture of now, the time observers fire, the end is noticed.
// NO when the player needs no more ticks.
- (BOOL)reef_tick
{
    ReefPlayerState *state = PlayerState(self);
    ReefItemState *item = ItemState(state->item);
    if (!item || !item->engine)
        return state->rate != 0;
    ReefPlaybackEngine *engine = item->engine;
    // the pictures are decoded at the size of the largest layer showing them (in pixels)
    double wantWidth = 0, wantHeight = 0;
    for (CALayer *layer in [state->layers allObjects]) {
        double scale = layer.contentsScale > 0 ? layer.contentsScale : 1;
        wantWidth = MAX(wantWidth, layer.bounds.size.width * scale);
        wantHeight = MAX(wantHeight, layer.bounds.size.height * scale);
    }
    if (engine->open)
        ReefMediaReaderSetPictureSize(engine->reader, (int)ceil(wantWidth), (int)ceil(wantHeight));
    BOOL ended = NO;
    ReefVideoFrame *frame = engine->open ? [engine takePicture:&ended] : NULL;
    static double statsStart;
    static int statsTicks, statsFrames;
    statsTicks++;
    if (frame)
        statsFrames++;
    double wall = Now();
    if (wall - statsStart >= 2) {
        if (state->rate != 0)
            ReefLog(@"display: %d pictures, %d ticks in %.1f s", statsFrames, statsTicks, wall - statsStart);
        statsStart = wall;
        statsTicks = statsFrames = 0;
    }
    if (frame) {
        CGImageRef image = CreateImage(frame); // the image owns the picture now
        for (CALayer *layer in [state->layers allObjects]) {
            [CATransaction begin];
            [CATransaction setDisableActions:YES];
            layer.contents = (__bridge id)image;
            [CATransaction commit];
            // a view that draws the layer's pictures itself (AVKit's player view) is told
            id delegate = layer.delegate;
            if ([delegate respondsToSelector:@selector(reef_playerLayerDidChangeContents:)])
                ((void (*)(id, SEL, id))objc_msgSend)(delegate, @selector(reef_playerLayerDidChangeContents:), layer);
            ReefLayerState *ls = LayerState(layer);
            if (!ls->ready)
                Notify(layer, @"readyForDisplay", ^{ ls->ready = YES; });
        }
        CGImageRelease(image);
    }
    double t = [engine currentTime];
    if (state->rate != 0) {
        for (ReefTimeObserver *o in [state->observers copy]) {
            if (o->periodic && t >= o->next) {
                o->next = (floor(t / o->interval) + 1) * o->interval;
                void (^block)(CMTime) = o->periodic;
                CMTime now = ReefTime(t);
                dispatch_async(o->queue, ^{ if (!o->removed) block(now); });
            } else if (o->boundary) {
                for (NSNumber *b in o->times) {
                    if (state->lastTime < b.doubleValue && b.doubleValue <= t) {
                        void (^block)(void) = o->boundary;
                        dispatch_async(o->queue, ^{ if (!o->removed) block(); });
                        break;
                    }
                }
            }
        }
    }
    state->lastTime = t;
    double forwardEnd = Seconds(item->forwardEnd);
    if (state->rate > 0 && !isnan(forwardEnd) && forwardEnd > 0 && t >= forwardEnd)
        ended = YES;
    if (ended && !item->reachedEnd) {
        item->reachedEnd = YES;
        ReefLog(@"item reached its end at %.3f (sound written: %.3f s)", t, engine->sampleRate ? (double)engine->written / engine->sampleRate : 0.0);
        AVPlayerItem *current = state->item;
        if (state->actionAtItemEnd == AVPlayerActionAtItemEndPause || state->actionAtItemEnd == AVPlayerActionAtItemEndNone) {
            [self willChangeValueForKey:@"timeControlStatus"];
            Notify(self, @"rate", ^{ state->rate = 0; });
            [engine setRate:0];
            [self didChangeValueForKey:@"timeControlStatus"];
            [self reef_fireTimeJump];
        }
        [[NSNotificationCenter defaultCenter] postNotificationName:AVPlayerItemDidPlayToEndTimeNotification object:current];
    }
    return state->rate != 0 || engine->wantsPicture || !engine->open;
}

- (void)reef_addLayer:(CALayer *)layer
{
    [PlayerState(self)->layers addObject:layer];
    ReefItemState *item = ItemState(self.currentItem);
    if (item && item->engine)
        item->engine->wantsPicture = YES;
    [ReefDisplayTimer addPlayer:self];
}

- (void)reef_removeLayer:(CALayer *)layer
{
    [PlayerState(self)->layers removeObject:layer];
}

@end

#pragma mark - AVPlayerLayer

static NSString *GravityToContents(NSString *gravity)
{
    if ([gravity isEqualToString:AVLayerVideoGravityResize])
        return kCAGravityResize;
    if ([gravity isEqualToString:AVLayerVideoGravityResizeAspectFill])
        return kCAGravityResizeAspectFill;
    return kCAGravityResizeAspect;
}

@implementation AVPlayerLayer (ReefPlayback)

+ (AVPlayerLayer *)playerLayerWithPlayer:(AVPlayer *)player
{
    AVPlayerLayer *layer = [self layer];
    layer.player = player;
    return layer;
}

- (AVPlayer *)player { return LayerState(self)->player; }

- (void)setPlayer:(AVPlayer *)player
{
    ReefLayerState *state = LayerState(self);
    if (state->player == player)
        return;
    [state->player reef_removeLayer:self];
    [self willChangeValueForKey:@"player"];
    state->player = player;
    [self didChangeValueForKey:@"player"];
    self.contentsGravity = GravityToContents(state->gravity);
    [player reef_addLayer:self];
}

- (NSString *)videoGravity { return LayerState(self)->gravity; }

- (void)setVideoGravity:(NSString *)gravity
{
    ReefLayerState *state = LayerState(self);
    state->gravity = [gravity copy] ?: AVLayerVideoGravityResizeAspect;
    self.contentsGravity = GravityToContents(state->gravity);
}

- (BOOL)isReadyForDisplay { return LayerState(self)->ready; }

- (CGRect)videoRect
{
    CGSize size = self.player.currentItem.presentationSize;
    CGRect bounds = self.bounds;
    if (size.width <= 0 || size.height <= 0)
        return CGRectZero;
    if ([LayerState(self)->gravity isEqualToString:AVLayerVideoGravityResize])
        return bounds;
    double scale = [LayerState(self)->gravity isEqualToString:AVLayerVideoGravityResizeAspectFill]
                       ? MAX(bounds.size.width / size.width, bounds.size.height / size.height)
                       : MIN(bounds.size.width / size.width, bounds.size.height / size.height);
    CGSize fit = CGSizeMake(size.width * scale, size.height * scale);
    return CGRectMake(CGRectGetMidX(bounds) - fit.width / 2, CGRectGetMidY(bounds) - fit.height / 2, fit.width, fit.height);
}

- (NSDictionary *)pixelBufferAttributes { return nil; }
- (void)setPixelBufferAttributes:(NSDictionary *)a {}

@end

#pragma mark - AVPlayerItemTrack

@implementation AVPlayerItemTrack (ReefPlayback)
- (CMPersistentTrackID)trackID { return [(AVAssetTrack *)[self valueForKey:@"assetTrack"] trackID]; }
- (float)currentVideoFrameRate { return [(AVAssetTrack *)[self valueForKey:@"assetTrack"] nominalFrameRate]; }
- (NSString *)videoFieldMode { return nil; }
- (void)setVideoFieldMode:(NSString *)mode {}
- (NSArray *)outputs { return @[]; }
- (void)addOutput:(id)output {}
- (AVPlayerItem *)_playerItem { return nil; }
@end

// Constants Reef's AVFoundation lacks (the values of macOS 26).
NSString *const AVAudioTimePitchAlgorithmSpectral = @"Spectral";
NSString *const AVAudioTimePitchAlgorithmTimeDomain = @"TimeDomain";
NSString *const AVAudioTimePitchAlgorithmVarispeed = @"Varispeed";
NSString *const AVPlayerItemFailedToPlayToEndTimeErrorKey = @"AVPlayerItemFailedToPlayToEndTimeErrorKey";
