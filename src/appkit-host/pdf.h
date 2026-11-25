// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A drawing context that writes a PDF document.

#pragma once

#include "ui/canvas_2d.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

struct reef_ui_face;
struct reef_ui_path_element;

// What a program gives for a document. Strings may be null.
struct reef_ui_pdf_document
{
    void (*write)(void* context, const void* bytes, size_t size);
    void* context;
    const char* title;
    const char* author;
    const char* subject;
    const char* keywords;
    const char* creator;
    // Seconds since 1970, or 0 for now.
    double date;
    // 16 bytes.
    const char* identifier;
    // The dictionary that says how strings and streams are encrypted, and what does it; the program gives memory of malloc.
    const char* encryption;
    char* (*encrypt)(void* context, int object, int generation, const char* bytes, size_t size, size_t* encryptedSize);
    // PDF/A-2b, which is never encrypted.
    int archival;
    // Dictionaries; %PROFILE% stands for the profile, which is the one of sRGB without another.
    const char* outputIntents;
    const char* profile;
    size_t profileSize;
    int profileComponents;
};

// The items of the outline come one after another, each below the last one that has a lower level.
struct reef_ui_pdf_outline_item
{
    const char* title;
    int level;
    // Counted from 1.
    int page;
    // NaN where the viewer stays, and for the top of the page.
    double x;
    double y;
    // In place of a page.
    const char* url;
};

namespace cutefish::reef {

// Text of one style on one line.
struct PdfTextSpan
{
    // Bytes of the text.
    size_t begin = 0;
    size_t end = 0;
    std::string family;
    double size = 13;
    bool bold = false;
    bool italic = false;
    double color[4] = {0, 0, 0, 1};
    bool underline = false;
    // As in CSS.
    std::string features;
    double baseline = 0;
    // Where a byte of the text is.
    std::function<double(size_t)> x;
};

class PdfWriter
{
public:
    explicit PdfWriter(const reef_ui_pdf_document& document);
    ~PdfWriter();
    PdfWriter(const PdfWriter&) = delete;
    PdfWriter& operator=(const PdfWriter&) = delete;

    // The media box, and the crop, bleed, trim and art box after each other: x, y, width and height.
    void beginPage(const double* mediaBox, const double* boxes);
    void endPage();
    void close();

    void addLink(double x, double y, double width, double height, const char* url, const char* destination);
    void addDestination(const char* name, double x, double y);
    // What is drawn until the end belongs to the element.
    void beginTag(const char* type, const char* actualText, const char* alternateText, const char* title, const char* language);
    void endTag();
    void setOutline(const reef_ui_pdf_outline_item* items, unsigned count);
    // XMP.
    void setMetadata(const char* bytes, size_t size);

    void save();
    void restore();
    void concat(const double* matrix);
    void setTransform(const double* matrix);
    void transform(double* matrix) const;
    void clipToRect(double x, double y, double width, double height);
    void clipToPath(const reef_ui_path_element* elements, unsigned count, int evenOdd);
    void clipBounds(double* rect) const;

    void setAlpha(double alpha);
    void setComposite(int composite);
    void setSmoothing(int smoothing);
    void setFillColor(double red, double green, double blue, double alpha);
    void setStrokeColor(double red, double green, double blue, double alpha);
    void fillColor(double* color) const;
    void setLineWidth(double width);
    void setLineCap(int cap);
    void setLineJoin(int join);
    void setMiterLimit(double limit);
    void setLineDash(const double* lengths, unsigned count, double offset);
    void setShadow(double dx, double dy, double blur, double red, double green, double blue, double alpha);

    void fillPath(const reef_ui_path_element* elements, unsigned count, int evenOdd);
    void strokePath(const reef_ui_path_element* elements, unsigned count);
    void fillRect(double x, double y, double width, double height);
    void strokeRect(double x, double y, double width, double height);
    void clearRect(double x, double y, double width, double height);
    void drawPixels(const void* pixels, int width, int height, size_t stride, int format, double x, double y, double drawnWidth,
                    double drawnHeight, int mirrored);
    // Bit 0 of `extend` goes on before the start, bit 1 after the end.
    void fillLinearGradient(const double* offsets, const double* colors, unsigned count, double x0, double y0, double x1, double y1,
                            int extend);
    void fillRadialGradient(const double* offsets, const double* colors, unsigned count, double x0, double y0, double r0, double x1,
                            double y1, double r1, int extend);
    // `draw` draws the same on a canvas, for shadows.
    void drawText(const std::string& text, const std::vector<PdfTextSpan>& spans, const std::function<void(ui::Canvas2D&)>& draw);
    void drawGlyphs(reef_ui_face* face, const double* matrix, const uint32_t* glyphs, const double* positions, uint32_t count, int flags);

    void beginGroup();
    void endGroup();
    // A group through the alpha of an image.
    void beginMaskedGroup();
    void endMaskedGroup(const void* pixels, int width, int height, size_t stride, double x, double y, double drawnWidth, double drawnHeight,
                        const double* matrix, int mirrored);

private:
    struct Private;
    std::unique_ptr<Private> d;
};

} // namespace cutefish::reef
