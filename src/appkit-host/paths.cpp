// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "paths.h"

namespace cutefish::reef {

// How many elements it has; as many as fit are written.
static unsigned writePath(const ui::Path& path, reef_ui_path_element* out, unsigned capacity)
{
    const std::vector<ui::Path::Element> elements = path.elements();
    unsigned count = 0;
    for (size_t i = 0; i < elements.size(); i++) {
        const ui::Path::Element& element = elements[i];
        reef_ui_path_element written = {};
        switch (element.type) {
        case ui::Path::Element::Type::MoveTo:
        case ui::Path::Element::Type::LineTo:
            written.type = int(element.type);
            written.points[0] = element.x;
            written.points[1] = element.y;
            break;
        case ui::Path::Element::Type::CurveTo:
            if (i + 2 >= elements.size())
                continue;
            written = {3, {element.x, element.y, elements[i + 1].x, elements[i + 1].y, elements[i + 2].x, elements[i + 2].y}};
            i += 2;
            break;
        default:
            continue;
        }
        if (out && count < capacity)
            out[count] = written;
        count++;
    }
    return count;
}

base::AffineTransform transformOf(const double* matrix)
{
    return {matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]};
}

ui::Path pathOf(const reef_ui_path_element* elements, unsigned count, int evenOdd)
{
    ui::Path path;
    for (unsigned i = 0; i < count; i++) {
        const double* points = elements[i].points;
        switch (elements[i].type) {
        case 0:
            path.moveTo({points[0], points[1]});
            break;
        case 1:
            path.lineTo({points[0], points[1]});
            break;
        case 2:
            path.quadTo({points[0], points[1]}, {points[2], points[3]});
            break;
        case 3:
            path.cubicTo({points[0], points[1]}, {points[2], points[3]}, {points[4], points[5]});
            break;
        case 4:
            path.close();
            break;
        }
    }
    path.setFillRule(evenOdd & 1 ? ui::Path::FillRule::EvenOdd : ui::Path::FillRule::Winding);
    return path;
}

// 166
// The area a pen covers along the path.
unsigned hostPathStroked(const reef_ui_path_element* elements, unsigned count, double width, int cap, int join, double miterLimit,
                         reef_ui_path_element* out, unsigned capacity)
{
    return writePath(pathOf(elements, count, 0).stroked(width, lineCapOf(cap), lineJoinOf(join), miterLimit), out, capacity);
}

} // namespace cutefish::reef
