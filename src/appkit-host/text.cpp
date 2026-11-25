// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Single lines of text in one font, their metrics, and what a drawing context does with text.

#include "text.h"

#include "fonts.h"
#include "graphics.h"
#include "host.h"
#include "paragraph.h"
#include "pdf.h"

#include "ui/glyph_run.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <tuple>

namespace cutefish::reef {

static std::string familyOrSystem(const char* family)
{
    return family && *family ? family : systemFontFamily();
}

static ui::Color toColor(double red, double green, double blue, double alpha)
{
    const auto component = [](double value) { return uint8_t(std::lround(std::clamp(value, 0.0, 1.0) * 255)); };
    return {component(red), component(green), component(blue), component(alpha)};
}

// 71
reef_ui_text* textCreate(const char* text, const char* family, double size, int bold, int italic, double scale, double red, double green,
                         double blue, double alpha, int kerned)
{
    const std::lock_guard lock(textMutex());
    auto result = new reef_ui_text;
    const ui::FontSpec font = {installedFamily(familyOrSystem(family)), size * 72 / 96, bold != 0, italic != 0};
    const std::string name = familyOrSystem(family);
    const bool hinted = !fixedFamily(name);
    result->layout = ui::TextLayout(text ? text : "", font, scale > 0 ? scale : 1, hinted);
    if (!kerned) {
        const double spacing = letterSpacing(name, size);
        if (spacing != 0)
            result->layout.setLetterSpacing(spacing);
    }
    result->text = text ? text : "";
    result->family = font.family;
    result->size = size;
    result->bold = bold;
    result->italic = italic;
    const double color[] = {red, green, blue, alpha};
    std::copy(color, color + 4, result->color);
    result->layout.setDefaultColor(toColor(red, green, blue, alpha));
    result->layout.setOpaqueColorGlyphs(true);
    result->layout.beginLayout();
    result->layout.createLine(1e7, {});
    result->layout.endLayout();
    if (result->layout.lineCount() > 0) {
        const double shift = baselineShift(name, size, result->layout.lineAt(0).ascent, scale);
        if (shift != 0) {
            result->layout.beginLayout();
            result->layout.createLine(1e7, {0, -shift});
            result->layout.endLayout();
        }
        const ui::TextLayout::Line& line = result->layout.lineAt(0);
        result->width = line.naturalTextWidth;
        result->ascent = line.ascent - shift;
        result->descent = lineDescent(name, size, line.ascent, line.descent, scale);
    }
    return result;
}

// 72
void textDestroy(reef_ui_text* text)
{
    const std::lock_guard lock(textMutex());
    delete text;
}

// 73
void textMetrics(reef_ui_text* text, double* width, double* ascent, double* descent)
{
    const std::lock_guard lock(textMutex());
    *width = text->width;
    *ascent = text->ascent;
    *descent = text->descent;
}

// 74
double textOffsetToX(reef_ui_text* text, size_t offset)
{
    const std::lock_guard lock(textMutex());
    return text->layout.lineCount() ? text->layout.cursorToX(0, offset) : 0;
}

// 75
size_t textXToOffset(reef_ui_text* text, double x)
{
    const std::lock_guard lock(textMutex());
    return text->layout.lineCount() ? text->layout.xToCursor(0, x) : 0;
}

// 76
void textFontMetrics(const char* family, double size, int bold, int italic, double scale, double* ascent, double* descent, double* leading)
{
    struct Metrics
    {
        double ascent;
        double descent;
        double leading;
    };
    static std::mutex mutex;
    static std::map<std::tuple<std::string, double, int, int, double>, Metrics> cache;
    const std::tuple<std::string, double, int, int, double> key(family ? family : "", size, bold, italic, scale);
    {
        const std::lock_guard lock(mutex);
        const auto it = cache.find(key);
        if (it != cache.end()) {
            *ascent = it->second.ascent;
            *descent = it->second.descent;
            *leading = it->second.leading;
            return;
        }
    }
    const ui::FontSpec font = {installedFamily(familyOrSystem(family)), size * 72 / 96, bold != 0, italic != 0};
    const ui::FontMetrics metrics(font, scale > 0 ? scale : 1);
    const std::string name = familyOrSystem(family);
    const double shift = baselineShift(name, size, metrics.ascentF(), scale);
    *ascent = metrics.ascentF() - shift;
    *descent = lineDescent(name, size, metrics.ascentF(), metrics.descentF(), scale);
    *leading = metrics.leadingF();
    if (const FixedFamily* fixed = fixedFamily(name)) {
        *ascent = std::round(size * fixed->ascent);
        *leading = std::round(size * fixed->leading);
    }
    const std::lock_guard lock(mutex);
    cache.emplace(key, Metrics{*ascent, *descent, *leading});
}

// 97
int textFontFamilies(char* buffer, int size)
{
    std::string text;
    for (const ui::FontFamily& family : ui::fontFamilies()) {
        text += family.name;
        text += family.fixedPitch ? "\t1\n" : "\t0\n";
    }
    return copyString(text, buffer, size);
}

namespace {

// Where y goes up, text is drawn as if it went down, from the point that (x, y) is.
class UprightText
{
public:
    UprightText(PdfWriter* pdf, double& x, double& y)
        : m_pdf(pdf)
    {
        pdf->transform(m_matrix);
        const double determinant = m_matrix[0] * m_matrix[3] - m_matrix[1] * m_matrix[2];
        if (determinant >= 0)
            return;
        const double scale = std::sqrt(-determinant);
        const double matrix[] = {scale, 0, 0, scale, m_matrix[0] * x + m_matrix[2] * y + m_matrix[4],
                                 m_matrix[1] * x + m_matrix[3] * y + m_matrix[5]};
        pdf->setTransform(matrix);
        m_changed = true;
        x = 0;
        y = 0;
    }

    ~UprightText()
    {
        if (m_changed)
            m_pdf->setTransform(m_matrix);
    }

    UprightText(const UprightText&) = delete;
    UprightText& operator=(const UprightText&) = delete;

private:
    PdfWriter* m_pdf;
    double m_matrix[6];
    bool m_changed = false;
};

} // namespace

// 68
void gcDrawText(reef_ui_gc* gc, reef_ui_text* text, double x, double y)
{
    const std::lock_guard lock(textMutex());
    if (gc->pdf) {
        if (!text->layout.lineCount())
            return;
        const UprightText upright(gc->pdf.get(), x, y);
        PdfTextSpan span;
        span.end = text->text.size();
        span.family = text->family;
        span.size = text->size;
        span.bold = text->bold;
        span.italic = text->italic;
        std::copy(text->color, text->color + 4, span.color);
        const ui::TextLayout::Line& line = text->layout.lineAt(0);
        span.baseline = y + line.position.y + line.ascent;
        span.x = [text, x](size_t offset) { return text->layout.cursorToX(0, offset) + x; };
        gc->pdf->drawText(text->text, {span}, [text, x, y](ui::Canvas2D& canvas) { canvas.drawText(text->layout, {x, y}); });
        return;
    }
    gc->canvas->drawText(text->layout, {x, y});
}

// 96
void gcDrawParagraph(reef_ui_gc* gc, reef_ui_paragraph* paragraph, double x, double y)
{
    if (gc->pdf) {
        const UprightText upright(gc->pdf.get(), x, y);
        gc->pdf->drawText(paragraphText(paragraph), paragraphSpans(paragraph, x, y),
                          [paragraph, x, y](ui::Canvas2D& canvas) { drawParagraph(canvas, paragraph, x, y); });
        return;
    }
    drawParagraph(*gc->canvas, paragraph, x, y);
}

// 147
void gcDrawGlyphs(reef_ui_gc* gc, reef_ui_face* face, const double* matrix, const uint32_t* glyphs, const double* positions,
                  uint32_t count, int flags)
{
    if (gc->pdf)
        gc->pdf->drawGlyphs(face, matrix, glyphs, positions, count, flags);
    else
        drawGlyphs(*gc->canvas, face, matrix, glyphs, positions, count, flags);
}

} // namespace cutefish::reef
