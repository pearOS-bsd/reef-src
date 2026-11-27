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
        if (dladdr((const void *)AudioDeviceStop, &info)) image = info.dli_fbase;
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
    OSStatus r = getData(o, a, qs, q, size, data);
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
    if (r && a && size && streamFormat(o, a, &format)) {
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

OSStatus AudioDeviceCreateIOProcID(AudioObjectID device, AudioDeviceIOProc proc, void *data, AudioDeviceIOProcID *id) {
    static OSStatus (*f)(AudioObjectID, AudioDeviceIOProc, void *, AudioDeviceIOProcID *);
    if (!f) f = original("AudioDeviceCreateIOProcID");
    OSStatus r = f(device, proc, data, id);
    if (logging()) fprintf(stderr, "hal: create IO proc on %u -> %d\n", device, (int)r);
    return r;
}

OSStatus AudioDeviceStart(AudioObjectID device, AudioDeviceIOProcID id) {
    static OSStatus (*f)(AudioObjectID, AudioDeviceIOProcID);
    if (!f) f = original("AudioDeviceStart");
    OSStatus r = f(device, id);
    if (logging()) fprintf(stderr, "hal: start %u -> %d\n", device, (int)r);
    return r;
}
