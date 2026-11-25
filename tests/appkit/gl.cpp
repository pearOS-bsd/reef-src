// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// OpenGL contexts and surfaces without a display (EGL of Mesa, drawn by the CPU): random programs, the log has the answers, what is
// current and hashes of the frames.
// usage: gl LIBRARY STEPS SEED
#include "load.h"

#include "host.h"

#include <EGL/egl.h>
#include <GL/glcorearb.h>
#include <unistd.h>

static std::vector<reef_ui_gl_context*> contexts;
static std::vector<reef_ui_gl_surface*> surfaces;
static std::vector<reef_ui_window*> windows;
static reef_ui_gl_context* active;
static std::vector<unsigned> textures;

static PFNGLCLEARCOLORPROC clearColor;
static PFNGLCLEARPROC clear;
static PFNGLSCISSORPROC scissor;
static PFNGLENABLEPROC enable;
static PFNGLGETINTEGERVPROC getIntegerv;
static PFNGLGENTEXTURESPROC genTextures;
static PFNGLBINDTEXTUREPROC bindTexture;
static PFNGLTEXIMAGE2DPROC texImage2D;
static PFNGLGETERRORPROC getError;

static void resize(void* context)
{
    say("resize %ld", long(context));
}

static void what()
{
    say("current: context %d draw %d read %d display %d", eglGetCurrentContext() != EGL_NO_CONTEXT,
        eglGetCurrentSurface(EGL_DRAW) != EGL_NO_SURFACE, eglGetCurrentSurface(EGL_READ) != EGL_NO_SURFACE,
        eglGetCurrentDisplay() != EGL_NO_DISPLAY);
    if (eglGetCurrentContext() != EGL_NO_CONTEXT && getIntegerv) {
        GLint major = 0, minor = 0, profile = 0, draw = 0, read = 0;
        getIntegerv(GL_MAJOR_VERSION, &major);
        getIntegerv(GL_MINOR_VERSION, &minor);
        getIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
        getIntegerv(GL_DRAW_BUFFER, &draw);
        getIntegerv(GL_READ_BUFFER, &read);
        say("  version %d.%d profile %d buffers %x %x error %x", major, minor, profile, draw, read, getError());
    }
}

static void step()
{
    reef_ui_gl_context* context = contexts.empty() || chance(10) ? nullptr : contexts[pick(int(contexts.size()))];
    reef_ui_gl_surface* surface = surfaces.empty() || chance(5) ? nullptr : surfaces[pick(int(surfaces.size()))];
    switch (pick(21)) {
    case 0:
        if (contexts.size() < 4) {
            reef_ui_gl_context* made = host->glContextCreate(pick(2), context);
            say("context %d", made != nullptr);
            if (made)
                contexts.push_back(made);
        }
        break;
    case 1:
        if (context && chance(30)) {
            host->glContextDestroy(context);
            std::erase(contexts, context);
            if (active == context)
                active = nullptr;
            what();
        }
        break;
    case 2:
        if (surfaces.size() < 4) {
            reef_ui_gl_surface* made = host->glSurfaceCreate(windows.empty() || chance(30) ? nullptr : windows[0]);
            say("surface %d offscreen %d", made != nullptr, host->glSurfaceIsOffscreen(made));
            if (made)
                surfaces.push_back(made);
        }
        break;
    case 3:
        if (surface && chance(30)) {
            host->glSurfaceDestroy(surface);
            std::erase(surfaces, surface);
            what();
        }
        break;
    case 4:
    case 5: {
        const int made = host->glContextMakeCurrent(context, surface);
        say("make current %d", made);
        if (made)
            active = context;
        what();
        break;
    }
    case 6:
    case 7:
        host->glSurfaceSetGeometry(surface, number(0, 10), number(0, 10), number(-3, 60), number(0, 60));
        break;
    case 8:
        host->glSurfaceSetBufferScale(surface, chance(30) ? 0 : number(0, 3));
        break;
    case 9:
    case 10:
    case 11:
    case 12: {
        int width = -1, height = -1;
        const int begun = host->glSurfaceBeginFrame(surface, context, chance(95) ? &width : nullptr, chance(95) ? &height : nullptr);
        say("begin %d %dx%d", begun, width, height);
        what();
        if (!begun)
            break;
        active = context;
        clearColor(float(number(0, 1)), float(number(0, 1)), float(number(0, 1)), float(number(0, 1)));
        clear(GL_COLOR_BUFFER_BIT);
        enable(GL_SCISSOR_TEST);
        scissor(2, 3, 10, 5);
        clearColor(1, 0.5f, 0.25f, 1);
        clear(GL_COLOR_BUFFER_BIT);
        if (chance(90))
            say("end %d", host->glSurfaceEndFrame(surface));
        if (width > 0 && height > 0 && chance(80)) {
            const size_t stride = size_t(width) * 4 + pick(3) * 8;
            std::vector<unsigned char> pixels(stride * height, 0x55);
            int taken = host->glSurfaceTakePixels(surface, pixels.data(), stride, width, chance(95) ? height : height + 1);
            say("taken %d %016lx", taken, (unsigned long)hashOf(pixels.data(), pixels.size()));
            taken = host->glSurfaceTakePixels(surface, pixels.data(), stride, width, height);
            say("again %d", taken);
        }
        break;
    }
    case 13:
        host->glSurfaceSetOpaque(surface, pick(2));
        break;
    case 14:
        host->glSurfaceSetAbove(surface, pick(2));
        break;
    case 15:
        say("ready %d %d", host->glSurfaceIsReady(surface), host->glSurfaceWaitForFrame(surface, pick(3)));
        break;
    case 16:
        host->glSurfaceSetResizeHandler(surface, chance(70) ? resize : nullptr, (void*)5L);
        break;
    case 17:
        say("address %d %d", host->glProcAddress(nullptr) != nullptr, host->glProcAddress("glClear") != nullptr);
        break;
    case 18:
        if (active) {
            unsigned texture = 0;
            genTextures(1, &texture);
            bindTexture(GL_TEXTURE_2D, texture);
            std::vector<unsigned char> pixels(32 * 32 * 4, 0x80);
            texImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
            int planes = -1, descriptors[4] = {-1, -1, -1, -1}, fourcc = -1, offsets[4] = {}, strides[4] = {};
            uint64_t modifier = 0;
            const int exported = host->glTextureExportDmaBuf(texture, &planes, descriptors, &fourcc, offsets, strides, &modifier);
            say("export %d %d %x %d %d %lx", exported, planes, fourcc, offsets[0], strides[0], modifier);
            if (exported) {
                say("import %d", host->glTextureFromDmaBuf(32, 32, fourcc, descriptors[0], offsets[0], strides[0], modifier) != 0);
                close(descriptors[0]);
            }
            say("bad import %u %u", host->glTextureFromDmaBuf(32, 32, 0x34324241, -1, 0, 128, 0),
                host->glTextureFromDmaBufPlanes(32, 32, 0x34324241, 5, descriptors, (const unsigned*)offsets, (const unsigned*)strides, 0));
            say("error %x", getError());
        }
        break;
    case 19:
        if (windows.empty())
            windows.push_back(host->hostWindowCreate((void*)7L, 100, 80, 0, nullptr, 0, 0));
        else if (chance(50))
            host->hostWindowShow(windows[0]);
        else
            host->hostTestScale(windows[0], 1 + pick(3) * 0.5);
        break;
    case 20:
        what();
        break;
    }
}

int main(int argc, char** argv)
{
    load(argv[1]);
    const int steps = argc > 2 ? atoi(argv[2]) : 100;
    seed(argc > 3 ? atoi(argv[3]) : 1);
    const char* arguments[] = {"test", "-ReefHeadless", "YES"};
    reef_ui_callbacks callbacks = {};
    say("initialize %d", host->hostInitialize(3, arguments, "org.example.test", &callbacks));
    auto address = [](const char* name) { return eglGetProcAddress(name); };
    clearColor = PFNGLCLEARCOLORPROC(address("glClearColor"));
    clear = PFNGLCLEARPROC(address("glClear"));
    scissor = PFNGLSCISSORPROC(address("glScissor"));
    enable = PFNGLENABLEPROC(address("glEnable"));
    getIntegerv = PFNGLGETINTEGERVPROC(address("glGetIntegerv"));
    genTextures = PFNGLGENTEXTURESPROC(address("glGenTextures"));
    bindTexture = PFNGLBINDTEXTUREPROC(address("glBindTexture"));
    texImage2D = PFNGLTEXIMAGE2DPROC(address("glTexImage2D"));
    getError = PFNGLGETERRORPROC(address("glGetError"));
    for (int i = 0; i < steps; i++) {
        step();
        fflush(stdout);
    }
    say("end");
    fflush(stdout);
    _exit(0);
}
