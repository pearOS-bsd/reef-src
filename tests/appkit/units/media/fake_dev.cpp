// Made up V4L2 and DRM devices, whose answers follow from a seed; every call is logged.
#include "stub_log.h"
#include <cerrno>
#include <cstdarg>
#include <dirent.h>
#include <dlfcn.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <map>
#include <poll.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <vector>

static bool on = false;
static uint64_t state_ = 1;
static int mode = 0; // 0: random answers; 1: good devices only
static uint32_t rnd()
{
    state_ = state_ * 6364136223846793005ull + 1442695040888963407ull;
    return uint32_t(state_ >> 33);
}
static bool chance(int percent) { return int(rnd() % 100) < percent; }
static std::vector<std::string> entries;
static std::map<std::string, int> devices; // path -> fd
static int nextFd = 1000;
static dirent entry;
static size_t entryIndex;
static std::map<void*, size_t> maps;
static int frameNumber = 0;

extern "C" void fake_dev(int enable, uint64_t seed, int m, const char* const* names, int count)
{
    on = enable;
    state_ = seed * 0x9E3779B97F4A7C15ull + 12345;
    for (int i = 0; i < 5; i++)
        rnd();
    mode = m;
    entries.assign(names, names + count);
    nextFd = 1000;
    frameNumber = 0;
}

extern "C" DIR* opendir(const char* path)
{
    static auto real = reinterpret_cast<DIR* (*)(const char*)>(dlsym(RTLD_NEXT, "opendir"));
    if (!on || strcmp(path, "/dev") != 0)
        return real(path);
    note("opendir %s", path);
    entryIndex = 0;
    if (mode == 2) {
        errno = EACCES;
        return nullptr;
    }
    return reinterpret_cast<DIR*>(&entryIndex);
}
extern "C" dirent* readdir(DIR* d)
{
    static auto real = reinterpret_cast<dirent* (*)(DIR*)>(dlsym(RTLD_NEXT, "readdir"));
    if (d != reinterpret_cast<DIR*>(&entryIndex))
        return real(d);
    if (entryIndex >= entries.size())
        return nullptr;
    snprintf(entry.d_name, sizeof entry.d_name, "%s", entries[entryIndex++].c_str());
    return &entry;
}
extern "C" int closedir(DIR* d)
{
    static auto real = reinterpret_cast<int (*)(DIR*)>(dlsym(RTLD_NEXT, "closedir"));
    if (d != reinterpret_cast<DIR*>(&entryIndex))
        return real(d);
    note("closedir");
    return 0;
}
extern "C" int open(const char* path, int flags, ...)
{
    va_list a;
    va_start(a, flags);
    const int m = flags & (O_CREAT | O_TMPFILE) ? va_arg(a, int) : 0;
    va_end(a);
    if (!on || (strncmp(path, "/dev/video", 10) != 0 && strncmp(path, "/dev/dri/", 9) != 0 && strncmp(path, "/fake", 5) != 0))
        return int(syscall(SYS_openat, AT_FDCWD, path, flags, m));
    int fd = nextFd++;
    if (mode != 1 && chance(15)) {
        errno = EBUSY;
        fd = -1;
    }
    note("open %s %x -> %d", path, flags, fd);
    return fd;
}
extern "C" int close(int fd)
{
    if (!on || fd < 1000)
        return int(syscall(SYS_close, fd));
    note("close %d", fd);
    return 0;
}
extern "C" int poll(pollfd* f, nfds_t n, int timeout)
{
    static auto real = reinterpret_cast<int (*)(pollfd*, nfds_t, int)>(dlsym(RTLD_NEXT, "poll"));
    if (!on || n != 1 || f->fd < 1000)
        return real(f, n, timeout);
    int r = 1;
    f->revents = POLLIN;
    if (mode != 1) {
        switch (rnd() % 12) {
        case 0: r = 0; break;
        case 1: r = -1; errno = EINTR; break;
        case 2: r = -1; errno = EINVAL; break;
        case 3: f->revents = POLLERR; break;
        case 4: f->revents = POLLIN | POLLHUP; break;
        case 5: f->revents = POLLNVAL; break;
        case 6: f->revents = 0; break;
        default: break;
        }
    }
    note("poll %d %x %d -> %d %x", f->fd, f->events, timeout, r, f->revents);
    return r;
}
extern "C" void* mmap(void* address, size_t length, int prot, int flags, int fd, off_t offset)
{
    if (!on || fd < 1000)
        return reinterpret_cast<void*>(syscall(SYS_mmap, address, length, prot, flags, fd, offset));
    if (mode != 1 && chance(5)) {
        note("mmap %zu %d %d %d %lld -> failed", length, prot, flags, fd, (long long)offset);
        errno = ENOMEM;
        return MAP_FAILED;
    }
    unsigned char* p = static_cast<unsigned char*>(malloc(length ? length : 1));
    for (size_t i = 0; i < length; i++)
        p[i] = (unsigned char)(i * 13 + offset);
    maps[p] = length;
    note("mmap %d %zu %d %d %d %lld -> buffer %zu", address != nullptr, length, prot, flags, fd, (long long)offset, maps.size());
    return p;
}
extern "C" int munmap(void* p, size_t length)
{
    auto it = maps.find(p);
    if (it == maps.end())
        return int(syscall(SYS_munmap, p, length));
    note("munmap %zu (%zu)", length, it->second);
    maps.erase(it);
    free(p);
    return 0;
}

static int camera(int fd, unsigned long request, void* arg)
{
    switch (request) {
    case VIDIOC_QUERYCAP: {
        auto c = static_cast<v4l2_capability*>(arg);
        note("  in %016llx", (unsigned long long)hashOf(c, sizeof *c));
        snprintf(reinterpret_cast<char*>(c->card), sizeof c->card, "Camera %u", rnd() % 1000);
        if (chance(30))
            memset(c->card, 'x', sizeof c->card - (mode == 0));
        snprintf(reinterpret_cast<char*>(c->bus_info), sizeof c->bus_info, "usb-%u", rnd() % 1000);
        c->bus_info[31] = 0;
        const uint32_t caps[] = {V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING, V4L2_CAP_VIDEO_CAPTURE, V4L2_CAP_STREAMING, 0, 0x85200001, 0x80000000, 0x05200001, 0xffffffff};
        c->capabilities = mode == 1 ? 0x85200001 : caps[rnd() % 8];
        c->device_caps = mode == 1 ? 0x04200001 : caps[rnd() % 8];
        return 0;
    }
    case VIDIOC_ENUM_FMT: {
        auto f = static_cast<v4l2_fmtdesc*>(arg);
        note("  in index %u type %u", f->index, f->type);
        static const uint32_t formats[] = {V4L2_PIX_FMT_YUYV, V4L2_PIX_FMT_UYVY, V4L2_PIX_FMT_NV12, V4L2_PIX_FMT_MJPEG, V4L2_PIX_FMT_H264, V4L2_PIX_FMT_RGB24, V4L2_PIX_FMT_YVYU};
        if (f->index >= (mode == 1 ? 7 : 1 + state_ % 9)) {
            errno = EINVAL;
            return -1;
        }
        f->pixelformat = formats[mode == 1 ? f->index % 7 : rnd() % 7];
        return 0;
    }
    case VIDIOC_ENUM_FRAMESIZES: {
        auto s = static_cast<v4l2_frmsizeenum*>(arg);
        note("  in index %u format %x rest %016llx", s->index, s->pixel_format, (unsigned long long)hashOf(reinterpret_cast<char*>(s) + 8, sizeof *s - 8));
        if (s->index >= (mode == 1 ? 5 : mode == 3 ? 60 : rnd() % 7)) {
            errno = EINVAL;
            return -1;
        }
        s->type = mode == 3 || chance(60) ? V4L2_FRMSIZE_TYPE_DISCRETE : chance(50) ? V4L2_FRMSIZE_TYPE_STEPWISE : V4L2_FRMSIZE_TYPE_CONTINUOUS;
        if (s->type == V4L2_FRMSIZE_TYPE_DISCRETE) {
            s->discrete.width = 16 * (1 + rnd() % 200);
            s->discrete.height = 8 * (1 + rnd() % 300);
        } else {
            static const uint32_t steps[] = {0, 1, 2, 4, 8, 16, 7, 320};
            s->stepwise.min_width = chance(50) ? 160 : rnd() % 700;
            s->stepwise.max_width = chance(50) ? 4096 : rnd() % 4000;
            s->stepwise.step_width = steps[rnd() % 8];
            s->stepwise.min_height = chance(50) ? 120 : rnd() % 500;
            s->stepwise.max_height = chance(50) ? 3000 : rnd() % 3000;
            s->stepwise.step_height = steps[rnd() % 8];
        }
        return 0;
    }
    case VIDIOC_ENUM_FRAMEINTERVALS: {
        auto v = static_cast<v4l2_frmivalenum*>(arg);
        note("  in index %u format %x %ux%u", v->index, v->pixel_format, v->width, v->height);
        if (v->index >= rnd() % 5) {
            errno = EINVAL;
            return -1;
        }
        v->type = chance(70) ? V4L2_FRMIVAL_TYPE_DISCRETE : chance(50) ? V4L2_FRMIVAL_TYPE_CONTINUOUS : V4L2_FRMIVAL_TYPE_STEPWISE;
        auto number = [] { return chance(15) ? 0 : 1 + rnd() % 1001; };
        if (v->type == V4L2_FRMIVAL_TYPE_DISCRETE) {
            v->discrete.numerator = number();
            v->discrete.denominator = chance(10) ? 0xffffffffu : number();
        } else {
            v->stepwise.min = {number(), number()};
            v->stepwise.max = {number(), number()};
            v->stepwise.step = {number(), number()};
        }
        return 0;
    }
    case VIDIOC_S_FMT: {
        auto f = static_cast<v4l2_format*>(arg);
        note("  in type %u %ux%u %x field %u all %016llx", f->type, f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.pixelformat, f->fmt.pix.field,
             (unsigned long long)hashOf(f, sizeof *f));
        if (mode != 1 && chance(5)) f->fmt.pix.width += 8;
        if (mode != 1 && chance(5)) f->fmt.pix.height += 8;
        if (mode != 1 && chance(5)) f->fmt.pix.pixelformat ^= 1;
        return 0;
    }
    case VIDIOC_S_PARM: {
        auto p = static_cast<v4l2_streamparm*>(arg);
        note("  in type %u %u/%u all %016llx", p->type, p->parm.capture.timeperframe.numerator, p->parm.capture.timeperframe.denominator,
             (unsigned long long)hashOf(p, sizeof *p));
        return 0;
    }
    case VIDIOC_REQBUFS: {
        auto b = static_cast<v4l2_requestbuffers*>(arg);
        note("  in count %u type %u memory %u all %016llx", b->count, b->type, b->memory, (unsigned long long)hashOf(b, sizeof *b));
        if (mode != 1)
            b->count = rnd() % 10;
        return 0;
    }
    case VIDIOC_QUERYBUF:
    case VIDIOC_QBUF: {
        auto b = static_cast<v4l2_buffer*>(arg);
        note("  in index %u type %u memory %u all %016llx", b->index, b->type, b->memory, (unsigned long long)hashOf(b, sizeof *b));
        if (request == VIDIOC_QUERYBUF) {
            b->length = 1000 + rnd() % 5000;
            b->m.offset = 4096 * b->index;
            b->flags = rnd();
        }
        return 0;
    }
    case VIDIOC_DQBUF: {
        auto b = static_cast<v4l2_buffer*>(arg);
        note("  in all %016llx", (unsigned long long)hashOf(b, sizeof *b));
        if (mode != 1 && chance(10)) {
            errno = chance(50) ? EAGAIN : ENODEV;
            return -1;
        }
        b->index = maps.empty() ? 0 : frameNumber++ % maps.size();
        b->bytesused = chance(20) ? 0 : rnd() % 7000;
        static const uint32_t kinds[] = {V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC, V4L2_BUF_FLAG_TIMESTAMP_UNKNOWN, V4L2_BUF_FLAG_TIMESTAMP_COPY, 0xe000, V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC | 5};
        b->flags = kinds[rnd() % 5];
        b->timestamp.tv_sec = chance(30) ? 0 : chance(20) ? 100000 : 990 + rnd() % 20;
        b->timestamp.tv_usec = chance(30) ? 0 : rnd() % 1000000;
        return 0;
    }
    case VIDIOC_STREAMON:
    case VIDIOC_STREAMOFF:
        note("  in %d", *static_cast<int*>(arg));
        return 0;
    }
    note("  unknown");
    return -1;
}

// DRM: each card has some CRTCs, connectors and encoders.
static int drm(int fd, unsigned long request, void* arg)
{
    const uint64_t card = uint64_t(fd) * 7919 + (mode == 1 ? 0 : state_ / 1000000007 % 3);
    const uint32_t crtcs = card % 5, connectors = card % 4 + (mode == 1);
    switch (request) {
    case DRM_IOCTL_MODE_GETRESOURCES: {
        auto r = static_cast<drm_mode_card_res*>(arg);
        note("  in crtcs %u connectors %u fbs %u encoders %u pointers %d %d", r->count_crtcs, r->count_connectors, r->count_fbs, r->count_encoders,
             r->crtc_id_ptr != 0, r->connector_id_ptr != 0);
        const uint32_t nc = mode == 1 ? 4 : crtcs, nk = mode == 1 ? 6 : connectors;
        if (r->count_crtcs && r->crtc_id_ptr)
            for (uint32_t i = 0; i < r->count_crtcs && i < nc; i++)
                reinterpret_cast<uint32_t*>(r->crtc_id_ptr)[i] = 100 + i;
        if (r->count_connectors && r->connector_id_ptr)
            for (uint32_t i = 0; i < r->count_connectors && i < nk; i++)
                reinterpret_cast<uint32_t*>(r->connector_id_ptr)[i] = 200 + i;
        r->count_crtcs = nc;
        r->count_connectors = nk;
        return 0;
    }
    case DRM_IOCTL_MODE_GETCONNECTOR: {
        auto c = static_cast<drm_mode_get_connector*>(arg);
        note("  in id %u all %016llx", c->connector_id, (unsigned long long)hashOf(c, sizeof *c));
        const uint32_t i = c->connector_id - 200;
        c->connector_type = mode == 1 ? (const uint32_t[]){14, 11, 10, 7, 25, 20}[i % 6] : rnd() % 24;
        c->connector_type_id = mode == 1 ? (const uint32_t[]){1, 1, 12, 123456, 7, 4294967295u}[i % 6] : 1 + rnd() % 3;
        c->encoder_id = mode == 1 ? (i == 3 ? 0 : 300 + i) : chance(20) ? 0 : 300 + i;
        return 0;
    }
    case DRM_IOCTL_MODE_GETENCODER: {
        auto e = static_cast<drm_mode_get_encoder*>(arg);
        note("  in id %u all %016llx", e->encoder_id, (unsigned long long)hashOf(e, sizeof *e));
        e->crtc_id = mode == 1 ? (const uint32_t[]){100, 101, 102, 103, 999, 0}[(e->encoder_id - 300) % 6] : chance(20) ? 0 : 100 + rnd() % 6;
        return 0;
    }
    case DRM_IOCTL_MODE_GETCRTC: {
        auto c = static_cast<drm_mode_crtc*>(arg);
        note("  in id %u all %016llx", c->crtc_id, (unsigned long long)hashOf(c, sizeof *c));
        c->mode_valid = !chance(20);
        c->mode.clock = chance(15) ? 0 : 25000 + rnd() % 600000;
        c->mode.htotal = chance(5) ? 0 : 800 + rnd() % 5000;
        c->mode.vtotal = 500 + rnd() % 3000;
        return 0;
    }
    case DRM_IOCTL_WAIT_VBLANK: {
        auto w = static_cast<drm_wait_vblank*>(arg);
        note("  in type %x sequence %u signal %lu", w->request.type, w->request.sequence, w->request.signal);
        w->reply.tval_sec = 1000 + rnd() % 100;
        w->reply.tval_usec = rnd() % 1000000;
        return 0;
    }
    }
    note("  unknown");
    return -1;
}

extern "C" int ioctl(int fd, unsigned long request, ...)
{
    va_list a;
    va_start(a, request);
    void* arg = va_arg(a, void*);
    va_end(a);
    if (!on || fd < 1000)
        return int(syscall(SYS_ioctl, fd, request, arg));
    note("ioctl %d %lx", fd, request);
    if (chance(8)) {
        note("  interrupted");
        errno = EINTR;
        return -1;
    }
    if (mode != 1 && chance(4)) {
        errno = chance(50) ? EINVAL : EAGAIN;
        note("  error %d", errno);
        return -1;
    }
    if (mode != 1 && chance(1)) {
        errno = chance(50) ? EINTR : 0;
        note("  returns 1");
        return 1;
    }
    return _IOC_TYPE(request) == 'd' ? drm(fd, request, arg) : camera(fd, request, arg);
}
