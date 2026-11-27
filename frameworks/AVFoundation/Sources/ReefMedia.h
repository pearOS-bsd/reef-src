// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// The media engine under AVFoundation's playback classes: containers are read and decoded by the host's FFmpeg libraries
// (reached through reef_host_symbol, so nothing of FFmpeg is linked or shipped), the pictures come out as 32-bit BGRA and
// the sound as interleaved 32-bit float.

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { ReefMediaVideo = 0, ReefMediaAudio = 1, ReefMediaSubtitle = 3 };

typedef struct ReefMediaStream {
    int index;              // the stream's number in the container
    int type;               // ReefMediaVideo, ReefMediaAudio, ...
    uint32_t codecType;     // four characters as CoreMedia names codecs: 'avc1', 'hvc1', 'aac ', ...
    char codecName[32];     // FFmpeg's short name ("h264", "aac")
    int width, height;      // coded size of a video stream
    double pixelAspect;     // width of a pixel over its height (1 when unknown)
    int rotation;           // degrees, clockwise, from the display matrix (0 when none)
    double frameRate;       // average frames per second
    int sampleRate, channels;
    int64_t bitRate;        // bits per second, 0 when unknown
    int timeScale;          // the denominator of the stream's time base
    double start, duration; // seconds
    int64_t frames;         // number of frames when the container says it
    int attachedPicture;    // a cover image, not a moving picture
    char language[8];
} ReefMediaStream;

typedef struct ReefMediaInfo {
    double duration;        // seconds
    double start;
    int64_t bitRate;
    char formatName[32];    // "mov,mp4,m4a,3gp,3g2,mj2", "matroska,webm", ...
    int streamCount;
    ReefMediaStream streams[16];
    char title[256], artist[256], album[256], comment[256];
} ReefMediaInfo;

// The host libraries are there (tried once).
int ReefMediaAvailable(void);
// Reads the container's description; 0 on success, otherwise a negative FFmpeg error.
int ReefMediaProbe(const char *path, ReefMediaInfo *info);

typedef struct ReefVideoFrame {
    double time;            // presentation time, seconds
    double duration;
    int width, height;
    size_t bytesPerRow;
    uint8_t *pixels;        // BGRA, premultiplied, opaque
} ReefVideoFrame;

typedef struct ReefAudioChunk {
    double time;
    int frames;             // sample frames
    int channels;
    float *samples;         // interleaved
} ReefAudioChunk;

typedef struct ReefMediaReader ReefMediaReader;

// Opens the decoders of the best video and audio streams (either may be absent). The sound is converted to `channels`
// interleaved channels at `sampleRate` (0: the stream's own rate and up to two channels).
ReefMediaReader *ReefMediaReaderOpen(const char *path, int wantVideo, int wantAudio, int sampleRate, int channels);
void ReefMediaReaderClose(ReefMediaReader *reader);
int ReefMediaReaderHasVideo(ReefMediaReader *reader);
int ReefMediaReaderHasAudio(ReefMediaReader *reader);
int ReefMediaReaderSampleRate(ReefMediaReader *reader);
int ReefMediaReaderChannels(ReefMediaReader *reader);
void ReefMediaReaderVideoSize(ReefMediaReader *reader, int *width, int *height);
// Decodes until something comes out: 1 a picture in *frame, 2 sound in *chunk, 0 the end, negative an error.
// The caller frees what it gets with ReefVideoFrameFree / ReefAudioChunkFree.
int ReefMediaReaderNext(ReefMediaReader *reader, ReefVideoFrame **frame, ReefAudioChunk **chunk);
// Goes to the key frame at or before `seconds`; what comes out next starts there (the caller drops what is early).
int ReefMediaReaderSeek(ReefMediaReader *reader, double seconds);
void ReefVideoFrameFree(ReefVideoFrame *frame);
void ReefAudioChunkFree(ReefAudioChunk *chunk);

// The host path of a file the program names (Reef maps some folders): the file is opened here and the host reads it
// through /proc/self/fd. `out` gets the path to give to the reader; the returned descriptor is closed by the caller (-1
// when the path is used as it is).
int ReefMediaHostPath(const char *path, char *out, size_t size);

#ifdef __cplusplus
}
#endif
