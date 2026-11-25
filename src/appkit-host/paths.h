// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Paths as programs give them.

#pragma once

#include "base/transform.h"
#include "ui/path.h"

struct reef_ui_path_element
{
    // 0 move, 1 line, 2 quadratic, 3 cubic, 4 close.
    int type;
    double points[6];
};

namespace cutefish::reef {

ui::Path pathOf(const reef_ui_path_element* elements, unsigned count, int evenOdd);
base::AffineTransform transformOf(const double* matrix);

// 1 round, 2 square, otherwise butt.
inline ui::LineCap lineCapOf(int cap)
{
    return cap == 1 ? ui::LineCap::Round : cap == 2 ? ui::LineCap::Square : ui::LineCap::Butt;
}

// 1 round, 2 bevel, otherwise miter.
inline ui::LineJoin lineJoinOf(int join)
{
    return join == 1 ? ui::LineJoin::Round : join == 2 ? ui::LineJoin::Bevel : ui::LineJoin::Miter;
}

} // namespace cutefish::reef
