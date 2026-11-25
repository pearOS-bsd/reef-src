// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Scripts, message handlers and content filters of a page.

#include "web.h"

#include <WebKit/WKScriptMessageRef.h>

namespace cutefish::reef::web {

using FilterFunction = void (*)(void* context, Filter* filter, uint32_t result);
using FilterCallback = Callback<FilterFunction>;

static void filterDone(WKUserContentFilterRef filter, WKUserContentExtensionStoreResult result, void* context)
{
    FilterCallback* callback = static_cast<FilterCallback*>(context);
    callback->function(callback->context, filter ? new Filter{filter} : nullptr, result);
    delete callback;
}

static void messageReceived(WKScriptMessageRef message, WKCompletionListenerRef listener, const void* context)
{
    const MessageHandler* handler = static_cast<const MessageHandler*>(context);
    std::string json;
    appendJson(json, WKScriptMessageGetBody(message));
    WKRetain(listener);
    handler->function(handler->context, handler->name.c_str(), json.c_str(), "", 1, listener);
}

using NamesFunction = void (*)(void* context, const char* const* names, int64_t count);

static void identifiersRead(WKArrayRef identifiers, void* context)
{
    Callback<NamesFunction>* callback = static_cast<Callback<NamesFunction>*>(context);
    const size_t count = WKArrayGetSize(identifiers);
    std::vector<std::string> texts;
    for (size_t i = 0; i < count; i++)
        texts.push_back(toString(static_cast<WKStringRef>(WKArrayGetItemAtIndex(identifiers, i))));
    std::vector<const char*> names;
    for (const std::string& text : texts)
        names.push_back(text.c_str());
    callback->function(callback->context, names.data(), int64_t(names.size()));
    delete callback;
}

// 86
void addMessageHandlerInWorld(UserContent* content, const char* name, const char* world, MessageFunction function, void* context)
{
    auto handler = std::make_unique<MessageHandler>();
    handler->name = name ? name : "";
    handler->function = function;
    handler->context = context;
    WKStringRef text = makeString(name);
    WKStringRef worldText = world ? makeString(world) : nullptr;
    WKUserContentControllerAddScriptMessageHandlerInWorld(content->controller, text, worldText, messageReceived, handler.get());
    if (worldText)
        WKRelease(worldText);
    WKRelease(text);
    content->handlers.push_back(std::move(handler));
}

// 8
void addMessageHandler(UserContent* content, const char* name, MessageFunction function, void* context)
{
    addMessageHandlerInWorld(content, name, nullptr, function, context);
}

// 75
void filterIdentifiers(FilterStore* store, NamesFunction function, void* context)
{
    WKUserContentExtensionStoreGetIdentifiers(store->store, new Callback<NamesFunction>{function, context}, identifiersRead);
}

// 5
void releaseUserContent(UserContent* content)
{
    if (!content)
        return;
    WKUserContentControllerRemoveAllUserMessageHandlers(content->controller);
    WKRelease(content->controller);
    delete content;
}

// 4
UserContent* createUserContent()
{
    if (!available())
        return nullptr;
    UserContent* content = new UserContent;
    content->controller = WKUserContentControllerCreate();
    return content;
}

// 85
void addUserScriptInWorld(UserContent* content, const char* source, int atStart, int mainFrameOnly, const char* world)
{
    WKStringRef text = makeString(source);
    WKStringRef name = world ? makeString(world) : nullptr;
    WKUserScriptRef script = WKUserScriptCreateWithSourceInWorld(text,
        atStart ? kWKInjectAtDocumentStart : kWKInjectAtDocumentEnd, mainFrameOnly != 0, name);
    WKUserContentControllerAddUserScript(content->controller, script);
    WKRelease(script);
    if (name)
        WKRelease(name);
    WKRelease(text);
}

// 6
void addUserScript(UserContent* content, const char* source, int atStart, int mainFrameOnly)
{
    addUserScriptInWorld(content, source, atStart, mainFrameOnly, nullptr);
}

// 7
void removeUserScripts(UserContent* content)
{
    WKUserContentControllerRemoveAllUserScripts(content->controller);
}

// 9
void removeMessageHandlers(UserContent* content)
{
    WKUserContentControllerRemoveAllUserMessageHandlers(content->controller);
}

// 70
FilterStore* createFilterStore(const char* path)
{
    if (!available() || !path || !*path)
        return nullptr;
    WKStringRef text = makeString(path);
    FilterStore* store = new FilterStore{WKUserContentExtensionStoreCreate(text)};
    WKRelease(text);
    return store;
}

// 71
void releaseFilterStore(FilterStore* store)
{
    if (!store)
        return;
    WKRelease(store->store);
    delete store;
}

// 72
void compileFilter(FilterStore* store, const char* identifier, const char* json, FilterFunction function, void* context)
{
    WKStringRef name = makeString(identifier);
    WKStringRef source = makeString(json);
    WKUserContentExtensionStoreCompile(store->store, name, source, new FilterCallback{function, context}, filterDone);
    WKRelease(source);
    WKRelease(name);
}

// 73
void lookUpFilter(FilterStore* store, const char* identifier, FilterFunction function, void* context)
{
    WKStringRef name = makeString(identifier);
    WKUserContentExtensionStoreLookup(store->store, name, new FilterCallback{function, context}, filterDone);
    WKRelease(name);
}

// 74
void removeFilter(FilterStore* store, const char* identifier, FilterFunction function, void* context)
{
    WKStringRef name = makeString(identifier);
    WKUserContentExtensionStoreRemove(store->store, name, new FilterCallback{function, context}, filterDone);
    WKRelease(name);
}

// 76
void releaseFilter(Filter* filter)
{
    if (!filter)
        return;
    WKRelease(filter->filter);
    delete filter;
}

// 77
void setFilters(UserContent* content, Filter* const* filters, int count)
{
    WKUserContentControllerRemoveAllUserContentFilters(content->controller);
    for (int i = 0; i < count; i++) {
        if (filters[i])
            WKUserContentControllerAddUserContentFilter(content->controller, filters[i]->filter);
    }
}

} // namespace cutefish::reef::web
