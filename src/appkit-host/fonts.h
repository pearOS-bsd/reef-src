// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Faces of fonts: which one a program gets, what it knows about it, and its glyphs on a canvas.

#pragma once

#include "ui/canvas_2d.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

struct reef_ui_face;
struct reef_ui_path_element;

// What a program asks for; null, 0 or -1 for what does not matter.
struct reef_ui_face_query
{
    // The PostScript name or the full name.
    const char* name;
    const char* family;
    const char* style;
    // 100 to 900.
    double weight;
    // 1 to 9.
    double width;
    int italic;
    int fixedPitch;
    const char* language;
};

// In units of the em.
struct reef_ui_face_metrics
{
    double unitsPerEm;
    double ascent;
    double descent;
    double leading;
    double capHeight;
    double xHeight;
    // Degrees.
    double italicAngle;
    double underlinePosition;
    double underlineThickness;
    // Left, bottom, right and top.
    double bounds[4];
    double weight;
    double width;
    uint32_t glyphCount;
    // Of the FaceFlags.
    uint32_t flags;
};

struct reef_ui_face_axis
{
    uint32_t tag;
    int hidden;
    double minimum;
    double standard;
    double maximum;
    const char* name;
};

// As hb_feature_t.
struct reef_ui_feature
{
    uint32_t tag;
    uint32_t value;
    uint32_t start;
    uint32_t end;
};

struct reef_ui_glyph
{
    uint32_t glyph;
    uint32_t cluster;
    double xAdvance;
    double yAdvance;
    double xOffset;
    double yOffset;
};

namespace cutefish::reef {

enum FaceFlags : uint32_t {
    FaceItalic = 0x1,
    FaceBold = 0x2,
    FaceFixedPitch = 0x4,
    FaceColor = 0x8,
    FaceScalable = 0x10,
    FaceVertical = 0x20,
    FaceVariable = 0x40,
    // What the face lacks and is made of its outlines.
    FaceMadeBold = 0x80,
    FaceMadeOblique = 0x100,
};

// For faceShape.
enum ShapeFlags {
    ShapeRightToLeft = 1,
    ShapeTopToBottom = 2,
};

// For drawGlyphs.
enum GlyphFlags {
    GlyphsAliased = 1,
    // Not through the cache of glyphs.
    GlyphsAsPaths = 2,
};

// The files that hold the outlines of a face.
enum OutlineFormat {
    OutlinesNone,
    OutlinesTrueType,
    OutlinesCff,
};

// `exact` is 1 when the face has the name or the family that was asked for, or one that stands in for it.
reef_ui_face* faceMatch(const reef_ui_face_query* query, int* exact);
// One face on a line: file, index, PostScript name, family, style, weight, width, whether it is slanted and whether it has a fixed pitch,
// with tabs between; all of them without a query. Returns the length.
int faceList(const reef_ui_face_query* query, char* buffer, int size);
reef_ui_face* faceFromFile(const char* path, int index);
reef_ui_face* faceFromData(const void* data, size_t size, int index);
void faceRetain(reef_ui_face* face);
void faceRelease(reef_ui_face* face);
void faceMetrics(reef_ui_face* face, reef_ui_face_metrics* metrics);
// A name of the name table, or the file for -1. Returns the length, -1 for none.
int faceName(reef_ui_face* face, int id, char* buffer, int size);
void faceGlyphs(reef_ui_face* face, const uint32_t* characters, uint32_t count, uint32_t* glyphs);
void faceAdvances(reef_ui_face* face, const uint32_t* glyphs, uint32_t count, int vertical, double* advances);
// Four numbers for each: x, y, width and height, from the baseline up.
void faceBounds(reef_ui_face* face, const uint32_t* glyphs, uint32_t count, double* bounds);
uint32_t faceOutline(reef_ui_face* face, uint32_t glyph, reef_ui_path_element* elements, uint32_t capacity);
int faceGlyphName(reef_ui_face* face, uint32_t glyph, char* buffer, int size);
uint32_t faceGlyphNamed(reef_ui_face* face, const char* name);
// Returns the size of the table, -1 for none; nothing is written into a buffer that is too small.
size_t faceTable(reef_ui_face* face, uint32_t tag, void* buffer, size_t size);
uint32_t faceTables(reef_ui_face* face, uint32_t* tags, uint32_t capacity);
// The characters with a glyph: first and last of each range.
uint32_t faceCharacterRanges(reef_ui_face* face, uint32_t* ranges, uint32_t capacity);
// One language on a line. Returns the length.
int faceLanguages(reef_ui_face* face, char* buffer, int size);
// A face for a character that the face lacks.
reef_ui_face* faceFallback(reef_ui_face* face, uint32_t character, const char* language);
// Returns how many glyphs there are, also when they do not fit.
uint32_t faceShape(reef_ui_face* face, const uint16_t* text, uint32_t length, uint32_t start, uint32_t count, double size, int flags,
                   uint32_t script, const char* language, const reef_ui_feature* features, uint32_t featureCount, reef_ui_glyph* glyphs,
                   uint32_t capacity);
uint32_t faceAxes(reef_ui_face* face, reef_ui_face_axis* axes, uint32_t capacity);
// Where the face is on its axes.
uint32_t faceCoordinates(reef_ui_face* face, uint32_t* tags, double* values, uint32_t capacity);
// The same face at another place on its axes.
reef_ui_face* faceWithVariations(reef_ui_face* face, const uint32_t* tags, const double* values, uint32_t count);
// Both return 0 when they did it, 1 otherwise.
int fontRegister(const char* path);
int fontUnregister(const char* path);

// What makes outlines bold and oblique, in ems and as the tangent of the angle.
double madeObliqueSlant();
double madeBoldStrength();
// How much wider than the font's own the glyphs are.
double faceStretch(reef_ui_face* face);
// How a PDF document can embed it.
OutlineFormat faceOutlineFormat(reef_ui_face* face);
// The first character of each glyph.
std::map<uint32_t, uint32_t> faceCharacters(reef_ui_face* face);
// The font file with the glyphs alone, empty when that is not possible.
std::string faceSubset(reef_ui_face* face, const std::vector<uint32_t>& glyphs, OutlineFormat* format);
// As in CSS: "liga=0,ss01", with ranges.
std::vector<reef_ui_feature> parseFeatures(const char* text);
// The matrix is that of the glyphs, as four numbers; the positions are pairs.
void drawGlyphs(ui::Canvas2D& canvas, reef_ui_face* face, const double* matrix, const uint32_t* glyphs, const double* positions,
                uint32_t count, int flags);

} // namespace cutefish::reef
