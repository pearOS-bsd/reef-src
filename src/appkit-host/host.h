// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the files of the host of AppKit share. The programs reach it through the table of table.cpp; the numbers in the
// comments are the indices there.

#pragma once

#include "base/dbus.h"
#include "base/event_loop.h"
#include "base/timer.h"
#include "ui/application.h"
#include "ui/geometry.h"
#include "ui/status_item.h"
#include "ui/view.h"
#include "ui/window.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct reef_ui_gc;
struct reef_ui_overlay;
struct reef_ui_plane;

// What the program gives to the host. `window` is the program's own object, given to hostWindowCreate.
struct reef_ui_callbacks
{
    void (*draw)(void* window, reef_ui_gc* gc, double x, double y, double width, double height);
    // 0 pressed, 1 released, 2 moved, 3 entered, 4 left.
    void (*pointer)(void* window, double x, double y, int type, int button, int clicks, unsigned modifiers);
    void (*scroll)(void* window, double deltaX, double deltaY, int precise, int stop, unsigned modifiers, int value120X, int value120Y,
                   unsigned time);
    int (*key)(void* window, int pressed, uint32_t key, uint32_t keycode, const char* text, unsigned modifiers, int repeat);
    void (*text)(void* window, const char* commit, const char* preedit, int cursor);
    // 0 resized, 1 close requested, 2 active, 3 scale, 4 maximized and full screen, 5 focus, 6 status item, 7 moved, 8 full screen bars,
    // 9 resized interactively.
    void (*window)(void* window, int event, double first, double second);
    void (*menuItem)(void* item);
    void (*menuOpens)(void* menu);
    // 0 entered, 1 moved, 2 left, 3 dropped.
    unsigned (*drag)(void* window, int event, double x, double y, void* offer, unsigned actions, unsigned modifiers);
    // action: 0 activated, 1 with the second button, 2 its context menu.
    void (*statusItem)(void* item, int action, double x, double y);
    void (*terminate)();
    void (*screensChanged)();
    void (*drawRects)(void* window, reef_ui_gc* gc, const double* rects, unsigned count);
    // Started again, by another process that was given these files or URLs and this activation token.
    void (*launched)(int count, const char* const* items, const char* token);
    void (*quit)();
    void (*themeChanged)();
    void (*languageChanged)();
    void (*drawPlane)(void* window, void* plane, reef_ui_gc* gc, const double* rects, unsigned count);
    void (*frame)(void* window);
    void (*dockMenu)();
    void (*gesture)(void* window, int type, unsigned fingers, double x, double y, double scale, double rotation, int cancelled,
                    unsigned modifiers, unsigned time);
};

namespace cutefish::reef {

class BaseView;
class FadeOverlay;
class RootView;

} // namespace cutefish::reef

struct reef_ui_window
{
    void* context = nullptr;
    std::unique_ptr<cutefish::ui::StatusItem> statusItem;
    std::unique_ptr<cutefish::ui::Window> owned;
    // That of `owned` or of the status item.
    cutefish::ui::Window* window = nullptr;
    cutefish::reef::RootView* root = nullptr;
    cutefish::reef::BaseView* base = nullptr;
    std::vector<reef_ui_plane*> planes;
    std::vector<reef_ui_overlay*> overlays;
    // Overlays that were destroyed, kept until what is below was drawn.
    std::vector<std::pair<std::chrono::steady_clock::time_point, std::unique_ptr<cutefish::reef::FadeOverlay>>> retired;
    std::unique_ptr<cutefish::base::Timer> retiredTimer;
    bool positioned = false;
    int placement = 0;
    cutefish::ui::PointF position;
    bool transparent = false;
    bool opaque = false;
    // Of a window that is not transparent itself.
    int transparentUnderlays = 0;
    // No background is drawn.
    bool cleared = false;
    // OpenGL surfaces below the window.
    int underlays = 0;
    cutefish::ui::RectF subsurfaceBlur;
    double subsurfaceBlurRadius = 0;
    // Five numbers each: the rectangle and the radius.
    std::vector<double> subsurfaceAreas;
    // None or nine numbers: tint, blend, color.
    std::vector<double> subsurfaceFill;
    bool subsurfaceBlurred = false;
    bool blurred = false;
    std::vector<double> behindMaterials;
    bool hasBehindMaterials = false;
    int cursor = 0;
    // How often it was changed, for tests.
    int cursorChanges = 0;
    std::shared_ptr<bool> alive = std::make_shared<bool>(true);
};

namespace cutefish::reef {

// application.cpp
extern ui::Application* application;
extern reef_ui_callbacks callbacks;
extern bool headless;
extern bool headlessStatusItems;
extern std::string applicationId;
// Whole numbers for programs, also where windows are drawn at the scale of the output.
double renderScale(double scale);
bool hasArgument(const char* name, const char* value);
base::dbus::Connection* sessionBus();
// The primary screen first.
std::vector<ui::Application::Screen> screens();
ui::PointF screensOrigin();
// sound.cpp, gl.cpp, gpu_activity.cpp
void setSoundSilent(bool silent);
void setGlHeadless(bool headless);
void logDrawnFrames();

// Makes one out of the process's own arguments, for a program that did not start it.
void ensureApplication();
// 6
const char* systemFontFamily();

// The smallest rectangle that has both.
inline ui::RectF united(const ui::RectF& a, const ui::RectF& b)
{
    const double left = std::min(a.x, b.x);
    const double top = std::min(a.y, b.y);
    return {left, top, std::max(a.right(), b.right()) - left, std::max(a.bottom(), b.bottom()) - top};
}

// blur.cpp
// An OpenGL surface is put below the window, or taken away.
void windowSetUnderlay(reef_ui_window* window, bool underlay);

// gl.cpp
// The window is going away.
void forgetWindow(reef_ui_window* window);

// windows.cpp
ui::Window* windowOf(reef_ui_window* window);

// menus.cpp
int hostStatusItemsSupported();

// desktop.cpp
// The length of the text; what fits is written with a zero at the end.
int copyString(const std::string& text, char* buffer, int size);

} // namespace cutefish::reef
