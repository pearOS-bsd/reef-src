// EGL fences, the clock, sleeping, the process and the user, all made up and logged.
#include "ui/frame_submission.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <map>
#include <mutex>
#include <pwd.h>
#include <sys/syscall.h>
#include <time.h>
#include <thread>
#include <unistd.h>

static pid_t realPid() { return pid_t(syscall(SYS_getpid)); }
static pid_t realTid() { return pid_t(syscall(SYS_gettid)); }
extern "C" pid_t getpid() { return 4242; }
extern "C" pid_t gettid() { return realPid() == realTid() ? 4242 : 5000; }
#include "stub_log.h"

static std::mutex m;
static std::condition_variable cv;
static int64_t nowNs = 5000000000000ll;
static const int64_t realOffset = 1785000000000000000ll;
static int arrived = 0, credits = 0, live = 0;
static bool on = false;
static std::map<int, int64_t> done;
static int created = 0;
static int64_t nextLife = 0;
static int nextResult = EGL_CONDITION_SATISFIED;
static bool nextFails = false;
static uintptr_t display = 0x77, context = 0x88;
static const char* missing = "";
static cutefish::ui::FrameDrawnHandler handler;

extern "C" {
void fake_gpu(int enable) { on = enable; }
void fake_gpu_advance(int64_t ns) { std::lock_guard l(m); nowNs += ns; }
void fake_gpu_next(int64_t life, int fails) { nextLife = life; nextFails = fails; }
void fake_gpu_context(uintptr_t d, uintptr_t c) { display = d; context = c; }
void fake_gpu_missing(const char* name) { missing = name; }
void* fake_gpu_handler() { return reinterpret_cast<void*>(handler); }
// Lets the watching thread sleep once, until it is asleep again or has nothing to watch.
void fake_gpu_step()
{
    std::unique_lock l(m);
    const int target = arrived + 1;
    credits++;
    cv.notify_all();
    cv.wait_for(l, std::chrono::seconds(5), [&] { return arrived >= target || live == 0; });
    l.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(live ? 0 : 5));
}
void fake_gpu_settle()
{
    std::unique_lock l(m);
    cv.wait_for(l, std::chrono::seconds(5), [&] { return arrived > 0 || live == 0; });
}
int fake_gpu_live() { std::lock_guard l(m); return live; }
// For a first sleep: the thread is asleep, or has nothing.
void fake_gpu_wait_asleep(int before)
{
    std::unique_lock l(m);
    cv.wait_for(l, std::chrono::seconds(5), [&] { return arrived > before || live == 0; });
    l.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(live ? 0 : 5));
}
int fake_gpu_arrived() { std::lock_guard l(m); return arrived; }

int clock_gettime(clockid_t id, timespec* t)
{
    if (!on)
        return int(syscall(SYS_clock_gettime, id, t));
    const int64_t v = nowNs + (id == CLOCK_REALTIME ? realOffset : 0);
    t->tv_sec = v / 1000000000;
    t->tv_nsec = v % 1000000000;
    return 0;
}
int nanosleep(const timespec* t, timespec* rest)
{
    if (!on || realPid() == realTid())
        return int(syscall(SYS_nanosleep, t, rest));
    std::unique_lock l(m);
    note("nanosleep %lld.%09ld", (long long)t->tv_sec, t->tv_nsec);
    arrived++;
    cv.notify_all();
    cv.wait(l, [] { return credits > 0; });
    credits--;
    return 0;
}

static EGLSyncKHR createSync(EGLDisplay d, EGLenum type, const EGLint* attributes)
{
    std::lock_guard l(m);
    if (nextFails) {
        note("eglCreateSyncKHR %llx %x %d -> none", (unsigned long long)uintptr_t(d), type, attributes != nullptr);
        return EGL_NO_SYNC_KHR;
    }
    const int id = ++created;
    done[id] = nextLife < 0 ? nextLife : nowNs + nextLife;
    live++;
    note("eglCreateSyncKHR %llx %x %d -> %d", (unsigned long long)uintptr_t(d), type, attributes != nullptr, id);
    return reinterpret_cast<EGLSyncKHR>(uintptr_t(id));
}
static EGLBoolean destroySync(EGLDisplay d, EGLSyncKHR s)
{
    std::lock_guard l(m);
    note("eglDestroySyncKHR %llx %d", (unsigned long long)uintptr_t(d), int(uintptr_t(s)));
    live--;
    cv.notify_all();
    return EGL_TRUE;
}
static EGLint clientWaitSync(EGLDisplay d, EGLSyncKHR s, EGLint flags, EGLTimeKHR timeout)
{
    std::lock_guard l(m);
    const int64_t end = done[int(uintptr_t(s))];
    const EGLint r = end == -1 ? EGL_FALSE : nowNs >= end ? EGL_CONDITION_SATISFIED_KHR : EGL_TIMEOUT_EXPIRED_KHR;
    note("eglClientWaitSyncKHR %llx %d %d %llu -> %x", (unsigned long long)uintptr_t(d), int(uintptr_t(s)), flags, (unsigned long long)timeout, r);
    return r;
}
__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char* name)
{
    note("eglGetProcAddress %s", name);
    if (strcmp(name, missing) == 0)
        return nullptr;
    if (strcmp(name, "eglCreateSyncKHR") == 0) return reinterpret_cast<__eglMustCastToProperFunctionPointerType>(createSync);
    if (strcmp(name, "eglDestroySyncKHR") == 0) return reinterpret_cast<__eglMustCastToProperFunctionPointerType>(destroySync);
    if (strcmp(name, "eglClientWaitSyncKHR") == 0) return reinterpret_cast<__eglMustCastToProperFunctionPointerType>(clientWaitSync);
    return nullptr;
}
EGLDisplay eglGetCurrentDisplay() { return reinterpret_cast<EGLDisplay>(display); }
EGLContext eglGetCurrentContext() { return reinterpret_cast<EGLContext>(context); }

passwd* getpwuid(uid_t uid)
{
    static passwd entry;
    const char* kind = getenv("FAKE_USER");
    note("getpwuid %s", kind ? kind : "-");
    if (!kind)
        return nullptr;
    entry.pw_dir = strcmp(kind, "null") == 0 ? nullptr : const_cast<char*>(kind);
    return &entry;
}
}

namespace cutefish::ui {
__attribute__((visibility("default"))) void setFrameDrawnHandler(FrameDrawnHandler h)
{
    note("setFrameDrawnHandler %d", h != nullptr);
    handler = h;
}
}
