// Drives the D-Bus classes of the library: "driver orig LIBRARY" calls the published binary by address, "driver ours" the
// classes linked in. It is controlled over the session bus (test.Driver at /driver).
#include <chrono>
#include <cstdarg>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

#include "base/dbus.h"
#define private public
#include "menu_bus.h"
#include "now_playing.h"
#include "status_item.h"
#undef private

#include "base/dbus.h"
#include "base/event_loop.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dlfcn.h>
#include <new>
#include <optional>
#include <unistd.h>

using namespace cutefish;
using namespace cutefish::reef;
using base::dbus::Connection;

// The clock of the test, for the position.
static int64_t frozen = -1;
namespace std::chrono { inline namespace _V2 {
steady_clock::time_point steady_clock::now() noexcept
{
    if (frozen >= 0)
        return time_point(nanoseconds(frozen));
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return time_point(nanoseconds(int64_t(t.tv_sec) * 1000000000 + t.tv_nsec));
}
} }

static std::vector<std::string> events;
static void record(const char* format, ...)
{
    char text[400];
    va_list list;
    va_start(list, format);
    vsnprintf(text, sizeof text, format, list);
    va_end(list);
    events.push_back(text);
}

struct Api
{
    void (*menuNew)(void*, Connection*, void (*)(void*), void (*)(void*));
    void (*menuDelete)(void*);
    void (*menuSetPath)(void*, const std::string&);
    void (*menuSetItems)(void*, std::vector<MenuExporter::Item>);
    void (*playingNew)(void*, Connection*, std::string, void (*)(int, double));
    void (*playingDelete)(void*);
    void (*playingUpdate)(void*, const reef_ui_now_playing&);
    void (*statusNew)(void*, Connection*, void*, void (*)(void*, int, double, double), std::string);
    void (*statusDelete)(void*);
    void (*statusSetTitle)(void*, std::string);
    void (*statusSetToolTip)(void*, std::string);
    void (*statusSetIcon)(void*, const uint32_t*, int, int);
    void (*statusSetMenu)(void*, MenuExporter*);
    // The ones that the table's functions never call, as functions.
    bool (*menuHasChildren)(const void*, int);
    std::vector<int> (*menuChildren)(const void*, int);
    std::string (*playingStatus)(const void*);
    int64_t (*playingPosition)(const void*);
    void (*playingWithdraw)(void*);
};

static Api ours()
{
    Api a{};
    a.menuNew = [](void* self, Connection* c, void (*x)(void*), void (*y)(void*)) { new (self) MenuExporter(c, x, y); };
    a.menuDelete = [](void* self) { static_cast<MenuExporter*>(self)->~MenuExporter(); };
    a.menuSetPath = [](void* self, const std::string& path) { static_cast<MenuExporter*>(self)->setPath(path); };
    a.menuSetItems = [](void* self, std::vector<MenuExporter::Item> items) { static_cast<MenuExporter*>(self)->setItems(std::move(items)); };
    a.playingNew = [](void* self, Connection* c, std::string entry, void (*h)(int, double)) { new (self) NowPlaying(c, std::move(entry), h); };
    a.playingDelete = [](void* self) { static_cast<NowPlaying*>(self)->~NowPlaying(); };
    a.playingUpdate = [](void* self, const reef_ui_now_playing& s) { static_cast<NowPlaying*>(self)->update(s); };
    a.statusNew = [](void* self, Connection* c, void* x, void (*h)(void*, int, double, double), std::string id) { new (self) StatusItem(c, x, h, std::move(id)); };
    a.statusDelete = [](void* self) { static_cast<StatusItem*>(self)->~StatusItem(); };
    a.statusSetTitle = [](void* self, std::string t) { static_cast<StatusItem*>(self)->setTitle(std::move(t)); };
    a.statusSetToolTip = [](void* self, std::string t) { static_cast<StatusItem*>(self)->setToolTip(std::move(t)); };
    a.statusSetIcon = [](void* self, const uint32_t* p, int w, int h) { static_cast<StatusItem*>(self)->setIcon(p, w, h); };
    a.statusSetMenu = [](void* self, MenuExporter* m) { static_cast<StatusItem*>(self)->setMenu(m); };
    a.menuHasChildren = [](const void* self, int id) { return static_cast<const MenuExporter*>(self)->hasChildren(id); };
    a.menuChildren = [](const void* self, int id) { return static_cast<const MenuExporter*>(self)->children(id); };
    a.playingStatus = [](const void* self) { return static_cast<const NowPlaying*>(self)->status(); };
    a.playingPosition = [](const void* self) { return static_cast<const NowPlaying*>(self)->position(); };
    a.playingWithdraw = [](void* self) { static_cast<NowPlaying*>(self)->withdraw(); };
    return a;
}

static Api original(const char* path)
{
    void* library = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!library) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    char* base = static_cast<char*>(dlsym(library, "reef_appkit_host_interface")) - 0x6bc10;
    Api a{};
    const auto set = [base](auto& f, size_t offset) { f = reinterpret_cast<std::decay_t<decltype(f)>>(base + offset); };
    set(a.menuNew, 0xa7050);
    set(a.menuDelete, 0xa5bd0);
    set(a.menuSetPath, 0xa84c0);
    set(a.menuSetItems, 0xabd80);
    set(a.playingNew, 0xb6550);
    set(a.playingDelete, 0xb6760);
    set(a.playingUpdate, 0xbc620);
    set(a.statusNew, 0x105e40);
    set(a.statusDelete, 0x1009f0);
    set(a.statusSetTitle, 0x1016d0);
    set(a.statusSetToolTip, 0x101960);
    set(a.statusSetIcon, 0x101c20);
    set(a.statusSetMenu, 0x1020a0);
    set(a.menuHasChildren, 0xa5140);
    set(a.menuChildren, 0xa51b0);
    set(a.playingStatus, 0xb69f0);
    set(a.playingPosition, 0xb6930);
    set(a.playingWithdraw, 0xb6a80);
    return a;
}

static void activated(void* item) { record("activated %p", item); }
static void opened(void* item) { record("opened %p", item); }
static void command(int c, double value) { record("command %d %.17g", c, value); }
static void status(void* context, int action, double x, double y) { record("status %p %d %.17g %.17g", context, action, x, y); }

int main(int argc, char** argv)
{
    base::EventLoop loop;
    const Api api = argc > 2 ? original(argv[2]) : ours();
    static_assert(sizeof(MenuExporter) == 0x70 && sizeof(NowPlaying) == 0x160 && sizeof(StatusItem) == 0xe0);
    std::unique_ptr<Connection> connection = Connection::open(Connection::Bus::Session, &loop);
    if (!connection) {
        fprintf(stderr, "no bus\n");
        return 1;
    }
    std::map<int, void*> objects;
    std::map<int, void (*)(void*)> live;
    int next = 1;
    const auto make = [&](size_t size) {
        // Dirty memory, so that what a constructor leaves out shows.
        void* p = malloc(size);
        memset(p, 0xa5, size);
        objects[next] = p;
        return std::pair<int, void*>(next++, p);
    };
    base::dbus::Object driver(connection.get(), "/driver", "test.Driver");
    driver.addMethod("Ping", [] {});
    driver.addMethod("TakeEvents", [] { return std::exchange(events, {}); });
    driver.addMethod("SetClock", [](int64_t t) { frozen = t; });
    driver.addMethod("Quit", [&] { loop.quit(0); });
    driver.addMethod("Free", [&](int i) { free(objects[i]); objects.erase(i); });
    driver.addMethod("MenuNew", [&](bool bus, bool callbacks) {
        auto [i, p] = make(sizeof(MenuExporter));
        api.menuNew(p, bus ? connection.get() : nullptr, callbacks ? activated : nullptr, callbacks ? opened : nullptr);
        live[i] = api.menuDelete;
        return i;
    });
    driver.addMethod("MenuDelete", [&](int i) { live.erase(i); api.menuDelete(objects[i]); memset(objects[i], 0x5a, sizeof(MenuExporter)); });
    driver.addMethod("MenuSetPath", [&](int i, std::string path) { api.menuSetPath(objects[i], path); });
    driver.addMethod("MenuSetItems", [&](int i, std::vector<std::tuple<std::string, int64_t, int32_t, uint32_t, std::string, uint32_t>> list) {
        std::vector<MenuExporter::Item> items;
        for (auto& [title, item, depth, flags, key, modifiers] : list)
            items.push_back({title, reinterpret_cast<void*>(item), depth, flags, key, modifiers});
        api.menuSetItems(objects[i], std::move(items));
    });
    driver.addMethod("MenuHasChildren", [&](int i, int id) { return api.menuHasChildren(objects[i], id); });
    driver.addMethod("MenuChildren", [&](int i, int id) { return api.menuChildren(objects[i], id); });
    driver.addMethod("PlayingNew", [&](bool bus, std::string entry) {
        auto [i, p] = make(sizeof(NowPlaying));
        api.playingNew(p, bus ? connection.get() : nullptr, entry, command);
        live[i] = api.playingDelete;
        return i;
    });
    driver.addMethod("PlayingDelete", [&](int i) { live.erase(i); api.playingDelete(objects[i]); memset(objects[i], 0x5a, sizeof(NowPlaying)); });
    driver.addMethod("PlayingUpdate", [&](int i, std::vector<std::tuple<bool, std::string>> texts, double duration, double position, double rate, int status, uint32_t commands, uint64_t track) {
        const auto text = [&](size_t n) { return std::get<0>(texts[n]) ? std::get<1>(texts[n]).c_str() : nullptr; };
        reef_ui_now_playing state{text(0), text(1), text(2), text(3), text(4), duration, position, rate, status, commands, track};
        api.playingUpdate(objects[i], state);
    });
    driver.addMethod("PlayingStatus", [&](int i) { return api.playingStatus(objects[i]); });
    driver.addMethod("PlayingPosition", [&](int i) { return api.playingPosition(objects[i]); });
    driver.addMethod("PlayingWithdraw", [&](int i) { api.playingWithdraw(objects[i]); });
    driver.addMethod("StatusNew", [&](bool bus, int64_t context, bool handler, std::string id) {
        auto [i, p] = make(sizeof(StatusItem));
        api.statusNew(p, bus ? connection.get() : nullptr, reinterpret_cast<void*>(context), handler ? status : nullptr, id);
        live[i] = api.statusDelete;
        return i;
    });
    driver.addMethod("StatusDelete", [&](int i) { live.erase(i); api.statusDelete(objects[i]); memset(objects[i], 0x5a, sizeof(StatusItem)); });
    driver.addMethod("StatusSetTitle", [&](int i, std::string text) { api.statusSetTitle(objects[i], text); });
    driver.addMethod("StatusSetToolTip", [&](int i, std::string text) { api.statusSetToolTip(objects[i], text); });
    driver.addMethod("StatusSetIcon", [&](int i, bool given, std::vector<uint32_t> pixels, int width, int height) {
        api.statusSetIcon(objects[i], given ? pixels.data() : nullptr, width, height);
    });
    driver.addMethod("StatusSetMenu", [&](int i, int menu) { api.statusSetMenu(objects[i], menu ? static_cast<MenuExporter*>(objects[menu]) : nullptr); });
    driver.publish();
    connection->requestName("test.Driver");
    printf("ready %s\n", connection->uniqueName().c_str());
    fflush(stdout);
    const int code = loop.run();
    for (auto& [i, destroy] : live)
        destroy(objects[i]);
    for (auto& [i, object] : objects)
        free(object);
    return code;
}
