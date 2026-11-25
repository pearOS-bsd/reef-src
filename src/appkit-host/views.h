// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The views of a window: the root takes the input, the base draws the window, planes draw above it.

#pragma once

#include "fade.h"
#include "graphics.h"
#include "host.h"

#include "ui/drag.h"
#include "ui/event.h"
#include "ui/painter.h"
#include "ui/theme.h"

#include <cfloat>

namespace cutefish::reef {

// gl.cpp
// The program's GPU work waits while a view is drawn.
void beginDrawing();
void endDrawing();

// views.cpp
unsigned modifiersOf(uint32_t modifiers);
int buttonOf(ui::MouseButton button);

inline void sendWindowEvent(reef_ui_window* window, int event, double first = 0, double second = 0)
{
    if (callbacks.window)
        callbacks.window(window->context, event, first, second);
}

// A view that the program draws.
class PaintedView : public ui::View
{
protected:
    // What is to be drawn, in few rectangles.
    std::vector<ui::RectF> damage(ui::Painter& painter) const
    {
        const ui::SizeF size = geometry().size();
        std::vector<ui::RectF> clip = painter.clipRects();
        // Those that waste the least become one.
        while (clip.size() > 8) {
            size_t first = 0, second = 1;
            double least = DBL_MAX;
            for (size_t i = 0; i + 1 < clip.size(); i++) {
                for (size_t j = i + 1; j < clip.size(); j++) {
                    const ui::RectF both = united(clip[i], clip[j]);
                    const double waste = both.width * both.height - clip[i].width * clip[i].height - clip[j].width * clip[j].height;
                    if (waste < least) {
                        least = waste;
                        first = i;
                        second = j;
                    }
                }
            }
            clip[first] = united(clip[first], clip[second]);
            clip.erase(clip.begin() + second);
        }
        std::vector<ui::RectF> rects;
        for (const ui::RectF& rect : clip) {
            const double left = std::max(0.0, rect.x);
            const double top = std::max(0.0, rect.y);
            const double right = std::min(size.width, rect.right());
            const double bottom = std::min(size.height, rect.bottom());
            if (right > left && bottom > top)
                rects.push_back({left, top, right - left, bottom - top});
        }
        return rects;
    }
};

class BaseView : public PaintedView
{
public:
    explicit BaseView(reef_ui_window* window)
        : m_window(window)
    {
    }

protected:
    void paint(ui::Painter& painter) override;

private:
    reef_ui_window* m_window;
};

class PlaneView : public PaintedView
{
public:
    explicit PlaneView(reef_ui_plane* plane)
        : m_plane(plane)
    {
        setLayered(true);
        setKeepsLayerOnResize(true);
    }

    // In the clip.
    ui::RectF rect;

protected:
    void paint(ui::Painter& painter) override;

private:
    reef_ui_plane* m_plane;
};

class RootView : public ui::View
{
public:
    explicit RootView(reef_ui_window* window)
        : m_window(window)
        , scale(renderScale(window->window->scale()))
    {
        setTracksHover(true);
    }

    void updateScale()
    {
        const double current = renderScale(m_window->window->scale());
        if (current == scale)
            return;
        scale = current;
        sendWindowEvent(m_window, 3, scale);
    }

    bool acceptsFocus() const override { return true; }
    ui::CursorShape cursorShape() const override { return shape; }
    bool acceptsPointer() const override { return true; }

protected:
    void layout() override
    {
        updateScale();
        const ui::SizeF size = geometry().size();
        if (m_window->base)
            m_window->base->setGeometry({0, 0, size.width, size.height});
        if (size == m_size)
            return;
        // While it is resized, a transparent window keeps its picture too.
        if (!m_window->opaque && m_window->base && m_size.width > 0 && m_size.height > 0) {
            if (!m_window->base->isLayered()) {
                m_window->base->setLayered(true);
                m_window->base->setKeepsLayerOnResize(true);
            }
            m_resizeTimer.onTimeout = [window = m_window] {
                if (window->base && !window->opaque)
                    window->base->setLayered(false);
            };
            m_resizeTimer.setSingleShot(true);
            m_resizeTimer.start(std::chrono::milliseconds(400));
        }
        m_size = size;
        if (m_window->blurred)
            m_window->window->setBackgroundBlur({0, 0, size.width, size.height}, ui::Theme::current().windowRadius);
        if (callbacks.window)
            callbacks.window(m_window->context, 0, size.width, size.height);
    }

    void pointerPress(const ui::PointerEvent& event) override
    {
        setFocus(ui::FocusReason::Mouse);
        pressed = true;
        send(0, event);
    }

    void pointerMove(const ui::PointerEvent& event) override { send(2, event); }

    void pointerRelease(const ui::PointerEvent& event) override
    {
        pressed = false;
        send(1, event);
    }

    void hoverChanged() override
    {
        if (callbacks.pointer)
            callbacks.pointer(m_window->context, 0, 0, isHovered() ? 3 : 4, 0, 0, 0);
    }

    void hoverMove(const ui::PointerEvent& event) override
    {
        if (!pressed)
            send(2, event);
    }

    void focusIn(ui::FocusReason) override { sendWindowEvent(m_window, 5, 1); }
    void focusOut() override { sendWindowEvent(m_window, 5, 0); }
    bool keyPress(const ui::KeyEvent& event) override { return send(1, event); }
    bool keyRelease(const ui::KeyEvent& event) override { return send(0, event); }

    bool scroll(const ui::ScrollEvent& event) override
    {
        if (callbacks.scroll)
            callbacks.scroll(m_window->context, event.deltaX, event.deltaY, event.precise, event.stop, modifiersOf(event.modifiers),
                             event.value120X, event.value120Y, event.time);
        return true;
    }

    bool acceptsTextInput() const override { return acceptsText; }

    ui::TextInputState textInputState() const override
    {
        ui::TextInputState state;
        state.cursorRectangle = textCursor;
        return state;
    }

    void textInput(const ui::TextInputEvent& event) override
    {
        if (callbacks.text)
            callbacks.text(m_window->context, event.commit.c_str(), event.preedit.c_str(), event.preeditCursorBegin);
    }

    bool canDrop(const ui::DropOffer&) const override { return callbacks.drag; }

    void dragEnter(const ui::DragEvent& event) override
    {
        offer = event.offer;
        send(0, event);
    }

    uint32_t dragUpdate(const ui::DragEvent& event) override
    {
        offer = event.offer;
        return send(1, event);
    }

    void dragLeave() override
    {
        if (callbacks.drag)
            callbacks.drag(m_window->context, 2, 0, 0, offer, 0, 0);
        offer = nullptr;
    }

    uint32_t drop(const ui::DragEvent& event) override
    {
        const unsigned action = send(3, event);
        offer = nullptr;
        return action;
    }

    bool gesture(const ui::GestureEvent& event) override
    {
        int type;
        switch (event.type) {
        case ui::GestureEvent::Type::SwipeBegin:
            type = 0;
            break;
        case ui::GestureEvent::Type::SwipeUpdate:
            type = 1;
            break;
        case ui::GestureEvent::Type::SwipeEnd:
            type = 2;
            break;
        case ui::GestureEvent::Type::PinchBegin:
            type = 3;
            break;
        case ui::GestureEvent::Type::PinchUpdate:
            type = 4;
            break;
        case ui::GestureEvent::Type::PinchEnd:
            type = 5;
            break;
        default:
            return false;
        }
        if (!callbacks.gesture)
            return false;
        callbacks.gesture(m_window->context, type, event.fingers, event.delta.x, event.delta.y, event.scale, event.rotation,
                          event.cancelled, modifiersOf(event.modifiers), event.time);
        return true;
    }

private:
    void send(int type, const ui::PointerEvent& event)
    {
        if (callbacks.pointer)
            callbacks.pointer(m_window->context, event.position.x, event.position.y, type, buttonOf(event.button), event.clickCount,
                              modifiersOf(event.modifiers));
    }

    bool send(int pressed, const ui::KeyEvent& event)
    {
        if (!callbacks.key)
            return false;
        return callbacks.key(m_window->context, pressed, event.key, event.keycode, event.text.c_str(), modifiersOf(event.modifiers),
                             event.repeat);
    }

    unsigned send(int type, const ui::DragEvent& event)
    {
        if (!callbacks.drag)
            return 0;
        return callbacks.drag(m_window->context, type, event.position.x, event.position.y, event.offer,
                              event.offer ? event.offer->sourceActions() : 0, modifiersOf(event.modifiers));
    }

    reef_ui_window* m_window;
    ui::SizeF m_size;
    // Runs out when the window was not resized for a while.
    base::Timer m_resizeTimer;

public:
    double scale;
    ui::CursorShape shape = ui::CursorShape::Default;
    bool pressed = false;
    bool acceptsText = false;
    ui::RectF textCursor;
    // While something is dragged over the window.
    ui::DropOffer* offer = nullptr;
};

} // namespace cutefish::reef

struct reef_ui_plane
{
    reef_ui_window* window = nullptr;
    void* context = nullptr;
    // Clips what is in it.
    cutefish::ui::View* clip = nullptr;
    cutefish::reef::PlaneView* view = nullptr;
};

struct reef_ui_overlay
{
    reef_ui_window* window;
    cutefish::ui::RectF rect;
    int width;
    int height;
    // What is below it was drawn anew.
    bool stale = false;
    std::unique_ptr<cutefish::reef::FadeOverlay> fade;
};

namespace cutefish::reef {

inline void PlaneView::paint(ui::Painter& painter)
{
    if (!callbacks.drawPlane)
        return;
    const std::vector<ui::RectF> rects = damage(painter);
    if (rects.empty())
        return;
    std::vector<double> numbers;
    for (const ui::RectF& rect : rects)
        numbers.insert(numbers.end(), {rect.x, rect.y, rect.width, rect.height});
    beginDrawing();
    reef_ui_gc gc;
    gc.canvas = std::make_unique<ui::Canvas2D>(painter);
    callbacks.drawPlane(m_plane->window->context, m_plane->context, &gc, numbers.data(), unsigned(rects.size()));
    endDrawing();
}

} // namespace cutefish::reef
