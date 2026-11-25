// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What a window blurs: behind itself, its subsurfaces and the shape it has.

#include "views.h"

#include "ui/image.h"
#include "ui/path.h"
#include "ui/popup_glass.h"
#include "ui/theme.h"

#include <algorithm>

namespace cutefish::reef {

// Of areas of five numbers: the rectangle and the radius.  With a path, more than one area becomes a shape.
static ui::RectF areasRect(const std::vector<double>& areas, ui::Path* path)
{
    ui::RectF bounds;
    const size_t count = areas.size() / 5;
    for (size_t i = 0; i < count; i++) {
        const ui::RectF rect = {areas[i * 5], areas[i * 5 + 1], areas[i * 5 + 2], areas[i * 5 + 3]};
        bounds = i == 0 ? rect : united(bounds, rect);
        if (path && count > 1) {
            const double radius = std::min(areas[i * 5 + 4], std::min(rect.width, rect.height) * 0.5);
            if (radius > 0)
                path->addRoundedRect(rect, radius, radius);
            else
                path->addRect(rect);
        }
    }
    return bounds;
}

// Nothing below shows through a window with a background.
static void applyBackground(reef_ui_window* window)
{
    const bool clear = window->transparentUnderlays > 0 || window->hasBehindMaterials;
    if (clear == window->cleared)
        return;
    window->cleared = clear;
    if (clear)
        window->window->setBackgroundColor(ui::ColorRecipe(ui::Color{0, 0, 0, 0}));
    else
        window->window->setBackgroundColor(std::nullopt);
    if (window->opaque)
        window->base->setOpaqueLayer(!clear);
    window->base->requestPaint();
}

static void applySubsurfaceBlur(reef_ui_window* window)
{
    const bool visible = window->underlays > 0 && window->subsurfaceBlur.width > 0 && window->subsurfaceBlur.height > 0;
    ui::Path shape;
    if (visible) {
        window->window->setSubsurfaceBlur(window->subsurfaceBlur, window->subsurfaceBlurRadius);
        areasRect(window->subsurfaceAreas, &shape);
    } else if (window->subsurfaceBlurred) {
        window->window->setSubsurfaceBlur({}, 0);
    }
    window->window->setSubsurfaceBlurShape(shape);
    ui::BackdropFill fill;
    if (visible && window->subsurfaceFill.size() == 9) {
        std::copy(window->subsurfaceFill.begin(), window->subsurfaceFill.begin() + 4, fill.tint);
        fill.blend = ui::BackdropFill::Blend(int(window->subsurfaceFill[4]));
        std::copy(window->subsurfaceFill.begin() + 5, window->subsurfaceFill.end(), fill.color);
    }
    window->window->setSubsurfaceBlurFill(fill);
    window->subsurfaceBlurred = visible;
}

// 272
void hostWindowSetSubsurfaceBlurFill(reef_ui_window* window, const double* tint, int blend, const double* color)
{
    std::vector<double> fill;
    if (blend >= 1 && blend <= 16) {
        fill.assign(tint, tint + 4);
        fill.push_back(blend);
        fill.insert(fill.end(), color, color + 4);
    }
    if (fill == window->subsurfaceFill)
        return;
    window->subsurfaceFill = fill;
    applySubsurfaceBlur(window);
}

// 269
void hostWindowSetSubsurfaceBlurAreas(reef_ui_window* window, const double* areas, unsigned count)
{
    const std::vector<double> list(areas, areas + size_t(count) * 5);
    if (list == window->subsurfaceAreas)
        return;
    window->subsurfaceAreas = list;
    window->subsurfaceBlur = count ? areasRect(list, nullptr) : ui::RectF();
    window->subsurfaceBlurRadius = count == 1 ? areas[4] : 0;
    applySubsurfaceBlur(window);
}

// 266
void hostWindowSetSubsurfaceBlur(reef_ui_window* window, double x, double y, double width, double height, double radius)
{
    ui::RectF rect;
    if (width > 0 && height > 0)
        rect = {x, y, width, height};
    if (rect == window->subsurfaceBlur && radius == window->subsurfaceBlurRadius && window->subsurfaceAreas.empty())
        return;
    window->subsurfaceBlur = rect;
    window->subsurfaceBlurRadius = radius;
    window->subsurfaceAreas.clear();
    applySubsurfaceBlur(window);
}

// 310
int hostWindowIsTransparent(reef_ui_window* window)
{
    return window->transparent || window->cleared;
}

// 300
unsigned hostWindowBehindMaterialCount(reef_ui_window* window)
{
    return unsigned(window->behindMaterials.size() / 5);
}

static void applyBackgroundBlur(reef_ui_window* window)
{
    const ui::SizeF size = window->window->size();
    ui::Path shape;
    if (window->blurred) {
        window->window->setBackgroundBlur({0, 0, size.width, size.height}, ui::Theme::current().windowRadius);
    } else if (window->behindMaterials.empty()) {
        window->window->setBackgroundBlur({}, 0);
    } else {
        const ui::RectF rect = areasRect(window->behindMaterials, &shape);
        window->window->setBackgroundBlur(rect, window->behindMaterials.size() == 5 ? window->behindMaterials[4] : 0);
    }
    window->window->setBackgroundBlurShape(shape);
}

// 232
void hostWindowSetBackgroundBlur(reef_ui_window* window, int blurred)
{
    window->blurred = blurred;
    applyBackgroundBlur(window);
}

// 256
double hostWindowCornerRadius()
{
    return ui::Theme::current().windowRadius;
}

// 190
// The shape is the outline of what the window draws, or the whole window.
void hostWindowSetShapeFromContent(reef_ui_window* window, int fromContent)
{
    if (!fromContent) {
        window->window->setBodyShape({});
        return;
    }
    const ui::Image image = window->window->grab();
    if (image.isNull())
        return;
    const ui::SizeF size = window->window->size();
    if (size.width <= 0)
        return;
    const double scale = image.width() / size.width;
    ui::Path shape;
    // The first pixel of each row that counts, and the one after the last.
    struct Row
    {
        int first = -1;
        int last = -1;
    };
    const int height = image.height();
    std::vector<Row> rows(std::max(0, height));
    for (int y = 0; y < height; y++) {
        const uint32_t* pixels = image.bits() + size_t(y) * image.width();
        for (int x = 0; x < image.width(); x++) {
            if (pixels[x] >> 24 < 128)
                continue;
            if (rows[y].first == -1)
                rows[y].first = x;
            rows[y].last = x + 1;
        }
    }
    for (int top = 0; top < height;) {
        if (rows[top].first < 0) {
            top++;
            continue;
        }
        int bottom = top;
        while (bottom < height && rows[bottom].first >= 0)
            bottom++;
        // Down the left edge and up the right one.
        std::vector<ui::PointF> points;
        for (int y = top; y < bottom; y++) {
            const int x = rows[y].first;
            if (y == top || x != rows[y - 1].first)
                points.push_back({x / scale, y / scale});
            if (y + 1 == bottom || x != rows[y + 1].first)
                points.push_back({x / scale, (y + 1) / scale});
        }
        for (int y = bottom - 1; y >= top; y--) {
            const int x = rows[y].last;
            if (y + 1 == bottom || x != rows[y + 1].last)
                points.push_back({x / scale, (y + 1) / scale});
            if (y == top || x != rows[y - 1].last)
                points.push_back({x / scale, y / scale});
        }
        shape.moveTo(points[0]);
        for (size_t i = 1; i < points.size(); i++)
            shape.lineTo(points[i]);
        shape.close();
        top = bottom;
    }
    // An empty shape would be the whole window.
    if (shape.isEmpty()) {
        const double x = size.width / 2;
        const double y = size.height / 2;
        shape.moveTo(x, y);
        shape.lineTo(x + 1, y);
        shape.lineTo(x + 1, y + 1);
        shape.lineTo(x, y + 1);
        shape.close();
    }
    window->window->setBodyShape(shape);
}

// 191
double hostPopupCornerRadius()
{
    return 14;
}

// 192
int hostWindowSetPopupGlass(reef_ui_window* window, int dark)
{
    ui::Window& target = *window->window;
    const ui::SizeF size = target.size();
    target.setBackgroundBlur({0, 0, size.width, size.height}, hostPopupCornerRadius());
    ui::PopupGlass::prepare(target);
    if (!target.backgroundGlassAvailable())
        return 0;
    target.setBackgroundGlass(ui::popupGlass(dark));
    return 1;
}

void windowSetUnderlay(reef_ui_window* window, bool underlay)
{
    if (!window)
        return;
    const int change = underlay ? 1 : -1;
    window->underlays += change;
    const std::weak_ptr<bool> alive = window->alive;
    base::EventLoop::current()->post([window, alive] {
        if (!alive.expired())
            applySubsurfaceBlur(window);
    });
    if (window->transparent)
        return;
    window->transparentUnderlays += change;
    base::EventLoop::current()->post([window, alive] {
        if (!alive.expired())
            applyBackground(window);
    });
}

// 268
void hostWindowSetBehindMaterials(reef_ui_window* window, const double* areas, unsigned count)
{
    const std::vector<double> list(areas, areas + size_t(count) * 5);
    if (list == window->behindMaterials)
        return;
    const bool first = !list.empty() && !window->hasBehindMaterials;
    window->behindMaterials = list;
    applyBackgroundBlur(window);
    if (!first)
        return;
    window->hasBehindMaterials = true;
    if (window->transparent)
        return;
    const std::weak_ptr<bool> alive = window->alive;
    base::EventLoop::current()->post([window, alive] {
        if (!alive.expired())
            applyBackground(window);
    });
}

} // namespace cutefish::reef
