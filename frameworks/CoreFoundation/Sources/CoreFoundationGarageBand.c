// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <CoreFoundation/CoreFoundation.h>

// File reference URLs have the form file:///.file/id=...
Boolean CFURLIsFileReferenceURL(CFURLRef url) {
    if (!url) return false;
    CFStringRef s = CFURLGetString(url);
    return s && CFStringHasPrefix(s, CFSTR("file:///.file/id="));
}
