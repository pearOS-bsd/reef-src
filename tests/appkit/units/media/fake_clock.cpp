// A clock that the test moves: clock_gettime and nanosleep of the whole program.
#include "stub_log.h"
#include <dlfcn.h>
#include <time.h>

static int64_t nowNs = 1000000000000ll;
static int64_t stepNs = 0;
static bool on = false;

extern "C" {
void fake_clock(int enable, int64_t ns, int64_t step) { on = enable; nowNs = ns; stepNs = step; }
void fake_clock_advance(int64_t ns) { nowNs += ns; }
int clock_gettime(clockid_t id, timespec* t)
{
    static auto real = reinterpret_cast<int (*)(clockid_t, timespec*)>(dlsym(RTLD_NEXT, "clock_gettime"));
    if (!on)
        return real(id, t);
    nowNs += stepNs;
    t->tv_sec = nowNs / 1000000000;
    t->tv_nsec = nowNs % 1000000000;
    return 0;
}
int nanosleep(const timespec* t, timespec* rest)
{
    static auto real = reinterpret_cast<int (*)(const timespec*, timespec*)>(dlsym(RTLD_NEXT, "nanosleep"));
    if (!on)
        return real(t, rest);
    note("nanosleep %lld.%09ld", (long long)t->tv_sec, t->tv_nsec);
    nowNs += t->tv_sec * 1000000000ll + t->tv_nsec;
    return 0;
}
}
