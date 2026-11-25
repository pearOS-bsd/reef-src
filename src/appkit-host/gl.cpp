// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OpenGL for programs: contexts, surfaces above or below a window, and textures that share memory with other devices.

#include "host.h"

#include "ui/subsurface.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <set>
#include <unistd.h>

#include <GL/glcorearb.h>

struct reef_ui_gl_context
{
    EGLContext context;
};

struct reef_ui_gl_surface
{
    reef_ui_window* window;
    // None without a Wayland display: the frames are kept as pixels then.
    std::unique_ptr<cutefish::ui::Subsurface> subsurface;
    bool above = false;
    bool opaque = true;
    // Of what follows, used by the main thread and the one that draws.
    std::mutex mutex;
    double width = 0;
    double height = 0;
    double scale = 1;
    // 0 for the window's.
    double bufferScale = 0;
    EGLSurface pbuffer = EGL_NO_SURFACE;
    cutefish::base::Size pbufferSize{0, 0};
    std::vector<uint8_t> pixels;
    cutefish::base::Size pixelsSize{0, 0};
};

namespace cutefish::reef {

struct Egl
{
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig config = nullptr;
    // The display is the one of the windows.
    bool wayland = false;
    bool available = false;
};

// What was current before one of the program's contexts was.
struct SavedContext
{
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface draw = EGL_NO_SURFACE;
    EGLSurface read = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
};

static bool glHeadless;
static std::set<reef_ui_gl_surface*> surfaces;
static Egl eglState;
static thread_local reef_ui_gl_context* current;
static thread_local SavedContext saved;
// How many views are drawn.
static thread_local int drawing;

static bool hasExtension(const char* extensions, const char* name)
{
    if (!extensions)
        return false;
    const size_t length = strlen(name);
    for (const char* found = strstr(extensions, name); found; found = strstr(found + 1, name)) {
        if ((found == extensions || found[-1] == ' ') && (found[length] == ' ' || found[length] == '\0'))
            return true;
    }
    return false;
}

static const Egl& egl()
{
    static std::once_flag once;
    std::call_once(once, [] {
        ensureApplication();
        wl_display* wayland = glHeadless ? nullptr : ui::Subsurface::waylandDisplay();
        if (wayland) {
            eglState.display = eglGetPlatformDisplay(EGL_PLATFORM_WAYLAND_KHR, wayland, nullptr);
            eglState.wayland = true;
        } else {
            eglState.display = eglGetPlatformDisplay(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, nullptr);
        }
        if (!eglState.display || !eglInitialize(eglState.display, nullptr, nullptr)) {
            fprintf(stderr, "reef: no EGL display for OpenGL\n");
            return;
        }
        const char* extensions = eglQueryString(eglState.display, EGL_EXTENSIONS);
        if (!hasExtension(extensions, "EGL_KHR_no_config_context") || !hasExtension(extensions, "EGL_KHR_surfaceless_context")) {
            fprintf(stderr, "reef: the EGL driver cannot make OpenGL contexts without a config\n");
            return;
        }
        const EGLint attributes[] = {
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_RED_SIZE,   8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE,   8,               EGL_DEPTH_SIZE,      24,             EGL_STENCIL_SIZE, 8, EGL_NONE,
        };
        EGLint count = 0;
        if (!eglChooseConfig(eglState.display, attributes, &eglState.config, 1, &count) || count < 1)
            eglState.config = nullptr;
        eglState.available = true;
    });
    return eglState;
}

template<typename Function>
static Function procAddress(const char* name)
{
    return reinterpret_cast<Function>(eglGetProcAddress(name));
}

// A context that was made current without a surface first draws to no buffer.
static void useDefaultBuffers()
{
    static const auto getIntegerv = procAddress<PFNGLGETINTEGERVPROC>("glGetIntegerv");
    static const auto bindFramebuffer = procAddress<PFNGLBINDFRAMEBUFFERPROC>("glBindFramebuffer");
    static const auto drawBuffer = procAddress<PFNGLDRAWBUFFERPROC>("glDrawBuffer");
    static const auto readBuffer = procAddress<PFNGLREADBUFFERPROC>("glReadBuffer");
    static const auto getBooleanv = procAddress<PFNGLGETBOOLEANVPROC>("glGetBooleanv");
    if (!getIntegerv || !getBooleanv || !bindFramebuffer || !drawBuffer || !readBuffer)
        return;
    GLint drawFramebuffer = 0, readFramebuffer = 0;
    getIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFramebuffer);
    getIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFramebuffer);
    bindFramebuffer(GL_FRAMEBUFFER, 0);
    GLboolean doubleBuffered = GL_FALSE;
    getBooleanv(GL_DOUBLEBUFFER, &doubleBuffered);
    const GLenum buffer = doubleBuffered ? GL_BACK : GL_FRONT;
    drawBuffer(buffer);
    readBuffer(buffer);
    bindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);
    bindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
}

static void saveCurrent()
{
    saved.context = eglGetCurrentContext();
    saved.display = eglGetCurrentDisplay();
    saved.draw = eglGetCurrentSurface(EGL_DRAW);
    saved.read = eglGetCurrentSurface(EGL_READ);
}

// While a view is drawn its surfaces are needed again; otherwise the window's may be gone.
static void restoreCurrent(EGLDisplay display)
{
    if (!saved.context)
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    else if (drawing > 0 || !eglMakeCurrent(saved.display, EGL_NO_SURFACE, EGL_NO_SURFACE, saved.context))
        eglMakeCurrent(saved.display, saved.draw, saved.read, saved.context);
    saved = {};
}

void setGlHeadless(bool headless)
{
    glHeadless = headless;
}

void beginDrawing()
{
    drawing++;
}

void endDrawing()
{
    drawing--;
}

void forgetWindow(reef_ui_window* window)
{
    for (reef_ui_gl_surface* surface : surfaces) {
        if (surface->window == window)
            surface->window = nullptr;
    }
}

// 221
reef_ui_gl_context* glContextCreate(int core, reef_ui_gl_context* shared)
{
    const Egl& state = egl();
    if (!state.available)
        return nullptr;
    const EGLenum api = eglQueryAPI();
    eglBindAPI(EGL_OPENGL_API);
    const EGLint coreAttributes[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3, EGL_CONTEXT_MINOR_VERSION, 2, EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, EGL_TRUE, EGL_NONE,
    };
    const EGLint legacyAttributes[] = {
        EGL_CONTEXT_MAJOR_VERSION, 2, EGL_CONTEXT_MINOR_VERSION, 1, EGL_CONTEXT_OPENGL_PROFILE_MASK,
        EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT, EGL_NONE,
    };
    const EGLContext context = eglCreateContext(state.display, EGL_NO_CONFIG_KHR, shared ? shared->context : EGL_NO_CONTEXT,
                                                core ? coreAttributes : legacyAttributes);
    eglBindAPI(api);
    if (!context) {
        fprintf(stderr, "reef: cannot create an OpenGL %s context (EGL error 0x%x)\n", core ? "core" : "legacy", eglGetError());
        return nullptr;
    }
    return new reef_ui_gl_context{context};
}

// 222
void glContextDestroy(reef_ui_gl_context* context)
{
    if (!context)
        return;
    if (current == context) {
        restoreCurrent(egl().display);
        current = nullptr;
    }
    eglDestroyContext(egl().display, context->context);
    delete context;
}

// 223
// What was current before comes back without a context.
int glContextMakeCurrent(reef_ui_gl_context* context, reef_ui_gl_surface* surface)
{
    const Egl& state = egl();
    if (!state.available)
        return 0;
    if (!context) {
        if (current)
            restoreCurrent(state.display);
        current = nullptr;
        return 1;
    }
    if (!current)
        saveCurrent();
    bool made;
    if (surface && surface->subsurface)
        made = surface->subsurface->makeCurrent(context->context);
    else if (surface && surface->pbuffer)
        made = eglMakeCurrent(state.display, surface->pbuffer, surface->pbuffer, context->context);
    else
        made = eglMakeCurrent(state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, context->context);
    if (!made)
        return 0;
    current = context;
    if (surface && (surface->subsurface || surface->pbuffer))
        useDefaultBuffers();
    return 1;
}

// 224
void* glProcAddress(const char* name)
{
    if (!name || !egl().available)
        return nullptr;
    return reinterpret_cast<void*>(eglGetProcAddress(name));
}

// 226
void glSurfaceDestroy(reef_ui_gl_surface* surface)
{
    if (!surface)
        return;
    surfaces.erase(surface);
    if (surface->subsurface) {
        surface->subsurface.reset();
        if (!surface->above)
            windowSetUnderlay(surface->window, false);
    }
    if (EGLSurface pbuffer = surface->pbuffer)
        eglDestroySurface(egl().display, pbuffer);
    delete surface;
}

static double surfaceScale(const reef_ui_gl_surface* surface, const ui::Window* window)
{
    return surface->bufferScale > 0 ? surface->bufferScale : window ? std::ceil(window->scale()) : 1;
}

// 227
void glSurfaceSetGeometry(reef_ui_gl_surface* surface, double x, double y, double width, double height)
{
    if (!surface)
        return;
    if (surface->subsurface) {
        surface->subsurface->setGeometry({x, y, width, height});
        return;
    }
    const ui::Window* window = windowOf(surface->window);
    std::lock_guard lock(surface->mutex);
    surface->width = width;
    surface->height = height;
    surface->scale = surfaceScale(surface, window);
}

// 307
void glSurfaceSetBufferScale(reef_ui_gl_surface* surface, double scale)
{
    if (!surface)
        return;
    if (surface->subsurface) {
        surface->subsurface->setBufferScale(scale);
        return;
    }
    const ui::Window* window = windowOf(surface->window);
    std::lock_guard lock(surface->mutex);
    surface->bufferScale = scale;
    surface->scale = surfaceScale(surface, window);
}

// 228
// Any thread.
int glSurfaceBeginFrame(reef_ui_gl_surface* surface, reef_ui_gl_context* context, int* width, int* height)
{
    if (width)
        *width = 0;
    if (height)
        *height = 0;
    if (!surface || !context)
        return 0;
    if (!current)
        saveCurrent();
    if (surface->subsurface) {
        const base::Size size = surface->subsurface->beginFrame(context->context);
        if (size.width() <= 0 || size.height() <= 0)
            return 0;
        current = context;
        useDefaultBuffers();
        if (width)
            *width = size.width();
        if (height)
            *height = size.height();
        return 1;
    }
    const Egl& state = egl();
    if (!state.available || !state.config)
        return 0;
    int frameWidth, frameHeight;
    {
        std::lock_guard lock(surface->mutex);
        frameWidth = int(std::lround(surface->width * surface->scale));
        frameHeight = int(std::lround(surface->height * surface->scale));
    }
    if (frameWidth <= 0 || frameHeight <= 0)
        return 0;
    if (surface->pbuffer && (surface->pbufferSize.width() != frameWidth || surface->pbufferSize.height() != frameHeight)) {
        if (current)
            eglMakeCurrent(state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, current->context);
        eglDestroySurface(state.display, surface->pbuffer);
        surface->pbuffer = EGL_NO_SURFACE;
    }
    if (!surface->pbuffer) {
        const EGLint attributes[] = {EGL_WIDTH, frameWidth, EGL_HEIGHT, frameHeight, EGL_NONE};
        surface->pbuffer = eglCreatePbufferSurface(state.display, state.config, attributes);
        if (!surface->pbuffer)
            return 0;
        surface->pbufferSize = {frameWidth, frameHeight};
    }
    if (!eglMakeCurrent(state.display, surface->pbuffer, surface->pbuffer, context->context))
        return 0;
    current = context;
    useDefaultBuffers();
    if (width)
        *width = frameWidth;
    if (height)
        *height = frameHeight;
    return 1;
}

// 229
int glSurfaceEndFrame(reef_ui_gl_surface* surface)
{
    if (!surface)
        return 0;
    if (surface->subsurface)
        return surface->subsurface->endFrame();
    if (!surface->pbuffer)
        return 0;
    static const auto readPixels = procAddress<PFNGLREADPIXELSPROC>("glReadPixels");
    static const auto finish = procAddress<PFNGLFINISHPROC>("glFinish");
    if (!readPixels || !finish)
        return 0;
    finish();
    const base::Size size = surface->pbufferSize;
    std::vector<uint8_t> pixels(size_t(size.width()) * size.height() * 4);
    readPixels(0, 0, size.width(), size.height(), GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, pixels.data());
    std::lock_guard lock(surface->mutex);
    surface->pixels = std::move(pixels);
    surface->pixelsSize = size;
    return 1;
}

// 230
// The program draws what glSurfaceTakePixels gives.
int glSurfaceIsOffscreen(reef_ui_gl_surface* surface)
{
    return surface && !surface->subsurface;
}

// 231
// The last frame, once.
int glSurfaceTakePixels(reef_ui_gl_surface* surface, void* pixels, size_t stride, int width, int height)
{
    if (!surface || !pixels)
        return 0;
    std::lock_guard lock(surface->mutex);
    if (surface->pixels.empty() || surface->pixelsSize != base::Size(width, height))
        return 0;
    const size_t row = size_t(width) * 4;
    for (int y = 0; y < height; y++)
        memcpy(static_cast<uint8_t*>(pixels) + y * stride, surface->pixels.data() + (height - 1 - y) * row, row);
    surface->pixels = std::vector<uint8_t>();
    return 1;
}

// 233
void glSurfaceSetOpaque(reef_ui_gl_surface* surface, int opaque)
{
    if (!surface)
        return;
    surface->opaque = opaque;
    if (surface->subsurface)
        surface->subsurface->setOpaque(opaque);
}

// 265
void glSurfaceSetAbove(reef_ui_gl_surface* surface, int above)
{
    if (!surface || !surface->subsurface || surface->above == (above != 0))
        return;
    surface->subsurface->setStacking(above ? ui::Subsurface::Stacking::Above : ui::Subsurface::Stacking::Below);
    surface->above = above;
    windowSetUnderlay(surface->window, !above);
}

// 287
// A texture on the memory of a DMA-BUF of 1 to 4 planes.
unsigned glTextureFromDmaBufPlanes(int width, int height, unsigned fourcc, int planes, const int* descriptors, const unsigned* offsets,
                                   const unsigned* strides, uint64_t modifier)
{
    static const auto createImage = procAddress<PFNEGLCREATEIMAGEKHRPROC>("eglCreateImageKHR");
    static const auto destroyImage = procAddress<PFNEGLDESTROYIMAGEKHRPROC>("eglDestroyImageKHR");
    static const auto targetTexture = procAddress<void (*)(GLenum, void*)>("glEGLImageTargetTexture2DOES");
    static const auto targetStorage = procAddress<void (*)(GLenum, void*, const GLint*)>("glEGLImageTargetTexStorageEXT");
    static const auto getError = procAddress<PFNGLGETERRORPROC>("glGetError");
    static const auto genTextures = procAddress<PFNGLGENTEXTURESPROC>("glGenTextures");
    static const auto bindTexture = procAddress<PFNGLBINDTEXTUREPROC>("glBindTexture");
    static const auto getIntegerv = procAddress<PFNGLGETINTEGERVPROC>("glGetIntegerv");
    static const auto deleteTextures = procAddress<PFNGLDELETETEXTURESPROC>("glDeleteTextures");
    static const auto texParameteri = procAddress<PFNGLTEXPARAMETERIPROC>("glTexParameteri");
    const Egl& state = egl();
    if (!state.available || !createImage || !destroyImage || !targetTexture || !genTextures || !bindTexture || !getIntegerv
        || !deleteTextures || !texParameteri || !getError)
        return 0;
    if (planes < 1 || planes > 4 || descriptors[0] < 0)
        return 0;
    static const EGLint names[4][5] = {
        {EGL_DMA_BUF_PLANE0_FD_EXT, EGL_DMA_BUF_PLANE0_OFFSET_EXT, EGL_DMA_BUF_PLANE0_PITCH_EXT, EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT,
         EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT},
        {EGL_DMA_BUF_PLANE1_FD_EXT, EGL_DMA_BUF_PLANE1_OFFSET_EXT, EGL_DMA_BUF_PLANE1_PITCH_EXT, EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT,
         EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT},
        {EGL_DMA_BUF_PLANE2_FD_EXT, EGL_DMA_BUF_PLANE2_OFFSET_EXT, EGL_DMA_BUF_PLANE2_PITCH_EXT, EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT,
         EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT},
        {EGL_DMA_BUF_PLANE3_FD_EXT, EGL_DMA_BUF_PLANE3_OFFSET_EXT, EGL_DMA_BUF_PLANE3_PITCH_EXT, EGL_DMA_BUF_PLANE3_MODIFIER_LO_EXT,
         EGL_DMA_BUF_PLANE3_MODIFIER_HI_EXT},
    };
    std::vector<EGLint> attributes = {EGL_WIDTH, width, EGL_HEIGHT, height, EGL_LINUX_DRM_FOURCC_EXT, EGLint(fourcc)};
    for (int i = 0; i < planes; i++) {
        attributes.insert(attributes.end(), {names[i][0], descriptors[i], names[i][1], EGLint(offsets[i]), names[i][2], EGLint(strides[i])});
        // DRM_FORMAT_MOD_INVALID
        if (modifier != 0x00ffffffffffffffULL)
            attributes.insert(attributes.end(), {names[i][3], EGLint(modifier & 0xffffffff), names[i][4], EGLint(modifier >> 32)});
    }
    attributes.push_back(EGL_NONE);
    const EGLImageKHR image = createImage(state.display, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr, attributes.data());
    if (!image)
        return 0;
    GLint previous = 0;
    getIntegerv(GL_TEXTURE_BINDING_2D, &previous);
    while (getError() != GL_NO_ERROR) {
    }
    GLuint texture = 0;
    genTextures(1, &texture);
    bindTexture(GL_TEXTURE_2D, texture);
    bool stored = false;
    if (targetStorage) {
        targetStorage(GL_TEXTURE_2D, image, nullptr);
        stored = getError() == GL_NO_ERROR;
    }
    if (!stored) {
        deleteTextures(1, &texture);
        genTextures(1, &texture);
        bindTexture(GL_TEXTURE_2D, texture);
        targetTexture(GL_TEXTURE_2D, image);
    }
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    bindTexture(GL_TEXTURE_2D, previous);
    destroyImage(state.display, image);
    return texture;
}

// 247
unsigned glTextureFromDmaBuf(int width, int height, unsigned fourcc, int descriptor, unsigned offset, unsigned stride, uint64_t modifier)
{
    return glTextureFromDmaBufPlanes(width, height, fourcc, 1, &descriptor, &offset, &stride, modifier);
}

// 286
// The descriptors are the caller's.
int glTextureExportDmaBuf(unsigned texture, int* planes, int* descriptors, int* fourcc, int* offsets, int* strides, uint64_t* modifier)
{
    static const auto createImage = procAddress<PFNEGLCREATEIMAGEKHRPROC>("eglCreateImageKHR");
    static const auto destroyImage = procAddress<PFNEGLDESTROYIMAGEKHRPROC>("eglDestroyImageKHR");
    static const auto queryImage = procAddress<PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC>("eglExportDMABUFImageQueryMESA");
    static const auto exportImage = procAddress<PFNEGLEXPORTDMABUFIMAGEMESAPROC>("eglExportDMABUFImageMESA");
    const Egl& state = egl();
    const EGLContext context = eglGetCurrentContext();
    if (!state.available || !createImage || !destroyImage || !queryImage || !exportImage || !texture || !context)
        return 0;
    const EGLint attributes[] = {EGL_GL_TEXTURE_LEVEL, 0, EGL_NONE};
    const EGLImageKHR image = createImage(state.display, context, EGL_GL_TEXTURE_2D, reinterpret_cast<EGLClientBuffer>(uintptr_t(texture)),
                                          attributes);
    if (!image)
        return 0;
    int format = 0, count = 0;
    EGLuint64KHR modifiers[4] = {};
    int exported[4] = {-1, -1, -1, -1};
    EGLint exportedStrides[4] = {};
    EGLint exportedOffsets[4] = {};
    const bool done = queryImage(state.display, image, &format, &count, modifiers) && count >= 1 && count <= 4
        && exportImage(state.display, image, exported, exportedStrides, exportedOffsets);
    destroyImage(state.display, image);
    if (!done) {
        for (int descriptor : exported) {
            if (descriptor >= 0)
                close(descriptor);
        }
        return 0;
    }
    *planes = count;
    *fourcc = format;
    *modifier = modifiers[0];
    for (int i = 0; i < count; i++) {
        descriptors[i] = exported[i];
        offsets[i] = exportedOffsets[i];
        strides[i] = exportedStrides[i];
    }
    return 1;
}

// 251
// The compositor took the last frame.
int glSurfaceIsReady(reef_ui_gl_surface* surface)
{
    if (!surface || !surface->subsurface)
        return 1;
    return surface->subsurface->waitForFrame(0);
}

// 311
int glSurfaceWaitForFrame(reef_ui_gl_surface* surface, int milliseconds)
{
    if (!surface || !surface->subsurface)
        return 1;
    return surface->subsurface->waitForFrame(milliseconds);
}

// 236
// Called before the window is drawn in its new size, to draw the surface in it.
void glSurfaceSetResizeHandler(reef_ui_gl_surface* surface, void (*handler)(void*), void* context)
{
    if (!surface || !surface->subsurface)
        return;
    if (!handler) {
        surface->subsurface->onResize = nullptr;
        return;
    }
    surface->subsurface->onResize = [handler, context](base::Size) {
        beginDrawing();
        handler(context);
        endDrawing();
    };
}

// 225
// Below the window.
reef_ui_gl_surface* glSurfaceCreate(reef_ui_window* window)
{
    auto surface = new reef_ui_gl_surface;
    surface->window = window;
    surfaces.insert(surface);
    if (!window || !egl().wayland)
        return surface;
    ui::Window* owner = windowOf(window);
    if (!owner)
        return surface;
    auto subsurface = std::make_unique<ui::Subsurface>(owner);
    if (!subsurface->isValid() || subsurface->eglDisplay() != egl().display)
        return surface;
    subsurface->setOpaque(true);
    surface->subsurface = std::move(subsurface);
    windowSetUnderlay(window, true);
    return surface;
}

} // namespace cutefish::reef
