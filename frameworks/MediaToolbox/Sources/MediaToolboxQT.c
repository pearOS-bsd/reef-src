// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <CoreMedia/CoreMedia.h>
#include <string.h>

// Names of media types and codecs as the Info panel shows them.
CFStringRef MTCopyLocalizedNameForMediaType(OSType type) {
    switch (type) {
    case 'vide': return CFSTR("Video");
    case 'soun': return CFSTR("Sound");
    case 'text': return CFSTR("Text");
    case 'sbtl': return CFSTR("Subtitle");
    case 'clcp': return CFSTR("Closed Caption");
    case 'tmcd': return CFSTR("Timecode");
    default: return CFSTR("Unknown");
    }
}
CFStringRef MTCopyLocalizedNameForMediaSubType(OSType mediaType, OSType subType) {
    switch (subType) {
    case 'avc1': return CFSTR("H.264");
    case 'hvc1': case 'hev1': return CFSTR("HEVC");
    case 'mp4v': return CFSTR("MPEG-4 Video");
    case 'apcn': return CFSTR("Apple ProRes 422");
    case 'ap4h': return CFSTR("Apple ProRes 4444");
    case 'jpeg': return CFSTR("Photo - JPEG");
    case 'mp4a': return CFSTR("AAC");
    case 'lpcm': return CFSTR("Linear PCM");
    case 'alac': return CFSTR("Apple Lossless");
    case 'ac-3': return CFSTR("AC-3");
    case '.mp3': return CFSTR("MPEG Layer 3");
    default: return CFSTR("Unknown");
    }
}
CFStringRef MTCopyLocalizedStringForVideoDynamicRange(CFStringRef range) { return CFSTR("Standard Dynamic Range (SDR)"); }
CFStringRef MTCopyStringForColorInfo(CFStringRef primaries, CFStringRef transfer, CFStringRef matrix) {
    return CFStringCreateWithFormat(NULL, NULL, CFSTR("%@-%@-%@"), primaries ?: CFSTR("Unknown"), transfer ?: CFSTR("Unknown"), matrix ?: CFSTR("Unknown"));
}
void MTRegisterProfessionalVideoWorkflowFormatReaders(void) {}
// Assets of the Figo layer: no such layer; the call fails with kFigAssetError (-12780: not supported here).
int FigAssetCreateWithURL(CFAllocatorRef allocator, CFURLRef url, int options, void **assetOut) {
    if (assetOut) *assetOut = NULL;
    return -12780;
}
void FigGetRotationAngleAndFlipsFromCGAffineTransform(double a, double b, double c, double d, double tx, double ty, double *angle, int *flipH, int *flipV) {}
