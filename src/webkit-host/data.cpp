// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Website data and cookies.

#include "web.h"

#include <algorithm>

namespace cutefish::reef::web {

struct Cookie
{
    const char* name;
    const char* value;
    const char* domain;
    const char* path;
    double expires;
    int session;
    int secure;
    int httpOnly;
    int sameSite;
};

using CookiesFunction = void (*)(void* context, const Cookie* cookies, int count);

// One of the two functions is set.
struct DataCall
{
    CookiesFunction cookies;
    DoneFunction done;
    void* context;
};

static DataStore* defaultStore()
{
    static DataStore store = {WKWebsiteDataStoreGetDefaultDataStore()};
    return &store;
}

static void recordsRemoved(void* context)
{
    DataCall* call = static_cast<DataCall*>(context);
    if (call->done)
        call->done(call->context);
    delete call;
}

static void cookieDone(void* context)
{
    DataCall* call = static_cast<DataCall*>(context);
    if (call->done)
        call->done(call->context);
    delete call;
}

static void cookiesRead(const WKHTTPCookie* cookies, size_t count, void* context)
{
    std::vector<Cookie> list(count);
    for (size_t i = 0; i < count; i++) {
        const WKHTTPCookie& from = cookies[i];
        list[i] = {from.name, from.value, from.domain, from.path, from.expires, from.session, from.secure, from.httpOnly,
                   int(from.sameSite)};
    }
    DataCall* call = static_cast<DataCall*>(context);
    call->cookies(call->context, list.data(), int(count));
    delete call;
}

// 11
DataStore* defaultDataStore()
{
    if (!available())
        return nullptr;
    return defaultStore();
}

// 12
DataStore* createDataStore()
{
    return available() ? new DataStore{WKWebsiteDataStoreCreateNonPersistentDataStore()} : nullptr;
}

// 13
void releaseDataStore(DataStore* store)
{
    if (!store)
        return;
    if (store == defaultDataStore())
        return;
    WKRelease(store->store);
    delete store;
}

// 15
void cookies(DataStore* store, CookiesFunction function, void* context)
{
    DataCall* call = new DataCall{function, nullptr, context};
    WKHTTPCookieStoreGetAllCookies(WKWebsiteDataStoreGetHTTPCookieStore(store->store), call, cookiesRead);
}

static WKHTTPCookie toWebKit(const Cookie* cookie)
{
    WKHTTPCookie result;
    result.name = cookie->name ? cookie->name : "";
    result.value = cookie->value ? cookie->value : "";
    result.domain = cookie->domain ? cookie->domain : "";
    result.path = cookie->path ? cookie->path : "/";
    result.expires = cookie->expires;
    result.session = cookie->session != 0;
    result.secure = cookie->secure != 0;
    result.httpOnly = cookie->httpOnly != 0;
    result.sameSite = WKHTTPCookieSameSitePolicy(cookie->sameSite);
    return result;
}

// 16
void setCookie(DataStore* store, const Cookie* cookie, DoneFunction function, void* context)
{
    const WKHTTPCookie value = toWebKit(cookie);
    DataCall* call = new DataCall{nullptr, function, context};
    WKHTTPCookieStoreSetCookie(WKWebsiteDataStoreGetHTTPCookieStore(store->store), &value, call, cookieDone);
}

// 17
void deleteCookie(DataStore* store, const Cookie* cookie, DoneFunction function, void* context)
{
    const WKHTTPCookie value = toWebKit(cookie);
    DataCall* call = new DataCall{nullptr, function, context};
    WKHTTPCookieStoreDeleteCookie(WKWebsiteDataStoreGetHTTPCookieStore(store->store), &value, call, cookieDone);
}

} // namespace cutefish::reef::web
