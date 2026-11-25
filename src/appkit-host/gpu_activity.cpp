// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The log of GPU work that runs long: a fence follows the work, and a thread watches the fences.

#include "gpu_activity.h"

#include "host.h"

#include "ui/frame_submission.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <fcntl.h>
#include <mutex>
#include <pwd.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <utility>

namespace cutefish::reef {

namespace {

struct Work
{
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSyncKHR sync = EGL_NO_SYNC_KHR;
    std::string name;
    std::string detail;
    // When it was issued: milliseconds of the monotonic clock, and the time of day.
    double issued = 0;
    timespec time = {};
    pid_t thread = 0;
    bool logged = false;
};

// Milliseconds: work that took longer is logged when it is done, and while it runs.
constexpr double slow = 100;
constexpr double stuck = 1000;
// The log starts anew beyond it; the old one stays next to it.
constexpr off_t logSize = 256 * 1024;

} // namespace

static std::mutex mutex;
static std::condition_variable workIssued;
static std::deque<Work> pending;
static bool watching = false;

static double milliseconds()
{
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000.0 + time.tv_nsec / 1000000.0;
}

static std::string formattedTime(const timespec& time)
{
    tm parts;
    localtime_r(&time.tv_sec, &parts);
    char text[64];
    const size_t length = strftime(text, sizeof text, "%Y-%m-%dT%H:%M:%S", &parts);
    char zone[16];
    strftime(zone, sizeof zone, "%z", &parts);
    snprintf(text + length, sizeof text - length, ".%06ld%s", time.tv_nsec / 1000, zone);
    return text;
}

static std::string processName()
{
    char name[64] = {};
    const int fd = open("/proc/self/comm", O_RDONLY | O_CLOEXEC);
    if (fd >= 0) {
        const ssize_t size = read(fd, name, sizeof name - 1);
        close(fd);
        if (size > 0 && name[size - 1] == '\n')
            name[size - 1] = 0;
    }
    return std::string(name) + " pid " + std::to_string(getpid());
}

static std::string homeFolder()
{
    const char* folder = secure_getenv("CFFIXED_USER_HOME");
    if (folder && *folder == '/')
        return folder;
    const passwd* user = getpwuid(getuid());
    if (!user || !user->pw_dir || !*user->pw_dir)
        return std::string();
    return user->pw_dir;
}

// Empty without a home folder.
static std::string logPath()
{
    const std::string home = homeFolder();
    if (home.empty())
        return std::string();
    std::string directory = home;
    for (const char* name : {".cache", "cutefish-reef"}) {
        directory += std::string("/") + name;
        mkdir(directory.c_str(), 0700);
    }
    return directory + "/gpu-activity.log";
}

static void logLine(const std::string& line)
{
    static const std::string path = logPath();
    if (path.empty())
        return;
    struct stat info;
    if (stat(path.c_str(), &info) == 0 && info.st_size > logSize)
        rename(path.c_str(), (path + ".old").c_str());
    const int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, 0600);
    if (fd < 0)
        return;
    const std::string text = line + "\n";
    [[maybe_unused]] const ssize_t written = write(fd, text.data(), text.size());
    close(fd);
}

static std::string prefix(const std::string& process, const Work& work, const char* state)
{
    return formattedTime(work.time) + " " + process + " tid " + std::to_string(work.thread) + " " + state + " ";
}

static std::string description(double now, const Work& work)
{
    return work.name + " for " + std::to_string(long(now - work.issued)) + " ms: " + work.detail;
}

static std::string otherWork(const std::deque<Work>& all, double now, const Work& work)
{
    std::string text;
    for (const Work& other : all) {
        if (&other == &work)
            continue;
        text += (text.empty() ? "" : "; ") + description(now, other);
    }
    return text.empty() ? "nothing else" : text;
}

// The thread that waits for the fences.
static void watch()
{
    static const auto destroySync = reinterpret_cast<PFNEGLDESTROYSYNCKHRPROC>(eglGetProcAddress("eglDestroySyncKHR"));
    static const auto clientWaitSync = reinterpret_cast<PFNEGLCLIENTWAITSYNCKHRPROC>(eglGetProcAddress("eglClientWaitSyncKHR"));
    static const std::string process = processName();
    std::unique_lock<std::mutex> lock(mutex);
    while (true) {
        workIssued.wait(lock, [] { return !pending.empty(); });
        const double now = milliseconds();
        for (auto work = pending.begin(); work != pending.end();) {
            const EGLint status = clientWaitSync(work->display, work->sync, 0, 0);
            const double elapsed = now - work->issued;
            if (status == EGL_TIMEOUT_EXPIRED_KHR) {
                if (elapsed > stuck && !work->logged) {
                    work->logged = true;
                    logLine(prefix(process, *work, "RUNNING") + description(now, *work) + " | also running: "
                            + otherWork(pending, now, *work));
                }
                ++work;
                continue;
            }
            if (elapsed > slow)
                logLine(prefix(process, *work, "SLOW") + description(now, *work) + " | also running: "
                        + otherWork(pending, now, *work));
            destroySync(work->display, work->sync);
            work = pending.erase(work);
        }
        if (pending.empty())
            continue;
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        lock.lock();
    }
}

// 314
void hostGpuWorkIssued(const char* name, const char* detail)
{
    static const auto createSync = reinterpret_cast<PFNEGLCREATESYNCKHRPROC>(eglGetProcAddress("eglCreateSyncKHR"));
    const EGLDisplay display = eglGetCurrentDisplay();
    if (!createSync || !display || !eglGetCurrentContext())
        return;
    Work work;
    work.display = display;
    work.sync = createSync(display, EGL_SYNC_FENCE_KHR, nullptr);
    if (!work.sync)
        return;
    work.name = name ? name : "";
    work.detail = detail ? detail : "";
    work.issued = milliseconds();
    clock_gettime(CLOCK_REALTIME, &work.time);
    work.thread = gettid();
    std::lock_guard<std::mutex> lock(mutex);
    if (!watching) {
        watching = true;
        std::thread(watch).detach();
    }
    pending.push_back(std::move(work));
    workIssued.notify_one();
}

static void frameDrawn(const ui::DrawnFrame& frame)
{
    char detail[160];
    snprintf(detail, sizeof detail, "%dx%d, %d glass panes over %.0f device pixels, largest blur %.1f", frame.width, frame.height,
             frame.glassPanes, frame.glassPixels, frame.largestBlur);
    hostGpuWorkIssued("window frame", detail);
}

void logDrawnFrames()
{
    ui::setFrameDrawnHandler(frameDrawn);
}

// 315
void hostGpuWorkMeasured(const char* name, const char* detail, int running, int thread, double duration, double time)
{
    static const std::string process = processName();
    Work work;
    work.name = name ? name : "";
    work.detail = detail ? detail : "";
    work.thread = thread;
    work.time.tv_sec = time_t(time);
    work.time.tv_nsec = long((time - work.time.tv_sec) * 1e9);
    const double now = milliseconds();
    work.issued = now - duration;
    std::lock_guard<std::mutex> lock(mutex);
    logLine(prefix(process, work, running ? "RUNNING" : "SLOW") + description(now, work) + " | also running: "
            + otherWork(pending, now, work));
}

// 316
int hostThreadId()
{
    return gettid();
}

} // namespace cutefish::reef
