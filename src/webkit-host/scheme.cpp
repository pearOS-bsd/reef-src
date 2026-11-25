// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The URL schemes that the program answers.

#include "web.h"

#include <WebKit/WKURLRequest.h>

namespace cutefish::reef::web {

void startTask(WKURLSchemeTaskRef task, const void* info)
{
    View* view = static_cast<const SchemeHandler*>(info)->view;
    if (!view) {
        WKURLSchemeTaskDidFail(task, "WebKitErrorDomain", 0, "The view is gone");
        return;
    }
    WKRetain(task);
    g_tasks.insert(task);
    std::string url;
    std::string method;
    if (WKURLRequestRef request = WKURLSchemeTaskCopyRequest(task)) {
        url = takeUrl(WKURLRequestCopyURL(request));
        method = takeString(WKURLRequestCopyHTTPMethod(request));
        WKRelease(request);
    }
    const auto function = view->client->at<void (*)(void*, WKURLSchemeTaskRef, const char*, const char*)>(0x70);
    if (!function) {
        WKURLSchemeTaskDidFail(task, "WebKitErrorDomain", 0, "No handler for the URL scheme");
        g_tasks.erase(task);
        WKRelease(task);
        return;
    }
    function(view->context, task, url.c_str(), method.c_str());
}

void stopTask(WKURLSchemeTaskRef task, const void* info)
{
    if (!g_tasks.erase(task))
        return;
    View* view = static_cast<const SchemeHandler*>(info)->view;
    if (view) {
        if (const auto function = view->client->at<void (*)(void*, WKURLSchemeTaskRef)>(0x78))
            function(view->context, task);
    }
    WKRelease(task);
}

// 51
void taskResponse(const void* task, const char* type, int64_t length, const char* encoding, int status)
{
    if (g_tasks.find(task) == g_tasks.end())
        return;
    WKURLSchemeTaskDidReceiveResponse(static_cast<WKURLSchemeTaskRef>(task), type ? type : "", length, encoding ? encoding : "", status);
}

// 52
void taskData(const void* task, const void* bytes, int64_t size)
{
    if (!g_tasks.count(task) || size <= 0)
        return;
    WKURLSchemeTaskDidReceiveData(static_cast<WKURLSchemeTaskRef>(task), bytes, size_t(size));
}

// 53
void taskFinish(const void* task)
{
    if (!g_tasks.erase(task))
        return;
    WKURLSchemeTaskDidFinish(static_cast<WKURLSchemeTaskRef>(task));
    WKRelease(task);
}

// 54
void taskFail(const void* task, const char* domain, int code, const char* description)
{
    if (!g_tasks.erase(task))
        return;
    WKURLSchemeTaskDidFail(static_cast<WKURLSchemeTaskRef>(task), domain ? domain : "", code, description ? description : "");
    WKRelease(task);
}

} // namespace cutefish::reef::web
