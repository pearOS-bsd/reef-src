// Displays, with made up DRM devices. test_displays LIBRARY SEED MODE (the devices are opened once per process)
#include "load.h"
#include "stub_log.h"

int main(int argc, char** argv)
{
    load(argv[1]);
    auto fake = reinterpret_cast<void (*)(int, uint64_t, int, const char* const*, int)>(dlsym(RTLD_DEFAULT, "fake_dev"));
    auto clock = reinterpret_cast<void (*)(int, int64_t, int64_t)>(dlsym(RTLD_DEFAULT, "fake_clock"));
    auto advance = reinterpret_cast<void (*)(int64_t)>(dlsym(RTLD_DEFAULT, "fake_clock_advance"));
    auto vblank = fn<int(const char*, uint64_t*)>(284);
    auto period = fn<uint64_t(const char*)>(285);
    const int seed = atoi(argv[2]), mode = atoi(argv[3]);
    fake(1, seed, mode, nullptr, 0);
    clock(1, 1000000000000ll, 1);
    const char* names[] = {nullptr, "", "eDP-1", "HDMI-A-1", "DP-1", "DP-12", "LVDS-123456", "Unknown-7", "USB-4294967295", "Unknown-1", "Virtual-1", "Unknown-2", "Writeback-1", "VGA-1",
                           "DP-2", "nothing", "eDP-11", "eDP", "a name that is longer than what fits into a string", "Composite-3"};
    for (int round = 0; round < 8; round++) {
        for (const char* name : names) {
            uint64_t time = 999;
            note("period %s: %llu", name ? name : "null", (unsigned long long)period(name));
            note("vblank %s: %d %llu", name ? name : "null", vblank(name, &time), (unsigned long long)time);
        }
        const int64_t steps[] = {0, 500000000, 499999000, 1000000000, 999999999, 1, 2000000000, 0};
        advance(steps[round]);
        note("== %d", round);
    }
    fflush(stdout);
    _exit(0);
}
