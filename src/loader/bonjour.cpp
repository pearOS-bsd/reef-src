// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DNS-SD for the system library, on Avahi through sd-bus (loaded when it is first needed).

#include "host.h"

#include <cstdio>
#include <cstring>
#include <deque>
#include <dlfcn.h>
#include <string>
#include <systemd/sd-bus.h>
#include <vector>

namespace cutefish::reef {

// The errors of dns_sd.h.
static constexpr int Unknown = -65537;
static constexpr int BadParameter = -65540;
static constexpr int NameConflict = -65548;
static constexpr int ServiceNotRunning = -65563;

struct SdBus
{
    decltype(&sd_bus_open_system) openSystem;
    decltype(&sd_bus_flush_close_unref) flushCloseUnref;
    decltype(&sd_bus_get_fd) getFd;
    decltype(&sd_bus_process) process;
    decltype(&sd_bus_call_method) callMethod;
    decltype(&sd_bus_message_new_method_call) newMethodCall;
    decltype(&sd_bus_message_append) append;
    decltype(&sd_bus_message_append_array) appendArray;
    decltype(&sd_bus_message_open_container) openContainer;
    decltype(&sd_bus_message_close_container) closeContainer;
    decltype(&sd_bus_call) call;
    decltype(&sd_bus_message_read) read;
    decltype(&sd_bus_message_read_array) readArray;
    decltype(&sd_bus_message_enter_container) enterContainer;
    decltype(&sd_bus_message_exit_container) exitContainer;
    decltype(&sd_bus_message_unref) messageUnref;
    decltype(&sd_bus_error_free) errorFree;
    decltype(&sd_bus_match_signal) matchSignal;
};

static SdBus functions;

// One function of libsystemd: whether it is there.
template<typename Function>
static bool load(void* library, Function& function, const char* name)
{
    function = reinterpret_cast<Function>(dlsym(library, name));
    return function != nullptr;
}

static const SdBus* sdBus()
{
    static const SdBus* const loaded = []() -> const SdBus* {
        void* library = dlopen("libsystemd.so.0", RTLD_NOW);
        if (!library)
            return nullptr;
        bool ok = true;
        ok &= load(library, functions.openSystem, "sd_bus_open_system");
        ok &= load(library, functions.flushCloseUnref, "sd_bus_flush_close_unref");
        ok &= load(library, functions.getFd, "sd_bus_get_fd");
        ok &= load(library, functions.process, "sd_bus_process");
        ok &= load(library, functions.callMethod, "sd_bus_call_method");
        ok &= load(library, functions.newMethodCall, "sd_bus_message_new_method_call");
        ok &= load(library, functions.append, "sd_bus_message_append");
        ok &= load(library, functions.appendArray, "sd_bus_message_append_array");
        ok &= load(library, functions.openContainer, "sd_bus_message_open_container");
        ok &= load(library, functions.closeContainer, "sd_bus_message_close_container");
        ok &= load(library, functions.call, "sd_bus_call");
        ok &= load(library, functions.read, "sd_bus_message_read");
        ok &= load(library, functions.readArray, "sd_bus_message_read_array");
        ok &= load(library, functions.enterContainer, "sd_bus_message_enter_container");
        ok &= load(library, functions.exitContainer, "sd_bus_message_exit_container");
        ok &= load(library, functions.messageUnref, "sd_bus_message_unref");
        ok &= load(library, functions.errorFree, "sd_bus_error_free");
        ok &= load(library, functions.matchSignal, "sd_bus_match_signal");
        if (!ok)
            return nullptr;
        return &functions;
    }();
    return loaded;
}

// One event of an operation, as the system library reads it.
struct BonjourEvent
{
    int32_t error;
    char name[256];
    char type[256];
    char domain[256];
    uint32_t added;
    int32_t interface;
    char host[256];
    uint16_t port;
    uint16_t length;
    uint8_t data[1300];
    uint16_t recordType;
    uint16_t recordClass;
};

enum class Operation { Register, Browse, Resolve, Query };

struct Client
{
    explicit Client(Operation operation)
        : operation(operation)
    {
    }
    virtual ~Client() = default;

    Operation operation;
    sd_bus* bus = nullptr;
    // The Avahi object of the operation.
    std::string path;
};

struct Requester : Client
{
    using Client::Client;
    std::deque<BonjourEvent> events;
};

struct Registration : Client
{
    Registration()
        : Client(Operation::Register)
    {
    }

    std::string name;
    std::string baseName;
    std::string type;
    std::string domain;
    uint16_t port = 0;
    std::vector<std::vector<uint8_t>> records;
    bool noAutoRename = false;
    int renames = 1;
    bool done = false;
    int result = 0;
};

static void destroy(Client* client)
{
    if (client->bus)
        sdBus()->flushCloseUnref(client->bus);
    delete client;
}

static int addService(Registration& service)
{
    const SdBus* bus = sdBus();
    sd_bus_message* message = nullptr;
    int r = bus->newMethodCall(service.bus, &message, "org.freedesktop.Avahi", service.path.c_str(), "org.freedesktop.Avahi.EntryGroup", "AddService");
    if (r < 0)
        return Unknown;
    bus->append(message, "iiussssq", -1, -1, 0u, service.name.c_str(), service.type.c_str(), service.domain.c_str(), "", service.port);
    bus->openContainer(message, 'a', "ay");
    for (const std::vector<uint8_t>& record : service.records)
        bus->appendArray(message, 'y', record.data(), record.size());
    bus->closeContainer(message);
    sd_bus_error error = {};
    r = bus->call(service.bus, message, 0, &error, nullptr);
    bus->messageUnref(message);
    const bool collision = error.name && strcmp(error.name, "org.freedesktop.Avahi.CollisionError") == 0;
    bus->errorFree(&error);
    if (r < 0)
        return collision ? NameConflict : Unknown;
    r = bus->callMethod(service.bus, "org.freedesktop.Avahi", service.path.c_str(), "org.freedesktop.Avahi.EntryGroup", "Commit", &error, nullptr, "");
    if (r < 0) {
        bus->errorFree(&error);
        return Unknown;
    }
    return 0;
}

static bool nextEvent(Client* client, BonjourEvent* event)
{
    const SdBus* bus = sdBus();
    while (bus->process(client->bus, nullptr) > 0) {
    }
    if (client->operation == Operation::Register) {
        Registration* service = static_cast<Registration*>(client);
        if (!service->done)
            return false;
        service->done = false;
        event->error = service->result;
        snprintf(event->name, sizeof event->name, "%s", service->name.c_str());
        snprintf(event->type, sizeof event->type, "%s.", service->type.c_str());
        snprintf(event->domain, sizeof event->domain, "%s.", service->domain.empty() ? "local" : service->domain.c_str());
        return true;
    }
    Requester* requester = static_cast<Requester*>(client);
    if (requester->events.empty())
        return false;
    *event = requester->events.front();
    requester->events.pop_front();
    return true;
}

static std::string withoutTrailingDots(const char* name)
{
    std::string text = name ? name : "";
    while (!text.empty() && text.back() == '.')
        text.pop_back();
    return text;
}

static int bonjourSocket(Client* client) { return sdBus()->getFd(client->bus); }

// A new name after a collision: "Name (2)".
static bool rename(Registration& service)
{
    if (service.noAutoRename)
        return false;
    const SdBus* bus = sdBus();
    sd_bus_error error = {};
    bus->callMethod(service.bus, "org.freedesktop.Avahi", service.path.c_str(), "org.freedesktop.Avahi.EntryGroup", "Reset", &error, nullptr, "");
    bus->errorFree(&error);
    service.renames++;
    service.name = service.baseName + " (" + std::to_string(service.renames) + ")";
    return addService(service) == 0;
}

static int stateChanged(sd_bus_message* message, void* data, sd_bus_error*)
{
    Registration* service = static_cast<Registration*>(data);
    int state = 0;
    const char* error = nullptr;
    if (sdBus()->read(message, "is", &state, &error) < 0)
        return 0;
    if (state == 2) {
        service->done = true;
        service->result = 0;
    } else if (state == 3) {
        if (!rename(*service)) {
            service->done = true;
            service->result = NameConflict;
        }
    } else if (state == 4) {
        service->done = true;
        service->result = Unknown;
    }
    return 0;
}

static int registerService(const char* name, const char* type, const char* domain, uint16_t port, const uint8_t* records, size_t length,
                           int noAutoRename, Client** client)
{
    if (!type || !*type)
        return BadParameter;
    const SdBus* bus = sdBus();
    if (!bus)
        return ServiceNotRunning;
    Registration* service = new Registration;
    if (bus->openSystem(&service->bus) < 0) {
        service->bus = nullptr;
        delete service;
        return ServiceNotRunning;
    }
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message* reply = nullptr;
    const char* path = nullptr;
    if (bus->callMethod(service->bus, "org.freedesktop.Avahi", "/", "org.freedesktop.Avahi.Server", "EntryGroupNew", &error, &reply, "") < 0 ||
        bus->read(reply, "o", &path) < 0) {
        bus->errorFree(&error);
        if (reply)
            bus->messageUnref(reply);
        destroy(service);
        return ServiceNotRunning;
    }
    service->path = path;
    bus->messageUnref(reply);
    service->baseName = name ? name : "";
    if (service->baseName.empty()) {
        reply = nullptr;
        const char* host = nullptr;
        if (bus->callMethod(service->bus, "org.freedesktop.Avahi", "/", "org.freedesktop.Avahi.Server", "GetHostName", &error, &reply, "") >= 0 &&
            bus->read(reply, "s", &host) >= 0)
            service->baseName = host;
        bus->errorFree(&error);
        if (reply)
            bus->messageUnref(reply);
    }
    service->name = service->baseName;
    service->type = withoutTrailingDots(type);
    service->domain = withoutTrailingDots(domain);
    if (service->domain == "local")
        service->domain.clear();
    service->port = port;
    std::vector<std::vector<uint8_t>> parsed;
    if (length && records) {
        // Each string has its length before it.
        for (size_t at = 0; at < length;) {
            const size_t size = records[at];
            if (at + 1 + size > length)
                break;
            parsed.emplace_back(records + at + 1, records + at + 1 + size);
            at += 1 + size;
        }
    }
    service->records = std::move(parsed);
    service->noAutoRename = noAutoRename != 0;
    bus->matchSignal(service->bus, nullptr, "org.freedesktop.Avahi", service->path.c_str(), "org.freedesktop.Avahi.EntryGroup", "StateChanged",
                     stateChanged, service);
    int r = addService(*service);
    if (r == NameConflict) {
        if (!rename(*service)) {
            destroy(service);
            return r;
        }
    } else if (r != 0) {
        destroy(service);
        return r;
    }
    *client = service;
    return 0;
}

static std::string withTrailingDot(const std::string& name)
{
    if (name.empty() || name.back() == '.')
        return name;
    return name + ".";
}

static Requester* newRequester(Operation operation)
{
    const SdBus* bus = sdBus();
    if (!bus)
        return nullptr;
    Requester* requester = new Requester(operation);
    if (bus->openSystem(&requester->bus) >= 0)
        return requester;
    requester->bus = nullptr;
    delete requester;
    return nullptr;
}

static void recordFound(sd_bus_message* message, Requester* requester, bool added)
{
    const SdBus* bus = sdBus();
    int32_t interface = 0;
    int32_t protocol = 0;
    const char* name = nullptr;
    uint16_t recordClass = 0;
    uint16_t recordType = 0;
    if (bus->read(message, "iisqq", &interface, &protocol, &name, &recordClass, &recordType) < 0)
        return;
    const void* data = nullptr;
    size_t size = 0;
    if (bus->readArray(message, 'y', &data, &size) < 0 || size > sizeof BonjourEvent::data || protocol == 1)
        return;
    BonjourEvent event = {};
    event.interface = interface;
    event.added = added;
    snprintf(event.name, sizeof event.name, "%s", withTrailingDot(name).c_str());
    event.recordType = recordType;
    event.recordClass = recordClass;
    event.length = uint16_t(size);
    if (size)
        memcpy(event.data, data, size);
    requester->events.push_back(event);
}

static int recordAdded(sd_bus_message* message, void* data, sd_bus_error*)
{
    recordFound(message, static_cast<Requester*>(data), true);
    return 0;
}

static int recordRemoved(sd_bus_message* message, void* data, sd_bus_error*)
{
    recordFound(message, static_cast<Requester*>(data), false);
    return 0;
}

static int failed(sd_bus_message*, void* data, sd_bus_error*)
{
    BonjourEvent event = {};
    event.error = Unknown;
    static_cast<Requester*>(data)->events.push_back(event);
    return 0;
}

static int queryRecord(const char* name, uint16_t type, uint16_t recordClass, int interface, Client** client)
{
    if (!name || !*name)
        return BadParameter;
    Requester* requester = newRequester(Operation::Query);
    if (requester) {
        const SdBus* bus = sdBus();
        bus->matchSignal(requester->bus, nullptr, "org.freedesktop.Avahi", nullptr, "org.freedesktop.Avahi.RecordBrowser", "ItemNew", recordAdded, requester);
        bus->matchSignal(requester->bus, nullptr, "org.freedesktop.Avahi", nullptr, "org.freedesktop.Avahi.RecordBrowser", "ItemRemove", recordRemoved, requester);
        bus->matchSignal(requester->bus, nullptr, "org.freedesktop.Avahi", nullptr, "org.freedesktop.Avahi.RecordBrowser", "Failure", failed, requester);
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message* reply = nullptr;
        const char* path = nullptr;
        if (bus->callMethod(requester->bus, "org.freedesktop.Avahi", "/", "org.freedesktop.Avahi.Server", "RecordBrowserNew", &error, &reply, "iisqqu",
                            interface, -1, withoutTrailingDots(name).c_str(), recordClass, type, 0u) >= 0 &&
            bus->read(reply, "o", &path) >= 0) {
            requester->path = path;
            bus->messageUnref(reply);
            *client = requester;
            return 0;
        }
        bus->errorFree(&error);
        destroy(requester);
    }
    return ServiceNotRunning;
}

static void serviceFound(sd_bus_message* message, Requester* requester, bool added)
{
    int32_t interface = 0;
    int32_t protocol = 0;
    const char* name = nullptr;
    const char* type = nullptr;
    const char* domain = nullptr;
    uint32_t flags = 0;
    if (sdBus()->read(message, "iisssu", &interface, &protocol, &name, &type, &domain, &flags) < 0 || protocol == 1)
        return;
    BonjourEvent event = {};
    event.interface = interface;
    event.added = added;
    snprintf(event.name, sizeof event.name, "%s", name);
    snprintf(event.type, sizeof event.type, "%s", withTrailingDot(type).c_str());
    snprintf(event.domain, sizeof event.domain, "%s", withTrailingDot(domain).c_str());
    requester->events.push_back(event);
}

static int serviceAdded(sd_bus_message* message, void* data, sd_bus_error*)
{
    serviceFound(message, static_cast<Requester*>(data), true);
    return 0;
}

static int serviceRemoved(sd_bus_message* message, void* data, sd_bus_error*)
{
    serviceFound(message, static_cast<Requester*>(data), false);
    return 0;
}

static int resolved(sd_bus_message* message, void* data, sd_bus_error*)
{
    const SdBus* bus = sdBus();
    int32_t interface = 0;
    int32_t protocol = 0;
    const char* name = nullptr;
    const char* type = nullptr;
    const char* domain = nullptr;
    const char* host = nullptr;
    int32_t addressProtocol = 0;
    const char* address = nullptr;
    uint16_t port = 0;
    if (bus->read(message, "iissssisq", &interface, &protocol, &name, &type, &domain, &host, &addressProtocol, &address, &port) < 0)
        return 0;
    BonjourEvent event = {};
    event.added = 1;
    event.interface = interface;
    snprintf(event.name, sizeof event.name, "%s", name);
    snprintf(event.type, sizeof event.type, "%s", withTrailingDot(type).c_str());
    snprintf(event.domain, sizeof event.domain, "%s", withTrailingDot(domain).c_str());
    snprintf(event.host, sizeof event.host, "%s", withTrailingDot(host).c_str());
    event.port = port;
    if (bus->enterContainer(message, 'a', "ay") > 0) {
        const void* record = nullptr;
        size_t size = 0;
        while (bus->readArray(message, 'y', &record, &size) > 0 && size < 256 && event.length + 1 + size <= sizeof event.data) {
            event.data[event.length++] = uint8_t(size);
            memcpy(event.data + event.length, record, size);
            event.length += size;
        }
        bus->exitContainer(message);
    }
    static_cast<Requester*>(data)->events.push_back(event);
    return 0;
}

static int resolve(int interface, const char* name, const char* type, const char* domain, Client** client)
{
    if (!interface || !name || !*name)
        return BadParameter;
    Requester* requester = newRequester(Operation::Resolve);
    if (!requester)
        return ServiceNotRunning;
    const SdBus* bus = sdBus();
    std::string zone = withoutTrailingDots(domain);
    bus->matchSignal(requester->bus, nullptr, "org.freedesktop.Avahi", nullptr, "org.freedesktop.Avahi.ServiceResolver", "Found", resolved, requester);
    bus->matchSignal(requester->bus, nullptr, "org.freedesktop.Avahi", nullptr, "org.freedesktop.Avahi.ServiceResolver", "Failure", failed, requester);
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message* reply = nullptr;
    const char* path = nullptr;
    const char* domainName = zone == "local" ? "" : zone.c_str();
    if (bus->callMethod(requester->bus, "org.freedesktop.Avahi", "/", "org.freedesktop.Avahi.Server", "ServiceResolverNew", &error, &reply, "iisssiu",
                        interface, -1, name, withoutTrailingDots(type).c_str(), domainName, -1, 0u) < 0 ||
        bus->read(reply, "o", &path) < 0) {
        bus->errorFree(&error);
        destroy(requester);
        return ServiceNotRunning;
    }
    requester->path = path;
    bus->messageUnref(reply);
    *client = requester;
    return 0;
}

static int browse(const char* type, const char* domain, int interface, Client** client)
{
    if (!type || !*type)
        return BadParameter;
    Requester* requester = newRequester(Operation::Browse);
    if (!requester)
        return ServiceNotRunning;
    const SdBus* bus = sdBus();
    std::string zone = withoutTrailingDots(domain);
    bus->matchSignal(requester->bus, nullptr, "org.freedesktop.Avahi", nullptr, "org.freedesktop.Avahi.ServiceBrowser", "ItemNew", serviceAdded, requester);
    bus->matchSignal(requester->bus, nullptr, "org.freedesktop.Avahi", nullptr, "org.freedesktop.Avahi.ServiceBrowser", "ItemRemove", serviceRemoved, requester);
    bus->matchSignal(requester->bus, nullptr, "org.freedesktop.Avahi", nullptr, "org.freedesktop.Avahi.ServiceBrowser", "Failure", failed, requester);
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message* reply = nullptr;
    const char* path = nullptr;
    const char* domainName = zone == "local" ? "" : zone.c_str();
    if (bus->callMethod(requester->bus, "org.freedesktop.Avahi", "/", "org.freedesktop.Avahi.Server", "ServiceBrowserNew", &error, &reply, "iissu",
                        interface, -1, withoutTrailingDots(type).c_str(), domainName, 0u) < 0 ||
        bus->read(reply, "o", &path) < 0) {
        bus->errorFree(&error);
        destroy(requester);
        return ServiceNotRunning;
    }
    requester->path = path;
    bus->messageUnref(reply);
    *client = requester;
    return 0;
}

void addBonjourServices(HostServices& services)
{
    services.bonjourRegister = reinterpret_cast<void*>(registerService);
    services.bonjourSocket = reinterpret_cast<void*>(bonjourSocket);
    services.bonjourQuery = reinterpret_cast<void*>(queryRecord);
    services.bonjourNext = reinterpret_cast<void*>(nextEvent);
    services.bonjourDestroy = reinterpret_cast<void*>(destroy);
    services.bonjourBrowse = reinterpret_cast<void*>(browse);
    services.bonjourResolve = reinterpret_cast<void*>(resolve);
}

} // namespace cutefish::reef
