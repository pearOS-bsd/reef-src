// A PulseAudio that logs. Writing can be gated: each write waits for a credit that the test gives.
#include "stub_log.h"
#include <condition_variable>

static std::mutex m;
static std::condition_variable cv;
static bool gated = false;
static int credits = 0, arrived = 0, freed = 0, writes = 0;
static int failNew = 0, failWriteAt = -1, failRead = 0;
static uint64_t latency = 1234;
static int handles = 0;

extern "C" {
void stub_gate(int on) { std::lock_guard l(m); gated = on; credits = 0; arrived = 0; freed = 0; writes = 0; cv.notify_all(); }
void stub_fail(int newStream, int writeAt, int read, uint64_t lat) { failNew = newStream; failWriteAt = writeAt; failRead = read; latency = lat; }
// Gives a credit; returns when the thread is at the gate again, or freed the stream.
void stub_step(int n)
{
    std::unique_lock l(m);
    const int target = arrived + n;
    credits += n;
    cv.notify_all();
    cv.wait_for(l, std::chrono::seconds(5), [&] { return arrived >= target || freed > 0; });
}
void stub_wait_arrived(int n)
{
    std::unique_lock l(m);
    cv.wait_for(l, std::chrono::seconds(5), [&] { return arrived >= n || freed > 0; });
}
void stub_credit(int n) { std::lock_guard l(m); credits += n; cv.notify_all(); }
int stub_writes() { std::lock_guard l(m); return writes; }

struct spec { int format; uint32_t rate; uint8_t channels; unsigned char pad[3]; };
struct attr { uint32_t maxlength, tlength, prebuf, minreq, fragsize; };

void* pa_simple_new(const char* server, const char* name, int dir, const char* dev, const char* stream, const spec* ss, const void* map,
                    const attr* a, int* error)
{
    note("pa_simple_new server=%s name=%s dir=%d dev=%s stream=%s format=%d rate=%u channels=%u map=%d error=%d", server ? server : "-",
         name, dir, dev ? dev : "-", stream, ss->format, ss->rate, ss->channels, map != nullptr, *error);
    if (a)
        note("  attr %u %u %u %u %u", a->maxlength, a->tlength, a->prebuf, a->minreq, a->fragsize);
    if (failNew) {
        *error = 6;
        return nullptr;
    }
    return reinterpret_cast<void*>(intptr_t(0x1000 + ++handles));
}
#ifndef VARIANT_nowrite
int pa_simple_write(void* s, const void* data, size_t bytes, int* error)
{
    std::unique_lock l(m);
    const int index = writes++;
    note("pa_simple_write %d size=%zu data=%016llx error=%d", int(intptr_t(s) - 0x1000), bytes, (unsigned long long)hashOf(data, bytes), *error);
    if (gated) {
        arrived++;
        cv.notify_all();
        cv.wait_for(l, std::chrono::seconds(10), [&] { return credits > 0 || !gated; });
        if (credits > 0)
            credits--;
    }
    if (index == failWriteAt) {
        *error = 3;
        return -1;
    }
    return 0;
}
#endif
#ifndef VARIANT_noread
int pa_simple_read(void* s, void* data, size_t bytes, int* error)
{
    note("pa_simple_read %d size=%zu error=%d", int(intptr_t(s) - 0x1000), bytes, *error);
    for (size_t i = 0; i < bytes; i++)
        static_cast<unsigned char*>(data)[i] = i * 7 + bytes;
    return failRead ? -5 : 0;
}
#endif
int pa_simple_drain(void* s, int* error) { note("pa_simple_drain %d error=%d", int(intptr_t(s) - 0x1000), *error); return 0; }
int pa_simple_flush(void* s, int* error) { note("pa_simple_flush %d error=%d", int(intptr_t(s) - 0x1000), *error); return 0; }
void pa_simple_free(void* s)
{
    note("pa_simple_free %d", int(intptr_t(s) - 0x1000));
    std::lock_guard l(m);
    freed++;
    cv.notify_all();
}
#ifndef VARIANT_nolatency
uint64_t pa_simple_get_latency(void* s, int* error) { note("pa_simple_get_latency %d error=%d", int(intptr_t(s) - 0x1000), *error); return latency; }
#endif
}
