// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The functions of the table that act on a view.

#include "web.h"

#include <algorithm>
#include <WebKit/WKPageUIClient.h>

namespace cutefish::reef::web {

// Table, in the order of the binary.
static void pdfDone(WKDataRef data, void* context)
{
    Callback<DataFunction>* callback = static_cast<Callback<DataFunction>*>(context);
    if (data)
        callback->function(callback->context, WKDataGetBytes(data), WKDataGetSize(data));
    else
        callback->function(callback->context, nullptr, 0);
    delete callback;
}

// 20
void setSize(View* view, WKSize size) { WKViewSetSize(view->view, size); }

// 78
void setObscuredInsets(View* view, double top, double left, double bottom, double right)
{
    WKViewSetObscuredContentInsets(view->view, float(top), float(left), float(bottom), float(right));
}

// 79
void setDark(View* view, int dark) { WKViewSetDarkAppearance(view->view, dark != 0); }

// 21
void setScale(View* view, double scale) { WKViewSetDeviceScaleFactor(view->view, scale > 0 ? scale : 1); }

// 22
void setState(View* view, uint32_t state)
{
    WKViewSetVisible(view->view, state & 1);
    WKViewSetInWindow(view->view, (state & 2) != 0);
    WKViewSetActive(view->view, (state & 4) != 0);
    WKViewSetFocused(view->view, (state & 8) != 0);
}

// 23
void setUserAgent(View* view, const char* agent)
{
    WKStringRef text = agent ? WKStringCreateWithUTF8CString(agent) : nullptr;
    WKPageSetCustomUserAgent(view->page, text);
    if (text)
        WKRelease(text);
}

// 24
void setZoom(View* view, double zoom) { WKPageSetPageZoomFactor(view->page, zoom > 0 ? zoom : 1); }

// 25
void loadUrl(View* view, const char* text)
{
    if (WKURLRef url = makeUrl(text)) {
        WKPageLoadURL(view->page, url);
        WKRelease(url);
    }
}

// 26
void loadHtml(View* view, const char* html, const char* base)
{
    WKStringRef text = makeString(html);
    WKURLRef url = makeUrl(base);
    WKPageLoadHTMLString(view->page, text, url);
    if (url)
        WKRelease(url);
    WKRelease(text);
}

// 27
void loadData(View* view, const unsigned char* bytes, int64_t size, const char* type, const char* encoding, const char* base)
{
    WKDataRef data = WKDataCreate(bytes, size_t(std::max<int64_t>(size, 0)));
    WKStringRef typeText = makeString(type && *type ? type : "text/html");
    WKStringRef encodingText = encoding && *encoding ? WKStringCreateWithUTF8CString(encoding) : nullptr;
    WKURLRef url = makeUrl(base);
    WKPageLoadData(view->page, data, typeText, encodingText, url);
    if (url)
        WKRelease(url);
    if (encodingText)
        WKRelease(encodingText);
    WKRelease(typeText);
    WKRelease(data);
}

// 28
void loadFile(View* view, const char* file, const char* directory)
{
    WKURLRef url = makeUrl(file);
    if (!url)
        return;
    WKURLRef access = makeUrl(directory);
    WKPageLoadFile(view->page, url, access);
    if (access)
        WKRelease(access);
    WKRelease(url);
}

// 29
void reload(View* view, int fromOrigin)
{
    if (fromOrigin)
        WKPageReloadFromOrigin(view->page);
    else
        WKPageReload(view->page);
}

// 30
void stopLoading(View* view) { WKPageStopLoading(view->page); }
// 31
void goBack(View* view) { WKPageGoBack(view->page); }
// 32
void goForward(View* view) { WKPageGoForward(view->page); }

// 33: until the next call
const char* title(View* view)
{
    view->title = takeString(WKPageCopyTitle(view->page));
    return view->title.c_str();
}

// 34
const char* url(View* view)
{
    view->url = takeUrl(WKPageCopyActiveURL(view->page));
    return view->url.c_str();
}

// 35
int isLoading(View* view)
{
    const double progress = WKPageGetEstimatedProgress(view->page);
    return progress > 0 && progress < 1;
}

// 36
double progress(View* view) { return WKPageGetEstimatedProgress(view->page); }
// 37
int canGoBack(View* view) { return WKPageCanGoBack(view->page); }
// 38
int canGoForward(View* view) { return WKPageCanGoForward(view->page); }

static void scriptDone(WKTypeRef result, WKErrorRef error, void* context)
{
    Callback<ScriptFunction>* callback = static_cast<Callback<ScriptFunction>*>(context);
    if (error) {
        const std::string text = takeString(WKErrorCopyLocalizedDescription(error));
        callback->function(callback->context, nullptr, text.empty() ? "JavaScript exception" : text.c_str());
    } else if (result) {
        std::string json;
        appendJson(json, result);
        callback->function(callback->context, json.c_str(), nullptr);
    } else {
        callback->function(callback->context, nullptr, nullptr);
    }
    delete callback;
}

// 39
void evaluate(View* view, const char* script, ScriptFunction function, void* context)
{
    WKStringRef text = makeString(script);
    WKPageEvaluateJavaScriptInMainFrame(view->page, text, new Callback<ScriptFunction>{function, context}, scriptDone);
    WKRelease(text);
}

// 87
void evaluateInWorld(View* view, const char* script, const char* world, ScriptFunction function, void* context)
{
    WKStringRef text = makeString(script);
    WKStringRef name = world ? makeString(world) : nullptr;
    WKPageEvaluateJavaScriptInFrameInWorld(view->page, nullptr, text, name,
        new Callback<ScriptFunction>{function, context}, scriptDone);
    if (name)
        WKRelease(name);
    WKRelease(text);
}

// 88
void callAsync(View* view, const char* body, const char* arguments, const char* world, ScriptFunction function, void* context)
{
    WKTypeRef values = nullptr;
    if (arguments) {
        const char* at = arguments;
        values = parseJson(at);
        if (values && WKGetTypeID(values) != WKDictionaryGetTypeID()) {
            WKRelease(values);
            values = nullptr;
        }
    }
    if (!values)
        values = WKMutableDictionaryCreate();
    WKStringRef text = makeString(body);
    WKStringRef name = world ? makeString(world) : nullptr;
    WKPageCallAsyncJavaScriptInWorld(view->page, text, static_cast<WKDictionaryRef>(values), nullptr, name,
        new Callback<ScriptFunction>{function, context}, scriptDone);
    if (name)
        WKRelease(name);
    WKRelease(text);
    if (values)
        WKRelease(values);
}

// 40
void execute(View* view, const char* command)
{
    WKStringRef text = makeString(command);
    WKPageExecuteCommand(view->page, text);
    WKRelease(text);
}

// 50
void frameDisplayed(View* view)
{
    if (!view->framePending)
        return;
    view->framePending = false;
    WKViewFrameDisplayed(view->view);
}

// 56
void answerPolicy(WKFramePolicyListenerRef listener, int policy)
{
    switch (policy) {
    case 1:
        WKFramePolicyListenerUse(listener);
        break;
    case 3:
        WKFramePolicyListenerDownload(listener);
        break;
    default:
        WKFramePolicyListenerIgnore(listener);
        break;
    }
    WKRelease(listener);
}

static WKFindOptions findOptions(uint32_t flags)
{
    WKFindOptions options = 0;
    if (flags & 1)
        options |= kWKFindOptionsCaseInsensitive;
    if (flags & 2)
        options |= kWKFindOptionsBackwards;
    if (flags & 4)
        options |= kWKFindOptionsWrapAround;
    if (flags & 8)
        options |= kWKFindOptionsShowOverlay | kWKFindOptionsShowFindIndicator;
    return options;
}

// 67
void find(View* view, const char* text, uint32_t flags)
{
    if (!text || !*text) {
        WKPageHideFindUI(view->page);
        return;
    }
    const WKFindOptions options = findOptions(flags);
    WKStringRef string = makeString(text);
    WKPageFindString(view->page, string, options, 1000);
    WKRelease(string);
}

// 92
uint32_t dragOperation(View* view) { return WKViewGetDragOperation(view->view); }

// 93
void dragSourceEnded(View* view, WKPoint point, uint32_t operation) { WKViewDragSourceEnded(view->view, point, operation); }

// 99
void drawPdf(View* view, int hasRect, double x, double y, double width, double height, DataFunction function, void* context)
{
    const WKRect rect = {{x, y}, {width, height}};
    WKPageDrawContentsToPDF(view->page, hasRect ? &rect : nullptr, new Callback<DataFunction>{function, context}, pdfDone);
}

// 100
void printPdf(View* view, double width, double height, double scale, DataFunction function, void* context)
{
    WKPagePrintLayout layout = {};
    layout.paperWidth = width;
    layout.paperHeight = height;
    layout.scale = scale;
    WKPageDrawPagesToPDFForPrinting(view->page, &layout, new Callback<DataFunction>{function, context}, pdfDone);
}

static void appendItem(View* view, WKBackForwardListItemRef item)
{
    view->itemTexts.push_back(takeUrl(WKBackForwardListItemCopyURL(item)));
    view->itemTexts.push_back(takeUrl(WKBackForwardListItemCopyOriginalURL(item)));
    view->itemTexts.push_back(takeString(WKBackForwardListItemCopyTitle(item)));
}

// 68: oldest first
int history(View* view, const HistoryItem** items, int* back, int* forward)
{
    view->itemTexts.clear();
    view->items.clear();
    WKBackForwardListRef list = WKPageGetBackForwardList(view->page);
    WKArrayRef backList = WKBackForwardListCopyBackListWithLimit(list, unsigned(-1));
    WKArrayRef forwardList = WKBackForwardListCopyForwardListWithLimit(list, unsigned(-1));
    WKBackForwardListItemRef current = WKBackForwardListGetCurrentItem(list);
    const size_t backCount = backList ? WKArrayGetSize(backList) : 0;
    const size_t forwardCount = forwardList ? WKArrayGetSize(forwardList) : 0;
    for (size_t i = 0; i < backCount; i++)
        appendItem(view, static_cast<WKBackForwardListItemRef>(WKArrayGetItemAtIndex(backList, i)));
    if (current)
        appendItem(view, current);
    for (size_t i = 0; i < forwardCount; i++)
        appendItem(view, static_cast<WKBackForwardListItemRef>(WKArrayGetItemAtIndex(forwardList, i)));
    if (backList)
        WKRelease(backList);
    if (forwardList)
        WKRelease(forwardList);
    for (size_t i = 0; i + 2 < view->itemTexts.size(); i += 3)
        view->items.push_back({view->itemTexts[i].c_str(), view->itemTexts[i + 1].c_str(), view->itemTexts[i + 2].c_str()});
    *items = view->items.data();
    *back = int(backCount);
    *forward = int(forwardCount);
    return int(view->items.size());
}

// 69
void goToItem(View* view, int index)
{
    if (WKBackForwardListItemRef item = WKBackForwardListGetItemAtIndex(WKPageGetBackForwardList(view->page), index))
        WKPageGoToBackForwardListItem(view->page, item);
}

} // namespace cutefish::reef::web
