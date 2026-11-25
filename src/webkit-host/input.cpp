// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "web.h"

#include <algorithm>
#include <WebKit/WKEvent.h>
#include <WebKit/WKEventCutefish.h>

namespace cutefish::reef::web {

const Pasteboard* g_pasteboard;

struct MouseEvent
{
    int kind;
    int button;
    uint32_t buttons;
    int clickCount;
    uint32_t modifiers;
    double x;
    double y;
    double time;
};

static WKEventMouseButton toButton(int button)
{
    switch (button) {
    case 1:
        return kWKEventMouseButtonLeftButton;
    case 2:
        return kWKEventMouseButtonMiddleButton;
    case 3:
        return kWKEventMouseButtonRightButton;
    default:
        return kWKEventMouseButtonNoButton;
    }
}

static WKEventMouseButtons toButtons(uint32_t buttons)
{
    WKEventMouseButtons result = 0;
    if (buttons & 1)
        result |= kWKEventMouseButtonsLeft;
    if (buttons & 2)
        result |= kWKEventMouseButtonsMiddle;
    if (buttons & 4)
        result |= kWKEventMouseButtonsRight;
    return result;
}

// 41
void sendMouse(View* view, const MouseEvent* event)
{
    WKMouseEvent mouse = {};
    switch (event->kind) {
    case 1:
        mouse.type = kWKEventMouseDown;
        break;
    case 2:
        mouse.type = kWKEventMouseUp;
        break;
    default:
        mouse.type = kWKEventMouseMove;
        break;
    }
    mouse.button = toButton(event->button);
    mouse.buttons = toButtons(event->buttons);
    mouse.position = {event->x, event->y};
    mouse.globalPosition = mouse.position;
    mouse.clickCount = event->clickCount;
    mouse.modifiers = event->modifiers & 0x1f;
    mouse.timestamp = event->time > 0 ? event->time : now();
    WKViewSendMouseEvent(view->view, &mouse);
}

struct WheelEvent
{
    double x;
    double y;
    double deltaX;
    double deltaY;
    double ticksX;
    double ticksY;
    int phase;
    int momentumPhase;
    int precise;
    uint32_t modifiers;
    double time;
};

static WKWheelEventPhase toPhase(int phase)
{
    switch (phase) {
    case 1:
        return kWKWheelEventPhaseBegan;
    case 2:
        return kWKWheelEventPhaseChanged;
    case 3:
        return kWKWheelEventPhaseEnded;
    case 4:
        return kWKWheelEventPhaseCancelled;
    default:
        return kWKWheelEventPhaseNone;
    }
}

// 42
void sendWheel(View* view, const WheelEvent* event)
{
    WKWheelEvent wheel = {};
    wheel.position = {event->x, event->y};
    wheel.globalPosition = wheel.position;
    wheel.delta = {event->deltaX, event->deltaY};
    wheel.wheelTicks = {event->ticksX, event->ticksY};
    wheel.phase = toPhase(event->phase);
    wheel.momentumPhase = toPhase(event->momentumPhase);
    wheel.hasPreciseDeltas = event->precise != 0;
    wheel.modifiers = event->modifiers & 0x1f;
    wheel.timestamp = event->time > 0 ? event->time : now();
    WKViewSendWheelEvent(view->view, &wheel);
}

struct KeyEvent
{
    int down;
    uint32_t keysym;
    uint32_t keycode;
    uint32_t modifiers;
    int repeat;
    const char* text;
    double time;
};

// 43
void sendKey(View* view, const KeyEvent* event)
{
    WKKeyboardEvent key = {};
    key.type = event->down ? kWKEventKeyDown : kWKEventKeyUp;
    key.keysym = event->keysym;
    key.keycode = event->keycode;
    key.text = event->text && *event->text ? event->text : nullptr;
    key.isAutoRepeat = event->repeat != 0;
    key.modifiers = event->modifiers & 0x1f;
    key.timestamp = event->time > 0 ? event->time : now();
    WKViewSendKeyboardEvent(view->view, &key);
}

// 44
void setComposition(View* view, const char* text, int cursor)
{
    WKViewSetComposition(view->view, text ? text : "", uint32_t(std::max(cursor, 0)));
}

// 45
void confirmComposition(View* view, const char* text) { WKViewConfirmComposition(view->view, text ? text : ""); }
// 46
void cancelComposition(View* view) { WKViewCancelComposition(view->view); }
// 47
void popupChosen(View* view, int index) { WKViewPopupMenuItemSelected(view->view, index); }
// 48
void menuChosen(View* view, int index) { WKViewContextMenuItemSelected(view->view, index); }

} // namespace cutefish::reef::web
