// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Scrolling, pointers and text input, and input that tests make.

#include "input.h"

#include "views.h"

#include "ui/scroller.h"
#include "ui/test_input.h"

struct reef_ui_scroller : cutefish::ui::Scroller::Target
{
    explicit reef_ui_scroller(const reef_ui_scroller_callbacks& callbacks)
        : m_callbacks(callbacks)
    {
    }

    cutefish::ui::Scroller::Axis horizontalAxis() const override
    {
        reef_ui_scroll_axis horizontal = {}, vertical = {};
        m_callbacks.axes(m_callbacks.context, &horizontal, &vertical);
        return {horizontal.minimum, horizontal.maximum, horizontal.viewport, horizontal.enabled != 0};
    }

    cutefish::ui::Scroller::Axis verticalAxis() const override
    {
        reef_ui_scroll_axis horizontal = {}, vertical = {};
        m_callbacks.axes(m_callbacks.context, &horizontal, &vertical);
        return {vertical.minimum, vertical.maximum, vertical.viewport, vertical.enabled != 0};
    }

    cutefish::ui::PointF position() const override
    {
        double x = 0, y = 0;
        m_callbacks.position(m_callbacks.context, &x, &y);
        return {x, y};
    }

    void applyPosition(cutefish::ui::PointF position) override
    {
        m_callbacks.applyPosition(m_callbacks.context, position.x, position.y);
    }

    void movingChanged(bool moving) override { m_callbacks.movingChanged(m_callbacks.context, moving); }

    reef_ui_scroller_callbacks m_callbacks;
    std::unique_ptr<cutefish::ui::Scroller> scroller;
};

namespace cutefish::reef {

// 195
reef_ui_scroller* hostScrollerCreate(reef_ui_window* window, const reef_ui_scroller_callbacks* callbacks)
{
    if (!window || !window->window || !callbacks)
        return nullptr;
    auto scroller = new reef_ui_scroller(*callbacks);
    scroller->scroller = std::make_unique<ui::Scroller>(window->window, scroller);
    return scroller;
}

// 196
void hostScrollerDestroy(reef_ui_scroller* scroller)
{
    delete scroller;
}

// 197
int hostScrollerScroll(reef_ui_scroller* scroller, double deltaX, double deltaY, int value120X, int value120Y, int precise, int stop,
                       unsigned time, unsigned modifiers)
{
    ui::ScrollEvent event;
    event.deltaX = deltaX;
    event.deltaY = deltaY;
    event.value120X = value120X;
    event.value120Y = value120Y;
    event.precise = precise;
    event.stop = stop;
    event.time = time;
    event.modifiers = modifiers;
    return scroller->scroller->handleScroll(event);
}

// 198
void hostScrollerSetElastic(reef_ui_scroller* scroller, int elastic)
{
    scroller->scroller->setElastic(elastic);
}

// 199
void hostScrollerViewMoved(reef_ui_scroller* scroller)
{
    scroller->scroller->viewMoved();
}

// 200
int hostScrollerIsActive(reef_ui_scroller* scroller)
{
    return scroller->scroller->isActive();
}

static ui::CursorShape cursorShapeOf(int cursor)
{
    switch (cursor) {
    case 1:
        return ui::CursorShape::Pointer;
    case 2:
        return ui::CursorShape::Text;
    case 3:
        return ui::CursorShape::Move;
    case 4:
        return ui::CursorShape::Grab;
    case 5:
        return ui::CursorShape::Grabbing;
    case 6:
        return ui::CursorShape::NotAllowed;
    case 7:
        return ui::CursorShape::ResizeHorizontal;
    case 8:
        return ui::CursorShape::ResizeVertical;
    case 9:
        return ui::CursorShape::Blank;
    case 10:
        return ui::CursorShape::Crosshair;
    case 11:
        return ui::CursorShape::Copy;
    case 12:
        return ui::CursorShape::Alias;
    case 13:
        return ui::CursorShape::ContextMenu;
    case 14:
        return ui::CursorShape::VerticalText;
    case 15:
        return ui::CursorShape::Help;
    case 16:
        return ui::CursorShape::Progress;
    case 17:
        return ui::CursorShape::Wait;
    case 18:
        return ui::CursorShape::Cell;
    case 19:
        return ui::CursorShape::NoDrop;
    case 20:
        return ui::CursorShape::AllScroll;
    case 21:
        return ui::CursorShape::ColumnResize;
    case 22:
        return ui::CursorShape::RowResize;
    case 23:
        return ui::CursorShape::ResizeNorth;
    case 24:
        return ui::CursorShape::ResizeEast;
    case 25:
        return ui::CursorShape::ResizeSouth;
    case 26:
        return ui::CursorShape::ResizeWest;
    case 27:
        return ui::CursorShape::ResizeNorthEast;
    case 28:
        return ui::CursorShape::ResizeNorthWest;
    case 29:
        return ui::CursorShape::ResizeSouthEast;
    case 30:
        return ui::CursorShape::ResizeSouthWest;
    case 31:
        return ui::CursorShape::ResizeNorthEastSouthWest;
    case 32:
        return ui::CursorShape::ResizeNorthWestSouthEast;
    case 33:
        return ui::CursorShape::ZoomIn;
    case 34:
        return ui::CursorShape::ZoomOut;
    case 35:
        return ui::CursorShape::Image;
    default:
        return ui::CursorShape::Default;
    }
}

// 26
void hostWindowSetCursor(reef_ui_window* window, int cursor)
{
    if (window->cursor != cursor) {
        window->cursor = cursor;
        window->cursorChanges++;
    }
    RootView* root = window->root;
    const ui::CursorShape shape = cursorShapeOf(cursor);
    if (root->shape != shape) {
        root->shape = shape;
        root->setCursorShape(shape);
    }
}

// 255
// For tests.
int hostWindowCursor(reef_ui_window* window, int* changes)
{
    if (changes)
        *changes = window->cursorChanges;
    return window->cursor;
}

// 303
int hostCanSendKeys()
{
    return application && application->canSendKeys();
}

// 304
int hostSendKey(unsigned key, int pressed)
{
    return application && application->sendKey(key, pressed);
}

// 27
// Where the program draws the insertion point.
void hostWindowSetTextInput(reef_ui_window* window, int accepts, double x, double y, double width, double height)
{
    RootView* root = window->root;
    root->acceptsText = accepts;
    root->textCursor = {x, y, width, height};
    root->updateTextInput();
}

// 289
int hostWindowAcceptsTextInput(reef_ui_window* window)
{
    return window->root->acceptsText;
}

static ui::MouseButton mouseButtonOf(int button)
{
    return button == 1 ? ui::MouseButton::Right : button == 2 ? ui::MouseButton::Middle : ui::MouseButton::Left;
}

// 29
// type: 0 pressed, 1 released, 2 moved, 4 left.
void hostTestPointer(reef_ui_window* window, int type, double x, double y, int button)
{
    ui::Window& target = *window->window;
    ui::TestInput::presentFrame(target);
    switch (type) {
    case 0:
        ui::TestInput::press(target, {x, y}, mouseButtonOf(button));
        break;
    case 1:
        ui::TestInput::release(target, {x, y}, mouseButtonOf(button));
        break;
    case 2:
        ui::TestInput::movePointer(target, {x, y});
        break;
    case 4:
        ui::TestInput::leavePointer(target);
        break;
    }
}

// 32
void hostTestScale(reef_ui_window* window, double scale)
{
    window->window->setScale(scale);
    window->root->updateScale();
}

// 290
// BGRA pixels.
void hostWindowSetCursorImage(reef_ui_window* window, const void* pixels, int width, int height, int scale, double hotSpotX, double hotSpotY)
{
    window->window->setCursorImage(
        ui::Bitmap::copyOf(pixels, width, height, size_t(width) * 4, ui::Bitmap::Format::Argb32Premultiplied), scale, {hotSpotX, hotSpotY});
}

// 30
void hostTestKey(reef_ui_window* window, unsigned key, unsigned modifiers)
{
    ui::TestInput::typeKey(*window->window, key, modifiers);
}

// 31
void hostTestText(reef_ui_window* window, const char* text)
{
    ui::TestInput::typeText(*window->window, text);
}

} // namespace cutefish::reef
