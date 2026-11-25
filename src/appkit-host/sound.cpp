// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Sounds read by libsndfile, and streams of samples, played and recorded through PulseAudio. Both libraries are loaded
// when a program first needs them.

#include "sound.h"

#include "host.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

// What sndfile.h and pulse/simple.h declare.
struct SndFile;

struct SndFileInfo
{
    int64_t frames;
    int sampleRate;
    int channels;
    int format;
    int sections;
    int seekable;
};

struct SndFileIo
{
    int64_t (*length)(void* context);
    int64_t (*seek)(int64_t offset, int whence, void* context);
    int64_t (*read)(void* buffer, int64_t count, void* context);
    int64_t (*write)(const void* buffer, int64_t count, void* context);
    int64_t (*tell)(void* context);
};

constexpr int sndFileRead = 0x10;

struct PulseStream;

enum PulseDirection { PulsePlayback = 1, PulseRecord = 2 };

struct PulseSampleSpec
{
    int format;
    uint32_t rate;
    uint8_t channels;
};

constexpr int pulseFloat32 = 5;

struct PulseBuffer
{
    uint32_t maxLength;
    uint32_t targetLength;
    uint32_t prebuffer;
    uint32_t minimumRequest;
    uint32_t fragmentSize;
};

struct Libraries
{
    bool loaded = false;
    bool sndFile = false;
    bool pulse = false;
    SndFile* (*open)(SndFileIo* io, int mode, SndFileInfo* info, void* context) = nullptr;
    int64_t (*readFrames)(SndFile* file, float* samples, int64_t frames) = nullptr;
    int64_t (*seek)(SndFile* file, int64_t frames, int whence) = nullptr;
    int (*close)(SndFile* file) = nullptr;
    PulseStream* (*newStream)(const char* server, const char* name, int direction, const char* device, const char* stream,
                              const PulseSampleSpec* spec, const void* map, const PulseBuffer* buffer, int* error) = nullptr;
    int (*writeStream)(PulseStream* stream, const void* data, size_t size, int* error) = nullptr;
    int (*readStream)(PulseStream* stream, void* data, size_t size, int* error) = nullptr;
    int (*drainStream)(PulseStream* stream, int* error) = nullptr;
    int (*flushStream)(PulseStream* stream, int* error) = nullptr;
    uint64_t (*streamLatency)(PulseStream* stream, int* error) = nullptr;
    void (*freeStream)(PulseStream* stream) = nullptr;
};

struct Bytes
{
    std::string data;
};

// Where libsndfile is in the bytes.
struct Reader
{
    std::shared_ptr<Bytes> bytes;
    int64_t offset = 0;
};

int64_t readerLength(void* context)
{
    return int64_t(static_cast<Reader*>(context)->bytes->data.size());
}

int64_t readerSeek(int64_t offset, int whence, void* context)
{
    Reader* reader = static_cast<Reader*>(context);
    const int64_t size = int64_t(reader->bytes->data.size());
    switch (whence) {
    case SEEK_SET:
        break;
    case SEEK_CUR:
        offset += reader->offset;
        break;
    default:
        offset += size;
        break;
    }
    reader->offset = std::min(std::max<int64_t>(offset, 0), size);
    return reader->offset;
}

int64_t readerRead(void* buffer, int64_t count, void* context)
{
    Reader* reader = static_cast<Reader*>(context);
    const std::string& data = reader->bytes->data;
    const int64_t size = std::max<int64_t>(0, std::min(count, int64_t(data.size()) - reader->offset));
    memcpy(buffer, data.data() + reader->offset, size_t(size));
    reader->offset += size;
    return size;
}

int64_t readerWrite(const void*, int64_t, void*)
{
    return 0;
}

int64_t readerTell(void* context)
{
    return static_cast<Reader*>(context)->offset;
}

SndFileIo readerIo = {readerLength, readerSeek, readerRead, readerWrite, readerTell};

} // namespace

struct reef_ui_sound
{
    std::shared_ptr<Bytes> bytes;
    SndFileInfo info = {};
    std::mutex mutex;
    std::condition_variable changed;
    std::thread thread;
    bool playing = false;
    bool paused = false;
    bool stopping = false;
    bool destroyed = false;
    bool loops = false;
    double volume = 1;
    int64_t position = 0;
    // The frame to go on from, or -1.
    int64_t seek = -1;
};

// Without a stream the samples go nowhere, in the time that playing them would take.
struct reef_ui_audio_output
{
    PulseStream* stream = nullptr;
    double rate;
    int channels;
    std::chrono::steady_clock::time_point time;
};

struct reef_ui_audio_input
{
    PulseStream* stream = nullptr;
    double rate;
    int channels;
    std::chrono::steady_clock::time_point time;
};

namespace cutefish::reef {

static bool silent = false;

template <typename Function>
static void load(void* library, const char* name, Function& function)
{
    function = reinterpret_cast<Function>(dlsym(library, name));
}

static const Libraries& libraries()
{
    static Libraries libraries;
    static std::once_flag once;
    std::call_once(once, [] {
        libraries.loaded = true;
        if (void* library = dlopen("libsndfile.so.1", RTLD_NOW)) {
            load(library, "sf_open_virtual", libraries.open);
            load(library, "sf_readf_float", libraries.readFrames);
            load(library, "sf_seek", libraries.seek);
            load(library, "sf_close", libraries.close);
            libraries.sndFile = libraries.open && libraries.readFrames && libraries.seek && libraries.close;
        }
        if (void* library = dlopen("libpulse-simple.so.0", RTLD_NOW)) {
            load(library, "pa_simple_new", libraries.newStream);
            load(library, "pa_simple_write", libraries.writeStream);
            load(library, "pa_simple_read", libraries.readStream);
            load(library, "pa_simple_drain", libraries.drainStream);
            load(library, "pa_simple_flush", libraries.flushStream);
            load(library, "pa_simple_free", libraries.freeStream);
            load(library, "pa_simple_get_latency", libraries.streamLatency);
            libraries.pulse = libraries.newStream && libraries.writeStream && libraries.drainStream && libraries.flushStream
                && libraries.freeStream;
        }
    });
    return libraries;
}

static SndFile* openBytes(Reader& reader, SndFileInfo& info)
{
    return libraries().open(&readerIo, sndFileRead, &info, &reader);
}

void setSoundSilent(bool value)
{
    silent = value;
}

static double secondsOf(const reef_ui_sound* sound, int64_t frames)
{
    return sound->info.sampleRate > 0 ? double(frames) / sound->info.sampleRate : 0;
}

// 160
double soundDuration(reef_ui_sound* sound)
{
    return sound ? secondsOf(sound, sound->info.frames) : 0;
}

// 161
double soundPosition(reef_ui_sound* sound)
{
    if (!sound)
        return 0;
    std::lock_guard<std::mutex> lock(sound->mutex);
    return secondsOf(sound, sound->seek >= 0 ? sound->seek : sound->position);
}

// 162
void soundSetPosition(reef_ui_sound* sound, double seconds)
{
    if (!sound)
        return;
    std::lock_guard<std::mutex> lock(sound->mutex);
    const int64_t frame = std::min(std::max<int64_t>(int64_t(seconds * sound->info.sampleRate), 0), sound->info.frames);
    if (sound->playing)
        sound->seek = frame;
    else
        sound->position = frame;
}

// 163
void soundSetVolume(reef_ui_sound* sound, double volume)
{
    if (!sound)
        return;
    std::lock_guard<std::mutex> lock(sound->mutex);
    sound->volume = std::min(std::max(volume, 0.0), 1.0);
}

// 164
int soundIsPlaying(reef_ui_sound* sound)
{
    if (!sound)
        return 0;
    std::lock_guard<std::mutex> lock(sound->mutex);
    return sound->playing && !sound->paused;
}

static void stop(reef_ui_sound* sound)
{
    {
        std::lock_guard<std::mutex> lock(sound->mutex);
        sound->stopping = true;
    }
    sound->changed.notify_all();
    if (sound->thread.joinable())
        sound->thread.join();
}

// 157
void soundStop(reef_ui_sound* sound)
{
    if (sound)
        stop(sound);
}

// 158
void soundPause(reef_ui_sound* sound)
{
    if (!sound)
        return;
    std::lock_guard<std::mutex> lock(sound->mutex);
    if (sound->playing)
        sound->paused = true;
}

// 159
void soundResume(reef_ui_sound* sound)
{
    if (!sound)
        return;
    {
        std::lock_guard<std::mutex> lock(sound->mutex);
        sound->paused = false;
    }
    sound->changed.notify_all();
}

// 216
reef_ui_audio_output* audioOutputCreate(const char* name, double rate, int channels, int frames)
{
    if (rate <= 0 || channels < 1 || channels > 32)
        return nullptr;
    reef_ui_audio_output* output = new reef_ui_audio_output;
    output->rate = rate;
    output->channels = channels;
    output->time = std::chrono::steady_clock::now();
    if (silent)
        return output;
    const Libraries& libs = libraries();
    if (!libs.pulse || !libs.streamLatency)
        return output;
    const PulseSampleSpec spec = {pulseFloat32, uint32_t(std::lround(rate)), uint8_t(channels)};
    const uint32_t count = std::max(frames, 64) * channels;
    const PulseBuffer buffer = {UINT32_MAX, count * 16, UINT32_MAX, count * 4, UINT32_MAX};
    int error = 0;
    output->stream = libs.newStream(nullptr, name && *name ? name : "cutefish-reef", PulsePlayback, nullptr, "playback",
                                    &spec, nullptr, &buffer, &error);
    return output;
}

// 217
int audioOutputWrite(reef_ui_audio_output* output, const float* samples, int frames)
{
    if (!output || !samples || frames <= 0)
        return 1;
    if (!output->stream) {
        output->time = std::max(output->time, std::chrono::steady_clock::now() - std::chrono::milliseconds(100));
        output->time += std::chrono::nanoseconds(int64_t(frames * 1e9 / output->rate));
        std::this_thread::sleep_until(output->time);
        return 0;
    }
    int error = 0;
    return libraries().writeStream(output->stream, samples, size_t(frames) * output->channels * sizeof(float), &error) < 0;
}

// 218
uint64_t audioOutputLatency(reef_ui_audio_output* output)
{
    if (!output || !output->stream)
        return 0;
    int error = 0;
    const uint64_t latency = libraries().streamLatency(output->stream, &error);
    return latency == uint64_t(-1) ? 0 : latency * 1000;
}

// 219
void audioOutputFlush(reef_ui_audio_output* output)
{
    if (!output || !output->stream)
        return;
    int error = 0;
    libraries().flushStream(output->stream, &error);
}

// 220
void audioOutputDestroy(reef_ui_audio_output* output)
{
    if (!output)
        return;
    if (output->stream)
        libraries().freeStream(output->stream);
    delete output;
}

// 279
reef_ui_audio_input* audioInputCreate(const char* name, double rate, int channels, int frames)
{
    if (rate <= 0 || channels < 1 || channels > 32)
        return nullptr;
    reef_ui_audio_input* input = new reef_ui_audio_input;
    input->rate = rate;
    input->channels = channels;
    input->time = std::chrono::steady_clock::now();
    if (silent)
        return input;
    const Libraries& libs = libraries();
    if (libs.pulse && libs.readStream) {
        const PulseSampleSpec spec = {pulseFloat32, uint32_t(std::lround(rate)), uint8_t(channels)};
        const uint32_t size = std::max(frames, 64) * channels * int(sizeof(float));
        const PulseBuffer buffer = {UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, size};
        int error = 0;
        input->stream = libs.newStream(nullptr, name && *name ? name : "cutefish-reef", PulseRecord, nullptr, "record",
                                       &spec, nullptr, &buffer, &error);
        if (input->stream)
            return input;
    }
    delete input;
    return nullptr;
}

// 280
int audioInputRead(reef_ui_audio_input* input, float* samples, int frames)
{
    if (!input || !samples || frames <= 0)
        return 1;
    if (!input->stream) {
        input->time = std::max(input->time, std::chrono::steady_clock::now() - std::chrono::milliseconds(100));
        input->time += std::chrono::nanoseconds(int64_t(frames * 1e9 / input->rate));
        std::this_thread::sleep_until(input->time);
        memset(samples, 0, size_t(frames) * input->channels * sizeof(float));
        return 0;
    }
    int error = 0;
    return libraries().readStream(input->stream, samples, size_t(frames) * input->channels * sizeof(float), &error) < 0;
}

// 281
uint64_t audioInputLatency(reef_ui_audio_input* input)
{
    if (!input || !input->stream)
        return 0;
    int error = 0;
    const uint64_t latency = libraries().streamLatency(input->stream, &error);
    return latency == uint64_t(-1) ? 0 : latency * 1000;
}

// 282
void audioInputDestroy(reef_ui_audio_input* input)
{
    if (!input)
        return;
    if (input->stream)
        libraries().freeStream(input->stream);
    delete input;
}

// 155
void soundDestroy(reef_ui_sound* sound)
{
    if (!sound)
        return;
    {
        std::lock_guard<std::mutex> lock(sound->mutex);
        sound->destroyed = true;
    }
    stop(sound);
    delete sound;
}

// The thread of a playing sound.
static void play(reef_ui_sound* sound, base::LoopHandle loop, void (*finished)(void*, int), void* context)
{
    const Libraries& libs = libraries();
    Reader reader = {sound->bytes};
    SndFileInfo info = {};
    SndFile* file = openBytes(reader, info);
    const PulseSampleSpec spec = {pulseFloat32, uint32_t(info.sampleRate), uint8_t(std::min(std::max(info.channels, 1), 8))};
    int error = 0;
    bool completed = false;
    PulseStream* stream = file ? libs.newStream(nullptr, "cutefish-reef", PulsePlayback, nullptr, "sound", &spec, nullptr,
                                                nullptr, &error)
                               : nullptr;
    if (stream) {
        {
            std::lock_guard<std::mutex> lock(sound->mutex);
            libs.seek(file, sound->position, SEEK_SET);
        }
        const int64_t frames = std::max(info.sampleRate / 50, 256);
        std::vector<float> samples(size_t(frames) * info.channels);
        while (true) {
            double volume;
            {
                std::unique_lock<std::mutex> lock(sound->mutex);
                sound->changed.wait(lock, [sound] { return !sound->paused || sound->stopping; });
                if (sound->stopping)
                    break;
                if (sound->seek >= 0) {
                    libs.flushStream(stream, &error);
                    sound->position = sound->seek;
                    sound->seek = -1;
                    libs.seek(file, sound->position, SEEK_SET);
                }
                volume = sound->volume;
            }
            const int64_t read = libs.readFrames(file, samples.data(), frames);
            if (read <= 0) {
                bool loops;
                {
                    std::lock_guard<std::mutex> lock(sound->mutex);
                    loops = sound->loops;
                    sound->position = loops ? 0 : sound->info.frames;
                }
                if (loops && libs.seek(file, 0, SEEK_SET) == 0)
                    continue;
                libs.drainStream(stream, &error);
                completed = true;
                break;
            }
            const size_t count = size_t(read) * info.channels;
            if (volume != 1) {
                for (size_t i = 0; i < count; i++)
                    samples[i] *= float(volume);
            }
            if (libs.writeStream(stream, samples.data(), count * sizeof(float), &error) < 0)
                break;
            std::lock_guard<std::mutex> lock(sound->mutex);
            sound->position += read;
        }
        libs.freeStream(stream);
    }
    if (file)
        libs.close(file);
    bool destroyed;
    {
        std::lock_guard<std::mutex> lock(sound->mutex);
        destroyed = sound->destroyed;
        sound->playing = false;
        sound->paused = false;
        if (completed)
            sound->position = 0;
    }
    if (finished && !destroyed)
        loop.post([finished, context, completed] { finished(context, completed); });
}

// 156
int soundPlay(reef_ui_sound* sound, double volume, int loops, void (*finished)(void*, int), void* context)
{
    if (!sound)
        return 1;
    stop(sound);
    base::EventLoop* loop = base::EventLoop::current();
    {
        std::lock_guard<std::mutex> lock(sound->mutex);
        sound->playing = true;
        sound->paused = false;
        sound->stopping = false;
        sound->loops = loops != 0;
        sound->seek = -1;
        sound->volume = std::min(std::max(volume, 0.0), 1.0);
    }
    if (silent || !libraries().pulse || !loop) {
        {
            std::lock_guard<std::mutex> lock(sound->mutex);
            sound->playing = false;
            sound->position = 0;
        }
        if (finished && loop)
            loop->post([finished, context] { finished(context, 1); });
        return !silent && !libraries().pulse ? 1 : 0;
    }
    const base::LoopHandle handle = loop->handle();
    sound->thread = std::thread(play, sound, handle, finished, context);
    return 0;
}

// 154
reef_ui_sound* soundCreate(const char* path, const void* bytes, size_t size)
{
    const Libraries& libs = libraries();
    if (!libs.sndFile)
        return nullptr;
    auto data = std::make_shared<Bytes>();
    if (path) {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            return nullptr;
        std::ostringstream stream;
        stream << file.rdbuf();
        data->data = stream.str();
    } else if (bytes && size) {
        data->data.assign(static_cast<const char*>(bytes), size);
    } else {
        return nullptr;
    }
    Reader reader = {data};
    SndFileInfo info = {};
    SndFile* file = openBytes(reader, info);
    if (!file)
        return nullptr;
    libs.close(file);
    if (info.channels <= 0 || info.sampleRate <= 0)
        return nullptr;
    reef_ui_sound* sound = new reef_ui_sound;
    sound->bytes = data;
    sound->info = info;
    return sound;
}

// 165
int soundPlayNamed(const char* name)
{
    if (!name || !*name)
        return 1;
    for (const char* directory : {"/usr/share/sounds/freedesktop/stereo/", "/usr/share/sounds/"}) {
        for (const char* extension : {".oga", ".ogg", ".wav"}) {
            const std::string path = std::string(directory) + name + extension;
            reef_ui_sound* sound = soundCreate(path.c_str(), nullptr, 0);
            if (!sound)
                continue;
            const int result = soundPlay(sound, 1, 0, [](void* context, int) {
                soundDestroy(static_cast<reef_ui_sound*>(context));
            }, sound);
            if (result)
                soundDestroy(sound);
            return result;
        }
    }
    return 1;
}

} // namespace cutefish::reef
