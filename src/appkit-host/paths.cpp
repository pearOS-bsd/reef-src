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

using Contour = std::vector<reef_ui_path_element>;

static ui::PointF endOf(const reef_ui_path_element& element)
{
    const int last = element.type == 3 ? 4 : 0;
    return {element.points[last], element.points[last + 1]};
}

// Curves become eight lines.
static std::vector<ui::PointF> polygonOf(const Contour& contour)
{
    std::vector<ui::PointF> polygon;
    ui::PointF current;
    for (const reef_ui_path_element& element : contour) {
        const double* points = element.points;
        if (element.type == 3) {
            for (int i = 1; i <= 8; i++) {
                const double t = i / 8.0;
                const double u = 1 - t;
                polygon.push_back(
                    {u * u * u * current.x + 3 * u * u * t * points[0] + 3 * u * t * t * points[2] + t * t * t * points[4],
                     u * u * u * current.y + 3 * u * u * t * points[1] + 3 * u * t * t * points[3] + t * t * t * points[5]});
            }
            current = {points[4], points[5]};
        } else {
            current = {points[0], points[1]};
            polygon.push_back(current);
        }
    }
    return polygon;
}

static bool contains(const std::vector<ui::PointF>& polygon, ui::PointF point)
{
    bool inside = false;
    for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        if ((polygon[i].y > point.y) != (polygon[j].y > point.y)
            && point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)
            inside = !inside;
    }
    return inside;
}

static double areaOf(const std::vector<ui::PointF>& polygon)
{
    double area = 0;
    for (size_t i = 0; i < polygon.size(); i++) {
        const ui::PointF& next = polygon[(i + 1) % polygon.size()];
        area += polygon[i].x * next.y - next.x * polygon[i].y;
    }
    return area / 2;
}

static Contour reversed(const Contour& contour)
{
    Contour result;
    const ui::PointF end = endOf(contour.back());
    result.push_back({0, {end.x, end.y}});
    for (size_t i = contour.size() - 1; i > 0; i--) {
        reef_ui_path_element element = contour[i];
        const ui::PointF start = endOf(contour[i - 1]);
        if (element.type == 3) {
            std::swap(element.points[0], element.points[2]);
            std::swap(element.points[1], element.points[3]);
            element.points[4] = start.x;
            element.points[5] = start.y;
        } else {
            element.points[0] = start.x;
            element.points[1] = start.y;
        }
        result.push_back(element);
    }
    return result;
}

// 167
// operation: 1 intersection, 2 the first without the second, 3 what only one of them has, otherwise both. The outlines go one way
// around, the holes the other, for either fill rule.
unsigned hostPathCombine(const reef_ui_path_element* first, unsigned firstCount, int firstEvenOdd, const reef_ui_path_element* second,
                         unsigned secondCount, int secondEvenOdd, int operation, reef_ui_path_element* out, unsigned capacity)
{
    const ui::Path a = pathOf(first, firstCount, firstEvenOdd);
    const ui::Path b = pathOf(second, secondCount, secondEvenOdd);
    ui::Path path;
    if (operation == 1)
        path = a.intersected(b);
    else if (operation == 2)
        path = a.subtracted(b);
    else if (operation == 3)
        path = a.subtracted(b).united(b.subtracted(a));
    else
        path = a.united(b);
    std::vector<reef_ui_path_element> elements(writePath(path, nullptr, 0));
    writePath(path, elements.data(), unsigned(elements.size()));
    std::vector<Contour> contours;
    for (const reef_ui_path_element& element : elements) {
        if (element.type == 0 || contours.empty())
            contours.emplace_back();
        contours.back().push_back(element);
    }
    std::vector<std::vector<ui::PointF>> polygons;
    for (const Contour& contour : contours)
        polygons.push_back(polygonOf(contour));
    elements.clear();
    for (size_t i = 0; i < contours.size(); i++) {
        Contour& contour = contours[i];
        if (!polygons[i].empty()) {
            int depth = 0;
            for (size_t j = 0; j < polygons.size(); j++) {
                if (j != i && polygons[j].size() > 2)
                    depth += contains(polygons[j], polygons[i].front());
            }
            const double area = areaOf(polygons[i]);
            if (contour.size() > 1 && area != 0 && (area > 0) != (depth % 2 == 0))
                contour = reversed(contour);
        }
        elements.insert(elements.end(), contour.begin(), contour.end());
    }
    for (unsigned i = 0; i < elements.size() && i < capacity; i++)
        out[i] = elements[i];
    return unsigned(elements.size());
}

} // namespace cutefish::reef
