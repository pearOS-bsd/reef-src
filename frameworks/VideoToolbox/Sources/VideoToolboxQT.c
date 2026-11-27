// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <CoreFoundation/CoreFoundation.h>

// Media extensions (decoders in app extensions): none installed.
CFArrayRef VTCopyVideoDecoderExtensionProperties(void) { return CFArrayCreate(NULL, NULL, 0, &kCFTypeArrayCallBacks); }
Boolean VTIsStereoMVHEVCDecodeSupported(void) { return false; }
