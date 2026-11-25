// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The application: its start, the event loop, the theme and the screens.

#include "host.h"

#include "ui/menu.h"
#include "ui/text_layout.h"
#include "ui/theme.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <thread>
#include <unistd.h>

namespace cutefish::reef {

ui::Application* application = nullptr;
reef_ui_callbacks callbacks;
bool headless = false;
bool headlessStatusItems = false;
std::string applicationId;

static std::vector<std::string> arguments;
static std::vector<char*> argumentPointers;
static bool headlessSessionBus = false;
static base::Subscription screensSubscription;
static base::Subscription themeSubscription;
static base::Subscription languageSubscription;
static std::string fontFamily;
static bool outputScaleRendering = false;
static bool exiting = false;
static bool started = false;

bool hasArgument(const char* name, const char* value)
{
    for (size_t i = 1; i < arguments.size(); i++) {
        if (arguments[i - 1] == name && arguments[i] == value)
            return true;
    }
    return false;
}

void requestTermination()
{
    if (application->isConnectionLost() && !exiting) {
        exiting = true;
        std::thread([] {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            _exit(1);
        }).detach();
    }
    if (callbacks.terminate)
        callbacks.terminate();
}

double renderScale(double scale)
{
    return outputScaleRendering ? std::ceil(scale) : scale;
}

// 1
int hostLoopDescriptor()
{
    return application ? application->loop().fd() : -1;
}

// 2
void hostLoopDispatch()
{
    if (application)
        application->loop().dispatchReceived();
}

// 3
bool hostIsHeadless()
{
    return headless;
}

// 6
const char* systemFontFamily()
{
    fontFamily = ui::Theme::current().fontFamily;
    if (fontFamily.empty())
        fontFamily = "sans-serif";
    return fontFamily.c_str();
}

// 146
const char* hostSystemFixedFontFamily()
{
    static std::string family;
    family = ui::systemFixedFont().family;
    if (family.empty())
        family = "monospace";
    return family.c_str();
}

// 7
bool hostIsDark()
{
    return ui::Theme::current().darkMode;
}

// 8
uint32_t hostHighlightColor()
{
    return ui::Theme::current().highlight.argb();
}

// 9
uint32_t hostTextColor()
{
    return ui::Theme::current().text.argb();
}

// The fonts, the icons and the accent stay as they are.
// 253
void hostSetDark(int dark)
{
    ui::Theme theme = dark ? ui::Theme::dark() : ui::Theme::light();
    const ui::Theme& current = ui::Theme::current();
    theme.highlight = current.highlight;
    theme.fontFamily = current.fontFamily;
    theme.fixedFontFamily = current.fixedFontFamily;
    theme.fontPointSize = current.fontPointSize;
    theme.iconTheme = current.iconTheme;
    theme.continuousCorners = current.continuousCorners;
    ui::Theme::setCurrent(theme);
}

// 254
void hostLanguageChanged()
{
    if (callbacks.languageChanged)
        callbacks.languageChanged();
}

base::dbus::Connection* sessionBus()
{
    return application ? application->sessionBus() : nullptr;
}

static void createApplication(const std::vector<std::string>& given)
{
    arguments = given;
    argumentPointers.clear();
    for (std::string& argument : arguments)
        argumentPointers.push_back(argument.data());
    argumentPointers.push_back(nullptr);
    headless = hasArgument("-ReefHeadless", "YES");
    headlessSessionBus = headless && hasArgument("-ReefHeadlessSessionBus", "YES");
    headlessStatusItems = headless && hasArgument("-ReefHeadlessStatusItems", "YES");
    setSoundSilent(headless && !hasArgument("-ReefHeadlessSound", "YES"));
    setGlHeadless(headless);
    const bool wholeScale = hasArgument("-ReefOutputScaleRendering", "NO");
    outputScaleRendering = !wholeScale;
    ui::Application::setWholeScaleRendering(wholeScale);
    ui::Application::setOutputScaleComposition(hasArgument("-ReefOutputScaleComposition", "YES"));
    ui::Application::setHeadlessSessionBus(headlessSessionBus);
    application = new ui::Application(int(arguments.size()), argumentPointers.data(), ui::Application::Platform(headless));
    logDrawnFrames();
    if (headless && hasArgument("-ReefHeadlessDarkMode", "YES")) {
        ui::Theme theme = ui::Theme::dark();
        theme.highlight = ui::Theme::current().highlight;
        ui::Theme::setCurrent(theme);
    }
}

// 0
int hostInitialize(int argc, const char* const* argv, const char* identifier, const reef_ui_callbacks* given)
{
    if (started)
        return 0;
    started = true;
    callbacks = *given;
    if (!application) {
        std::vector<std::string> list;
        for (int i = 0; i < argc; i++)
            list.push_back(argv[i]);
        createApplication(list);
    }
    if (identifier && *identifier) {
        application->setApplicationId(identifier);
        applicationId = identifier;
    }
    application->loop().setQuitHandler([](int) { requestTermination(); });
    screensSubscription = application->screensChanged.connect([] {
        if (callbacks.screensChanged)
            callbacks.screensChanged();
    });
    themeSubscription = ui::Theme::changed().connect([] {
        if (callbacks.themeChanged)
            callbacks.themeChanged();
    });
    languageSubscription = application->languageChanged.connect([] {
        if (callbacks.languageChanged)
            callbacks.languageChanged();
    });
    return 0;
}

std::vector<ui::Application::Screen> screens()
{
    std::vector<ui::Application::Screen> screens;
    if (!application)
        return screens;
    screens = application->screens();
    std::stable_partition(screens.begin(), screens.end(), [](const ui::Application::Screen& screen) { return screen.primary; });
    for (ui::Application::Screen& screen : screens)
        screen.scale = renderScale(screen.scale);
    return screens;
}

// 288
int hostScreenName(uint32_t number, char* buffer, size_t size)
{
    const std::vector<ui::Application::Screen> list = screens();
    if (number == 0 || number > list.size() || list[number - 1].name.empty())
        return 0;
    snprintf(buffer, size, "%s", list[number - 1].name.c_str());
    return 1;
}

ui::PointF screensOrigin()
{
    const std::vector<ui::Application::Screen> list = screens();
    return list.empty() ? ui::PointF() : list.front().geometry.topLeft();
}

static ui::Window* firstWindow()
{
    return application && !application->windows().empty() ? application->windows().front() : nullptr;
}

// 5
double hostMainScale()
{
    if (const ui::Window* window = firstWindow())
        return renderScale(window->scale());
    const std::vector<ui::Application::Screen> list = screens();
    return list.empty() ? 1 : renderScale(list.front().scale);
}

// 203
int hostScreenCount()
{
    return int(screens().size());
}

// 204
int hostScreenGeometry(uint32_t index, double* frame, double* visible, double* scale)
{
    const std::vector<ui::Application::Screen> list = screens();
    if (index >= list.size())
        return 0;
    const ui::PointF origin = list.front().geometry.topLeft();
    const ui::Application::Screen& screen = list[index];
    const bool hasWorkArea = screen.workArea.width > 0 && screen.workArea.height > 0;
    const ui::RectF& area = hasWorkArea ? screen.workArea : screen.geometry;
    frame[0] = screen.geometry.x - origin.x;
    frame[1] = screen.geometry.y - origin.y;
    frame[2] = screen.geometry.width;
    frame[3] = screen.geometry.height;
    visible[0] = area.x - origin.x;
    visible[1] = area.y - origin.y;
    visible[2] = area.width;
    visible[3] = area.height;
    *scale = screen.scale;
    return 1;
}

// 4
void hostMainScreenSize(double* width, double* height)
{
    ui::SizeF size;
    const std::vector<ui::Application::Screen> list = screens();
    if (!list.empty())
        size = list.front().geometry.size();
    else if (const ui::Window* window = firstWindow())
        size = window->screenSize();
    if (size.width <= 0 || size.height <= 0)
        size = {1440, 900};
    *width = size.width;
    *height = size.height;
}

// 153
void hostMainVisibleFrame(double* frame)
{
    double screen[4], scale;
    if (hostScreenGeometry(0, screen, frame, &scale))
        return;
    ui::SizeF size;
    hostMainScreenSize(&size.width, &size.height);
    if (const ui::Window* window = firstWindow()) {
        const ui::SizeF available = window->availableSize();
        if (available.width > 0 && available.height > 0)
            size = available;
    }
    frame[0] = 0;
    frame[1] = 0;
    frame[2] = size.width;
    frame[3] = size.height;
}

void ensureApplication()
{
    if (application || gettid() != getpid())
        return;
    std::ifstream file("/proc/self/cmdline");
    std::vector<std::string> list;
    std::string argument;
    while (std::getline(file, argument, '\0'))
        list.push_back(argument);
    createApplication(list);
}

} // namespace cutefish::reef
