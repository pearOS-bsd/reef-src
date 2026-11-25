// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EGL and OpenGL as reef uses them: a surfaceless display, contexts without a config, the entry points of OpenGL 4.5.

#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/glcorearb.h>
#include <cstdint>
#include <string>

namespace cutefish::reef::diagnose {

// GL_OES_EGL_image, which is not among the core extensions.
using ImageTargetTexture2D = void (*)(GLenum target, EGLImageKHR image);

struct EglDisplay
{
    EGLDisplay display = EGL_NO_DISPLAY;
    // Why reef cannot use it.
    std::string error;
};

const EglDisplay& eglDisplay();
bool hasEglExtension(const char* name);
EGLContext createContext(EGLContext shared);

struct Gl
{
    PFNGLGETSTRINGPROC getString = nullptr;
    PFNGLGETSTRINGIPROC getStringi = nullptr;
    PFNGLGETINTEGERVPROC getIntegerv = nullptr;
    PFNGLGETERRORPROC getError = nullptr;
    PFNGLCREATETEXTURESPROC createTextures = nullptr;
    PFNGLGENTEXTURESPROC genTextures = nullptr;
    PFNGLBINDTEXTUREPROC bindTexture = nullptr;
    PFNGLTEXIMAGE2DPROC texImage2D = nullptr;
    PFNGLTEXPARAMETERIPROC texParameteri = nullptr;
    PFNGLTEXTURESTORAGE2DPROC textureStorage2D = nullptr;
    PFNGLTEXTUREPARAMETERIPROC textureParameteri = nullptr;
    PFNGLTEXTUREVIEWPROC textureView = nullptr;
    PFNGLGETTEXTUREIMAGEPROC getTextureImage = nullptr;
    PFNGLDELETETEXTURESPROC deleteTextures = nullptr;
    PFNGLBINDTEXTUREUNITPROC bindTextureUnit = nullptr;
    PFNGLCREATEFRAMEBUFFERSPROC createFramebuffers = nullptr;
    PFNGLNAMEDFRAMEBUFFERTEXTUREPROC namedFramebufferTexture = nullptr;
    PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC checkNamedFramebufferStatus = nullptr;
    PFNGLBINDFRAMEBUFFERPROC bindFramebuffer = nullptr;
    PFNGLDELETEFRAMEBUFFERSPROC deleteFramebuffers = nullptr;
    PFNGLVIEWPORTPROC viewport = nullptr;
    PFNGLREADPIXELSPROC readPixels = nullptr;
    PFNGLPIXELSTOREIPROC pixelStorei = nullptr;
    PFNGLFINISHPROC finish = nullptr;
    PFNGLFLUSHPROC flush = nullptr;
    PFNGLCREATESHADERPROC createShader = nullptr;
    PFNGLSHADERSOURCEPROC shaderSource = nullptr;
    PFNGLCOMPILESHADERPROC compileShader = nullptr;
    PFNGLGETSHADERIVPROC getShaderiv = nullptr;
    PFNGLGETSHADERINFOLOGPROC getShaderInfoLog = nullptr;
    PFNGLDELETESHADERPROC deleteShader = nullptr;
    PFNGLCREATEPROGRAMPROC createProgram = nullptr;
    PFNGLATTACHSHADERPROC attachShader = nullptr;
    PFNGLLINKPROGRAMPROC linkProgram = nullptr;
    PFNGLGETPROGRAMIVPROC getProgramiv = nullptr;
    PFNGLUSEPROGRAMPROC useProgram = nullptr;
    PFNGLDELETEPROGRAMPROC deleteProgram = nullptr;
    PFNGLGETUNIFORMLOCATIONPROC getUniformLocation = nullptr;
    PFNGLUNIFORM4FPROC uniform4f = nullptr;
    PFNGLCREATEVERTEXARRAYSPROC createVertexArrays = nullptr;
    PFNGLBINDVERTEXARRAYPROC bindVertexArray = nullptr;
    PFNGLDELETEVERTEXARRAYSPROC deleteVertexArrays = nullptr;
    PFNGLDRAWARRAYSPROC drawArrays = nullptr;
    PFNGLCREATEBUFFERSPROC createBuffers = nullptr;
    PFNGLNAMEDBUFFERSTORAGEPROC namedBufferStorage = nullptr;
    PFNGLCLEARNAMEDBUFFERDATAPROC clearNamedBufferData = nullptr;
    PFNGLGETNAMEDBUFFERSUBDATAPROC getNamedBufferSubData = nullptr;
    PFNGLDELETEBUFFERSPROC deleteBuffers = nullptr;
    // Not in every driver.
    ImageTargetTexture2D imageTargetTexture2D = nullptr;
    PFNGLEGLIMAGETARGETTEXSTORAGEEXTPROC imageTargetTexStorage = nullptr;

    // Those of the context that is current; the names that are missing, with spaces between them.
    std::string load();
    bool hasExtension(const char* name) const;
};

// As a DRM format modifier and a fourcc are named in the kernel.
std::string modifierName(uint64_t modifier);
std::string fourccName(uint32_t fourcc);

} // namespace cutefish::reef::diagnose
