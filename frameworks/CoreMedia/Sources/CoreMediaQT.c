// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <CoreMedia/CoreMedia.h>

const CFStringRef kFigFormatDescriptionExtension_BitsPerComponent = CFSTR("BitsPerComponent");

const AudioChannelLayout *CMAudioFormatDescriptionGetChannelLayout(CMAudioFormatDescriptionRef desc, size_t *sizeOut) {
    if (sizeOut) *sizeOut = 0;
    return NULL;
}
const AudioFormatListItem *CMAudioFormatDescriptionGetRichestDecodableFormat(CMAudioFormatDescriptionRef desc) { return NULL; }
const void *FigAudioFormatDescriptionGetRichestDecodableFormatAndChannelLayout(CMAudioFormatDescriptionRef desc, const void **layout) {
    if (layout) *layout = NULL;
    return NULL;
}
CGSize CMVideoFormatDescriptionGetPresentationDimensions(CMVideoFormatDescriptionRef desc, Boolean usePixelAspectRatio, Boolean useCleanAperture) {
    CMVideoDimensions d = CMVideoFormatDescriptionGetDimensions(desc);
    return CGSizeMake(d.width, d.height);
}
// The dynamic range of the video: nothing special (SDR) answers NULL.
CFStringRef CMVideoFormatDescriptionGetVideoDynamicRange(CMVideoFormatDescriptionRef desc) { return NULL; }
Boolean FigVideoFormatDescriptionContainsAlphaChannel(CMVideoFormatDescriptionRef desc) {
    CFTypeRef v = desc ? CMFormatDescriptionGetExtension(desc, CFSTR("ContainsAlphaChannel")) : NULL;
    return v == kCFBooleanTrue;
}
