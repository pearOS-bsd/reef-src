// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Real-time threads through RealtimeKit on the system bus, with libsystemd loaded when it is first needed.

#include "host.h"

#include <atomic>
#include <cstdint>
#include <dlfcn.h>
#include <mutex>
#include <sys/resource.h>

namespace cutefish::reef {

namespace {

struct BusError
{
    const char* name;
    const char* message;
    int needFree;
};

// The functions of libsystemd that are used.
struct Systemd
{
    int (*open)(void** bus);
    void* (*close)(void* bus);
    int (*call)(void* bus, const char* destination, const char* path, const char* interface, const char* member, BusError* error, void** reply,
                const char* types, ...);
    int (*read)(void* message, const char* types, ...);
    void* (*unref)(void* message);
    void (*freeError)(BusError* error);
};

} // namespace

static Systemd systemd;
static std::atomic<bool> unavailable;
static std::mutex mutex;

static const Systemd* loadSystemd()
{
    void* library = dlopen("libsystemd.so.0", RTLD_NOW);
    if (!library)
        return nullptr;
    systemd.open = reinterpret_cast<decltype(systemd.open)>(dlsym(library, "sd_bus_open_system"));
    systemd.close = reinterpret_cast<decltype(systemd.close)>(dlsym(library, "sd_bus_flush_close_unref"));
    systemd.call = reinterpret_cast<decltype(systemd.call)>(dlsym(library, "sd_bus_call_method"));
    systemd.read = reinterpret_cast<decltype(systemd.read)>(dlsym(library, "sd_bus_message_read"));
    systemd.unref = reinterpret_cast<decltype(systemd.unref)>(dlsym(library, "sd_bus_message_unref"));
    systemd.freeError = reinterpret_cast<decltype(systemd.freeError)>(dlsym(library, "sd_bus_error_free"));
    if (!systemd.open || !systemd.close || !systemd.call || !systemd.read || !systemd.unref || !systemd.freeError)
        return nullptr;
    return &systemd;
}

// A property of RealtimeKit.
static bool property(const Systemd* functions, void* bus, const char* name, const char* type, void* value)
{
    BusError error = {};
    void* reply = nullptr;
    bool found = false;
    if (functions->call(bus, "org.freedesktop.RealtimeKit1", "/org/freedesktop/RealtimeKit1", "org.freedesktop.DBus.Properties", "Get", &error, &reply,
                        "ss", "org.freedesktop.RealtimeKit1", name) >= 0)
        found = functions->read(reply, "v", type, value) >= 0;
    if (reply)
        functions->unref(reply);
    functions->freeError(&error);
    return found;
}

static int makeThreadRealtime(uint64_t thread)
{
    if (unavailable)
        return -1;
    std::lock_guard lock(mutex);
    static const Systemd* const functions = loadSystemd();
    void* bus = nullptr;
    if (!functions || functions->open(&bus) < 0) {
        unavailable = true;
        return -1;
    }
    int priority = 0;
    int64_t limit = 0;
    if (!property(functions, bus, "MaxRealtimePriority", "i", &priority) || priority <= 0 ||
        !property(functions, bus, "RTTimeUSecMax", "x", &limit) || limit <= 0) {
        functions->close(bus);
        unavailable = true;
        return -1;
    }
    // RealtimeKit only accepts threads that cannot run longer than it allows.
    rlimit current = {};
    if (getrlimit(RLIMIT_RTTIME, &current) == 0 && (current.rlim_max == RLIM_INFINITY || rlim_t(limit) < current.rlim_max)) {
        current.rlim_cur = current.rlim_max = rlim_t(limit);
        setrlimit(RLIMIT_RTTIME, &current);
    }
    BusError error = {};
    void* reply = nullptr;
    const int result = functions->call(bus, "org.freedesktop.RealtimeKit1", "/org/freedesktop/RealtimeKit1", "org.freedesktop.RealtimeKit1",
                                       "MakeThreadRealtime", &error, &reply, "tu", thread, priority);
    if (reply)
        functions->unref(reply);
    functions->freeError(&error);
    functions->close(bus);
    return result < 0 ? -1 : 0;
}

void addRealtimeServices(HostServices& services)
{
    services.makeThreadRealtime = makeThreadRealtime;
}

} // namespace cutefish::reef
