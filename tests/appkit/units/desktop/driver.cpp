// driver LIBRARY [nocallbacks]: drives the host table of the library with commands of stdin, a line each, fields separated by
// tabs ("~" is null). What the functions return and the callbacks get goes to stdout.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dlfcn.h>
#include <exception>
#include <functional>
#include <poll.h>
#include <string>
#include <unistd.h>
#include <vector>

// The waits of the library, which come here: the driver exports its symbols.
static int waits = 0;
static timespec waited = {};
extern "C" int nanosleep(const timespec* time, timespec* rest)
{
    waits++;
    waited = *time;
    return clock_nanosleep(CLOCK_MONOTONIC, 0, time, rest);
}

struct Callbacks
{
    void* slots[21];
};

struct NowPlayingState
{
    const char *application, *title, *artist, *album, *artwork;
    double duration, position, rate;
    int status;
    uint32_t commands;
    uint64_t track;
};

static void* const* table;
template<typename F> static F* fn(int index) { return reinterpret_cast<F*>(table[index]); }

static void print(const char* text)
{
    if (!text) {
        printf("~");
        return;
    }
    for (const char* p = text; *p; p++) {
        if (*p == '\n') printf("\\n");
        else if (*p == '\\') printf("\\\\");
        else putchar(*p);
    }
}

static void launched(int count, const char* const* items, const char* token)
{
    printf("launched count=%d", count);
    for (int i = 0; i < count; i++) { printf(" ["); print(items[i]); printf("]"); }
    printf(" token=["); print(token); printf("] items %s\n", items ? "set" : "null");
}
static void quit() { printf("quit\n"); }
static void terminate_() { printf("terminate\n"); }
static void command(int command, double value) { printf("command %d %.6f\n", command, value); }

static std::vector<std::string> split(const std::string& line)
{
    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        const size_t at = line.find('\t', start);
        parts.push_back(line.substr(start, at == std::string::npos ? at : at - start));
        if (at == std::string::npos)
            return parts;
        start = at + 1;
    }
}

static const char* text(const std::vector<std::string>& parts, size_t index)
{
    return index < parts.size() && parts[index] != "~" ? parts[index].c_str() : nullptr;
}
static int number(const std::vector<std::string>& parts, size_t index) { return index < parts.size() ? atoi(parts[index].c_str()) : 0; }

static std::vector<const char*> rest(const std::vector<std::string>& parts, size_t index)
{
    std::vector<const char*> list;
    for (size_t i = index; i < parts.size(); i++)
        list.push_back(text(parts, i));
    return list;
}

// A buffer with a pattern around it: what was written shows.

static void buffered(int size, bool null, const std::function<int(char*, int)>& call)
{
    char buffer[600];
    memset(buffer, '#', sizeof buffer);
    const int result = call(null ? nullptr : buffer + 8, size);
    printf("%d ", result);
    int touched = 0;
    for (int i = 0; i < int(sizeof buffer); i++) {
        if (buffer[i] != '#')
            touched = i + 1;
    }
    for (int i = 0; i < touched; i++) {
        if (buffer[i] == 0) printf("\\0");
        else putchar(buffer[i]);
    }
    printf("\n");
}

static bool initialized = false;

static bool run(const std::string& line, bool callbacksNull)
{
    const std::vector<std::string> p = split(line);
    const std::string& c = p[0];
    if (c == "init") {
        static Callbacks callbacks = {};
        if (!callbacksNull) {
            callbacks.slots[10] = (void*)terminate_;
            callbacks.slots[13] = (void*)launched;
            callbacks.slots[14] = (void*)quit;
        }
        const std::vector<const char*> arguments = rest(p, 2);
        printf("init %d\n", fn<int(int, const char* const*, const char*, const Callbacks*)>(0)(int(arguments.size()), arguments.data(), text(p, 1), &callbacks));
        initialized = true;
    } else if (c == "type") {
        printf("type ");
        buffered(number(p, 2), p.size() > 3, [&](char* b, int s) { return fn<int(const char*, char*, int)>(84)(text(p, 1), b, s); });
    } else if (c == "icon") {
        printf("icon ");
        buffered(number(p, 3), p.size() > 4, [&](char* b, int s) { return fn<int(const char*, int, char*, int)>(85)(text(p, 1), number(p, 2), b, s); });
    } else if (c == "default") {
        printf("default ");
        buffered(number(p, 2), p.size() > 3, [&](char* b, int s) { return fn<int(const char*, char*, int)>(86)(text(p, 1), b, s); });
    } else if (c == "open") {
        const std::vector<const char*> arguments = rest(p, 3);
        try {
            printf("open %d\n", fn<int(const char*, const char* const*, int)>(87)(text(p, 1), arguments.data(), number(p, 2)));
        } catch (const std::exception& e) {
            printf("open threw %s\n", e.what());
        }
    } else if (c == "show") {
        const std::vector<const char*> paths = rest(p, 3);
        const int count = number(p, 2);
        printf("show %d\n", fn<int(const char* const*, unsigned, int)>(201)(count < 0 ? nullptr : paths.data(), unsigned(count < 0 ? -count : count), number(p, 1)));
    } else if (c == "claim") {
        const std::vector<const char*> items = rest(p, 4);
        waits = 0;
        waited = {};
        const int result = fn<int(const char*, int, int, const char* const*)>(213)(text(p, 1), number(p, 2), number(p, 3), items.data());
        printf("claim %d waits %d of %ld.%09ld\n", result, waits, long(waited.tv_sec), long(waited.tv_nsec));
    } else if (c == "dock") {
        fn<void(int)>(215)(number(p, 1));
        printf("dock\n");
    } else if (c == "token") {
        fn<void(const char*)>(234)(text(p, 1));
        printf("token\n");
    } else if (c == "playing") {
        if (p.size() < 12) {
            fn<void(const NowPlayingState*, void (*)(int, double))>(305)(nullptr, command);
        } else {
            NowPlayingState state = {text(p, 1), text(p, 2), text(p, 3), text(p, 4), text(p, 5), atof(p[6].c_str()), atof(p[7].c_str()), atof(p[8].c_str()),
                                     number(p, 9), unsigned(number(p, 10)), uint64_t(number(p, 11))};
            fn<void(const NowPlayingState*, void (*)(int, double))>(305)(&state, command);
        }
        printf("playing\n");
    } else if (c == "sleep") {
        usleep(useconds_t(number(p, 1)) * 1000);
    } else if (c == "end") {
        printf("end\n");
        return false;
    } else {
        printf("unknown %s\n", c.c_str());
    }
    return true;
}

int main(int argc, char** argv)
{
    setvbuf(stdout, nullptr, _IOLBF, 1 << 16);
    void* library = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (!library) {
        fprintf(stderr, "%s\n", dlerror());
        return 2;
    }
    const auto interface = reinterpret_cast<const void* (*)(unsigned)>(dlsym(library, "reef_appkit_host_interface"));
    const uint64_t* words = static_cast<const uint64_t*>(interface(0x18));
    if (!words) {
        fprintf(stderr, "no interface\n");
        return 2;
    }
    table = reinterpret_cast<void* const*>(words + 1);
    const bool callbacksNull = argc > 2;
    std::string input;
    bool open = true;
    while (open) {
        pollfd polled[2] = {{0, POLLIN, 0}, {initialized ? fn<int()>(1)() : -1, POLLIN, 0}};
        if (poll(polled, initialized ? 2 : 1, -1) < 0)
            continue;
        if (initialized && polled[1].revents)
            fn<void()>(2)();
        if (polled[0].revents) {
            char buffer[4096];
            const ssize_t length = read(0, buffer, sizeof buffer);
            if (length <= 0)
                break;
            input.append(buffer, size_t(length));
            size_t at;
            while (open && (at = input.find('\n')) != std::string::npos) {
                const std::string line = input.substr(0, at);
                input.erase(0, at + 1);
                open = run(line, callbacksNull);
                if (initialized)
                    fn<void()>(2)();
            }
        }
    }
    if (getenv("COVERAGE"))
        exit(0);
    fflush(stdout);
    _exit(0);
}
