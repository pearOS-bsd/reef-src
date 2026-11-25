// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Paragraphs of text in several fonts, and the metrics of the fonts that programs expect.

#include "paragraph.h"

#include "fonts.h"

#include "ui/glyph_run.h"
#include "ui/text_layout.h"
#include "ui/theme.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <memory>

namespace {

// A run with font features, which the layouts of the toolkit do not take.
struct ShapedRun
{
    struct Group
    {
        reef_ui_face* face = nullptr;
        std::vector<uint32_t> glyphs;
        // Two for each glyph.
        std::vector<double> positions;
    };

    ~ShapedRun()
    {
        for (const Group& group : groups)
            cutefish::reef::faceRelease(group.face);
    }

    std::vector<Group> groups;
    // Where each byte of the text is, and its end.
    std::vector<double> offsets;
    double size = 0;
    cutefish::ui::ColorF color;
    bool underline = false;
    double underlinePosition = 0;
    double underlineThickness = 1;
};

// The whole text of a run in one line; the lines of the paragraph show parts of it.
struct Piece
{
    size_t start = 0;
    size_t end = 0;
    cutefish::ui::TextLayout layout;
    std::shared_ptr<ShapedRun> shaped;
    double ascent = 0;
    double descent = 0;
    double shift = 0;
    double depth = 0;

    // In the line of the piece; offsets outside of it are at its ends.
    double offsetToX(size_t offset) const
    {
        const size_t local = std::min(offset, end) - std::min(offset, start);
        if (shaped) {
            const std::vector<double>& offsets = shaped->offsets;
            return offsets.empty() ? 0 : offsets[std::min(local, offsets.size() - 1)];
        }
        return layout.lineCount() ? layout.cursorToX(0, local) : 0;
    }

    size_t xToOffset(double x) const
    {
        size_t offset = start;
        if (shaped) {
            const std::vector<double>& offsets = shaped->offsets;
            size_t nearest = 0;
            for (size_t i = 0; i < offsets.size(); i++) {
                if (std::abs(offsets[i] - x) < std::abs(offsets[nearest] - x))
                    nearest = i;
            }
            offset += nearest;
        } else if (layout.lineCount()) {
            offset += layout.xToCursor(0, x);
        }
        return offset;
    }
};

// The part of a piece that a line shows.
struct Fragment
{
    size_t piece = 0;
    size_t start = 0;
    size_t end = 0;
    double x = 0;
    double width = 0;
};

struct Line
{
    size_t start = 0;
    size_t end = 0;
    double x = 0;
    double y = 0;
    double height = 0;
    double baseline = 0;
    double width = 0;
    std::vector<Fragment> fragments;
};

// What documents are to know of a run.
struct Run
{
    size_t start = 0;
    size_t end = 0;
    std::string family;
    double size = 13;
    bool bold = false;
    bool italic = false;
    double color[4] = {0, 0, 0, 1};
    bool underline = false;
    std::string features;
};

} // namespace

struct reef_ui_paragraph
{
    std::string text;
    std::vector<Run> runs;
    // One font without features: the layout is that of the paragraph, and there are no pieces.
    bool single = true;
    cutefish::ui::TextLayout layout;
    std::vector<Piece> pieces;
    std::vector<Line> lines;
};

namespace cutefish::reef {

static constexpr FixedFamily fixedFamilies[] = {
    {"monospace", "monospace", 0.966797, 0.210938, 0, 0.618164},
    {"Menlo", "monospace", 0.928223, 0.23584, 0, 0.602051},
    {"Monaco", "monospace", 1, 0.25, 0.083496, 0.600098},
    {"Courier", "monospace", 0.753906, 0.246094, 0, 0.600098},
};

static std::recursive_mutex layoutMutex;

std::recursive_mutex& textMutex()
{
    return layoutMutex;
}

static std::string familyOrSystem(const char* name)
{
    const std::string family = name && *name ? std::string(name) : ui::Theme::current().fontFamily;
    return family.empty() ? std::string("sans-serif") : family;
}

static bool isSystemFamily(const std::string& family)
{
    const std::string& system = ui::Theme::current().fontFamily;
    return family == (system.empty() ? std::string("sans-serif") : system);
}

const FixedFamily* fixedFamily(const std::string& family)
{
    static std::mutex mutex;
    static std::map<std::string, const FixedFamily*> cache;
    const std::lock_guard lock(mutex);
    const auto it = cache.find(family);
    if (it != cache.end())
        return it->second;
    const FixedFamily* found = nullptr;
    for (const FixedFamily& fixed : fixedFamilies) {
        if (family != fixed.name)
            continue;
        int exact = 0;
        reef_ui_face_query query = {};
        query.family = family.c_str();
        query.weight = 400;
        query.width = 5;
        if (reef_ui_face* face = faceMatch(&query, &exact))
            faceRelease(face);
        if (family == "monospace" || !exact)
            found = &fixed;
    }
    cache.emplace(family, found);
    return found;
}

std::string installedFamily(const std::string& family)
{
    const FixedFamily* fixed = fixedFamily(family);
    return fixed ? fixed->substitute : family;
}

double baselineShift(const std::string& family, double size, double ascent, double scale)
{
    scale = scale > 0 ? scale : 1;
    double expected;
    if (const FixedFamily* fixed = fixedFamily(family))
        expected = std::round(size * fixed->ascent);
    else if (isSystemFamily(family))
        expected = size * 0.952;
    else
        return 0;
    return std::round((ascent - expected) * scale) / scale;
}

static ui::FontSpec fontSpec(const reef_ui_text_run& run)
{
    return {installedFamily(familyOrSystem(run.family)), run.size * 72 / 96, run.bold != 0, run.italic != 0};
}

// The stand-ins are laid out with advances that are not rounded.
static bool hintsLayout(const reef_ui_text_run& run)
{
    return !fixedFamily(familyOrSystem(run.family));
}

// Of the system's font, in ems.
struct SystemMetrics
{
    // 0 if the font does not tell.
    double capHeight = 0;
    // As in Noto Sans.
    double capHeightOfAscent = 0.714 / 1.069;
    double advance = 0;
};

static const SystemMetrics& systemMetrics(const std::string& family)
{
    static std::mutex mutex;
    static std::map<std::string, SystemMetrics> cache;
    const std::lock_guard lock(mutex);
    const auto it = cache.find(family);
    if (it != cache.end())
        return it->second;
    SystemMetrics metrics;
    reef_ui_face_query query = {};
    query.family = family.c_str();
    query.weight = 400;
    query.width = 5;
    if (reef_ui_face* face = faceMatch(&query, nullptr)) {
        reef_ui_face_metrics found = {};
        faceMetrics(face, &found);
        if (found.capHeight > 0 && found.unitsPerEm > 0)
            metrics.capHeight = found.capHeight / found.unitsPerEm;
        faceRelease(face);
    }
    static const char text[] = "45 bytesCreatedEncodingInspectorLines: 2On startup:Unicode (UTF-8)Reopen windows from last session";
    ui::TextLayout layout(text, {family, 75, false, false}, 1, false);
    layout.beginLayout();
    layout.createLine(1e7, {});
    layout.endLayout();
    metrics.advance = layout.maximumWidth() / 100 / (sizeof text - 1);
    return cache.emplace(family, metrics).first->second;
}

double lineDescent(const std::string& family, double size, double ascent, double descent, double scale)
{
    if (const FixedFamily* fixed = fixedFamily(family))
        return std::round(size * fixed->descent);
    const double shift = baselineShift(family, size, ascent, scale);
    if (!isSystemFamily(family))
        return descent + shift;
    scale = scale > 0 ? scale : 1;
    const SystemMetrics& metrics = systemMetrics(family);
    const double capHeight = metrics.capHeight > 0 ? metrics.capHeight * size : ascent * metrics.capHeightOfAscent;
    return ascent - shift - std::round(capHeight * scale) / scale;
}

// Of a digit, in ems.
static double digitAdvance(const std::string& family)
{
    ui::TextLayout layout("0000000000", {family, 75, false, false}, 1, false);
    layout.beginLayout();
    layout.createLine(1e7, {});
    layout.endLayout();
    return layout.maximumWidth() / 100 / 10;
}

// What the program's system font has on average at the size, in ems.
static double expectedAdvance(double size)
{
    static constexpr struct
    {
        double size;
        double advance;
    } table[] = {{9, 0.52774}, {10, 0.5209}, {11, 0.51504}, {12, 0.50918}, {13, 0.50332}, {15, 0.49356}};
    constexpr size_t last = std::size(table) - 1;
    if (size <= table[0].size)
        return table[0].advance;
    if (size >= table[last].size)
        return table[last].advance;
    for (size_t i = 0; i < last; i++) {
        const auto& before = table[i];
        const auto& after = table[i + 1];
        if (size <= after.size)
            return before.advance + (size - before.size) / (after.size - before.size) * (after.advance - before.advance);
    }
    return table[last].advance;
}

double letterSpacing(const std::string& family, double size)
{
    if (const FixedFamily* fixed = fixedFamily(family)) {
        static std::mutex mutex;
        static std::map<std::string, double> advances;
        const std::lock_guard lock(mutex);
        auto it = advances.find(fixed->substitute);
        if (it == advances.end())
            it = advances.emplace(fixed->substitute, digitAdvance(fixed->substitute)).first;
        return it->second > 0 ? fixed->advance - it->second : 0;
    }
    if (!isSystemFamily(family) || size <= 0)
        return 0;
    const SystemMetrics& metrics = systemMetrics(family);
    return metrics.advance > 0 ? expectedAdvance(size) - metrics.advance : 0;
}

static bool isRightToLeft(uint32_t c)
{
    return (c >= 0x590 && c <= 0x8ff) || (c >= 0xfb1d && c <= 0xfdff) || (c >= 0xfe70 && c <= 0xfeff) || (c >= 0x10800 && c <= 0x10fff)
            || (c >= 0x1e800 && c <= 0x1efff);
}

static std::shared_ptr<ShapedRun> shapeRun(const std::string& text, const reef_ui_text_run& run, double tab)
{
    auto shaped = std::make_shared<ShapedRun>();
    shaped->size = run.size > 0 ? run.size : 13;
    shaped->color = {run.red, run.green, run.blue, run.alpha};
    shaped->underline = run.underline != 0;
    const std::vector<reef_ui_feature> features = parseFeatures(run.features);
    const std::string family = fontSpec(run).family;
    reef_ui_face_query query = {};
    query.family = family.c_str();
    query.weight = run.bold ? 700 : 400;
    query.width = -1;
    query.italic = run.italic != 0;
    query.fixedPitch = -1;
    reef_ui_face* face = faceMatch(&query, nullptr);
    shaped->offsets.assign(text.size() + 1, 0);
    if (!face)
        return shaped;
    reef_ui_face_metrics metrics = {};
    faceMetrics(face, &metrics);
    const double unitsPerEm = metrics.unitsPerEm > 0 ? metrics.unitsPerEm : 1000;
    shaped->underlinePosition = -metrics.underlinePosition / unitsPerEm * shaped->size;
    shaped->underlineThickness = std::max(metrics.underlineThickness / unitsPerEm * shaped->size, 1.0);

    std::vector<uint32_t> characters;
    std::vector<size_t> starts;
    for (size_t i = 0; i < text.size();) {
        const unsigned char lead = text[i];
        size_t length = lead < 0x80 ? 1 : (lead & 0xe0) == 0xc0 ? 2 : (lead & 0xf0) == 0xe0 ? 3 : 4;
        length = std::min(length, text.size() - i);
        uint32_t c = length == 1 ? lead : lead & (0xff >> (length + 1));
        for (size_t k = 1; k < length; k++)
            c = c << 6 | (text[i + k] & 0x3f);
        characters.push_back(c);
        starts.push_back(i);
        i += length;
    }
    starts.push_back(text.size());

    // Control characters have none.
    std::vector<reef_ui_face*> faces(characters.size(), nullptr);
    for (size_t i = 0; i < characters.size(); i++) {
        const uint32_t c = characters[i];
        if (c < 0x20 || c == 0x7f)
            continue;
        uint32_t glyph = 0;
        faceGlyphs(face, &c, 1, &glyph);
        reef_ui_face* fallback = glyph ? nullptr : faceFallback(face, c, nullptr);
        if (fallback)
            faceRelease(fallback);
        faces[i] = fallback ? fallback : face;
    }

    double x = 0;
    for (size_t i = 0; i < characters.size();) {
        if (!faces[i]) {
            shaped->offsets[starts[i]] = x;
            if (characters[i] == '\t' && tab > 0)
                x = (std::floor(x / tab + 1e-6) + 1) * tab;
            for (size_t k = starts[i] + 1; k <= starts[i + 1]; k++)
                shaped->offsets[k] = x;
            i++;
            continue;
        }
        const bool rightToLeft = isRightToLeft(characters[i]);
        size_t end = i + 1;
        while (end < characters.size() && faces[end] == faces[i] && isRightToLeft(characters[end]) == rightToLeft)
            end++;
        std::u16string utf16;
        // The character of each unit.
        std::vector<size_t> owners;
        for (size_t k = i; k < end; k++) {
            uint32_t c = characters[k];
            if (c > 0xffff) {
                c -= 0x10000;
                utf16.push_back(char16_t(0xd800 + (c >> 10)));
                utf16.push_back(char16_t(0xdc00 + (c & 0x3ff)));
                owners.push_back(k);
                owners.push_back(k);
            } else {
                utf16.push_back(char16_t(c));
                owners.push_back(k);
            }
        }
        std::vector<reef_ui_glyph> glyphs((utf16.size() + 4) * 2);
        uint32_t count = 0;
        for (int attempt = 0; attempt < 2; attempt++) {
            count = faceShape(faces[i], reinterpret_cast<const uint16_t*>(utf16.data()), uint32_t(utf16.size()), 0, uint32_t(utf16.size()),
                              shaped->size, rightToLeft ? ShapeRightToLeft : 0, 0, nullptr, features.data(), uint32_t(features.size()),
                              glyphs.data(), uint32_t(glyphs.size()));
            if (count <= glyphs.size())
                break;
            glyphs.resize(count);
        }
        count = std::min(count, uint32_t(glyphs.size()));
        ShapedRun::Group group;
        faceRetain(faces[i]);
        group.face = faces[i];
        std::vector<double> advances(end - i, 0);
        double pen = x;
        double width = 0;
        for (uint32_t k = 0; k < count; k++) {
            const reef_ui_glyph& glyph = glyphs[k];
            group.glyphs.push_back(glyph.glyph);
            group.positions.push_back(pen + glyph.xOffset);
            group.positions.push_back(-glyph.yOffset);
            pen += glyph.xAdvance;
            width += glyph.xAdvance;
            const size_t owner = glyph.cluster < owners.size() ? owners[glyph.cluster] : i;
            advances[owner - i] += glyph.xAdvance;
        }
        double along = 0;
        for (size_t k = i; k < end; k++) {
            const double left = rightToLeft ? x + width - along : x + along;
            for (size_t byte = starts[k]; byte < starts[k + 1]; byte++)
                shaped->offsets[byte] = left;
            along += advances[k - i];
        }
        x += width;
        shaped->offsets[starts[end]] = rightToLeft ? x - width : x;
        shaped->groups.push_back(std::move(group));
        i = end;
    }
    shaped->offsets[text.size()] = x;
    faceRelease(face);
    return shaped;
}

static ui::Color runColor(const reef_ui_text_run& run)
{
    const auto component = [](double value) { return uint8_t(std::lround(std::clamp(value, 0.0, 1.0) * 255)); };
    return {component(run.red), component(run.green), component(run.blue), component(run.alpha)};
}

static double lineIndent(const reef_ui_paragraph_style& style, size_t line)
{
    return line == 0 ? style.firstLineIndent : style.indent;
}

static double availableWidth(const reef_ui_paragraph_style& style, size_t line)
{
    const double right = style.tailIndent > 0 ? style.tailIndent : style.width + style.tailIndent;
    return std::max(right - lineIndent(style, line), 1.0);
}

static double lineWidth(const reef_ui_paragraph_style& style, size_t line)
{
    if (style.width <= 0 || !style.wraps)
        return 1e7;
    return availableWidth(style, line);
}

static double lineHeight(const reef_ui_paragraph_style& style, double height)
{
    if (style.lineHeightMultiple > 0)
        height *= style.lineHeightMultiple;
    if (style.minimumLineHeight > 0)
        height = std::max(height, style.minimumLineHeight);
    if (style.maximumLineHeight > 0)
        height = std::min(height, style.maximumLineHeight);
    return height;
}

static bool isBlank(char c)
{
    return c == ' ' || c == '\t';
}

// Without the blanks at the end.
static double visibleWidth(const std::string& text, size_t start, size_t end, double width, const std::function<double(size_t)>& offsetToX)
{
    size_t visible = end;
    while (visible > start && isBlank(text[visible - 1]))
        visible--;
    return visible == end ? width : offsetToX(visible);
}

static double alignedOffset(const reef_ui_paragraph_style& style, size_t line, double width)
{
    if (style.width <= 0 || style.width >= 1e6)
        return 0;
    const double available = availableWidth(style, line);
    switch (style.alignment) {
    case 1:
        return std::max(available - width, 0.0);
    case 2:
        return std::max((available - width) / 2, 0.0);
    default:
        return 0;
    }
}

static void layOutSingle(reef_ui_paragraph* paragraph, const reef_ui_text_run* runs, unsigned count, const reef_ui_paragraph_style& style)
{
    const reef_ui_text_run& first = runs[0];
    const double scale = style.scale > 0 ? style.scale : 1;
    paragraph->layout = ui::TextLayout(paragraph->text, fontSpec(first), scale, hintsLayout(first));
    ui::TextLayout& layout = paragraph->layout;
    if (!first.kerned) {
        const double spacing = letterSpacing(familyOrSystem(first.family), first.size);
        if (spacing != 0)
            layout.setLetterSpacing(spacing);
    }
    layout.setWrap(!style.wraps || style.width <= 0 ? ui::TextLayout::Wrap::None : ui::TextLayout::Wrap::WordOrAnywhere);
    double tab = style.tabInterval;
    if (style.tabCount > 1)
        tab = style.tabs[1] - style.tabs[0];
    else if (style.tabCount == 1)
        tab = style.tabs[0];
    layout.setTabStopDistance(std::max(tab, 0.0));
    std::vector<ui::TextFormatRange> formats;
    for (unsigned i = 0; i < count; i++) {
        const reef_ui_text_run& run = runs[i];
        ui::TextFormatRange format;
        format.start = run.start;
        format.length = run.length;
        format.color = runColor(run);
        format.bold = run.bold != 0;
        format.italic = run.italic != 0;
        format.underline = run.underline != 0;
        formats.push_back(format);
    }
    layout.setFormats(std::move(formats));
    layout.setDefaultColor(runColor(first));
    layout.setOpaqueColorGlyphs(true);
    paragraph->lines.clear();

    layout.beginLayout();
    for (size_t i = 0; layout.createLine(lineWidth(style, i), {}) >= 0; i++) {
    }
    layout.endLayout();

    // Placed once the height of the lines is known.
    std::vector<double> ascents;
    const std::string family = familyOrSystem(first.family);
    double y = 0;
    for (int i = 0; i < layout.lineCount(); i++) {
        const ui::TextLayout::Line& laid = layout.lineAt(i);
        Line line;
        line.start = laid.textStart;
        line.end = laid.textStart + laid.textLength;
        line.width = laid.naturalTextWidth;
        const double shift = baselineShift(family, first.size, laid.ascent, scale);
        const double descent = lineDescent(family, first.size, laid.ascent, laid.descent, scale);
        line.height = lineHeight(style, laid.ascent - shift + descent);
        line.baseline = line.height - descent;
        const double visible = visibleWidth(paragraph->text, line.start, line.end, line.width,
                                            [&layout, i](size_t offset) { return layout.cursorToX(i, offset); });
        line.x = lineIndent(style, i) + alignedOffset(style, i, visible);
        line.y = y;
        y += line.height + style.lineSpacing;
        paragraph->lines.push_back(line);
        ascents.push_back(laid.ascent);
    }
    layout.beginLayout();
    for (size_t i = 0; i < paragraph->lines.size(); i++) {
        const Line& line = paragraph->lines[i];
        layout.createLine(lineWidth(style, i), {line.x, line.y + line.baseline - ascents[i]});
    }
    layout.endLayout();
}

static void layOutRuns(reef_ui_paragraph* paragraph, const reef_ui_text_run* runs, unsigned count, const reef_ui_paragraph_style& style)
{
    paragraph->single = false;
    const double scale = style.scale > 0 ? style.scale : 1;
    std::vector<Piece>& pieces = paragraph->pieces;
    for (unsigned i = 0; i < count; i++) {
        const reef_ui_text_run& run = runs[i];
        Piece piece;
        piece.start = std::min(paragraph->text.size(), run.start);
        piece.end = std::min(paragraph->text.size(), run.start + run.length);
        if (run.features && *run.features) {
            piece.shaped = shapeRun(paragraph->text.substr(piece.start, piece.end - piece.start), run, std::max(style.tabInterval, 0.0));
            const ui::FontMetrics metrics(fontSpec(run), scale);
            piece.ascent = metrics.ascentF();
            piece.descent = metrics.descentF();
            piece.shift = baselineShift(familyOrSystem(run.family), run.size, piece.ascent, scale);
            piece.depth = lineDescent(familyOrSystem(run.family), run.size, piece.ascent, piece.descent, scale);
            pieces.push_back(std::move(piece));
            continue;
        }
        piece.layout = ui::TextLayout(paragraph->text.substr(piece.start, piece.end - piece.start), fontSpec(run), scale, hintsLayout(run));
        if (!run.kerned) {
            const double spacing = letterSpacing(familyOrSystem(run.family), run.size);
            if (spacing != 0)
                piece.layout.setLetterSpacing(spacing);
        }
        piece.layout.setTabStopDistance(std::max(style.tabInterval, 0.0));
        ui::TextFormatRange format;
        format.length = piece.end - piece.start;
        format.underline = run.underline != 0;
        format.color = runColor(run);
        piece.layout.setFormats({format});
        piece.layout.setDefaultColor(runColor(run));
        piece.layout.setOpaqueColorGlyphs(true);
        piece.layout.beginLayout();
        piece.layout.createLine(1e7, {});
        piece.layout.endLayout();
        if (piece.layout.lineCount() > 0) {
            piece.ascent = piece.layout.lineAt(0).ascent;
            piece.descent = piece.layout.lineAt(0).descent;
        }
        piece.shift = baselineShift(familyOrSystem(run.family), run.size, piece.ascent, scale);
        piece.depth = lineDescent(familyOrSystem(run.family), run.size, piece.ascent, piece.descent, scale);
        pieces.push_back(std::move(piece));
    }

    const auto pieceAt = [&](size_t offset) -> size_t {
        for (size_t i = 0; i < pieces.size(); i++) {
            if (offset < pieces[i].end)
                return i;
        }
        return pieces.empty() ? 0 : pieces.size() - 1;
    };
    const auto offsetToX = [&](size_t index, size_t offset) { return pieces[index].offsetToX(offset); };
    const auto width = [&](size_t start, size_t end) {
        double total = 0;
        size_t position = start;
        while (position < end) {
            const size_t index = pieceAt(position);
            const size_t next = std::min(end, pieces[index].end);
            total += offsetToX(index, next) - offsetToX(index, position);
            if (next == position)
                break;
            position = next;
        }
        return total;
    };

    const std::string& text = paragraph->text;
    const size_t size = text.size();
    size_t start = 0;
    double y = 0;
    do {
        const size_t number = paragraph->lines.size();
        const double available = lineWidth(style, number);
        size_t end = start;
        double x = 0;
        while (end < size) {
            const size_t index = pieceAt(end);
            const double advance = offsetToX(index, pieces[index].end) - offsetToX(index, end);
            if (x + advance <= available) {
                x += advance;
                end = pieces[index].end;
                continue;
            }
            const double left = available - x;
            size_t fitting = pieces[index].xToOffset(offsetToX(index, end) + left);
            while (fitting > end && offsetToX(index, fitting) - offsetToX(index, end) > left)
                fitting--;
            end = std::max(end, fitting);
            if (end < size) {
                size_t word = end;
                while (word > start && !isBlank(text[word - 1]))
                    word--;
                if (word > start && style.wraps != 2)
                    end = word;
                while (end < size && isBlank(text[end]))
                    end++;
                if (end == start)
                    end++;
                while (end < size && (text[end] & 0xc0) == 0x80)
                    end++;
            }
            break;
        }

        Line line;
        line.start = start;
        line.end = end;
        x = 0;
        double ascent = 0;
        double descent = 0;
        size_t position = start;
        while (position < end) {
            const size_t index = pieceAt(position);
            const Piece& piece = pieces[index];
            const size_t next = std::min(end, piece.end);
            const double advance = offsetToX(index, next) - offsetToX(index, position);
            ascent = std::max(ascent, piece.ascent - piece.shift);
            descent = std::max(descent, piece.depth);
            line.fragments.push_back({index, position, next, x, advance});
            x += advance;
            if (next == position)
                break;
            position = next;
        }
        if (line.fragments.empty() && !pieces.empty()) {
            ascent = pieces[pieceAt(start)].ascent - pieces[pieceAt(start)].shift;
            descent = pieces[pieceAt(start)].depth;
        }
        line.width = x;
        line.height = lineHeight(style, ascent + descent);
        line.baseline = line.height - descent;
        const double visible = visibleWidth(text, start, end, x, [&](size_t offset) { return width(start, offset); });
        line.x = lineIndent(style, number) + alignedOffset(style, number, visible);
        line.y = y;
        y += line.height + style.lineSpacing;
        paragraph->lines.push_back(line);
        start = end;
    } while (start < size);
}

// 90
reef_ui_paragraph* paragraphCreate(const char* text, const reef_ui_text_run* runs, unsigned count, const reef_ui_paragraph_style* style)
{
    const std::lock_guard lock(layoutMutex);
    auto paragraph = new reef_ui_paragraph;
    paragraph->text = text ? text : "";
    reef_ui_text_run whole = {};
    whole.length = paragraph->text.size();
    whole.size = 13;
    whole.alpha = 1;
    if (!count) {
        runs = &whole;
        count = 1;
    }
    for (unsigned i = 0; i < count; i++) {
        const reef_ui_text_run& run = runs[i];
        Run copy;
        copy.start = std::min(paragraph->text.size(), run.start);
        copy.end = std::min(paragraph->text.size(), run.start + run.length);
        copy.family = fontSpec(run).family;
        copy.size = run.size;
        copy.bold = run.bold != 0;
        copy.italic = run.italic != 0;
        copy.color[0] = run.red;
        copy.color[1] = run.green;
        copy.color[2] = run.blue;
        copy.color[3] = run.alpha;
        copy.underline = run.underline != 0;
        copy.features = run.features ? run.features : "";
        paragraph->runs.push_back(copy);
    }
    bool single = true;
    for (unsigned i = 0; i < count; i++)
        single &= !(runs[i].features && *runs[i].features);
    for (unsigned i = 1; i < count; i++) {
        const ui::FontSpec first = fontSpec(runs[0]);
        const ui::FontSpec font = fontSpec(runs[i]);
        single &= first.family == font.family && first.pointSize == font.pointSize;
    }
    if (single)
        layOutSingle(paragraph, runs, count, *style);
    else
        layOutRuns(paragraph, runs, count, *style);
    return paragraph;
}

// 91
void paragraphDestroy(reef_ui_paragraph* paragraph)
{
    const std::lock_guard lock(layoutMutex);
    delete paragraph;
}

// 92
unsigned paragraphLineCount(reef_ui_paragraph* paragraph)
{
    const std::lock_guard lock(layoutMutex);
    return unsigned(paragraph->lines.size());
}

// 93
void paragraphLine(reef_ui_paragraph* paragraph, unsigned index, reef_ui_paragraph_line* line)
{
    const std::lock_guard lock(layoutMutex);
    if (index >= paragraph->lines.size()) {
        *line = {};
        return;
    }
    const Line& found = paragraph->lines[index];
    line->start = found.start;
    line->length = found.end - found.start;
    line->x = found.x;
    line->y = found.y;
    line->height = found.height;
    line->baseline = found.baseline;
    line->width = found.width;
}

// 94
double paragraphOffsetToX(reef_ui_paragraph* paragraph, unsigned index, size_t offset)
{
    const std::lock_guard lock(layoutMutex);
    if (index >= paragraph->lines.size())
        return 0;
    if (paragraph->single)
        return int(index) < paragraph->layout.lineCount() ? paragraph->layout.cursorToX(int(index), offset) : 0;
    const Line& line = paragraph->lines[index];
    for (const Fragment& fragment : line.fragments) {
        if (offset >= fragment.start && offset <= fragment.end) {
            const Piece& piece = paragraph->pieces[fragment.piece];
            return line.x + fragment.x + piece.offsetToX(offset) - piece.offsetToX(fragment.start);
        }
    }
    return line.x + (offset > line.start ? line.width : 0);
}

// 95
size_t paragraphXToOffset(reef_ui_paragraph* paragraph, unsigned index, double x)
{
    const std::lock_guard lock(layoutMutex);
    if (index >= paragraph->lines.size())
        return 0;
    const Line& line = paragraph->lines[index];
    if (paragraph->single)
        return int(index) < paragraph->layout.lineCount() ? paragraph->layout.xToCursor(int(index), x) : line.start;
    x -= line.x;
    for (const Fragment& fragment : line.fragments) {
        if (x < fragment.x + fragment.width || &fragment == &line.fragments.back()) {
            const Piece& piece = paragraph->pieces[fragment.piece];
            const size_t offset = piece.xToOffset(x - fragment.x + piece.offsetToX(fragment.start));
            return std::clamp(offset, fragment.start, fragment.end);
        }
    }
    return line.start;
}

const std::string& paragraphText(reef_ui_paragraph* paragraph)
{
    const std::lock_guard lock(layoutMutex);
    return paragraph->text;
}

static void drawShaped(ui::Canvas2D& canvas, const ShapedRun& shaped, double x, double baseline)
{
    const double matrix[] = {shaped.size, 0, 0, -shaped.size};
    canvas.save();
    canvas.setFillColor(shaped.color);
    for (const ShapedRun::Group& group : shaped.groups) {
        std::vector<double> positions = group.positions;
        for (size_t i = 0; i < positions.size(); i += 2) {
            positions[i] += x;
            positions[i + 1] += baseline;
        }
        drawGlyphs(canvas, group.face, matrix, group.glyphs.data(), positions.data(), uint32_t(group.glyphs.size()), 0);
    }
    if (shaped.underline && !shaped.offsets.empty())
        canvas.fillRect({x, baseline + shaped.underlinePosition, shaped.offsets.back(), shaped.underlineThickness});
    canvas.restore();
}

void drawParagraph(ui::Canvas2D& canvas, reef_ui_paragraph* paragraph, double x, double y)
{
    const std::lock_guard lock(layoutMutex);
    if (paragraph->single) {
        canvas.drawText(paragraph->layout, {x, y});
        return;
    }
    const base::AffineTransform transform = canvas.transform();
    const double determinant = transform.determinant();
    canvas.save();
    // Upright where y goes up.
    if (determinant < 0) {
        const double scale = std::sqrt(-determinant);
        canvas.setTransform({scale, 0, 0, scale, transform.m11() * x + transform.m21() * y + transform.dx(),
                             transform.m12() * x + transform.m22() * y + transform.dy()});
        x = 0;
        y = 0;
    }
    for (const Line& line : paragraph->lines) {
        for (const Fragment& fragment : line.fragments) {
            Piece& piece = paragraph->pieces[fragment.piece];
            const double start = piece.offsetToX(fragment.start);
            // The last glyph may reach beyond its advance.
            const double extra = fragment.end < piece.end ? 0.0 : 1.0;
            canvas.save();
            canvas.clipToRect({x + line.x + fragment.x, y + line.y, fragment.width + extra, line.height});
            const double left = x + line.x + fragment.x - start;
            const double top = y + line.y + line.baseline - piece.ascent;
            if (piece.shaped)
                drawShaped(canvas, *piece.shaped, left, top + piece.ascent);
            else
                canvas.drawText(piece.layout, {left, top});
            canvas.restore();
        }
    }
    canvas.restore();
}

static PdfTextSpan makeSpan(const Run& run, size_t begin, size_t end)
{
    PdfTextSpan span;
    span.begin = begin;
    span.end = end;
    span.family = run.family;
    span.size = run.size;
    span.bold = run.bold;
    span.italic = run.italic;
    std::copy(run.color, run.color + 4, span.color);
    span.underline = run.underline;
    span.features = run.features;
    return span;
}

std::vector<PdfTextSpan> paragraphSpans(reef_ui_paragraph* paragraph, double x, double y)
{
    const std::lock_guard lock(layoutMutex);
    std::vector<PdfTextSpan> spans;
    if (paragraph->single) {
        const ui::TextLayout& layout = paragraph->layout;
        for (int i = 0; i < layout.lineCount(); i++) {
            const ui::TextLayout::Line& line = layout.lineAt(i);
            const size_t end = line.textStart + line.textLength;
            // Where runs overlap, the later one is drawn.
            std::vector<int> owners(line.textLength, -1);
            for (size_t run = 0; run < paragraph->runs.size(); run++) {
                for (size_t k = std::max(paragraph->runs[run].start, line.textStart); k < std::min(paragraph->runs[run].end, end); k++)
                    owners[k - line.textStart] = int(run);
            }
            for (size_t k = 0; k < owners.size();) {
                size_t next = k + 1;
                while (next < owners.size() && owners[next] == owners[k])
                    next++;
                const Run& run = paragraph->runs[std::max(owners[k], 0)];
                PdfTextSpan span = makeSpan(run, line.textStart + k, line.textStart + next);
                span.baseline = y + line.position.y + line.ascent;
                span.x = [&layout, i, x](size_t offset) { return layout.cursorToX(i, offset) + x; };
                spans.push_back(std::move(span));
                k = next;
            }
        }
        return spans;
    }
    for (const Line& line : paragraph->lines) {
        for (const Fragment& fragment : line.fragments) {
            if (fragment.piece >= paragraph->runs.size() || fragment.end <= fragment.start)
                continue;
            const Piece& piece = paragraph->pieces[fragment.piece];
            const double start = piece.offsetToX(fragment.start);
            PdfTextSpan span = makeSpan(paragraph->runs[fragment.piece], fragment.start, fragment.end);
            span.baseline = y + line.y + line.baseline;
            const double left = x + line.x + fragment.x - start;
            span.x = [&piece, left](size_t offset) { return piece.offsetToX(offset) + left; };
            spans.push_back(std::move(span));
        }
    }
    return spans;
}

} // namespace cutefish::reef
