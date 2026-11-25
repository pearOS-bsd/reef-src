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
    if (!available())
        return nullptr;
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

enum {
    Cookies = 1, MemoryCache = 2, DiskCache = 4, LocalStorage = 8, IndexedDatabases = 0x10, ServiceWorkers = 0x20, FetchCache = 0x40,
};

// Calls back once, when the last of the steps is done.
struct Clearing
{
    int steps;
    DoneFunction done;
    void* context;
};

static void stepDone(Clearing* clearing)
{
    if (--clearing->steps)
        return;
    if (clearing->done)
        clearing->done(clearing->context);
    delete clearing;
}

static void cookiesCleared(void* context) { stepDone(static_cast<Clearing*>(context)); }
static void fetchCachesCleared(void* context) { stepDone(static_cast<Clearing*>(context)); }
static void memoryCleared(void* context) { stepDone(static_cast<Clearing*>(context)); }
static void networkCacheCleared(void* context) { stepDone(static_cast<Clearing*>(context)); }
static void localStorageCleared(void* context) { stepDone(static_cast<Clearing*>(context)); }
static void databasesCleared(void* context) { stepDone(static_cast<Clearing*>(context)); }
static void workersCleared(void* context) { stepDone(static_cast<Clearing*>(context)); }

// 11
DataStore* defaultDataStore()
{
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

// 14
void clearData(DataStore* store, uint32_t kinds, DoneFunction function, void* context)
{
    Clearing* clearing = new Clearing{1, function, context};
    WKWebsiteDataStoreRef ref = store->store;
    if (kinds & Cookies) {
        clearing->steps++;
        WKHTTPCookieStoreDeleteAllCookies(WKWebsiteDataStoreGetHTTPCookieStore(ref), clearing, cookiesCleared);
    }
    if (kinds & (MemoryCache | DiskCache)) {
        clearing->steps++;
        WKWebsiteDataStoreRemoveMemoryCaches(ref, clearing, memoryCleared);
        clearing->steps++;
        WKWebsiteDataStoreRemoveNetworkCache(ref, clearing, networkCacheCleared);
    }
    if (kinds & LocalStorage) {
        clearing->steps++;
        WKWebsiteDataStoreRemoveLocalStorage(ref, clearing, localStorageCleared);
    }
    if (kinds & IndexedDatabases) {
        clearing->steps++;
        WKWebsiteDataStoreRemoveAllIndexedDatabases(ref, clearing, databasesCleared);
    }
    if (kinds & ServiceWorkers) {
        clearing->steps++;
        WKWebsiteDataStoreRemoveAllServiceWorkerRegistrations(ref, clearing, workersCleared);
    }
    if (kinds & FetchCache) {
        clearing->steps++;
        WKWebsiteDataStoreRemoveAllFetchCaches(ref, clearing, fetchCachesCleared);
    }
    stepDone(clearing);
}

// The table's kinds of data and those of WebKit.
struct Kind
{
    int kind;
    uint32_t webKit;
};

static constexpr Kind s_kinds[] = {
    {0x1, kWKWebsiteDataCookies},
    {0x2, kWKWebsiteDataMemoryCache},
    {0x4, kWKWebsiteDataDiskCache},
    {0x8, kWKWebsiteDataLocalStorage},
    {0x10, kWKWebsiteDataIndexedDBDatabases},
    {0x20, kWKWebsiteDataServiceWorkerRegistrations},
    {0x40, kWKWebsiteDataDOMCache},
    {0x80, kWKWebsiteDataSessionStorage},
    {0x100, kWKWebsiteDataWebSQLDatabases},
    {0x200, kWKWebsiteDataMediaKeys},
    {0x400, kWKWebsiteDataSearchFieldRecentSearches},
    {0x800, kWKWebsiteDataFileSystem},
};

static uint32_t toWebKitKinds(uint32_t kinds)
{
    uint32_t result = 0;
    for (const Kind& kind : s_kinds) {
        if (kinds & kind.kind)
            result |= kind.webKit;
    }
    return result;
}

static int toKinds(uint32_t webKit)
{
    int result = 0;
    for (const Kind& kind : s_kinds) {
        if (webKit & kind.webKit)
            result |= kind.kind;
    }
    return result;
}

struct Record
{
    const char* name;
    int kinds;
};

using RecordsFunction = void (*)(void* context, const Record* records, int64_t count);

static void recordsRead(const WKWebsiteDataRecord* records, size_t count, void* context)
{
    Callback<RecordsFunction>* callback = static_cast<Callback<RecordsFunction>*>(context);
    std::vector<Record> list;
    for (size_t i = 0; i < count; i++) {
        const uint32_t kinds = toKinds(records[i].types);
        if (kinds)
            list.push_back({records[i].displayName, int(kinds)});
    }
    callback->function(callback->context, list.data(), int64_t(list.size()));
    delete callback;
}

// 89
void records(DataStore* store, uint32_t kinds, RecordsFunction function, void* context)
{
    WKWebsiteDataStoreFetchDataRecords(store->store, toWebKitKinds(kinds), new Callback<RecordsFunction>{function, context}, recordsRead);
}

// 90
void removeRecords(DataStore* store, uint32_t kinds, const char* const* names, int count, DoneFunction function, void* context)
{
    DataCall* call = new DataCall{nullptr, function, context};
    WKWebsiteDataStoreRemoveDataRecords(store->store, toWebKitKinds(kinds), names, size_t(std::max(count, 0)), call, recordsRemoved);
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
