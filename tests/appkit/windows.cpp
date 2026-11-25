// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// Windows, planes, input and menus without a display: random programs; the log has the callbacks, the answers and hashes of the
// windows' pixels.
// usage: windows LIBRARY STEPS SEED
#include "load.h"

#include "host.h"

#include "input.h"
#include "menus.h"
#include "planes.h"

#include <cmath>
#include <unistd.h>

static int depth;

static void drawSomething(reef_ui_gc* gc, double x, double y, double width, double height)
{
    const double red = fmod(fabs(x) * 0.013 + 0.1, 1), green = fmod(fabs(y) * 0.017 + 0.2, 1), blue = fmod(fabs(width + height) * 0.007, 1);
    host->gcSetFillColor(gc, red, green, blue, 1);
    host->gcFillRect(gc, x, y, width, height);
    host->gcSetStrokeColor(gc, 1 - red, 1 - green, 1 - blue, 0.5);
    host->gcSetLineWidth(gc, 2);
    host->gcStrokeRect(gc, 3, 4, 50, 30);
}

static void onDraw(void* window, reef_ui_gc* gc, double x, double y, double width, double height)
{
    say("draw %ld %a %a %a %a scale %a", long(window), x, y, width, height, host->gcDeviceScale(gc));
    drawSomething(gc, x, y, width, height);
}

static void sayRects(const char* name, void* window, const double* rects, unsigned count)
{
    say("%s %ld %u", name, long(window), count);
    for (unsigned i = 0; i < count; i++)
        say("  %a %a %a %a", rects[i * 4], rects[i * 4 + 1], rects[i * 4 + 2], rects[i * 4 + 3]);
}

static void onDrawRects(void* window, reef_ui_gc* gc, const double* rects, unsigned count)
{
    sayRects("rects", window, rects, count);
    for (unsigned i = 0; i < count; i++)
        drawSomething(gc, rects[i * 4], rects[i * 4 + 1], rects[i * 4 + 2], rects[i * 4 + 3]);
}

static void onDrawPlane(void* window, void* plane, reef_ui_gc* gc, const double* rects, unsigned count)
{
    sayRects("plane", window, rects, count);
    say("  of %ld", long(plane));
    for (unsigned i = 0; i < count; i++)
        drawSomething(gc, rects[i * 4], rects[i * 4 + 1], rects[i * 4 + 2], rects[i * 4 + 3]);
}

static void onPointer(void* window, double x, double y, int type, int button, int clicks, unsigned modifiers)
{
    say("pointer %ld %a %a type %d button %d clicks %d modifiers %u", long(window), x, y, type, button, clicks, modifiers);
}

static void onScroll(void* window, double dx, double dy, int precise, int stop, unsigned modifiers, int x120, int y120, unsigned)
{
    say("scroll %ld %a %a %d %d %u %d %d", long(window), dx, dy, precise, stop, modifiers, x120, y120);
}

static int onKey(void* window, int pressed, uint32_t key, uint32_t code, const char* text, unsigned modifiers, int repeat)
{
    say("key %ld %d %u %u '%s' %u %d", long(window), pressed, key, code, text, modifiers, repeat);
    return key & 1;
}

static void onText(void* window, const char* commit, const char* preedit, int cursor)
{
    say("text %ld '%s' '%s' %d", long(window), commit, preedit, cursor);
}

static void onWindow(void* window, int event, double first, double second)
{
    say("window %ld event %d %a %a", long(window), event, first, second);
}

static void onMenuItem(void* item)
{
    say("menu item %ld", long(item));
}

static void onMenuOpens(void* item)
{
    say("menu opens %ld", long(item));
}

static unsigned onDrag(void* window, int event, double x, double y, void* offer, unsigned actions, unsigned modifiers)
{
    say("drag %ld %d %a %a %d %u %u", long(window), event, x, y, offer != nullptr, actions, modifiers);
    return actions & 1;
}

static void onScreens() { say("screens"); }
static void onTheme() { say("theme"); }
static void onLanguage() { say("language"); }
static void onTerminate() { say("terminate"); }
static void onDockMenu() { say("dock menu"); }
static void onFrame(void* window) { say("frame %ld", long(window)); }

struct Plane
{
    reef_ui_plane* plane;
    int window;
};

static std::vector<reef_ui_window*> windows;
static std::vector<Plane> planes;
static std::vector<reef_ui_menu*> menus;
static std::vector<reef_ui_scroller*> scrollers;
static int names = 1;

static void axes(void* context, reef_ui_scroll_axis* horizontal, reef_ui_scroll_axis* vertical)
{
    *horizontal = {0, 300, 100, 1};
    *vertical = {-10, 1000, 200, long(context) & 1 ? 1 : 5};
}

static double scrolledX, scrolledY;

static void position(void*, double* x, double* y)
{
    *x = scrolledX;
    *y = scrolledY;
}

static void applyPosition(void* context, double x, double y)
{
    scrolledX = x;
    scrolledY = y;
    say("scrolled %ld %a %a", long(context), x, y);
}

static void movingChanged(void* context, int moving)
{
    say("moving %ld %d", long(context), moving);
}

static void grab(reef_ui_window* window)
{
    int width = -1, height = -1;
    void* pixels = host->hostWindowGrab(window, &width, &height);
    say("grab %d %dx%d %016lx", pixels != nullptr, width, height, pixels ? (unsigned long)hashOf(pixels, size_t(width) * height * 4) : 0);
    free(pixels);
}

static void randomGlass(reef_ui_glass& glass)
{
    memset(&glass, 0, sizeof glass);
    glass.key = pick(4);
    glass.rect[0] = number(0, 100);
    glass.rect[1] = number(0, 100);
    glass.rect[2] = number(10, 100);
    glass.rect[3] = number(10, 100);
    glass.radius = number(0, 20);
    glass.opacity = number(0.3, 1);
    for (double& value : glass.tint)
        value = number(0, 1);
    glass.blur = number(0, 20);
    glass.saturation = number(0.5, 2);
    glass.bezel = number(0, 10);
    glass.bend = number(0, 10);
    glass.profile = number(0, 3);
    glass.zoom = 1;
    glass.exponent = 2;
    glass.sideBezel = number(0, 10);
    glass.blurPower = number(-1, 2);
    glass.edgeDevicePixels = number(0, 2);
    for (double& value : glass.edge)
        value = number(0, 1);
}

static void step()
{
    if (windows.empty() || (windows.size() < 3 && chance(4))) {
        const int flags = chance(50) ? 0 : pick(16) & ~(windows.empty() ? 1 : 0);
        reef_ui_window* parent = windows.empty() || chance(50) ? nullptr : windows[pick(int(windows.size()))];
        reef_ui_window* window = host->hostWindowCreate((void*)long(names++), number(20, 300), number(20, 300), flags, parent, number(0, 50), number(0, 50));
        say("create %d flags %d", window != nullptr, flags);
        if (window)
            windows.push_back(window);
        return;
    }
    const int at = pick(int(windows.size()));
    reef_ui_window* window = windows[at];
    double a, b, c, d;
    switch (pick(62)) {
    case 0:
        if (chance(15)) {
            std::erase_if(planes, [&](const Plane& plane) { return plane.window == at; });
            for (Plane& plane : planes)
                plane.window -= plane.window > at;
            host->hostWindowDestroy(window);
            windows.erase(windows.begin() + at);
            say("destroyed");
        }
        break;
    case 1: host->hostWindowSetTitle(window, chance(20) ? nullptr : "title"); break;
    case 2: host->hostWindowResize(window, number(20, 300), number(20, 300)); break;
    case 3: host->hostWindowSetMinimumSize(window, number(0, 50), number(0, 50)); break;
    case 4: host->hostWindowSetMaximumSize(window, number(300, 500), number(300, 500)); break;
    case 5:
    case 6: host->hostWindowShow(window); break;
    case 7: if (chance(30)) host->hostWindowHide(window); break;
    case 8: host->hostWindowRequestPaint(window); break;
    case 9: host->hostWindowRequestPaintRect(window, number(0, 100), number(0, 100), number(0, 100), number(0, 100)); break;
    case 10: host->hostWindowScroll(window, number(0, 100), number(0, 100), number(0, 100), number(0, 100), number(-20, 20), number(-20, 20)); break;
    case 11:
        host->hostWindowSize(window, &a, &b);
        say("size %a %a scale %a %a active %d", a, b, host->hostWindowScale(window), host->hostWindowOutputScale(window), host->hostWindowIsActive(window));
        break;
    case 12: host->hostWindowActivate(window); break;
    case 13: host->hostWindowSetMaximized(window, pick(2)); break;
    case 14: host->hostWindowSetPosition(window, number(-50, 50), number(-50, 50)); break;
    case 15: {
        const int known = host->hostWindowPosition(window, &a, &b);
        say("position %d %a %a placement %d", known, a, b, host->hostWindowPlacement(window));
        break;
    }
    case 16: host->hostWindowSetPlacement(window, pick(3)); break;
    case 17: host->hostWindowSetAlwaysOnTop(window, pick(2)); break;
    case 18: host->hostWindowSetLevel(window, pick(4) - 1); break;
    case 19: host->hostWindowSetOpacity(window, number(0, 1)); break;
    case 20: host->hostWindowSetInputTransparent(window, pick(2)); break;
    case 21: host->hostWindowSetCastsShadow(window, pick(2)); break;
    case 22: host->hostWindowSetUserResizable(window, pick(2)); break;
    case 23: host->hostWindowRequestLayout(window); break;
    case 24:
    case 25:
    case 26: grab(window); break;
    case 27: host->hostWindowSetToolTip(window, 1, 2, 30, 40, chance(50) ? "tip" : nullptr); break;
    case 28: {
        uint64_t rendered, presented, discarded;
        host->hostWindowFrameStatistics(window, &rendered, &presented, &discarded);
        say("frames %lu %lu %lu", rendered, presented, discarded);
        break;
    }
    case 29:
    case 30:
    case 31: host->hostTestPointer(window, pick(6), number(0, 200), number(0, 200), pick(4)); break;
    case 32: host->hostTestKey(window, 32 + pick(90), pick(16)); break;
    case 33: host->hostTestText(window, chance(50) ? "some text" : "ä"); break;
    case 34: host->hostTestScale(window, 1 + pick(5) * 0.25); break;
    case 35:
        host->hostWindowSetCursor(window, pick(40) - 2);
        {
            int changes = -1;
            const int cursor = host->hostWindowCursor(window, &changes);
            say("cursor %d %d", cursor, changes);
        }
        break;
    case 36:
        host->hostWindowSetTextInput(window, pick(2), 1, 2, 3, 4);
        say("accepts %d", host->hostWindowAcceptsTextInput(window));
        break;
    case 37:
    case 38:
        if (planes.size() < 6) {
            reef_ui_plane* plane = host->hostPlaneCreate(window, (void*)long(1000 + names++));
            say("plane %d", plane != nullptr);
            if (plane)
                planes.push_back({plane, at});
        }
        break;
    case 39:
        if (!planes.empty() && chance(40)) {
            const int i = pick(int(planes.size()));
            host->hostPlaneDestroy(planes[i].plane);
            planes.erase(planes.begin() + i);
        }
        break;
    case 40:
    case 41:
    case 42:
        if (!planes.empty()) {
            reef_ui_plane* plane = planes[pick(int(planes.size()))].plane;
            a = number(0, 100), b = number(0, 100), c = number(1, 150), d = number(1, 150);
            host->hostPlaneSetGeometry(plane, a, b, c, d, number(0, 60), number(0, 60), chance(40) ? -1 : number(0, 120), number(-5, 120),
                                       number(0, 20), chance(50) ? 1 : number(0, 1), pick(2));
        }
        break;
    case 43: {
        std::vector<reef_ui_plane*> order;
        for (const Plane& plane : planes) {
            if (plane.window == at)
                order.insert(order.begin() + pick(int(order.size()) + 1), plane.plane);
        }
        host->hostWindowSetPlaneOrder(window, order.data(), unsigned(order.size()));
        break;
    }
    case 44:
    case 45:
        if (!planes.empty())
            host->hostPlaneRequestPaint(planes[pick(int(planes.size()))].plane, number(0, 50), number(0, 50), chance(30) ? -1 : number(0, 100), number(0, 100));
        break;
    case 46:
        if (!planes.empty())
            host->hostPlaneScroll(planes[pick(int(planes.size()))].plane, number(0, 50), number(0, 50), number(0, 100), number(0, 100), number(-9, 9), number(-9, 9));
        break;
    case 47:
        if (!planes.empty()) {
            double areas[15];
            for (double& value : areas)
                value = number(0, 60);
            host->hostPlaneSetBackdrop(planes[pick(int(planes.size()))].plane, areas, pick(4), number(0, 10), number(0.5, 2));
        }
        break;
    case 48:
        if (!planes.empty())
            host->hostPlaneSetScale(planes[pick(int(planes.size()))].plane, chance(30) ? 1 : number(0.5, 1.5));
        break;
    case 49:
        if (!planes.empty()) {
            reef_ui_glass glass[2];
            randomGlass(glass[0]);
            randomGlass(glass[1]);
            const double light[4] = {0.5, 1, 2, 0.25};
            glass[0].light = light;
            glass[0].lightCount = pick(3);
            host->hostPlaneSetGlass(planes[pick(int(planes.size()))].plane, glass, pick(3));
        }
        break;
    case 50: {
        std::vector<reef_ui_menu_item> items;
        const int count = pick(8);
        int level = 0;
        const char* keys[] = {nullptr, "", "a", "\t", "\xef\x9c\x80", "\xef\x9c\x85", "\xef\x9c\xa6", "ß"};
        for (int i = 0; i < count; i++) {
            items.push_back({chance(10) ? nullptr : "item", (void*)long(5000 + i), level, unsigned(pick(128)), keys[pick(8)], unsigned(pick(16))});
            level = std::max(0, level + pick(3) - 1);
        }
        if (menus.size() < 3)
            menus.push_back(host->hostMenuCreate());
        reef_ui_menu* menu = menus[pick(int(menus.size()))];
        host->hostMenuSetItems(menu, items.data(), unsigned(items.size()));
        if (chance(30))
            host->hostMenuSetMain(menu);
        if (chance(30))
            host->hostMenuSetDock(menu);
        say("menu");
        break;
    }
    case 51:
        if (!menus.empty() && chance(30)) {
            const int i = pick(int(menus.size()));
            host->hostMenuDestroy(menus[i]);
            menus.erase(menus.begin() + i);
            say("menu destroyed");
        }
        break;
    case 52: {
        double frame[4], visible[4], scale;
        say("screens %d main %a", host->hostScreenCount(), host->hostMainScale());
        if (host->hostScreenGeometry(pick(3), frame, visible, &scale))
            say("screen %a %a %a %a %a %a %a %a %a", frame[0], frame[1], frame[2], frame[3], visible[0], visible[1], visible[2], visible[3], scale);
        host->hostMainScreenSize(&a, &b);
        host->hostMainVisibleFrame(frame);
        say("main %a %a visible %a %a %a %a", a, b, frame[0], frame[1], frame[2], frame[3]);
        break;
    }
    case 53:
        host->hostSetDark(pick(2));
        say("dark %d %08x %08x %s", int(host->hostIsDark()), host->hostHighlightColor(), host->hostTextColor(), host->systemFontFamily());
        break;
    case 54: {
        double areas[10];
        for (double& value : areas)
            value = number(0, 80);
        host->hostWindowSetBehindMaterials(window, areas, pick(3));
        break;
    }
    case 55: host->hostWindowSetShapeFromContent(window, pick(2)); break;
    case 56:
        if (scrollers.size() < 2 && chance(40)) {
            const reef_ui_scroller_callbacks callbacks = {(void*)long(names++), axes, position, applyPosition, movingChanged};
            scrollers.push_back(host->hostScrollerCreate(window, &callbacks));
            say("scroller %d", scrollers.back() != nullptr);
        } else if (!scrollers.empty() && scrollers.back()) {
            host->hostScrollerSetElastic(scrollers.back(), pick(2));
            say("scroll %d", host->hostScrollerScroll(scrollers.back(), number(-30, 30), number(-30, 30), pick(3) * 120 - 120, pick(3) * 120 - 120, pick(2), pick(2), 100 + names++, 0));
            say("active %d", host->hostScrollerIsActive(scrollers.back()));
        }
        break;
    case 57: {
        double rect[4];
        int dark, highlighted;
        uint32_t color;
        host->hostWindowStatusItemState(window, rect, &dark, &color, &highlighted);
        say("status item %a %a %a %a %d %08x %d", rect[0], rect[1], rect[2], rect[3], dark, color, highlighted);
        host->hostWindowSetStatusItemHighlighted(window, pick(2));
        host->hostWindowSetStatusItemIdentifier(window, chance(50) ? "name" : nullptr);
        break;
    }
    case 58: host->hostWindowSetFullScreen(window, pick(2)); break;
    case 59: host->hostWindowSetTransientParent(window, chance(30) ? nullptr : windows[0] == window ? nullptr : windows[0]); break;
    case 60: {
        const unsigned char pixels[4 * 4 * 4] = {9, 8, 7, 255};
        host->hostWindowSetCursorImage(window, pixels, 4, 4, 1 + pick(2), 1, 2);
        break;
    }
    case 61: {
        double curve[4] = {0.25, 0.1, 0.25, 1};
        const unsigned char pixels[4 * 4 * 4] = {};
        say("overlay %d", host->hostOverlayCreate(window, 1, 2, 4, 4, number(-1, 1), 0, pixels, 4, 4, 16, curve) != nullptr);
        break;
    }
    }
}

int main(int argc, char** argv)
{
    load(argv[1]);
    const int steps = argc > 2 ? atoi(argv[2]) : 300;
    const int first = argc > 3 ? atoi(argv[3]) : 1;
    seed(first);
    const char* arguments[] = {"test", "-ReefHeadless", "YES", "-ReefHeadlessStatusItems", first % 3 ? "YES" : "NO",
                               "-ReefOutputScaleRendering", first % 2 ? "YES" : "NO", "-ReefHeadlessDarkMode", first % 5 ? "NO" : "YES"};
    reef_ui_callbacks callbacks = {};
    callbacks.draw = onDraw;
    callbacks.pointer = onPointer;
    callbacks.scroll = onScroll;
    callbacks.key = onKey;
    callbacks.text = onText;
    callbacks.window = onWindow;
    callbacks.menuItem = onMenuItem;
    callbacks.menuOpens = onMenuOpens;
    callbacks.drag = onDrag;
    callbacks.terminate = onTerminate;
    callbacks.screensChanged = onScreens;
    callbacks.drawRects = first % 4 ? onDrawRects : nullptr;
    callbacks.themeChanged = onTheme;
    callbacks.languageChanged = onLanguage;
    callbacks.drawPlane = onDrawPlane;
    callbacks.frame = onFrame;
    callbacks.dockMenu = onDockMenu;
    say("initialize %d", host->hostInitialize(9, arguments, "org.example.test", &callbacks));
    say("headless %d descriptor %d status items %d", int(host->hostIsHeadless()), host->hostLoopDescriptor() >= 0, host->hostStatusItemsSupported());
    for (int i = 0; i < steps; i++) {
        step();
        if (i % 16 == 15) {
            host->hostLoopDispatch();
            for (reef_ui_window* window : windows)
                grab(window);
            fflush(stdout);
        }
    }
    say("end");
    fflush(stdout);
    _exit(0);
}
