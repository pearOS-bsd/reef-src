// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The drawing context of programs: on a canvas of the toolkit, or into a PDF document.

#pragma once

#include "host.h"
#include "pdf.h"

#include "ui/bitmap.h"
#include "ui/canvas_2d.h"

#include <memory>
#include <vector>

namespace cutefish::reef {

struct GcState
{
    int composite = 0;
    // Offset, blur and color.
    double shadow[7] = {};
    bool antialias = true;
    ui::Canvas2D::Smoothing smoothing = ui::Canvas2D::Smoothing::Medium;
    ui::LineCap lineCap = ui::LineCap::Butt;
    ui::LineJoin lineJoin = ui::LineJoin::Miter;
    double miterLimit = 10;
    double dashOffset = 0;
    std::vector<double> dashes;
    // Begun since the last save.
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
