// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Video decoding for programs: decoders and frames.

#include "video.h"
#include "video_private.h"

#include <algorithm>
#include <atomic>

struct reef_ui_video_decoder
{
    std::unique_ptr<cutefish::reef::video::Decoder> decoder;
};

struct reef_ui_video_frame
{
    std::shared_ptr<cutefish::reef::video::Surface> surface;
    int codedWidth = 0;
    int codedHeight = 0;
    int width = 0;
    int height = 0;
    std::atomic<int> references = 1;
};

namespace cutefish::reef {

enum : uint32_t {
    H264 = 0x61766331, // avc1
    Hevc = 0x68766331, // hvc1
    Vp9 = 0x76703039, // vp09
};

static int visibleWidth(const video::Decoder& decoder)
{
    return decoder.width ? decoder.width : decoder.codedWidth;
}

static int visibleHeight(const video::Decoder& decoder)
{
    return decoder.height ? decoder.height : decoder.codedHeight;
}

// 240
reef_ui_video_decoder* videoDecoderCreate(uint32_t codec, const void* configuration, size_t length, int* width, int* height,
                                          int* depth)
{
    if (!configuration)
        return nullptr;
    const uint8_t* bytes = static_cast<const uint8_t*>(configuration);
    std::unique_ptr<video::Decoder> decoder;
    if (codec == H264)
        decoder = video::makeH264Decoder(bytes, length);
    else if (codec == Hevc)
        decoder = video::makeHevcDecoder(bytes, length);
    else if (codec == Vp9 && width && height)
        decoder = video::makeVp9Decoder(bytes, length, *width, *height);
    if (!decoder)
        return nullptr;
    if (width)
        *width = visibleWidth(*decoder);
    if (height)
        *height = visibleHeight(*decoder);
    if (depth)
        *depth = decoder->depth;
    return new reef_ui_video_decoder{std::move(decoder)};
}

// 242
void videoDecoderDestroy(reef_ui_video_decoder* decoder)
{
    delete decoder;
}

// 243
int videoFrameCopy(reef_ui_video_frame* frame, void* const* planes, const size_t* strides, int width, int height)
{
    if (!frame || !planes || !strides || width <= 0 || height <= 0)
        return false;
    uint8_t* const destination[2] = {static_cast<uint8_t*>(planes[0]), static_cast<uint8_t*>(planes[1])};
    return frame->surface->copy(destination, strides, std::min(width, frame->codedWidth),
                                std::min(height, frame->codedHeight));
}

// 244
int videoFrameExport(reef_ui_video_frame* frame, int* count, int* fds, uint32_t* formats, uint32_t* offsets,
                     uint32_t* pitches, uint64_t* modifiers)
{
    if (!frame || !count || !fds || !formats || !offsets || !pitches || !modifiers)
        return false;
    video::Planes planes;
    if (!frame->surface->exportPlanes(planes))
        return false;
    *count = planes.count;
    for (int i = 0; i < planes.count; i++) {
        fds[i] = planes.fds[i];
        formats[i] = planes.formats[i];
        offsets[i] = planes.offsets[i];
        pitches[i] = planes.pitches[i];
        modifiers[i] = planes.modifiers[i];
    }
    return true;
}

// 245
reef_ui_video_frame* videoFrameRetain(reef_ui_video_frame* frame)
{
    if (frame)
        frame->references++;
    return frame;
}

// 306
void videoFrameSize(reef_ui_video_frame* frame, int* width, int* height)
{
    if (width)
        *width = frame ? frame->width : 0;
    if (height)
        *height = frame ? frame->height : 0;
}

// 248
int videoDecoderSupports(uint32_t codec)
{
    const std::shared_ptr<video::VaDisplay> display = video::VaDisplay::open();
    if (!display)
        return false;
    if (codec == H264)
        return display->supports(video::VAProfileH264High) || display->supports(video::VAProfileH264Main);
    if (codec == Hevc)
        return display->supports(video::VAProfileHEVCMain);
    if (codec == Vp9)
        return display->supports(video::VAProfileVP9Profile0);
    return false;
}

// 241
int videoDecoderDecode(reef_ui_video_decoder* decoder, const void* data, size_t length, reef_ui_video_frame** frame)
{
    if (frame)
        *frame = nullptr;
    if (!decoder || !data)
        return false;
    std::shared_ptr<video::Surface> surface;
    if (!decoder->decoder->decode(static_cast<const uint8_t*>(data), length, surface))
        return false;
    if (surface && frame) {
        auto* result = new reef_ui_video_frame;
        result->surface = std::move(surface);
        result->codedWidth = decoder->decoder->codedWidth;
        result->codedHeight = decoder->decoder->codedHeight;
        result->width = visibleWidth(*decoder->decoder);
        result->height = visibleHeight(*decoder->decoder);
        *frame = result;
    }
    return true;
}

// 246
void videoFrameRelease(reef_ui_video_frame* frame)
{
    if (frame && --frame->references == 0)
        delete frame;
}

} // namespace cutefish::reef
