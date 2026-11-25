// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the view itself asks of the program.

#include "web.h"

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

void changed(View* view, int property)
{
    if (const auto function = view->client->at<void (*)(void*, int)>(0x18))
        function(view->context, property);
}

Buffer::~Buffer()
{
    if (bo)
        gbm_bo_destroy(bo);
    if (!shared) {
        for (uint32_t i = 0; i < planes; i++)
            close(fds[i]);
    }
}

} // namespace cutefish::reef::web
