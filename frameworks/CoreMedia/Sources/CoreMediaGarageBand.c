// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// CoreMedia functions GarageBand imports and Reef's CoreMedia lacks (text, timecode and audio device clock).
#include <CoreFoundation/CoreFoundation.h>
#include <stdint.h>

typedef struct { int64_t value; int32_t timescale; uint32_t flags; int64_t epoch; } ReefTime;
#define InvalidParameter (-12710)   // kCMFormatDescriptionError_InvalidParameter

int32_t CMTextFormatDescriptionGetDisplayFlags(const void *d, uint32_t *flags) { return InvalidParameter; }
int32_t CMTextFormatDescriptionGetJustification(const void *d, int8_t *h, int8_t *v) { return InvalidParameter; }
int32_t CMTextFormatDescriptionGetDefaultTextBox(const void *d, Boolean originIsTop, double height, void *box) { return InvalidParameter; }
int32_t CMTextFormatDescriptionGetDefaultStyle(const void *d, uint16_t *font, Boolean *bold, Boolean *italic, Boolean *underline, double *size, double *color) { return InvalidParameter; }
int32_t CMTextFormatDescriptionGetFontName(const void *d, uint16_t font, CFStringRef *name) { return InvalidParameter; }
ReefTime CMTimeCodeFormatDescriptionGetFrameDuration(const void *d) { return (ReefTime){0, 0, 0, 0}; }   // kCMTimeInvalid
int32_t CMAudioDeviceClockGetAudioDevice(const void *clock, CFStringRef *uid, uint32_t *device, Boolean *tracking) { return -12745; }   // kCMBaseObjectError_ParamErr
int32_t CMSampleBufferSetOutputPresentationTimeStamp(void *buffer, ReefTime time) { return 0; }
