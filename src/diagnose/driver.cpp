// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Checks of the OpenGL driver without reef: textures another process reads, and what new memory has in it.

#include "gl.h"
#include "sections.h"

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>

namespace cutefish::reef::diagnose {

static constexpr int exportSize = 256;
// Colours as glReadPixels gives them.
static constexpr uint32_t firstColor = 0xff2040c0;
static constexpr uint32_t secondColor = 0xff60e010;

static PFNEGLCREATEIMAGEKHRPROC createImageFunction()
{
    static const auto function = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(eglGetProcAddress("eglCreateImageKHR"));
    return function;
}

static EGLImageKHR createImage(EGLDisplay display, EGLContext context, EGLenum target, EGLClientBuffer buffer, const EGLint* attributes)
{
    const auto create = createImageFunction();
    return create ? create(display, context, target, buffer, attributes) : EGL_NO_IMAGE_KHR;
}

static void destroyImage(EGLDisplay display, EGLImageKHR image)
{
    static const auto destroy = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(eglGetProcAddress("eglDestroyImageKHR"));
    if (destroy)
        destroy(display, image);
}

namespace {

struct Pixels
{
    size_t wrong = 0;
    long first = -1;
    uint32_t value = 0;
};

// A texture exported as DMA-BUF.
struct Exported
{
    bool done = false;
    int planes = 0;
    int descriptors[4] = {-1, -1, -1, -1};
    EGLint pitches[4] = {};
    EGLint offsets[4] = {};
    int fourcc = 0;
    EGLuint64KHR modifier = 0;

    void close()
    {
        for (int& descriptor : descriptors) {
            if (descriptor >= 0)
                ::close(descriptor);
            descriptor = -1;
        }
    }
};

// A context of plain OpenGL.
struct Driver : Gl
{
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLContext context = EGL_NO_CONTEXT;
    GLuint colorProgram = 0;
    GLuint copyProgram = 0;
    GLuint vertexArray = 0;

    ~Driver()
    {
        if (context == EGL_NO_CONTEXT)
            return;
        if (deleteProgram) {
            deleteProgram(colorProgram);
            deleteProgram(copyProgram);
            deleteVertexArrays(1, &vertexArray);
        }
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(display, context);
    }

    bool open(std::string& error);
    GLuint texture(int width, int height);
    void fill(GLuint texture, int width, int height, uint32_t color);
    void copy(GLuint from, GLuint to);
    bool read(GLuint texture, int width, int height, std::vector<uint32_t>& pixels);
    Pixels compare(GLuint texture, uint32_t color);
    Exported exportTexture(GLuint texture);

private:
    GLuint program(const char* fragment, std::string& error);
};

} // namespace

static Pixels wrongPixels(const std::vector<uint32_t>& pixels, uint32_t color)
{
    Pixels result;
    for (size_t i = 0; i < pixels.size(); i++) {
        if (pixels[i] == color)
            continue;
        if (result.first == -1) {
            result.first = i;
            result.value = pixels[i];
        }
        result.wrong++;
    }
    return result;
}

static std::string describe(const Pixels& pixels, int width, uint32_t color)
{
    if (pixels.wrong == 0)
        return "right";
    return format("%zu pixels wrong, first at x %ld y %ld: RGBA %08x, expected %08x", pixels.wrong, pixels.first % width,
                  pixels.first / width, __builtin_bswap32(pixels.value), __builtin_bswap32(color));
}

GLuint Driver::texture(int width, int height)
{
    GLuint texture = 0;
    createTextures(GL_TEXTURE_2D, 1, &texture);
    textureStorage2D(texture, 1, GL_RGBA8, width, height);
    textureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    textureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return texture;
}

void Driver::fill(GLuint texture, int width, int height, uint32_t color)
{
    GLuint framebuffer = 0;
    createFramebuffers(1, &framebuffer);
    namedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texture, 0);
    bindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    viewport(0, 0, width, height);
    useProgram(colorProgram);
    uniform4f(getUniformLocation(colorProgram, "color"), (color & 0xff) / 255.0f, (color >> 8 & 0xff) / 255.0f,
              (color >> 16 & 0xff) / 255.0f, 1.0f);
    drawArrays(GL_TRIANGLES, 0, 3);
    bindFramebuffer(GL_FRAMEBUFFER, 0);
    deleteFramebuffers(1, &framebuffer);
}

void Driver::copy(GLuint from, GLuint to)
{
    GLuint framebuffer = 0;
    createFramebuffers(1, &framebuffer);
    namedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, to, 0);
    bindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    viewport(0, 0, exportSize, exportSize);
    useProgram(copyProgram);
    bindTextureUnit(0, from);
    drawArrays(GL_TRIANGLES, 0, 3);
    bindTextureUnit(0, 0);
    bindFramebuffer(GL_FRAMEBUFFER, 0);
    deleteFramebuffers(1, &framebuffer);
}

bool Driver::read(GLuint texture, int width, int height, std::vector<uint32_t>& pixels)
{
    pixels.assign(size_t(width) * height, 0);
    GLuint framebuffer = 0;
    createFramebuffers(1, &framebuffer);
    namedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texture, 0);
    const bool complete = checkNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if (complete) {
        bindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        readPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        bindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    deleteFramebuffers(1, &framebuffer);
    return complete;
}

Pixels Driver::compare(GLuint texture, uint32_t color)
{
    std::vector<uint32_t> pixels;
    if (!read(texture, exportSize, exportSize, pixels))
        return {exportSize * exportSize, 0, 0};
    return wrongPixels(pixels, color);
}

Exported Driver::exportTexture(GLuint texture)
{
    static const auto query = reinterpret_cast<PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC>(eglGetProcAddress("eglExportDMABUFImageQueryMESA"));
    static const auto exportImage = reinterpret_cast<PFNEGLEXPORTDMABUFIMAGEMESAPROC>(eglGetProcAddress("eglExportDMABUFImageMESA"));
    Exported exported;
    if (!query || !exportImage)
        return exported;
    const EGLint attributes[] = {EGL_GL_TEXTURE_LEVEL_KHR, 0, EGL_NONE};
    const EGLImageKHR image = createImage(display, context, EGL_GL_TEXTURE_2D_KHR, reinterpret_cast<EGLClientBuffer>(uintptr_t(texture)),
                                          attributes);
    if (image == EGL_NO_IMAGE_KHR)
        return exported;
    EGLuint64KHR modifiers[4] = {};
    exported.done = query(display, image, &exported.fourcc, &exported.planes, modifiers) && exported.planes >= 1 && exported.planes <= 4
        && exportImage(display, image, exported.descriptors, exported.pitches, exported.offsets);
    exported.modifier = modifiers[0];
    destroyImage(display, image);
    flush();
    if (!exported.done)
        exported.close();
    return exported;
}

static std::string describe(const Exported& exported)
{
    if (!exported.done)
        return "not exported";
    return format("%s, %d plane%s, modifier %s (0x%llx), pitch %d", fourccName(exported.fourcc).c_str(), exported.planes,
                  exported.planes == 1 ? "" : "s", modifierName(exported.modifier).c_str(),
                  static_cast<unsigned long long>(exported.modifier), exported.pitches[0]);
}

GLuint Driver::program(const char* fragment, std::string& error)
{
    auto compile = [&](GLenum type, const char* source) {
        const GLuint shader = createShader(type);
        shaderSource(shader, 1, &source, nullptr);
        compileShader(shader);
        GLint compiled = 0;
        getShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            char log[512] = {};
            getShaderInfoLog(shader, sizeof(log) - 1, nullptr, log);
            error = std::string("a shader does not compile: ") + log;
        }
        return shader;
    };
    const GLuint vertexShader = compile(GL_VERTEX_SHADER, "#version 330 core\n"
                                        "void main() {\n"
                                        "    vec2 uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);\n"
                                        "    gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);\n"
                                        "}\n");
    const GLuint fragmentShader = compile(GL_FRAGMENT_SHADER, fragment);
    const GLuint program = createProgram();
    attachShader(program, vertexShader);
    attachShader(program, fragmentShader);
    linkProgram(program);
    deleteShader(vertexShader);
    deleteShader(fragmentShader);
    GLint linked = 0;
    getProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked && error.empty())
        error = "a program does not link";
    return program;
}

// A process of this program that imports the texture and reads it.
static CommandResult runImport(const Exported& exported)
{
    char self[4096];
    const ssize_t length = readlink("/proc/self/exe", self, sizeof(self) - 1);
    if (length <= 0)
        return {};
    self[length] = 0;
    std::vector<std::string> arguments = {
        self, "--import-check", "256", "256", std::to_string(exported.fourcc), std::to_string(exported.modifier),
        format("%u", secondColor), std::to_string(exported.planes),
    };
    for (int i = 0; i < exported.planes; i++) {
        fcntl(exported.descriptors[i], F_SETFD, 0);
        arguments.push_back(std::to_string(exported.descriptors[i]));
        arguments.push_back(std::to_string(exported.offsets[i]));
        arguments.push_back(std::to_string(exported.pitches[i]));
    }
    const CommandResult result = run(arguments, 30);
    for (int i = 0; i < exported.planes; i++)
        fcntl(exported.descriptors[i], F_SETFD, FD_CLOEXEC);
    return result;
}

bool Driver::open(std::string& error)
{
    const EglDisplay& egl = eglDisplay();
    if (egl.display == EGL_NO_DISPLAY || !egl.error.empty()) {
        error = egl.error;
        return false;
    }
    display = egl.display;
    context = createContext(EGL_NO_CONTEXT);
    if (context == EGL_NO_CONTEXT || !eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
        error = format("no core OpenGL context (EGL error 0x%x)", eglGetError());
        return false;
    }
    const std::string missing = load();
    if (!missing.empty()) {
        error = "missing OpenGL entry points: " + missing;
        return false;
    }
    colorProgram = program("#version 330 core\n"
                           "uniform vec4 color;\n"
                           "out vec4 result;\n"
                           "void main() { result = color; }\n", error);
    copyProgram = program("#version 330 core\n"
                          "uniform sampler2D image;\n"
                          "out vec4 result;\n"
                          "void main() { result = texelFetch(image, ivec2(gl_FragCoord.xy), 0); }\n", error);
    createVertexArrays(1, &vertexArray);
    bindVertexArray(vertexArray);
    pixelStorei(GL_PACK_ALIGNMENT, 1);
    return error.empty();
}

int importCheck(int argc, char** argv)
{
    if (argc < 9)
        return 2;
    const int width = strtol(argv[2], nullptr, 10);
    const int height = strtol(argv[3], nullptr, 10);
    const uint32_t fourcc = strtoul(argv[4], nullptr, 10);
    const uint64_t modifier = strtoull(argv[5], nullptr, 10);
    const uint32_t color = strtoul(argv[6], nullptr, 10);
    const int planes = strtol(argv[7], nullptr, 10);
    if (planes < 1 || planes > 4 || argc < 8 + planes * 3)
        return 2;
    Driver driver;
    std::string error;
    if (!driver.open(error)) {
        printf("%s\n", error.c_str());
        return 1;
    }
    static const EGLint names[4][5] = {
        {EGL_DMA_BUF_PLANE0_FD_EXT, EGL_DMA_BUF_PLANE0_OFFSET_EXT, EGL_DMA_BUF_PLANE0_PITCH_EXT,
         EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT, EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT},
        {EGL_DMA_BUF_PLANE1_FD_EXT, EGL_DMA_BUF_PLANE1_OFFSET_EXT, EGL_DMA_BUF_PLANE1_PITCH_EXT,
         EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT, EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT},
        {EGL_DMA_BUF_PLANE2_FD_EXT, EGL_DMA_BUF_PLANE2_OFFSET_EXT, EGL_DMA_BUF_PLANE2_PITCH_EXT,
         EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT, EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT},
        {EGL_DMA_BUF_PLANE3_FD_EXT, EGL_DMA_BUF_PLANE3_OFFSET_EXT, EGL_DMA_BUF_PLANE3_PITCH_EXT,
         EGL_DMA_BUF_PLANE3_MODIFIER_LO_EXT, EGL_DMA_BUF_PLANE3_MODIFIER_HI_EXT},
    };
    std::vector<EGLint> attributes = {EGL_WIDTH, width, EGL_HEIGHT, height, EGL_LINUX_DRM_FOURCC_EXT, EGLint(fourcc)};
    for (int i = 0; i < planes; i++) {
        char** const values = argv + 8 + i * 3;
        attributes.insert(attributes.end(), {
            names[i][0], EGLint(strtol(values[0], nullptr, 10)), names[i][1], EGLint(strtol(values[1], nullptr, 10)),
            names[i][2], EGLint(strtol(values[2], nullptr, 10)), names[i][3], EGLint(modifier), names[i][4], EGLint(modifier >> 32),
        });
    }
    attributes.push_back(EGL_NONE);
    const EGLImageKHR image = createImage(driver.display, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr, attributes.data());
    if (image == EGL_NO_IMAGE_KHR) {
        printf("READ not imported (EGL error 0x%x)\n", eglGetError());
        return 0;
    }
    while (driver.getError()) {
    }
    GLuint texture = 0;
    driver.genTextures(1, &texture);
    driver.bindTexture(GL_TEXTURE_2D, texture);
    bool bound = false;
    if (driver.imageTargetTexStorage) {
        driver.imageTargetTexStorage(GL_TEXTURE_2D, image, nullptr);
        bound = driver.getError() == 0;
    }
    if (!bound) {
        driver.deleteTextures(1, &texture);
        driver.genTextures(1, &texture);
        driver.bindTexture(GL_TEXTURE_2D, texture);
        driver.imageTargetTexture2D(GL_TEXTURE_2D, image);
    }
    driver.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    driver.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    driver.bindTexture(GL_TEXTURE_2D, 0);
    std::vector<uint32_t> pixels;
    if (!driver.read(texture, width, height, pixels))
        printf("READ not readable (GL error 0x%x)\n", driver.getError());
    else
        printf("READ %s\n", describe(wrongPixels(pixels, color), width, color).c_str());

    // Metal makes views of textures other processes drew.
    while (driver.getError()) {
    }
    GLuint view = 0;
    driver.genTextures(1, &view);
    driver.textureView(view, GL_TEXTURE_2D, texture, GL_RGBA8, 0, 1, 0, 1);
    const GLenum mutableError = driver.getError();
    driver.deleteTextures(1, &view);
    GLenum storageError = GL_INVALID_OPERATION;
    GLenum viewError = GL_INVALID_OPERATION;
    if (driver.imageTargetTexStorage) {
        GLuint immutable = 0;
        driver.genTextures(1, &immutable);
        driver.bindTexture(GL_TEXTURE_2D, immutable);
        driver.imageTargetTexStorage(GL_TEXTURE_2D, image, nullptr);
        driver.bindTexture(GL_TEXTURE_2D, 0);
        storageError = driver.getError();
        if (storageError == 0) {
            driver.genTextures(1, &view);
            driver.textureView(view, GL_TEXTURE_2D, immutable, GL_RGBA8, 0, 1, 0, 1);
            viewError = driver.getError();
            driver.deleteTextures(1, &view);
        }
        driver.deleteTextures(1, &immutable);
    }
    const std::string mutableViews = mutableError ? format("views of mutable imports fail (GL error 0x%x)", mutableError)
                                                  : "views of mutable imports work";
    if (storageError) {
        printf("VIEW %s driver.import.view: the driver takes no immutable imports (%s); %s\n", mutableError ? "FAIL" : "PASS",
               driver.imageTargetTexStorage ? format("GL error 0x%x", storageError).c_str() : "no glEGLImageTargetTexStorageEXT",
               mutableViews.c_str());
    } else if (viewError) {
        printf("VIEW FAIL driver.import.view: views of immutable imports fail (GL error 0x%x); %s\n", viewError, mutableViews.c_str());
    } else {
        printf("VIEW PASS driver.import.view: views of immutable imports work; %s\n", mutableViews.c_str());
    }
    driver.deleteTextures(1, &texture);
    destroyImage(driver.display, image);
    return 0;
}

static void addImport(Report& report, const std::string& check, const CommandResult& result, const std::string& drawn, bool views)
{
    if (!result.started || result.timedOut || result.status != 0) {
        report.result("FAIL", check, drawn + ": the importing process failed (" + trimmed(result.output) + ")");
        return;
    }
    std::istringstream lines(result.output);
    std::string line;
    while (std::getline(lines, line)) {
        if (line.starts_with("READ "))
            report.result(line.substr(5).starts_with("right") ? "PASS" : "FAIL", check,
                          drawn + ", another process read it: " + line.substr(5));
        else if (views && line.starts_with("VIEW "))
            report.result(line.substr(5));
    }
}

static void addExports(Report& report, Driver& driver)
{
    const GLuint target = driver.texture(exportSize, exportSize);
    const GLuint used = driver.texture(exportSize, exportSize);
    driver.fill(used, exportSize, exportSize, firstColor);
    driver.copy(used, target);
    driver.finish();
    Exported afterUse = driver.exportTexture(used);
    driver.fill(used, exportSize, exportSize, secondColor);
    driver.finish();
    const Pixels readBack = driver.compare(used, secondColor);
    driver.copy(used, target);
    const Pixels sampled = driver.compare(target, secondColor);
    const std::string check = "driver.export.after-use";
    report.result("INFO", check, "drawn and sampled, then exported: " + describe(afterUse));
    if (!afterUse.done) {
        report.result("FAIL", check, "the driver does not export a texture it drew as a DMA-BUF");
    } else {
        report.result(readBack.wrong ? "FAIL" : "PASS", check,
                      "drawn again after the export, read back here: " + describe(readBack, exportSize, secondColor));
        report.result(sampled.wrong ? "FAIL" : "PASS", check,
                      "drawn again after the export, sampled here: " + describe(sampled, exportSize, secondColor));
        addImport(report, check, runImport(afterUse), "drawn again after the export", true);
    }
    afterUse.close();

    const GLuint unused = driver.texture(exportSize, exportSize);
    Exported beforeUse = driver.exportTexture(unused);
    driver.fill(unused, exportSize, exportSize, secondColor);
    driver.finish();
    report.result("INFO", "driver.export.before-use", "exported before any use: " + describe(beforeUse));
    if (beforeUse.done) {
        const Pixels pixels = driver.compare(unused, secondColor);
        report.result(pixels.wrong ? "FAIL" : "PASS", "driver.export.before-use",
                      "drawn after the export, read back here: " + describe(pixels, exportSize, secondColor));
        addImport(report, "driver.export.before-use", runImport(beforeUse), "drawn after the export", false);
    }
    beforeUse.close();
    driver.deleteTextures(1, &target);
    driver.deleteTextures(1, &used);
    driver.deleteTextures(1, &unused);
}

// New textures after others of the same size, drawn in one colour, were deleted.
static void addTextureMemory(Report& report, Driver& driver, bool storage)
{
    constexpr int count = 48;
    constexpr int size = 512;
    constexpr uint32_t oldColor = 0xffff00ff;
    const std::string check = storage ? "driver.memory.textures" : "driver.memory.textures-teximage";
    std::vector<GLuint> textures(count);
    for (GLuint& texture : textures) {
        texture = driver.texture(size, size);
        driver.fill(texture, size, size, oldColor);
    }
    driver.finish();
    driver.deleteTextures(count, textures.data());
    driver.finish();
    std::vector<uint32_t> pixels(size * size);
    size_t old = 0, other = 0;
    for (int i = 0; i < count; i++) {
        GLuint texture = 0;
        if (storage) {
            texture = driver.texture(size, size);
        } else {
            driver.genTextures(1, &texture);
            driver.bindTexture(GL_TEXTURE_2D, texture);
            driver.texImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            driver.bindTexture(GL_TEXTURE_2D, 0);
        }
        driver.getTextureImage(texture, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.size() * 4, pixels.data());
        for (const uint32_t pixel : pixels) {
            if (pixel == oldColor)
                old++;
            else if (pixel)
                other++;
        }
        driver.deleteTextures(1, &texture);
    }
    if (old + other == 0)
        report.result("PASS", check, format("%d new %dx%d textures read back as zero", count, size, size));
    else
        report.result("WARN", check, format("%d new %dx%d textures: %zu of %zu pixels not zero, %zu with freed textures' old colour, %zu other",
                                            count, size, size, old + other, pixels.size() * count, old, other));
}

static void addBufferMemory(Report& report, Driver& driver)
{
    constexpr int count = 48;
    constexpr int size = 1024 * 1024;
    std::vector<GLuint> buffers(count);
    driver.createBuffers(count, buffers.data());
    const uint8_t oldValue = 0xab;
    for (const GLuint buffer : buffers) {
        driver.namedBufferStorage(buffer, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
        driver.clearNamedBufferData(buffer, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &oldValue);
    }
    driver.finish();
    driver.deleteBuffers(count, buffers.data());
    driver.finish();
    std::vector<uint8_t> bytes(size);
    size_t old = 0, other = 0;
    for (int i = 0; i < count; i++) {
        GLuint buffer = 0;
        driver.createBuffers(1, &buffer);
        driver.namedBufferStorage(buffer, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
        driver.getNamedBufferSubData(buffer, 0, size, bytes.data());
        for (const uint8_t byte : bytes) {
            if (byte == oldValue)
                old++;
            else if (byte)
                other++;
        }
        driver.deleteBuffers(1, &buffer);
    }
    if (old + other == 0)
        report.result("PASS", "driver.memory.buffers", format("%d new 1 MB buffers read back as zero", count));
    else
        report.result("WARN", "driver.memory.buffers", format("%d new 1 MB buffers: %zu bytes with freed buffers' old value, %zu other",
                                                              count, old, other));
}

void addDriver(Report& report)
{
    report.section("Driver checks, plain OpenGL");
    report.line("The driver on its own, without reef. Memory that is not zero here is not a fault of the driver,");
    report.line("which need not clear it; reef must clear what programs expect to be clear.");
    Driver driver;
    std::string error;
    if (!driver.open(error)) {
        report.result("FAIL", "driver", error);
        return;
    }
    addExports(report, driver);
    for (const bool storage : {true, false})
        addTextureMemory(report, driver, storage);
    addBufferMemory(report, driver);
}

} // namespace cutefish::reef::diagnose
