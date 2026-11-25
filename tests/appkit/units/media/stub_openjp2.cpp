// An OpenJPEG that makes images up out of a seed (decoding) and calls the program's stream functions in odd ways; it logs.
#include "stub_log.h"
#include <vector>

struct Comp { uint32_t dx, dy, w, h, x0, y0, prec, bpp, sgnd, resno, factor; int32_t* data; uint16_t alpha; };
struct Image { uint32_t x0, y0, x1, y1, numcomps; int color; Comp* comps; unsigned char* icc; uint32_t iccLength; };
struct Parm { uint32_t dx, dy, w, h, x0, y0, prec, bpp, sgnd; };
static_assert(sizeof(Comp) == 64 && sizeof(Image) == 48 && sizeof(Parm) == 36);
struct Stream { size_t (*read)(void*, size_t, void*); size_t (*write)(void*, size_t, void*); int64_t (*skip)(int64_t, void*); int (*seek)(int64_t, void*); void* user; };

static uint64_t state_ = 1;
static int failAt = -1, calls = 0;
static uint32_t rnd() { state_ = state_ * 6364136223846793005ull + 1442695040888963407ull; return uint32_t(state_ >> 33); }
static bool chance(int p) { return int(rnd() % 100) < p; }
static bool fails() { return calls++ == failAt; }

extern "C" {
void stub_seed(uint64_t seed, int fail) { state_ = seed * 0x9E3779B97F4A7C15ull + 99; rnd(); rnd(); failAt = fail; calls = 0; }

Stream* opj_stream_create(size_t size, int input) { note("opj_stream_create %zu %d", size, input); return new Stream{}; }
void opj_stream_set_read_function(Stream* s, size_t (*f)(void*, size_t, void*)) { note("set_read"); s->read = f; }
void opj_stream_set_write_function(Stream* s, size_t (*f)(void*, size_t, void*)) { note("set_write"); s->write = f; }
void opj_stream_set_skip_function(Stream* s, int64_t (*f)(int64_t, void*)) { note("set_skip"); s->skip = f; }
void opj_stream_set_seek_function(Stream* s, int (*f)(int64_t, void*)) { note("set_seek"); s->seek = f; }
void opj_stream_set_user_data(Stream* s, void* u, void (*f)(void*)) { note("set_user_data %d %d", u != nullptr, f != nullptr); s->user = u; }
void opj_stream_set_user_data_length(Stream*, uint64_t n) { note("set_length %llu", (unsigned long long)n); }
void opj_stream_destroy(Stream* s) { note("opj_stream_destroy"); delete s; }
void* opj_create_decompress(int format) { note("opj_create_decompress %d", format); return fails() ? nullptr : malloc(1); }
void* opj_create_compress(int format) { note("opj_create_compress %d", format); return fails() ? nullptr : malloc(1); }
void opj_destroy_codec(void* c) { note("opj_destroy_codec"); free(c); }
void opj_set_default_decoder_parameters(void* p) { note("decoder_parameters %016llx", (unsigned long long)hashOf(p, 65536)); memset(p, 7, 8252); }
int opj_setup_decoder(void*, void* p) { note("opj_setup_decoder %016llx", (unsigned long long)hashOf(p, 65536)); return !fails(); }
int opj_set_error_handler(void*, void (*f)(const char*, void*), void* c) { note("set_error_handler %d", c != nullptr); f("an error", c); return 1; }
int opj_set_warning_handler(void*, void (*f)(const char*, void*), void* c) { note("set_warning_handler %d", c != nullptr); f("a warning", c); return 1; }

int opj_read_header(Stream* s, void*, Image** out)
{
    unsigned char buffer[300];
    for (int i = 0; i < 20; i++) {
        switch (rnd() % 3) {
        case 0: {
            const size_t n = rnd() % 200;
            memset(buffer, 0xee, sizeof buffer);
            const size_t got = s->read(buffer, n, s->user);
            note("read %zu -> %lld %016llx", n, (long long)got, (unsigned long long)hashOf(buffer, sizeof buffer));
            break;
        }
        case 1: {
            const int64_t n = int64_t(rnd() % 400) - 200;
            note("skip %lld -> %lld", (long long)n, (long long)s->skip(n, s->user));
            break;
        }
        default: {
            const int64_t n = int64_t(rnd() % 500) - 100;
            note("seek %lld -> %d", (long long)n, s->seek(n, s->user));
        }
        }
    }
    if (fails())
        return 0;
    if (chance(3)) {
        note("no image");
        return 1;
    }
    Image* image = static_cast<Image*>(calloc(1, sizeof(Image)));
    image->x0 = chance(70) ? 0 : rnd() % 20;
    image->y0 = chance(70) ? 0 : rnd() % 20;
    image->x1 = image->x0 + rnd() % 40;
    image->y1 = image->y0 + rnd() % 30;
    if (chance(3)) image->x1 = image->x0 + 65536 + rnd() % 3;
    if (chance(3)) image->y1 = image->y0 - 1;
    if (chance(2)) image->x1 = image->x0 + 70000;
    image->numcomps = chance(5) ? 0 : 1 + rnd() % 6;
    static const int colors[] = {-1, 0, 1, 2, 3, 4, 5};
    image->color = colors[rnd() % 7];
    const uint32_t w = image->x1 - image->x0, h = image->y1 - image->y0;
    if (!chance(3))
        image->comps = static_cast<Comp*>(calloc(image->numcomps ? image->numcomps : 1, sizeof(Comp)));
    static const uint32_t precs[] = {8, 8, 8, 1, 2, 4, 7, 9, 10, 12, 16, 24, 30, 31};
    const bool alphas = chance(40);
    for (uint32_t i = 0; image->comps && i < image->numcomps; i++) {
        Comp& c = image->comps[i];
        c.dx = i == 0 ? 1 : 1 + rnd() % 3;
        c.dy = i == 0 ? 1 : 1 + rnd() % 3;
        if (i == 0 && chance(20)) c.dx = rnd() % 4, c.dy = rnd() % 4;
        else if (chance(4)) c.dx = 0;
        c.w = (w + (c.dx ? c.dx : 1) - 1) / (c.dx ? c.dx : 1);
        c.h = (h + (c.dy ? c.dy : 1) - 1) / (c.dy ? c.dy : 1);
        if (i == 0 && chance(5)) c.w++;
        if (i > 0 && chance(10)) c.w = c.w > 3 ? c.w - 2 : c.w, c.h = c.h > 3 ? c.h - 2 : c.h;
        c.prec = precs[rnd() % 14];
        if (i == 0 && chance(3)) c.prec = chance(50) ? 0 : 32;
        c.bpp = c.prec;
        c.sgnd = chance(20);
        // Reading past what a component has would differ from run to run.
        const uint64_t count = (uint64_t(c.w) + 2) * (uint64_t(c.h) + 2);
        if (count > 3000000 || (i == 0 && chance(3)))
            c.data = nullptr;
        else {
            c.data = static_cast<int32_t*>(malloc(count * 4));
            for (uint64_t k = 0; k < count; k++)
                c.data[k] = int32_t(rnd() % (1u << (c.prec < 31 ? c.prec + 1 : 31))) - (chance(30) ? (1 << (c.prec ? (c.prec < 31 ? c.prec : 31) - 1 : 0)) : 0);
        }
        c.alpha = alphas && chance(40) ? 1 + rnd() % 2 : 0;
        if (!c.data && i > 0)
            c.data = static_cast<int32_t*>(calloc(count ? count : 1, 4));
        if (c.w == 0 || c.h == 0)
            c.w = c.w, c.h = c.h;
    }
    note("image %u %u %u %u comps %u color %d", image->x0, image->y0, image->x1, image->y1, image->numcomps, image->color);
    *out = image;
    return 1;
}
int opj_decode(void*, Stream*, Image*) { note("opj_decode"); return !fails(); }
int opj_end_decompress(void*, Stream*) { note("opj_end_decompress"); return !fails(); }
void opj_image_destroy(Image* image)
{
    note("opj_image_destroy %d", image != nullptr);
    for (uint32_t i = 0; image->comps && i < image->numcomps; i++)
        free(image->comps[i].data);
    free(image->comps);
    free(image);
}
#ifndef VARIANT_noencoder
void opj_set_default_encoder_parameters(void* p)
{
    note("encoder_parameters %016llx", (unsigned long long)hashOf(p, 65536));
    memset(p, 0, 18688);
    static const int counts[] = {6, 1, 0, 2, 33, -1, 10};
    *reinterpret_cast<int*>(static_cast<char*>(p) + 0x15e0) = counts[rnd() % 7];
}
int opj_setup_encoder(void*, void* p, Image* image)
{
    note("opj_setup_encoder %016llx numres %d rate %.9g irreversible %d", (unsigned long long)hashOf(p, 65536), *reinterpret_cast<int*>(static_cast<char*>(p) + 0x15e0),
         *reinterpret_cast<float*>(static_cast<char*>(p) + 0x12c0), *reinterpret_cast<int*>(static_cast<char*>(p) + 0x15f0));
    note("  image %u %u %u %u %u %d", image->x0, image->y0, image->x1, image->y1, image->numcomps, image->color);
    for (uint32_t i = 0; i < image->numcomps; i++)
        note("  data %016llx alpha %d", (unsigned long long)hashOf(image->comps[i].data, size_t(image->comps[i].w) * image->comps[i].h * 4), image->comps[i].alpha);
    return !fails();
}
Image* opj_image_create(uint32_t n, Parm* parm, int color)
{
    note("opj_image_create %u %d %016llx", n, color, (unsigned long long)hashOf(parm, sizeof(Parm) * 4));
    if (fails())
        return nullptr;
    Image* image = static_cast<Image*>(calloc(1, sizeof(Image)));
    image->numcomps = n;
    image->color = color;
    image->comps = static_cast<Comp*>(calloc(n, sizeof(Comp)));
    for (uint32_t i = 0; i < n; i++) {
        Comp& c = image->comps[i];
        c.dx = parm[i].dx; c.dy = parm[i].dy; c.w = parm[i].w; c.h = parm[i].h; c.prec = parm[i].prec; c.bpp = parm[i].bpp; c.sgnd = parm[i].sgnd;
        c.data = static_cast<int32_t*>(calloc(size_t(c.w) * c.h, 4));
    }
    return image;
}
int opj_start_compress(void*, Image*, Stream*) { note("opj_start_compress"); return !fails(); }
int opj_encode(void*, Stream* s)
{
    unsigned char buffer[300];
    const int n = rnd() % 25;
    for (int i = 0; i < n; i++) {
        switch (rnd() % 4) {
        case 0: {
            const int64_t k = int64_t(rnd() % 300) - 150;
            note("skip %lld -> %lld", (long long)k, (long long)s->skip(k, s->user));
            break;
        }
        case 1: {
            const int64_t k = int64_t(rnd() % 500) - 100;
            note("seek %lld -> %d", (long long)k, s->seek(k, s->user));
            break;
        }
        default: {
            const size_t k = rnd() % 250;
            for (size_t j = 0; j < k; j++)
                buffer[j] = (unsigned char)rnd();
            note("write %zu -> %zu", k, s->write(buffer, k, s->user));
        }
        }
    }
    return !fails();
}
int opj_end_compress(void*, Stream*) { note("opj_end_compress"); return !fails(); }
#endif
}
