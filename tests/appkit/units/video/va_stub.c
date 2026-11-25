#define _GNU_SOURCE
// libva.so.2 and libva-drm.so.2 that decode nothing: they log every call with the bytes of the buffers and hand out numbers.
// -DDRM: the second library. -DNO_<name>: a library without that function.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

extern void test_log(const char* text, size_t length) __attribute__((weak));
extern int test_fail(const char* name) __attribute__((weak));
extern uint32_t test_option(const char* name, uint32_t fallback) __attribute__((weak));

static void out(const char* format, ...)
{
    char line[512];
    va_list list;
    va_start(list, format);
    int n = vsnprintf(line, sizeof(line), format, list);
    va_end(list);
    if (test_log)
        test_log(line, n);
}
static int failed(const char* name)
{
    int f = test_fail ? test_fail(name) : 0;
    out("  va%s%s", name, f ? " FAILS" : "");
    return f;
}
static uint32_t option(const char* name, uint32_t fallback) { return test_option ? test_option(name, fallback) : fallback; }

#ifdef DRM
void* vaGetDisplayDRM(int fd)
{
    if (failed("GetDisplayDRM")) { out("\n"); return 0; }
    out(" %s\n", fcntl(fd, F_GETFD) & FD_CLOEXEC ? "cloexec" : "no cloexec");
    static int display;
    return &display;
}
#else

typedef void* Display;
typedef uint32_t Id;

typedef struct { uint32_t fourcc, byteOrder, bits, depth, r, g, b, a, reserved[4]; } Format;
typedef struct {
    Id id;
    Format format;
    Id buf;
    uint16_t width, height;
    uint32_t size, planes, pitches[3], offsets[3];
    int32_t paletteEntries, entryBytes;
    int8_t order[4];
    uint32_t reserved[4];
} Image;
typedef struct { int type; uint32_t flags; int valueType; union { int i; float f; void* p; } value; } SurfaceAttrib;
typedef struct { int type; uint32_t value; } ConfigAttrib;
typedef struct {
    uint32_t fourcc, width, height, objects;
    struct { int fd; uint32_t size; uint64_t modifier; } object[4];
    uint32_t layers;
    struct { uint32_t format, planes, index[4], offset[4], pitch[4]; } layer[4];
} Descriptor;

static uint64_t hash(const void* data, size_t n, uint64_t h)
{
    const uint8_t* p = data;
    for (size_t i = 0; i < n; i++)
        h = (h ^ p[i]) * 0x100000001b3ull;
    return h;
}

#define MAX 4096
static struct { int used; uint32_t fourcc; int width, height; uint64_t content; } surfaces[MAX];
static struct { int used; void* data; size_t size; } buffers[65536];
static struct { int used; Id buffer; } images[MAX];
static int surfaceCount, bufferCount, imageCount, contextCount, configCount;
static Id target;
static uint64_t rendered;

static void hex(const void* data, size_t n)
{
    const uint8_t* p = data;
    char line[200];
    for (size_t i = 0; i < n; i += 64) {
        int k = 0;
        for (size_t j = i; j < n && j < i + 64; j++)
            k += sprintf(line + k, "%02x", p[j]);
        out("    %s\n", line);
    }
}

int vaInitialize(Display d, int* major, int* minor)
{
    int f = failed("Initialize");
    out(" %d %d\n", *major, *minor);
    *major = 1;
    *minor = 22;
    return f ? 1 : 0;
}
int vaTerminate(Display d) { failed("Terminate"); out("\n"); return 0; }
const char* vaErrorStr(int s) { return "error"; }
#ifndef NO_callbacks
typedef void (*Callback)(void*, const char*);
Callback vaSetErrorCallback(Display d, Callback c, void* x) { failed("SetErrorCallback"); out(" %d %d\n", c != 0, x != 0); if (c) c(x, "an error"); return 0; }
Callback vaSetInfoCallback(Display d, Callback c, void* x) { failed("SetInfoCallback"); out(" %d %d\n", c != 0, x != 0); if (c) c(x, "an info"); return 0; }
#endif
// What the last answers were: no more than one is written after a failed question.
static int maxProfiles = 40, maxEntrypoints = 10, maxFormats = 8;
int vaMaxNumProfiles(Display d) { int f = failed("MaxNumProfiles"); out("\n"); return maxProfiles = f ? 0 : 40; }
int vaQueryConfigProfiles(Display d, int* list, int* count)
{
    int f = failed("QueryConfigProfiles");
    out(" %d\n", *count);
    if (f) return 1;
    static const int all[] = {-1, 0, 6, 7, 12, 13, 17, 18, 19, 21, 22};
    uint32_t mask = option("profiles", 0xffffffff);
    int n = 0;
    for (int i = 0; i < (int)(sizeof(all) / sizeof(*all)); i++)
        if ((mask >> i & 1) && (n == 0 || n < maxProfiles))
            list[n++] = all[i];
    *count = n;
    return 0;
}
int vaMaxNumEntrypoints(Display d) { int f = failed("MaxNumEntrypoints"); out("\n"); return maxEntrypoints = f ? 0 : 10; }
int vaQueryConfigEntrypoints(Display d, int profile, int* list, int* count)
{
    int f = failed("QueryConfigEntrypoints");
    out(" %d %d\n", profile, *count);
    if (f) return 1;
    int n = 0;
    list[n++] = 6;
    if (profile != 22 && option("vld", 1) && maxEntrypoints)
        list[n++] = 1;
    *count = n;
    return 0;
}
#ifndef NO_getconfig
int vaGetConfigAttributes(Display d, int profile, int entrypoint, ConfigAttrib* a, int n)
{
    int f = failed("GetConfigAttributes");
    out(" %d %d", profile, entrypoint);
    for (int i = 0; i < n; i++) {
        out(" %d:%x", a[i].type, a[i].value);
        a[i].value = option("rtformat", 0x101);
    }
    out("\n");
    return f ? 1 : 0;
}
#endif
int vaCreateConfig(Display d, int profile, int entrypoint, ConfigAttrib* a, int n, Id* config)
{
    int f = failed("CreateConfig");
    out(" %d %d", profile, entrypoint);
    for (int i = 0; i < n; i++)
        out(" %d:%x", a[i].type, a[i].value);
    out(" -> %x", *config);
    if (f) { *config = 77; out("\n"); return 1; }
    *config = 0x1000 + configCount++;
    out(" %x\n", *config);
    return 0;
}
int vaDestroyConfig(Display d, Id config) { failed("DestroyConfig"); out(" %x\n", config); return 0; }
int vaCreateSurfaces(Display d, unsigned format, unsigned width, unsigned height, Id* ids, unsigned count, SurfaceAttrib* a, unsigned n)
{
    int f = failed("CreateSurfaces");
    out(" %x %u %u %u", format, width, height, count);
    uint32_t fourcc = 0;
    for (unsigned i = 0; i < n; i++) {
        out(" %d/%x/%d/%x", a[i].type, a[i].flags, a[i].valueType, a[i].value.i);
        if (a[i].type == 1)
            fourcc = a[i].value.i;
    }
    if (f || surfaceCount + count > MAX) { out("\n"); return 2; }
    for (unsigned i = 0; i < count; i++) {
        int k = surfaceCount++;
        surfaces[k].used = 1;
        surfaces[k].fourcc = fourcc;
        surfaces[k].width = width;
        surfaces[k].height = height;
        surfaces[k].content = 0;
        ids[i] = 0x100 + k;
        out(" -> %x", ids[i]);
    }
    out("\n");
    return 0;
}
int vaDestroySurfaces(Display d, Id* ids, int count)
{
    failed("DestroySurfaces");
    for (int i = 0; i < count; i++) {
        out(" %x", ids[i]);
        if (ids[i] - 0x100 < MAX)
            surfaces[ids[i] - 0x100].used = 0;
    }
    out("\n");
    return 0;
}
int vaCreateContext(Display d, Id config, int width, int height, int flag, Id* targets, int count, Id* context)
{
    int f = failed("CreateContext");
    out(" %x %d %d %x %d %d -> %x", config, width, height, flag, targets != 0, count, *context);
    if (f) { *context = 78; out("\n"); return 1; }
    *context = 0x2000 + contextCount++;
    out(" %x\n", *context);
    return 0;
}
int vaDestroyContext(Display d, Id context) { failed("DestroyContext"); out(" %x\n", context); return 0; }
int vaCreateBuffer(Display d, Id context, int type, unsigned size, unsigned count, void* data, Id* buffer)
{
    int f = failed("CreateBuffer");
    out(" %x type %d size %u count %u -> %x", context, type, size, count, *buffer);
    if (f || bufferCount >= 65536) { out("\n"); return 2; }
    int k = bufferCount++;
    buffers[k].used = 1;
    buffers[k].size = (size_t)size * count;
    buffers[k].data = malloc(buffers[k].size + 1);
    memcpy(buffers[k].data, data, buffers[k].size);
    *buffer = 0x10000 + k;
    out(" %x\n", *buffer);
    if (type == 5)
        out("    data %016llx\n", (unsigned long long)hash(data, buffers[k].size, 0xcbf29ce484222325ull));
    else
        hex(data, buffers[k].size);
    return 0;
}
int vaDestroyBuffer(Display d, Id buffer)
{
    failed("DestroyBuffer");
    out(" %x\n", buffer);
    uint32_t k = buffer - 0x10000;
    if (k < 65536 && buffers[k].used) {
        free(buffers[k].data);
        buffers[k].data = 0;
        buffers[k].used = 0;
    }
    return 0;
}
int vaBeginPicture(Display d, Id context, Id t)
{
    int f = failed("BeginPicture");
    out(" %x %x%s\n", context, t, t - 0x100 < MAX && surfaces[t - 0x100].used ? "" : " (no surface)");
    target = t;
    rendered = 0xcbf29ce484222325ull;
    return f ? 1 : 0;
}
int vaRenderPicture(Display d, Id context, Id* list, int count)
{
    int f = failed("RenderPicture");
    out(" %x", context);
    for (int i = 0; i < count; i++) {
        out(" %x", list[i]);
        uint32_t k = list[i] - 0x10000;
        if (k < 65536 && buffers[k].used)
            rendered = hash(buffers[k].data, buffers[k].size, rendered);
        else
            out(" (no buffer)");
    }
    out("\n");
    return f ? 1 : 0;
}
int vaEndPicture(Display d, Id context)
{
    int f = failed("EndPicture");
    out(" %x\n", context);
    if (target - 0x100 < MAX)
        surfaces[target - 0x100].content = rendered;
    return f ? 1 : 0;
}
int vaSyncSurface(Display d, Id surface) { int f = failed("SyncSurface"); out(" %x\n", surface); return f ? 1 : 0; }
int vaMaxNumImageFormats(Display d) { int f = failed("MaxNumImageFormats"); out("\n"); return maxFormats = f ? 0 : 8; }
int vaQueryImageFormats(Display d, Format* list, int* count)
{
    int f = failed("QueryImageFormats");
    out(" %d\n", *count);
    if (f) return 1;
    static const char names[][5] = {"YV12", "NV12", "P010", "BGRA"};
    memset(list, 0, (maxFormats ? 4 : 1) * sizeof(Format));
    int n = 0;
    for (int i = 0; i < 4; i++) {
        if (!(option("formats", 15) >> i & 1) || (n && !maxFormats))
            continue;
        memcpy(&list[n].fourcc, names[i], 4);
        list[n].byteOrder = 1;
        list[n++].bits = 12 + i;
    }
    *count = n;
    return 0;
}
static uint32_t bytesOf(uint32_t fourcc) { return fourcc == 0x30313050 ? 2 : 1; }
static void makeImage(Image* image, const Format* format, int width, int height)
{
    uint32_t extra = option("pitch", 48);
    memset(image, 0, sizeof(*image));
    image->id = 0x3000 + imageCount;
    image->format = *format;
    image->width = width;
    image->height = height;
    image->planes = option("planes", 2);
    image->pitches[0] = width * bytesOf(format->fourcc) + extra;
    image->pitches[1] = ((width + 1) / 2) * 2 * bytesOf(format->fourcc) + extra;
    image->offsets[0] = 64;
    image->offsets[1] = 64 + image->pitches[0] * (height + 1);
    image->size = image->offsets[1] + image->pitches[1] * (height / 2 + 2);
    int k = bufferCount++;
    buffers[k].used = 1;
    buffers[k].size = image->size;
    buffers[k].data = calloc(1, image->size + 1);
    image->buf = 0x10000 + k;
    images[imageCount].used = 1;
    images[imageCount].buffer = image->buf;
    imageCount++;
}
static void fill(Id buffer, uint64_t seed)
{
    uint32_t k = buffer - 0x10000;
    uint8_t* p = buffers[k].data;
    for (size_t i = 0; i < buffers[k].size; i++) {
        seed = seed * 6364136223846793005ull + 1442695040888963407ull;
        p[i] = seed >> 56;
    }
}
int vaCreateImage(Display d, Format* format, int width, int height, Image* image)
{
    int f = failed("CreateImage");
    out(" %x %d %d", format->fourcc, width, height);
    if (f || (uint64_t)width * height > 20000000) { out("\n"); return 2; }
    makeImage(image, format, width, height);
    out(" -> %x\n", image->id);
    return 0;
}
#ifndef NO_derive
int vaDeriveImage(Display d, Id surface, Image* image)
{
    int f = failed("DeriveImage");
    out(" %x", surface);
    if (f || (uint64_t)surfaces[surface - 0x100].width * surfaces[surface - 0x100].height > 20000000) { out("\n"); return 2; }
    Format format = {0};
    format.fourcc = option("derived", surfaces[surface - 0x100].fourcc);
    makeImage(image, &format, surfaces[surface - 0x100].width, surfaces[surface - 0x100].height);
    fill(image->buf, surfaces[surface - 0x100].content + 1);
    out(" -> %x\n", image->id);
    return 0;
}
#endif
int vaGetImage(Display d, Id surface, int x, int y, unsigned width, unsigned height, Id image)
{
    int f = failed("GetImage");
    out(" %x %d %d %u %u %x\n", surface, x, y, width, height, image);
    if (f) return 1;
    fill(images[image - 0x3000].buffer, surfaces[surface - 0x100].content);
    return 0;
}
int vaDestroyImage(Display d, Id image)
{
    failed("DestroyImage");
    out(" %x\n", image);
    uint32_t k = images[image - 0x3000].buffer - 0x10000;
    free(buffers[k].data);
    buffers[k].used = 0;
    return 0;
}
int vaMapBuffer(Display d, Id buffer, void** data)
{
    int f = failed("MapBuffer");
    out(" %x\n", buffer);
    if (f) return 1;
    *data = buffers[buffer - 0x10000].data;
    return 0;
}
int vaUnmapBuffer(Display d, Id buffer) { failed("UnmapBuffer"); out(" %x\n", buffer); return 0; }
#ifndef NO_export
int vaExportSurfaceHandle(Display d, Id surface, uint32_t type, uint32_t flags, void* result)
{
    int f = failed("ExportSurfaceHandle");
    out(" %x %x %x\n", surface, type, flags);
    if (f) return 1;
    Descriptor* x = result;
    uint32_t objects = option("objects", 1), layers = option("layers", 2);
    x->fourcc = surfaces[surface - 0x100].fourcc;
    x->width = surfaces[surface - 0x100].width;
    x->height = surfaces[surface - 0x100].height;
    x->objects = objects;
    for (uint32_t i = 0; i < objects; i++) {
        x->object[i].fd = memfd_create("plane", MFD_CLOEXEC);
        char text[32];
        int n = sprintf(text, "object %u of %x", i, surface);
        if (write(x->object[i].fd, text, n) != n) abort();
        x->object[i].size = 1000 * (i + 1);
        x->object[i].modifier = 0x0100000000000001ull + i;
    }
    x->layers = layers;
    for (uint32_t i = 0; i < layers && i < 4; i++) {
        x->layer[i].format = i ? 0x38385247 : 0x20203852;
        x->layer[i].planes = 1;
        x->layer[i].index[0] = i % (objects ? objects : 1);
        x->layer[i].offset[0] = 4096 * i + 3;
        x->layer[i].pitch[0] = 512 + i;
    }
    return 0;
}
#endif
#endif
