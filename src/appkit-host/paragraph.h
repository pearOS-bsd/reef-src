// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Paragraphs of text in several fonts, and the metrics of the fonts that programs expect.

#pragma once

#include "pdf.h"

#include "ui/canvas_2d.h"

#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

struct reef_ui_paragraph;

// Offsets and lengths are bytes of the paragraph's text.
struct reef_ui_text_run
{
    size_t start;
    size_t length;
    const char* family;
    double size;
    int bold;
    int italic;
    double red;
    double green;
    double blue;
    double alpha;
    int underline;
    // Not set as far apart as in the font the program expects.
    int kerned;
    // OpenType features, separated by commas.
    const char* features;
};

struct reef_ui_paragraph_style
{
    // 0 for none.
    double width;
    double firstLineIndent;
    double indent;
    // From the left; when it is not above 0, from the right.
    double tailIndent;
    const double* tabs;
    unsigned tabCount;
    // 1 right, 2 center.
    int alignment;
    double tabInterval;
    // 1 at words, 2 at characters.
    int wraps;
    double lineSpacing;
    double minimumLineHeight;
    double maximumLineHeight;
    double lineHeightMultiple;
    // Of the device it is drawn on.
    double scale;
};

struct reef_ui_paragraph_line
{
    size_t start;
    size_t length;
    double x;
    double y;
    double height;
    double baseline;
    double width;
};

namespace cutefish::reef {

// The metrics of a fixed pitch font that the program asks for, in ems, and the family that stands in for it.
struct FixedFamily
{
    const char* name;
    const char* substitute;
    double ascent;
    double descent;
    double leading;
    double advance;
};

// The layouts of the toolkit are not for several threads at once.
std::recursive_mutex& textMutex();

// Null for any other family, and for one that is installed.
const FixedFamily* fixedFamily(const std::string& family);
// The family, or what stands in for it.
std::string installedFamily(const std::string& family);
// How much higher the baseline is in a line that is as high as with the font the program expects.
double baselineShift(const std::string& family, double size, double ascent, double scale);
// What is below the baseline in such a line.
double lineDescent(const std::string& family, double size, double ascent, double descent, double scale);
// In ems, what is added to the advances to be those of the font the program expects.
double letterSpacing(const std::string& family, double size);

reef_ui_paragraph* paragraphCreate(const char* text, const reef_ui_text_run* runs, unsigned count, const reef_ui_paragraph_style* style);
void paragraphDestroy(reef_ui_paragraph* paragraph);
unsigned paragraphLineCount(reef_ui_paragraph* paragraph);
void paragraphLine(reef_ui_paragraph* paragraph, unsigned index, reef_ui_paragraph_line* line);
double paragraphOffsetToX(reef_ui_paragraph* paragraph, unsigned line, size_t offset);
size_t paragraphXToOffset(reef_ui_paragraph* paragraph, unsigned line, double x);

const std::string& paragraphText(reef_ui_paragraph* paragraph);
// With the top left at the point.
void drawParagraph(ui::Canvas2D& canvas, reef_ui_paragraph* paragraph, double x, double y);
std::vector<PdfTextSpan> paragraphSpans(reef_ui_paragraph* paragraph, double x, double y);

} // namespace cutefish::reef
