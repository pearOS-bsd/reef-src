// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Single lines of text in one font, their metrics, and what a drawing context does with text.

#pragma once

#include "ui/text_layout.h"

#include <cstddef>
#include <cstdint>
#include <string>

struct reef_ui_face;
struct reef_ui_gc;
struct reef_ui_paragraph;

struct reef_ui_text
{
    cutefish::ui::TextLayout layout;
    double width = 0;
    double ascent = 0;
    double descent = 0;
    std::string text;
    std::string family;
    double size = 0;
    bool bold = false;
    bool italic = false;
    double color[4] = {0, 0, 0, 1};
};

namespace cutefish::reef {

// The sizes are points of 72 to the inch, as large as the logical pixels of the toolkit; without a family the text is in
// the system's font. `scale` is that of the device it is drawn on. Only where `kerned` is 0 the letters are set as far
// apart as in the font the program expects.
reef_ui_text* textCreate(const char* text, const char* family, double size, int bold, int italic, double scale, double red, double green,
                         double blue, double alpha, int kerned);
void textDestroy(reef_ui_text* text);
void textMetrics(reef_ui_text* text, double* width, double* ascent, double* descent);
// Offsets are bytes.
double textOffsetToX(reef_ui_text* text, size_t offset);
size_t textXToOffset(reef_ui_text* text, double x);
void textFontMetrics(const char* family, double size, int bold, int italic, double scale, double* ascent, double* descent, double* leading);
// A line for each family: its name, a tab, 1 if all its glyphs are as wide as each other, or 0. Returns the length.
int textFontFamilies(char* buffer, int size);

// The top left of the text goes to the point, upright whatever the direction of y.
void gcDrawText(reef_ui_gc* gc, reef_ui_text* text, double x, double y);
void gcDrawParagraph(reef_ui_gc* gc, reef_ui_paragraph* paragraph, double x, double y);
void gcDrawGlyphs(reef_ui_gc* gc, reef_ui_face* face, const double* matrix, const uint32_t* glyphs, const double* positions,
                  uint32_t count, int flags);

} // namespace cutefish::reef
