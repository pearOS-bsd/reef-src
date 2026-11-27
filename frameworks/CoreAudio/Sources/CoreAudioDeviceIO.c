// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Device IO: Reef's CoreAudio keeps IO procs but never starts a device (AudioDeviceStart answers
// kAudioHardwareNotRunningError). Here the IO procs of the started devices run from an output AudioQueue of Reef's
// AudioToolbox (which plays through the AppKit host's audio output): every queue buffer, the procs of input devices
// get silence, the procs of output devices fill the buffer, in the device's format (32-bit float, interleaved, the
// nominal rate and the channels of its stream configuration). The buffer frame size can be set.
#include <CoreAudio/CoreAudio.h>
#include <dlfcn.h>
#include <mach/mach_time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct OpaqueAudioQueue *AudioQueueRef;
typedef struct AudioQueueBuffer {
    const UInt32 mAudioDataBytesCapacity;
    void *const mAudioData;
    UInt32 mAudioDataByteSize;
    void *mUserData;
    const UInt32 mPacketDescriptionCapacity;
    void *const mPacketDescriptions;
    UInt32 mPacketDescriptionCount;
} AudioQueueBuffer, *AudioQueueBufferRef;
typedef void (*AudioQueueOutputCallback)(void *, AudioQueueRef, AudioQueueBufferRef);

struct proc {
    AudioObjectID device;
    AudioDeviceIOProc function;
    void *data;
    int started;
    int input;   // the device has input streams and no output streams
    struct proc *next;
};

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static struct proc *procs;
static AudioQueueRef queue;
static UInt32 frameSize = 512;
static UInt32 queueChannels = 2;
static Float64 queueRate = 48000;
static Float64 sampleTime;
static float *inputSilence;
static size_t inputSilenceSize;

extern UInt32 ReefDeviceChannels(AudioObjectID device, AudioObjectPropertyScope scope);
extern Float64 ReefDeviceRate(AudioObjectID device);

static void *toolbox(const char *name) {
    static void *image;
    if (!image) image = dlopen("/System/Library/Frameworks/AudioToolbox.framework/Versions/A/AudioToolbox", RTLD_LAZY);
    return image ? dlsym(image, name) : NULL;
}

UInt32 ReefDeviceFrameSize(void) { return frameSize; }
void ReefSetDeviceFrameSize(UInt32 frames) { if (frames >= 16 && frames <= 8192) frameSize = frames; }

int ReefDeviceIsRunning(AudioObjectID device) {
    int running = 0;
    pthread_mutex_lock(&lock);
    for (struct proc *p = procs; p; p = p->next) running |= p->device == device && p->started;
    pthread_mutex_unlock(&lock);
    return running;
}

static void render(void *user, AudioQueueRef q, AudioQueueBufferRef buffer) {
    const UInt32 frames = buffer->mAudioDataBytesCapacity / (4 * queueChannels);
    memset(buffer->mAudioData, 0, buffer->mAudioDataBytesCapacity);
    AudioTimeStamp now = {0};
    now.mSampleTime = sampleTime;
    now.mHostTime = mach_absolute_time();
    now.mRateScalar = 1;
    now.mFlags = kAudioTimeStampSampleTimeValid | kAudioTimeStampHostTimeValid | kAudioTimeStampRateScalarValid;
    AudioTimeStamp inputTime = now, outputTime = now;
    outputTime.mSampleTime += frames;
    outputTime.mHostTime += (UInt64)(frames / queueRate * 1e9);   // mach ticks are nanoseconds on Reef
    pthread_mutex_lock(&lock);
    for (int pass = 0; pass < 2; pass++) {
        for (struct proc *p = procs; p; p = p->next) {
            if (!p->started || p->input != (pass == 0)) continue;
            AudioBufferList list = {1, {{queueChannels, frames * 4 * queueChannels, NULL}}};
            if (p->input) {
                UInt32 channels = ReefDeviceChannels(p->device, kAudioObjectPropertyScopeInput);
                size_t size = (size_t)frames * 4 * (channels ? channels : 1);
                if (size > inputSilenceSize) { free(inputSilence); inputSilence = calloc(1, size); inputSilenceSize = size; }
                else memset(inputSilence, 0, size);
                AudioBufferList in = {1, {{channels ? channels : 1, (UInt32)size, inputSilence}}};
                p->function(p->device, &now, &in, &inputTime, NULL, &outputTime, p->data);
            } else {
                list.mBuffers[0].mData = buffer->mAudioData;
                p->function(p->device, &now, NULL, &inputTime, &list, &outputTime, p->data);
            }
        }
    }
    pthread_mutex_unlock(&lock);
    sampleTime += frames;
    static unsigned long renders;
    if (getenv("REEF_LOG_HAL") && (renders++ % 500) == 0) {
        int n = 0;
        pthread_mutex_lock(&lock);
        for (struct proc *p = procs; p; p = p->next) n += p->started;
        pthread_mutex_unlock(&lock);
        fprintf(stderr, "hal: render %lu (%u frames, %d procs running)\n", renders, frames, n);
    }
    buffer->mAudioDataByteSize = frames * 4 * queueChannels;
    OSStatus (*enqueue)(AudioQueueRef, AudioQueueBufferRef, UInt32, const void *) = toolbox("AudioQueueEnqueueBuffer");
    if (enqueue) enqueue(q, buffer, 0, NULL);
}

static OSStatus startQueue(AudioObjectID output) {
    if (queue) return noErr;
    OSStatus (*newOutput)(const AudioStreamBasicDescription *, AudioQueueOutputCallback, void *, CFRunLoopRef, CFStringRef, UInt32, AudioQueueRef *) =
        toolbox("AudioQueueNewOutput");
    OSStatus (*allocate)(AudioQueueRef, UInt32, AudioQueueBufferRef *) = toolbox("AudioQueueAllocateBuffer");
    OSStatus (*start)(AudioQueueRef, const AudioTimeStamp *) = toolbox("AudioQueueStart");
    if (!newOutput || !allocate || !start) return kAudioHardwareNotRunningError;
    UInt32 channels = output ? ReefDeviceChannels(output, kAudioObjectPropertyScopeOutput) : 2;
    queueChannels = channels ? channels : 2;
    queueRate = output ? ReefDeviceRate(output) : 48000;
    AudioStreamBasicDescription format = {queueRate, kAudioFormatLinearPCM, kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked,
                                          4 * queueChannels, 1, 4 * queueChannels, queueChannels, 32, 0};
    AudioQueueRef q = NULL;
    OSStatus r = newOutput(&format, render, NULL, NULL, NULL, 0, &q);
    if (r) return r;
    for (int i = 0; i < 3; i++) {
        AudioQueueBufferRef b = NULL;
        if (allocate(q, frameSize * 4 * queueChannels, &b) == noErr) render(NULL, q, b);
    }
    queue = q;
    return start(q, NULL);
}

static int hasOutput(AudioObjectID device) { return ReefDeviceChannels(device, kAudioObjectPropertyScopeOutput) > 0; }

OSStatus AudioDeviceCreateIOProcID(AudioObjectID device, AudioDeviceIOProc function, void *data, AudioDeviceIOProcID *id) {
    if (!function || !id) return kAudioHardwareIllegalOperationError;
    struct proc *p = calloc(1, sizeof *p);
    p->device = device;
    p->function = function;
    p->data = data;
    p->input = !hasOutput(device);
    pthread_mutex_lock(&lock);
    p->next = procs;
    procs = p;
    pthread_mutex_unlock(&lock);
    *id = (AudioDeviceIOProcID)p;
    return noErr;
}

OSStatus AudioDeviceDestroyIOProcID(AudioObjectID device, AudioDeviceIOProcID id) {
    pthread_mutex_lock(&lock);
    for (struct proc **p = &procs; *p; p = &(*p)->next) {
        if (*p == (struct proc *)id) {
            struct proc *gone = *p;
            *p = gone->next;
            free(gone);
            break;
        }
    }
    pthread_mutex_unlock(&lock);
    return noErr;
}

OSStatus AudioDeviceStart(AudioObjectID device, AudioDeviceIOProcID id) {
    AudioObjectID output = 0;
    pthread_mutex_lock(&lock);
    for (struct proc *p = procs; p; p = p->next) {
        if (p->device == device && (!id || p == (struct proc *)id)) p->started = 1;
        if (p->started && !p->input) output = p->device;
    }
    pthread_mutex_unlock(&lock);
    if (!output) {
        AudioObjectPropertyAddress a = {kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
        UInt32 size = sizeof output;
        AudioObjectGetPropertyData(kAudioObjectSystemObject, &a, 0, NULL, &size, &output);
    }
    OSStatus r = startQueue(output);
    if (getenv("REEF_LOG_HAL")) fprintf(stderr, "hal: start %u -> %d\n", device, (int)r);
    return r;
}

OSStatus AudioDeviceStop(AudioObjectID device, AudioDeviceIOProcID id) {
    pthread_mutex_lock(&lock);
    for (struct proc *p = procs; p; p = p->next)
        if (p->device == device && (!id || p == (struct proc *)id)) p->started = 0;
    pthread_mutex_unlock(&lock);
    return noErr;
}
