// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the view itself asks of the program.

#include "web.h"

#include <cstring>
#include <poll.h>
#include <unistd.h>

namespace cutefish::reef::web {

View* viewOf(const void* clientInfo) { return static_cast<View*>(const_cast<void*>(clientInfo)); }

void setCursor(WKViewRef, const char* name, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*, const char*)>(0x8))
        function(view->context, name);
}

void toolTipChanged(WKViewRef, const char* text, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*, const char*)>(0x10))
        function(view->context, text);
}

void contentsSizeChanged(WKViewRef, WKSize size, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*, WKSize)>(0xb8))
        function(view->context, size);
}

void enterFullScreen(WKViewRef, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*)>(0xd8))
        function(view->context);
}

void exitFullScreen(WKViewRef, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*)>(0xe0))
        function(view->context);
}

void hidePopupMenu(WKViewRef, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*)>(0x88))
        function(view->context);
}

void wheelEventNotHandled(WKViewRef, WKSize delta, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*, WKSize)>(0xc0))
        function(view->context, delta);
}

void underPageColorChanged(WKViewRef, double red, double green, double blue, double alpha, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*, double, double, double, double)>(0xc8))
        function(view->context, red, green, blue, alpha);
}

void inputMethodStateChanged(WKViewRef, bool enabled, WKRect caret, const void* info)
{
    View* view = viewOf(info);
    if (const auto function = view->client->at<void (*)(void*, int, double, double, double, double)>(0x68))
        function(view->context, enabled, caret.origin.x, caret.origin.y, caret.size.width, caret.size.height);
}

Error makeError(WKErrorRef error, ErrorText& text)
{
    text.description = takeString(WKErrorCopyLocalizedDescription(error));
    text.domain = takeString(WKErrorCopyDomain(error));
    text.url = takeUrl(WKErrorCopyFailingURL(error));
    return {WKErrorGetErrorCode(error), text.domain.c_str(), text.description.c_str(), text.url.c_str()};
}

void navigationEvent(View* view, int kind, WKErrorRef error)
{
    using Function = void (*)(void*, int, const Error*);
    if (!view->client->at<Function>(0x20))
        return;
    if (!error) {
        view->client->at<Function>(0x20)(view->context, kind, nullptr);
        return;
    }
    ErrorText text;
    const Error value = makeError(error, text);
    view->client->at<Function>(0x20)(view->context, kind, &value);
}

void changed(View* view, int property)
{
    if (const auto function = view->client->at<void (*)(void*, int)>(0x18))
        function(view->context, property);
}

void releaseBuffer(Buffer& buffer)
{
    if (buffer.bo)
        gbm_bo_destroy(buffer.bo);
    if (buffer.buffer.type == kWKViewBufferTypeDMABuf) {
        for (uint32_t i = 0; i < buffer.buffer.planeCount; i++)
            close(buffer.buffer.fds[i]);
    }
}

void bufferCreated(WKViewRef, const WKViewBuffer* buffer, const void* info)
{
    viewOf(info)->buffers[buffer->identifier].buffer = *buffer;
}

void bufferDestroyed(WKViewRef, uint64_t identifier, const void* info)
{
    View* view = viewOf(info);
    const auto found = view->buffers.find(identifier);
    if (found == view->buffers.end())
        return;
    releaseBuffer(found->second);
    view->buffers.erase(found);
    if (view->hasCurrent && view->current == identifier)
        view->hasCurrent = false;
    if (const auto function = view->client->at<void (*)(void*, uint64_t)>(0xd0))
        function(view->context, identifier);
}

void frameReady(WKViewRef ref, uint64_t identifier, const WKRect*, size_t, int fence, const void* info)
{
    View* view = viewOf(info);
    if (view->fence >= 0)
        close(view->fence);
    view->fence = fence;
    if (view->buffers.find(identifier) == view->buffers.end()) {
        WKViewReleaseBuffer(ref, identifier, -1);
        WKViewFrameDisplayed(ref);
        return;
    }
    if (view->hasCurrent && view->current != identifier)
        WKViewReleaseBuffer(ref, view->current, -1);
    view->current = identifier;
    view->hasCurrent = true;
    view->frame++;
    view->framePending = true;
    if (const auto function = view->client->at<void (*)(void*)>(0))
        function(view->context);
}

struct MenuItem
{
    const char* title;
    int type;
    int tag;
    int depth;
    int enabled;
    int checked;
    int isLabel;
};

static std::vector<MenuItem> menuItems(const WKViewMenuItem* items, size_t count)
{
    std::vector<MenuItem> list(count);
    for (size_t i = 0; i < count; i++)
        list[i] = {items[i].title, int(items[i].type), int(items[i].tag), int(items[i].depth), items[i].enabled, items[i].checked,
                   items[i].isLabel};
    return list;
}

void showPopupMenu(WKViewRef ref, const WKViewMenuItem* items, size_t count, int32_t selected, WKRect rect, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const MenuItem*, int, int, double, double, double, double);
    if (!view->client->at<Function>(0x80)) {
        WKViewPopupMenuItemSelected(ref, -1);
        return;
    }
    const std::vector<MenuItem> list = menuItems(items, count);
    view->client->at<Function>(0x80)(view->context, list.data(), int(count), selected, rect.origin.x, rect.origin.y,
        rect.size.width, rect.size.height);
}

void showContextMenu(WKViewRef ref, const WKViewMenuItem* items, size_t count, WKPoint point, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const MenuItem*, int, double, double);
    if (!view->client->at<Function>(0x90)) {
        WKViewContextMenuItemSelected(ref, -1);
        return;
    }
    const std::vector<MenuItem> list = menuItems(items, count);
    view->client->at<Function>(0x90)(view->context, list.data(), int(count), point.x, point.y);
}

struct Drag
{
    const char* text;
    const char* markup;
    const char* url;
    const char* urlTitle;
    const char* const* files;
    int fileCount;
    const void* image;
    int64_t imageSize;
};

void startDrag(WKViewRef, const WKViewDragData* data, uint32_t operations, const void* pixels, uint32_t width, uint32_t height,
               uint32_t stride, WKPoint hotspot, const void* info)
{
    View* view = viewOf(info);
    using Function = void (*)(void*, const Drag*, uint32_t, const void*, uint32_t, uint32_t, uint32_t, double, double);
    const auto function = view->client->at<Function>(0xf0);
    if (!function) {
        WKViewDragSourceEnded(view->view, hotspot, 0);
        return;
    }
    const Drag drag = {data->text, data->markup, data->url, data->urlTitle, data->fileURLs, int(data->fileURLCount),
                       data->image, int64_t(data->imageSize)};
    function(view->context, &drag, operations, pixels, width, height, stride, hotspot.x, hotspot.y);
}

static void copyPixels(View* view, const unsigned char* from, uint32_t width, uint32_t height, uint32_t stride, uint32_t format)
{
    const bool alpha = format == GBM_FORMAT_ARGB8888 || format == GBM_FORMAT_ABGR8888;
    const bool bgra = format == GBM_FORMAT_ARGB8888 || format == GBM_FORMAT_XRGB8888;
    view->width = width;
    view->height = height;
    view->pixels.resize(size_t(width) * height * 4);
    for (uint32_t y = 0; y < height; y++) {
        const unsigned char* source = from + size_t(y) * stride;
        unsigned char* target = view->pixels.data() + size_t(y) * width * 4;
        if (bgra && alpha) {
            memcpy(target, source, width * 4);
            continue;
        }
        for (uint32_t x = 0; x < width; x++) {
            target[x * 4] = source[x * 4 + (bgra ? 0 : 2)];
            target[x * 4 + 1] = source[x * 4 + 1];
            target[x * 4 + 2] = source[x * 4 + (bgra ? 2 : 0)];
            target[x * 4 + 3] = alpha ? source[x * 4 + 3] : 255;
        }
    }
}

bool readPixels(View* view, Buffer& buffer)
{
    if (view->fence >= 0) {
        pollfd wait = {view->fence, POLLIN, 0};
        poll(&wait, 1, 1000);
        close(view->fence);
        view->fence = -1;
    }
    const WKViewBuffer& from = buffer.buffer;
    if (from.type == kWKViewBufferTypeSharedMemory) {
        copyPixels(view, static_cast<const unsigned char*>(from.data), from.width, from.height, from.strides[0], from.fourcc);
        return true;
    }
    if (!g_device)
        return false;
    if (!buffer.bo) {
        gbm_import_fd_modifier_data data = {
            .width = from.width, .height = from.height, .format = from.fourcc, .num_fds = from.planeCount, .modifier = from.modifier,
        };
        for (uint32_t i = 0; i < from.planeCount; i++) {
            data.fds[i] = from.fds[i];
            data.strides[i] = int(from.strides[i]);
            data.offsets[i] = int(from.offsets[i]);
        }
        buffer.bo = gbm_bo_import(g_device, GBM_BO_IMPORT_FD_MODIFIER, &data, 0);
        if (!buffer.bo)
            return false;
    }
    uint32_t stride = 0;
    void* mapping = nullptr;
    void* bytes = gbm_bo_map(buffer.bo, 0, 0, from.width, from.height, GBM_BO_TRANSFER_READ, &stride, &mapping);
    if (!bytes)
        return false;
    copyPixels(view, static_cast<const unsigned char*>(bytes), from.width, from.height, stride, from.fourcc);
    gbm_bo_unmap(buffer.bo, mapping);
    return true;
}

} // namespace cutefish::reef::web
