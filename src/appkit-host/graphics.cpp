// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "graphics.h"

#include "paths.h"

#include <algorithm>
#include <cmath>

// Bitmaps of the GPU's, that programs keep (images.cpp).
struct reef_ui_image
{
    cutefish::ui::RetainedBitmap bitmap;
};

namespace cutefish::reef {

// One offset and four numbers of color each.
static std::vector<ui::GradientStopF> gradientStops(const double* offsets, const double* colors, unsigned count)
{
    std::vector<ui::GradientStopF> stops;
    for (unsigned i = 0; i < count; i++)
        stops.push_back({offsets[i], {colors[i * 4], colors[i * 4 + 1], colors[i * 4 + 2], colors[i * 4 + 3]}});
    return stops;
}

// 33
void gcSave(reef_ui_gc* gc)
{
    if (gc->pdf)
        return gc->pdf->save();
    gc->canvas->save();
    gc->saved.push_back(gc->state);
    gc->state.groups = 0;
}

// What is excluded stays so, whatever the clip is.
static void applyExcluded(reef_ui_gc* gc)
{
    if (gc->excluded.empty())
        return;
    const base::AffineTransform transform = gc->canvas->transform();
    gc->canvas->setTransform({});
    ui::Path path;
    path.setFillRule(ui::Path::FillRule::EvenOdd);
    path.addRect({-1e6, -1e6, 2e6, 2e6});
    for (const ui::RectF& rect : gc->excluded)
        path.addRect(rect);
    gc->canvas->clipToPath(path);
    gc->canvas->setTransform(transform);
}

// 34
void gcRestore(reef_ui_gc* gc)
{
    if (gc->pdf)
        return gc->pdf->restore();
    gc->canvas->restore();
    if (!gc->saved.empty()) {
        gc->state = gc->saved.back();
        gc->saved.pop_back();
    }
    applyExcluded(gc);
}

// 270
void gcExcludeRect(reef_ui_gc* gc, double x, double y, double width, double height)
{
    if (gc->pdf || !gc->canvas)
        return;
    gc->excluded.push_back({x, y, width, height});
    applyExcluded(gc);
}

// 271
// All of the bitmap of the context, whatever the clip is.
int gcClearBitmap(reef_ui_gc* gc)
{
    if (gc->pdf || gc->bitmap.isNull())
        return 0;
    if (gc->state.groups)
        return 0;
    for (const GcState& state : gc->saved) {
        if (state.groups)
            return 0;
    }
    gc->bitmap.fill(0);
    return 1;
}

// 35
void gcConcat(reef_ui_gc* gc, const double* matrix)
{
    if (gc->pdf)
        return gc->pdf->concat(matrix);
    gc->canvas->concat(transformOf(matrix));
}

// 36
void gcSetTransform(reef_ui_gc* gc, const double* matrix)
{
    if (gc->pdf)
        return gc->pdf->setTransform(matrix);
    gc->canvas->setTransform(transformOf(matrix));
}

// 37
void gcTransform(reef_ui_gc* gc, double* matrix)
{
    if (gc->pdf)
        return gc->pdf->transform(matrix);
    const base::AffineTransform& transform = gc->canvas->transform();
    matrix[0] = transform.m11();
    matrix[1] = transform.m12();
    matrix[2] = transform.m21();
    matrix[3] = transform.m22();
    matrix[4] = transform.dx();
    matrix[5] = transform.dy();
}

// 38
double gcDeviceScale(reef_ui_gc* gc)
{
    if (gc->pdf) {
        double matrix[6];
        gc->pdf->transform(matrix);
        return std::sqrt(std::abs(matrix[0] * matrix[3] - matrix[1] * matrix[2]));
    }
    return gc->canvas->deviceScale();
}

// 39
void gcClipToRect(reef_ui_gc* gc, double x, double y, double width, double height)
{
    if (gc->pdf)
        return gc->pdf->clipToRect(x, y, width, height);
    gc->canvas->clipToRect({x, y, width, height});
}

// 40
void gcClipToPath(reef_ui_gc* gc, const reef_ui_path_element* elements, unsigned count, int evenOdd)
{
    if (gc->pdf)
        return gc->pdf->clipToPath(elements, count, evenOdd);
    gc->canvas->clipToPath(pathOf(elements, count, evenOdd));
}

// 42
void gcClipBounds(reef_ui_gc* gc, double* rect)
{
    if (gc->pdf)
        return gc->pdf->clipBounds(rect);
    const ui::RectF bounds = gc->canvas->clipBounds();
    rect[0] = bounds.x;
    rect[1] = bounds.y;
    rect[2] = bounds.width;
    rect[3] = bounds.height;
}

// 43
void gcSetAlpha(reef_ui_gc* gc, double alpha)
{
    if (gc->pdf)
        return gc->pdf->setAlpha(alpha);
    gc->canvas->setGlobalAlpha(alpha);
}

static ui::Canvas2D::Composite compositeOf(int composite)
{
    return composite == 26 ? ui::Canvas2D::Composite::Multiply
        : composite == 27  ? ui::Canvas2D::Composite::Lighter
                           : ui::Canvas2D::Composite(std::clamp(composite, 0, 25));
}

// 44
void gcSetComposite(reef_ui_gc* gc, int composite)
{
    if (gc->pdf)
        return gc->pdf->setComposite(composite);
    gc->state.composite = composite;
    gc->canvas->setComposite(compositeOf(composite));
}

// 45
void gcSetAntialias(reef_ui_gc* gc, int antialias)
{
    if (gc->pdf)
        return;
    gc->state.antialias = antialias;
    gc->canvas->setAntialias(antialias);
}

// 46
void gcSetSmoothing(reef_ui_gc* gc, int smoothing)
{
    if (gc->pdf)
        return gc->pdf->setSmoothing(smoothing);
    gc->state.smoothing = ui::Canvas2D::Smoothing(std::clamp(smoothing, 0, 3));
    gc->canvas->setImageSmoothing(gc->state.smoothing);
}

// 47
void gcSetFillColor(reef_ui_gc* gc, double red, double green, double blue, double alpha)
{
    if (gc->pdf)
        return gc->pdf->setFillColor(red, green, blue, alpha);
    gc->canvas->setFillColor({red, green, blue, alpha});
}

// 48
void gcSetStrokeColor(reef_ui_gc* gc, double red, double green, double blue, double alpha)
{
    if (gc->pdf)
        return gc->pdf->setStrokeColor(red, green, blue, alpha);
    gc->canvas->setStrokeColor({red, green, blue, alpha});
}

// 49
void gcSetLineWidth(reef_ui_gc* gc, double width)
{
    if (gc->pdf)
        return gc->pdf->setLineWidth(width);
    gc->canvas->setLineWidth(width);
}

// 50
void gcSetLineCap(reef_ui_gc* gc, int cap)
{
    if (gc->pdf)
        return gc->pdf->setLineCap(cap);
    gc->state.lineCap = lineCapOf(cap);
    gc->canvas->setLineCap(gc->state.lineCap);
}

// 51
void gcSetLineJoin(reef_ui_gc* gc, int join)
{
    if (gc->pdf)
        return gc->pdf->setLineJoin(join);
    gc->state.lineJoin = lineJoinOf(join);
    gc->canvas->setLineJoin(gc->state.lineJoin);
}

// 52
void gcSetMiterLimit(reef_ui_gc* gc, double limit)
{
    if (gc->pdf)
        return gc->pdf->setMiterLimit(limit);
    gc->state.miterLimit = limit;
    gc->canvas->setMiterLimit(limit);
}

// 53
void gcSetLineDash(reef_ui_gc* gc, const double* lengths, unsigned count, double offset)
{
    if (gc->pdf)
        return gc->pdf->setLineDash(lengths, count, offset);
    gc->state.dashOffset = offset;
    gc->state.dashes.assign(lengths, lengths + count);
    gc->canvas->setLineDash(gc->state.dashes, offset);
}

// 54
void gcSetShadow(reef_ui_gc* gc, double dx, double dy, double blur, double red, double green, double blue, double alpha)
{
    if (gc->pdf)
        return gc->pdf->setShadow(dx, dy, blur, red, green, blue, alpha);
    const double shadow[7] = {dx, dy, blur, red, green, blue, alpha};
    std::copy(shadow, shadow + 7, gc->state.shadow);
    gc->canvas->setShadow({dx, dy}, blur, {red, green, blue, alpha});
}

static void setShadow(reef_ui_gc* gc, const double* shadow)
{
    gcSetShadow(gc, shadow[0], shadow[1], shadow[2], shadow[3], shadow[4], shadow[5], shadow[6]);
}

// 41
// The clip of the last save; the rest of the state stays.
void gcResetClip(reef_ui_gc* gc)
{
    if (gc->pdf)
        return;
    if (gc->saved.empty() || gc->state.groups)
        return;
    ui::Canvas2D* canvas = gc->canvas.get();
    const base::AffineTransform transform = canvas->transform();
    const double alpha = canvas->globalAlpha();
    const ui::ColorF fill = canvas->fillColor();
    const ui::ColorF stroke = canvas->strokeColor();
    const double lineWidth = canvas->lineWidth();
    const GcState state = gc->state;
    gcRestore(gc);
    gcSave(gc);
    gc->state = state;
    canvas->setTransform(transform);
    canvas->setGlobalAlpha(alpha);
    canvas->setFillColor(fill);
    canvas->setStrokeColor(stroke);
    canvas->setLineWidth(lineWidth);
    canvas->setAntialias(state.antialias);
    canvas->setImageSmoothing(state.smoothing);
    canvas->setLineCap(state.lineCap);
    canvas->setLineJoin(state.lineJoin);
    canvas->setMiterLimit(state.miterLimit);
    canvas->setLineDash(state.dashes, state.dashOffset);
    gcSetComposite(gc, state.composite);
    setShadow(gc, state.shadow);
}

// 55
void gcFillPath(reef_ui_gc* gc, const reef_ui_path_element* elements, unsigned count, int evenOdd)
{
    if (gc->pdf)
        return gc->pdf->fillPath(elements, count, evenOdd);
    gc->canvas->fillPath(pathOf(elements, count, evenOdd));
}

// 56
void gcStrokePath(reef_ui_gc* gc, const reef_ui_path_element* elements, unsigned count)
{
    if (gc->pdf)
        return gc->pdf->strokePath(elements, count);
    gc->canvas->strokePath(pathOf(elements, count, 0));
}

// 57
void gcFillRect(reef_ui_gc* gc, double x, double y, double width, double height)
{
    if (gc->pdf)
        return gc->pdf->fillRect(x, y, width, height);
    gc->canvas->fillRect({x, y, width, height});
}

// 58
void gcStrokeRect(reef_ui_gc* gc, double x, double y, double width, double height)
{
    if (gc->pdf)
        return gc->pdf->strokeRect(x, y, width, height);
    gc->canvas->strokeRect({x, y, width, height});
}

// 59
void gcClearRect(reef_ui_gc* gc, double x, double y, double width, double height)
{
    if (gc->pdf)
        return gc->pdf->clearRect(x, y, width, height);
    gc->canvas->clearRect({x, y, width, height});
}

// 267
void gcBlurBackdrop(reef_ui_gc* gc, const reef_ui_path_element* elements, unsigned count, double radius, double saturation)
{
    if (gc->pdf || !gc->canvas)
        return;
    gc->canvas->blurBackdrop(pathOf(elements, count, 0), radius, saturation);
}

// 60
// Bit 0 of `extend` goes on before the start, bit 1 after the end.
void gcFillLinearGradient(reef_ui_gc* gc, const double* offsets, const double* colors, unsigned count, double x0, double y0, double x1,
                          double y1, int extend)
{
    if (gc->pdf)
        return gc->pdf->fillLinearGradient(offsets, colors, count, x0, y0, x1, y1, extend);
    gc->canvas->fillLinearGradient(gradientStops(offsets, colors, count), {x0, y0}, {x1, y1}, extend & 1, extend & 2);
}

// 61
void gcFillRadialGradient(reef_ui_gc* gc, const double* offsets, const double* colors, unsigned count, double x0, double y0, double r0,
                          double x1, double y1, double r1, int extend)
{
    if (gc->pdf)
        return gc->pdf->fillRadialGradient(offsets, colors, count, x0, y0, r0, x1, y1, r1, extend);
    gc->canvas->fillRadialGradient(gradientStops(offsets, colors, count), {x0, y0}, r0, {x1, y1}, r1, extend & 1, extend & 2);
}

// 66
void gcBeginGroup(reef_ui_gc* gc)
{
    if (gc->pdf)
        return gc->pdf->beginGroup();
    gc->state.groups++;
    gc->canvas->beginGroup();
}

// 67
void gcEndGroup(reef_ui_gc* gc)
{
    if (gc->pdf)
        return gc->pdf->endGroup();
    if (gc->state.groups)
        gc->state.groups--;
    gc->canvas->endGroup();
}

// 148
void gcFillColor(reef_ui_gc* gc, double* color)
{
    if (gc->pdf)
        return gc->pdf->fillColor(color);
    const ui::ColorF& fill = gc->canvas->fillColor();
    color[0] = fill.red;
    color[1] = fill.green;
    color[2] = fill.blue;
    color[3] = fill.alpha;
}

// 149
// A group that one of the ends below keeps by the alpha of an image.
void gcBeginMaskedGroup(reef_ui_gc* gc)
{
    if (gc->pdf)
        return gc->pdf->beginMaskedGroup();
    ui::Canvas2D* canvas = gc->canvas.get();
    const double alpha = canvas->globalAlpha();
    gcSave(gc);
    canvas->setGlobalAlpha(1);
    canvas->setComposite(ui::Canvas2D::Composite::SourceOver);
    canvas->setShadow({}, 0, {0, 0, 0, 0});
    canvas->beginGroup();
    gc->state.groups++;
    const GcState state = gc->state;
    canvas->setGlobalAlpha(alpha);
    gcSetComposite(gc, state.composite);
    setShadow(gc, state.shadow);
}

// Of what was drawn, only what `draw` covers with the mask in the rectangle stays.
template<typename Draw>
static void endMaskedGroup(reef_ui_gc* gc, const double* matrix, double x, double y, double width, double height, bool masked, Draw draw)
{
    ui::Canvas2D* canvas = gc->canvas.get();
    canvas->save();
    canvas->setTransform(transformOf(matrix));
    canvas->setGlobalAlpha(1);
    canvas->setShadow({}, 0, {0, 0, 0, 0});
    canvas->setComposite(ui::Canvas2D::Composite::DestinationIn);
    if (masked) {
        canvas->setAntialias(false);
        ui::Path outside;
        outside.addRect({-1e6, -1e6, 2e6, 2e6});
        outside.addRect({x, y, width, height});
        outside.setFillRule(ui::Path::FillRule::EvenOdd);
        canvas->setFillColor({0, 0, 0, 1});
        canvas->setComposite(ui::Canvas2D::Composite::DestinationOut);
        canvas->fillPath(outside);
        canvas->setComposite(ui::Canvas2D::Composite::DestinationIn);
        draw(canvas);
    } else {
        canvas->clearRect({-1e6, -1e6, 2e6, 2e6});
    }
    canvas->restore();
    canvas->endGroup();
    gcRestore(gc);
}

// 258
int gcEndGroupMaskedByImage(reef_ui_gc* gc, reef_ui_image* image, double x, double y, double width, double height, const double* matrix,
                            int mirrored)
{
    if (gc->pdf || !image)
        return 0;
    endMaskedGroup(gc, matrix, x, y, width, height, true,
                   [&](ui::Canvas2D* canvas) { canvas->drawBitmap(image->bitmap, {x, y, width, height}, mirrored); });
    return 1;
}

// 274
int gcClipToImage(reef_ui_gc* gc, reef_ui_image* image, double x, double y, double width, double height, int mirrored)
{
    if (gc->pdf || !image)
        return 0;
    gc->canvas->clipToBitmap(image->bitmap, {x, y, width, height}, mirrored);
    return 1;
}

// 168
reef_ui_gc* pdfCreate(const reef_ui_pdf_document* document)
{
    if (!document || !document->write)
        return nullptr;
    auto gc = new reef_ui_gc;
    gc->pdf = std::make_unique<PdfWriter>(*document);
    return gc;
}

// 169
void pdfBeginPage(reef_ui_gc* gc, const double* mediaBox, const double* boxes)
{
    if (gc && gc->pdf)
        gc->pdf->beginPage(mediaBox, boxes);
}

// 170
void pdfEndPage(reef_ui_gc* gc)
{
    if (gc && gc->pdf)
        gc->pdf->endPage();
}

// 171
void pdfAddLink(reef_ui_gc* gc, double x, double y, double width, double height, const char* url, const char* destination)
{
    if (gc && gc->pdf)
        gc->pdf->addLink(x, y, width, height, url, destination);
}

// 172
void pdfAddDestination(reef_ui_gc* gc, const char* name, double x, double y)
{
    if (gc && gc->pdf)
        gc->pdf->addDestination(name, x, y);
}

// 173
void pdfBeginTag(reef_ui_gc* gc, const char* type, const char* actualText, const char* alternateText, const char* title,
                 const char* language)
{
    if (gc && gc->pdf)
        gc->pdf->beginTag(type, actualText, alternateText, title, language);
}

// 174
void pdfEndTag(reef_ui_gc* gc)
{
    if (gc && gc->pdf)
        gc->pdf->endTag();
}

// 175
void pdfSetOutline(reef_ui_gc* gc, const reef_ui_pdf_outline_item* items, unsigned count)
{
    if (gc && gc->pdf)
        gc->pdf->setOutline(items, count);
}

// 176
void pdfSetMetadata(reef_ui_gc* gc, const char* bytes, size_t size)
{
    if (gc && gc->pdf)
        gc->pdf->setMetadata(bytes, size);
}

// 177
void pdfClose(reef_ui_gc* gc)
{
    if (gc && gc->pdf)
        gc->pdf->close();
}

// 62
// Format 1 is RGBA, others BGRA.
void gcDrawPixels(reef_ui_gc* gc, const void* pixels, int width, int height, size_t stride, int format, double x, double y,
                  double drawnWidth, double drawnHeight, int mirrored)
{
    if (gc->pdf)
        return gc->pdf->drawPixels(pixels, width, height, stride, format, x, y, drawnWidth, drawnHeight, mirrored);
    const ui::Bitmap bitmap = ui::Bitmap::wrap(const_cast<void*>(pixels), width, height, stride,
                                               format == 1 ? ui::Bitmap::Format::Rgba8888Premultiplied
                                                           : ui::Bitmap::Format::Argb32Premultiplied);
    gc->canvas->drawBitmap(bitmap, {x, y, drawnWidth, drawnHeight}, mirrored);
}

// 70
void gcDestroy(reef_ui_gc* gc)
{
    delete gc;
}

// 178
void pdfDestroy(reef_ui_gc* gc)
{
    delete gc;
}

// 150
// Everything goes without pixels.
void gcEndGroupMaskedByPixels(reef_ui_gc* gc, const void* pixels, int width, int height, size_t stride, double x, double y,
                              double drawnWidth, double drawnHeight, const double* matrix, int mirrored)
{
    if (gc->pdf)
        return gc->pdf->endMaskedGroup(pixels, width, height, stride, x, y, drawnWidth, drawnHeight, matrix, mirrored);
    endMaskedGroup(gc, matrix, x, y, drawnWidth, drawnHeight, pixels && width > 0 && height > 0, [&](ui::Canvas2D* canvas) {
        const ui::Bitmap bitmap = ui::Bitmap::wrap(const_cast<void*>(pixels), width, height, stride, ui::Bitmap::Format::Argb32Premultiplied);
        canvas->drawBitmap(bitmap, {x, y, drawnWidth, drawnHeight}, mirrored);
    });
}

// 69
reef_ui_gc* gcCreateForPixels(void* pixels, int width, int height, size_t stride, int format)
{
    auto gc = new reef_ui_gc;
    gc->bitmap = ui::Bitmap::wrap(pixels, width, height, stride,
                                  format == 1 ? ui::Bitmap::Format::Rgba8888Premultiplied : ui::Bitmap::Format::Argb32Premultiplied);
    gc->canvas = std::make_unique<ui::Canvas2D>(gc->bitmap);
    if (!gc->canvas->isValid()) {
        delete gc;
        return nullptr;
    }
    return gc;
}

} // namespace cutefish::reef
