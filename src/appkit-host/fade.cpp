// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "fade.h"

#include <GLES3/gl3.h>
#include <algorithm>

namespace cutefish::reef {

using namespace std::chrono_literals;

FadeOverlay::FadeOverlay(ui::Window* window, const ui::RectF& rect, std::vector<unsigned char> pixels, int width, int height,
                         double duration, double elapsed, const double* curve)
    : m_pixels(std::move(pixels))
    , m_width(width)
    , m_height(height)
    , m_duration(duration)
{
    std::copy(curve, curve + 4, m_curve);
    m_start = std::chrono::steady_clock::now() - std::chrono::nanoseconds(int64_t(elapsed * 1e9));
    auto surface = std::make_unique<ui::Subsurface>(window, ui::Subsurface::Stacking::Above);
    if (!surface->isValid())
        return;
    surface->setOpaque(false);
    surface->setGeometry(rect);
    m_surface = std::move(surface);
    m_thread = std::thread([this] { run(); });
}

FadeOverlay::~FadeOverlay()
{
    if (!m_surface)
        return;
    {
        std::lock_guard lock(m_mutex);
        m_stopped = true;
    }
    m_condition.notify_all();
    m_surface->wakeUp();
    m_thread.join();
}

void FadeOverlay::setPixels(std::vector<unsigned char> pixels)
{
    if (!m_surface || pixels.size() != m_pixels.size())
        return;
    {
        std::lock_guard lock(m_mutex);
        m_pixels = std::move(pixels);
        m_pixelsChanged = true;
    }
    m_condition.notify_all();
}

void FadeOverlay::putBelow()
{
    if (m_surface)
        m_surface->setStacking(ui::Subsurface::Stacking::Below);
}

double FadeOverlay::opacity() const
{
    const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - m_start;
    const double progress = elapsed.count() / m_duration;
    if (progress <= 0)
        return 0;
    if (progress >= 1)
        return 1;
    auto bezier = [](double first, double second, double t) {
        const double rest = 1 - t;
        return 3 * rest * rest * t * first + 3 * rest * t * t * second + t * t * t;
    };
    double low = 0, high = 1;
    for (int i = 0; i < 32; i++) {
        const double middle = (low + high) * 0.5;
        if (bezier(m_curve[0], m_curve[2], middle) < progress)
            low = middle;
        else
            high = middle;
    }
    return bezier(m_curve[1], m_curve[3], (low + high) * 0.5);
}

static GLuint compile(GLenum type, const char* source)
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

void FadeOverlay::run()
{
    GLuint texture = 0, vertexArray = 0, program = 0;
    GLint opacityUniform = -1;
    bool done = false;
    while (!m_stopped) {
        done = done && !m_pixelsChanged;
        if (done) {
            std::unique_lock lock(m_mutex);
            m_condition.wait_for(lock, 100ms, [this] { return m_stopped || m_pixelsChanged; });
            continue;
        }
        const base::Size size = m_surface->beginFrame();
        if (size.width() <= 0 || size.height() <= 0) {
            std::unique_lock lock(m_mutex);
            m_condition.wait_for(lock, 2ms, [this] { return m_stopped.load(); });
            continue;
        }
        if (!program) {
            const GLuint vertexShader = compile(GL_VERTEX_SHADER, "#version 300 es\n"
                                                "out vec2 at;\n"
                                                "void main() {\n"
                                                "    vec2 corner = vec2(float(gl_VertexID & 1), float(gl_VertexID >> 1));\n"
                                                "    at = vec2(corner.x, 1.0 - corner.y);\n"
                                                "    gl_Position = vec4(corner * 2.0 - 1.0, 0.0, 1.0);\n"
                                                "}\n");
            const GLuint fragmentShader = compile(GL_FRAGMENT_SHADER, "#version 300 es\n"
                                                  "precision mediump float;\n"
                                                  "uniform sampler2D picture;\n"
                                                  "uniform float opacity;\n"
                                                  "in vec2 at;\n"
                                                  "out vec4 color;\n"
                                                  "void main() { color = texture(picture, at) * opacity; }\n");
            program = glCreateProgram();
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            glLinkProgram(program);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            opacityUniform = glGetUniformLocation(program, "opacity");
            glGenVertexArrays(1, &vertexArray);
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }
        if (m_pixelsChanged) {
            std::lock_guard lock(m_mutex);
            m_pixelsChanged = false;
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels.data());
        }
        const double opacity = this->opacity();
        glViewport(0, 0, size.width(), size.height());
        glDisable(GL_BLEND);
        glUseProgram(program);
        glUniform1f(opacityUniform, float(opacity));
        glBindVertexArray(vertexArray);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        m_surface->endFrame();
        if (opacity >= 1)
            done = true;
        else
            m_surface->waitForFrame(100);
    }
    if (program) {
        m_surface->makeCurrent();
        glDeleteTextures(1, &texture);
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteProgram(program);
    }
    m_surface->doneCurrent();
}

} // namespace cutefish::reef
