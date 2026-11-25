// LD_PRELOAD: the host library is always at /fake/host.so, so that the fingerprints agree.
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
int dladdr(const void* addr, Dl_info* info) { memset(info, 0, sizeof *info); info->dli_fname = "/fake/host.so"; return 1; }
int stat(const char* path, struct stat* s)
{
    if (strcmp(path, "/fake/host.so") == 0) { memset(s, 0, sizeof *s); s->st_size = 123456; s->st_mtim.tv_sec = getenv("FAKE_MTIME") ? atol(getenv("FAKE_MTIME")) : 1700000000; s->st_mtim.tv_nsec = 987654321; return 0; }
    static int (*real)(const char*, struct stat*);
    if (!real) real = dlsym(RTLD_NEXT, "stat");
    return real(path, s);
}
