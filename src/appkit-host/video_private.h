// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the video decoders share. The headers of the streams are parsed here, the GPU decodes the rest.

#pragma once

#include "video_va.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

namespace cutefish::reef::video {

// A NAL unit without the bytes that keep start codes out of it, and where they were.
struct Rbsp
{
    std::vector<uint8_t> bytes;
    std::vector<size_t> removed;

    size_t removedBefore(size_t position) const;
};

Rbsp unescape(const uint8_t* data, size_t size);

// Past the end there are zeros.
class BitReader
{
public:
    BitReader(const uint8_t* data, size_t size);
    explicit BitReader(const Rbsp& rbsp);

    uint32_t bits(int count);
    bool bit();
    // Exponential Golomb codes.
    uint32_t ue();
    int32_t se();
    void skip(size_t count);
    // Whether there is something before the trailing bits.
    bool more() const;

    size_t position() const { return m_position; }
    bool failed() const { return m_failed; }

private:
    const uint8_t* m_data;
    size_t m_size;
    size_t m_position = 0;
    bool m_failed = false;
};

class VaDisplay
{
public:
    // The first render node that can decode. One for all decoders, as long as there are some.
    static std::shared_ptr<VaDisplay> open();
    ~VaDisplay();

    bool supports(VAProfile profile) const;

    const VaApi& api() const { return *m_api; }
    VADisplay handle() const { return m_handle; }

private:
    const VaApi* m_api = nullptr;
    VADisplay m_handle = nullptr;
    int m_fd = -1;
    std::vector<VAProfile> m_profiles;
};

class Surface;

// The surfaces that are not shown any more are used again.
class SurfacePool : public std::enable_shared_from_this<SurfacePool>
{
public:
    SurfacePool(std::shared_ptr<VaDisplay> display, uint32_t format, uint32_t fourcc, int width, int height);
    ~SurfacePool();

    std::shared_ptr<Surface> take();
    void put(VASurfaceID id);
    // For reading the pixels. With the mutex.
    const VAImage* image();

    const VaDisplay& display() const { return *m_display; }
    uint32_t fourcc() const { return m_fourcc; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    std::mutex& imageMutex() { return m_imageMutex; }

private:
    std::shared_ptr<VaDisplay> m_display;
    uint32_t m_format;
    uint32_t m_fourcc;
    int m_width;
    int m_height;
    std::mutex m_mutex;
    std::vector<VASurfaceID> m_free;
    std::vector<VASurfaceID> m_all;
    std::mutex m_imageMutex;
    VAImage m_image = {};
    bool m_hasImage = false;
    bool m_imageFailed = false;
};

// A layer of a surface as a DMA buffer.
struct Planes
{
    uint32_t fourcc = 0;
    int count = 0;
    int fds[4] = {-1, -1, -1, -1};
    uint32_t formats[4] = {};
    uint32_t offsets[4] = {};
    uint32_t pitches[4] = {};
    uint64_t modifiers[4] = {};
};

class Surface
{
public:
    Surface(std::shared_ptr<SurfacePool> pool, VASurfaceID id) : m_pool(std::move(pool)), m_id(id) {}
    ~Surface();

    VASurfaceID id() const { return m_id; }

    // As NV12 or P010.
    bool copy(uint8_t* const* planes, const size_t* strides, int width, int height) const;
    // The descriptors are the caller's.
    bool exportPlanes(Planes& planes) const;

private:
    std::shared_ptr<SurfacePool> m_pool;
    VASurfaceID m_id;
};

class Context
{
public:
    Context(std::shared_ptr<VaDisplay> display, VAProfile profile, int width, int height, int depth);
    ~Context();

    bool isValid() const { return m_context != VA_INVALID_ID; }
    std::shared_ptr<Surface> takeSurface() { return m_pool->take(); }
    void resize(int width, int height);

    // What the next picture consists of.
    bool addBuffer(VABufferType type, const void* data, size_t size);
    bool render(VASurfaceID target);
    void discard();

private:
    std::shared_ptr<VaDisplay> m_display;
    VAConfigID m_config = VA_INVALID_ID;
    VAContextID m_context = VA_INVALID_ID;
    std::shared_ptr<SurfacePool> m_pool;
    std::vector<VABufferID> m_buffers;
    uint32_t m_format = 0;
    int m_depth = 8;
};

class Decoder
{
public:
    virtual ~Decoder() = default;
    // One frame in decoding order. There may be no picture to show.
    virtual bool decode(const uint8_t* data, size_t size, std::shared_ptr<Surface>& surface) = 0;

    int codedWidth = 0;
    int codedHeight = 0;
    // The part that is shown, zero for all of it.
    int width = 0;
    int height = 0;
    int depth = 8;
};

// The NAL units of a frame, each after its length.
template<typename Handler>
bool forEachUnit(const uint8_t* data, size_t size, int lengthSize, Handler&& handler)
{
    size_t offset = 0;
    while (offset + lengthSize <= size) {
        size_t length = 0;
        for (int i = 0; i < lengthSize; i++)
            length = length << 8 | data[offset + i];
        offset += lengthSize;
        if (length > size - offset)
            return false;
        if (length > 0 && !handler(data + offset, length))
            return false;
        offset += length;
    }
    return offset == size;
}

// The configuration is what MP4 files have (avcC, hvcC, vpcC), the units of frames have their lengths before them.
std::unique_ptr<Decoder> makeH264Decoder(const uint8_t* configuration, size_t size);
std::unique_ptr<Decoder> makeHevcDecoder(const uint8_t* configuration, size_t size);
std::unique_ptr<Decoder> makeVp9Decoder(const uint8_t* configuration, size_t size, int width, int height);

} // namespace cutefish::reef::video
