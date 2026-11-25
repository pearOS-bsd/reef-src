// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the WebKit host's files share.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gbm.h>

#include <WebKit/WebKit2_C.h>
#include <WebKit/WKContextPrivate.h>
#include <WebKit/WKContentWorld.h>
#include <WebKit/WKContextConfigurationRef.h>
#include <WebKit/WKContextSandbox.h>
#include <WebKit/WKCredential.h>
#include <WebKit/WKCredentialTypes.h>
#include <WebKit/WKAuthenticationChallenge.h>
#include <WebKit/WKAuthenticationDecisionListener.h>
#include <WebKit/WKProtectionSpace.h>
#include <WebKit/WKProtectionSpaceTypes.h>
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
#include <WebKit/WKViewClient.h>
#include <WebKit/WKWebsiteDataRecords.h>
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
    WKViewBuffer buffer;
    gbm_bo* bo;
};

// Properties of a page.
enum { Title = 1, Url, Loading, Progress, CanGoBack, CanGoForward, SecureContent };

struct View;

// What the scheme callbacks have; it stays when the view is gone.
struct SchemeHandler
{
    View* view;
};

struct HistoryItem
{
    const char* url;
    const char* originalUrl;
    const char* title;
};

struct View
{
    WKViewRef view = nullptr;
    SchemeHandler* schemes = nullptr;
    WKPageRef page = nullptr;
    void* context = nullptr;
    const ViewClient* client = nullptr;
    std::unordered_map<uint64_t, Buffer> buffers;
    uint64_t current = 0;
    bool hasCurrent = false;
    uint64_t frame = 0;
    std::vector<unsigned char> pixels;
    uint64_t pixelsFrame = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    bool framePending = false;
    int fence = -1;
    std::string title;
    std::string url;
    std::vector<std::string> itemTexts;
    std::vector<HistoryItem> items;
};

struct Error
{
    int code;
    const char* domain;
    const char* description;
    const char* url;
};

// The texts that an Error points to.
struct ErrorText
{
    std::string description;
    std::string domain;
    std::string url;
};

struct Challenge
{
    const char* host;
    int port;
    const char* protocol;
    const char* realm;
    int scheme;
    int isProxy;
    int failures;
    const char* user;
    WKAuthenticationDecisionListenerRef listener;
};

struct ChallengeText
{
    std::string host;
    std::string protocol;
    std::string realm;
    std::string user;
};

Error makeError(WKErrorRef error, ErrorText& text);
Challenge makeChallenge(WKAuthenticationChallengeRef challenge, ChallengeText& text);
void releaseBuffer(Buffer& buffer);
void appendJson(std::string& text, WKTypeRef value);
WKTypeRef parseJson(const char*& text);

using MessageFunction = void (*)(void* context, const char* name, const char* json, const char* origin, int isMainFrame,
                                 WKCompletionListenerRef listener);

struct MessageHandler
{
    std::string name;
    MessageFunction function;
    void* context;
};

struct UserContent
{
    WKUserContentControllerRef controller;
    std::vector<std::unique_ptr<MessageHandler>> handlers;
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
extern const Pasteboard* g_pasteboard;
// The tasks of URL schemes that are still to be answered.
extern std::unordered_set<const void*> g_tasks;

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
