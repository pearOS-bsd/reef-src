// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// The host's FFmpeg libraries for the media engine (see ReefMedia.h). The functions are looked up by name in the
// libraries of one FFmpeg release (the newest one the host has); the few fields of FFmpeg's structures that are read
// here are at the offsets of that release's ABI (measured with offsetof against each release's headers: FFmpeg 6.1 is
// libavformat 60, 7.1 is libavformat 61).

#include "ReefMedia.h"

#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern void *reef_host_symbol(const char *libraries, const char *name);

#define NOPTS ((int64_t)0x8000000000000000ULL)
#define ERROR_EOF (-541478725)
#define ERROR_AGAIN (-11)
#define TIME_BASE 1000000.0
#define PIX_FMT_BGRA 28
#define SAMPLE_FMT_FLT 3
#define SWS_BILINEAR 2
#define SEEK_BACKWARD 1

typedef struct { int num, den; } Rational;
typedef struct { char *key, *value; } DictEntry;
typedef struct { int order, nb_channels; uint64_t mask; void *opaque; } ChannelLayout; // 24 bytes in 6.1 and 7.1

// The fields of a release's structures this file reads (the others are at the same offsets in every supported release).
typedef struct Release {
    int major;
    const char *format, *codec, *util, *scale, *resample;
    int fcStart, fcDuration, fcBitRate, fcMetadata;
    int cpFormat, cpBitRate, cpWidth, cpHeight, cpAspect, cpSampleRate, cpLayout;
    int frSampleRate, frLayout;
} Release;

static const Release releases[] = {
    {61, "libavformat.so.61", "libavcodec.so.61", "libavutil.so.59", "libswscale.so.8", "libswresample.so.5",
     96, 104, 112, 192, 44, 48, 72, 76, 80, 152, 128, 192, 408},
    {60, "libavformat.so.60", "libavcodec.so.60", "libavutil.so.58", "libswscale.so.7", "libswresample.so.4",
     64, 72, 80, 176, 28, 32, 56, 60, 64, 116, 144, 208, 448},
};

// Offsets that are the same in both releases.
enum {
    FC_NB_STREAMS = 44, FC_STREAMS = 48,
    ST_INDEX = 8, ST_CODECPAR = 16, ST_TIME_BASE = 32, ST_START = 40, ST_DURATION = 48, ST_FRAMES = 56, ST_DISPOSITION = 64,
    ST_ASPECT = 72, ST_METADATA = 80, ST_AVG_RATE = 88,
    CP_TYPE = 0, CP_ID = 4,
    PK_PTS = 8, PK_DTS = 16, PK_STREAM = 36, PK_DURATION = 64,
    FR_DATA = 0, FR_LINESIZE = 64, FR_EXTENDED = 96, FR_WIDTH = 104, FR_HEIGHT = 108, FR_SAMPLES = 112, FR_FORMAT = 116,
    FR_PTS = 136, FR_PKT_DTS = 144,
};
#define AT(type, base, offset) (*(type *)((char *)(base) + (offset)))

static struct {
    const Release *release;
    unsigned (*avformat_version)(void);
    int (*avformat_open_input)(void **, const char *, const void *, void **);
    int (*avformat_find_stream_info)(void *, void **);
    void (*avformat_close_input)(void **);
    int (*av_read_frame)(void *, void *);
    int (*av_seek_frame)(void *, int, int64_t, int);
    int (*av_find_best_stream)(void *, int, int, int, const void **, int);
    const void *(*avcodec_find_decoder)(int);
    void *(*avcodec_alloc_context3)(const void *);
    int (*avcodec_parameters_to_context)(void *, const void *);
    int (*avcodec_open2)(void *, const void *, void **);
    int (*avcodec_send_packet)(void *, const void *);
    int (*avcodec_receive_frame)(void *, void *);
    void (*avcodec_flush_buffers)(void *);
    void (*avcodec_free_context)(void **);
    const char *(*avcodec_get_name)(int);
    void *(*av_packet_alloc)(void);
    void (*av_packet_free)(void **);
    void (*av_packet_unref)(void *);
    void *(*av_frame_alloc)(void);
    void (*av_frame_free)(void **);
    void (*av_frame_unref)(void *);
    int (*av_opt_set_int)(void *, const char *, int64_t, int);
    void (*av_channel_layout_default)(ChannelLayout *, int);
    void (*av_log_set_level)(int);
    DictEntry *(*av_dict_get)(const void *, const char *, const DictEntry *, int);
    void *(*sws_getContext)(int, int, int, int, int, int, int, void *, void *, const double *);
    int (*sws_scale)(void *, const uint8_t *const *, const int *, int, int, uint8_t *const *, const int *);
    void (*sws_freeContext)(void *);
    int (*swr_alloc_set_opts2)(void **, const ChannelLayout *, int, int, const ChannelLayout *, int, int, int, void *);
    int (*swr_init)(void *);
    int (*swr_convert)(void *, uint8_t **, int, const uint8_t **, int);
    void (*swr_free)(void **);
} ff;

static int bind(const Release *r)
{
#define GET(lib, name) if (!(*(void **)&ff.name = reef_host_symbol(r->lib, #name))) return 0;
    GET(format, avformat_version) GET(format, avformat_open_input) GET(format, avformat_find_stream_info)
    GET(format, avformat_close_input) GET(format, av_read_frame) GET(format, av_seek_frame) GET(format, av_find_best_stream)
    GET(codec, avcodec_find_decoder) GET(codec, avcodec_alloc_context3) GET(codec, avcodec_parameters_to_context)
    GET(codec, avcodec_open2) GET(codec, avcodec_send_packet) GET(codec, avcodec_receive_frame)
    GET(codec, avcodec_flush_buffers) GET(codec, avcodec_free_context) GET(codec, avcodec_get_name)
    GET(codec, av_packet_alloc) GET(codec, av_packet_free) GET(codec, av_packet_unref)
    GET(util, av_frame_alloc) GET(util, av_frame_free) GET(util, av_frame_unref) GET(util, av_opt_set_int)
    GET(util, av_channel_layout_default) GET(util, av_log_set_level) GET(util, av_dict_get)
    GET(scale, sws_getContext) GET(scale, sws_scale) GET(scale, sws_freeContext)
    GET(resample, swr_alloc_set_opts2) GET(resample, swr_init) GET(resample, swr_convert) GET(resample, swr_free)
#undef GET
    return (int)(ff.avformat_version() >> 16) == r->major;
}

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void load(void)
{
    for (size_t i = 0; i < sizeof releases / sizeof *releases; i++) {
        if (bind(&releases[i])) {
            ff.release = &releases[i];
            const char *level = getenv("REEF_MEDIA_LOG");
            ff.av_log_set_level(level ? atoi(level) : -8);
            return;
        }
    }
    memset(&ff, 0, sizeof ff);
}

int ReefMediaAvailable(void)
{
    pthread_once(&once, load);
    return ff.release != 0;
}

int ReefMediaHostPath(const char *path, char *out, size_t size)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        snprintf(out, size, "%s", path);
        return -1;
    }
    snprintf(out, size, "/proc/self/fd/%d", fd);
    return fd;
}

static double seconds(int64_t value, Rational base)
{
    return value == NOPTS || !base.den ? NAN : (double)value * base.num / base.den;
}

static uint32_t fourcc(const char *a)
{
    return (uint32_t)a[0] << 24 | (uint32_t)a[1] << 16 | (uint32_t)a[2] << 8 | (uint32_t)a[3];
}

// CoreMedia's codec types for FFmpeg's names.
static uint32_t codecType(const char *name)
{
    static const char *const table[][2] = {
        {"h264", "avc1"}, {"hevc", "hvc1"}, {"aac", "aac "}, {"mp3", ".mp3"}, {"mp3float", ".mp3"}, {"prores", "apcn"},
        {"vp9", "vp09"}, {"vp8", "vp08"}, {"av1", "av01"}, {"mpeg4", "mp4v"}, {"mjpeg", "jpeg"}, {"alac", "alac"},
        {"ac3", "ac-3"}, {"eac3", "ec-3"}, {"opus", "opus"}, {"flac", "flac"}, {"mpeg2video", "mp2v"}, {"png", "png "},
        {"mp2", ".mp2"}, {"amr_nb", "samr"}, {"vorbis", "vorb"}, {"dvvideo", "dvc "}, {"mov_text", "tx3g"},
    };
    if (!name)
        return 0;
    if (!strncmp(name, "pcm_", 4))
        return fourcc("lpcm");
    for (size_t i = 0; i < sizeof table / sizeof *table; i++)
        if (!strcmp(name, table[i][0]))
            return fourcc(table[i][1]);
    return 0;
}

static void copyTag(char *out, size_t size, const void *dictionary, const char *key)
{
    DictEntry *entry = dictionary ? ff.av_dict_get(dictionary, key, 0, 0) : 0;
    if (entry && entry->value)
        snprintf(out, size, "%s", entry->value);
}

static void describeStream(void *stream, ReefMediaStream *out, double fileDuration)
{
    const Release *r = ff.release;
    void *par = AT(void *, stream, ST_CODECPAR);
    Rational base = AT(Rational, stream, ST_TIME_BASE);
    memset(out, 0, sizeof *out);
    out->index = AT(int, stream, ST_INDEX);
    out->type = AT(int, par, CP_TYPE);
    const char *name = ff.avcodec_get_name(AT(int, par, CP_ID));
    snprintf(out->codecName, sizeof out->codecName, "%s", name ? name : "");
    out->codecType = codecType(name);
    out->width = AT(int, par, r->cpWidth);
    out->height = AT(int, par, r->cpHeight);
    Rational aspect = AT(Rational, stream, ST_ASPECT);
    if (!aspect.num || !aspect.den)
        aspect = AT(Rational, par, r->cpAspect);
    out->pixelAspect = aspect.num > 0 && aspect.den > 0 ? (double)aspect.num / aspect.den : 1;
    Rational rate = AT(Rational, stream, ST_AVG_RATE);
    out->frameRate = rate.num > 0 && rate.den > 0 ? (double)rate.num / rate.den : 0;
    out->sampleRate = AT(int, par, r->cpSampleRate);
    out->channels = AT(ChannelLayout, par, r->cpLayout).nb_channels;
    out->bitRate = AT(int64_t, par, r->cpBitRate);
    out->timeScale = base.den > 0 ? base.den : 600;
    double start = seconds(AT(int64_t, stream, ST_START), base);
    double duration = seconds(AT(int64_t, stream, ST_DURATION), base);
    out->start = isnan(start) ? 0 : start;
    out->duration = isnan(duration) || duration <= 0 ? fileDuration : duration;
    out->frames = AT(int64_t, stream, ST_FRAMES);
    out->attachedPicture = (AT(int, stream, ST_DISPOSITION) & 0x400) != 0;
    copyTag(out->language, sizeof out->language, AT(void *, stream, ST_METADATA), "language");
}

static double fileDuration(void *format)
{
    int64_t duration = AT(int64_t, format, ff.release->fcDuration);
    return duration == NOPTS || duration <= 0 ? 0 : duration / TIME_BASE;
}

static double fileStart(void *format)
{
    int64_t start = AT(int64_t, format, ff.release->fcStart);
    return start == NOPTS ? 0 : start / TIME_BASE;
}

int ReefMediaProbe(const char *path, ReefMediaInfo *info)
{
    memset(info, 0, sizeof *info);
    if (!ReefMediaAvailable())
        return -1;
    char host[64 + 1024];
    int fd = ReefMediaHostPath(path, host, sizeof host);
    void *format = 0;
    int error = ff.avformat_open_input(&format, host, 0, 0);
    if (error >= 0)
        error = ff.avformat_find_stream_info(format, 0);
    if (error < 0) {
        if (format)
            ff.avformat_close_input(&format);
        if (fd >= 0)
            close(fd);
        return error;
    }
    info->duration = fileDuration(format);
    info->start = fileStart(format);
    info->bitRate = AT(int64_t, format, ff.release->fcBitRate);
    const char *const *iformat = AT(const char *const *, format, 8); // AVInputFormat's first field is its name
    if (iformat && iformat[0])
        snprintf(info->formatName, sizeof info->formatName, "%s", iformat[0]);
    void *metadata = AT(void *, format, ff.release->fcMetadata);
    copyTag(info->title, sizeof info->title, metadata, "title");
    copyTag(info->artist, sizeof info->artist, metadata, "artist");
    copyTag(info->album, sizeof info->album, metadata, "album");
    copyTag(info->comment, sizeof info->comment, metadata, "comment");
    unsigned count = AT(unsigned, format, FC_NB_STREAMS);
    void **streams = AT(void **, format, FC_STREAMS);
    for (unsigned i = 0; i < count && info->streamCount < 16; i++) {
        ReefMediaStream *s = &info->streams[info->streamCount];
        describeStream(streams[i], s, info->duration);
        if (s->type == ReefMediaVideo || s->type == ReefMediaAudio || s->type == ReefMediaSubtitle)
            info->streamCount++;
        if (info->duration <= 0 && s->duration > 0)
            info->duration = s->duration;
    }
    ff.avformat_close_input(&format);
    if (fd >= 0)
        close(fd);
    return 0;
}

struct ReefMediaReader {
    void *format;
    int fd;
    int video, audio;           // stream numbers, -1 when absent
    void *videoCodec, *audioCodec;
    Rational videoBase, audioBase;
    void *packet, *frame;
    void *sws;
    int swsWidth, swsHeight, swsFormat;
    void *swr;
    int rate, channels;         // what the sound becomes
    int wantRate, wantChannels;
    double start;               // the file's start time, subtracted from every time
    double nextAudio;           // time after the last sound, for frames without a time
    double videoFrameDuration;
    int eof, videoDone, audioDone;
    int width, height;
    volatile int wantWidth, wantHeight; // the picture size asked for (0: coded size)
    int swsOutWidth, swsOutHeight;
};

static void *openDecoder(void *format, int index, Rational *base)
{
    void *stream = AT(void **, format, FC_STREAMS)[index];
    void *par = AT(void *, stream, ST_CODECPAR);
    const void *codec = ff.avcodec_find_decoder(AT(int, par, CP_ID));
    if (!codec)
        return 0;
    void *context = ff.avcodec_alloc_context3(codec);
    if (!context)
        return 0;
    if (ff.avcodec_parameters_to_context(context, par) < 0) {
        ff.avcodec_free_context(&context);
        return 0;
    }
    ff.av_opt_set_int(context, "threads", 0, 0);
    if (ff.avcodec_open2(context, codec, 0) < 0) {
        ff.avcodec_free_context(&context);
        return 0;
    }
    *base = AT(Rational, stream, ST_TIME_BASE);
    return context;
}

ReefMediaReader *ReefMediaReaderOpen(const char *path, int wantVideo, int wantAudio, int sampleRate, int channels)
{
    if (!ReefMediaAvailable())
        return 0;
    ReefMediaReader *r = calloc(1, sizeof *r);
    char host[64 + 1024];
    r->fd = ReefMediaHostPath(path, host, sizeof host);
    r->video = r->audio = -1;
    if (ff.avformat_open_input(&r->format, host, 0, 0) < 0 || ff.avformat_find_stream_info(r->format, 0) < 0) {
        ReefMediaReaderClose(r);
        return 0;
    }
    r->start = fileStart(r->format);
    if (wantVideo) {
        int index = ff.av_find_best_stream(r->format, ReefMediaVideo, -1, -1, 0, 0);
        if (index >= 0) {
            void *stream = AT(void **, r->format, FC_STREAMS)[index];
            if (!(AT(int, stream, ST_DISPOSITION) & 0x400) && (r->videoCodec = openDecoder(r->format, index, &r->videoBase))) {
                r->video = index;
                void *par = AT(void *, stream, ST_CODECPAR);
                r->width = AT(int, par, ff.release->cpWidth);
                r->height = AT(int, par, ff.release->cpHeight);
                Rational rate = AT(Rational, stream, ST_AVG_RATE);
                r->videoFrameDuration = rate.num > 0 && rate.den > 0 ? (double)rate.den / rate.num : 1 / 30.0;
            }
        }
    }
    if (wantAudio) {
        int index = ff.av_find_best_stream(r->format, ReefMediaAudio, -1, r->video, 0, 0);
        if (index >= 0 && (r->audioCodec = openDecoder(r->format, index, &r->audioBase))) {
            r->audio = index;
            void *par = AT(void *, AT(void **, r->format, FC_STREAMS)[index], ST_CODECPAR);
            int sourceChannels = AT(ChannelLayout, par, ff.release->cpLayout).nb_channels;
            r->rate = sampleRate > 0 ? sampleRate : AT(int, par, ff.release->cpSampleRate);
            r->channels = channels > 0 ? channels : (sourceChannels >= 2 ? 2 : 1);
            if (r->rate <= 0)
                r->rate = 44100;
        }
    }
    if (r->video < 0 && r->audio < 0) {
        ReefMediaReaderClose(r);
        return 0;
    }
    r->packet = ff.av_packet_alloc();
    r->frame = ff.av_frame_alloc();
    return r;
}

void ReefMediaReaderClose(ReefMediaReader *r)
{
    if (!r)
        return;
    if (r->videoCodec)
        ff.avcodec_free_context(&r->videoCodec);
    if (r->audioCodec)
        ff.avcodec_free_context(&r->audioCodec);
    if (r->packet)
        ff.av_packet_free(&r->packet);
    if (r->frame)
        ff.av_frame_free(&r->frame);
    if (r->sws)
        ff.sws_freeContext(r->sws);
    if (r->swr)
        ff.swr_free(&r->swr);
    if (r->format)
        ff.avformat_close_input(&r->format);
    if (r->fd >= 0)
        close(r->fd);
    free(r);
}

int ReefMediaReaderHasVideo(ReefMediaReader *r) { return r && r->video >= 0; }
int ReefMediaReaderHasAudio(ReefMediaReader *r) { return r && r->audio >= 0; }
int ReefMediaReaderSampleRate(ReefMediaReader *r) { return r ? r->rate : 0; }
int ReefMediaReaderChannels(ReefMediaReader *r) { return r ? r->channels : 0; }

void ReefMediaReaderSetPictureSize(ReefMediaReader *r, int width, int height)
{
    if (r) {
        r->wantWidth = width;
        r->wantHeight = height;
    }
}

void ReefMediaReaderVideoSize(ReefMediaReader *r, int *width, int *height)
{
    *width = r ? r->width : 0;
    *height = r ? r->height : 0;
}

static double frameTime(void *frame, Rational base)
{
    int64_t pts = AT(int64_t, frame, FR_PTS);
    if (pts == NOPTS)
        pts = AT(int64_t, frame, FR_PKT_DTS);
    return seconds(pts, base);
}

static ReefVideoFrame *convertPicture(ReefMediaReader *r)
{
    void *f = r->frame;
    int width = AT(int, f, FR_WIDTH), height = AT(int, f, FR_HEIGHT), format = AT(int, f, FR_FORMAT);
    if (width <= 0 || height <= 0)
        return 0;
    // scaled down to the size asked for (scaling up is left to whoever draws the picture)
    int outWidth = width, outHeight = height, wantWidth = r->wantWidth, wantHeight = r->wantHeight;
    if (wantWidth > 0 && wantHeight > 0 && wantWidth < width && wantHeight < height) {
        outWidth = wantWidth & ~1;
        outHeight = wantHeight & ~1;
    }
    if (!r->sws || width != r->swsWidth || height != r->swsHeight || format != r->swsFormat || outWidth != r->swsOutWidth ||
        outHeight != r->swsOutHeight) {
        if (r->sws)
            ff.sws_freeContext(r->sws);
        r->sws = ff.sws_getContext(width, height, format, outWidth, outHeight, PIX_FMT_BGRA, SWS_BILINEAR, 0, 0, 0);
        r->swsWidth = width, r->swsHeight = height, r->swsFormat = format;
        r->swsOutWidth = outWidth, r->swsOutHeight = outHeight;
        if (!r->sws)
            return 0;
    }
    ReefVideoFrame *out = calloc(1, sizeof *out);
    out->width = outWidth;
    out->height = outHeight;
    out->bytesPerRow = ((size_t)width * 4 + 63) & ~(size_t)63;
    out->pixels = malloc(out->bytesPerRow * outHeight);
    uint8_t *planes[4] = {out->pixels, 0, 0, 0};
    int strides[4] = {(int)out->bytesPerRow, 0, 0, 0};
    ff.sws_scale(r->sws, (const uint8_t *const *)&AT(uint8_t *, f, FR_DATA), &AT(int, f, FR_LINESIZE), 0, height, planes, strides);
    double t = frameTime(f, r->videoBase);
    out->time = isnan(t) ? 0 : t - r->start;
    out->duration = r->videoFrameDuration;
    return out;
}

static ReefAudioChunk *convertSound(ReefMediaReader *r)
{
    void *f = r->frame;
    int samples = AT(int, f, FR_SAMPLES), format = AT(int, f, FR_FORMAT);
    int inRate = AT(int, f, ff.release->frSampleRate);
    ChannelLayout *inLayout = &AT(ChannelLayout, f, ff.release->frLayout);
    if (samples <= 0 || inRate <= 0)
        return 0;
    if (!r->swr) {
        ChannelLayout outLayout;
        memset(&outLayout, 0, sizeof outLayout);
        ff.av_channel_layout_default(&outLayout, r->channels);
        if (ff.swr_alloc_set_opts2(&r->swr, &outLayout, SAMPLE_FMT_FLT, r->rate, inLayout, format, inRate, 0, 0) < 0 ||
            ff.swr_init(r->swr) < 0) {
            if (r->swr)
                ff.swr_free(&r->swr);
            return 0;
        }
    }
    int capacity = (int)((int64_t)samples * r->rate / inRate) + 256;
    ReefAudioChunk *out = calloc(1, sizeof *out);
    out->channels = r->channels;
    out->samples = malloc((size_t)capacity * r->channels * sizeof(float));
    uint8_t *planes[1] = {(uint8_t *)out->samples};
    int made = ff.swr_convert(r->swr, planes, capacity, (const uint8_t **)AT(uint8_t **, f, FR_EXTENDED), samples);
    if (made <= 0) {
        ReefAudioChunkFree(out);
        return 0;
    }
    out->frames = made;
    double t = frameTime(f, r->audioBase);
    out->time = isnan(t) ? r->nextAudio : t - r->start;
    r->nextAudio = out->time + (double)made / r->rate;
    return out;
}

int ReefMediaReaderNext(ReefMediaReader *r, ReefVideoFrame **frame, ReefAudioChunk **chunk)
{
    *frame = 0;
    *chunk = 0;
    for (;;) {
        if (r->videoCodec && !r->videoDone) {
            int error = ff.avcodec_receive_frame(r->videoCodec, r->frame);
            if (error == 0) {
                *frame = convertPicture(r);
                ff.av_frame_unref(r->frame);
                if (*frame)
                    return 1;
                continue;
            }
            if (error == ERROR_EOF)
                r->videoDone = 1;
        }
        if (r->audioCodec && !r->audioDone) {
            int error = ff.avcodec_receive_frame(r->audioCodec, r->frame);
            if (error == 0) {
                *chunk = convertSound(r);
                ff.av_frame_unref(r->frame);
                if (*chunk)
                    return 2;
                continue;
            }
            if (error == ERROR_EOF)
                r->audioDone = 1;
        }
        if (r->eof) {
            if ((!r->videoCodec || r->videoDone) && (!r->audioCodec || r->audioDone))
                return 0;
            continue;
        }
        int error = ff.av_read_frame(r->format, r->packet);
        if (error < 0) {
            // the end (or an unreadable rest): the decoders give out what they hold
            r->eof = 1;
            if (r->videoCodec)
                ff.avcodec_send_packet(r->videoCodec, 0);
            if (r->audioCodec)
                ff.avcodec_send_packet(r->audioCodec, 0);
            continue;
        }
        int index = AT(int, r->packet, PK_STREAM);
        void *codec = index == r->video ? r->videoCodec : index == r->audio ? r->audioCodec : 0;
        if (codec) {
            int sent = ff.avcodec_send_packet(codec, r->packet);
            if (sent == ERROR_AGAIN) {
                // the decoder is full: take its frames first, then give it this packet again
                for (;;) {
                    int got = ff.avcodec_receive_frame(codec, r->frame);
                    if (got != 0)
                        break;
                    if (codec == r->videoCodec) {
                        ReefVideoFrame *v = convertPicture(r);
                        ff.av_frame_unref(r->frame);
                        if (v && !*frame) *frame = v; else ReefVideoFrameFree(v);
                    } else {
                        ReefAudioChunk *a = convertSound(r);
                        ff.av_frame_unref(r->frame);
                        if (a && !*chunk) *chunk = a; else ReefAudioChunkFree(a);
                    }
                }
                ff.avcodec_send_packet(codec, r->packet);
                ff.av_packet_unref(r->packet);
                if (*frame) {
                    if (*chunk) { ReefAudioChunkFree(*chunk); *chunk = 0; }
                    return 1;
                }
                if (*chunk)
                    return 2;
                continue;
            }
        }
        ff.av_packet_unref(r->packet);
    }
}

int ReefMediaReaderSeek(ReefMediaReader *r, double t)
{
    if (!r)
        return -1;
    int64_t target = (int64_t)((t + r->start) * TIME_BASE);
    int error = ff.av_seek_frame(r->format, -1, target, SEEK_BACKWARD);
    if (error < 0)
        error = ff.av_seek_frame(r->format, -1, target, 0);
    if (r->videoCodec)
        ff.avcodec_flush_buffers(r->videoCodec);
    if (r->audioCodec)
        ff.avcodec_flush_buffers(r->audioCodec);
    if (r->swr)
        ff.swr_free(&r->swr);
    r->eof = r->videoDone = r->audioDone = 0;
    r->nextAudio = t;
    return error < 0 ? error : 0;
}

void ReefVideoFrameFree(ReefVideoFrame *frame)
{
    if (frame) {
        free(frame->pixels);
        free(frame);
    }
}

void ReefAudioChunkFree(ReefAudioChunk *chunk)
{
    if (chunk) {
        free(chunk->samples);
        free(chunk);
    }
}
