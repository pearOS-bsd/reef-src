// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What a window has above its base: planes with their own pictures, glass, and overlays that fade.

#include "views.h"

#include "fade.h"

#include "ui/view_glass.h"

#include <algorithm>
#include <cfloat>
#include <cstring>
#include <utility>

// One pane of glass, see ui::ViewGlass.
struct reef_ui_glass
{
    uint64_t key;
    double rect[4];
    double radius;
    double opacity;
    double tint[4];
    const double* light;
    unsigned lightCount;
    double over[4];
    double edge[4];
    double sideEdge[4];
    double edgeDevicePixels;
    double blur;
    double saturation;
    double bezel;
    double bend;
    double profile;
    double dispersion;
    double zoom;
    double glow[4];
    double glowRadius;
    double glowEdge;
    double glowLag;
    double fadeTop;
    double fadeBottom;
    double shadow[4];
    double shadowBlur;
    double shadowOffset;
    double duration;
    double curve[4];
    double shapeDuration;
    double shapeCurve[4];
    double exponent;
    double sideBezel;
    double bottomEdge[4];
    double multiply[4];
    int blurAtHalfResolution;
    int edgeOnPixels;
    double bendPeak;
    double dispersionSides;
    double shadowGap;
    double bendAlongAxes;
    double edgeSplit;
    double sideBendPeak;
    double exponentSlopes[2];
    double blurPower;
    double fadeBlur;
    const double* fadeProfile;
    unsigned fadeProfileCount;
};

namespace cutefish::reef {

using namespace std::chrono_literals;

static std::vector<unsigned char> copyPixels(const void* pixels, int width, int height, size_t stride)
{
    const size_t row = size_t(width) * 4;
    std::vector<unsigned char> copy(row * height);
    for (int y = 0; y < height; y++)
        memcpy(copy.data() + y * row, static_cast<const unsigned char*>(pixels) + y * stride, row);
    return copy;
}

// Where a rectangle of a plane is in its window, clipped.
static ui::RectF planeRect(const reef_ui_plane* plane, const ui::RectF& rect)
{
    const ui::RectF& clip = plane->clip->geometry();
    const double x = clip.x + plane->view->rect.x;
    const double y = clip.y + plane->view->rect.y;
    const double left = std::max(clip.x, x + rect.x);
    const double top = std::max(clip.y, y + rect.y);
    const double right = std::min(clip.right(), x + rect.right());
    const double bottom = std::min(clip.bottom(), y + rect.bottom());
    return {left, top, std::max(0.0, right - left), std::max(0.0, bottom - top)};
}

static void markOverlays(reef_ui_window* window, const ui::RectF& rect)
{
    for (reef_ui_overlay* overlay : window->overlays) {
        if (rect.x < overlay->rect.right() && overlay->rect.x < rect.right() && rect.y < overlay->rect.bottom()
            && overlay->rect.y < rect.bottom())
            overlay->stale = true;
    }
}

// 17
void hostWindowRequestPaint(reef_ui_window* window)
{
    for (reef_ui_overlay* overlay : window->overlays)
        overlay->stale = true;
    window->base->requestPaint();
    for (reef_ui_plane* plane : window->planes)
        plane->view->requestPaint();
}

// 202
void hostWindowRequestPaintRect(reef_ui_window* window, double x, double y, double width, double height)
{
    markOverlays(window, {x, y, width, height});
    window->base->requestPaint({x, y, width, height});
}

// 209
void hostWindowScroll(reef_ui_window* window, double x, double y, double width, double height, double dx, double dy)
{
    markOverlays(window, {x, y, width, height});
    window->base->scrollPainted({x, y, width, height}, dx, dy);
}

// 259
reef_ui_plane* hostPlaneCreate(reef_ui_window* window, void* context)
{
    if (!window || !window->root)
        return nullptr;
    auto plane = new reef_ui_plane{window, context};
    plane->clip = window->root->addChild<ui::View>();
    plane->view = plane->clip->addChild<PlaneView>(plane);
    window->planes.push_back(plane);
    return plane;
}

// 260
void hostPlaneDestroy(reef_ui_plane* plane)
{
    if (!plane)
        return;
    reef_ui_window* window = plane->window;
    markOverlays(window, planeRect(plane, {0, 0, plane->view->rect.width, plane->view->rect.height}));
    std::erase(window->planes, plane);
    window->root->takeChild(plane->clip);
    delete plane;
}

static ui::RectF localRect(const reef_ui_plane* plane)
{
    return {0, 0, plane->view->rect.width, plane->view->rect.height};
}

// 261
// Not clipped with a negative width of the clip.
void hostPlaneSetGeometry(reef_ui_plane* plane, double x, double y, double width, double height, double clipX, double clipY,
                          double clipWidth, double clipHeight, double radius, double opacity, int opaque)
{
    reef_ui_window* window = plane->window;
    const bool clips = clipWidth >= 0;
    const ui::RectF clip = clips ? ui::RectF{clipX, clipY, clipWidth, std::max(clipHeight, 0.0)} : window->root->localRect();
    const ui::RectF rect = {x - clip.x, y - clip.y, width, height};
    const bool changed = clip != plane->clip->geometry() || rect != plane->view->rect || opacity != plane->view->opacity();
    if (changed)
        markOverlays(window, planeRect(plane, localRect(plane)));
    plane->clip->setGeometry(clip);
    plane->clip->setClipsChildren(clips);
    plane->clip->setChildClipRadius(clips ? radius : 0);
    if (rect != plane->view->rect) {
        plane->view->rect = rect;
        plane->view->setGeometry(rect);
    }
    if (changed)
        markOverlays(plane->window, planeRect(plane, localRect(plane)));
    plane->view->setOpacity(opacity);
    plane->view->setOpaqueLayer(opaque);
}

// 262
void hostWindowSetPlaneOrder(reef_ui_window* window, reef_ui_plane* const* planes, unsigned count)
{
    const auto& children = window->root->children();
    unsigned first = 0;
    while (first < count && first + 1 < children.size() && children[first + 1].get() == planes[first]->clip)
        first++;
    for (unsigned i = first; i < count; i++)
        window->root->raiseChild(planes[i]->clip);
}

// 263
// All of it with a negative width.
void hostPlaneRequestPaint(reef_ui_plane* plane, double x, double y, double width, double height)
{
    markOverlays(plane->window, planeRect(plane, width < 0 ? localRect(plane) : ui::RectF{x, y, width, height}));
    if (width < 0)
        plane->view->requestPaint();
    else
        plane->view->requestPaint({x, y, width, height});
}

// 264
void hostPlaneScroll(reef_ui_plane* plane, double x, double y, double width, double height, double dx, double dy)
{
    markOverlays(plane->window, planeRect(plane, {x, y, width, height}));
    plane->view->scrollPainted({x, y, width, height}, dx, dy);
}

// 295
// Areas of five numbers: the rectangle and the radius.
void hostPlaneSetBackdrop(reef_ui_plane* plane, const double* areas, unsigned count, double blur, double saturation)
{
    std::vector<ui::View::BackdropArea> list;
    for (unsigned i = 0; i < count; i++)
        list.push_back({{areas[i * 5], areas[i * 5 + 1], areas[i * 5 + 2], areas[i * 5 + 3]}, areas[i * 5 + 4]});
    plane->view->setBackdrop(list, blur, saturation);
}

static ui::RectF scaledRect(const reef_ui_plane* plane, double scale)
{
    const ui::RectF& rect = plane->view->rect;
    scale = std::max(1.0, scale);
    return planeRect(plane, {(1 - scale) * rect.width * 0.5, (1 - scale) * rect.height * 0.5, rect.width * scale, rect.height * scale});
}

// 298
void hostPlaneSetScale(reef_ui_plane* plane, double scale)
{
    const ui::Transform transform = {{}, scale};
    if (plane->view->transform() == transform)
        return;
    markOverlays(plane->window, scaledRect(plane, plane->view->transform().scale));
    plane->view->setTransform(transform);
    markOverlays(plane->window, scaledRect(plane, scale));
}

// 275
reef_ui_overlay* hostOverlayCreate(reef_ui_window* window, double x, double y, double width, double height, double duration,
                                   double elapsed, const void* pixels, int pixelWidth, int pixelHeight, size_t stride,
                                   const double* curve)
{
    if (!window || !window->window || !pixels || pixelWidth <= 0 || pixelHeight <= 0 || duration <= 0)
        return nullptr;
    const ui::RectF rect = {x, y, width, height};
    auto fade = std::make_unique<FadeOverlay>(window->window, rect, copyPixels(pixels, pixelWidth, pixelHeight, stride), pixelWidth,
                                              pixelHeight, duration, elapsed, curve);
    if (!fade->isValid())
        return nullptr;
    auto overlay = new reef_ui_overlay{window, rect, pixelWidth, pixelHeight, false, std::move(fade)};
    window->overlays.push_back(overlay);
    return overlay;
}

// 276
int hostOverlayNeedsPixels(reef_ui_overlay* overlay)
{
    if (std::exchange(overlay->stale, false))
        return 1;
    return !overlay->window;
}

// 277
void hostOverlaySetPixels(reef_ui_overlay* overlay, const void* pixels, size_t stride)
{
    FadeOverlay* fade = overlay->fade.get();
    if (pixels && fade)
        fade->setPixels(copyPixels(pixels, overlay->width, overlay->height, stride));
}

// 278
// What it showed stays below the window until the window was drawn without it.
void hostOverlayDestroy(reef_ui_overlay* overlay)
{
    if (reef_ui_window* window = overlay->window) {
        std::erase(window->overlays, overlay);
        overlay->fade->putBelow();
        window->retired.emplace_back(std::chrono::steady_clock::now(), std::move(overlay->fade));
        if (!window->retiredTimer) {
            window->retiredTimer = std::make_unique<base::Timer>();
            window->retiredTimer->setSingleShot(true);
            window->retiredTimer->onTimeout = [window] {
                const auto now = std::chrono::steady_clock::now();
                std::erase_if(window->retired, [now](const auto& entry) { return now - entry.first >= 250ms; });
                if (!window->retired.empty())
                    window->retiredTimer->start(250ms);
            };
        }
        if (window->retired.size() == 1)
            window->retiredTimer->start(250ms);
    }
    delete overlay;
}

static void copyNumbers(double (&to)[4], const double (&from)[4])
{
    std::copy(from, from + 4, to);
}

// 297
void hostPlaneSetGlass(reef_ui_plane* plane, const reef_ui_glass* panes, unsigned count)
{
    std::vector<ui::ViewGlass> list;
    list.reserve(count);
    for (unsigned i = 0; i < count; i++) {
        const reef_ui_glass& pane = panes[i];
        ui::ViewGlass glass;
        glass.key = pane.key;
        glass.rect = {pane.rect[0], pane.rect[1], pane.rect[2], pane.rect[3]};
        glass.radius = pane.radius;
        glass.opacity = pane.opacity;
        copyNumbers(glass.tint, pane.tint);
        for (unsigned j = 0; j < pane.lightCount; j++)
            glass.light.push_back({pane.light[j * 2], pane.light[j * 2 + 1]});
        copyNumbers(glass.over, pane.over);
        copyNumbers(glass.edge, pane.edge);
        copyNumbers(glass.sideEdge, pane.sideEdge);
        glass.edgeDevicePixels = pane.edgeDevicePixels;
        glass.blur = pane.blur;
        glass.saturation = pane.saturation;
        glass.bezel = pane.bezel;
        glass.sideBezel = pane.sideBezel;
        glass.bend = pane.bend;
        glass.profile = pane.profile;
        glass.dispersion = pane.dispersion;
        glass.zoom = pane.zoom;
        glass.exponent = pane.exponent;
        glass.edgeOnPixels = pane.edgeOnPixels;
        copyNumbers(glass.bottomEdge, pane.bottomEdge);
        copyNumbers(glass.multiply, pane.multiply);
        glass.blurAtHalfResolution = pane.blurAtHalfResolution;
        glass.bendPeak = pane.bendPeak;
        glass.dispersionSides = pane.dispersionSides;
        glass.shadowGap = pane.shadowGap;
        glass.bendAlongAxes = pane.bendAlongAxes;
        glass.edgeSplit = pane.edgeSplit;
        glass.sideBendPeak = pane.sideBendPeak;
        glass.exponentSlopes[0] = pane.exponentSlopes[0];
        glass.exponentSlopes[1] = pane.exponentSlopes[1];
        glass.blurPower = pane.blurPower > 0 ? pane.blurPower : 1;
        glass.fadeBlur = pane.fadeBlur;
        for (unsigned j = 0; j < pane.fadeProfileCount; j++)
            glass.fadeProfile.push_back({pane.fadeProfile[j * 2], pane.fadeProfile[j * 2 + 1]});
        copyNumbers(glass.glow, pane.glow);
        glass.glowRadius = pane.glowRadius;
        glass.glowEdge = pane.glowEdge;
        glass.glowLag = pane.glowLag;
        glass.fadeTop = pane.fadeTop;
        glass.fadeBottom = pane.fadeBottom;
        copyNumbers(glass.shadow, pane.shadow);
        glass.shadowBlur = pane.shadowBlur;
        glass.shadowOffset = pane.shadowOffset;
        glass.duration = pane.duration;
        copyNumbers(glass.curve, pane.curve);
        glass.shapeDuration = pane.shapeDuration;
        copyNumbers(glass.shapeCurve, pane.shapeCurve);
        list.push_back(std::move(glass));
    }
    plane->view->setGlass(std::move(list));
}

} // namespace cutefish::reef
