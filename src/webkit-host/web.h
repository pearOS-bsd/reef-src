// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the WebKit host's files share.

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <gbm.h>

#include <WebKit/WebKit2_C.h>
#include <WebKit/WKContextPrivate.h>
#include <WebKit/WKContentWorld.h>
#include <WebKit/WKContextConfigurationRef.h>
#include <WebKit/WKContextSandbox.h>
#include <WebKit/WKCredential.h>
#include <WebKit/WKCredentialTypes.h>
#include <WebKit/WKAuthenticationDecisionListener.h>
#include <WebKit/WKDownloadRef.h>
#include <WebKit/WKFrameInfoRef.h>
#include <WebKit/WKHTTPCookieStoreRef.h>
#include <WebKit/WKMutableArray.h>
#include <WebKit/WKOpenPanelResultListener.h>
#include <WebKit/WKPagePDF.h>
#include <WebKit/WKPageUIClient.h>
#include <WebKit/WKRunLoop.h>
#include <WebKit/WKURLSchemeHandler.h>
#include <WebKit/WKDisplaySync.h>
#include <WebKit/WKMainRunLoop.h>
#include <WebKit/WKPagePrivate.h>
#include <WebKit/WKPasteboard.h>
#include <WebKit/WKSpellChecker.h>
#include <WebKit/WKUserContentControllerRef.h>
#include <WebKit/WKUserContentExtensionStoreRef.h>
#include <WebKit/WKUserScriptRef.h>
#include <WebKit/WKView.h>
#include <WebKit/WKWebsiteDataStoreRef.h>

namespace cutefish::reef::web {

// A caller's function and its context, until the answer comes.
template<typename Function>
struct Callback
{
    Function function;
    void* context;
};

using DataFunction = void (*)(void* context, const void* bytes, size_t size);
using DoneFunction = void (*)(void* context);
using ScriptFunction = void (*)(void* context, const char* json, const char* error);

struct SpellChecker
{
    void (*check)(const uint16_t* text, size_t length, int64_t* location, int64_t* misspelled);
    void (*guesses)(const uint16_t* word, size_t length, WKSpellCheckerAddGuessFunction add, void* context);
    void (*learn)(const uint16_t* word, size_t length);
    void (*ignore)(const uint16_t* word, size_t length);
};

struct Pasteboard
{
    int64_t (*changeCount)();
    void (*types)(void (*add)(const char* type, void* context), void* context);
    void (*data)(const char* type, void (*set)(const void* bytes, int64_t length, void* context), void* context);
    void (*write)(int count, const char* const* types, const void* const* bytes, const int64_t* lengths);
};

// One of the functions of the program that has the view; null for what it does not handle.
struct ViewClient
{
    void* slots[40];
    template<typename Function>
    Function at(size_t offset) const { return reinterpret_cast<Function>(slots[offset / 8]); }
};

struct Buffer
{
    ~Buffer();

    uint64_t identifier;
    bool shared;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint64_t modifier;
    uint32_t planes;
    int fds[4];
    uint32_t offsets[4];
    uint32_t strides[4];
    // Not identified yet.
    uint64_t reserved[2];
    gbm_bo* bo;
};

struct View
{
    WKViewRef view;
    void* configuration;
    WKPageRef page;
    void* context;
    const ViewClient* client;
    std::unordered_map<uint64_t, Buffer> buffers;
    uint64_t current;
    bool hasCurrent;
    uint64_t frame;
    std::vector<uint32_t> pixels;
    uint64_t pixelsFrame;
    int width;
    int height;
    bool framePending;
    std::string title;
    std::string url;
};

struct UserContent
{
    WKUserContentControllerRef controller;
    std::vector<void*> handlers;
};

struct FilterStore
{
    WKUserContentExtensionStoreRef store;
};

struct Filter
{
    WKUserContentFilterRef filter;
};

struct DataStore
{
    WKWebsiteDataStoreRef store;
};

extern WKContextRef g_context;
extern gbm_device* g_device;

// Whether WebKit is there; starts it the first time.
bool available();

WKStringRef makeString(const char* text);
// Null for no text.
WKURLRef makeUrl(const char* text);
// These release what they read.
std::string takeString(WKStringRef string);
std::string takeUrl(WKURLRef url);
std::string toString(WKStringRef string);
double now();
View* viewOf(const void* clientInfo);

} // namespace cutefish::reef::web
