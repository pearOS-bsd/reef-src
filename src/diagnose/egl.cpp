// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the report tells of EGL and OpenGL.

#include "gl.h"
#include "sections.h"

#include <cstring>
#include <sstream>

namespace cutefish::reef::diagnose {

static const char* const notableExtensions[] = {
    "GL_ARB_shader_ballot", "GL_ARB_shader_group_vote", "GL_ARB_gpu_shader_int64", "GL_KHR_shader_subgroup", "GL_ARB_texture_view",
    "GL_EXT_EGL_image_storage", "GL_OES_EGL_image", "GL_ARB_bindless_texture", "GL_ARB_shader_viewport_layer_array",
    "GL_EXT_texture_compression_s3tc", "GL_ARB_texture_compression_bptc", "GL_KHR_texture_compression_astc_ldr",
    "GL_ARB_sparse_texture", "GL_ARB_fragment_shader_interlock", "GL_INTEL_fragment_shader_ordering",
    "GL_EXT_shader_framebuffer_fetch", "GL_ARB_timer_query",
};

static const struct { const char* name; GLenum value; } limits[] = {
    {"Max texture size", GL_MAX_TEXTURE_SIZE},
    {"Max 3D texture size", GL_MAX_3D_TEXTURE_SIZE},
    {"Max array layers", GL_MAX_ARRAY_TEXTURE_LAYERS},
    {"Max samples", GL_MAX_SAMPLES},
    {"Max compute invocations", GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS},
    {"Max compute shared memory", GL_MAX_COMPUTE_SHARED_MEMORY_SIZE},
    {"Max SSBO bindings", GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS},
    {"Max image units", GL_MAX_IMAGE_UNITS},
    {"Max texture units", GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS},
};

static const char* const importedFormats[] = {"AR24", "XR24", "AB24", "XB24", "AR30", "AB4H", "NV12", "P010"};

static bool listed(const char* extensions, const char* name)
{
    const size_t length = strlen(name);
    for (const char* at = strstr(extensions, name); at; at = strstr(at + 1, name)) {
        if ((at == extensions || at[-1] == ' ') && (at[length] == ' ' || !at[length]))
            return true;
    }
    return false;
}

const EglDisplay& eglDisplay()
{
    static EglDisplay display;
    static bool opened = false;
    if (opened)
        return display;
    opened = true;
    display.display = eglGetPlatformDisplay(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, nullptr);
    if (display.display == EGL_NO_DISPLAY) {
        display.error = format("no surfaceless EGL display (EGL error 0x%x)", eglGetError());
    } else if (!eglInitialize(display.display, nullptr, nullptr)) {
        display.error = format("the EGL display does not initialize (EGL error 0x%x)", eglGetError());
        display.display = EGL_NO_DISPLAY;
    } else if (!hasEglExtension("EGL_KHR_no_config_context") || !hasEglExtension("EGL_KHR_surfaceless_context")) {
        display.error = "the EGL driver cannot make contexts without a config or a surface, which reef needs";
    }
    return display;
}

bool hasEglExtension(const char* name)
{
    const EglDisplay& egl = eglDisplay();
    if (egl.display == EGL_NO_DISPLAY)
        return false;
    const char* extensions = eglQueryString(egl.display, EGL_EXTENSIONS);
    return extensions && listed(extensions, name);
}

bool Gl::hasExtension(const char* name) const
{
    GLint count = 0;
    getIntegerv(GL_NUM_EXTENSIONS, &count);
    for (GLint i = 0; i < count; i++) {
        const char* extension = reinterpret_cast<const char*>(getStringi(GL_EXTENSIONS, i));
        if (extension && strcmp(extension, name) == 0)
            return true;
    }
    return false;
}

EGLContext createContext(EGLContext shared)
{
    const EglDisplay& egl = eglDisplay();
    if (egl.display == EGL_NO_DISPLAY || !egl.error.empty())
        return EGL_NO_CONTEXT;
    eglBindAPI(EGL_OPENGL_API);
    const EGLint attributes[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3, EGL_CONTEXT_MINOR_VERSION, 2,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, EGL_TRUE, EGL_NONE,
    };
    return eglCreateContext(egl.display, EGL_NO_CONFIG_KHR, shared, attributes);
}

std::string Gl::load()
{
    std::string missing;
    auto load = [&missing](auto& function, const char* name) {
        function = reinterpret_cast<std::remove_reference_t<decltype(function)>>(eglGetProcAddress(name));
        if (!function)
            missing += (missing.empty() ? "" : " ") + std::string(name);
    };
    load(getString, "glGetString");
    load(getStringi, "glGetStringi");
    load(getIntegerv, "glGetIntegerv");
    load(getError, "glGetError");
    load(createTextures, "glCreateTextures");
    load(genTextures, "glGenTextures");
    load(bindTexture, "glBindTexture");
    load(texImage2D, "glTexImage2D");
    load(texParameteri, "glTexParameteri");
    load(textureStorage2D, "glTextureStorage2D");
    load(textureParameteri, "glTextureParameteri");
    load(textureView, "glTextureView");
    load(getTextureImage, "glGetTextureImage");
    load(deleteTextures, "glDeleteTextures");
    load(bindTextureUnit, "glBindTextureUnit");
    load(createFramebuffers, "glCreateFramebuffers");
    load(namedFramebufferTexture, "glNamedFramebufferTexture");
    load(checkNamedFramebufferStatus, "glCheckNamedFramebufferStatus");
    load(bindFramebuffer, "glBindFramebuffer");
    load(deleteFramebuffers, "glDeleteFramebuffers");
    load(viewport, "glViewport");
    load(readPixels, "glReadPixels");
    load(pixelStorei, "glPixelStorei");
    load(finish, "glFinish");
    load(flush, "glFlush");
    load(createShader, "glCreateShader");
    load(shaderSource, "glShaderSource");
    load(compileShader, "glCompileShader");
    load(getShaderiv, "glGetShaderiv");
    load(getShaderInfoLog, "glGetShaderInfoLog");
    load(deleteShader, "glDeleteShader");
    load(createProgram, "glCreateProgram");
    load(attachShader, "glAttachShader");
    load(linkProgram, "glLinkProgram");
    load(getProgramiv, "glGetProgramiv");
    load(useProgram, "glUseProgram");
    load(deleteProgram, "glDeleteProgram");
    load(getUniformLocation, "glGetUniformLocation");
    load(uniform4f, "glUniform4f");
    load(createVertexArrays, "glCreateVertexArrays");
    load(bindVertexArray, "glBindVertexArray");
    load(deleteVertexArrays, "glDeleteVertexArrays");
    load(drawArrays, "glDrawArrays");
    load(createBuffers, "glCreateBuffers");
    load(namedBufferStorage, "glNamedBufferStorage");
    load(clearNamedBufferData, "glClearNamedBufferData");
    load(getNamedBufferSubData, "glGetNamedBufferSubData");
    load(deleteBuffers, "glDeleteBuffers");
    imageTargetTexture2D = reinterpret_cast<ImageTargetTexture2D>(eglGetProcAddress("glEGLImageTargetTexture2DOES"));
    imageTargetTexStorage = reinterpret_cast<PFNGLEGLIMAGETARGETTEXSTORAGEEXTPROC>(eglGetProcAddress("glEGLImageTargetTexStorageEXT"));
    return missing;
}

std::string modifierName(uint64_t modifier)
{
    static const char* const vendors[] = {
        "NONE", "INTEL", "AMD", "NVIDIA", "SAMSUNG", "QCOM", "VIVANTE", "BROADCOM", "ARM", "ALLWINNER", "AMLOGIC",
    };
    static const char* const intel[] = {
        nullptr, "X_TILED", "Y_TILED", "Yf_TILED", "Y_TILED_CCS", "Yf_TILED_CCS", "Y_TILED_GEN12_RC_CCS", "Y_TILED_GEN12_MC_CCS",
        "Y_TILED_GEN12_RC_CCS_CC", "4_TILED", "4_TILED_DG2_RC_CCS", "4_TILED_DG2_MC_CCS", "4_TILED_DG2_RC_CCS_CC",
    };
    if (modifier == 0)
        return "LINEAR";
    if (modifier == 0xffffffffffffff)
        return "INVALID";
    const uint64_t vendor = modifier >> 56;
    const uint64_t value = modifier & 0xffffffffffffff;
    if (vendor == 1) {
        if (value < std::size(intel) && intel[value])
            return std::string("I915_") + intel[value];
        return format("I915_0x%llx", static_cast<unsigned long long>(value));
    }
    return format("%s_0x%llx", vendor < std::size(vendors) ? vendors[vendor] : "VENDOR", static_cast<unsigned long long>(value));
}

std::string fourccName(uint32_t fourcc)
{
    std::string name(4, '?');
    for (int i = 0; i < 4; i++) {
        const char c = fourcc >> (8 * i);
        if (c >= 0x20 && c < 0x7f)
            name[i] = c;
    }
    return trimmed(name);
}

// Words in lines of up to 100 columns.
static void addWords(Report& report, const std::vector<std::string>& words)
{
    std::string line = "  ";
    for (const std::string& word : words) {
        if (line.size() + word.size() + 1 > 100 && line.size() > 2) {
            report.line(line);
            line = "  ";
        }
        line += (line.size() > 2 ? " " : "") + word;
    }
    if (line.size() > 2)
        report.line(line);
}

static void addFormats(Report& report)
{
    const auto queryFormats = reinterpret_cast<PFNEGLQUERYDMABUFFORMATSEXTPROC>(eglGetProcAddress("eglQueryDmaBufFormatsEXT"));
    const auto queryModifiers = reinterpret_cast<PFNEGLQUERYDMABUFMODIFIERSEXTPROC>(eglGetProcAddress("eglQueryDmaBufModifiersEXT"));
    const EGLDisplay display = eglDisplay().display;
    if (!hasEglExtension("EGL_EXT_image_dma_buf_import_modifiers") || !queryFormats || !queryModifiers) {
        report.line("The driver lists no DMA-BUF modifiers (EGL_EXT_image_dma_buf_import_modifiers).");
        return;
    }
    EGLint count = 0;
    queryFormats(display, 0, nullptr, &count);
    std::vector<EGLint> formats(count > 0 ? count : 0);
    queryFormats(display, count, formats.data(), &count);
    std::vector<std::string> names;
    for (const EGLint fourcc : formats)
        names.push_back(fourccName(fourcc));
    report.line(format("DMA-BUF formats imported (%d):", count));
    addWords(report, names);
    for (const char* name : importedFormats) {
        const EGLint fourcc = name[0] | name[1] << 8 | name[2] << 16 | name[3] << 24;
        EGLint count = 0;
        if (!queryModifiers(display, fourcc, 0, nullptr, nullptr, &count))
            continue;
        std::vector<EGLuint64KHR> modifiers(count > 0 ? count : 0);
        std::vector<EGLBoolean> external(modifiers.size());
        queryModifiers(display, fourcc, count, modifiers.data(), external.data(), &count);
        std::vector<std::string> words;
        for (EGLint i = 0; i < count; i++)
            words.push_back(format("%s(0x%llx)%s", modifierName(modifiers[i]).c_str(), static_cast<unsigned long long>(modifiers[i]),
                                   external[i] ? "[external only]" : ""));
        report.line(format("Modifiers for %s:", name));
        if (words.empty())
            report.line("  none listed");
        else
            addWords(report, words);
    }
}

static std::string eglString(EGLDisplay display, EGLint name)
{
    const char* text = eglQueryString(display, name);
    return text ? text : "";
}

void addEgl(Report& report)
{
    report.section("EGL and OpenGL as reef uses them");
    const EglDisplay& egl = eglDisplay();
    if (egl.display == EGL_NO_DISPLAY) {
        report.result("FAIL", "egl", egl.error);
        return;
    }
    report.field("EGL vendor", eglString(egl.display, EGL_VENDOR));
    report.field("EGL version", eglString(egl.display, EGL_VERSION));
    report.field("EGL client APIs", eglString(egl.display, EGL_CLIENT_APIS));
    const auto queryDisplayAttribute = reinterpret_cast<PFNEGLQUERYDISPLAYATTRIBEXTPROC>(eglGetProcAddress("eglQueryDisplayAttribEXT"));
    const auto queryDeviceString = reinterpret_cast<PFNEGLQUERYDEVICESTRINGEXTPROC>(eglGetProcAddress("eglQueryDeviceStringEXT"));
    EGLAttrib device = 0;
    if (queryDisplayAttribute && queryDeviceString && queryDisplayAttribute(eglDisplay().display, EGL_DEVICE_EXT, &device) && device) {
        EGLDeviceEXT handle = reinterpret_cast<EGLDeviceEXT>(device);
        const char* extensions = queryDeviceString(handle, EGL_EXTENSIONS);
        auto has = [extensions](const char* name) { return extensions && listed(extensions, name); };
        auto string = [&](EGLint name) -> std::string {
            const char* text = queryDeviceString(handle, name);
            return text ? text : "not reported";
        };
        report.field("EGL device extensions", extensions ? extensions : "");
        if (has("EGL_EXT_device_drm"))
            report.field("EGL device node", string(EGL_DRM_DEVICE_FILE_EXT));
        if (has("EGL_EXT_device_drm_render_node"))
            report.field("EGL render node", string(EGL_DRM_RENDER_NODE_FILE_EXT));
        if (has("EGL_MESA_device_software"))
            report.field("EGL device", "software rendering (llvmpipe), not the GPU");
    } else {
        report.field("EGL device", "not reported");
    }
    report.line("EGL extensions:");
    std::istringstream extensions(eglString(egl.display, EGL_EXTENSIONS));
    std::vector<std::string> words;
    std::string word;
    while (extensions >> word)
        words.push_back(word);
    addWords(report, words);
    if (!egl.error.empty()) {
        report.result("FAIL", "egl", egl.error);
        return;
    }
    const EGLContext context = createContext(EGL_NO_CONTEXT);
    if (context == EGL_NO_CONTEXT || !eglMakeCurrent(egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
        report.result("FAIL", "egl", format("no core OpenGL context (EGL error 0x%x)", eglGetError()));
        if (context != EGL_NO_CONTEXT)
            eglDestroyContext(egl.display, context);
        return;
    }
    Gl gl;
    const std::string missing = gl.load();
    auto string = [&gl](GLenum name) -> std::string {
        const char* text = gl.getString ? reinterpret_cast<const char*>(gl.getString(name)) : nullptr;
        return text ? text : "";
    };
    report.field("GL vendor", string(GL_VENDOR));
    report.field("GL renderer", string(GL_RENDERER));
    report.field("GL version", string(GL_VERSION));
    report.field("GLSL version", string(GL_SHADING_LANGUAGE_VERSION));
    if (!missing.empty())
        report.result("FAIL", "egl", "the driver lacks OpenGL entry points reef uses: " + missing);
    if (gl.getIntegerv) {
        GLint major = 0, minor = 0;
        gl.getIntegerv(GL_MAJOR_VERSION, &major);
        gl.getIntegerv(GL_MINOR_VERSION, &minor);
        if (major < 4 || (major == 4 && minor < 5))
            report.result("FAIL", "egl", format("OpenGL %d.%d; reef's Metal needs 4.5", major, minor));
        while (gl.getError()) {
        }
        for (const auto& limit : limits) {
            GLint value = 0;
            gl.getIntegerv(limit.value, &value);
            report.field(limit.name, std::to_string(value));
        }
        if (gl.hasExtension("GL_KHR_shader_subgroup")) {
            GLint size = 0;
            gl.getIntegerv(GL_SUBGROUP_SIZE_KHR, &size);
            report.field("Subgroup size", std::to_string(size));
        }
        std::string present, absent;
        for (const char* name : notableExtensions)
            (gl.hasExtension(name) ? present : absent) += std::string(name) + " ";
        report.field("Extensions of note", trimmed(present));
        report.field("Extensions of note missing", absent.empty() ? "none" : trimmed(absent));
        GLint count = 0;
        gl.getIntegerv(GL_NUM_EXTENSIONS, &count);
        std::vector<std::string> names;
        for (GLint i = 0; i < count; i++)
            names.push_back(reinterpret_cast<const char*>(gl.getStringi(GL_EXTENSIONS, i)));
        report.line(format("OpenGL extensions (%d):", count));
        addWords(report, names);
    }
    addFormats(report);
    eglMakeCurrent(egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(egl.display, context);
}

} // namespace cutefish::reef::diagnose
