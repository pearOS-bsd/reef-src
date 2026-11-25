// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// JPEG 2000 images, read and written by OpenJPEG, which is loaded when a program first needs it.

#include "jpeg2000.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <mutex>
#include <vector>

namespace cutefish::reef {

namespace {

// What openjpeg.h declares.
struct Codec;
struct Stream;

struct Component
{
    uint32_t dx;
    uint32_t dy;
    uint32_t width;
    uint32_t height;
    uint32_t x0;
    uint32_t y0;
    uint32_t precision;
    uint32_t bitsPerPixel;
    uint32_t isSigned;
    uint32_t decodedResolutions;
    uint32_t factor;
    int32_t* data;
    uint16_t alpha;
};

struct Image
{
    uint32_t x0;
    uint32_t y0;
    uint32_t x1;
    uint32_t y1;
    uint32_t componentCount;
    int colorSpace;
    Component* components;
};

enum ColorSpace { SRgb = 1, SYcc = 3, EYcc = 4, Cmyk = 5 };
enum Format { Codestream = 0, Jp2 = 2 };

struct ComponentParameters
{
    uint32_t dx;
    uint32_t dy;
    uint32_t width;
    uint32_t height;
    uint32_t x0;
    uint32_t y0;
    uint32_t precision;
    uint32_t bitsPerPixel;
    uint32_t isSigned;
};

// The first of them, up to the last that is set.
struct EncoderParameters
{
    int tileSizeOn;
    int tileX0;
    int tileY0;
    int tileWidth;
    int tileHeight;
    int allocateByRate;
    int allocateFixed;
    int allocateByQuality;
    int* matrices;
    char* comment;
    int codingStyle;
    int progressionOrder;
    unsigned char progressionChanges[32][148];
    uint32_t progressionChangeCount;
    int layerCount;
    float rates[100];
    float qualities[100];
    int resolutionCount;
    int blockWidth;
    int blockHeight;
    int mode;
    int irreversible;
};

// More than the parameters of any version take.
constexpr size_t parametersSize = 65536;
constexpr size_t streamBufferSize = 65536;
constexpr uint32_t maximumSide = 65536;

using MessageHandler = void (*)(const char* message, void* context);

struct Library
{
    bool loaded = false;
    bool decodes = false;
    Stream* (*createStream)(size_t bufferSize, int input);
    void (*setRead)(Stream* stream, size_t (*read)(void* buffer, size_t size, void* context));
    void (*setSkip)(Stream* stream, int64_t (*skip)(int64_t count, void* context));
    void (*setSeek)(Stream* stream, int (*seek)(int64_t offset, void* context));
    void (*setContext)(Stream* stream, void* context, void (*free)(void* context));
    void (*setLength)(Stream* stream, uint64_t length);
    void (*destroyStream)(Stream* stream);
    Codec* (*createDecompress)(int format);
    void (*destroyCodec)(Codec* codec);
    void (*setDefaultDecoderParameters)(void* parameters);
    int (*setupDecoder)(Codec* codec, void* parameters);
    int (*readHeader)(Stream* stream, Codec* codec, Image** image);
    int (*decode)(Codec* codec, Stream* stream, Image* image);
    int (*endDecompress)(Codec* codec, Stream* stream);
    void (*destroyImage)(Image* image);
    int (*setErrorHandler)(Codec* codec, MessageHandler handler, void* context);
    int (*setWarningHandler)(Codec* codec, MessageHandler handler, void* context);
    bool encodes = false;
    void (*setWrite)(Stream* stream, size_t (*write)(void* buffer, size_t size, void* context));
    Codec* (*createCompress)(int format);
    void (*setDefaultEncoderParameters)(EncoderParameters* parameters);
    int (*setupEncoder)(Codec* codec, EncoderParameters* parameters, Image* image);
    Image* (*createImage)(uint32_t count, ComponentParameters* parameters, int colorSpace);
    int (*startCompress)(Codec* codec, Image* image, Stream* stream);
    int (*encode)(Codec* codec, Stream* stream);
    int (*endCompress)(Codec* codec, Stream* stream);
};

// What is read, and what is written.
struct Source
{
    const unsigned char* data;
    size_t size;
    size_t offset;
};

struct Sink
{
    std::vector<unsigned char> data;
    size_t offset = 0;
};

} // namespace

static Library loadedLibrary;
static std::mutex libraryMutex;

template <typename Function>
static bool load(void* handle, const char* name, Function& function)
{
    function = reinterpret_cast<Function>(dlsym(handle, name));
    return function != nullptr;
}

static const Library& library()
{
    Library& library = loadedLibrary;
    std::lock_guard<std::mutex> lock(libraryMutex);
    if (library.loaded)
        return library;
    library.loaded = true;
    void* handle = dlopen("libopenjp2.so.7", RTLD_NOW);
    if (!handle)
        return library;
    library.decodes = load(handle, "opj_stream_create", library.createStream)
        && load(handle, "opj_stream_set_read_function", library.setRead)
        && load(handle, "opj_stream_set_skip_function", library.setSkip)
        && load(handle, "opj_stream_set_seek_function", library.setSeek)
        && load(handle, "opj_stream_set_user_data", library.setContext)
        && load(handle, "opj_stream_set_user_data_length", library.setLength)
        && load(handle, "opj_stream_destroy", library.destroyStream)
        && load(handle, "opj_create_decompress", library.createDecompress)
        && load(handle, "opj_destroy_codec", library.destroyCodec)
        && load(handle, "opj_set_default_decoder_parameters", library.setDefaultDecoderParameters)
        && load(handle, "opj_setup_decoder", library.setupDecoder)
        && load(handle, "opj_read_header", library.readHeader)
        && load(handle, "opj_decode", library.decode)
        && load(handle, "opj_end_decompress", library.endDecompress)
        && load(handle, "opj_image_destroy", library.destroyImage)
        && load(handle, "opj_set_error_handler", library.setErrorHandler)
        && load(handle, "opj_set_warning_handler", library.setWarningHandler);
    library.encodes = library.decodes
        && load(handle, "opj_stream_set_write_function", library.setWrite)
        && load(handle, "opj_create_compress", library.createCompress)
        && load(handle, "opj_set_default_encoder_parameters", library.setDefaultEncoderParameters)
        && load(handle, "opj_setup_encoder", library.setupEncoder)
        && load(handle, "opj_image_create", library.createImage)
        && load(handle, "opj_start_compress", library.startCompress)
        && load(handle, "opj_encode", library.encode)
        && load(handle, "opj_end_compress", library.endCompress);
    return library;
}

static size_t readSource(void* buffer, size_t size, void* context)
{
    Source* source = static_cast<Source*>(context);
    if (source->offset >= source->size)
        return size_t(-1);
    const size_t length = std::min(size, source->size - source->offset);
    memcpy(buffer, source->data + source->offset, length);
    source->offset += length;
    return length;
}

static int64_t skipSource(int64_t count, void* context)
{
    Source* source = static_cast<Source*>(context);
    const int64_t offset = int64_t(source->offset);
    source->offset = size_t(std::min(std::max<int64_t>(offset + count, 0), int64_t(source->size)));
    return int64_t(source->offset) - offset;
}

static int seekSource(int64_t offset, void* context)
{
    Source* source = static_cast<Source*>(context);
    if (offset < 0 || size_t(offset) > source->size)
        return 0;
    source->offset = size_t(offset);
    return 1;
}

static size_t writeSink(void* buffer, size_t size, void* context)
{
    Sink* sink = static_cast<Sink*>(context);
    const size_t end = sink->offset + size;
    if (sink->data.size() < end)
        sink->data.resize(end);
    memcpy(sink->data.data() + sink->offset, buffer, size);
    sink->offset += size;
    return size;
}

static int seekSink(int64_t offset, void* context)
{
    Sink* sink = static_cast<Sink*>(context);
    if (offset < 0)
        return 0;
    sink->offset = size_t(offset);
    if (sink->data.size() < sink->offset)
        sink->data.resize(sink->offset);
    return 1;
}

static int64_t skipSink(int64_t count, void* context)
{
    Sink* sink = static_cast<Sink*>(context);
    const int64_t offset = int64_t(sink->offset) + count;
    if (offset < 0)
        return -1;
    sink->offset = size_t(offset);
    if (sink->data.size() < sink->offset)
        sink->data.resize(sink->offset);
    return count;
}

static void ignoreMessage(const char*, void*) {}

// The sample for a pixel of the image, as 8 bits.
static int sampleAt(const Component& component, uint32_t x, uint32_t y)
{
    const uint32_t column = std::min(x / std::max(component.dx, 1u), component.width ? component.width - 1 : 0);
    const uint32_t row = std::min(y / std::max(component.dy, 1u), component.height ? component.height - 1 : 0);
    int value = component.data[size_t(row) * component.width + column];
    if (component.isSigned)
        value += 1 << (component.precision - 1);
    const int maximum = (1 << std::min(component.precision, 30u)) - 1;
    value = std::min(std::max(value, 0), maximum);
    return component.precision == 8 ? value : int(std::lround(value * 255.0 / maximum));
}

static int clampByte(double value)
{
    return int(std::min(std::max(std::lround(value), 0l), 255l));
}

bool isJpeg2000(const void* data, size_t size)
{
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    if (size >= 12 && memcmp(bytes, "\0\0\0\x0cjP  \r\n\x87\n", 12) == 0)
        return true;
    return size >= 4 && bytes[0] == 0xff && bytes[1] == 0x4f && bytes[2] == 0xff && bytes[3] == 0x51;
}

uint32_t* decodeJpeg2000(const void* data, size_t size, int* width, int* height)
{
    const Library& lib = library();
    if (!lib.decodes || !isJpeg2000(data, size))
        return nullptr;
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    Codec* codec = lib.createDecompress(bytes[0] == 0xff ? Codestream : Jp2);
    if (!codec)
        return nullptr;
    lib.setErrorHandler(codec, ignoreMessage, nullptr);
    lib.setWarningHandler(codec, ignoreMessage, nullptr);
    void* parameters = calloc(1, parametersSize);
    lib.setDefaultDecoderParameters(parameters);
    Source source = {bytes, size, 0};
    Stream* stream = lib.createStream(streamBufferSize, 1);
    lib.setRead(stream, readSource);
    lib.setSkip(stream, skipSource);
    lib.setSeek(stream, seekSource);
    lib.setContext(stream, &source, nullptr);
    lib.setLength(stream, size);
    Image* image = nullptr;
    uint32_t* pixels = nullptr;
    if (lib.setupDecoder(codec, parameters) && lib.readHeader(stream, codec, &image) && lib.decode(codec, stream, image)
        && lib.endDecompress(codec, stream) && image && image->componentCount && image->components) {
        const Component* components = image->components;
        const uint32_t imageWidth = image->x1 - image->x0;
        const uint32_t imageHeight = image->y1 - image->y0;
        if (imageWidth > 0 && imageHeight > 0 && imageWidth <= maximumSide && imageHeight <= maximumSide && components[0].data
            && components[0].precision > 0 && components[0].precision <= 31
            && components[0].width == (imageWidth + components[0].dx - 1) / std::max(components[0].dx, 1u)) {
            int alpha = -1;
            uint32_t colors = image->componentCount;
            for (uint32_t i = 0; i < image->componentCount; i++) {
                if (components[i].alpha) {
                    alpha = int(i);
                    colors = std::min(colors, i);
                }
            }
            // Files without a channel definition.
            if (alpha < 0 && (image->componentCount == 2 || (image->componentCount == 4 && image->colorSpace != Cmyk))) {
                alpha = int(image->componentCount - 1);
                colors = image->componentCount - 1;
            }
            pixels = static_cast<uint32_t*>(malloc(size_t(imageWidth) * imageHeight * 4));
            if (pixels) {
                for (uint32_t y = 0; y < imageHeight; y++) {
                    for (uint32_t x = 0; x < imageWidth; x++) {
                        int red, green, blue;
                        if (colors >= 4) {
                            const int cyan = sampleAt(components[0], x, y);
                            const int magenta = sampleAt(components[1], x, y);
                            const int yellow = sampleAt(components[2], x, y);
                            const int rest = 255 - sampleAt(components[3], x, y);
                            red = (255 - cyan) * rest / 255;
                            green = (255 - magenta) * rest / 255;
                            blue = (255 - yellow) * rest / 255;
                        } else if (colors == 3) {
                            red = sampleAt(components[0], x, y);
                            green = sampleAt(components[1], x, y);
                            blue = sampleAt(components[2], x, y);
                            if (image->colorSpace == SYcc || image->colorSpace == EYcc) {
                                const double cb = green - 128.0;
                                const double cr = blue - 128.0;
                                const double luma = red;
                                red = clampByte(luma + 1.402 * cr);
                                green = clampByte(luma - 0.344136 * cb - 0.714136 * cr);
                                blue = clampByte(luma + 1.772 * cb);
                            }
                        } else {
                            red = green = blue = sampleAt(components[0], x, y);
                        }
                        const int opacity = alpha >= 0 ? sampleAt(components[alpha], x, y) : 255;
                        pixels[size_t(y) * imageWidth + x] = uint32_t(opacity) << 24 | uint32_t(red * opacity / 255) << 16
                            | uint32_t(green * opacity / 255) << 8 | uint32_t(blue * opacity / 255);
                    }
                }
                *width = int(imageWidth);
                *height = int(imageHeight);
            }
        }
    }
    if (image)
        lib.destroyImage(image);
    lib.destroyStream(stream);
    lib.destroyCodec(codec);
    free(parameters);
    return pixels;
}

// 312
void* hostImageEncodeJpeg2000(const void* pixels, int width, int height, size_t stride, double quality, size_t* size)
{
    const Library& lib = library();
    if (!lib.encodes || !pixels || width < 1 || height < 1 || width > int(maximumSide) || height > int(maximumSide)
        || stride < size_t(width) * 4 || !size)
        return nullptr;
    const unsigned char* bytes = static_cast<const unsigned char*>(pixels);
    bool opaque = true;
    for (int y = 0; y < height && opaque; y++) {
        const uint32_t* row = reinterpret_cast<const uint32_t*>(bytes + y * stride);
        for (int x = 0; x < width && opaque; x++)
            opaque = row[x] >> 24 == 255;
    }
    const uint32_t count = opaque ? 3 : 4;
    ComponentParameters components[4] = {};
    for (uint32_t i = 0; i < count; i++) {
        components[i].dx = 1;
        components[i].dy = 1;
        components[i].width = uint32_t(width);
        components[i].height = uint32_t(height);
        components[i].precision = 8;
        components[i].bitsPerPixel = 8;
    }
    Image* image = lib.createImage(count, components, SRgb);
    if (!image)
        return nullptr;
    image->x1 = uint32_t(width);
    image->y1 = uint32_t(height);
    if (!opaque)
        image->components[3].alpha = 1;
    for (int y = 0; y < height; y++) {
        const uint32_t* row = reinterpret_cast<const uint32_t*>(bytes + y * stride);
        for (int x = 0; x < width; x++) {
            const uint32_t pixel = row[x];
            const uint32_t alpha = pixel >> 24;
            uint32_t red = 0, green = 0, blue = 0;
            if (alpha) {
                red = pixel >> 16 & 0xff;
                green = pixel >> 8 & 0xff;
                blue = pixel & 0xff;
                if (alpha != 255) {
                    red = std::min((red * 255 + alpha / 2) / alpha, 255u);
                    green = std::min((green * 255 + alpha / 2) / alpha, 255u);
                    blue = std::min((blue * 255 + alpha / 2) / alpha, 255u);
                }
            }
            const size_t index = size_t(y) * width + x;
            image->components[0].data[index] = int(red);
            image->components[1].data[index] = int(green);
            image->components[2].data[index] = int(blue);
            if (!opaque)
                image->components[3].data[index] = int(alpha);
        }
    }
    Codec* codec = lib.createCompress(Jp2);
    if (!codec) {
        lib.destroyImage(image);
        return nullptr;
    }
    lib.setErrorHandler(codec, ignoreMessage, nullptr);
    lib.setWarningHandler(codec, ignoreMessage, nullptr);
    EncoderParameters* parameters = static_cast<EncoderParameters*>(calloc(1, parametersSize));
    lib.setDefaultEncoderParameters(parameters);
    parameters->layerCount = 1;
    parameters->allocateByRate = 1;
    if (quality >= 0 && quality < 1) {
        const double loss = 1 - quality;
        parameters->irreversible = 1;
        parameters->rates[0] = float(1 + 19 * loss * loss);
    } else {
        parameters->rates[0] = 0;
    }
    int resolutions = 1;
    while (resolutions < parameters->resolutionCount && std::min(width, height) >> resolutions > 0)
        resolutions++;
    parameters->resolutionCount = resolutions;
    Sink sink;
    Stream* stream = lib.createStream(streamBufferSize, 0);
    lib.setWrite(stream, writeSink);
    lib.setSkip(stream, skipSink);
    lib.setSeek(stream, seekSink);
    lib.setContext(stream, &sink, nullptr);
    const bool written = lib.setupEncoder(codec, parameters, image) && lib.startCompress(codec, image, stream)
        && lib.encode(codec, stream) && lib.endCompress(codec, stream);
    lib.destroyStream(stream);
    lib.destroyCodec(codec);
    lib.destroyImage(image);
    free(parameters);
    if (!written || sink.data.empty())
        return nullptr;
    void* result = malloc(sink.data.size());
    if (result) {
        memcpy(result, sink.data.data(), sink.data.size());
        *size = sink.data.size();
    }
    return result;
}

} // namespace cutefish::reef
