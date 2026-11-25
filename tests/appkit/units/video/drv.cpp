// Runs case files through the video functions of the original (-DORIGINAL) or of ours and writes a log of everything.
//   drv LOG [options] case...      every case in a child process
//   drv LOG --fuzz FIRST COUNT case...   changed configurations, frames and failing calls; a line with a hash for each
// Options: --keep n, --option name=value, --fail name:n, --drop n (leave out every nth frame), --full (whole logs when fuzzing)
#include "/w/src/appkit-host/video.h"
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <dlfcn.h>
#include <exception>
#include <fcntl.h>
#include <map>
#include <string>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// ---- what the stub and the libraries call

struct Shared
{
    size_t length;
    char text[256 << 20];
};
static Shared* shared;
static std::map<std::string, uint32_t> options;
static std::string failName;
static int failAt = -1;
static int failRate = 0;
static uint64_t failSeed = 1;
static std::map<std::string, int> calls;

static uint32_t randomNumber(uint64_t& state)
{
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return uint32_t(state >> 16);
}

extern "C" __attribute__((visibility("default"))) void test_log(const char* text, size_t length)
{
    if (shared->length + length <= sizeof(shared->text)) {
        memcpy(shared->text + shared->length, text, length);
        shared->length += length;
    }
}
extern "C" __attribute__((visibility("default"))) int test_fail(const char* name)
{
    if (failRate && randomNumber(failSeed) % failRate == 0)
        return 1;
    return failName == name && calls[name]++ == failAt;
}
extern "C" __attribute__((visibility("default"))) uint32_t test_option(const char* name, uint32_t fallback)
{
    const auto found = options.find(name);
    return found == options.end() ? fallback : found->second;
}

static void out(const char* format, ...)
{
    char line[4096];
    va_list list;
    va_start(list, format);
    const int n = vsnprintf(line, sizeof(line), format, list);
    va_end(list);
    test_log(line, n);
}

extern "C" __attribute__((visibility("default"))) int open(const char* path, int flags, ...)
{
    int mode = 0;
    if (flags & (O_CREAT | O_TMPFILE)) {
        va_list list;
        va_start(list, flags);
        mode = va_arg(list, int);
        va_end(list);
    }
    if (!strncmp(path, "/dev/dri/", 9)) {
        out("  open %s %x\n", path, flags);
        if (atoi(path + 16) < int(test_option("node", 128))) {
            errno = ENOENT;
            return -1;
        }
        path = "/dev/null";
    }
    return syscall(SYS_openat, AT_FDCWD, path, flags, mode);
}

// ---- the functions

struct Table
{
    reef_ui_video_decoder* (*create)(uint32_t, const void*, size_t, int*, int*, int*);
    int (*decode)(reef_ui_video_decoder*, const void*, size_t, reef_ui_video_frame**);
    void (*destroy)(reef_ui_video_decoder*);
    int (*copy)(reef_ui_video_frame*, void* const*, const size_t*, int, int);
    int (*exportFrame)(reef_ui_video_frame*, int*, int*, uint32_t*, uint32_t*, uint32_t*, uint64_t*);
    reef_ui_video_frame* (*retain)(reef_ui_video_frame*);
    void (*release)(reef_ui_video_frame*);
    int (*supports)(uint32_t);
    void (*size)(reef_ui_video_frame*, int*, int*);
};

static Table table()
{
#ifdef ORIGINAL
    void* library = dlopen("/o/lib/cutefish-reef/host/libreef-appkit-host.so", RTLD_NOW);
    if (!library) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    auto interface = reinterpret_cast<void* const* (*)(uint32_t)>(dlsym(library, "reef_appkit_host_interface"));
    void* const* t = interface(0x18);
    const auto at = [t](int index, auto& f) { f = reinterpret_cast<std::remove_reference_t<decltype(f)>>(t[index + 1]); };
    Table r;
    at(240, r.create);
    at(241, r.decode);
    at(242, r.destroy);
    at(243, r.copy);
    at(244, r.exportFrame);
    at(245, r.retain);
    at(246, r.release);
    at(248, r.supports);
    at(306, r.size);
    return r;
#else
    using namespace cutefish::reef;
    return {videoDecoderCreate, videoDecoderDecode, videoDecoderDestroy, videoFrameCopy, videoFrameExport,
            videoFrameRetain, videoFrameRelease, videoDecoderSupports, videoFrameSize};
#endif
}

static Table t;

#ifdef COVERAGE
extern "C" void __gcov_dump();
#else
static void __gcov_dump() {}
#endif

// ---- cases

using Bytes = std::vector<uint8_t>;
struct Case
{
    std::string name;
    uint32_t codec = 0;
    int width = 0;
    int height = 0;
    Bytes config;
    std::vector<Bytes> samples;
};

static Case load(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f) {
        perror(path);
        exit(1);
    }
    Case c;
    c.name = strrchr(path, '/') ? strrchr(path, '/') + 1 : path;
    char magic[4];
    uint32_t n = 0;
    auto get = [f](void* p, size_t size) {
        if (size && fread(p, size, 1, f) != 1)
            abort();
    };
    get(magic, 4);
    get(&c.codec, 4);
    get(&c.width, 4);
    get(&c.height, 4);
    get(&n, 4);
    c.config.resize(n);
    get(c.config.data(), n);
    get(&n, 4);
    c.samples.resize(n);
    for (Bytes& s : c.samples) {
        get(&n, 4);
        s.resize(n);
        get(s.data(), n);
    }
    fclose(f);
    return c;
}

static uint64_t hash(const void* data, size_t n)
{
    uint64_t h = 0xcbf29ce484222325ull;
    const uint8_t* p = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < n; i++)
        h = (h ^ p[i]) * 0x100000001b3ull;
    return h;
}

static int keep = 3;
static int drop = 0;
static int every = 1;

static int openFiles()
{
    int n = 0;
    DIR* d = opendir("/proc/self/fd");
    while (readdir(d))
        n++;
    closedir(d);
    return n;
}

static void look(reef_ui_video_frame* frame, int number)
{
    int width = -1, height = -1;
    t.size(frame, &width, &height);
    out(" frame %d x %d\n", width, height);
    if (number % every)
        return;
    // Copies of various sizes, with more and less room than there is.
    const int widths[] = {width, width / 2 + 1, width + 40, 5};
    const int heights[] = {height, height / 2 + 1, height + 40, 3};
    const int w = widths[number / every % 4];
    const int h = heights[number / every % 3];
    if (w < 1 || h < 1 || width * int64_t(height) > 4000000)
        return;
    const size_t strides[2] = {size_t(std::max(w, 16)) * (number % 5 == 1 ? 2 : 1) + (number % 7), size_t(std::max(w, 16)) * 2 + 2};
    Bytes first(strides[0] * (std::max(h, 1) + 2) + 64, 0xaa), second(strides[1] * (std::max(h, 1) + 2) + 64, 0x55);
    void* planes[2] = {first.data(), second.data()};
    int r = t.copy(frame, planes, strides, w, h);
    out(" copy %d x %d: %d %016llx %016llx\n", w, h, r, (unsigned long long)hash(first.data(), first.size()),
        (unsigned long long)hash(second.data(), second.size()));
    int count = -7;
    int fds[4] = {-5, -5, -5, -5};
    uint32_t formats[4] = {1, 1, 1, 1}, offsets[4] = {2, 2, 2, 2}, pitches[4] = {3, 3, 3, 3};
    uint64_t modifiers[4] = {4, 4, 4, 4};
    r = t.exportFrame(frame, &count, fds, formats, offsets, pitches, modifiers);
    out(" export: %d count %d\n", r, count);
    for (int i = 0; i < 4; i++) {
        out("  %x %x %x %llx", formats[i], offsets[i], pitches[i], (unsigned long long)modifiers[i]);
        if (fds[i] >= 0) {
            char text[64] = {};
            const ssize_t n = pread(fds[i], text, sizeof(text) - 1, 0);
            out(" fd %s '%s' %s\n", n >= 0 ? "ok" : "bad", text, fcntl(fds[i], F_GETFD) & FD_CLOEXEC ? "cloexec" : "no cloexec");
            close(fds[i]);
        } else {
            out(" %d\n", fds[i]);
        }
    }
}

static void run(const Case& c)
{
    try {
        int width = c.width, height = c.height, depth = -1;
        out("create %x %zu\n", c.codec, c.config.size());
        const int before = openFiles();
        reef_ui_video_decoder* decoder = t.create(c.codec, c.config.data(), c.config.size(), &width, &height, &depth);
        out(" %s %d %d %d\n", decoder ? "decoder" : "none", width, height, depth);
        if (!decoder) {
            out("files %d\n", openFiles() - before);
            return;
        }
        std::vector<reef_ui_video_frame*> frames;
        int number = 0;
        for (size_t i = 0; i < c.samples.size(); i++) {
            if (drop && int(i) % drop == drop - 1)
                continue;
            const Bytes& s = c.samples[i];
            out("decode %zu: %zu\n", i, s.size());
            reef_ui_video_frame* frame = reinterpret_cast<reef_ui_video_frame*>(1);
            int r = 0;
            try {
                // The sample at the end of a page of its own, reading too much is noticed.
                r = t.decode(decoder, s.data(), s.size(), i % 23 == 22 ? nullptr : &frame);
            } catch (const std::exception& e) {
                out(" exception %s\n", e.what());
                continue;
            }
            out(" %d %s\n", r, frame ? frame == reinterpret_cast<reef_ui_video_frame*>(1) ? "untouched" : "frame" : "none");
            if (frame && frame != reinterpret_cast<reef_ui_video_frame*>(1)) {
                look(frame, number);
                if (number % 3 == 0) {
                    out(" retain %d\n", t.retain(frame) == frame);
                    t.release(frame);
                }
                frames.push_back(frame);
                number++;
            }
            while (int(frames.size()) > keep) {
                const size_t k = number % 5 == 2 ? frames.size() - 1 : 0;
                t.release(frames[k]);
                frames.erase(frames.begin() + k);
            }
        }
        out("destroy\n");
        t.destroy(decoder);
        for (reef_ui_video_frame* frame : frames) {
            look(frame, 0);
            t.release(frame);
        }
        out("files %d\n", openFiles() - before);
    } catch (const std::exception& e) {
        out("exception %s\n", e.what());
    }
}

// ---- changes

static void change(Bytes& data, uint64_t& state, size_t limit)
{
    if (data.empty())
        return;
    const size_t n = std::min(data.size(), limit);
    switch (randomNumber(state) % 8) {
    case 0:
        data.resize(randomNumber(state) % data.size());
        break;
    case 1:
        data[randomNumber(state) % n] = randomNumber(state);
        break;
    case 2:
        data.insert(data.begin() + randomNumber(state) % n, uint8_t(randomNumber(state)));
        break;
    case 3:
        data.erase(data.begin() + randomNumber(state) % n);
        break;
    default: {
        int flips = 1 + randomNumber(state) % 3;
        while (flips--)
            data[randomNumber(state) % n] ^= 1 << randomNumber(state) % 8;
    }
    }
}

// The same for units with lengths in front: what is after the length changes, the length is kept right.
static void changeUnits(Bytes& data, uint64_t& state)
{
    size_t offset = 0;
    Bytes result;
    while (offset + 4 <= data.size()) {
        const size_t length = size_t(data[offset]) << 24 | data[offset + 1] << 16 | data[offset + 2] << 8 | data[offset + 3];
        offset += 4;
        if (length > data.size() - offset)
            return;
        Bytes unit(data.begin() + offset, data.begin() + offset + length);
        offset += length;
        if (randomNumber(state) % 2 && unit.size() > 1) {
            int flips = 1 + randomNumber(state) % 2;
            const size_t n = std::min<size_t>(unit.size(), 6 + randomNumber(state) % 24);
            while (flips--)
                unit[randomNumber(state) % n] ^= 1 << randomNumber(state) % 8;
        }
        for (int shift = 24; shift >= 0; shift -= 8)
            result.push_back(unit.size() >> shift);
        result.insert(result.end(), unit.begin(), unit.end());
    }
    data = result;
}

static Case fuzzed(const Case& original, uint64_t seed)
{
    uint64_t state = seed * 0x9e3779b97f4a7c15ull + 0x1234567;
    for (int i = 0; i < 8; i++)
        randomNumber(state);
    Case c = original;
    // A part of the stream is enough.
    const size_t length = std::min<size_t>(c.samples.size(), 6 + randomNumber(state) % 12);
    c.samples.resize(length);
    const uint32_t kind = randomNumber(state) % 8;
    if (kind < 3)
        change(c.config, state, c.config.size());
    if (kind == 2 || kind >= 3) {
        for (Bytes& s : c.samples) {
            if (randomNumber(state) % 3)
                continue;
            if (c.codec == 0x76703039 || randomNumber(state) % 4 == 0)
                change(s, state, 40);
            else
                changeUnits(s, state);
        }
    }
    if (kind == 7) {
        // Other parameter sets than those of the configuration in between, and frames in another order.
        for (size_t i = 0; i + 1 < c.samples.size(); i++) {
            if (randomNumber(state) % 3 == 0)
                std::swap(c.samples[i], c.samples[i + 1]);
        }
    }
    failRate = randomNumber(state) % 4 == 0 ? 20 + randomNumber(state) % 200 : 0;
    failSeed = state | 1;
    if (randomNumber(state) % 16 == 0)
        options["layers"] = randomNumber(state) % 6;
    if (randomNumber(state) % 16 == 0)
        options["objects"] = 1 + randomNumber(state) % 4;
    if (randomNumber(state) % 16 == 0)
        options["formats"] = randomNumber(state) % 16;
    if (randomNumber(state) % 16 == 0)
        options["profiles"] = randomNumber(state);
    if (randomNumber(state) % 16 == 0)
        options["rtformat"] = randomNumber(state) % 2 ? 1 : 0x100;
    if (randomNumber(state) % 16 == 0)
        options["node"] = 128 + randomNumber(state) % 10;
    if (randomNumber(state) % 16 == 0)
        options["planes"] = 1 + randomNumber(state) % 3;
    if (randomNumber(state) % 8 == 0)
        c.width = int(randomNumber(state) % 700) - 20;
    if (randomNumber(state) % 16 == 0)
        c.height = int(randomNumber(state) % 700) - 20;
    keep = randomNumber(state) % 6;
    every = 4;
    return c;
}

static std::string child(const Case& c)
{
    shared->length = 0;
    const pid_t pid = fork();
    if (pid == 0) {
        const int null = syscall(SYS_openat, AT_FDCWD, "/dev/null", O_RDWR);
        dup2(null, 2);
        alarm(60);
#ifndef SANITIZE
        const rlimit limit = {1500u << 20, 1500u << 20};
        setrlimit(RLIMIT_AS, &limit);
#endif
        run(c);
        __gcov_dump();
        _exit(0);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFSIGNALED(status))
        return "signal " + std::to_string(WTERMSIG(status));
    return "exit " + std::to_string(WEXITSTATUS(status));
}

int main(int argc, char** argv)
{
    shared = static_cast<Shared*>(mmap(nullptr, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    FILE* log = fopen(argv[1], "w");
    bool full = false, fuzz = false;
    uint64_t first = 0, count = 0;
    std::vector<Case> cases;
    std::map<std::string, uint32_t> set;
    for (int i = 2; i < argc; i++) {
        const std::string a = argv[i];
        if (a == "--keep")
            keep = atoi(argv[++i]);
        else if (a == "--drop")
            drop = atoi(argv[++i]);
        else if (a == "--every")
            every = atoi(argv[++i]);
        else if (a == "--full")
            full = true;
        else if (a == "--fuzz") {
            fuzz = true;
            first = strtoull(argv[++i], nullptr, 0);
            count = strtoull(argv[++i], nullptr, 0);
        } else if (a == "--option") {
            const std::string o = argv[++i];
            set[o.substr(0, o.find('='))] = strtoul(o.c_str() + o.find('=') + 1, nullptr, 0);
        } else if (a == "--fail") {
            const std::string o = argv[++i];
            failName = o.substr(0, o.find(':'));
            failAt = atoi(o.c_str() + o.find(':') + 1);
        } else {
            cases.push_back(load(argv[i]));
        }
    }
    options = set;
    t = table();
    if (!fuzz) {
        // 248 and the arguments that are not there.
        Case none;
        shared->length = 0;
        const pid_t pid = fork();
        if (pid == 0) {
            for (uint32_t codec : {0x61766331u, 0x68766331u, 0x76703039u, 0u, 0x61763031u})
                out("supports %x: %d\n", codec, t.supports(codec));
            out("%d\n", t.create(0x61766331, nullptr, 10, nullptr, nullptr, nullptr) != nullptr);
            t.destroy(nullptr);
            t.release(nullptr);
            out("%d\n", t.retain(nullptr) != nullptr);
            int width = 5, height = 6;
            t.size(nullptr, &width, &height);
            out("%d %d\n", width, height);
            t.size(nullptr, nullptr, nullptr);
            reef_ui_video_frame* frame = reinterpret_cast<reef_ui_video_frame*>(1);
            out("%d", t.decode(nullptr, "a", 1, &frame));
            out(" %d\n", frame != nullptr);
            out("%d\n", t.decode(nullptr, "a", 1, nullptr));
            out("%d\n", t.copy(nullptr, nullptr, nullptr, 1, 1));
            out("%d\n", t.exportFrame(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
            __gcov_dump();
            _exit(0);
        }
        waitpid(pid, nullptr, 0);
        fwrite(shared->text, shared->length, 1, log);
        for (const Case& c : cases) {
            const std::string result = child(c);
            fprintf(log, "==== %s\n", c.name.c_str());
            fwrite(shared->text, shared->length, 1, log);
            fprintf(log, "%s\n", result.c_str());
        }
    } else {
        const int keptKeep = keep;
        for (uint64_t i = first; i < first + count; i++) {
            options = set;
            keep = keptKeep;
            const Case c = fuzzed(cases[i % cases.size()], i);
            const std::string result = child(c);
            size_t lines = std::count(shared->text, shared->text + shared->length, '\n');
            if (full) {
                fprintf(log, "==== %llu %s\n", (unsigned long long)i, c.name.c_str());
                fwrite(shared->text, shared->length, 1, log);
                fprintf(log, "%s\n", result.c_str());
            } else {
                // How far it came, for the coverage of the cases.
                size_t pictures = 0;
                for (const char* p = shared->text; p && p < shared->text + shared->length;) {
                    p = static_cast<const char*>(memmem(p, shared->text + shared->length - p, "vaEndPicture", 12));
                    if (p) {
                        pictures++;
                        p += 12;
                    }
                }
                fprintf(log, "%llu %s %s %zu lines %zu pictures %016llx\n", (unsigned long long)i, c.name.c_str(), result.c_str(), lines,
                        pictures, (unsigned long long)hash(shared->text, shared->length));
            }
        }
    }
    fclose(log);
}
