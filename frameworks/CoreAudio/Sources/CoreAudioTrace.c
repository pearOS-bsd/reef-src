// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Properties of the audio hardware that Reef's CoreAudio does not answer: the stream formats of a device
// (kAudioDevicePropertyStreamFormats, from its stream configuration and sample rate: 32-bit float, packed, as the
// HAL's virtual format), and names and identifiers asked in a scope the device has no streams in (they are global).
// Debug aid: REEF_LOG_HAL=1 prints every property call (object, selector, scope, element, result).
#include <CoreAudio/CoreAudio.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Reef's CoreAudio, which this library re-exports: the image of a function it does not override
static void *original(const char *name) {
    static void *image;
    if (!image) {
        Dl_info info;
        if (dladdr((const void *)AudioGetCurrentHostTime, &info)) image = info.dli_fbase;
    }
    return image ? dlsym(image, name) : NULL;
}

static int logging(void) {
    static int on = -1;
    if (on < 0) on = getenv("REEF_LOG_HAL") != NULL;
    return on;
}

static const char *code(UInt32 c, char *buf) {
    for (int i = 0; i < 4; i++) {
        char ch = (char)(c >> (24 - 8 * i));
        buf[i] = ch >= 32 && ch < 127 ? ch : '?';
    }
    buf[4] = 0;
    return buf;
}

static void show(const char *call, AudioObjectID object, const AudioObjectPropertyAddress *a, OSStatus status, UInt32 size, const void *data) {
    if (!logging()) return;
    char s[5], c[5];
    fprintf(stderr, "hal: %s %u '%s' '%s' %u -> %d", call, object, a ? code(a->mSelector, s) : "", a ? code(a->mScope, c) : "", a ? a->mElement : 0, (int)status);
    if (!status && data && size == 4) fprintf(stderr, " = %u", *(const UInt32 *)data);
    if (!status && data && size == 8) fprintf(stderr, " = %g", *(const Float64 *)data);
    fprintf(stderr, " (%u bytes)\n", size);
}

static OSStatus (*getData)(AudioObjectID, const AudioObjectPropertyAddress *, UInt32, const void *, UInt32 *, void *);
static OSStatus (*getSize)(AudioObjectID, const AudioObjectPropertyAddress *, UInt32, const void *, UInt32 *);

static void load(void) {
    if (!getData) getData = original("AudioObjectGetPropertyData");
    if (!getSize) getSize = original("AudioObjectGetPropertyDataSize");
}

// The channels of the device in that scope, 0 if it has none
static UInt32 channels(AudioObjectID o, AudioObjectPropertyScope scope) {
    load();
    AudioObjectPropertyAddress a = {kAudioDevicePropertyStreamConfiguration, scope, kAudioObjectPropertyElementMain};
    UInt32 size = 0;
    if (getSize(o, &a, 0, NULL, &size) || size < sizeof(AudioBufferList)) return 0;
    AudioBufferList *list = malloc(size);
    UInt32 n = 0;
    if (!getData(o, &a, 0, NULL, &size, list))
        for (UInt32 i = 0; i < list->mNumberBuffers; i++) n += list->mBuffers[i].mNumberChannels;
    free(list);
    return n;
}

UInt32 ReefDeviceChannels(AudioObjectID o, AudioObjectPropertyScope scope) { return channels(o, scope); }
Float64 ReefDeviceRate(AudioObjectID o) {
    load();
    Float64 rate = 48000;
    AudioObjectPropertyAddress r = {kAudioDevicePropertyNominalSampleRate, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
    UInt32 size = sizeof rate;
    getData(o, &r, 0, NULL, &size, &rate);
    return rate;
}
extern UInt32 ReefDeviceFrameSize(void);
extern void ReefSetDeviceFrameSize(UInt32);
extern int ReefDeviceIsRunning(AudioObjectID);

// Buffer sizes: the frame size is ours (the IO runs from our queue), from 32 to 4096 frames
static int bufferAnswer(AudioObjectID o, const AudioObjectPropertyAddress *a, UInt32 *size, void *data) {
    UInt32 n = channels(o, kAudioObjectPropertyScopeOutput) + channels(o, kAudioObjectPropertyScopeInput);
    if (!n) return 0;
    switch (a->mSelector) {
    case kAudioDevicePropertyBufferFrameSize:
        if (*size < 4) return 0;
        *(UInt32 *)data = ReefDeviceFrameSize(); *size = 4; return 1;
    case kAudioDevicePropertyBufferFrameSizeRange:
        if (*size < sizeof(AudioValueRange)) return 0;
        *(AudioValueRange *)data = (AudioValueRange){32, 4096}; *size = sizeof(AudioValueRange); return 1;
    case kAudioDevicePropertyBufferSize: {
        if (*size < 4) return 0;
        UInt32 c = channels(o, a->mScope);
        *(UInt32 *)data = ReefDeviceFrameSize() * 4 * (c ? c : 2); *size = 4; return 1;
    }
    case kAudioDevicePropertyBufferSizeRange: {
        if (*size < sizeof(AudioValueRange)) return 0;
        UInt32 c = channels(o, a->mScope); c = c ? c : 2;
        *(AudioValueRange *)data = (AudioValueRange){32.0 * 4 * c, 4096.0 * 4 * c}; *size = sizeof(AudioValueRange); return 1;
    }
    case kAudioDevicePropertyDeviceIsRunning:
        if (*size < 4) return 0;
        *(UInt32 *)data = (UInt32)ReefDeviceIsRunning(o); *size = 4; return 1;
    }
    return 0;
}

static int streamFormat(AudioObjectID o, const AudioObjectPropertyAddress *a, AudioStreamBasicDescription *out) {
    if (a->mSelector != kAudioDevicePropertyStreamFormats && a->mSelector != kAudioDevicePropertyStreamFormat) return 0;
    UInt32 n = channels(o, a->mScope);
    if (!n) return 0;
    Float64 rate = 48000;
    AudioObjectPropertyAddress r = {kAudioDevicePropertyNominalSampleRate, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
    UInt32 size = sizeof rate;
    getData(o, &r, 0, NULL, &size, &rate);
    *out = (AudioStreamBasicDescription){rate, kAudioFormatLinearPCM, kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked, 4 * n, 1, 4 * n, n, 32, 0};
    return 1;
}

static int globalNamed(const AudioObjectPropertyAddress *a) {
    switch (a->mSelector) {
    case kAudioObjectPropertyName: case kAudioObjectPropertyManufacturer: case kAudioDevicePropertyDeviceUID:
    case kAudioDevicePropertyModelUID: case kAudioDevicePropertyTransportType:
        return a->mScope != kAudioObjectPropertyScopeGlobal;
    }
    return 0;
}

OSStatus AudioObjectGetPropertyData(AudioObjectID o, const AudioObjectPropertyAddress *a, UInt32 qs, const void *q, UInt32 *size, void *data) {
    load();
    OSStatus r;
    if (a && size && data && bufferAnswer(o, a, size, data)) {
        show("get", o, a, 0, *size, data);
        return 0;
    }
    r = getData(o, a, qs, q, size, data);
    AudioStreamBasicDescription format;
    if (r && a && size && *size >= sizeof format && streamFormat(o, a, &format)) {
        memcpy(data, &format, sizeof format);
        *size = sizeof format;
        r = 0;
    } else if (r && a && globalNamed(a)) {
        AudioObjectPropertyAddress g = *a;
        g.mScope = kAudioObjectPropertyScopeGlobal;
        r = getData(o, &g, qs, q, size, data);
    }
    show("get", o, a, r, size ? *size : 0, data);
    return r;
}

OSStatus AudioObjectGetPropertyDataSize(AudioObjectID o, const AudioObjectPropertyAddress *a, UInt32 qs, const void *q, UInt32 *size) {
    load();
    OSStatus r = getSize(o, a, qs, q, size);
    AudioStreamBasicDescription format;
    if (a && size && (a->mSelector == kAudioDevicePropertyBufferFrameSizeRange || a->mSelector == kAudioDevicePropertyBufferSizeRange)) {
        *size = sizeof(AudioValueRange);
        r = 0;
    } else if (a && size && (a->mSelector == kAudioDevicePropertyBufferFrameSize || a->mSelector == kAudioDevicePropertyBufferSize ||
                             a->mSelector == kAudioDevicePropertyDeviceIsRunning) && r) {
        *size = 4;
        r = 0;
    } else if (r && a && size && streamFormat(o, a, &format)) {
        *size = sizeof format;
        r = 0;
    } else if (r && a && globalNamed(a)) {
        AudioObjectPropertyAddress g = *a;
        g.mScope = kAudioObjectPropertyScopeGlobal;
        r = getSize(o, &g, qs, q, size);
    }
    show("size", o, a, r, size ? *size : 0, NULL);
    return r;
}

OSStatus AudioObjectSetPropertyData(AudioObjectID o, const AudioObjectPropertyAddress *a, UInt32 qs, const void *q, UInt32 size, const void *data) {
    static OSStatus (*f)(AudioObjectID, const AudioObjectPropertyAddress *, UInt32, const void *, UInt32, const void *);
    if (!f) f = original("AudioObjectSetPropertyData");
    if (a && a->mSelector == kAudioDevicePropertyBufferFrameSize && size >= 4 && data) {
        ReefSetDeviceFrameSize(*(const UInt32 *)data);
        show("set", o, a, 0, size, data);
        return 0;
    }
    OSStatus r = f(o, a, qs, q, size, data);
    show("set", o, a, r, size, data);
    return r;
}

Boolean AudioObjectHasProperty(AudioObjectID o, const AudioObjectPropertyAddress *a) {
    static Boolean (*f)(AudioObjectID, const AudioObjectPropertyAddress *);
    if (!f) f = original("AudioObjectHasProperty");
    Boolean r = f(o, a);
    show("has", o, a, r ? 0 : -1, 0, NULL);
    return r;
}
