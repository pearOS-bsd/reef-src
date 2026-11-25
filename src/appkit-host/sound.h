// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Sounds read by libsndfile, and streams of samples, played and recorded through PulseAudio.

#pragma once

#include <cstddef>
#include <cstdint>

struct reef_ui_sound;
struct reef_ui_audio_output;
struct reef_ui_audio_input;

namespace cutefish::reef {

// Of the file at the path, or of the bytes without a path. Null when it is no sound that libsndfile reads.
reef_ui_sound* soundCreate(const char* path, const void* bytes, size_t size);
void soundDestroy(reef_ui_sound* sound);
// Both return 0, or 1 when there is no such sound or no PulseAudio. The handler runs on the loop of the calling thread,
// with 1 when the sound was played to its end or not at all. A sound that is destroyed does not call it.
int soundPlay(reef_ui_sound* sound, double volume, int loops, void (*finished)(void*, int), void* context);
int soundPlayNamed(const char* name);
void soundStop(reef_ui_sound* sound);
void soundPause(reef_ui_sound* sound);
void soundResume(reef_ui_sound* sound);
int soundIsPlaying(reef_ui_sound* sound);
// Seconds.
double soundDuration(reef_ui_sound* sound);
double soundPosition(reef_ui_sound* sound);
void soundSetPosition(reef_ui_sound* sound, double seconds);
void soundSetVolume(reef_ui_sound* sound, double volume);

// Interleaved samples; `frames` is what the program writes or reads at a time. Latencies are nanoseconds. Writing and
// reading block, and return 0 when they succeed.
reef_ui_audio_output* audioOutputCreate(const char* name, double rate, int channels, int frames);
int audioOutputWrite(reef_ui_audio_output* output, const float* samples, int frames);
uint64_t audioOutputLatency(reef_ui_audio_output* output);
void audioOutputFlush(reef_ui_audio_output* output);
void audioOutputDestroy(reef_ui_audio_output* output);
reef_ui_audio_input* audioInputCreate(const char* name, double rate, int channels, int frames);
int audioInputRead(reef_ui_audio_input* input, float* samples, int frames);
uint64_t audioInputLatency(reef_ui_audio_input* input);
void audioInputDestroy(reef_ui_audio_input* input);

} // namespace cutefish::reef
