// JPEG 2000. test_jpeg2000 LIBRARY stub SEEDS | real FILE...
#include "load.h"
#include "stub_log.h"
#include <string>
#include <vector>

static auto isJp2(const void* d, size_t n) { return fn<bool(const void*, size_t)>(319)(d, n); }
static auto decode(const void* d, size_t n, int* w, int* h) { return fn<uint32_t*(const void*, size_t, int*, int*)>(320)(d, n, w, h); }
static auto encode(const void* p, int w, int h, size_t stride, double q, size_t* n) { return fn<void*(const void*, int, int, size_t, double, size_t*)>(312)(p, w, h, stride, q, n); }

static uint64_t state_ = 1;
static uint32_t rnd() { state_ = state_ * 6364136223846793005ull + 1442695040888963407ull; return uint32_t(state_ >> 33); }

static void tryDecode(const std::string& data)
{
    int w = -7, h = -8;
    const bool is = isJp2(data.data(), data.size());
    uint32_t* p = decode(data.data(), data.size(), &w, &h);
    note("is %d decode %d %dx%d %016llx", is, p != nullptr, w, h, p ? (unsigned long long)hashOf(p, size_t(w) * h * 4) : 0ull);
    free(p);
}

static std::vector<uint32_t> image(int w, int h, size_t stride, int kind)
{
    std::vector<uint32_t> v(stride / 4 * h + 4);
    for (uint32_t& p : v) {
        const uint32_t a = kind == 0 ? 255 : kind == 1 ? rnd() % 256 : (rnd() % 10 ? 255 : rnd() % 256);
        // Also what is not premultiplied.
        const uint32_t top = kind == 1 && rnd() % 8 == 0 ? 255 : a;
        p = a << 24 | (rnd() % (top + 1)) << 16 | (rnd() % (top + 1)) << 8 | rnd() % (top + 1);
    }
    return v;
}

static void encodes(int i, bool real)
{
    static const double qualities[] = {1, 0, 0.5, 0.999, -0.1, 2, 0.75, 0.1, 1e-9, 0.9999999999999999};
    const int w = 1 + rnd() % (real ? 90 : 50), h = 1 + rnd() % (real ? 80 : 40);
    const size_t stride = w * 4 + (rnd() % 3) * 4;
    const auto v = image(w, h, stride, i % 4 == 3 ? 0 : i % 4);
    size_t n = 777;
    void* p = encode(v.data(), w, h, stride, qualities[i % 10], &n);
    note("encode %dx%d stride %zu quality %g -> %d %zu %016llx", w, h, stride, qualities[i % 10], p != nullptr, n, p ? (unsigned long long)hashOf(p, n) : 0ull);
    if (p && real) {
        tryDecode(std::string(static_cast<char*>(p), n));
        if (i % 5 == 0)
            for (int k = 0; k < 10; k++) {
                std::string bad(static_cast<char*>(p), n);
                for (int j = 0; j < 1 + int(rnd() % 3); j++)
                    bad[rnd() % bad.size()] = char(rnd());
                if (rnd() % 2)
                    bad.resize(rnd() % bad.size());
                tryDecode(bad);
            }
    }
    free(p);
}

int main(int argc, char** argv)
{
    load(argv[1]);
    const bool real = std::string(argv[2]) == "real";
    const std::string jp2("\0\0\0\x0cjP  \r\n\x87\n", 12), j2k("\xff\x4f\xff\x51");
    auto seed = real ? nullptr : reinterpret_cast<void (*)(uint64_t, int)>(dlsym(dlopen("libopenjp2.so.7", RTLD_NOW), "stub_seed"));
    for (const std::string& d : {std::string(), std::string("x"), jp2, j2k, jp2.substr(0, 11), j2k.substr(0, 3), jp2 + "more", j2k + "more and more bytes", std::string("\0\0\0\x0cjP  \r\n\x87\r", 12),
                                 std::string("\0\0\0\x0cjP  \r\n\x87\n", 12).substr(0, 8) + j2k, std::string("\xff\x4f\xff"), std::string("\xff\x4f\xff\x52 more than that"), std::string("\xff\xd8\xff\xe0 jpeg header....."),
                                 std::string("RIFF....WEBPVP8 ......")}) {
        if (seed)
            seed(d.size(), -1);
        note("bytes %zu: %d", d.size(), isJp2(d.data(), d.size()));
        tryDecode(d);
    }
    size_t n = 5;
    const std::vector<uint32_t> v(100, 0xff102030);
    if (seed)
        seed(1, -1);
    for (const auto& [p, w, h, stride, size] : {std::tuple<const void*, int, int, size_t, size_t*>{nullptr, 5, 5, 20, &n}, {v.data(), 0, 5, 20, &n}, {v.data(), 5, 0, 20, &n}, {v.data(), -5, 5, 20, &n},
                                                {v.data(), 5, -5, 20, &n}, {v.data(), 65537, 1, 4 * 65537, &n}, {v.data(), 1, 65537, 4, &n}, {v.data(), 5, 5, 19, &n}, {v.data(), 5, 5, 20, nullptr},
                                                {v.data(), 5, 5, 0, &n}, {v.data(), 5, 5, 20, &n}, {v.data(), 5, 5, 24, &n}, {v.data(), 1, 1, 4, &n}}) {
        void* r = encode(p, w, h, stride, 1, size);
        note("encode %d %d %d %zu -> %d %zu", p != nullptr, w, h, stride, r != nullptr, n);
        free(r);
    }
    if (real) {
        for (int i = 0; i < 300; i++) {
            state_ = i + 77;
            encodes(i, true);
        }
        for (int i = 3; i < argc; i++) {
            std::string data;
            if (FILE* f = fopen(argv[i], "rb")) {
                char b[65536];
                for (size_t k; (k = fread(b, 1, sizeof b, f)) > 0;)
                    data.append(b, k);
                fclose(f);
            }
            note("%s", argv[i]);
            tryDecode(data);
        }
    } else {
        const int seeds = atoi(argv[3]);
        for (int i = 0; i < seeds; i++) {
            note("== %d", i);
            state_ = i;
            seed(i, i % 7 == 6 ? int(i / 7 % 6) : -1);
            std::string data = i % 2 ? jp2 : j2k;
            for (int k = rnd() % 300; k > 0; k--)
                data.push_back(char(rnd()));
            tryDecode(data);
            seed(i, i % 7 == 6 ? int(i / 7 % 7) : -1);
            encodes(i, false);
        }
    }
    fflush(stdout);
    _exit(0);
}
