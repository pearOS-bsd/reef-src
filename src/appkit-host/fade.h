// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A picture above a window that fades in, drawn by a thread of its own.

#pragma once

#include "ui/subsurface.h"
#include "ui/window.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace cutefish::reef {

class FadeOverlay
{
public:
    // RGBA pixels. `elapsed` of the `duration`, in seconds, went by; the curve is a cubic Bezier one.
    FadeOverlay(ui::Window* window, const ui::RectF& rect, std::vector<unsigned char> pixels, int width, int height, double duration,
                double elapsed, const double* curve);
    ~FadeOverlay();

    bool isValid() const { return m_surface != nullptr; }
    void setPixels(std::vector<unsigned char> pixels);
    void putBelow();

private:
    double opacity() const;
    void run();

    std::unique_ptr<ui::Subsurface> m_surface;
    std::vector<unsigned char> m_pixels;
    int m_width;
    int m_height;
    double m_duration;
    double m_curve[4] = {};
    std::chrono::steady_clock::time_point m_start;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stopped = false;
    std::atomic<bool> m_pixelsChanged = true;
};

} // namespace cutefish::reef
