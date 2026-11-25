// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The answers to what a page asks.

#include "web.h"

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

} // namespace cutefish::reef::web
