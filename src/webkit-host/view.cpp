// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A view and its clients.

#include "web.h"

#include <WebKit/WKPageConfigurationRef.h>
#include <WebKit/WKPageFindClient.h>
#include <WebKit/WKPageNavigationClient.h>
#include <WebKit/WKPageStateClient.h>
#include <WebKit/WKPreferencesRef.h>
#include <WebKit/WKPreferencesRefPrivate.h>
#include <WebKit/WKViewClient.h>

namespace cutefish::reef::web {

enum { Title = 1, Url, Loading, Progress, CanGoBack, CanGoForward, SecureContent };

void changed(View* view, int property);

// events.cpp, and those still to be reconstructed
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

void setStateClient(View* view)
{
    WKPageStateClientV0 client = {
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
    WKPageSetPageStateClient(view->page, &client.base);
}

} // namespace cutefish::reef::web
