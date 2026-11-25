// A libsndfile that reads a made up format through the given functions and logs: "SND1", frames (8), rate (4), channels (4), flags (4).
#include "stub_log.h"

struct Info { int64_t frames; int samplerate, channels, format, sections, seekable; };
struct Io
{
    int64_t (*length)(void*);
    int64_t (*seek)(int64_t, int, void*);
    int64_t (*read)(void*, int64_t, void*);
    int64_t (*write)(const void*, int64_t, void*);
    int64_t (*tell)(void*);
};
struct File { Io* io; void* user; int64_t frames, position; int channels; int flags; int id; };
static int ids = 0;

extern "C" {
void* sf_open_virtual(Io* io, int mode, Info* info, void* user)
{
    note("sf_open_virtual mode=%d info=%lld %d %d %d %d %d", mode, (long long)info->frames, info->samplerate, info->channels, info->format,
         info->sections, info->seekable);
    note("  length=%lld tell=%lld", (long long)io->length(user), (long long)io->tell(user));
    unsigned char head[24] = {};
    const int64_t got = io->read(head, 24, user);
    note("  read 24 -> %lld tell=%lld", (long long)got, (long long)io->tell(user));
    for (const auto& [offset, whence] : {std::pair<int64_t, int>{5, 0}, {3, 1}, {-100, 1}, {-2, 2}, {100000, 0}, {7, 2}, {-5, 0}, {1, 7}, {0, 0}})
        note("  seek %lld %d -> %lld tell=%lld", (long long)offset, whence, (long long)io->seek(offset, whence, user), (long long)io->tell(user));
    unsigned char rest[64] = {};
    io->seek(-3, 2, user);
    note("  read at end %lld", (long long)io->read(rest, 64, user));
    note("  read past end %lld", (long long)io->read(rest, 64, user));
    note("  read -5 %lld", (long long)io->read(rest, -5, user));
    note("  write %lld", (long long)io->write(rest, 4, user));
    if (got < 24 || memcmp(head, "SND1", 4) != 0)
        return nullptr;
    File* f = new File{io, user, 0, 0, 0, 0, ++ids};
    memcpy(&f->frames, head + 4, 8);
    memcpy(&info->samplerate, head + 12, 4);
    memcpy(&f->channels, head + 16, 4);
    memcpy(&f->flags, head + 20, 4);
    info->frames = f->frames;
    info->channels = f->channels;
    info->format = 0x10006;
    info->sections = 1;
    info->seekable = 1;
    return f;
}
#ifndef VARIANT_nosndread
int64_t sf_readf_float(void* file, float* out, int64_t frames)
{
    File* f = static_cast<File*>(file);
    int64_t n = f->frames - f->position;
    if (n > frames)
        n = frames;
    if (n < 0)
        n = 0;
    if (f->flags & 2)
        n = n > 100 ? 100 : n;
    for (int64_t i = 0; i < n * f->channels; i++)
        out[i] = float((f->position * f->channels + i) % 1000) / 1000 - 0.5f;
    note("sf_readf_float %d frames=%lld at %lld -> %lld", f->id, (long long)frames, (long long)f->position, (long long)n);
    f->position += n;
    return n;
}
#endif
int64_t sf_seek(void* file, int64_t frames, int whence)
{
    File* f = static_cast<File*>(file);
    note("sf_seek %d %lld %d", f->id, (long long)frames, whence);
    if (f->flags & 1)
        return -1;
    f->position = frames;
    return frames;
}
int sf_close(void* file) { note("sf_close %d", static_cast<File*>(file)->id); delete static_cast<File*>(file); return 0; }
}
