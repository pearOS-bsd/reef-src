// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What WebKit tells the program of a view.

#include "web.h"

#include <WebKit/WKPageNavigationClient.h>
#include <WebKit/WKNavigationActionRef.h>
#include <WebKit/WKNavigationResponseRef.h>
#include <WebKit/WKPageStateClient.h>
#include <WebKit/WKURLRequest.h>
#include <WebKit/WKURLResponse.h>

namespace cutefish::reef::web {

// Kinds of navigation events.
enum { Started = 1, Redirected, Committed, Finished, FailedProvisional, Failed, DocumentLoaded };
// Properties of a page.

void navigationEvent(View* view, int kind, WKErrorRef error);
void changed(View* view, int property);

void didStartNavigation(WKPageRef, WKNavigationRef, WKTypeRef, const void* info)
{
    navigationEvent(viewOf(info), Started, nullptr);
}

void didReceiveRedirect(WKPageRef, WKNavigationRef, WKTypeRef, const void* info)
{
    navigationEvent(viewOf(info), Redirected, nullptr);
}

void didFailProvisionalNavigation(WKPageRef, WKNavigationRef, WKErrorRef error, WKTypeRef, const void* info)
{
    navigationEvent(viewOf(info), FailedProvisional, error);
}

void didCommitNavigation(WKPageRef, WKNavigationRef, WKTypeRef, const void* info)
{
    navigationEvent(viewOf(info), Committed, nullptr);
}

void didFinishDocumentLoad(WKPageRef, WKNavigationRef, WKTypeRef, const void* info)
{
    navigationEvent(viewOf(info), DocumentLoaded, nullptr);
}

void didFinishNavigation(WKPageRef, WKNavigationRef, WKTypeRef, const void* info)
{
    navigationEvent(viewOf(info), Finished, nullptr);
}

void didFailNavigation(WKPageRef, WKNavigationRef, WKErrorRef error, WKTypeRef, const void* info)
{
    navigationEvent(viewOf(info), Failed, error);
}

void webProcessDidCrash(WKPageRef, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*)>(0x60))
        function(view->context);
}

void didFindString(WKPageRef, WKStringRef, unsigned count, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*, int, unsigned)>(0xb0))
        function(view->context, 1, count);
}

void didFailToFindString(WKPageRef, WKStringRef, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*, int, unsigned)>(0xb0))
        function(view->context, 0, 0);
}

void closePage(WKPageRef, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*)>(0x40))
        function(view->context);
}

struct Action
{
    const char* url;
    const char* method;
    int type;
    int hasTarget;
    int targetIsMain;
    int sourceIsMain;
    uint32_t modifiers;
    int button;
    int gesture;
    WKFramePolicyListenerRef listener;
    int download;
};

struct ActionText
{
    std::string url;
    std::string method;
};

static int toButton(WKEventMouseButton button)
{
    switch (button) {
    case kWKEventMouseButtonLeftButton:
        return 1;
    case kWKEventMouseButtonMiddleButton:
        return 2;
    case kWKEventMouseButtonRightButton:
        return 3;
    default:
        return 0;
    }
}

static Action makeAction(WKNavigationActionRef action, ActionText& text)
{
    Action result = {};
    if (WKURLRequestRef request = WKNavigationActionCopyRequest(action)) {
        text.url = takeUrl(WKURLRequestCopyURL(request));
        text.method = takeString(WKURLRequestCopyHTTPMethod(request));
        WKRelease(request);
    }
    result.url = text.url.c_str();
    result.method = text.method.c_str();
    result.type = WKNavigationActionGetNavigationType(action);
    if (WKFrameInfoRef frame = WKNavigationActionCopyTargetFrameInfo(action)) {
        result.hasTarget = 1;
        result.targetIsMain = WKFrameInfoGetIsMainFrame(frame);
        WKRelease(frame);
    }
    result.sourceIsMain = 1;
    result.modifiers = WKNavigationActionGetModifiers(action) & 0x1f;
    result.button = toButton(WKNavigationActionGetMouseButton(action));
    result.gesture = WKNavigationActionHasUnconsumedUserGesture(action);
    result.download = WKNavigationActionShouldPerformDownload(action);
    return result;
}

void decidePolicyForAction(WKPageRef, WKNavigationActionRef action, WKFramePolicyListenerRef listener, WKTypeRef, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const Action*);
    if (!view->client->at<Function>(0x28)) {
        WKFramePolicyListenerUse(listener);
        return;
    }
    ActionText text;
    Action value = makeAction(action, text);
    WKRetain(listener);
    value.listener = listener;
    view->client->at<Function>(0x28)(view->context, &value);
}

WKPageRef createNewPage(WKPageRef, WKPageConfigurationRef, WKNavigationActionRef action, WKWindowFeaturesRef, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const Action*);
    if (view->client->at<Function>(0x38)) {
        ActionText text;
        const Action value = makeAction(action, text);
        view->client->at<Function>(0x38)(view->context, &value);
    }
    return nullptr;
}

void contentRuleNotification(WKPageRef, WKURLRef url, WKArrayRef names, WKArrayRef texts, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const char*, const char*, const char*);
    if (!view->client->at<Function>(0xe8))
        return;
    const std::string urlText = takeString(url ? WKURLCopyString(url) : nullptr);
    const size_t count = WKArrayGetSize(texts);
    for (size_t i = 0; i < count && i < WKArrayGetSize(names); i++) {
        const std::string name = toString(static_cast<WKStringRef>(WKArrayGetItemAtIndex(names, i)));
        const std::string text = toString(static_cast<WKStringRef>(WKArrayGetItemAtIndex(texts, i)));
        view->client->at<Function>(0xe8)(view->context, urlText.c_str(), name.c_str(), text.c_str());
    }
}

struct Response
{
    const char* url;
    const char* type;
    const char* name;
    int64_t size;
    int status;
    int canShow;
    int isMainFrame;
    WKFramePolicyListenerRef listener;
};

struct ResponseText
{
    std::string url;
    std::string type;
    std::string name;
};

void decidePolicyForResponse(WKPageRef, WKNavigationResponseRef navigation, WKFramePolicyListenerRef listener, WKTypeRef, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const Response*);
    if (!view->client->at<Function>(0x30)) {
        WKFramePolicyListenerUse(listener);
        return;
    }
    ResponseText text;
    Response value = {};
    if (WKURLResponseRef response = WKNavigationResponseCopyResponse(navigation)) {
        text.url = takeUrl(WKURLResponseCopyURL(response));
        text.type = takeString(WKURLResponseCopyMIMEType(response));
        text.name = takeString(WKURLResponseCopySuggestedFilename(response));
        value.status = WKURLResponseHTTPStatusCode(response);
        value.size = WKURLResponseGetExpectedContentLength(response);
        WKRelease(response);
    }
    value.url = text.url.c_str();
    value.type = text.type.c_str();
    value.name = text.name.c_str();
    value.canShow = WKNavigationResponseCanShowMIMEType(navigation);
    if (WKFrameInfoRef frame = WKNavigationResponseCopyFrameInfo(navigation)) {
        value.isMainFrame = WKFrameInfoGetIsMainFrame(frame);
        WKRelease(frame);
    } else {
        value.isMainFrame = 1;
    }
    WKRetain(listener);
    value.listener = listener;
    view->client->at<Function>(0x30)(view->context, &value);
}

} // namespace cutefish::reef::web
