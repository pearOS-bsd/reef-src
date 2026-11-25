// Sounds and streams with the stubs of libsndfile and libpulse-simple. test_sound LIBRARY CASE
#include "base/event_loop.h"
#include "load.h"
#include "stub_log.h"
#include <chrono>
#include <cmath>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <vector>

using namespace cutefish;
struct Sound;
struct Stream;

static std::string bytesOf(int64_t frames, int rate, int channels, int flags, size_t extra = 100)
{
    std::string b = "SND1";
    b.append(reinterpret_cast<char*>(&frames), 8);
    b.append(reinterpret_cast<char*>(&rate), 4);
    b.append(reinterpret_cast<char*>(&channels), 4);
    b.append(reinterpret_cast<char*>(&flags), 4);
    for (size_t i = 0; i < extra; i++)
        b.push_back(char(i * 3));
    return b;
}

static auto create(const char* p, const void* b, size_t n) { return fn<Sound*(const char*, const void*, size_t)>(154)(p, b, n); }
static void destroy(Sound* s) { fn<void(Sound*)>(155)(s); }
static int play(Sound* s, double v, int loops, void (*f)(void*, int), void* c) { return fn<int(Sound*, double, int, void (*)(void*, int), void*)>(156)(s, v, loops, f, c); }
static void stop(Sound* s) { fn<void(Sound*)>(157)(s); }
static void pause_(Sound* s) { fn<void(Sound*)>(158)(s); }
static void resume(Sound* s) { fn<void(Sound*)>(159)(s); }
static double duration(Sound* s) { return fn<double(Sound*)>(160)(s); }
static double position(Sound* s) { return fn<double(Sound*)>(161)(s); }
static void setPosition(Sound* s, double v) { fn<void(Sound*, double)>(162)(s, v); }
static void setVolume(Sound* s, double v) { fn<void(Sound*, double)>(163)(s, v); }
static int playing(Sound* s) { return fn<int(Sound*)>(164)(s); }
static int named(const char* n) { return fn<int(const char*)>(165)(n); }
static void silent(bool v) { fn<void(bool)>(317)(v); }

template <typename F> static F* stub(const char* name)
{
    static void* lib = dlopen("libpulse-simple.so.0", RTLD_NOW);
    return lib ? reinterpret_cast<F*>(dlsym(lib, name)) : nullptr;
}
static void gate(int on) { if (auto f = stub<void(int)>("stub_gate")) f(on); }
static void step(int n) { stub<void(int)>("stub_step")(n); }
static void credit(int n) { stub<void(int)>("stub_credit")(n); }
static void waitArrived(int n) { stub<void(int)>("stub_wait_arrived")(n); }
static void fail(int a, int b, int c, uint64_t l) { if (auto f = stub<void(int, int, int, uint64_t)>("stub_fail")) f(a, b, c, l); }

static int finishedCount = 0;
static void finished(void* context, int status)
{
    note("finished %d %d", int(intptr_t(context)), status);
    finishedCount++;
}
static void state(const char* what, Sound* s)
{
    note("%s: duration %.6f position %.6f playing %d", what, duration(s), position(s), playing(s));
}
static base::EventLoop* loop;
static void runUntil(int count, int ms = 3000)
{
    const auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    while (finishedCount < count && std::chrono::steady_clock::now() < end) {
        loop->dispatch();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    loop->dispatch();
    note("loop done %d", finishedCount);
}
// Stops it while it is at the gate, then lets it go on.
static void stopAtGate(Sound* s, bool destroyIt = false)
{
    std::thread other([=] { destroyIt ? destroy(s) : stop(s); });
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    credit(1);
    other.join();
}
static void settle() { std::this_thread::sleep_for(std::chrono::milliseconds(60)); }

static void creation()
{
    const std::string good = bytesOf(1000, 8000, 2, 0);
    auto show = [](const char* what, Sound* s) {
        note("%s -> %d", what, s != nullptr);
        if (s) {
            state(what, s);
            destroy(s);
        }
    };
    show("nothing", create(nullptr, nullptr, 0));
    show("no size", create(nullptr, good.data(), 0));
    show("no bytes", create(nullptr, nullptr, 10));
    show("bytes", create(nullptr, good.data(), good.size()));
    show("short", create(nullptr, good.data(), 1));
    show("small", create(nullptr, good.data(), 15));
    show("head", create(nullptr, good.data(), 24));
    show("bad", create(nullptr, "some bytes that are no sound at all", 30));
    show("rate 0", create(nullptr, bytesOf(1000, 0, 2, 0).data(), 124));
    show("rate -1", create(nullptr, bytesOf(1000, -1, 2, 0).data(), 124));
    show("channels 0", create(nullptr, bytesOf(1000, 8000, 0, 0).data(), 124));
    show("channels -2", create(nullptr, bytesOf(1000, 8000, -2, 0).data(), 124));
    show("frames 0", create(nullptr, bytesOf(0, 8000, 1, 0).data(), 124));
    show("frames -4", create(nullptr, bytesOf(-4, 8000, 1, 0).data(), 124));
    const std::string big = bytesOf(100000, 44100, 6, 0, 100000);
    show("big", create(nullptr, big.data(), big.size()));
    const char* path = "/tmp/media/reeftest.snd";
    for (const std::string& content : {good, std::string(), std::string("SND1"), big}) {
        FILE* f = fopen(path, "wb");
        fwrite(content.data(), 1, content.size(), f);
        fclose(f);
        show("file", create(path, nullptr, 0));
        show("file and bytes", create(path, good.data(), good.size()));
    }
    unlink(path);
    show("missing file", create(path, good.data(), good.size()));
    show("empty path", create("", good.data(), good.size()));
    show("folder", create("/tmp", nullptr, 0));
    destroy(nullptr);
    note("null: %f %f %d %d", duration(nullptr), position(nullptr), playing(nullptr), play(nullptr, 1, 0, finished, nullptr));
    stop(nullptr); pause_(nullptr); resume(nullptr); setPosition(nullptr, 1); setVolume(nullptr, 1);
    Sound* s = create(nullptr, good.data(), good.size());
    for (double v : {0.0, 0.01, -1.0, 0.05, 1e9, -1e30, 0.1, 1e300, 0.124999, 0.125, 0.1251}) {
        setPosition(s, v);
        state("set", s);
    }
    stop(s); pause_(s); state("paused", s); resume(s); state("resumed", s);
    destroy(s);
}

static void playback()
{
    const std::string good = bytesOf(1000, 8000, 2, 0);
    Sound* s = create(nullptr, good.data(), good.size());
    state("before", s);
    note("play %d", play(s, 1, 0, finished, (void*)1));
    runUntil(1);
    state("after", s);
    for (double v : {0.5, 0.0, -1.0, 2.0, 0.999, 1e-9}) {
        finishedCount = 0;
        note("play volume %g: %d", v, play(s, v, 0, finished, (void*)2));
        runUntil(1);
    }
    for (double v : {0.25, -3.0, 7.0}) {
        setVolume(s, v);
        finishedCount = 0;
        note("play after setVolume %g: %d", v, play(s, 0.75, 0, finished, (void*)3));
        runUntil(1);
    }
    // From a position.
    setPosition(s, 0.1);
    state("from", s);
    finishedCount = 0;
    play(s, 1, 0, finished, (void*)4);
    runUntil(1);
    state("after", s);
    // Without a handler.
    play(s, 1, 0, nullptr, nullptr);
    settle();
    loop->dispatch();
    state("no handler", s);
    destroy(s);
    for (const auto& [frames, rate, channels, flags] : {std::tuple<int, int, int, int>{30, 8000, 1, 0}, {2000, 44100, 1, 0}, {2000, 48000, 2, 0}, {5000, 192000, 8, 0},
                                                        {3000, 12799, 3, 0}, {3000, 12800, 3, 0}, {3000, 12849, 3, 0}, {3000, 12850, 3, 0}, {700, 100, 9, 0}, {700, 8000, 64, 0}, {900, 8000, 2, 2}, {0, 8000, 2, 0}}) {
        const std::string b = bytesOf(frames, rate, channels, flags);
        Sound* t = create(nullptr, b.data(), b.size());
        for (double v : {1.0, 0.3}) {
            finishedCount = 0;
            note("frames %d rate %d channels %d: %d", frames, rate, channels, play(t, v, 0, finished, (void*)5));
            runUntil(1);
        }
        destroy(t);
    }
}

static void control()
{
    const std::string good = bytesOf(4000, 8000, 2, 0);
    Sound* s = create(nullptr, good.data(), good.size());
    gate(1);
    note("play %d", play(s, 1, 0, finished, (void*)1));
    waitArrived(1);
    state("at 1", s);
    step(1);
    state("at 2", s);
    step(1);
    state("at 3", s);
    setVolume(s, 0.5);
    step(1);
    state("at 4", s);
    step(1);
    state("at 5", s);
    // Seek: flush and seek.
    setPosition(s, 0.4);
    state("seeking", s);
    step(1);
    state("sought", s);
    setPosition(s, -5);
    step(1);
    state("sought to start", s);
    // Pause.
    pause_(s);
    state("paused", s);
    credit(1);
    settle();
    state("paused and waiting", s);
    setPosition(s, 0.2);
    state("paused and sought", s);
    resume(s);
    waitArrived(8);
    state("resumed", s);
    step(1);
    state("going on", s);
    // Stop.
    stopAtGate(s);
    state("stopped", s);
    runUntil(1);
    state("after", s);
    gate(0);
    destroy(s);

    // Stop while paused.
    s = create(nullptr, good.data(), good.size());
    gate(1);
    finishedCount = 0;
    play(s, 1, 0, finished, (void*)2);
    waitArrived(1);
    pause_(s);
    credit(1);
    settle();
    stop(s);
    runUntil(1);
    state("stopped while paused", s);
    // Play again while it plays.
    gate(1);
    finishedCount = 0;
    play(s, 1, 0, finished, (void*)3);
    waitArrived(1);
    {
        std::thread other([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            credit(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            gate(0);
        });
        note("again %d", play(s, 0.5, 0, finished, (void*)4));
        other.join();
    }
    runUntil(2);
    state("played again", s);
    // Seek before the end and to the end.
    gate(1);
    finishedCount = 0;
    play(s, 1, 0, finished, (void*)5);
    waitArrived(1);
    setPosition(s, 100);
    credit(10);
    runUntil(1);
    state("sought to the end", s);
    gate(0);
    // Destroy while it plays: no handler.
    gate(1);
    finishedCount = 0;
    play(s, 1, 0, finished, (void*)6);
    waitArrived(1);
    stopAtGate(s, true);
    runUntil(1, 100);
    gate(0);
    // Destroy while paused.
    s = create(nullptr, good.data(), good.size());
    gate(1);
    play(s, 1, 0, finished, (void*)7);
    waitArrived(1);
    pause_(s);
    credit(1);
    settle();
    destroy(s);
    runUntil(1, 100);
    gate(0);
}

static void loops()
{
    for (int flags : {0, 1}) {
        const std::string b = bytesOf(600, 8000, 2, flags);
        Sound* s = create(nullptr, b.data(), b.size());
        gate(1);
        finishedCount = 0;
        note("loop %d", play(s, 1, 1, finished, (void*)1));
        waitArrived(1);
        if (flags == 0) {
            for (int i = 0; i < 7; i++) {
                step(1);
                state("looping", s);
            }
            stopAtGate(s);
        } else {
            credit(20);
        }
        runUntil(1);
        state("after", s);
        gate(0);
        destroy(s);
    }
}

static void failures()
{
    const std::string good = bytesOf(4000, 8000, 2, 0);
    Sound* s = create(nullptr, good.data(), good.size());
    fail(1, -1, 0, 0);
    note("no stream %d", play(s, 1, 0, finished, (void*)1));
    runUntil(1);
    state("after", s);
    for (int at : {0, 1, 3}) {
        fail(0, at, 0, 0);
        gate(0);
        finishedCount = 0;
        note("write fails %d: %d", at, play(s, 1, 0, finished, (void*)2));
        runUntil(1);
        state("after", s);
    }
    fail(0, -1, 0, 0);
    destroy(s);
}

static void silence()
{
    const std::string good = bytesOf(4000, 8000, 2, 0);
    silent(true);
    Sound* s = create(nullptr, good.data(), good.size());
    note("silent %d", play(s, 1, 1, finished, (void*)1));
    state("silent", s);
    runUntil(1);
    setPosition(s, 0.2);
    state("silent", s);
    note("silent, no handler %d", play(s, 1, 1, nullptr, nullptr));
    state("silent", s);
    destroy(s);
    silent(false);
}

static void noLoop()
{
    const std::string good = bytesOf(4000, 8000, 2, 0);
    Sound* s = create(nullptr, good.data(), good.size());
    if (!s) {
        note("no sound");
        return;
    }
    note("no loop %d", play(s, 1, 1, finished, (void*)1));
    state("no loop", s);
    setPosition(s, 0.3);
    note("no loop %d", play(s, 1, 0, nullptr, nullptr));
    state("no loop", s);
    destroy(s);
}

static void names()
{
    mkdir("/usr/share/sounds/freedesktop", 0755);
    mkdir("/usr/share/sounds/freedesktop/stereo", 0755);
    const std::string good = bytesOf(500, 8000, 1, 0), bad = "nothing";
    auto file = [](const char* path, const std::string& content) {
        FILE* f = fopen(path, "wb");
        if (!f)
            return note("cannot write %s", path);
        fwrite(content.data(), 1, content.size(), f);
        fclose(f);
    };
    const char* a = "/usr/share/sounds/freedesktop/stereo/reeftest-a.oga";
    const char* b = "/usr/share/sounds/freedesktop/stereo/reeftest-b.wav";
    const char* c = "/usr/share/sounds/reeftest-c.ogg";
    const char* d = "/usr/share/sounds/freedesktop/stereo/reeftest-d.oga";
    const char* d2 = "/usr/share/sounds/reeftest-d.wav";
    file(a, good); file(b, good); file(c, good); file(d, bad); file(d2, good);
    for (const char* name : {(const char*)nullptr, "", "reeftest-a", "reeftest-b", "reeftest-c", "reeftest-d", "reeftest-e", "../sounds/reeftest-c"}) {
        note("named %s: %d", name ? name : "null", named(name));
        settle();
        loop->dispatch();
    }
    unlink(a); unlink(b); unlink(c); unlink(d); unlink(d2);
}

static void streams()
{
    auto outCreate = fn<Stream*(const char*, double, int, int)>(216);
    auto outWrite = fn<int(Stream*, const float*, int)>(217);
    auto outLatency = fn<uint64_t(Stream*)>(218);
    auto outFlush = fn<void(Stream*)>(219);
    auto outDestroy = fn<void(Stream*)>(220);
    auto inCreate = fn<Stream*(const char*, double, int, int)>(279);
    auto inRead = fn<int(Stream*, float*, int)>(280);
    auto inLatency = fn<uint64_t(Stream*)>(281);
    auto inDestroy = fn<void(Stream*)>(282);
    auto clock = reinterpret_cast<void (*)(int, int64_t, int64_t)>(dlsym(RTLD_DEFAULT, "fake_clock"));
    auto advance = reinterpret_cast<void (*)(int64_t)>(dlsym(RTLD_DEFAULT, "fake_clock_advance"));
    std::vector<float> samples(100000);
    for (size_t i = 0; i < samples.size(); i++)
        samples[i] = float(i) / 1000;
    for (int fails : {0, 1}) {
        fail(fails, -1, 0, 1234);
        for (bool quiet : {false, true}) {
            silent(quiet);
            for (const auto& [name, rate, channels, frames] : {std::tuple<const char*, double, int, int>{nullptr, 44100, 2, 512}, {"", 48000.4, 1, 64}, {"App", 48000.5, 1, 63},
                                                               {"App", 0, 2, 512}, {"App", -1, 2, 512}, {"App", NAN, 2, 512}, {"App", 8000, 0, 512}, {"App", 8000, -1, 512}, {"App", 8000, 33, 512},
                                                               {"App", 8000, 32, 1024}, {"App", 8000, 2, 0}, {"App", 8000, 2, -100}, {"App", 8000, 2, 1000000000}, {"App", 0.4, 2, 100}, {"App", 1e12, 2, 100}}) {
                Stream* o = outCreate(name, rate, channels, frames);
                note("output %s %g %d %d -> %d", name ? name : "null", rate, channels, frames, o != nullptr);
                if (o)
                    outDestroy(o);
                Stream* i = inCreate(name, rate, channels, frames);
                note("input %s %g %d %d -> %d", name ? name : "null", rate, channels, frames, i != nullptr);
                if (i)
                    inDestroy(i);
            }
        }
    }
    for (bool quiet : {false, true}) {
        silent(quiet);
        fail(0, -1, 0, 1234);
        gate(0);
        if (clock)
            clock(1, 5000000000000ll, 1000);
        Stream* o = outCreate("App", 44100, 2, 512);
        Stream* i = inCreate("App", 44100, 2, 512);
        for (int n : {0, 1, 512, 4096, -3})
            note("write %d: %d", n, outWrite(o, samples.data(), n));
        note("write null: %d %d", outWrite(o, nullptr, 10), outWrite(nullptr, samples.data(), 10));
        for (int n : {0, 1, 512, 4096, -3}) {
            std::fill(samples.begin(), samples.end(), 5.f);
            note("read %d: %d %016llx", n, i ? inRead(i, samples.data(), n) : 99, (unsigned long long)hashOf(samples.data(), samples.size() * 4));
        }
        if (i)
            note("read null: %d %d", inRead(i, nullptr, 10), inRead(nullptr, samples.data(), 10));
        // Pacing: behind, and ahead.
        if (clock && quiet) {
            for (int64_t pause : {0ll, 1000000ll, 50000000ll, 99999999ll, 100000000ll, 150000000ll, 3000000000ll, 0ll, 0ll}) {
                advance(pause);
                note("paced write %d", outWrite(o, samples.data(), 441));
                advance(pause);
                note("paced read %d", inRead(i, samples.data(), 441));
            }
        }
        for (uint64_t l : {uint64_t(0), uint64_t(77), uint64_t(-1), uint64_t(-2)}) {
            fail(0, -1, 0, l);
            note("latency %llu %llu", (unsigned long long)outLatency(o), (unsigned long long)(i ? inLatency(i) : 99));
        }
        fail(0, 0, 1, 1);
        gate(0);
        note("failing write %d read %d", outWrite(o, samples.data(), 100), i ? inRead(i, samples.data(), 100) : 99);
        fail(0, -1, 0, 1);
        outFlush(o);
        outFlush(nullptr);
        note("null latency %llu %llu", (unsigned long long)outLatency(nullptr), (unsigned long long)inLatency(nullptr));
        if (clock)
            clock(0, 0, 0);
        outDestroy(o);
        if (i)
            inDestroy(i);
        outDestroy(nullptr);
        inDestroy(nullptr);
    }
}

// With the real libsndfile: files, and their bytes.
static void realFiles(int count, char** files)
{
    for (int i = 0; i < count; i++) {
        std::string content;
        if (FILE* f = fopen(files[i], "rb")) {
            char b[65536];
            for (size_t k; (k = fread(b, 1, sizeof b, f)) > 0;)
                content.append(b, k);
            fclose(f);
        }
        for (int way = 0; way < 4; way++) {
            const std::string part = way < 2 ? content : content.substr(0, content.size() / (way == 2 ? 2 : 40));
            Sound* s = way == 0 ? create(files[i], nullptr, 0) : create(nullptr, part.data(), part.size());
            note("%s way %d -> %d", files[i], way, s != nullptr);
            if (!s)
                continue;
            state("created", s);
            for (double v : {1.0, 0.4}) {
                finishedCount = 0;
                setPosition(s, v == 1.0 ? 0 : 0.15);
                note("play %d", play(s, v, 0, finished, (void*)1));
                runUntil(1);
                state("played", s);
            }
            destroy(s);
        }
    }
}

int main(int argc, char** argv)
{
    load(argv[1]);
    const std::string test = argv[2];
    if (test == "noloop") {
        noLoop();
        return 0;
    }
    base::EventLoop eventLoop;
    loop = &eventLoop;
    if (test == "creation") creation();
    if (test == "playback") playback();
    if (test == "control") control();
    if (test == "loops") loops();
    if (test == "failures") failures();
    if (test == "silence") silence();
    if (test == "names") names();
    if (test == "streams") streams();
    if (test == "nothing") noLoop();
    if (test == "real") realFiles(argc - 3, argv + 3);
    fflush(stdout);
    _exit(0);
}
