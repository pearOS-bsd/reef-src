// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What WebKit tells the program of a view.

#include "web.h"

#include <WebKit/WKPageNavigationClient.h>
#include <WebKit/WKPageStateClient.h>

namespace cutefish::reef::web {

// Kinds of navigation events.
enum { Started = 1, Redirected, Committed, Finished, FailedProvisional, Failed, DocumentLoaded };
// Properties of a page.
enum { Title = 1, Url, Loading, Progress, CanGoBack, CanGoForward, SecureContent };

void navigationEvent(View* view, int kind, WKErrorRef error);
void changed(View* view, int property);
struct Download;
Download* createDownload(WKDownloadRef download);

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

void actionBecameDownload(WKPageRef, WKNavigationActionRef, WKDownloadRef download, const void* info)
{
    View* view = viewOf(info);
    const auto function = view->client->at<void (*)(void*, int, Download*)>(0xa0);
    if (!function) {
        WKDownloadCancel(download, nullptr, nullptr);
        return;
    }
    function(view->context, 1, createDownload(download));
}

void responseBecameDownload(WKPageRef, WKNavigationResponseRef, WKDownloadRef download, const void* info)
{
    View* view = viewOf(info);
    const auto function = view->client->at<void (*)(void*, int, Download*)>(0xa0);
    if (!function) {
        WKDownloadCancel(download, nullptr, nullptr);
        return;
    }
    function(view->context, 2, createDownload(download));
}

} // namespace cutefish::reef::web
