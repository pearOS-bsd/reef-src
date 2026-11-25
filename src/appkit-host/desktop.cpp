// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The desktop around the program: types of files and what opens them, the file manager, the running instance of the
// application, what it plays.

#include "desktop.h"

#include "host.h"

#include "base/desktop_entry.h"
#include "base/mime_apps.h"
#include "base/mime_database.h"
#include "base/url.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <fstream>
#include <optional>
#include <spawn.h>
#include <sstream>
#include <thread>
#include <unistd.h>

namespace cutefish::reef {

static constexpr const char* instancePath = "/org/cutefish/Application";
static constexpr const char* instanceInterface = "org.cutefish.Application";
static constexpr const char* fileManagerService = "org.freedesktop.FileManager1";
static constexpr const char* fileManagerPath = "/org/freedesktop/FileManager1";

// On the bus as long as the process lives.
static base::dbus::Object* instance = nullptr;
static bool shownInDock = true;

int copyString(const std::string& text, char* buffer, int size)
{
    if (buffer && size > 0) {
        const size_t length = std::min(text.size(), size_t(size - 1));
        memcpy(buffer, text.data(), length);
        buffer[length] = 0;
    }
    return int(text.size());
}

// 84
int hostMimeTypeForFile(const char* path, char* buffer, int size)
{
    return copyString(path ? base::mime::typeForFile(path) : std::string(), buffer, size);
}

// 85
int hostMimeIconName(const char* type, int generic, char* buffer, int size)
{
    if (!type)
        return copyString(std::string(), buffer, size);
    return copyString(generic ? base::mime::genericIconName(type) : base::mime::iconName(type), buffer, size);
}

// 86
int hostDefaultApplication(const char* type, char* buffer, int size)
{
    if (!type)
        return copyString(std::string(), buffer, size);
    return copyString(base::mimeapps::defaultApplication(base::mimeapps::listPaths(), type), buffer, size);
}

// 201
int hostShowInFileManager(const char* const* paths, unsigned count, int select)
{
    base::dbus::Connection* bus = sessionBus();
    if (!bus || !paths || count == 0)
        return 0;
    std::vector<std::string> urls;
    for (unsigned i = 0; i < count; i++) {
        if (paths[i] && *paths[i])
            urls.push_back(base::url::fromLocalFile(paths[i]));
    }
    base::dbus::Message call = bus->newMethodCall(fileManagerService, fileManagerPath, fileManagerService, select ? "ShowItems" : "ShowFolders");
    call.append(urls, std::string());
    return bus->send(call);
}

// The bus name of an application. The elements of one do not start with a digit.
static std::string instanceName(const std::string& identifier)
{
    std::string name = instanceInterface;
    std::string element;
    const auto append = [&] {
        if (element.empty())
            return;
        if (element[0] >= '0' && element[0] <= '9')
            element.insert(element.begin(), '_');
        name += "." + element;
        element.clear();
    };
    for (char c : identifier) {
        if (c == '.')
            append();
        else
            element += isalnum(static_cast<unsigned char>(c)) ? c : '_';
    }
    append();
    return name;
}

// 213
int hostInstanceClaim(const char* identifier, int forward, int count, const char* const* items)
{
    base::dbus::Connection* bus = sessionBus();
    if (!bus || !identifier || !*identifier)
        return -1;
    if (!instance) {
        const std::string id = identifier;
        auto* object = new base::dbus::Object(bus, instancePath, instanceInterface);
        object->addProperty("AppId", [id] { return id; });
        object->addProperty("ShowsInDock", [] { return shownInDock; });
        object->addMethod("Launch", [](const std::vector<std::string>& items, const std::string& token) {
            if (!callbacks.launched)
                return;
            std::vector<const char*> pointers;
            for (const std::string& item : items)
                pointers.push_back(item.c_str());
            callbacks.launched(int(pointers.size()), pointers.data(), token.c_str());
        });
        object->addMethod("Quit", [] {
            if (callbacks.quit)
                callbacks.quit();
        });
        object->publish();
        instance = object;
    }
    const std::string name = instanceName(identifier);
    if (bus->requestName(name, base::dbus::Connection::DoNotQueue))
        return 1;
    if (!forward)
        return -1;
    std::vector<std::string> list;
    for (int i = 0; i < count; i++) {
        if (items[i])
            list.push_back(items[i]);
    }
    const base::dbus::Proxy proxy(bus, name, instancePath, instanceInterface);
    const std::string token = application ? application->takeActivationToken() : std::string();
    // The owner of the name may not answer yet.
    for (int i = 0; i < 5; i++) {
        if (proxy.call("Launch", list, token))
            return 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return -1;
}

// 215
void hostInstanceSetShownInDock(int shown)
{
    if (shownInDock == (shown != 0))
        return;
    shownInDock = shown != 0;
    if (instance)
        instance->emitPropertiesChanged({"ShowsInDock"});
}

// 234
void hostSetActivationToken(const char* token)
{
    if (application && token)
        application->setActivationToken(token);
}

// 87
int hostLaunchApplication(const char* path, const char* const* arguments, int count)
{
    if (!path)
        return 1;
    std::ifstream file(path);
    std::stringstream contents;
    contents << file.rdbuf();
    std::string id = path;
    const size_t slash = id.rfind('/');
    if (slash != std::string::npos)
        id = id.substr(slash + 1);
    const std::optional<base::DesktopEntry> entry = base::DesktopEntry::parse(id, path, contents.str());
    if (!entry)
        return 1;
    std::vector<std::string> list;
    for (int i = 0; i < count; i++)
        list.push_back(arguments[i]);
    std::vector<std::string> command = entry->commandForArguments(list);
    if (command.empty())
        return 1;
    std::vector<char*> pointers;
    for (std::string& argument : command)
        pointers.push_back(argument.data());
    pointers.push_back(nullptr);
    pid_t process;
    return posix_spawnp(&process, pointers[0], nullptr, nullptr, pointers.data(), environ) == 0 ? 0 : 1;
}

// 305
void hostNowPlayingUpdate(const reef_ui_now_playing* state, NowPlaying::CommandHandler command)
{
    static std::unique_ptr<NowPlaying> player;
    if (!state || !state->status) {
        player.reset();
        return;
    }
    if (!player)
        player = std::make_unique<NowPlaying>(sessionBus(), applicationId, command);
    player->update(*state);
}

} // namespace cutefish::reef
