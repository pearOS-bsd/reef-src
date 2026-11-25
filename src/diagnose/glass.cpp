// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// How fast the toolkit draws glass over moving content on the GPU, in a window of the screen's size.

#include "sections.h"

#include "base/event_loop.h"
#include "ui/application.h"
#include "ui/embedded_gpu.h"
#include "ui/embedded_window.h"
#include "ui/painter.h"
#include "ui/view.h"
#include "ui/view_glass.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <dirent.h>
#include <sstream>

namespace cutefish::reef::diagnose {

static constexpr int frames = 40;

namespace {

// Moving stripes and boxes.
class Scene : public ui::View
{
public:
    int frame = 0;

protected:
    void paint(ui::Painter& painter) override
    {
        const ui::RectF area = localRect();
        painter.fillRect(area, {30, 30, 42, 255});
        for (double y = -48; y < area.height + 48; y += 24) {
            const int row = int(y);
            painter.fillRect({0, y + std::fmod(frame * 3.0, 24.0), area.width, 12},
                             {uint8_t(80 + row % 120), 140, uint8_t(200 - row % 90), 255});
        }
        for (double x = 0; x < area.width; x += 96)
            painter.drawRoundedRect({x, std::fmod(frame * 5.0 + x * 7, area.height), 60, 40}, {10}, {240, 200, 60, 255}, 0, {0, 0, 0, 255});
    }
};

// A surfaceless context of OpenGL ES, robust where the driver makes them.
struct Context
{
    bool robust = false;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLContext context = EGL_NO_CONTEXT;
    PFNGLGETGRAPHICSRESETSTATUSPROC resetStatus = nullptr;
    std::string error;

    Context()
    {
        const auto getDisplay = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
        if (getDisplay)
            display = getDisplay(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, nullptr);
        if (display == EGL_NO_DISPLAY || !eglInitialize(display, nullptr, nullptr)) {
            error = "no surfaceless EGL display";
            return;
        }
        eglBindAPI(EGL_OPENGL_ES_API);
        const EGLint robustAttributes[] = {
            EGL_CONTEXT_MAJOR_VERSION, 3, EGL_CONTEXT_OPENGL_ROBUST_ACCESS, EGL_TRUE,
            EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY, EGL_LOSE_CONTEXT_ON_RESET, EGL_NONE,
        };
        context = eglCreateContext(display, EGL_NO_CONFIG_KHR, EGL_NO_CONTEXT, robustAttributes);
        robust = context != EGL_NO_CONTEXT;
        if (!robust) {
            const EGLint attributes[] = {EGL_CONTEXT_MAJOR_VERSION, 3, EGL_NONE};
            context = eglCreateContext(display, EGL_NO_CONFIG_KHR, EGL_NO_CONTEXT, attributes);
        }
        if (context == EGL_NO_CONTEXT || !eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
            error = format("no OpenGL ES 3 context (EGL error 0x%x)", eglGetError());
            return;
        }
        resetStatus = reinterpret_cast<PFNGLGETGRAPHICSRESETSTATUSPROC>(eglGetProcAddress("glGetGraphicsResetStatus"));
    }

    ~Context()
    {
        if (display == EGL_NO_DISPLAY)
            return;
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT)
            eglDestroyContext(display, context);
    }
};

struct Test
{
    const char* name;
    std::vector<ui::ViewGlass> panes;
};

} // namespace

static double monotonic()
{
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec / 1e9;
}

// The mode of the first connected screen.
static void screenSize(int& width, int& height)
{
    width = 1920;
    height = 1080;
    DIR* directory = opendir("/sys/class/drm");
    if (!directory)
        return;
    std::vector<std::string> names;
    while (const dirent* entry = readdir(directory))
        names.push_back(entry->d_name);
    closedir(directory);
    std::sort(names.begin(), names.end());
    for (const std::string& name : names) {
        const std::string path = "/sys/class/drm/" + name;
        if (name.find('-') == std::string::npos || trimmed(readFile(path + "/status")) != "connected")
            continue;
        std::istringstream modes(readFile(path + "/modes"));
        std::string mode;
        int modeWidth = 0, modeHeight = 0;
        if (std::getline(modes, mode) && sscanf(mode.c_str(), "%dx%d", &modeWidth, &modeHeight) == 2 && modeWidth > 0 && modeHeight > 0) {
            width = modeWidth;
            height = modeHeight;
            return;
        }
    }
}

static void light(ui::ViewGlass& glass)
{
    glass.light = {{0.5, 0.24}, {1.5, 0.14}, {2.5, 0.047}, {3.5, 0.035}};
}

void addGlass(Report& report)
{
    report.section("Glass, drawn by the toolkit on the GPU");
    int width, height;
    screenSize(width, height);
    const std::string timeout = trimmed(readFile("/sys/class/drm/card0/engine/rcs0/preempt_timeout_ms"));
    const long preemption = timeout.empty() ? 0 : strtol(timeout.c_str(), nullptr, 10);
    report.field("Window", format("%dx%d at scale 1, the screen's size", width, height));
    report.field("Preemption timeout", preemption ? format("%ld ms (the kernel resets the GPU past it)", preemption)
                                                  : "not reported by this driver");
    Context context;
    if (!context.error.empty()) {
        report.result("FAIL", "glass", context.error);
        return;
    }
    report.field("Reset detection", context.robust ? "robust context" : "none: the driver makes no robust context");
    base::EventLoop loop;
    static char name[] = "reef-diagnose";
    char* arguments[] = {name, nullptr};
    ui::Application application(1, arguments, ui::Application::Platform::Embedded);
    const std::unique_ptr<ui::EmbeddedGpu> gpu = ui::EmbeddedGpu::createForCurrentContext();
    if (!gpu) {
        report.result("FAIL", "glass", "Skia cannot draw on this OpenGL ES context");
        return;
    }

    const double w = width, h = height;
    ui::ViewGlass bar;
    bar.key = 1;
    bar.rect = {0, 0, w, 52};
    bar.blur = 20;
    bar.saturation = 1.8;
    bar.edge[3] = 0.097;
    bar.sideEdge[3] = 0.24;
    std::vector<ui::ViewGlass> titleBar = {bar};
    for (int i = 0; i < 4; i++) {
        ui::ViewGlass button;
        button.key = 2 + i;
        button.rect = {80.0 + i * 84, 10, 72, 32};
        button.radius = 16;
        button.tint[3] = 0.1;
        button.edge[3] = 0.106;
        button.sideEdge[3] = 0.33;
        button.blur = 1.5;
        button.saturation = 1.6;
        button.bezel = 12;
        button.bend = 6;
        button.dispersion = 0.12;
        button.shadow[3] = 0.05;
        button.shadowBlur = 12;
        button.shadowOffset = 4;
        light(button);
        titleBar.push_back(std::move(button));
    }
    std::vector<ui::ViewGlass> sidebar = titleBar;
    ui::ViewGlass side;
    side.key = 10;
    side.rect = {8, 8, 260, h - 16};
    side.radius = 18;
    side.blur = 20;
    side.saturation = 1.8;
    side.edge[3] = 0.097;
    side.sideEdge[3] = 0.24;
    light(side);
    sidebar.push_back(std::move(side));
    std::vector<ui::ViewGlass> largePane = sidebar;
    ui::ViewGlass large;
    large.key = 20;
    large.rect = {w * 0.15, h * 0.15, w * 0.7, h * 0.7};
    large.radius = 26;
    large.tint[3] = 0.2;
    large.edge[3] = 0.1;
    large.sideEdge[3] = 0.35;
    large.blur = 1;
    large.saturation = 0.4;
    const double extent = std::min(large.rect.width, large.rect.height);
    large.bezel = std::min(17 * std::sqrt(extent / 36), extent * 0.5);
    large.bend = large.bezel * 10 / 17;
    large.profile = 2.5;
    large.exponent = 2;
    large.blurAtHalfResolution = true;
    large.blurPower = 1.3;
    light(large);
    largePane.push_back(std::move(large));

    const Test tests[] = {
        {"glass.none", {}},
        {"glass.title-bar", titleBar},
        {"glass.sidebar", sidebar},
        {"glass.large-pane", largePane},
    };
    for (const Test& test : tests) {
        ui::EmbeddedWindow window;
        auto content = std::make_unique<Scene>();
        Scene* const scene = content.get();
        auto child = std::make_unique<ui::View>();
        ui::View* const glass = child.get();
        scene->addChild(std::move(child));
        glass->setGeometry({0, 0, w, h});
        glass->setGlass(test.panes);
        window.setContentView(std::move(content));
        window.setScale(1);
        window.setSize({w, h});
        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, window.pixelWidth(), window.pixelHeight());
        glBindTexture(GL_TEXTURE_2D, 0);
        std::vector<double> times;
        double first = 0;
        bool drawn = true;
        GLenum reset = 0;
        for (int i = 0; i < frames && drawn && !reset; i++) {
            scene->frame = i;
            scene->requestPaint();
            const double start = monotonic();
            drawn = window.render(*gpu, {texture, window.pixelWidth(), window.pixelHeight(), i > 0}) != nullptr;
            glFinish();
            const double milliseconds = (monotonic() - start) * 1000;
            if (context.robust && context.resetStatus)
                reset = context.resetStatus();
            if (i == 0)
                first = milliseconds;
            else if (i > 3)
                times.push_back(milliseconds);
        }
        glDeleteTextures(1, &texture);
        if (reset) {
            report.result("FAIL", test.name, format("the GPU was reset while it drew the glass (status 0x%x)", reset));
            return;
        }
        if (!drawn || times.empty()) {
            report.result("FAIL", test.name, "Skia did not draw on the GPU");
            continue;
        }
        std::sort(times.begin(), times.end());
        const double median = times[times.size() / 2];
        const double slowest = times.back();
        const std::string summary = format("%zu panes, %d frames: median %.1f ms, slowest %.1f ms, first %.1f ms (shaders compiled)",
                                           test.panes.size(), frames, median, slowest, first);
        if (preemption && std::max(first, slowest) > preemption * 0.5)
            report.result("FAIL", test.name, summary + format("; within half of the %ld ms preemption timeout", preemption));
        else if (median > 1000.0 / 60)
            report.result("WARN", test.name, summary + "; slower than 60 frames a second");
        else
            report.result("PASS", test.name, summary);
    }
}

} // namespace cutefish::reef::diagnose
