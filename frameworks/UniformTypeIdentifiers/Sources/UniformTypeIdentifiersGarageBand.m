// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

// Core types that Reef's framework does not export: made when the library is loaded.
UTType *ReefTypeAIFF __asm__("_UTTypeAIFF");
UTType *ReefTypeWAV __asm__("_UTTypeWAV");

__attribute__((constructor)) static void makeTypes(void) {
    ReefTypeAIFF = [UTType typeWithIdentifier:@"public.aiff-audio"];
    ReefTypeWAV = [UTType typeWithIdentifier:@"com.microsoft.waveform-audio"];
}
