// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A view and its clients.

#include "web.h"

#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

#include <WebKit/WKPageConfigurationRef.h>
#include <WebKit/WKPageFindClient.h>
#include <WebKit/WKPageNavigationClient.h>
#include <WebKit/WKPageStateClient.h>
#include <WebKit/WKPageUIClient.h>
#include <WebKit/WKPreferencesRef.h>
#include <WebKit/WKPreferencesRefPrivate.h>
#include <WebKit/WKViewClient.h>

namespace cutefish::reef::web {


void changed(View* view, int property);

void bufferCreated(WKViewRef, const WKViewBuffer*, const void*);
void bufferDestroyed(WKViewRef, uint64_t, const void*);
void frameReady(WKViewRef, uint64_t, const WKRect*, size_t, int, const void*);
void setCursor(WKViewRef, const char*, const void*);
void toolTipChanged(WKViewRef, const char*, const void*);
void enterFullScreen(WKViewRef, const void*);
void exitFullScreen(WKViewRef, const void*);
void inputMethodStateChanged(WKViewRef, bool, WKRect, const void*);
void showPopupMenu(WKViewRef, const WKViewMenuItem*, size_t, int32_t, WKRect, const void*);
void hidePopupMenu(WKViewRef, const void*);
void showContextMenu(WKViewRef, const WKViewMenuItem*, size_t, WKPoint, const void*);
void wheelEventNotHandled(WKViewRef, WKSize, const void*);
void contentsSizeChanged(WKViewRef, WKSize, const void*);
void underPageColorChanged(WKViewRef, double, double, double, double, const void*);
void startDrag(WKViewRef, const WKViewDragData*, uint32_t, const void*, uint32_t, uint32_t, uint32_t, WKPoint, const void*);
void startTask(WKURLSchemeTaskRef, const void*);
void stopTask(WKURLSchemeTaskRef, const void*);
bool readPixels(View* view, Buffer& buffer);

void decidePolicyForAction(WKPageRef, WKNavigationActionRef, WKFramePolicyListenerRef, WKTypeRef, const void*);
void decidePolicyForResponse(WKPageRef, WKNavigationResponseRef, WKFramePolicyListenerRef, WKTypeRef, const void*);
void didStartNavigation(WKPageRef, WKNavigationRef, WKTypeRef, const void*);
void didReceiveRedirect(WKPageRef, WKNavigationRef, WKTypeRef, const void*);
void didFailProvisionalNavigation(WKPageRef, WKNavigationRef, WKErrorRef, WKTypeRef, const void*);
void didCommitNavigation(WKPageRef, WKNavigationRef, WKTypeRef, const void*);
void didFinishDocumentLoad(WKPageRef, WKNavigationRef, WKTypeRef, const void*);
void didFinishNavigation(WKPageRef, WKNavigationRef, WKTypeRef, const void*);
void didFailNavigation(WKPageRef, WKNavigationRef, WKErrorRef, WKTypeRef, const void*);
void didReceiveChallenge(WKPageRef, WKAuthenticationChallengeRef, const void*);
void webProcessDidCrash(WKPageRef, const void*);
void contentRuleNotification(WKPageRef, WKURLRef, WKArrayRef, WKArrayRef, const void*);
void actionBecameDownload(WKPageRef, WKNavigationActionRef, WKDownloadRef, const void*);
void responseBecameDownload(WKPageRef, WKNavigationResponseRef, WKDownloadRef, const void*);
void didFindString(WKPageRef, WKStringRef, unsigned, const void*);
void didFailToFindString(WKPageRef, WKStringRef, const void*);
void closePage(WKPageRef, const void*);
WKPageRef createNewPage(WKPageRef, WKPageConfigurationRef, WKNavigationActionRef, WKWindowFeaturesRef, const void*);
void runAlert(WKPageRef, WKStringRef, WKFrameRef, WKSecurityOriginRef, WKPageRunJavaScriptAlertResultListenerRef, const void*);
void runConfirm(WKPageRef, WKStringRef, WKFrameRef, WKSecurityOriginRef, WKPageRunJavaScriptConfirmResultListenerRef, const void*);
void runPrompt(WKPageRef, WKStringRef, WKStringRef, WKFrameRef, WKSecurityOriginRef, WKPageRunJavaScriptPromptResultListenerRef,
               const void*);
void runOpenPanel(WKPageRef, WKFrameRef, WKOpenPanelParametersRef, WKOpenPanelResultListenerRef, const void*);

static void ignored(const void*)
{
}

static void progressChanged(const void* info) { changed(viewOf(info), Progress); }
static void backChanged(const void* info) { changed(viewOf(info), CanGoBack); }
static void forwardChanged(const void* info) { changed(viewOf(info), CanGoForward); }
static void secureChanged(const void* info) { changed(viewOf(info), SecureContent); }
static void urlChanged(const void* info) { changed(viewOf(info), Url); }
static void titleChanged(const void* info) { changed(viewOf(info), Title); }
static void loadingChanged(const void* info) { changed(viewOf(info), Loading); }

struct ViewOptions
{
    void* context;
    const ViewClient* client;
    UserContent* content;
    DataStore* store;
    WKSize size;
    double scale;
    const char* applicationName;
    int javaScript;
    const char* const* schemes;
    int schemeCount;
    int fullScreen;
};

// 18
View* createView(const ViewOptions* options)
{
    if (!available())
        return nullptr;
    View* view = new View;
    view->context = options->context;
    view->client = options->client;
    WKPageConfigurationRef configuration = WKPageConfigurationCreate();
    WKPageConfigurationSetContext(configuration, g_context);
    if (options->content)
        WKPageConfigurationSetUserContentController(configuration, options->content->controller);
    if (options->store)
        WKPageConfigurationSetWebsiteDataStore(configuration, options->store->store);
    view->schemes = new SchemeHandler{view};
    const WKURLSchemeHandlerClientV0 schemes = {{0, view->schemes}, startTask, stopTask};
    for (int i = 0; i < options->schemeCount; i++) {
        if (options->schemes[i] && *options->schemes[i])
            WKPageConfigurationSetURLSchemeHandler(configuration, options->schemes[i], &schemes.base);
    }
    WKPreferencesRef preferences = WKPreferencesCreate();
    WKPreferencesSetJavaScriptEnabled(preferences, options->javaScript != 0);
    WKPreferencesSetFullScreenEnabled(preferences, options->fullScreen != 0);
    WKPageConfigurationSetPreferences(configuration, preferences);
    WKRelease(preferences);
    view->view = WKViewCreate(configuration, options->size);
    WKRelease(configuration);
    view->page = WKViewGetPage(view->view);
    WKViewSetDeviceScaleFactor(view->view, options->scale > 0 ? options->scale : 1);
    if (options->applicationName && *options->applicationName) {
        WKStringRef name = makeString(options->applicationName);
        WKPageSetApplicationNameForUserAgent(view->page, name);
        WKRelease(name);
    }
    const WKViewClientV1 client = {
        {1, view}, bufferCreated, bufferDestroyed, frameReady, setCursor, toolTipChanged, enterFullScreen, exitFullScreen,
        inputMethodStateChanged, showPopupMenu, hidePopupMenu, showContextMenu, wheelEventNotHandled, contentsSizeChanged,
        underPageColorChanged, startDrag,
    };
    WKViewSetViewClient(view->view, &client.base);
    WKPageNavigationClientV3 navigation = {};
    navigation.base.version = 3;
    navigation.base.clientInfo = view;
    navigation.decidePolicyForNavigationAction = decidePolicyForAction;
    navigation.decidePolicyForNavigationResponse = decidePolicyForResponse;
    navigation.didStartProvisionalNavigation = didStartNavigation;
    navigation.didReceiveServerRedirectForProvisionalNavigation = didReceiveRedirect;
    navigation.didFailProvisionalNavigation = didFailProvisionalNavigation;
    navigation.didCommitNavigation = didCommitNavigation;
    navigation.didFinishNavigation = didFinishNavigation;
    navigation.didFailNavigation = didFailNavigation;
    navigation.didFinishDocumentLoad = didFinishDocumentLoad;
    navigation.didReceiveAuthenticationChallenge = didReceiveChallenge;
    navigation.webProcessDidCrash = webProcessDidCrash;
    navigation.contentRuleListNotification = contentRuleNotification;
    navigation.navigationActionDidBecomeDownload = actionBecameDownload;
    navigation.navigationResponseDidBecomeDownload = responseBecameDownload;
    WKPageSetPageNavigationClient(view->page, &navigation.base);
    const WKPageFindClientV0 find = {{0, view}, didFindString, didFailToFindString, nullptr};
    WKPageSetPageFindClient(view->page, &find.base);
    WKPageUIClientV6 ui = {};
    ui.base.version = 6;
    ui.base.clientInfo = view;
    ui.close = closePage;
    ui.runOpenPanel = runOpenPanel;
    ui.createNewPage = createNewPage;
    ui.runJavaScriptAlert = runAlert;
    ui.runJavaScriptConfirm = runConfirm;
    ui.runJavaScriptPrompt = runPrompt;
    WKPageSetPageUIClient(view->page, &ui.base);
    WKPageStateClientV0 state = {
        {0, view},
        ignored, loadingChanged,
        ignored, titleChanged,
        ignored, urlChanged,
        ignored, secureChanged,
        ignored, progressChanged,
        ignored, backChanged,
        ignored, forwardChanged,
        ignored, ignored,
        ignored, ignored,
        ignored, ignored,
        ignored,
    };
    WKPageSetPageStateClient(view->page, &state.base);
    return view;
}

// 19
void releaseView(View* view)
{
    if (!view)
        return;
    WKPageSetPageNavigationClient(view->page, nullptr);
    WKPageSetPageUIClient(view->page, nullptr);
    WKPageSetPageStateClient(view->page, nullptr);
    WKPageSetPageFindClient(view->page, nullptr);
    WKViewSetViewClient(view->view, nullptr);
    if (view->fence >= 0)
        close(view->fence);
    view->schemes->view = nullptr;
    for (auto& [identifier, buffer] : view->buffers)
        releaseBuffer(buffer);
    WKRelease(view->view);
    delete view;
}

struct Pixels
{
    const unsigned char* bytes;
    uint32_t width;
    uint32_t height;
    int stride;
};

// 49
int pixels(View* view, Pixels* result)
{
    if (view->hasCurrent && view->pixelsFrame != view->frame) {
        const auto found = view->buffers.find(view->current);
        if (found != view->buffers.end() && readPixels(view, found->second))
            view->pixelsFrame = view->frame;
    }
    if (view->pixels.empty())
        return 0;
    result->bytes = view->pixels.data();
    result->width = view->width;
    result->height = view->height;
    result->stride = int(view->width * 4);
    return 1;
}

struct Frame
{
    uint64_t identifier;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint64_t modifier;
    int planes;
    int fds[4];
    uint32_t offsets[4];
    uint32_t strides[4];
    int fence;
};

// 82
int currentFrame(View* view, Frame* frame)
{
    if (!view->hasCurrent)
        return 0;
    const auto found = view->buffers.find(view->current);
    if (found == view->buffers.end())
        return 0;
    const WKViewBuffer& buffer = found->second.buffer;
    if (buffer.type != kWKViewBufferTypeDMABuf)
        return 0;
    frame->identifier = buffer.identifier;
    frame->width = buffer.width;
    frame->height = buffer.height;
    frame->format = buffer.fourcc;
    frame->modifier = buffer.modifier;
    frame->planes = int(std::min(buffer.planeCount, 4u));
    for (int i = 0; i < frame->planes; i++) {
        frame->fds[i] = buffer.fds[i];
        frame->offsets[i] = buffer.offsets[i];
        frame->strides[i] = buffer.strides[i];
    }
    frame->fence = view->fence >= 0 ? fcntl(view->fence, F_DUPFD_CLOEXEC, 0) : -1;
    return 1;
}

struct DragInfo
{
    const char* text;
    const char* markup;
    const char* url;
    const char* urlTitle;
    const char* const* files;
    int fileCount;
    const void* image;
    int64_t imageSize;
};

// 91
void drag(View* view, int kind, const DragInfo* info, double x, double y, uint32_t operations)
{
    WKViewDragData data = {};
    if (info) {
        data.text = info->text;
        data.markup = info->markup;
        data.url = info->url;
        data.urlTitle = info->urlTitle;
        data.fileURLs = info->files;
        data.fileURLCount = size_t(std::max(info->fileCount, 0));
        data.image = info->image;
        data.imageSize = size_t(std::max<int64_t>(info->imageSize, 0));
    }
    const WKPoint point = {x, y};
    switch (kind) {
    case 1:
        WKViewDragEntered(view->view, &data, point, operations);
        break;
    case 2:
        WKViewDragUpdated(view->view, &data, point, operations);
        break;
    case 3:
        WKViewDragExited(view->view, &data, point, operations);
        break;
    case 4:
        WKViewPerformDrag(view->view, &data, point, operations);
        break;
    }
}

} // namespace cutefish::reef::web
