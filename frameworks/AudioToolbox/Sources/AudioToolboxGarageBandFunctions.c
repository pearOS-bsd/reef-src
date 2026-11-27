// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// AudioToolbox functions GarageBand imports and Reef's AudioToolbox lacks.
#include <AudioToolbox/AudioUnit.h>
#include <AudioToolbox/MusicDevice.h>
#include <dispatch/dispatch.h>
#include <stdlib.h>

// Parameter and property event listeners: listeners exist, parameter changes go to the unit at once.
typedef struct { void *proc; void *userData; dispatch_queue_t queue; void *block; } Listener;

OSStatus AUEventListenerCreate(void *proc, void *userData, CFRunLoopRef runLoop, CFStringRef mode, Float32 interval, Float32 granularity, void **out) {
    Listener *l = calloc(1, sizeof *l);
    l->proc = proc;
    l->userData = userData;
    *out = l;
    return noErr;
}
OSStatus AUEventListenerCreateWithDispatchQueue(void **out, Float32 interval, Float32 granularity, dispatch_queue_t queue, void *block) {
    Listener *l = calloc(1, sizeof *l);
    l->queue = queue;
    *out = l;
    return noErr;
}
OSStatus AUEventListenerAddEventType(void *listener, void *object, const void *event) { return noErr; }
OSStatus AUEventListenerRemoveEventType(void *listener, void *object, const void *event) { return noErr; }
OSStatus AUListenerDispose(void *listener) { free(listener); return noErr; }
OSStatus AUParameterListenerNotify(void *listener, void *object, const AudioUnitParameter *parameter) { return noErr; }
OSStatus AUParameterSet(void *listener, void *object, const AudioUnitParameter *p, AudioUnitParameterValue value, UInt32 offset) {
    return AudioUnitSetParameter(p->mAudioUnit, p->mParameterID, p->mScope, p->mElement, value, offset);
}

// Registration of components inside the application, and their configuration: not supported yet.
AudioComponent AudioComponentRegister(const AudioComponentDescription *description, CFStringRef name, UInt32 version, AudioComponentFactoryFunction factory) { return NULL; }
OSStatus AudioComponentCopyConfigurationInfo(AudioComponent component, CFDictionaryRef *info) { return kAudio_UnimplementedError; }
CFURLRef AudioComponentCopyComponentURL(AudioComponent component, OSStatus *error) { if (error) *error = kAudio_UnimplementedError; return NULL; }
void AudioComponentInstantiate(AudioComponent component, UInt32 options, void (^handler)(AudioComponentInstance, OSStatus)) {
    AudioComponentInstance instance = NULL;
    OSStatus status = AudioComponentInstanceNew(component, &instance);
    if (handler) handler(instance, status);
}

// Work intervals for real-time threads: a workgroup object the os_workgroup calls accept
void *AudioWorkIntervalCreate(const char *name, uint32_t clock, void *attributes) { return dispatch_group_create(); }

// MIDI to an instrument unit: not supported yet
OSStatus MusicDeviceMIDIEvent(MusicDeviceComponent unit, UInt32 status, UInt32 data1, UInt32 data2, UInt32 offset) { return kAudio_UnimplementedError; }
OSStatus MusicDeviceSysEx(MusicDeviceComponent unit, const UInt8 *data, UInt32 length) { return kAudio_UnimplementedError; }
OSStatus MusicDeviceMIDIEventList(MusicDeviceComponent unit, UInt32 offset, const struct MIDIEventList *events) { return kAudio_UnimplementedError; }

// Audio files: not supported yet (no file type is recognised)
#define Unsupported 0x7479703f   // 'typ?' kAudioFileUnsupportedFileTypeError
OSStatus AudioFileOpenURL(CFURLRef url, SInt8 permissions, UInt32 hint, void **file) { return Unsupported; }
OSStatus AudioFileCreateWithURL(CFURLRef url, UInt32 type, const void *format, UInt32 flags, void **file) { return Unsupported; }
OSStatus AudioFileGetPropertyInfo(void *file, UInt32 property, UInt32 *size, UInt32 *writable) { return Unsupported; }
OSStatus AudioFileGetProperty(void *file, UInt32 property, UInt32 *size, void *data) { return Unsupported; }
OSStatus AudioFileReadPacketData(void *file, Boolean cache, UInt32 *bytes, void *descriptions, SInt64 start, UInt32 *packets, void *buffer) { return Unsupported; }
OSStatus AudioFileWriteBytes(void *file, Boolean cache, SInt64 start, UInt32 *bytes, const void *buffer) { return Unsupported; }
OSStatus AudioFileOptimize(void *file) { return Unsupported; }
OSStatus AudioFileCountUserData(void *file, UInt32 id, UInt32 *count) { if (count) *count = 0; return Unsupported; }
OSStatus AudioFileGetUserDataSize(void *file, UInt32 id, UInt32 index, UInt32 *size) { return Unsupported; }
OSStatus AudioFileGetUserData(void *file, UInt32 id, UInt32 index, UInt32 *size, void *data) { return Unsupported; }
OSStatus AudioFileSetUserData(void *file, UInt32 id, UInt32 index, UInt32 size, const void *data) { return Unsupported; }
OSStatus AudioFileRemoveUserData(void *file, UInt32 id, UInt32 index) { return Unsupported; }
OSStatus AudioFileStreamOpen(void *client, void *properties, void *packets, UInt32 hint, void **stream) { return Unsupported; }
OSStatus AudioFileStreamParseBytes(void *stream, UInt32 size, const void *data, UInt32 flags) { return Unsupported; }
OSStatus AudioFileStreamGetPropertyInfo(void *stream, UInt32 property, UInt32 *size, Boolean *writable) { return Unsupported; }
OSStatus AudioFileStreamGetProperty(void *stream, UInt32 property, UInt32 *size, void *data) { return Unsupported; }
OSStatus AudioFileStreamClose(void *stream) { return noErr; }
OSStatus ExtAudioFileOpenURL(CFURLRef url, void **file) { return Unsupported; }
OSStatus ExtAudioFileCreateWithURL(CFURLRef url, UInt32 type, const void *format, const void *layout, UInt32 flags, void **file) { return Unsupported; }
OSStatus ExtAudioFileWrapAudioFileID(void *audioFile, Boolean forWriting, void **file) { return Unsupported; }
OSStatus ExtAudioFileDispose(void *file) { return noErr; }
OSStatus ExtAudioFileRead(void *file, UInt32 *frames, void *buffers) { if (frames) *frames = 0; return Unsupported; }
OSStatus ExtAudioFileWrite(void *file, UInt32 frames, const void *buffers) { return Unsupported; }
OSStatus ExtAudioFileWriteAsync(void *file, UInt32 frames, const void *buffers) { return Unsupported; }
OSStatus ExtAudioFileSeek(void *file, SInt64 frame) { return Unsupported; }
OSStatus ExtAudioFileTell(void *file, SInt64 *frame) { return Unsupported; }
OSStatus ExtAudioFileGetProperty(void *file, UInt32 property, UInt32 *size, void *data) { return Unsupported; }
OSStatus ExtAudioFileSetProperty(void *file, UInt32 property, UInt32 size, const void *data) { return Unsupported; }

// Standard MIDI files: not supported yet
OSStatus NewMusicSequence(void **sequence) { *sequence = calloc(1, 16); return noErr; }
OSStatus DisposeMusicSequence(void *sequence) { free(sequence); return noErr; }
OSStatus MusicSequenceFileLoad(void *sequence, CFURLRef url, UInt32 type, UInt32 flags) { return Unsupported; }
OSStatus MusicSequenceGetTrackCount(void *sequence, UInt32 *count) { *count = 0; return noErr; }
OSStatus MusicSequenceGetIndTrack(void *sequence, UInt32 index, void **track) { return -50; }
OSStatus MusicTrackGetProperty(void *track, UInt32 property, void *data, UInt32 *length) { return -50; }
OSStatus NewMusicEventIterator(void *track, void **iterator) { return -50; }
OSStatus DisposeMusicEventIterator(void *iterator) { return noErr; }
OSStatus MusicEventIteratorSeek(void *iterator, double time) { return -50; }
OSStatus MusicEventIteratorNextEvent(void *iterator) { return -50; }
OSStatus MusicEventIteratorHasCurrentEvent(void *iterator, Boolean *has) { *has = false; return noErr; }
OSStatus MusicEventIteratorGetEventInfo(void *iterator, double *time, UInt32 *type, const void **data, UInt32 *size) { return -50; }
