// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Small functions of Reef's CoreFoundation: the uncaught exception handler slot, forced preferences (none), weak reference flag.
#include "CFInternal.h"

typedef void NSUncaughtExceptionHandler(void *exception);

static NSUncaughtExceptionHandler *uncaughtHandler;

NSUncaughtExceptionHandler *NSGetUncaughtExceptionHandler(void) {
    return uncaughtHandler;
}

void NSSetUncaughtExceptionHandler(NSUncaughtExceptionHandler *handler) {
    uncaughtHandler = handler;
}

Boolean CFPreferencesAppValueIsForced(CFStringRef key, CFStringRef applicationID) {
    return false;
}

void _CFSetWeaklyReferenced(CFTypeRef cf) {
    CFRuntimeBase *base = (CFRuntimeBase *)cf;
    if ((uint64_t)base->_cfinfoa >> 32 != 0) {
        __c11_atomic_fetch_or(&base->_cfinfoa, 0x100000, __ATOMIC_SEQ_CST);
    }
}
