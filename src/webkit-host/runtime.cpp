// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "web.h"

#include <WebKit/WKScreens.h>

#include <cstring>
#include <fcntl.h>

#include <glib.h>

namespace cutefish::reef::web {

WKContextRef g_context;
gbm_device* g_device;
std::unordered_set<const void*> g_tasks;

bool g_sandbox = true;
std::string g_sandboxPath;

static bool s_continuousSpellChecking;
static const SpellChecker* s_spellChecker;
static bool s_available;
static bool s_started;

static void checkSpelling(const uint16_t* text, size_t length, size_t* location, size_t* misspelled, void*)
{
    int64_t at = 0;
    int64_t count = 0;
    s_spellChecker->check(text, length, &at, &count);
    *location = at < 0 ? 0 : size_t(at);
    *misspelled = count < 0 ? 0 : size_t(count);
}

struct Guesses
{
    WKSpellCheckerAddGuessFunction add;
    void* context;
};

static void addGuess(const uint16_t* guess, size_t length, void* context)
{
    const Guesses* guesses = static_cast<const Guesses*>(context);
    guesses->add(guess, length, guesses->context);
}

static void spellingGuesses(const uint16_t* word, size_t length, WKSpellCheckerAddGuessFunction add, void* addContext, void*)
{
    Guesses guesses = {add, addContext};
    s_spellChecker->guesses(word, length, addGuess, &guesses);
}

static void learnWord(const uint16_t* word, size_t length, void*)
{
    s_spellChecker->learn(word, length);
}

static void ignoreWord(const uint16_t* word, size_t length, void*)
{
    s_spellChecker->ignore(word, length);
}

static bool start()
{
    s_started = true;
    WKRunLoopInitializeMain();
    if (s_spellChecker) {
        const WKSpellCheckerClient client = {nullptr, checkSpelling, spellingGuesses, learnWord, ignoreWord};
        WKSpellCheckerSetClient(&client);
        WKSpellCheckerSetContinuousSpellCheckingEnabled(s_continuousSpellChecking);
    }
    WKContextConfigurationRef configuration = WKContextConfigurationCreate();
    g_context = WKContextCreateWithConfiguration(configuration);
    WKRelease(configuration);
    WKContextSetSandboxEnabled(g_context, g_sandbox);
    if (!g_sandboxPath.empty())
        WKContextAddPathToSandbox(g_context, g_sandboxPath.c_str(), true);
    const int fd = open("/dev/dri/renderD128", O_RDWR | O_CLOEXEC);
    if (fd >= 0)
        g_device = gbm_create_device(fd);
    s_available = g_context != nullptr;
    return s_available;
}

bool available()
{
    if (s_started)
        return s_available;
    return start();
}

static int64_t pasteboardChangeCount(const void* clientInfo)
{
    return g_pasteboard->changeCount();
}

static void pasteboardTypes(WKPasteboardAddTypeFunction add, void* context, const void*)
{
    g_pasteboard->types(add, context);
}

struct SetData
{
    WKPasteboardSetDataFunction set;
    void* context;
};

static void setData(const void* bytes, int64_t length, void* context)
{
    const SetData* data = static_cast<const SetData*>(context);
    data->set(bytes, size_t(length), data->context);
}

static void pasteboardData(const char* type, WKPasteboardSetDataFunction set, void* context, const void*)
{
    SetData data = {set, context};
    g_pasteboard->data(type, setData, &data);
}

static void pasteboardWrite(size_t count, const char* const* types, const void* const* bytes, const size_t* lengths, const void*)
{
    const std::vector<int64_t> sizes(lengths, lengths + count);
    g_pasteboard->write(int(count), types, bytes, sizes.data());
}

// Table 96
void setSpellChecker(const SpellChecker* checker)
{
    s_spellChecker = checker;
}

// 97
void setContinuousSpellChecking(int enabled)
{
    s_continuousSpellChecking = enabled != 0;
    if (s_started)
        WKSpellCheckerSetContinuousSpellCheckingEnabled(s_continuousSpellChecking);
}

// 81
void displaySyncFired()
{
    WKDisplaySyncFired();
}

// 80
void setDisplaySync(void (*startSync)(void*), void (*stopSync)(void*), void* context)
{
    const WKDisplaySyncClient client = {context, startSync, stopSync, 60000};
    WKDisplaySyncSetClient(&client);
}

// 3: seconds until the next call is due, negative for none
double iterate()
{
    if (!available())
        return -1;
    return WKMainRunLoopIterate();
}

// 2
int descriptor()
{
    if (!available())
        return -1;
    return WKMainRunLoopGetDescriptor();
}

// 1
int isAvailable()
{
    return available();
}

// 58
void setPasteboard(const Pasteboard* pasteboard)
{
    if (!available())
        return;
    g_pasteboard = pasteboard;
    if (!pasteboard) {
        WKPasteboardSetClient(nullptr);
        return;
    }
    static WKPasteboardClientV0 client;
    client.base.version = 0;
    client.changeCount = pasteboardChangeCount;
    client.copyTypes = pasteboardTypes;
    client.copyData = pasteboardData;
    client.write = pasteboardWrite;
    WKPasteboardSetClient(&client.base);
}

// 83
void setSandbox(int enabled, const char* path)
{
    g_sandbox = enabled != 0;
    g_sandboxPath = path ? path : "";
}

// 84: the application identifier, before WebKit starts
void setApplication(const char* identifier)
{
    if (s_started || !identifier)
        return;
    const size_t size = strlen(identifier);
    if (size == 0 || size > 255)
        return;
    int parts = 0;
    for (const char* part = identifier; part;) {
        const char* dot = strchr(part, '.');
        const size_t length = dot ? size_t(dot - part) : strlen(part);
        if (length == 0 || (*part >= '0' && *part <= '9'))
            return;
        for (size_t i = 0; i < length; i++) {
            const char c = part[i];
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_'))
                return;
        }
        parts++;
        part = dot ? dot + 1 : nullptr;
    }
    if (parts > 1 && !g_get_prgname())
        g_set_prgname(identifier);
}

struct Screen
{
    uint32_t identifier;
    double x;
    double y;
    double width;
    double height;
    double availableX;
    double availableY;
    double availableWidth;
    double availableHeight;
    int depth;
    int depthPerComponent;
    double dpi;
};

// 98
void setScreens(const Screen* screens, int count, uint32_t primary)
{
    std::vector<WKScreenInfo> list;
    for (int i = 0; i < count; i++) {
        const Screen& screen = screens[i];
        WKScreenInfo info = {};
        info.identifier = screen.identifier;
        info.frame = {{screen.x, screen.y}, {screen.width, screen.height}};
        info.availableFrame = {{screen.availableX, screen.availableY}, {screen.availableWidth, screen.availableHeight}};
        info.depth = screen.depth;
        info.depthPerComponent = screen.depthPerComponent;
        info.dpi = screen.dpi;
        list.push_back(info);
    }
    WKSetScreens(list.data(), list.size(), primary);
}

} // namespace cutefish::reef::web
