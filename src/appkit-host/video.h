// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Video decoding for programs.

#pragma once

#include <cstddef>
#include <cstdint>

struct reef_ui_video_decoder;
struct reef_ui_video_frame;

namespace cutefish::reef {

// The codecs are the four characters of the sample entry: avc1, hvc1, vp09. VP9 needs the size, for the others it is the result.
reef_ui_video_decoder* videoDecoderCreate(uint32_t codec, const void* configuration, size_t length, int* width, int* height,
                                          int* depth);
int videoDecoderDecode(reef_ui_video_decoder* decoder, const void* data, size_t length, reef_ui_video_frame** frame);
void videoDecoderDestroy(reef_ui_video_decoder* decoder);
int videoFrameCopy(reef_ui_video_frame* frame, void* const* planes, const size_t* strides, int width, int height);
int videoFrameExport(reef_ui_video_frame* frame, int* count, int* fds, uint32_t* formats, uint32_t* offsets,
                     uint32_t* pitches, uint64_t* modifiers);
reef_ui_video_frame* videoFrameRetain(reef_ui_video_frame* frame);
void videoFrameRelease(reef_ui_video_frame* frame);
int videoDecoderSupports(uint32_t codec);
void videoFrameSize(reef_ui_video_frame* frame, int* width, int* height);

} // namespace cutefish::reef
