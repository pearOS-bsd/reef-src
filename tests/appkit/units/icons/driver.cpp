// driver DIR GROUP... : runs the functions of the original library (DIR/orig.so) and ours (DIR/ours.so) on the same inputs
// and compares what they do. Both find the same DIR/symbols. See run.sh.
#include "graphics.h"
#include "images.h"
#include "ui/canvas_2d.h"
#include "ui/icon.h"
#include "ui/theme.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <set>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace cutefish;

static void** table;
static void* (*oursFunction)(int);
// 0 the original's, 1 ours.
template <typename R, typename... A>
static R call(int side, int index, A... a)
{
    void* f = side == 0 ? table[1 + index] : oursFunction(index);
    return reinterpret_cast<R (*)(A...)>(f)(a...);
}
// Functions of other units: the original's.
template <typename R, typename... A>
static R orig(int index, A... a)
{
    return reinterpret_cast<R (*)(A...)>(table[1 + index])(a...);
}

static unsigned long checks, failures, interesting;
static std::string context;
static void fail(const char* what, const std::string& detail = {})
{
    failures++;
    if (failures < 60)
        printf("DIFFERENT %s: %s %s\n", what, context.c_str(), detail.c_str());
}
static void check(const char* what, long a, long b)
{
    checks++;
    if (a != b)
        fail(what, std::to_string(a) + " " + std::to_string(b));
}
static void check(const char* what, const std::string& a, const std::string& b)
{
    checks++;
    if (a != b)
        fail(what, std::to_string(a.size()) + " " + std::to_string(b.size()) + " bytes");
}

static uint32_t seed = 1;
static uint32_t rnd(uint32_t n)
{
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) % n;
}
static double rndf(double lo, double hi) { return lo + (hi - lo) * (rnd(10000) / 9999.0); }

// The next malloc of this size fails (malloc.cpp).
extern "C" __attribute__((weak)) void failMalloc(size_t size);

// Pixels that a function gave: size, what is in them.
struct Got
{
    void* pixels = nullptr;
    int width = -7, height = -9;
    std::string bytes;
};
static void compare(const char* what, Got g[2])
{
    for (int side = 0; side < 2; side++) {
        if (g[side].pixels)
            g[side].bytes.assign((const char*)g[side].pixels, size_t(g[side].width) * 4 * size_t(g[side].height));
        call<void>(side, 83, g[side].pixels);
    }
    check(what, g[0].pixels != nullptr, g[1].pixels != nullptr);
    check(what, g[0].width, g[1].width);
    check(what, g[0].height, g[1].height);
    check(what, g[0].bytes, g[1].bytes);
    bool ink = false;
    for (char c : g[0].bytes)
        ink |= c != 0;
    interesting += ink;
}

static std::string dir;

// --- images that the GPU keeps

struct Canvas
{
    int width, height;
    std::vector<uint32_t> pixels;
    reef_ui_gc* gc;
    Canvas(int width, int height)
        : width(width), height(height), pixels(width * height, 0xff8090a0)
    {
        gc = orig<reef_ui_gc*>(69, (void*)pixels.data(), width, height, size_t(width * 4), 0);
    }
    ~Canvas() { orig<void>(70, gc); }
};

static std::vector<uint32_t> randomPixels(int width, int height, int stride)
{
    std::vector<uint32_t> p(size_t(stride) / 4 * std::max(height, 1));
    for (uint32_t& v : p) {
        const uint32_t a = rnd(3) ? 255 : rnd(256);
        v = a << 24 | (rnd(a + 1) << 16) | (rnd(a + 1) << 8) | rnd(a + 1);
    }
    return p;
}

static void testImages()
{
    for (int round = 0; round < 400; round++) {
        const int width = round % 40 == 0 ? int(rnd(3)) - 1 : 1 + rnd(40), height = round % 40 == 1 ? int(rnd(3)) - 1 : 1 + rnd(40);
        int stride = std::max(width, 1) * 4 + rnd(3) * 4;
        const std::vector<uint32_t> pixels = randomPixels(width, height, stride), other = randomPixels(width, height, stride);
        if (round % 23 == 3)
            stride = std::max(0, width * 4 - 1 - int(rnd(5)));
        context = "image " + std::to_string(width) + "x" + std::to_string(height) + " stride " + std::to_string(stride);
        const bool noPixels = round % 50 == 7;
        reef_ui_image* image[2];
        for (int side = 0; side < 2; side++)
            image[side] = call<reef_ui_image*>(side, 63, noPixels ? nullptr : (const void*)pixels.data(), width, height, size_t(stride));
        check("create", image[0] != nullptr, image[1] != nullptr);
        if (!image[0] != !image[1])
            continue;
        if (image[0]) {
            check("width", image[0]->bitmap.width(), image[1]->bitmap.width());
            check("height", image[0]->bitmap.height(), image[1]->bitmap.height());
            check("bytes", image[0]->bitmap.byteCount(), image[1]->bitmap.byteCount());
        }
        const double x = rndf(-10, 60), y = rndf(-10, 60), w = rndf(-5, 80), h = rndf(-5, 80);
        const int mirrored = rnd(3);
        const int ux = int(rnd(width + 6)) - 3, uy = int(rnd(height + 6)) - 3, uw = int(rnd(width + 6)) - 2, uh = int(rnd(height + 6)) - 2;
        for (int pass = 0; pass < 3; pass++) {
            if (pass == 1 && image[0]) {
                for (int side = 0; side < 2; side++)
                    call<void>(side, 257, image[side], (const void*)other.data(), size_t(stride), ux, uy, uw, uh);
            }
            Canvas a(90, 90), b(90, 90);
            Canvas* c[2] = {&a, &b};
            int r[2];
            for (int side = 0; side < 2; side++) {
                if (pass == 2)
                    orig<void>(46, c[side]->gc, 0);
                r[side] = call<int>(side, 65, c[side]->gc, image[side], x, y, w, h, mirrored);
            }
            check("draw result", r[0], r[1]);
            check("draw result known", r[0], image[0] ? 1 : 0);
            checks++;
            if (a.pixels != b.pixels)
                fail("drawn");
            interesting += std::any_of(a.pixels.begin(), a.pixels.end(), [](uint32_t p) { return p != 0xff8090a0; });
        }
        // Into a PDF document nothing.
        if (round % 20 == 0) {
            reef_ui_pdf_document document = {};
            document.write = [](void*, const void*, size_t) {};
            reef_ui_gc* pdf = orig<reef_ui_gc*>(168, (const reef_ui_pdf_document*)&document);
            for (int side = 0; side < 2; side++) {
                check("pdf", call<int>(side, 65, pdf, image[side], x, y, w, h, mirrored), 0);
                check("pdf dmabuf", call<int>(side, 239, pdf, (reef_ui_dma_buf*)nullptr, x, y, w, h, mirrored), 0);
            }
            orig<void>(70, pdf);
        }
        for (int side = 0; side < 2; side++)
            call<void>(side, 64, image[side]);
    }
    for (int side = 0; side < 2; side++)
        call<void>(side, 64, (reef_ui_image*)nullptr);
}

// --- DMA-BUFs (there is no GPU: they are only made)

static int descriptors()
{
    int n = 0;
    DIR* d = opendir("/proc/self/fd");
    while (readdir(d))
        n++;
    closedir(d);
    return n;
}

static bool open(int fd) { return fcntl(fd, F_GETFD) >= 0; }

static void testDmaBuf()
{
    for (int round = 0; round < 300; round++) {
        const int width = int(rnd(70)) - 3, height = int(rnd(70)) - 3, count = int(rnd(7)) - 1;
        const uint32_t fourcc = rnd(2) ? 0x34325241 : rnd(2) ? 0x34325258 : rnd(10000);
        const uint64_t modifier = rnd(2) ? 0 : rnd(2) ? 0x00ffffffffffffffull : rnd(1000);
        int fds[8];
        uint32_t offsets[8], strides[8];
        for (int i = 0; i < 8; i++) {
            fds[i] = rnd(9) ? memfd_create("plane", 0) : -1;
            if (fds[i] >= 0)
                ftruncate(fds[i], 65536);
            offsets[i] = rnd(100);
            strides[i] = rnd(1000);
        }
        context = "dmabuf " + std::to_string(width) + "x" + std::to_string(height) + " planes " + std::to_string(count);
        int opened[2][4] = {};
        for (int side = 0; side < 2; side++) {
            const int before = descriptors();
            reef_ui_dma_buf* image = call<reef_ui_dma_buf*>(side, 237, width, height, fourcc, modifier, count, (const int*)fds,
                                                            (const uint32_t*)offsets, (const uint32_t*)strides);
            opened[side][0] = image != nullptr;
            opened[side][1] = descriptors() - before;
            if (image) {
                opened[side][2] = image->image.width();
                opened[side][3] = image->image.height();
                interesting += side == 0;
            }
            Canvas c(80, 80);
            const std::vector<uint32_t> was = c.pixels;
            check("not drawn in software", call<int>(side, 239, c.gc, image, 0.0, 0.0, 40.0, 40.0, int(rnd(2))), 0);
            checks++;
            if (c.pixels != was)
                fail("drawn in software");
            // Fences: the last one is closed with the image, and at once without.
            const int fence = memfd_create("fence", 0), fence2 = memfd_create("fence", 0);
            call<void>(side, 308, image, fence);
            const bool kept = open(fence);
            check("fence", kept, image != nullptr);
            call<void>(side, 308, image, fence2);
            check("fence replaced", open(fence), 0);
            check("second fence", open(fence2), image != nullptr);
            call<void>(side, 308, image, -1);
            call<void>(side, 308, (reef_ui_dma_buf*)nullptr, -1);
            call<void>(side, 238, image);
            check("fence closed", open(fence2), 0);
            check("descriptors", descriptors() - before, 0);
        }
        for (int i = 0; i < 4; i++)
            check("made", opened[0][i], opened[1][i]);
        for (int fd : fds) {
            if (fd >= 0)
                close(fd);
        }
    }
}

// --- decoding and encoding

static void decodeBoth(const char* what, const char* path, const std::string* bytes)
{
    Got g[2];
    bool threw[2] = {};
    for (int side = 0; side < 2; side++) {
        // Reading a folder throws.
        try {
            g[side].pixels = call<void*>(side, 81, path, bytes ? (const void*)bytes->data() : nullptr, bytes ? bytes->size() : size_t(0),
                                         &g[side].width, &g[side].height);
        } catch (const std::exception&) {
            threw[side] = true;
        }
    }
    check("threw", threw[0], threw[1]);
    compare(what, g);
}

static std::string readFile(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(f)), {});
}

static void writeFile(const std::string& path, const std::string& text)
{
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream(path, std::ios::binary) << text;
}

static void testDecode()
{
    std::vector<std::string> files = {"/no/such/file", "/", "/tmp", "", "/dev/null", "/proc/self/cmdline"};
    for (const char* d : {"/tk/app/media/test/images", "/tmp/icons/test/pictures"}) {
        std::error_code error;
        for (const auto& e : std::filesystem::directory_iterator(d, error))
            files.push_back(e.path());
    }
    for (int i = 0; i < 40; i++)
        files.push_back(dir + "/symbols/" + std::vector<std::string>{"house", "star", "folder", "x", "a-arrow-down"}[i % 5] + ".svg");
    std::sort(files.begin(), files.end());
    for (const std::string& file : files) {
        context = file;
        decodeBoth("decode file", file.c_str(), nullptr);
        if (std::filesystem::is_directory(file) || file.starts_with("/dev"))
            continue;
        const std::string bytes = readFile(file);
        decodeBoth("decode bytes", nullptr, &bytes);
        // A path goes first.
        decodeBoth("decode path and bytes", file.c_str(), &bytes);
        decodeBoth("decode other path and bytes", "/no/such", &bytes);
        // Damaged and cut.
        for (int i = 0; i < 12 && !bytes.empty(); i++) {
            std::string bad = bytes;
            if (i % 3 == 0)
                bad.resize(rnd(bad.size()) + (i == 0));
            else
                for (int k = 0; k < 1 + int(rnd(4)); k++)
                    bad[rnd(i % 3 == 1 ? std::min<size_t>(bad.size(), 64) : bad.size())] ^= 1 << rnd(8);
            context = file + " damaged " + std::to_string(i);
            decodeBoth("decode damaged", nullptr, &bad);
            const std::string path = dir + "/damaged";
            writeFile(path, bad);
            decodeBoth("decode damaged file", path.c_str(), nullptr);
        }
    }
    context = "none";
    decodeBoth("none", nullptr, nullptr);
    std::string empty;
    decodeBoth("empty", nullptr, &empty);
    // Bytes without a size, a size without bytes.
    Got g[2];
    for (int side = 0; side < 2; side++)
        g[side].pixels = call<void*>(side, 81, (const char*)nullptr, (const void*)nullptr, size_t(10), &g[side].width, &g[side].height);
    compare("size alone", g);
}

static void testEncode()
{
    for (int round = 0; round < 300; round++) {
        const int width = round % 10 == 0 ? int(rnd(4)) - 2 : 1 + rnd(60), height = round % 10 == 1 ? int(rnd(4)) - 2 : 1 + rnd(60);
        int stride = std::max(width, 1) * 4 + (rnd(2) ? rnd(5) * 4 : 0);
        const std::vector<uint32_t> pixels = randomPixels(width, height, stride);
        // Too short rows are none.
        if (round % 23 == 3)
            stride = std::max(0, width * 4 - 1 - int(rnd(5)));
        context = "encode " + std::to_string(width) + "x" + std::to_string(height);
        std::string bytes[2];
        size_t sizes[2] = {77, 77};
        bool made[2];
        const bool noPixels = round % 30 == 5, noSize = round % 30 == 6, noMemory = round % 5 == 4 && failMalloc;
        size_t expected = 0;
        for (int side = 0; side < 2; side++) {
            if (noMemory && expected)
                failMalloc(expected);
            void* png = call<void*>(side, 120, noPixels ? nullptr : (const void*)pixels.data(), width, height, size_t(stride),
                                    noSize ? nullptr : &sizes[side]);
            made[side] = png;
            if (png)
                bytes[side].assign((char*)png, sizes[side]);
            call<void>(side, 83, png);
            if (failMalloc)
                failMalloc(0);
            if (noMemory && side == 0 && png) {
                // Again, now with the size known.
                expected = sizes[0];
                sizes[0] = 77;
                bytes[0].clear();
                failMalloc(expected);
                png = call<void*>(0, 120, (const void*)pixels.data(), width, height, size_t(stride), &sizes[0]);
                made[0] = png;
                if (png)
                    printf("memory did not fail\n");
                failMalloc(0);
            }
        }
        check("encoded", made[0], made[1]);
        check("encoded size", sizes[0], sizes[1]);
        check("encoded bytes", bytes[0], bytes[1]);
        interesting += made[0];
        if (made[0])
            decodeBoth("encoded and decoded", nullptr, &bytes[0]);
    }
}

// --- icons

static void testIcons()
{
    // A theme of our own, whatever the machine has.
    std::vector<std::string> names = {"folder", "only-scalable", "only-16", "only-png", "app.example.Thing", "missing", "", "a", "with space",
                                      "folder-documents-symbolic", "x-office-document", "/tmp/icons/test/pictures/rgba.png",
                                      "/no/such.png", "broken", "empty", "text-x-generic", "image-x-generic", "../folder", "only-pixmap"};
    for (int loop = 0; loop < 2; loop++) {
        for (const std::string& name : names) {
            for (int size : {-1, 0, 1, 12, 16, 22, 24, 32, 48, 64, 128, 256, 1000}) {
                context = "icon " + name + " " + std::to_string(size);
                Got g[2];
                for (int side = 0; side < 2; side++)
                    g[side].pixels = call<void*>(side, 82, name.c_str(), size, &g[side].width, &g[side].height);
                compare("icon", g);
                for (int pixels : {-1, 0, 1, 16, 33, 64, 200, 512}) {
                    context = "icon " + name + " " + std::to_string(size) + " pixels " + std::to_string(pixels);
                    Got h[2];
                    for (int side = 0; side < 2; side++)
                        h[side].pixels = call<void*>(side, 301, name.c_str(), size, pixels, &h[side].width, &h[side].height);
                    compare("scaled icon", h);
                }
            }
        }
        ui::Theme theme = ui::Theme::current();
        theme.iconTheme = loop == 0 ? "second" : "hicolor";
        ui::Theme::setCurrent(theme);
        ui::reloadThemeIcons();
    }
    Got g[2];
    context = "icon null";
    for (int side = 0; side < 2; side++)
        g[side].pixels = call<void*>(side, 82, (const char*)nullptr, 16, &g[side].width, &g[side].height);
    compare("icon null", g);
    Got h[2];
    for (int side = 0; side < 2; side++)
        h[side].pixels = call<void*>(side, 301, (const char*)nullptr, 16, 16, &h[side].width, &h[side].height);
    compare("scaled icon null", h);
    // Memory runs out: the size stays.
    if (failMalloc) {
        for (int size : {16, 48}) {
            context = "icon without memory " + std::to_string(size);
            Got k[2];
            for (int side = 0; side < 2; side++) {
                failMalloc(size * size * 4);
                k[side].pixels = call<void*>(side, 82, "only-scalable", size, &k[side].width, &k[side].height);
                failMalloc(0);
            }
            compare("without memory", k);
            check("without memory", k[0].pixels == nullptr, true);
        }
    }
}

// --- symbols

static std::vector<std::string> symbolNames()
{
    std::vector<std::string> names;
    for (const auto& e : std::filesystem::directory_iterator(dir + "/symbols")) {
        const std::string n = e.path().filename();
        if (n.ends_with(".svg"))
            names.push_back(n.substr(0, n.size() - 4));
    }
    std::sort(names.begin(), names.end());
    return names;
}

static bool sameDoubles(const double* a, const double* b, int n)
{
    return memcmp(a, b, n * sizeof *a) == 0;
}

static void boundsBoth(const char* name)
{
    double b[2][4] = {{-1, -2, -3, -4}, {-1, -2, -3, -4}};
    int r[2];
    for (int side = 0; side < 2; side++) {
        try {
            r[side] = call<int>(side, 212, name, b[side]);
        } catch (const std::exception&) {
            r[side] = -5;
        }
    }
    check("bounds", r[0], r[1]);
    checks++;
    if (!sameDoubles(b[0], b[1], 4))
        fail("bounds", std::to_string(b[0][0]) + " " + std::to_string(b[1][0]));
    interesting += r[0] == 1 && (b[0][2] != 24 || b[0][3] != 24);
}

static void symbolBoth(const char* name, int width, int height, const double* rect, double weight, int filled, int margin)
{
    Got g[2];
    bool threw[2] = {};
    for (int side = 0; side < 2; side++) {
        try {
            g[side].pixels = call<void*>(side, 210, name, width, height, rect, weight, filled, margin, &g[side].width, &g[side].height);
        } catch (const std::exception&) {
            threw[side] = true;
        }
    }
    check("threw", threw[0], threw[1]);
    compare("symbol", g);
}

static void existsBoth(const char* name)
{
    int r[2];
    for (int side = 0; side < 2; side++)
        r[side] = call<int>(side, 211, name);
    check("exists", r[0], r[1]);
}

// What the library says of its folder is made up (FAKE_DLADDR).
static void testFolder()
{
    const double whole[4] = {0, 0, 24, 24};
    for (const char* name : {"normal", "house", "dot", "no-such", "", "a/b"}) {
        context = std::string("folder ") + (getenv("FAKE_DLADDR") ? getenv("FAKE_DLADDR") : "") + " " + name;
        const int r = call<int>(0, 211, name);
        existsBoth(name);
        boundsBoth(name);
        symbolBoth(name, 24, 24, whole, 2, 0, 0);
        printf("%s: %d\n", context.c_str(), r);
    }
}

static void testSymbols(bool all)
{
    std::vector<std::string> names = symbolNames();
    printf("%zu symbols\n", names.size());
    unsigned at = 0;
    for (const std::string& name : names) {
        context = "symbol " + name;
        if (!all && at++ % 4)
            continue;
        existsBoth(name.c_str());
        boundsBoth(name.c_str());
        const double whole[4] = {0, 0, 24, 24};
        symbolBoth(name.c_str(), 24, 24, whole, 2, 0, 0);
        for (int i = 0; i < 2; i++) {
            const int width = 1 + rnd(90), height = 1 + rnd(90);
            const double rect[4] = {rndf(-5, 20), rndf(-5, 20), rndf(0.5, 80), rndf(0.5, 80)};
            const double weight = rndf(0.2, 9);
            const int filled = rnd(2), margin = rnd(7);
            context = "symbol " + name + " " + std::to_string(width) + "x" + std::to_string(height) + " " + std::to_string(rect[2]) + "x"
                + std::to_string(rect[3]) + " weight " + std::to_string(weight) + " filled " + std::to_string(filled);
            symbolBoth(name.c_str(), width, height, rect, weight, filled, margin);
        }
    }
    // Over and over: what is kept.
    for (int i = 0; i < 1500; i++) {
        const std::string& name = names[rnd(std::min<size_t>(names.size(), 40))];
        const int size = 8 + rnd(24);
        const double rect[4] = {0, 0, double(size), double(size)};
        context = "kept " + name;
        symbolBoth(name.c_str(), size, size, rect, 1.5 + rnd(3), rnd(3), rnd(2));
    }
    static const double numbers[] = {0, -0.0, 1, -1, 0.05, 0.1, 0.99999, 1.00001, 24, 1e-9, 1e9, -1e9, 1e300, NAN, INFINITY, -INFINITY, 0.1235, 0.12351, 12.3456};
    for (int i = 0; i < 1500; i++) {
        const std::string& name = names[rnd(names.size() < 400 ? names.size() : 60)];
        double rect[4];
        for (double& v : rect)
            v = rnd(3) ? rndf(0, 30) : numbers[rnd(sizeof numbers / sizeof *numbers)];
        const double weight = rnd(3) ? rndf(0, 5) : numbers[rnd(sizeof numbers / sizeof *numbers)];
        const int width = int(rnd(40)) - 3, height = int(rnd(40)) - 3, filled = int(rnd(4)) - 1, margin = rnd(4) ? rnd(6) : -int(rnd(12));
        char text[300];
        snprintf(text, sizeof text, "odd %s %d %d (%g %g %g %g) %g %d %d", name.c_str(), width, height, rect[0], rect[1], rect[2], rect[3], weight, filled, margin);
        context = text;
        symbolBoth(name.c_str(), width, height, rnd(40) ? rect : nullptr, weight, filled, margin);
    }
    // Names.
    for (const char* name : {"", "/", "a/b", "../symbols/house", "no-such-symbol", "house.svg", ".", "..", "LICENSE.lucide", " house", "house ",
                             "a-very-long-name-that-does-not-fit-anywhere-and-is-longer-than-the-file-system-takes-a-very-long-name-that-does-not-fit-anywhere-and-"
                             "is-longer-than-the-file-system-takes-a-very-long-name-that-does-not-fit-anywhere-and-is-longer-than-the-file-system-takes-a-very-long-"
                             "name-that-does-not-fit-anywhere-and-is-longer-than-the-file-system-takes-a-very-long-name-that-does-not-fit-anywhere-and-is-longer"}) {
        context = std::string("name ") + name;
        existsBoth(name);
        boundsBoth(name);
        const double whole[4] = {0, 0, 24, 24};
        symbolBoth(name, 24, 24, whole, 2, 0, 0);
    }
    context = "null";
    existsBoth(nullptr);
    boundsBoth(nullptr);
    const double whole[4] = {0, 0, 24, 24};
    symbolBoth(nullptr, 24, 24, whole, 2, 0, 0);
    for (int side = 0; side < 2; side++)
        check("no bounds", call<int>(side, 212, names.front().c_str(), (double*)nullptr), 0);
}

// Many threads at once, the same from both.
static void testThreads()
{
    const std::vector<std::string> names = symbolNames();
    std::vector<std::string> expected(64);
    std::vector<std::thread> threads;
    std::atomic<unsigned long> bad = 0, done = 0;
    for (int t = 0; t < 8; t++) {
        threads.emplace_back([&, t] {
            uint32_t s = t * 77 + 5;
            for (int i = 0; i < 300; i++) {
                s = s * 1103515245 + 12345;
                const std::string& name = names[(s >> 16) % std::min<size_t>(names.size(), 90)];
                const int size = 10 + (s >> 8) % 8;
                const double rect[4] = {0, 0, double(size), double(size)};
                std::string got[2];
                for (int side = 0; side < 2; side++) {
                    int w = 0, h = 0;
                    void* p = call<void*>(side, 210, name.c_str(), size, size, (const double*)rect, 1.5, 0, 1, &w, &h);
                    if (p)
                        got[side].assign((char*)p, size_t(w) * h * 4);
                    call<void>(side, 83, p);
                }
                done++;
                bad += got[0] != got[1];
            }
        });
    }
    for (std::thread& t : threads)
        t.join();
    checks += done;
    context = "threads";
    if (bad)
        fail("threads", std::to_string(bad));
}

int main(int argc, char** argv)
{
    dir = argv[1];
    void* a = dlopen((dir + "/orig.so").c_str(), RTLD_NOW);
    void* b = a ? dlopen((dir + "/ours.so").c_str(), RTLD_NOW) : nullptr;
    if (!a || !b) {
        printf("%s\n", dlerror());
        return 2;
    }
    table = reinterpret_cast<void** (*)(int)>(dlsym(a, "reef_appkit_host_interface"))(0x18);
    oursFunction = reinterpret_cast<void* (*)(int)>(dlsym(b, "oursFunction"));
    if (getenv("SEED"))
        seed = atoi(getenv("SEED"));
    for (int i = 2; i < argc; i++) {
        const std::string g = argv[i];
        const unsigned long c = checks, f = failures, n = interesting;
        if (g == "images") testImages();
        else if (g == "dmabuf") testDmaBuf();
        else if (g == "decode") testDecode();
        else if (g == "encode") testEncode();
        else if (g == "icons") testIcons();
        else if (g == "symbols") testSymbols(false);
        else if (g == "allsymbols") testSymbols(true);
        else if (g == "threads") testThreads();
        else if (g == "folder") testFolder();
        else { printf("no group %s\n", g.c_str()); return 2; }
        printf("%s: %lu checks, %lu different, %lu with something in them\n", g.c_str(), checks - c, failures - f, interesting - n);
        fflush(stdout);
    }
    fflush(stdout);
    // The libraries stay: statics of both are torn down by exit.
    return failures != 0;
}
