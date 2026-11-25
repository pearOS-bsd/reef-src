// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The drawing context of programs: on a canvas of the toolkit, or into a PDF document.

#pragma once

#include "pdf.h"

#include "ui/bitmap.h"
#include "ui/canvas_2d.h"

#include <memory>
#include <vector>

namespace cutefish::reef {

struct GcState
{
    int composite = 0;
    ui::PointF shadowOffset;
    double shadowBlur = 0;
    ui::ColorF shadowColor = {0, 0, 0, 0};
    bool antialias = true;
    int smoothing = 2;
    int lineCap = 0;
    int lineJoin = 0;
    double miterLimit = 10;
    double dashOffset = 0;
    std::vector<double> dashes;
    int groups = 0;
};

} // namespace cutefish::reef

struct reef_ui_gc
{
    std::unique_ptr<cutefish::ui::Canvas2D> canvas;
    // Of a context of its own.
    cutefish::ui::Bitmap bitmap;
    cutefish::reef::GcState state;
    std::vector<cutefish::reef::GcState> saved;
    std::unique_ptr<cutefish::reef::PdfWriter> pdf;
    // Device rectangles that nothing draws into.
    std::vector<cutefish::ui::RectF> excluded;
};
