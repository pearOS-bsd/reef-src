// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <Foundation/Foundation.h>

// Transparency, Consent and Control: a client of the privacy database. Reef has no prompts: nothing was ever
// decided, so a preflight answers "unknown" (2) and a request is not granted.
const CFStringRef kTCCServiceScreenCapture = CFSTR("kTCCServiceScreenCapture");
int TCCAccessPreflight(CFStringRef service, CFDictionaryRef options) { return 2; }
void TCCAccessRequest(CFStringRef service, CFDictionaryRef options, void (^callback)(Boolean granted)) {
    if (callback) callback(false);
}
