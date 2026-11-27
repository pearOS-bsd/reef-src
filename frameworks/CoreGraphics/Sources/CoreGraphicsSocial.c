// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <CoreGraphics/CoreGraphics.h>

// A tap restricted to the process with this serial number. Granting taps needs the user's approval
// (Accessibility/Input Monitoring), which Reef does not have: the answer is that of CGEventTapCreate.
CFMachPortRef CGEventTapCreateForPSN(void *processSerialNumber, CGEventTapPlacement place, CGEventTapOptions options,
                                     CGEventMask eventsOfInterest, CGEventTapCallBack callback, void *userInfo) {
    (void)processSerialNumber;
    return CGEventTapCreate(kCGSessionEventTap, place, options, eventsOfInterest, callback, userInfo);
}
