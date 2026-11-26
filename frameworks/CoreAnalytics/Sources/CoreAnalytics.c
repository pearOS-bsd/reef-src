// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// AnalyticsSendEventLazy(CFStringRef name, CFDictionaryRef (^payload)(void)): analytics events are dropped.
void AnalyticsSendEventLazy(const void *name, const void *payload) {
    (void)name;
    (void)payload;
}
