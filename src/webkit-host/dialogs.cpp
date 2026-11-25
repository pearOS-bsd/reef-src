// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The answers to what a page asks.

#include "web.h"

#include <WebKit/WKOpenPanelParametersRef.h>

namespace cutefish::reef::web {

// 59
void answerAuthentication(WKAuthenticationDecisionListenerRef listener, int disposition, const char* user,
                          const char* password, int persistence)
{
    switch (disposition) {
    case 1: {
        WKStringRef userText = makeString(user);
        WKStringRef passwordText = makeString(password);
        WKCredentialRef credential = WKCredentialCreate(userText, passwordText,
            persistence == 2 ? kWKCredentialPersistencePermanent
            : persistence == 1 ? kWKCredentialPersistenceForSession : kWKCredentialPersistenceNone);
        WKAuthenticationDecisionListenerUseCredential(listener, credential);
        WKRelease(credential);
        WKRelease(passwordText);
        WKRelease(userText);
        break;
    }
    case 3:
        WKAuthenticationDecisionListenerCancel(listener);
        break;
    case 4:
        WKAuthenticationDecisionListenerRejectProtectionSpaceAndContinue(listener);
        break;
    default:
        WKAuthenticationDecisionListenerPerformDefaultHandling(listener);
        break;
    }
    WKRelease(listener);
}

// 55
void answerOpenPanel(WKOpenPanelResultListenerRef listener, const char* const* urls, int count)
{
    if (count < 0) {
        WKOpenPanelResultListenerCancel(listener);
        WKRelease(listener);
        return;
    }
    WKMutableArrayRef files = WKMutableArrayCreate();
    for (int i = 0; i < count; i++) {
        if (WKURLRef url = makeUrl(urls[i])) {
            WKArrayAppendItem(files, url);
            WKRelease(url);
        }
    }
    WKMutableArrayRef types = WKMutableArrayCreate();
    WKOpenPanelResultListenerChooseFiles(listener, files, types);
    WKRelease(types);
    WKRelease(files);
    WKRelease(listener);
}

struct ScriptDialog
{
    int kind;
    WKTypeRef listener;
};

// 57
void answerScriptDialog(ScriptDialog* dialog, const char* text)
{
    switch (dialog->kind) {
    case 0:
        WKPageRunJavaScriptAlertResultListenerCall(static_cast<WKPageRunJavaScriptAlertResultListenerRef>(dialog->listener));
        break;
    case 1:
        WKPageRunJavaScriptConfirmResultListenerCall(
            static_cast<WKPageRunJavaScriptConfirmResultListenerRef>(dialog->listener), text != nullptr);
        break;
    case 2: {
        WKStringRef string = text ? WKStringCreateWithUTF8CString(text) : nullptr;
        WKPageRunJavaScriptPromptResultListenerCall(
            static_cast<WKPageRunJavaScriptPromptResultListenerRef>(dialog->listener), string);
        if (string)
            WKRelease(string);
        break;
    }
    }
    WKRelease(dialog->listener);
    delete dialog;
}

void runAlert(WKPageRef, WKStringRef message, WKFrameRef, WKSecurityOriginRef, WKPageRunJavaScriptAlertResultListenerRef listener,
              const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const char*, ScriptDialog*);
    if (!view->client->at<Function>(0x48)) {
        WKPageRunJavaScriptAlertResultListenerCall(listener);
        return;
    }
    WKRetain(listener);
    const std::string text = toString(message);
    view->client->at<Function>(0x48)(view->context, text.c_str(), new ScriptDialog{0, listener});
}

void runConfirm(WKPageRef, WKStringRef message, WKFrameRef, WKSecurityOriginRef, WKPageRunJavaScriptConfirmResultListenerRef listener,
                const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const char*, ScriptDialog*);
    if (!view->client->at<Function>(0x50)) {
        WKPageRunJavaScriptConfirmResultListenerCall(listener, false);
        return;
    }
    WKRetain(listener);
    const std::string text = toString(message);
    view->client->at<Function>(0x50)(view->context, text.c_str(), new ScriptDialog{1, listener});
}

void runPrompt(WKPageRef, WKStringRef message, WKStringRef defaultValue, WKFrameRef, WKSecurityOriginRef,
               WKPageRunJavaScriptPromptResultListenerRef listener, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const char*, const char*, ScriptDialog*);
    if (!view->client->at<Function>(0x58)) {
        WKPageRunJavaScriptPromptResultListenerCall(listener, nullptr);
        return;
    }
    WKRetain(listener);
    const std::string text = toString(message);
    const std::string value = toString(defaultValue);
    view->client->at<Function>(0x58)(view->context, text.c_str(), value.c_str(), new ScriptDialog{2, listener});
}

void runOpenPanel(WKPageRef, WKFrameRef, WKOpenPanelParametersRef parameters, WKOpenPanelResultListenerRef listener, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, int, int, const char* const*, int, WKOpenPanelResultListenerRef);
    if (!view->client->at<Function>(0x98)) {
        WKOpenPanelResultListenerCancel(listener);
        return;
    }
    std::vector<std::string> extensions;
    if (WKArrayRef array = WKOpenPanelParametersCopyAcceptedFileExtensions(parameters)) {
        const size_t count = WKArrayGetSize(array);
        for (size_t i = 0; i < count; i++) {
            std::string extension = toString(static_cast<WKStringRef>(WKArrayGetItemAtIndex(array, i)));
            if (!extension.empty() && extension.front() == '.')
                extension.erase(0, 1);
            if (!extension.empty())
                extensions.push_back(extension);
        }
        WKRelease(array);
    }
    std::vector<const char*> list;
    for (const std::string& extension : extensions)
        list.push_back(extension.c_str());
    WKRetain(listener);
    view->client->at<Function>(0x98)(view->context, WKOpenPanelParametersGetAllowsMultipleFiles(parameters),
        WKOpenPanelParametersGetAllowsDirectories(parameters), list.data(), int(list.size()), listener);
}

static const char* protocolName(WKProtectionSpaceServerType type)
{
    switch (type) {
    case kWKProtectionSpaceServerTypeHTTPS:
        return "https";
    case kWKProtectionSpaceServerTypeFTP:
        return "ftp";
    case kWKProtectionSpaceServerTypeFTPS:
        return "ftps";
    case kWKProtectionSpaceProxyTypeHTTP:
        return "http-proxy";
    case kWKProtectionSpaceProxyTypeHTTPS:
        return "https-proxy";
    case kWKProtectionSpaceProxyTypeFTP:
        return "ftp-proxy";
    case kWKProtectionSpaceProxyTypeSOCKS:
        return "socks-proxy";
    default:
        return "http";
    }
}

static int schemeOf(WKProtectionSpaceAuthenticationScheme scheme)
{
    return scheme >= 1 && scheme <= 9 ? int(scheme) : 1;
}

Challenge makeChallenge(WKAuthenticationChallengeRef challenge, ChallengeText& text)
{
    Challenge result = {};
    WKProtectionSpaceRef space = WKAuthenticationChallengeGetProtectionSpace(challenge);
    text.host = takeString(WKProtectionSpaceCopyHost(space));
    text.protocol = protocolName(WKProtectionSpaceGetServerType(space));
    text.realm = takeString(WKProtectionSpaceCopyRealm(space));
    result.host = text.host.c_str();
    result.port = WKProtectionSpaceGetPort(space);
    result.protocol = text.protocol.c_str();
    result.realm = text.realm.c_str();
    result.scheme = schemeOf(WKProtectionSpaceGetAuthenticationScheme(space));
    result.isProxy = WKProtectionSpaceGetIsProxy(space);
    result.failures = WKAuthenticationChallengeGetPreviousFailureCount(challenge);
    if (WKCredentialRef credential = WKAuthenticationChallengeGetProposedCredential(challenge)) {
        text.user = takeString(WKCredentialCopyUser(credential));
        result.user = text.user.c_str();
    }
    WKAuthenticationDecisionListenerRef listener = WKAuthenticationChallengeGetDecisionListener(challenge);
    WKRetain(listener);
    result.listener = listener;
    return result;
}

void didReceiveChallenge(WKPageRef, WKAuthenticationChallengeRef challenge, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const Challenge*);
    if (!view->client->at<Function>(0xa8)) {
        WKAuthenticationDecisionListenerPerformDefaultHandling(WKAuthenticationChallengeGetDecisionListener(challenge));
        return;
    }
    ChallengeText text;
    const Challenge value = makeChallenge(challenge, text);
    view->client->at<Function>(0xa8)(view->context, &value);
}

} // namespace cutefish::reef::web
