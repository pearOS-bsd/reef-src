// LD_PRELOAD: a malloc of one size fails (failMalloc), dladdr as FAKE_DLADDR says.
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <string>
static __thread size_t failing;
extern "C" void* __libc_malloc(size_t);
extern "C" void failMalloc(size_t size) { failing = size; }
extern "C" void* malloc(size_t size)
{
    if (failing && size == failing) {
        // Only where one of the two libraries asks itself.
        const size_t was = failing;
        failing = 0;
        Dl_info info = {};
        static const auto real = reinterpret_cast<int (*)(const void*, Dl_info*)>(dlsym(RTLD_NEXT, "dladdr"));
        const bool ours = real(__builtin_return_address(0), &info) && info.dli_fname && (strstr(info.dli_fname, "/orig.so") || strstr(info.dli_fname, "/ours.so"));
        failing = was;
        if (ours)
            return nullptr;
    }
    return __libc_malloc(size);
}
extern "C" int dladdr(const void* address, Dl_info* info)
{
    static const auto real = reinterpret_cast<int (*)(const void*, Dl_info*)>(dlsym(RTLD_NEXT, "dladdr"));
    const char* fake = getenv("FAKE_DLADDR");
    if (!fake)
        return real(address, info);
    const int r = real(address, info);
    if (!r || !strstr(info->dli_fname, "/o") || !strstr(info->dli_fname, ".so"))
        return r;
    if (!strcmp(fake, "fail"))
        return 0;
    if (!strcmp(fake, "null")) {
        info->dli_fname = nullptr;
        return 1;
    }
    info->dli_fname = fake;
    return 1;
}
// The home folder is FAKE_HOME's (the toolkit asks the user database).
#include <pwd.h>
#include <unistd.h>
extern "C" int getpwuid_r(uid_t uid, passwd* entry, char* buffer, size_t size, passwd** result)
{
    static const auto real = reinterpret_cast<int (*)(uid_t, passwd*, char*, size_t, passwd**)>(dlsym(RTLD_NEXT, "getpwuid_r"));
    const int r = real(uid, entry, buffer, size, result);
    if (r == 0 && *result && getenv("FAKE_HOME"))
        entry->pw_dir = getenv("FAKE_HOME");
    return r;
}
