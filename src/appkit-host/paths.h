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

ui::Path pathOf(const reef_ui_path_element* elements, unsigned count, bool evenOdd);
base::AffineTransform transformOf(const double* matrix);

} // namespace cutefish::reef
