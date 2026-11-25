// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "images.h"

#include "graphics.h"
#include "jpeg2000.h"

#include "ui/bitmap.h"
#include "ui/icon.h"

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <fstream>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unistd.h>
#include <vector>

namespace cutefish::reef {

// 63
reef_ui_image* imageCreate(const void* pixels, int width, int height, size_t stride)
{
    const ui::RetainedBitmap bitmap(
        ui::Bitmap::wrap(const_cast<void*>(pixels), width, height, stride, ui::Bitmap::Format::Argb32Premultiplied));
    return bitmap.isNull() ? nullptr : new reef_ui_image{bitmap};
}

// 257
void imageUpdate(reef_ui_image* image, const void* pixels, size_t stride, int x, int y, int width, int height)
{
    const int imageWidth = image->bitmap.width();
    const int imageHeight = image->bitmap.height();
    image->bitmap.update(ui::Bitmap::wrap(const_cast<void*>(pixels), imageWidth, imageHeight, stride,
                                          ui::Bitmap::Format::Argb32Premultiplied),
                         {x, y, width, height});
}

// 64
void imageDestroy(reef_ui_image* image)
{
    delete image;
}

// 65
int gcDrawImage(reef_ui_gc* gc, reef_ui_image* image, double x, double y, double width, double height, int mirrored)
{
    if (gc->pdf || !image)
        return 0;
    gc->canvas->drawBitmap(image->bitmap, {x, y, width, height}, mirrored);
    return 1;
}

// 237
reef_ui_dma_buf* dmaBufCreate(int width, int height, uint32_t fourcc, uint64_t modifier, int planeCount, const int* fds,
                              const uint32_t* offsets, const uint32_t* strides)
{
    std::vector<ui::DmaBufImage::Plane> planes;
    for (int i = 0; i < planeCount; i++)
        planes.push_back({fds[i], offsets[i], strides[i]});
    const ui::DmaBufImage image(width, height, fourcc, modifier, planes);
    return image.isNull() ? nullptr : new reef_ui_dma_buf{image};
}

// 238
void dmaBufDestroy(reef_ui_dma_buf* dmaBuf)
{
    delete dmaBuf;
}

// 308
void dmaBufSetFence(reef_ui_dma_buf* dmaBuf, int fd)
{
    if (dmaBuf)
        dmaBuf->image.setReadyFence(fd);
    else if (fd >= 0)
        close(fd);
}

// 239
int gcDrawDmaBuf(reef_ui_gc* gc, reef_ui_dma_buf* dmaBuf, double x, double y, double width, double height, int mirrored)
{
    if (gc->pdf || !dmaBuf)
        return 0;
    return gc->canvas->drawDmaBufImage(dmaBuf->image, {x, y, width, height}, mirrored);
}

// What a program gets of a bitmap.
static void* pixelsOf(const ui::Bitmap& bitmap, int* width, int* height)
{
    if (bitmap.isNull())
        return nullptr;
    const ui::Bitmap argb = bitmap.format() == ui::Bitmap::Format::Argb32Premultiplied
        ? bitmap
        : bitmap.converted(ui::Bitmap::Format::Argb32Premultiplied);
    const size_t rowBytes = size_t(argb.width()) * 4;
    uint8_t* pixels = static_cast<uint8_t*>(malloc(rowBytes * argb.height()));
    if (!pixels)
        return nullptr;
    for (int y = 0; y < argb.height(); y++)
        memcpy(pixels + y * rowBytes, argb.constScanLine(y), rowBytes);
    *width = argb.width();
    *height = argb.height();
    return pixels;
}

// 81
void* pixelsDecode(const char* path, const void* bytes, size_t size, int* width, int* height)
{
    if (path) {
        if (void* pixels = pixelsOf(ui::Bitmap::decodeFile(path), width, height))
            return pixels;
        std::ifstream file(path, std::ios::binary);
        const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (!isJpeg2000(content.data(), content.size()))
            return nullptr;
        return decodeJpeg2000(content.data(), content.size(), width, height);
    }
    if (!bytes || !size)
        return nullptr;
    if (isJpeg2000(bytes, size))
        return decodeJpeg2000(bytes, size, width, height);
    return pixelsOf(ui::Bitmap::decode({static_cast<const char*>(bytes), size}), width, height);
}

// 83
void pixelsFree(void* pixels)
{
    free(pixels);
}

// 120
void* pixelsEncodePng(const void* pixels, int width, int height, size_t stride, size_t* size)
{
    if (!pixels || width <= 0 || height <= 0 || !size)
        return nullptr;
    const ui::Bitmap bitmap = ui::Bitmap::copyOf(pixels, width, height, stride, ui::Bitmap::Format::Argb32Premultiplied);
    const std::optional<std::string> png = bitmap.encodePng();
    if (!png)
        return nullptr;
    void* bytes = malloc(png->size());
    if (bytes) {
        memcpy(bytes, png->data(), png->size());
        *size = png->size();
    }
    return bytes;
}

// 82
void* iconPixels(const char* name, int size, int* width, int* height)
{
    if (!name || !*name || size <= 0)
        return nullptr;
    const std::string path = ui::themeIconPath(name, size, 1);
    if (path.empty())
        return nullptr;
    return pixelsOf(ui::Bitmap::decodeFile(path, {size, size}), width, height);
}

// 301
void* iconPixelsScaled(const char* name, int size, int pixelSize, int* width, int* height)
{
    if (!name || !*name || size <= 0 || pixelSize <= 0)
        return nullptr;
    const std::string path = ui::themeIconPath(name, size, 1);
    if (path.empty())
        return nullptr;
    return pixelsOf(ui::Bitmap::decodeFile(path, {pixelSize, pixelSize}), width, height);
}

// The symbols are drawn anew in other sizes by changing their files.
struct Symbol
{
    std::string svg;
    double strokeWidth = 2;
    // Of the lines without their width.
    double bounds[4] = {0, 0, 24, 24};
};

static const std::string& symbolsDirectory()
{
    static const std::string directory = []() -> std::string {
        Dl_info info = {};
        if (!dladdr(reinterpret_cast<void*>(symbolsDirectory), &info) || !info.dli_fname)
            return {};
        const std::string path = info.dli_fname;
        return path.substr(0, path.rfind('/') + 1) + "symbols/";
    }();
    return directory;
}

// Empty for a name that is none.
static std::string symbolPath(const char* name)
{
    if (!name || !*name || strchr(name, '/') || symbolsDirectory().empty())
        return {};
    return symbolsDirectory() + name + ".svg";
}

// Attributes of the root.
static std::string attribute(const std::string& svg, const char* name)
{
    const size_t begin = svg.find("<svg");
    const size_t end = begin == std::string::npos ? std::string::npos : svg.find('>', begin);
    const std::string key = std::string(" ") + name + "=\"";
    const size_t at = begin == std::string::npos ? std::string::npos : svg.find(key, begin);
    if (at == std::string::npos || at > end)
        return {};
    const size_t value = at + key.size();
    const size_t quote = svg.find('"', value);
    if (quote == std::string::npos)
        return {};
    return svg.substr(value, quote - value);
}

static void setAttribute(std::string& svg, const char* name, const std::string& value)
{
    const size_t begin = svg.find("<svg");
    if (begin == std::string::npos)
        return;
    const size_t end = svg.find('>', begin);
    const std::string key = std::string(" ") + name + "=\"";
    const size_t at = svg.find(key, begin);
    if (at != std::string::npos && at < end) {
        const size_t old = at + key.size();
        const size_t quote = svg.find('"', old);
        if (quote != std::string::npos)
            svg.replace(old, quote - old, value);
        return;
    }
    svg.insert(begin + 4, key + value + "\"");
}

// The part of the file in the box, drawn in another size; all lines are `weight` times as wide.
static std::string symbolPart(std::string svg, const double* box, int width, int height, double weight, bool filled)
{
    char text[128];
    snprintf(text, sizeof text, "%.5f %.5f %.5f %.5f", box[0], box[1], box[2], box[3]);
    setAttribute(svg, "viewBox", text);
    setAttribute(svg, "width", std::to_string(width));
    setAttribute(svg, "height", std::to_string(height));
    static const std::string key = "stroke-width=\"";
    for (size_t at = svg.find(key); at != std::string::npos; at = svg.find(key, at)) {
        at += key.size();
        const size_t quote = svg.find('"', at);
        if (quote == std::string::npos)
            break;
        double strokeWidth = 0;
        std::from_chars(svg.data() + at, svg.data() + quote, strokeWidth);
        const std::to_chars_result result = std::to_chars(text, text + sizeof text, weight * strokeWidth);
        svg.replace(at, quote - at, text, result.ptr - text);
    }
    if (filled && attribute(svg, "fill") == "none")
        setAttribute(svg, "fill", "currentColor");
    return svg;
}

static const Symbol* symbolAt(const std::string& path)
{
    static std::mutex mutex;
    static std::map<std::string, std::unique_ptr<Symbol>> symbols;
    {
        std::lock_guard<std::mutex> lock(mutex);
        const auto found = symbols.find(path);
        if (found != symbols.end())
            return found->second.get();
    }
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return nullptr;
    auto symbol = std::make_unique<Symbol>();
    symbol->svg.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const std::string strokeWidth = attribute(symbol->svg, "stroke-width");
    if (!strokeWidth.empty())
        std::from_chars(strokeWidth.data(), strokeWidth.data() + strokeWidth.size(), symbol->strokeWidth);
    if (symbol->strokeWidth <= 0)
        symbol->strokeWidth = 2;
    // Drawn large in thin lines to see where they are.
    constexpr int units = 24;
    constexpr int scale = 20;
    const double box[4] = {0, 0, units, units};
    const ui::Bitmap bitmap = ui::Bitmap::decode(
        symbolPart(symbol->svg, box, units * scale, units * scale, 0.03 / symbol->strokeWidth, false),
        {units * scale, units * scale});
    int left = bitmap.width(), top = bitmap.height(), right = -1, bottom = -1;
    for (int y = 0; y < bitmap.height(); y++) {
        for (int x = 0; x < bitmap.width(); x++) {
            if (bitmap.alpha(x, y) <= 8)
                continue;
            left = std::min(left, x);
            right = std::max(right, x);
            top = std::min(top, y);
            bottom = std::max(bottom, y);
        }
    }
    if (right >= 0) {
        symbol->bounds[0] = double(left) / scale;
        symbol->bounds[1] = double(top) / scale;
        symbol->bounds[2] = double(right + 1 - left) / scale;
        symbol->bounds[3] = double(bottom + 1 - top) / scale;
    }
    std::lock_guard<std::mutex> lock(mutex);
    std::unique_ptr<Symbol>& kept = symbols[path];
    if (!kept)
        kept = std::move(symbol);
    return kept.get();
}

static ui::Bitmap drawSymbol(const Symbol& symbol, int width, int height, const double* rect, double weight, bool filled, int margin)
{
    // The bounds are those of the middle of the lines.
    const double drawnWidth = std::max(rect[2] - weight, 0.1);
    const double drawnHeight = std::max(rect[3] - weight, 0.1);
    double scale = 0;
    if (symbol.bounds[2] >= 1)
        scale = drawnWidth / symbol.bounds[2];
    if (symbol.bounds[3] >= 1) {
        const double vertical = drawnHeight / symbol.bounds[3];
        scale = scale > 0 ? std::min(scale, vertical) : vertical;
    }
    if (scale <= 0)
        scale = drawnWidth / std::max(symbol.bounds[2], 1.0);
    const int fullWidth = width + 2 * margin;
    const int fullHeight = height + 2 * margin;
    const double box[4] = {symbol.bounds[0] + symbol.bounds[2] / 2 - (rect[0] + rect[2] / 2 + margin) / scale,
                           symbol.bounds[1] + symbol.bounds[3] / 2 - (rect[1] + rect[3] / 2 + margin) / scale, fullWidth / scale,
                           fullHeight / scale};
    return ui::Bitmap::decode(
        symbolPart(symbol.svg, box, fullWidth, fullHeight, weight / scale / symbol.strokeWidth, filled), {fullWidth, fullHeight});
}

// 211
int symbolExists(const char* name)
{
    const std::string path = symbolPath(name);
    return !path.empty() && access(path.c_str(), R_OK) == 0;
}

// 212
int symbolBounds(const char* name, double* bounds)
{
    const std::string path = symbolPath(name);
    if (path.empty())
        return 0;
    const Symbol* symbol = symbolAt(path);
    if (!symbol || !bounds)
        return 0;
    std::copy(symbol->bounds, symbol->bounds + 4, bounds);
    return 1;
}

// 210
void* symbolPixels(const char* name, int width, int height, const double* rect, double weight, int filled, int margin,
                   int* pixelWidth, int* pixelHeight)
{
    const std::string path = symbolPath(name);
    if (path.empty() || width <= 0 || height <= 0 || !rect)
        return nullptr;
    const Symbol* symbol = symbolAt(path);
    if (!symbol)
        return nullptr;
    char key[384];
    snprintf(key, sizeof key, "%s %d %d %.3f %.3f %.3f %.3f %.4f %d %d", path.c_str(), width, height, rect[0], rect[1], rect[2], rect[3],
             weight, filled, margin);
    static std::mutex mutex;
    static std::map<std::string, ui::Bitmap> bitmaps;
    ui::Bitmap bitmap;
    {
        std::lock_guard<std::mutex> lock(mutex);
        const auto found = bitmaps.find(key);
        if (found != bitmaps.end())
            bitmap = found->second;
    }
    if (bitmap.isNull()) {
        bitmap = drawSymbol(*symbol, width, height, rect, weight, filled, margin);
        if (bitmap.isNull())
            return nullptr;
        std::lock_guard<std::mutex> lock(mutex);
        if (bitmaps.size() >= 512)
            bitmaps.clear();
        bitmaps.emplace(key, bitmap);
    }
    return pixelsOf(bitmap, pixelWidth, pixelHeight);
}

} // namespace cutefish::reef
