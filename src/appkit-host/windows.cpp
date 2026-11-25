// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Windows, and what their root view passes to the program.

#include "views.h"

#include "ui/frame_client.h"
#include "ui/image.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

// Calls the program before each frame of a window.
struct reef_ui_frame_clock : cutefish::ui::FrameClient
{
    reef_ui_frame_clock(cutefish::ui::Window* window, void (*callback)(void*), void* context)
        : FrameClient(window)
        , m_callback(callback)
        , m_context(context)
    {
    }

    using FrameClient::startTicking;
    using FrameClient::stopTicking;

protected:
    void tick(uint64_t) override { m_callback(m_context); }
    void interrupt(Interruption) override {}

private:
    void (*m_callback)(void*);
    void* m_context;
};

namespace cutefish::reef {

using namespace std::chrono_literals;

// 10
// flags: 1 a popup of the parent, 2 transparent, 4 without a grab, 8 a status item.
reef_ui_window* hostWindowCreate(void* context, double width, double height, int flags, reef_ui_window* parent, double x, double y)
{
    if ((flags & 8) && !hostStatusItemsSupported())
        return nullptr;
    auto window = new reef_ui_window;
    window->context = context;
    if (flags & 8) {
        window->statusItem = std::make_unique<ui::StatusItem>();
        window->window = &window->statusItem->window();
        auto changed = [window] { sendWindowEvent(window, 6); };
        window->statusItem->onAppearanceChanged = changed;
        window->statusItem->onHighlightChanged = changed;
        window->statusItem->onScreenRectChanged = changed;
        window->statusItem->onConfigured = changed;
        window->statusItem->setWidth(width);
    } else {
        if (parent && (flags & 1)) {
            ui::PopupPlacement placement;
            placement.anchorRect = {x, y, 1, 1};
            placement.anchor = ui::PopupAnchor::TopLeft;
            placement.gravity = ui::PopupAnchor::BottomRight;
            window->owned = std::make_unique<ui::Window>(parent->window, placement, !(flags & 4));
        } else {
            window->owned = std::make_unique<ui::Window>();
        }
        window->window = window->owned.get();
        window->window->resize({width, height});
    }
    window->transparent = flags & 2;
    if (window->transparent)
        window->window->setBackgroundColor(ui::ColorRecipe(ui::Color{0, 0, 0, 0}));
    auto root = std::make_unique<RootView>(window);
    window->base = root->addChild<BaseView>(window);
    if (!(flags & (2 | 8))) {
        window->base->setLayered(true);
        window->base->setOpaqueLayer(true);
        window->base->setKeepsLayerOnResize(true);
        window->opaque = true;
    }
    window->root = root.get();
    window->window->setContentView(std::move(root));
    window->window->setPrepareFrameHandler([window] {
        if (callbacks.frame)
            callbacks.frame(window->context);
    });
    window->window->onCloseRequested = [window] { sendWindowEvent(window, 1); };
    window->window->onActiveChanged = [window] {
        if (callbacks.window)
            callbacks.window(window->context, 2, window->window->isActive(), 0);
    };
    window->window->onStatesChanged = [window] {
        const ui::WindowStates& states = window->window->states();
        sendWindowEvent(window, 4, states.maximized, states.fullScreen);
    };
    window->window->onInteractiveResizeChanged = [window] { sendWindowEvent(window, 9, window->window->isInteractivelyResized()); };
    window->window->setFullScreenRevealHandler([window](int offset, int duration) { sendWindowEvent(window, 8, offset, duration); });
    window->window->onScreenPositionChanged = [window] {
        const ui::PointF origin = screensOrigin();
        window->positioned = true;
        window->position = *window->window->screenPosition() - origin;
        sendWindowEvent(window, 7, window->position.x, window->position.y);
    };
    return window;
}

ui::Window* windowOf(reef_ui_window* window)
{
    return window ? window->window : nullptr;
}

// 13
void hostWindowResize(reef_ui_window* window, double width, double height)
{
    if (window->statusItem)
        window->statusItem->setWidth(width);
    else
        window->window->resize({width, height});
}

// 14
void hostWindowSetMinimumSize(reef_ui_window* window, double width, double height)
{
    window->window->setMinimumSize({width, height});
}

// 15
void hostWindowSetMaximumSize(reef_ui_window* window, double width, double height)
{
    window->window->setMaximumSize({width, height});
}

// 193
void hostWindowHide(reef_ui_window* window)
{
    window->window->hide();
}

// 194
void hostWindowSetCastsShadow(reef_ui_window* window, int castsShadow)
{
    window->window->setCastsShadow(castsShadow);
}

static bool isToplevel(const reef_ui_window* window)
{
    const bool popup = window->window->isPopup();
    const bool statusItem = window->statusItem != nullptr;
    return !popup && !statusItem;
}

// 16
void hostWindowShow(reef_ui_window* window)
{
    const int placement = window->placement;
    if (placement && window->positioned) {
        const ui::PointF position = window->position + screensOrigin();
        ui::PlacementAnchor anchor;
        anchor.rect = {position.x, position.y, 0, 0};
        anchor.anchor = placement == 2 ? ui::PopupAnchor::Bottom : ui::PopupAnchor::TopLeft;
        anchor.gravity = placement == 2 ? ui::PopupAnchor::Bottom : ui::PopupAnchor::BottomRight;
        window->window->setPlacementAnchor(anchor);
    } else {
        window->window->setPlacementAnchor(std::nullopt);
    }
    if (isToplevel(window))
        window->window->setSkipTaskbar(window->placement != 0);
    window->window->show();
    window->root->setFocus(ui::FocusReason::Other);
}

// 273
void hostWindowRequestLayout(reef_ui_window* window)
{
    window->window->requestLayout();
}

// 18
void hostWindowSize(reef_ui_window* window, double* width, double* height)
{
    const ui::SizeF size = window->window->size();
    *width = size.width;
    *height = size.height;
}

// 299
double hostWindowOutputScale(reef_ui_window* window)
{
    return window->window->scale();
}

// 19
double hostWindowScale(reef_ui_window* window)
{
    return renderScale(window->window->scale());
}

// 20
int hostWindowIsActive(reef_ui_window* window)
{
    return window->window->isActive();
}

// 21
void hostWindowActivate(reef_ui_window* window)
{
    window->window->activate();
}

// 22
void hostWindowStartMove(reef_ui_window* window)
{
    if (window->root)
        window->root->pressed = false;
    window->window->startMove();
}

// 23
void hostWindowMinimize(reef_ui_window* window)
{
    window->window->minimize();
}

// 24
void hostWindowHideApplication(reef_ui_window* window)
{
    window->window->hideApplication();
}

// 25
void hostWindowSetMaximized(reef_ui_window* window, int maximized)
{
    window->window->setMaximized(maximized);
}

static void moveToPosition(reef_ui_window* window)
{
    if (window->placement == 0 && isToplevel(window)) {
        const ui::PointF origin = screensOrigin();
        window->window->setScreenPosition(window->position + origin);
    }
}

// 151
void hostWindowSetPosition(reef_ui_window* window, double x, double y)
{
    window->positioned = true;
    window->position = {x, y};
    moveToPosition(window);
}

// 183
void hostWindowSetPlacement(reef_ui_window* window, int placement)
{
    if (window->placement == placement)
        return;
    window->placement = placement;
    if (window->positioned)
        moveToPosition(window);
}

// 205
void hostWindowSetAlwaysOnTop(reef_ui_window* window, int onTop)
{
    window->window->setAlwaysOnTop(onTop);
}

// 302
void hostWindowSetLevel(reef_ui_window* window, int level)
{
    window->window->setLevel(level == 2 ? ui::WindowLevel::AboveDock
                             : level == 1 ? ui::WindowLevel::Floating
                                          : ui::WindowLevel::Normal);
}

// 296
void hostWindowSetTransientParent(reef_ui_window* window, reef_ui_window* parent)
{
    window->window->setTransientParent(parent ? parent->window : nullptr, parent != nullptr);
}

// 207
void hostWindowSetFullScreen(reef_ui_window* window, int fullScreen)
{
    window->window->setFullScreen(fullScreen);
}

// 252
void hostWindowSetFullScreenBars(reef_ui_window* window, double height, int push)
{
    window->window->setFullScreenBars(int(std::ceil(height)), push);
}

// 208
void hostWindowSetContext(reef_ui_window* window, void* context)
{
    window->context = context;
}

// 206
void hostWindowSetOpacity(reef_ui_window* window, double opacity)
{
    window->window->setOpacity(opacity);
}

// 313
void hostWindowSetInputTransparent(reef_ui_window* window, int transparent)
{
    window->window->setInputTransparent(transparent);
}

// 184
int hostWindowPlacement(reef_ui_window* window)
{
    return window->placement;
}

// 152
int hostWindowPosition(reef_ui_window* window, double* x, double* y)
{
    *x = window->position.x;
    *y = window->position.y;
    return window->positioned;
}

// 309
void hostWindowHoldPresentation(reef_ui_window* window, int hold)
{
    if (!window || !window->window)
        return;
    if (hold)
        window->window->holdPresentation();
    else
        window->window->releasePresentation();
}

// 179
reef_ui_frame_clock* hostFrameClockCreate(reef_ui_window* window, void (*callback)(void*), void* context)
{
    if (!window || !window->window || !callback)
        return nullptr;
    return new reef_ui_frame_clock(window->window, callback, context);
}

// 180
void hostFrameClockStart(reef_ui_frame_clock* clock)
{
    if (clock)
        clock->startTicking();
}

// 181
void hostFrameClockStop(reef_ui_frame_clock* clock)
{
    if (clock)
        clock->stopTicking();
}

// 182
void hostFrameClockDestroy(reef_ui_frame_clock* clock)
{
    delete clock;
}

// 189
void hostWindowSetUserResizable(reef_ui_window* window, int resizable)
{
    window->window->setUserResizable(resizable);
}

// 119
// RGBA pixels to free.
void* hostWindowGrab(reef_ui_window* window, int* width, int* height)
{
    const ui::Image image = window->window->grab();
    if (image.isNull())
        return nullptr;
    const size_t size = size_t(image.width()) * image.height() * 4;
    void* pixels = malloc(size);
    if (!pixels)
        return nullptr;
    memcpy(pixels, image.bits(), size);
    *width = image.width();
    *height = image.height();
    return pixels;
}

// 235
void hostWindowFrameStatistics(reef_ui_window* window, uint64_t* rendered, uint64_t* presented, uint64_t* discarded)
{
    const ui::FrameStatistics& statistics = window->window->frameStatistics();
    *rendered = statistics.rendered;
    *presented = statistics.presented;
    *discarded = statistics.discarded;
}

// 11
void hostWindowDestroy(reef_ui_window* window)
{
    for (reef_ui_overlay* overlay : window->overlays) {
        overlay->window = nullptr;
        overlay->fade.reset();
    }
    forgetWindow(window);
    delete window;
}

// 12
void hostWindowSetTitle(reef_ui_window* window, const char* title)
{
    window->window->setTitle(title ? title : "");
}

// 214
void hostWindowActivateWithToken(reef_ui_window* window, const char* token)
{
    if (token && *token)
        window->window->activate(token);
    else
        window->window->activate();
}

// 98
void hostWindowSetToolTip(reef_ui_window* window, double x, double y, double width, double height, const char* text)
{
    if (text && *text)
        window->window->pinToolTip(window->root, {x, y, width, height}, text);
    else
        window->window->unpinToolTip(window->root);
}

// 28
int hostWindowSavePng(reef_ui_window* window, const char* path)
{
    const ui::Image image = window->window->grab();
    if (image.isNull())
        return 1;
    return !image.savePng(path);
}

} // namespace cutefish::reef
