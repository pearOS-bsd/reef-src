// The log of GPU work. test_gpu LIBRARY SEED LOG
#include "load.h"
#include "stub_log.h"
#include "ui/frame_submission.h"
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <vector>

static uint64_t state_ = 1;
static uint32_t rnd() { state_ = state_ * 6364136223846793005ull + 1442695040888963407ull; return uint32_t(state_ >> 33); }

template <typename F> static F* fake(const char* name) { return reinterpret_cast<F*>(dlsym(RTLD_DEFAULT, name)); }

static std::string logFile;
static size_t shown = 0;
static void showLog()
{
    for (const std::string& f : {logFile + ".old", logFile}) {
        struct stat st;
        if (stat(f.c_str(), &st) != 0) {
            note("%s: none", f.c_str());
            continue;
        }
        note("%s: size %lld mode %o", f.c_str(), (long long)st.st_size, st.st_mode);
        if (f != logFile)
            continue;
        std::string text;
        if (FILE* h = fopen(f.c_str(), "rb")) {
            char b[4096];
            for (size_t k; (k = fread(b, 1, sizeof b, h)) > 0;)
                text.append(b, k);
            fclose(h);
        }
        size_t from = shown > text.size() ? 0 : shown;
        shown = text.size();
        while (from < text.size()) {
            const size_t end = text.find('\n', from);
            note("LOG %s", text.substr(from, (end == std::string::npos ? text.size() : end) - from).c_str());
            if (end == std::string::npos)
                break;
            from = end + 1;
        }
    }
}

int main(int argc, char** argv)
{
    load(argv[1]);
    state_ = strtoull(argv[2], nullptr, 10) * 977 + 5;
    logFile = argv[3];
    auto enable = fake<void(int)>("fake_gpu");
    auto advance = fake<void(int64_t)>("fake_gpu_advance");
    auto next = fake<void(int64_t, int)>("fake_gpu_next");
    auto setContext = fake<void(uintptr_t, uintptr_t)>("fake_gpu_context");
    auto missing = fake<void(const char*)>("fake_gpu_missing");
    auto handler = fake<void*()>("fake_gpu_handler");
    auto step = fake<void()>("fake_gpu_step");
    auto live = fake<int()>("fake_gpu_live");
    auto arrived = fake<int()>("fake_gpu_arrived");
    auto waitAsleep = fake<void(int)>("fake_gpu_wait_asleep");
    auto issued = fn<void(const char*, const char*)>(314);
    auto measured = fn<void(const char*, const char*, int, int, double, double)>(315);
    auto thread = fn<int()>(316);
    auto logFrames = fn<void()>(318);
    if (const char* name = getenv("FAKE_MISSING"))
        missing(name);
    enable(1);
    note("thread %d", thread());
    logFrames();
    auto drawn = reinterpret_cast<void (*)(const cutefish::ui::DrawnFrame&)>(handler());
    const char* names[] = {nullptr, "", "a", "blur", "window frame", "a name that is longer than what fits into a short string", "upload; of texture", "x | also running: y"};
    const char* details[] = {nullptr, "", "d", "512x512", "the detail of the work, which is also long enough to be allocated", "two\tparts", "a; b"};
    setContext(0, 0x88);
    issued("no display", "d");
    setContext(0x77, 0);
    issued("no context", "d");
    setContext(0x77, 0x88);
    next(0, 1);
    issued("no sync", "d");
    if (getenv("FAKE_MISSING")) {
        issued("x", "y");
        measured("x", "y", 0, 7, 50, 1785000000.5);
        showLog();
        fflush(stdout);
        _exit(0);
    }
    for (int round = 0; round < 60; round++) {
        note("-- %d live %d", round, live());
        const int action = rnd() % 10;
        if (action < 5) {
            // Life times: at once, fast, slow, stuck, failing.
            const int64_t lives[] = {0, 20000000, 99000000, 101000000, 150000000, 1100000000, 2500000000ll, 30000000000ll, -1};
            next(lives[rnd() % 9], rnd() % 15 == 0);
            setContext(0x77 + rnd() % 2, 0x88);
            const int before = arrived();
            const bool idle = live() == 0;
            if (rnd() % 3 == 0) {
                cutefish::ui::DrawnFrame frame;
                frame.width = rnd() % 4000;
                frame.height = int(rnd() % 3000) - 100;
                frame.glassPanes = rnd() % 12;
                frame.glassPixels = rnd() / 7.0;
                frame.largestBlur = rnd() % 1000 / 13.0;
                drawn(frame);
            } else {
                issued(names[rnd() % 8], details[rnd() % 7]);
            }
            if (idle && live() > 0)
                waitAsleep(before);
        } else if (action < 6) {
            const double times[] = {1785000000.123456, 0, 1785000000, 1e9 + 0.999999999, 1784999999.5, -1.5, 951782400.25};
            const double durations[] = {0, 100, 101.9, 1234.5678, -5, 1e7};
            const int threads[] = {0, 1, 4242, 99999, 2147483647, -7, -2147483647};
            measured(names[rnd() % 8], details[rnd() % 7], rnd() % 3, threads[rnd() % 7], durations[rnd() % 6], times[rnd() % 7]);
        } else {
            const int64_t steps[] = {0, 1000000, 20000000, 79000000, 100000000, 100000001, 1000000000, 1000000001, 3000000000ll, 999999};
            advance(steps[rnd() % 10]);
            if (live() > 0)
                step();
        }
        if (round % 5 == 4)
            showLog();
    }
    showLog();
    fflush(stdout);
    _exit(0);
}
