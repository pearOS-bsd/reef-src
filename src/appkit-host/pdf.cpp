// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PDF documents. What PDF has no operator for, shadows and glyphs without outlines, becomes images.

#include "pdf.h"

#include "fonts.h"
#include "paths.h"

#include "ui/bitmap.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <random>
#include <set>
#include <utility>
#include <zlib.h>

namespace cutefish::reef {

// The first point is x' = a x + c y + e, y' = b x + d y + f.
struct PdfMatrix
{
    double a = 1;
    double b = 0;
    double c = 0;
    double d = 1;
    double e = 0;
    double f = 0;

    // The other one comes first.
    PdfMatrix operator*(const PdfMatrix& other) const;
    PdfMatrix inverted() const;
    void map(double x, double y, double* mappedX, double* mappedY) const;
    double scale() const { return std::sqrt(std::abs(a * d - b * c)); }
};

struct PdfRect
{
    double x = 0;
    double y = 0;
    double width = 0;
    double height = 0;

    bool isEmpty() const { return width <= 0 || height <= 0; }
    PdfRect intersected(const PdfRect& other) const;
    PdfRect united(const PdfRect& other) const;
    // The bounds of the mapped corners.
    PdfRect mapped(const PdfMatrix& matrix) const;
};

namespace {

struct State
{
    PdfMatrix transform;
    double fill[4] = {0, 0, 0, 1};
    double stroke[4] = {0, 0, 0, 1};
    double alpha = 1;
    int composite = 0;
    double lineWidth = 1;
    int lineCap = 0;
    int lineJoin = 0;
    double miterLimit = 10;
    std::vector<double> dashes;
    double dashOffset = 0;
    // The offset, the blur and the color.
    double shadow[7] = {};
    bool smoothing = true;
    // On the page.
    PdfRect clip;
};

// The operators of a page, or of a group on it.
struct Layer
{
    std::string content;
    // The names of what the operators use.
    std::set<std::string> resources;
    // States that were saved and not restored yet.
    int saves = 0;
    // How many states there were when the group began.
    size_t depth = 0;
};

struct Group
{
    double alpha = 1;
    int composite = 0;
    double shadow[7] = {};
    bool masked = false;
    // It is also drawn into a bitmap, for its shadow.
    bool raster = false;
};

struct Raster
{
    ui::Bitmap bitmap;
    std::unique_ptr<ui::Canvas2D> canvas;
    double scale = 2;
};

struct Link
{
    PdfRect rect;
    std::string url;
    std::string destination;
};

struct Page
{
    int object = 0;
    PdfRect mediaBox;
    // Crop, bleed, trim and art box.
    PdfRect boxes[4];
    std::vector<Link> links;
    // The elements of the marked contents, by identifier.
    std::vector<int> marks;
};

struct Font
{
    reef_ui_face* face = nullptr;
    std::string name;
    int object = 0;
    int format = OutlinesNone;
    std::set<uint32_t> glyphs;
    // What glyphs stand for.
    std::map<uint32_t, std::u32string> texts;
    reef_ui_face_metrics metrics = {};
};

// Of the structure of the document.
struct Element
{
    int object = 0;
    int parent = -1;
    std::string type;
    std::string actualText;
    std::string alternateText;
    std::string title;
    std::string language;
    // Page and identifier.
    std::vector<std::pair<int, int>> marks;
    std::vector<int> children;
    // Children, and marks as -1, -2 and so on.
    std::vector<int> kids;
};

} // namespace

static std::string text(const char* value)
{
    return value ? value : "";
}

static std::string number(double value)
{
    if (!std::isfinite(value) || std::abs(value) < 0.000005)
        return "0";
    char buffer[64];
    snprintf(buffer, sizeof buffer, "%.5f", value);
    std::string text = buffer;
    while (!text.empty() && text.back() == '0')
        text.pop_back();
    if (!text.empty() && text.back() == '.')
        text.pop_back();
    if (text == "-0")
        text = "0";
    return text;
}

static std::string numbers(std::initializer_list<double> values)
{
    std::string text;
    for (double value : values) {
        if (!text.empty())
            text += ' ';
        text += number(value);
    }
    return text;
}

static std::string numbers(const PdfMatrix& matrix)
{
    return numbers({matrix.a, matrix.b, matrix.c, matrix.d, matrix.e, matrix.f});
}

static std::string rectangle(const PdfRect& rect)
{
    return "[" + numbers({rect.x, rect.y, rect.x + rect.width, rect.y + rect.height}) + "]";
}

static std::string array(const std::vector<double>& values)
{
    std::string text;
    for (double value : values)
        text += (text.empty() ? "" : " ") + number(value);
    return "[" + text + "]";
}

static const char hexDigits[] = "0123456789ABCDEF";

static std::string name(const std::string& text)
{
    std::string result = "/";
    for (char c : text) {
        if (c > ' ' && c < 0x7f && !strchr("#()<>[]{}/%", c)) {
            result.push_back(c);
        } else {
            result += '#';
            result += hexDigits[uint8_t(c) >> 4];
            result += hexDigits[uint8_t(c) & 15];
        }
    }
    return result;
}

static std::string hexString(const std::string& bytes)
{
    std::string text = "<";
    for (char c : bytes) {
        text += hexDigits[uint8_t(c) >> 4];
        text += hexDigits[uint8_t(c) & 15];
    }
    return text + ">";
}

static std::string xml(const std::string& text)
{
    std::string result;
    for (char c : text) {
        switch (c) {
        case '<':
            result += "&lt;";
            break;
        case '>':
            result += "&gt;";
            break;
        case '&':
            result += "&amp;";
            break;
        case '"':
            result += "&quot;";
            break;
        default:
            result += c;
        }
    }
    return result;
}

// What is not UTF-8 becomes U+FFFD. The offsets are the bytes where the characters begin, and the size of the text.
static std::u32string decoded(const std::string& text, std::vector<size_t>* offsets = nullptr)
{
    std::u32string result;
    for (size_t i = 0; i < text.size();) {
        const char byte = text[i];
        char32_t c = byte;
        size_t size = 1;
        if (byte < 0) {
            c = 0xfffd;
            if ((byte & 0xe0) == 0xc0) {
                if (i + 1 < text.size()) {
                    c = (byte & 0x1f) << 6 | (text[i + 1] & 0x3f);
                    size = 2;
                }
            } else if ((byte & 0xf0) == 0xe0) {
                if (i + 2 < text.size()) {
                    c = (byte & 0x0f) << 12 | (text[i + 1] & 0x3f) << 6 | (text[i + 2] & 0x3f);
                    size = 3;
                }
            } else if ((byte & 0xf8) == 0xf0 && i + 3 < text.size()) {
                c = (byte & 0x07) << 18 | (text[i + 1] & 0x3f) << 12 | (text[i + 2] & 0x3f) << 6 | (text[i + 3] & 0x3f);
                size = 4;
            }
        }
        if (offsets)
            offsets->push_back(i);
        result += c;
        i += size;
    }
    if (offsets)
        offsets->push_back(text.size());
    return result;
}

static void appendUtf16(std::u16string& text, char32_t c)
{
    if (c >= 0x10000) {
        text += char16_t(0xd800 + ((c - 0x10000) >> 10));
        text += char16_t(0xdc00 + ((c - 0x10000) & 0x3ff));
    } else {
        text += char16_t(c);
    }
}

// The bytes of a string object: ASCII as it is, other text as UTF-16.
static std::string encoded(const std::string& text)
{
    bool ascii = true;
    for (char c : text)
        ascii &= c >= ' ' && c < 0x7f;
    if (ascii)
        return text;
    std::string result = "\xfe\xff";
    std::u16string units;
    for (char32_t c : decoded(text))
        appendUtf16(units, c);
    for (char16_t unit : units) {
        result += char(unit >> 8);
        result += char(unit);
    }
    return result;
}

static std::string dateText(double seconds, bool iso)
{
    const time_t time = seconds > 0 ? time_t(seconds) : ::time(nullptr);
    tm parts = {};
    gmtime_r(&time, &parts);
    char buffer[64];
    if (iso)
        strftime(buffer, sizeof buffer, "%Y-%m-%dT%H:%M:%SZ", &parts);
    else
        strftime(buffer, sizeof buffer, "D:%Y%m%d%H%M%SZ", &parts);
    return buffer;
}

static std::string deflated(const std::string& data)
{
    z_stream stream = {};
    if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK)
        return {};
    std::string result;
    result.resize(deflateBound(&stream, data.size()));
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));
    stream.avail_in = uInt(data.size());
    stream.next_out = reinterpret_cast<Bytef*>(result.data());
    stream.avail_out = uInt(result.size());
    deflate(&stream, Z_FINISH);
    result.resize(stream.total_out);
    deflateEnd(&stream);
    return result;
}

static void put32(std::string& data, uint32_t value)
{
    data += char(value >> 24);
    data += char(value >> 16);
    data += char(value >> 8);
    data += char(value);
}

// The ICC profile of sRGB, with a curve of 1024 values.
static std::string srgbProfile()
{
    std::string description = "desc";
    put32(description, 0);
    const std::string label = "sRGB IEC61966-2.1";
    put32(description, uint32_t(label.size()) + 1);
    description += label;
    description += '\0';
    put32(description, 0);
    put32(description, 0);
    description += std::string(70, '\0');
    std::string copyright = "text";
    put32(copyright, 0);
    copyright += "No copyright, use freely";
    copyright += '\0';
    std::string curve = "curv";
    put32(curve, 0);
    put32(curve, 1024);
    for (int i = 0; i < 1024; i++) {
        const double x = i / 1023.0;
        const long y = std::lround((x <= 0.04045 ? x / 12.92 : std::pow((x + 0.055) / 1.055, 2.4)) * 65535);
        curve += char(y >> 8);
        curve += char(y);
    }
    const auto padded = [](std::string data) {
        while (data.size() % 4)
            data += '\0';
        return data;
    };
    const auto xyz = [](uint32_t x, uint32_t y, uint32_t z) {
        std::string data = "XYZ ";
        put32(data, 0);
        put32(data, x);
        put32(data, y);
        put32(data, z);
        return data;
    };
    struct Tag
    {
        const char* name;
        std::string data;
    };
    const std::vector<Tag> tags = {
        {"desc", padded(description)},
        {"cprt", padded(copyright)},
        {"wtpt", xyz(0xf6d6, 0x10000, 0xd32d)},
        {"rXYZ", xyz(0x6fa4, 0x38f6, 0x38f)},
        {"gXYZ", xyz(0x6296, 0xb787, 0x18dc)},
        {"bXYZ", xyz(0x24a2, 0xf83, 0xb6cf)},
        {"rTRC", padded(curve)},
    };
    std::string table;
    std::string body;
    const uint32_t count = uint32_t(tags.size()) + 2;
    put32(table, count);
    uint32_t curveOffset = 0;
    for (const Tag& tag : tags) {
        const uint32_t offset = 128 + 4 + count * 12 + uint32_t(body.size());
        table += tag.name;
        put32(table, offset);
        put32(table, uint32_t(tag.data.size()));
        if (strcmp(tag.name, "rTRC") == 0)
            curveOffset = offset;
        body += tag.data;
    }
    // The three colors share the curve.
    for (const char* other : {"gTRC", "bTRC"}) {
        table += other;
        put32(table, curveOffset);
        put32(table, uint32_t(tags.back().data.size()));
    }
    std::string header;
    put32(header, uint32_t(128 + table.size() + body.size()));
    put32(header, 0);
    put32(header, 0x02100000);
    header += "mntrRGB XYZ ";
    for (int part : {2026, 1, 1, 0, 0, 0}) {
        header += char(part >> 8);
        header += char(part);
    }
    header += "acsp";
    header += std::string(28, '\0');
    put32(header, 0xf6d6);
    put32(header, 0x10000);
    put32(header, 0xd32d);
    header.resize(128);
    return header + table + body;
}

// The PostScript name, or else the family, with what a name may have.
static std::string fontName(reef_ui_face* face)
{
    char buffer[256] = {};
    std::string result;
    for (int id : {6, 1}) {
        faceName(face, id, buffer, sizeof buffer);
        for (const char* c = buffer; *c; c++) {
            if (*c > ' ' && *c < 0x7f && !strchr("()<>[]{}/%#", *c))
                result.push_back(*c);
        }
        if (!result.empty())
            break;
    }
    return result.empty() ? "Font" : result;
}

static bool isRightToLeft(char32_t c)
{
    return (c >= 0x590 && c < 0x900) || (c >= 0xfb1d && c < 0xfe00) || (c >= 0xfe70 && c < 0xff00) || (c >= 0x10800 && c < 0x11000)
        || (c >= 0x1e800 && c < 0x1f000);
}

static const char* blendMode(int composite)
{
    static const char* const modes[] = {"Normal",    "Multiply",  "Screen",     "Overlay",   "Darken", "Lighten",    "ColorDodge", "ColorBurn",
                                        "SoftLight", "HardLight", "Difference", "Exclusion", "Hue",    "Saturation", "Color",      "Luminosity"};
    if (composite >= 0 && composite < 16)
        return modes[composite];
    if (composite == 26)
        return "Multiply";
    if (composite == 27)
        return "Screen";
    return "Normal";
}

static bool hasShadow(const State& state)
{
    const double* shadow = state.shadow;
    return shadow[6] > 0 && (shadow[2] > 0 || shadow[0] != 0 || shadow[1] != 0);
}

static ui::ColorF toColor(const double* components)
{
    return {components[0], components[1], components[2], components[3]};
}

static ui::LineCap toLineCap(int cap)
{
    return cap == 1 ? ui::LineCap::Round : cap == 2 ? ui::LineCap::Square : ui::LineCap::Butt;
}

static ui::LineJoin toLineJoin(int join)
{
    return join == 1 ? ui::LineJoin::Round : join == 2 ? ui::LineJoin::Bevel : ui::LineJoin::Miter;
}

static void setStroke(ui::Canvas2D& canvas, const State& state)
{
    canvas.setLineWidth(state.lineWidth);
    canvas.setLineCap(toLineCap(state.lineCap));
    canvas.setLineJoin(toLineJoin(state.lineJoin));
    canvas.setMiterLimit(state.miterLimit);
    canvas.setLineDash(state.dashes, state.dashOffset);
}

// From the page onto a bitmap of an area of it.
static base::AffineTransform onBitmap(const PdfMatrix& matrix, const PdfRect& area, double scale)
{
    return {matrix.a * scale, matrix.b * scale, matrix.c * scale, matrix.d * scale, (matrix.e - area.x) * scale, (matrix.f - area.y) * scale};
}

// Images are upside down in their unit square.
static PdfMatrix imageMatrix(const PdfRect& rect)
{
    return {rect.width, 0, 0, -rect.height, rect.x, rect.y + rect.height};
}

static PdfMatrix toMatrix(const double* matrix)
{
    return {matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]};
}

static std::array<reef_ui_path_element, 5> rectPath(double x, double y, double width, double height)
{
    return {{
        {0, {x, y}},
        {1, {x + width, y}},
        {1, {x + width, y + height}},
        {1, {x, y + height}},
        {4, {}},
    }};
}

static ui::Path toPath(const reef_ui_path_element* elements, unsigned count, int evenOdd)
{
    ui::Path path;
    for (unsigned i = 0; i < count; i++) {
        const double* p = elements[i].points;
        switch (elements[i].type) {
        case 0:
            path.moveTo(p[0], p[1]);
            break;
        case 1:
            path.lineTo(p[0], p[1]);
            break;
        case 2:
            path.quadTo(p[0], p[1], p[2], p[3]);
            break;
        case 3:
            path.cubicTo(p[0], p[1], p[2], p[3], p[4], p[5]);
            break;
        case 4:
            path.close();
            break;
        }
    }
    path.setFillRule(ui::Path::FillRule(evenOdd & 1));
    return path;
}

static std::vector<ui::GradientStopF> gradientStops(const double* offsets, const double* colors, unsigned count)
{
    std::vector<ui::GradientStopF> stops;
    for (unsigned i = 0; i < count; i++)
        stops.push_back({offsets[i], {colors[i * 4], colors[i * 4 + 1], colors[i * 4 + 2], colors[i * 4 + 3]}});
    return stops;
}

// The alpha of the bitmap in the color of the shadow, moved, and blurred by three box filters.
static ui::Bitmap shadowOf(const ui::Bitmap& bitmap, double dx, double dy, double blur, double alpha, const double* shadow)
{
    const int width = bitmap.width();
    const int height = bitmap.height();
    std::vector<float> values(size_t(width) * size_t(height));
    const int offsetX = int(std::lround(dx));
    const int offsetY = int(std::lround(dy));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const int fromX = x - offsetX;
            const int fromY = y - offsetY;
            if (fromX >= 0 && fromY >= 0 && fromX < width && fromY < height)
                values[size_t(y) * width + x] = bitmap.alpha(fromX, fromY) / 255.0f;
        }
    }
    const int radius = int(std::lround(blur * 0.5));
    std::vector<float> line(size_t(std::max(width, height)) + 1);
    if (radius > 0) {
        const int size = radius * 2 + 1;
        for (int pass = 0; pass < 3; pass++) {
            for (int y = 0; y < height; y++) {
                float* row = values.data() + size_t(y) * width;
                double sum = 0;
                for (int x = -radius; x <= radius; x++)
                    sum += x >= 0 && x < width ? row[x] : 0;
                for (int x = 0; x < width; x++) {
                    line[x] = float(sum / size);
                    const float next = x + radius + 1 < width ? row[x + radius + 1] : 0;
                    const float last = x - radius >= 0 ? row[x - radius] : 0;
                    sum += next - last;
                }
                std::copy(line.begin(), line.begin() + width, row);
            }
            for (int x = 0; x < width; x++) {
                float* column = values.data() + x;
                double sum = 0;
                for (int y = -radius; y <= radius; y++)
                    sum += y >= 0 && y < height ? column[ptrdiff_t(y) * width] : 0;
                for (int y = 0; y < height; y++) {
                    line[y] = float(sum / size);
                    const float next = y + radius + 1 < height ? column[ptrdiff_t(y + radius + 1) * width] : 0;
                    const float last = y - radius >= 0 ? column[ptrdiff_t(y - radius) * width] : 0;
                    sum += next - last;
                }
                for (int y = 0; y < height; y++)
                    column[ptrdiff_t(y) * width] = line[y];
            }
        }
    }
    ui::Bitmap result(width, height, ui::Bitmap::Format::Argb32Premultiplied);
    if (result.isNull())
        return result;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const double opacity = std::clamp(values[size_t(y) * width + x] * shadow[6] * alpha, 0.0, 1.0);
            const auto component = [opacity](double value) { return uint32_t(std::lround(255 * (std::clamp(value, 0.0, 1.0) * opacity))); };
            const uint32_t a = uint32_t(std::lround(255 * opacity));
            result.setPixel(x, y, a << 24 | component(shadow[3]) << 16 | component(shadow[4]) << 8 | component(shadow[5]));
        }
    }
    return result;
}

PdfMatrix PdfMatrix::operator*(const PdfMatrix& other) const
{
    return {other.a * a + other.b * c,     other.a * b + other.b * d,     other.c * a + other.d * c, other.c * b + other.d * d,
            other.e * a + other.f * c + e, other.e * b + other.f * d + f};
}

PdfMatrix PdfMatrix::inverted() const
{
    const double determinant = a * d - b * c;
    if (std::abs(determinant) < 1e-12)
        return {};
    return {d / determinant,
            -b / determinant,
            -c / determinant,
            a / determinant,
            (c * f - d * e) / determinant,
            (b * e - a * f) / determinant};
}

void PdfMatrix::map(double x, double y, double* mappedX, double* mappedY) const
{
    *mappedX = a * x + c * y + e;
    *mappedY = b * x + d * y + f;
}

PdfRect PdfRect::intersected(const PdfRect& other) const
{
    const double left = std::max(x, other.x);
    const double top = std::max(y, other.y);
    const double right = std::min(x + width, other.x + other.width);
    const double bottom = std::min(y + height, other.y + other.height);
    if (right <= left || bottom <= top)
        return {left, top, 0, 0};
    return {left, top, right - left, bottom - top};
}

PdfRect PdfRect::united(const PdfRect& other) const
{
    if (isEmpty())
        return other;
    if (other.isEmpty())
        return *this;
    const double left = std::min(x, other.x);
    const double top = std::min(y, other.y);
    const double right = std::max(x + width, other.x + other.width);
    const double bottom = std::max(y + height, other.y + other.height);
    return {left, top, right - left, bottom - top};
}

PdfRect PdfRect::mapped(const PdfMatrix& matrix) const
{
    double xs[4];
    double ys[4];
    matrix.map(x, y, &xs[0], &ys[0]);
    matrix.map(x + width, y, &xs[1], &ys[1]);
    matrix.map(x, y + height, &xs[2], &ys[2]);
    matrix.map(x + width, y + height, &xs[3], &ys[3]);
    const double left = std::min({xs[0], xs[1], xs[2], xs[3]});
    const double top = std::min({ys[0], ys[1], ys[2], ys[3]});
    return {left, top, std::max({xs[0], xs[1], xs[2], xs[3]}) - left, std::max({ys[0], ys[1], ys[2], ys[3]}) - top};
}

struct PdfWriter::Private
{
    reef_ui_pdf_document document = {};
    std::string title;
    std::string author;
    std::string subject;
    std::string keywords;
    std::string creator;
    std::string encryption;
    std::string outputIntents;
    std::string profile;
    std::string identifier;
    // What was written.
    size_t offset = 0;
    // Where the objects are.
    std::vector<size_t> offsets;
    std::vector<Page> pages;
    bool inPage = false;
    bool finished = false;
    double pageHeight = 792;
    // The page, with y down.
    PdfRect bounds;
    State state;
    std::vector<State> saved;
    std::vector<Layer> layers;
    std::vector<Group> groups;
    std::unique_ptr<Raster> raster;
    // By the name in the resources.
    std::map<std::string, int> objects;
    // Names of resources: by the dictionary, by a checksum.
    std::map<std::string, std::string> graphicsStates;
    std::map<uint64_t, std::string> images;
    std::map<reef_ui_face*, Font> fonts;
    int names = 0;
    std::map<std::string, std::pair<int, std::pair<double, double>>> destinations;
    std::vector<reef_ui_pdf_outline_item> outline;
    std::vector<std::string> outlineStrings;
    std::string metadata;
    std::vector<Element> elements;
    // The elements that were begun.
    std::vector<int> tags;
    int pagesObject = 0;

    void write(const std::string& bytes)
    {
        if (bytes.empty())
            return;
        document.write(document.context, bytes.data(), bytes.size());
        offset += bytes.size();
    }

    int newObject()
    {
        offsets.push_back(0);
        return int(offsets.size());
    }

    void writeObject(int object, const std::string& body)
    {
        offsets[object - 1] = offset;
        write(std::to_string(object) + " 0 obj\n" + body + "\nendobj\n");
    }

    void writeStream(int object, const std::string& dictionary, const std::string& data, bool compressed, bool encrypted = true)
    {
        std::string bytes = compressed ? deflated(data) : data;
        if (document.encrypt && encrypted) {
            size_t size = 0;
            char* result = document.encrypt(document.context, object, 0, bytes.data(), bytes.size(), &size);
            bytes.assign(result, result ? size : 0);
            free(result);
        }
        std::string head = "<< " + dictionary + " /Length " + std::to_string(bytes.size());
        if (compressed)
            head += " /Filter /FlateDecode";
        head += " >>\nstream\n";
        offsets[object - 1] = offset;
        write(std::to_string(object) + " 0 obj\n" + head);
        write(bytes);
        write("\nendstream\nendobj\n");
    }

    std::string string(int object, const std::string& value) const
    {
        if (!document.encrypt)
            return hexString(value);
        size_t size = 0;
        char* result = document.encrypt(document.context, object, 0, value.data(), value.size(), &size);
        const std::string bytes(result, result ? size : 0);
        free(result);
        return hexString(bytes);
    }

    std::string textString(int object, const std::string& value) const { return string(object, encoded(value)); }

    void emit(const std::string& operators)
    {
        if (!layers.empty())
            layers.back().content += operators;
    }

    void use(const std::string& name)
    {
        if (!layers.empty())
            layers.back().resources.insert(name);
    }

    std::string newName(const char* prefix) { return prefix + std::to_string(++names); }

    std::string formName(int object) const
    {
        for (const auto& [name, number] : objects) {
            if (number == object && name.compare(0, 2, "Fm") == 0)
                return name;
        }
        return {};
    }

    std::string resourceDictionary(const std::set<std::string>& used)
    {
        static const struct
        {
            const char* prefix;
            const char* category;
        } categories[] = {
            {"Gs", "ExtGState"}, {"Im", "XObject"}, {"Fm", "XObject"}, {"Sh", "Shading"}, {"Ft", "Font"},
        };
        std::map<std::string, std::string> entries;
        for (const std::string& name : used) {
            for (const auto& category : categories) {
                if (name.compare(0, 2, category.prefix) == 0) {
                    entries[category.category] += "/" + name + " " + std::to_string(objects[name]) + " 0 R ";
                    break;
                }
            }
        }
        std::string dictionary = "<< /ProcSet [/PDF /Text /ImageB /ImageC /ImageI]";
        for (const auto& [category, list] : entries)
            dictionary += " /" + category + " << " + list + ">>";
        return dictionary + " >>";
    }

    // With the form of a mask, if it is not 0.
    std::string graphicsState(double fillAlpha, double strokeAlpha, int composite, int mask = 0)
    {
        std::string dictionary =
            "<< /Type /ExtGState /ca " + number(fillAlpha) + " /CA " + number(strokeAlpha) + " /BM /" + blendMode(composite);
        if (mask)
            dictionary += " /SMask << /Type /Mask /S /Luminosity /G " + std::to_string(mask) + " 0 R /BC [0] >>";
        dictionary += " >>";
        std::string name;
        const auto found = graphicsStates.find(dictionary);
        if (found != graphicsStates.end()) {
            name = found->second;
        } else {
            name = newName("Gs");
            const int object = newObject();
            writeObject(object, dictionary);
            objects[name] = object;
            graphicsStates[dictionary] = name;
        }
        use(name);
        return name;
    }

    // The canvas of the bitmap that a group with a shadow is also drawn into, with the state.
    ui::Canvas2D* canvas()
    {
        if (!raster)
            return nullptr;
        ui::Canvas2D* canvas = raster->canvas.get();
        canvas->setTransform(onBitmap(state.transform, bounds, raster->scale));
        canvas->setGlobalAlpha(state.alpha);
        canvas->setFillColor(toColor(state.fill));
        canvas->setStrokeColor(toColor(state.stroke));
        return canvas;
    }

    Font& font(reef_ui_face* face)
    {
        const auto found = fonts.find(face);
        if (found != fonts.end())
            return found->second;
        Font& font = fonts[face];
        faceRetain(face);
        font.face = face;
        faceMetrics(face, &font.metrics);
        font.format = faceOutlineFormat(face);
        if (font.format) {
            font.name = newName("Ft");
            font.object = newObject();
            objects[font.name] = font.object;
        }
        return font;
    }

    // The bounds of glyphs on the page.
    PdfRect glyphBounds(const reef_ui_face_metrics& metrics, const PdfMatrix& matrix, const double* positions, unsigned count) const
    {
        const double em = metrics.unitsPerEm > 0 ? metrics.unitsPerEm : 1000;
        PdfRect box = {metrics.left / em, metrics.bottom / em, (metrics.right - metrics.left) / em, (metrics.top - metrics.bottom) / em};
        if (box.isEmpty())
            box = {-0.5, -0.5, 2, 2};
        PdfRect result;
        for (unsigned i = 0; i < count; i++) {
            const PdfMatrix glyph = {matrix.a, matrix.b, matrix.c, matrix.d, matrix.e + positions[i * 2], matrix.f + positions[i * 2 + 1]};
            result = result.united(box.mapped(glyph).mapped(state.transform));
        }
        return result;
    }

    // Opens the content of the page to the element.
    void beginMark(int index)
    {
        Page& page = pages.back();
        const int id = int(page.marks.size());
        page.marks.push_back(index);
        Element& element = elements[index];
        element.marks.push_back({int(pages.size()) - 1, id});
        element.kids.push_back(-int(element.marks.size()));
        emit(name(element.type) + " << /MCID " + std::to_string(id) + " >> BDC\n");
    }

    PdfRect pathBounds(const reef_ui_path_element* elements, unsigned count) const;
    std::string colorOperator(const double* color, bool stroke) const;
    std::string pathOperators(const reef_ui_path_element* elements, unsigned count) const;
    std::string strokeOperators() const;
    int form(const Layer& layer, const std::string& group);
    std::string maskImage(const std::vector<uint8_t>& pixels, int width, int height);
    std::string image(const void* pixels, int width, int height, size_t stride, bool rgba, bool interpolated);
    void drawImage(const std::string& name, const PdfMatrix& matrix, double alpha, int composite);
    void drawShadow(const PdfRect& bounds, const std::function<void(ui::Canvas2D&)>& draw);
    void drawGlyphImage(reef_ui_face* face, const double* matrix, const unsigned* glyphs, const double* positions, unsigned count);
    void glyphRun(reef_ui_face* face, const PdfMatrix& matrix, const unsigned* glyphs, const double* positions, unsigned count,
                  const std::u32string* texts);
    int gradientFunction(const double* offsets, const double* colors, unsigned count, int first, int components);
    void fillShading(int type, const std::string& coordinates, const double* offsets, const double* colors, unsigned count, int extend);
    void finishPage(const Layer& content);
    void writeFont(const Font& font);
    void finish();
};

PdfRect PdfWriter::Private::pathBounds(const reef_ui_path_element* elements, unsigned count) const
{
    double left = 1e300;
    double top = 1e300;
    double right = -1e300;
    double bottom = -1e300;
    for (unsigned i = 0; i < count; i++) {
        const reef_ui_path_element& element = elements[i];
        const int points = element.type == 3 ? 3 : element.type == 2 ? 2 : element.type == 4 ? 0 : 1;
        for (int j = 0; j < points; j++) {
            double x, y;
            state.transform.map(element.points[j * 2], element.points[j * 2 + 1], &x, &y);
            left = std::min(left, x);
            top = std::min(top, y);
            right = std::max(right, x);
            bottom = std::max(bottom, y);
        }
    }
    if (left > right)
        return {};
    return {left, top, right - left, bottom - top};
}

std::string PdfWriter::Private::colorOperator(const double* color, bool stroke) const
{
    return numbers({color[0], color[1], color[2]}) + (stroke ? " RG\n" : " rg\n");
}

std::string PdfWriter::Private::pathOperators(const reef_ui_path_element* elements, unsigned count) const
{
    std::string operators;
    double x = 0;
    double y = 0;
    double startX = 0;
    double startY = 0;
    for (unsigned i = 0; i < count; i++) {
        const double* p = elements[i].points;
        switch (elements[i].type) {
        case 0:
            operators += numbers({p[0], p[1]}) + " m\n";
            startX = x = p[0];
            startY = y = p[1];
            break;
        case 1:
            operators += numbers({p[0], p[1]}) + " l\n";
            x = p[0];
            y = p[1];
            break;
        case 2:
            operators += numbers({x + (p[0] - x) * (2.0 / 3), y + (p[1] - y) * (2.0 / 3), p[2] + (p[0] - p[2]) * (2.0 / 3),
                                  p[3] + (p[1] - p[3]) * (2.0 / 3), p[2], p[3]})
                + " c\n";
            x = p[2];
            y = p[3];
            break;
        case 3:
            operators += numbers({p[0], p[1], p[2], p[3], p[4], p[5]}) + " c\n";
            x = p[4];
            y = p[5];
            break;
        case 4:
            operators += "h\n";
            x = startX;
            y = startY;
            break;
        }
    }
    return operators;
}

std::string PdfWriter::Private::strokeOperators() const
{
    std::string operators = number(state.lineWidth) + " w " + std::to_string(state.lineCap) + " J " + std::to_string(state.lineJoin) + " j "
        + number(std::max(state.miterLimit, 1.0)) + " M [";
    for (size_t i = 0; i < state.dashes.size(); i++)
        operators += (i ? " " : "") + number(state.dashes[i]);
    return operators + "] " + number(state.dashOffset) + " d\n";
}

// Returns the object of a form.
int PdfWriter::Private::form(const Layer& layer, const std::string& group)
{
    const int object = newObject();
    objects["Fm" + std::to_string(++names)] = object;
    std::string dictionary = "/Type /XObject /Subtype /Form /BBox ["
        + numbers({bounds.x, bounds.y, bounds.x + bounds.width, bounds.y + bounds.height}) + "] /Resources "
        + resourceDictionary(layer.resources);
    if (!group.empty())
        dictionary += " /Group " + group;
    std::string content = layer.content;
    for (int i = 0; i < layer.saves; i++)
        content += "Q\n";
    writeStream(object, dictionary, content, true);
    return object;
}

// Returns the name of an image of one byte for each pixel.
std::string PdfWriter::Private::maskImage(const std::vector<uint8_t>& pixels, int width, int height)
{
    const int object = newObject();
    writeStream(object,
                "/Type /XObject /Subtype /Image /Width " + std::to_string(width) + " /Height " + std::to_string(height)
                    + " /ColorSpace /DeviceGray /BitsPerComponent 8 /Interpolate true",
                std::string(pixels.begin(), pixels.end()), true);
    const std::string name = newName("Im");
    objects[name] = object;
    use(name);
    return name;
}

// Returns the name of an image; each is written once.
std::string PdfWriter::Private::image(const void* pixels, int width, int height, size_t stride, bool rgba, bool interpolated)
{
    uint64_t checksum = 1469598103934665603ull;
    const auto add = [&checksum](const void* data, size_t size) {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < size; i++)
            checksum = (checksum ^ bytes[i]) * 1099511628211ull;
    };
    add(&width, sizeof width);
    add(&height, sizeof height);
    add(&interpolated, sizeof interpolated);
    for (int y = 0; y < height; y++)
        add(static_cast<const uint8_t*>(pixels) + size_t(y) * stride, size_t(width) * 4);
    const auto found = images.find(checksum);
    if (found != images.end()) {
        use(found->second);
        return found->second;
    }
    std::string colors(size_t(width) * height * 3, '\0');
    std::vector<uint8_t> alphas(size_t(width) * height);
    bool transparent = false;
    for (int y = 0; y < height; y++) {
        const uint8_t* row = static_cast<const uint8_t*>(pixels) + size_t(y) * stride;
        for (int x = 0; x < width; x++) {
            int red, green, blue, alpha;
            if (rgba) {
                red = row[x * 4];
                green = row[x * 4 + 1];
                blue = row[x * 4 + 2];
                alpha = row[x * 4 + 3];
            } else {
                const uint32_t pixel = reinterpret_cast<const uint32_t*>(row)[x];
                red = pixel >> 16 & 255;
                green = pixel >> 8 & 255;
                blue = pixel & 255;
                alpha = pixel >> 24;
            }
            if (alpha > 0 && alpha < 255) {
                red = std::min(255, (red * 255 + alpha / 2) / alpha);
                green = std::min(255, (green * 255 + alpha / 2) / alpha);
                blue = std::min(255, (blue * 255 + alpha / 2) / alpha);
            }
            const size_t index = size_t(y) * width + x;
            colors[index * 3] = char(red);
            colors[index * 3 + 1] = char(green);
            colors[index * 3 + 2] = char(blue);
            alphas[index] = uint8_t(alpha);
            transparent |= alpha != 255;
        }
    }
    std::string dictionary = "/Type /XObject /Subtype /Image /Width " + std::to_string(width) + " /Height " + std::to_string(height)
        + " /ColorSpace /DeviceRGB /BitsPerComponent 8";
    if (interpolated)
        dictionary += " /Interpolate true";
    if (transparent) {
        const int mask = newObject();
        std::string maskDictionary = "/Type /XObject /Subtype /Image /Width " + std::to_string(width) + " /Height " + std::to_string(height)
            + " /ColorSpace /DeviceGray /BitsPerComponent 8";
        if (interpolated)
            maskDictionary += " /Interpolate true";
        writeStream(mask, maskDictionary, std::string(alphas.begin(), alphas.end()), true);
        dictionary += " /SMask " + std::to_string(mask) + " 0 R";
    }
    const int object = newObject();
    writeStream(object, dictionary, colors, true);
    const std::string name = newName("Im");
    objects[name] = object;
    images[checksum] = name;
    use(name);
    return name;
}

void PdfWriter::Private::drawImage(const std::string& name, const PdfMatrix& matrix, double alpha, int composite)
{
    std::string operators = "q\n";
    if (alpha < 1 || composite != 0)
        operators += "/" + graphicsState(alpha, alpha, composite) + " gs\n";
    operators += numbers(matrix) + " cm\n/" + name + " Do\nQ\n";
    emit(operators);
}

// Draws the shadow of what `draw` draws within the bounds.
void PdfWriter::Private::drawShadow(const PdfRect& bounds, const std::function<void(ui::Canvas2D&)>& draw)
{
    const double dx = state.shadow[0];
    const double dy = state.shadow[1];
    const double blur = state.shadow[2];
    const PdfRect moved = {bounds.x + dx, bounds.y + dy, bounds.width, bounds.height};
    PdfRect area = bounds.united(moved);
    area = {area.x - blur * 2 - 2, area.y - blur * 2 - 2, area.width + blur * 4 + 4, area.height + blur * 4 + 4};
    area = area.intersected(this->bounds);
    if (area.isEmpty() || moved.intersected(state.clip).isEmpty())
        return;
    const double scale = std::min(3.0, 4096 / std::max(area.width, area.height));
    const int width = int(std::ceil(area.width * scale));
    const int height = int(std::ceil(area.height * scale));
    ui::Bitmap bitmap(width, height, ui::Bitmap::Format::Argb32Premultiplied);
    if (bitmap.isNull())
        return;
    {
        ui::Canvas2D canvas(bitmap);
        if (!canvas.isValid())
            return;
        canvas.setTransform(onBitmap(state.transform, area, scale));
        draw(canvas);
    }
    const ui::Bitmap shadow = shadowOf(bitmap, dx * scale, dy * scale, blur * scale, state.alpha, state.shadow);
    if (shadow.isNull())
        return;
    const std::string name = image(shadow.constBits(), width, height, shadow.stride(), false, true);
    drawImage(name, imageMatrix(area), 1, state.composite);
}

// For glyphs that cannot be embedded.
void PdfWriter::Private::drawGlyphImage(reef_ui_face* face, const double* matrix, const unsigned* glyphs, const double* positions,
                                        unsigned count)
{
    reef_ui_face_metrics metrics = {};
    faceMetrics(face, &metrics);
    const PdfRect drawn = glyphBounds(metrics, {matrix[0], matrix[1], matrix[2], matrix[3], 0, 0}, positions, count);
    const PdfRect area = drawn.intersected(state.clip).intersected(bounds);
    if (area.isEmpty())
        return;
    const double scale = std::min(4.0, 4096 / std::max(area.width, area.height));
    const int width = int(std::ceil(area.width * scale));
    const int height = int(std::ceil(area.height * scale));
    ui::Bitmap bitmap(width, height, ui::Bitmap::Format::Argb32Premultiplied);
    if (bitmap.isNull())
        return;
    {
        ui::Canvas2D canvas(bitmap);
        if (!canvas.isValid())
            return;
        canvas.setTransform(onBitmap(state.transform, area, scale));
        canvas.setFillColor(toColor(state.fill));
        reef::drawGlyphs(canvas, face, matrix, glyphs, positions, count, 0);
    }
    const std::string name = image(bitmap.constBits(), width, height, bitmap.stride(), false, true);
    drawImage(name, imageMatrix(area), state.alpha, state.composite);
}

// `texts` has what each glyph stands for.
void PdfWriter::Private::glyphRun(reef_ui_face* face, const PdfMatrix& matrix, const unsigned* glyphs, const double* positions,
                                  unsigned count, const std::u32string* texts)
{
    if (layers.empty() || !face || !count)
        return;
    Font& font = this->font(face);
    const double linear[] = {matrix.a, matrix.b, matrix.c, matrix.d};
    if (ui::Canvas2D* canvas = this->canvas())
        reef::drawGlyphs(*canvas, face, linear, glyphs, positions, count, 0);
    if (hasShadow(state)) {
        const ui::ColorF color = toColor(state.fill);
        drawShadow(glyphBounds(font.metrics, matrix, positions, count), [&](ui::Canvas2D& canvas) {
            canvas.setFillColor(color);
            reef::drawGlyphs(canvas, face, linear, glyphs, positions, count, 0);
        });
    }
    if (font.metrics.flags & FaceColor || !(font.metrics.flags & FaceScalable)) {
        drawGlyphImage(face, linear, glyphs, positions, count);
        return;
    }
    const double em = font.metrics.unitsPerEm > 0 ? font.metrics.unitsPerEm : 1000;
    if (!font.format) {
        std::vector<reef_ui_path_element> outline(256);
        std::vector<reef_ui_path_element> path;
        for (unsigned i = 0; i < count; i++) {
            unsigned size = faceOutline(face, glyphs[i], outline.data(), unsigned(outline.size()));
            if (size > outline.size()) {
                outline.resize(size);
                size = faceOutline(face, glyphs[i], outline.data(), size);
            }
            const PdfMatrix glyph = {matrix.a, matrix.b, matrix.c, matrix.d, positions[i * 2] + matrix.e, positions[i * 2 + 1] + matrix.f};
            for (unsigned j = 0; j < size; j++) {
                reef_ui_path_element element = outline[j];
                for (int k = 0; k < 6; k += 2)
                    glyph.map(element.points[k] / em, element.points[k + 1] / em, &element.points[k], &element.points[k + 1]);
                path.push_back(element);
            }
        }
        std::string operators = "q\n";
        const double alpha = state.alpha * state.fill[3];
        if (alpha < 1 || state.composite != 0)
            operators += "/" + graphicsState(alpha, alpha, state.composite) + " gs\n";
        operators += colorOperator(state.fill, false) + numbers(state.transform) + " cm\n" + pathOperators(path.data(), unsigned(path.size()))
            + "f\nQ\n";
        emit(operators);
        return;
    }
    use(font.name);
    PdfMatrix text = matrix;
    const double stretch = faceStretch(font.face);
    if (stretch != 1)
        text = text * PdfMatrix{stretch, 0, 0, 1, 0, 0};
    if (font.metrics.flags & FaceMadeOblique)
        text = text * PdfMatrix{1, 0, madeObliqueSlant(), 1, 0, 0};
    std::string operators = "q\n";
    const double alpha = state.alpha * state.fill[3];
    if (alpha < 1 || state.composite != 0)
        operators += "/" + graphicsState(alpha, alpha, state.composite) + " gs\n";
    operators += colorOperator(state.fill, false);
    operators += numbers(state.transform) + " cm\nBT\n/" + font.name + " 1 Tf\n";
    if (font.metrics.flags & FaceMadeBold)
        operators += colorOperator(state.fill, true) + number(madeBoldStrength() * text.scale()) + " w 2 Tr\n";
    for (unsigned i = 0; i < count; i++) {
        if (glyphs[i] > 0xffff || glyphs[i] >= font.metrics.glyphCount)
            continue;
        PdfMatrix glyph = text;
        glyph.e = positions[i * 2];
        glyph.f = positions[i * 2 + 1];
        char code[8];
        snprintf(code, sizeof code, "%04X", glyphs[i]);
        operators += numbers(glyph) + " Tm <" + code + "> Tj\n";
        font.glyphs.insert(glyphs[i]);
        if (texts && !texts[i].empty() && font.texts.find(glyphs[i]) == font.texts.end())
            font.texts[glyphs[i]] = texts[i];
    }
    operators += "ET\nQ\n";
    emit(operators);
}

// Returns the object of a function of some components of the colors.
int PdfWriter::Private::gradientFunction(const double* offsets, const double* colors, unsigned count, int first, int components)
{
    std::vector<double> positions;
    std::vector<std::vector<double>> values;
    for (unsigned i = 0; i < count; i++) {
        positions.push_back(std::clamp(offsets[i], 0.0, 1.0));
        std::vector<double> value;
        for (int j = 0; j < components; j++)
            value.push_back(colors[i * 4 + first + j]);
        values.push_back(value);
    }
    if (positions.empty()) {
        positions = {0, 1};
        values = {std::vector<double>(components), std::vector<double>(components)};
    }
    if (positions.front() > 0) {
        positions.insert(positions.begin(), 0);
        values.insert(values.begin(), values.front());
    }
    if (positions.back() < 1) {
        positions.push_back(1);
        values.push_back(values.back());
    }
    if (positions.size() == 1) {
        positions.push_back(1);
        values.push_back(values.back());
    }
    std::string functions;
    std::string bounds;
    std::string encode;
    for (size_t i = 0; i + 1 < positions.size(); i++) {
        functions += "<< /FunctionType 2 /Domain [0 1] /C0 " + array(values[i]) + " /C1 " + array(values[i + 1]) + " /N 1 >> ";
        if (i > 0)
            bounds += number(positions[i]) + " ";
        encode += "0 1 ";
    }
    const int object = newObject();
    writeObject(object,
                "<< /FunctionType 3 /Domain [0 1] /Functions [" + functions + "] /Bounds [" + bounds + "] /Encode [" + encode + "] >>");
    return object;
}

void PdfWriter::Private::fillShading(int type, const std::string& coordinates, const double* offsets, const double* colors, unsigned count,
                                     int extend)
{
    if (layers.empty())
        return;
    const std::string extension = std::string(" /Extend [") + (extend & 1 ? "true" : "false") + " " + (extend & 2 ? "true" : "false") + "]";
    const auto shading = [&](const char* colorSpace, int function) {
        const int object = newObject();
        writeObject(object,
                    "<< /ShadingType " + std::to_string(type) + colorSpace + coordinates + "] /Function " + std::to_string(function) + " 0 R"
                        + extension + " >>");
        const std::string name = newName("Sh");
        objects[name] = object;
        return name;
    };
    const std::string name = shading(" /ColorSpace /DeviceRGB /Coords [", gradientFunction(offsets, colors, count, 0, 3));
    bool transparent = false;
    for (unsigned i = 0; i < count; i++)
        transparent |= colors[i * 4 + 3] < 1;
    int mask = 0;
    if (transparent) {
        const std::string alphaName = shading(" /ColorSpace /DeviceGray /Coords [", gradientFunction(offsets, colors, count, 3, 1));
        Layer layer;
        layer.content = numbers(state.transform) + " cm\n/" + alphaName + " sh\n";
        layer.resources.insert(alphaName);
        mask = form(layer, "<< /Type /Group /S /Transparency /CS /DeviceGray >>");
    }
    use(name);
    std::string operators = "q\n";
    if (mask || state.alpha < 1 || state.composite != 0)
        operators += "/" + graphicsState(state.alpha, state.alpha, state.composite, mask) + " gs\n";
    operators += numbers(state.transform) + " cm\n/" + name + " sh\nQ\n";
    emit(operators);
}

void PdfWriter::Private::finishPage(const Layer& content)
{
    const Page& page = pages.back();
    const int contents = newObject();
    writeStream(contents, "", "1 0 0 -1 0 " + number(pageHeight) + " cm\n" + content.content, true);
    std::string annotations;
    const double height = pageHeight;
    for (const Link& link : page.links) {
        const int object = newObject();
        const PdfRect& rect = link.rect;
        std::string dictionary = "<< /Type /Annot /Subtype /Link /Rect ["
            + numbers({rect.x, height - rect.y - rect.height, rect.x + rect.width, height - rect.y}) + "] /Border [0 0 0] /F 4";
        if (!link.url.empty())
            dictionary += " /A << /S /URI /URI " + string(object, link.url) + " >>";
        else
            dictionary += " /Dest " + string(object, link.destination);
        writeObject(object, dictionary + " >>");
        annotations += std::to_string(object) + " 0 R ";
    }
    std::string dictionary =
        "<< /Type /Page /Parent " + std::to_string(pagesObject) + " 0 R /MediaBox " + rectangle(page.mediaBox);
    static const char* const boxNames[] = {"CropBox", "BleedBox", "TrimBox", "ArtBox"};
    for (int i = 0; i < 4; i++) {
        if (page.boxes[i].width > 0 && page.boxes[i].height > 0)
            dictionary += std::string(" /") + boxNames[i] + " " + rectangle(page.boxes[i]);
    }
    dictionary += " /Resources " + resourceDictionary(content.resources) + " /Contents " + std::to_string(contents)
        + " 0 R /Group << /Type /Group /S /Transparency /CS /DeviceRGB >>";
    if (!annotations.empty())
        dictionary += " /Annots [" + annotations + "]";
    if (!page.marks.empty())
        dictionary += " /StructParents " + std::to_string(pages.size() - 1) + " /Tabs /S";
    writeObject(page.object, dictionary + " >>");
}

void PdfWriter::Private::writeFont(const Font& font)
{
    if (!font.object)
        return;
    const std::vector<uint32_t> glyphs(font.glyphs.begin(), font.glyphs.end());
    OutlineFormat format = OutlinesNone;
    const std::string file = faceSubset(font.face, glyphs, &format);
    const double em = font.metrics.unitsPerEm > 0 ? font.metrics.unitsPerEm : 1000;
    uint64_t hash = uint64_t(font.object) * 7919;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(glyphs.data());
    for (size_t i = 0; i < glyphs.size() * sizeof(uint32_t); i++)
        hash = (hash ^ bytes[i]) * 1099511628211ull;
    std::string tag;
    for (int i = 0; i < 6; i++) {
        tag.push_back(char('A' + hash % 26));
        hash /= 26;
    }
    const std::string baseFont = tag + "+" + fontName(font.face);
    const bool compact = font.format == OutlinesCff;
    int fontFile = 0;
    if (!file.empty() && font.format == format) {
        fontFile = newObject();
        if (compact)
            writeStream(fontFile, "/Subtype /OpenType", file, true);
        else
            writeStream(fontFile, "/Length1 " + std::to_string(file.size()), file, true);
    }
    const int descriptor = newObject();
    int flags = 4;
    if (font.metrics.flags & FaceFixedPitch)
        flags |= 1;
    if (font.metrics.flags & (FaceItalic | FaceMadeOblique))
        flags |= 64;
    const reef_ui_face_metrics& metrics = font.metrics;
    std::string dictionary = "<< /Type /FontDescriptor /FontName " + name(baseFont) + " /Flags " + std::to_string(flags) + " /FontBBox ["
        + number(metrics.left * 1000 / em) + " " + number(metrics.bottom * 1000 / em) + " " + number(metrics.right * 1000 / em) + " "
        + number(metrics.top * 1000 / em) + "] /ItalicAngle " + number(metrics.italicAngle) + " /Ascent " + number(metrics.ascent * 1000 / em)
        + " /Descent " + number(-metrics.descent * 1000 / em) + " /CapHeight "
        + number((metrics.capHeight > 0 ? metrics.capHeight : metrics.ascent) * 1000 / em) + " /StemV "
        + std::to_string(metrics.weight >= 600 ? 120 : 80);
    if (fontFile)
        dictionary += std::string(compact ? " /FontFile3 " : " /FontFile2 ") + std::to_string(fontFile) + " 0 R";
    writeObject(descriptor, dictionary + " >>");
    std::vector<double> advances(glyphs.size());
    if (!glyphs.empty())
        faceAdvances(font.face, glyphs.data(), uint32_t(glyphs.size()), 0, advances.data());
    std::string widths;
    for (size_t i = 0; i < glyphs.size();) {
        size_t end = i + 1;
        while (end < glyphs.size() && glyphs[end] == glyphs[end - 1] + 1)
            end++;
        widths += std::to_string(glyphs[i]) + " [";
        for (size_t j = i; j < end; j++)
            widths += (j > i ? " " : "") + number(advances[j] / faceStretch(font.face) * 1000 / em);
        widths += "] ";
        i = end;
    }
    const int descendant = newObject();
    dictionary = std::string("<< /Type /Font /Subtype ") + (compact ? "/CIDFontType0" : "/CIDFontType2") + " /BaseFont " + name(baseFont)
        + " /CIDSystemInfo << /Registry " + textString(descendant, "Adobe") + " /Ordering " + textString(descendant, "Identity")
        + " /Supplement 0 >> /FontDescriptor " + std::to_string(descriptor) + " 0 R /DW 1000 /W [" + widths + "]";
    if (!compact)
        dictionary += " /CIDToGIDMap /Identity";
    writeObject(descendant, dictionary + " >>");
    const std::map<uint32_t, uint32_t> characters = faceCharacters(font.face);
    std::vector<std::pair<uint32_t, std::u16string>> texts;
    for (uint32_t glyph : glyphs) {
        std::u16string units;
        const auto text = font.texts.find(glyph);
        if (text != font.texts.end()) {
            for (char32_t c : text->second)
                appendUtf16(units, c);
        } else {
            const auto character = characters.find(glyph);
            if (character != characters.end())
                appendUtf16(units, character->second);
        }
        if (!units.empty())
            texts.push_back({glyph, units});
    }
    std::string map = "/CIDInit /ProcSet findresource begin\n12 dict begin\nbegincmap\n"
                      "/CIDSystemInfo << /Registry (Adobe) /Ordering (UCS) /Supplement 0 >> def\n"
                      "/CMapName /Adobe-Identity-UCS def\n/CMapType 2 def\n"
                      "1 begincodespacerange\n<0000> <FFFF>\nendcodespacerange\n";
    for (size_t i = 0; i < texts.size(); i += 100) {
        const size_t end = std::min(i + 100, texts.size());
        map += std::to_string(end - i) + " beginbfchar\n";
        for (size_t j = i; j < end; j++) {
            char code[8];
            map += "<";
            snprintf(code, sizeof code, "%04X", texts[j].first);
            map += code;
            map += "> <";
            for (char16_t unit : texts[j].second) {
                char hex[8];
                snprintf(hex, sizeof hex, "%04X", unit);
                map += hex;
            }
            map += ">\n";
        }
        map += "endbfchar\n";
    }
    map += "endcmap\nCMapName currentdict /CMap defineresource pop\nend\nend\n";
    const int unicode = newObject();
    writeStream(unicode, "", map, true);
    writeObject(font.object,
                "<< /Type /Font /Subtype /Type0 /BaseFont " + name(baseFont) + " /Encoding /Identity-H /DescendantFonts ["
                    + std::to_string(descendant) + " 0 R] /ToUnicode " + std::to_string(unicode) + " 0 R >>");
}

void PdfWriter::Private::finish()
{
    for (const auto& [face, font] : fonts)
        writeFont(font);
    std::string catalog;
    if (!elements.empty()) {
        const int root = newObject();
        const int parentTree = newObject();
        std::string top;
        for (size_t i = 0; i < elements.size(); i++) {
            const Element& element = elements[i];
            if (element.parent < 0)
                top += std::to_string(element.object) + " 0 R ";
            std::string kids;
            for (int kid : element.kids) {
                if (kid >= 0) {
                    kids += std::to_string(elements[kid].object) + " 0 R ";
                } else {
                    const std::pair<int, int>& mark = element.marks[-kid - 1];
                    kids += "<< /Type /MCR /Pg " + std::to_string(pages[mark.first].object) + " 0 R /MCID " + std::to_string(mark.second)
                        + " >> ";
                }
            }
            std::string dictionary = "<< /Type /StructElem /S " + name(element.type) + " /P "
                + std::to_string(element.parent >= 0 ? elements[element.parent].object : root) + " 0 R /K [" + kids + "]";
            if (!element.alternateText.empty())
                dictionary += " /Alt " + textString(element.object, element.alternateText);
            if (!element.actualText.empty())
                dictionary += " /ActualText " + textString(element.object, element.actualText);
            if (!element.title.empty())
                dictionary += " /T " + textString(element.object, element.title);
            if (!element.language.empty())
                dictionary += " /Lang " + textString(element.object, element.language);
            writeObject(element.object, dictionary + " >>");
        }
        std::string parents;
        for (size_t i = 0; i < pages.size(); i++) {
            if (pages[i].marks.empty())
                continue;
            parents += std::to_string(i) + " [";
            for (int index : pages[i].marks)
                parents += std::to_string(elements[index].object) + " 0 R ";
            parents += "] ";
        }
        writeObject(parentTree, "<< /Nums [" + parents + "] >>");
        writeObject(root,
                    "<< /Type /StructTreeRoot /K [" + top + "] /ParentTree " + std::to_string(parentTree) + " 0 R /ParentTreeNextKey "
                        + std::to_string(pages.size()) + " >>");
        catalog += " /MarkInfo << /Marked true >> /StructTreeRoot " + std::to_string(root) + " 0 R";
    }
    if (!outline.empty()) {
        const int root = newObject();
        std::vector<int> numbers(outline.size());
        for (int& object : numbers)
            object = newObject();
        std::vector<int> parents(outline.size(), -1);
        // The last has those at the top.
        std::vector<std::vector<int>> children(outline.size() + 1);
        std::vector<int> path;
        for (size_t i = 0; i < outline.size(); i++) {
            const int level = std::max(outline[i].level, 0);
            while (int(path.size()) > level)
                path.pop_back();
            parents[i] = path.empty() ? -1 : path.back();
            children[parents[i] < 0 ? outline.size() : size_t(parents[i])].push_back(int(i));
            path.push_back(int(i));
        }
        std::function<int(size_t)> descendants = [&children, &descendants](size_t index) {
            int count = 0;
            for (int child : children[index])
                count += 1 + descendants(size_t(child));
            return count;
        };
        for (size_t i = 0; i < outline.size(); i++) {
            const reef_ui_pdf_outline_item& item = outline[i];
            const int object = numbers[i];
            std::string dictionary = "<< /Title " + textString(object, item.title) + " /Parent "
                + std::to_string(parents[i] < 0 ? root : numbers[parents[i]]) + " 0 R";
            const std::vector<int>& siblings = children[parents[i] < 0 ? outline.size() : size_t(parents[i])];
            const auto at = std::find(siblings.begin(), siblings.end(), int(i));
            if (at != siblings.begin())
                dictionary += " /Prev " + std::to_string(numbers[*(at - 1)]) + " 0 R";
            if (at + 1 != siblings.end())
                dictionary += " /Next " + std::to_string(numbers[*(at + 1)]) + " 0 R";
            if (!children[i].empty()) {
                dictionary += " /First " + std::to_string(numbers[children[i].front()]) + " 0 R /Last "
                    + std::to_string(numbers[children[i].back()]) + " 0 R /Count " + std::to_string(descendants(i));
            }
            if (item.url && *item.url) {
                dictionary += " /A << /S /URI /URI " + string(object, item.url) + " >>";
            } else if (item.page > 0 && size_t(item.page) <= pages.size()) {
                const Page& page = pages[item.page - 1];
                const std::string x = std::isnan(item.x) ? "null" : number(item.x);
                const std::string y = number(std::isnan(item.y) ? page.mediaBox.y + page.mediaBox.height : item.y);
                dictionary += " /Dest [" + std::to_string(page.object) + " 0 R /XYZ " + x + " " + y + " null]";
            }
            writeObject(object, dictionary + " >>");
        }
        const std::vector<int>& top = children[outline.size()];
        writeObject(root,
                    "<< /Type /Outlines /First " + std::to_string(numbers[top.front()]) + " 0 R /Last " + std::to_string(numbers[top.back()])
                        + " 0 R /Count " + std::to_string(outline.size()) + " >>");
        catalog += " /Outlines " + std::to_string(root) + " 0 R /PageMode /UseOutlines";
    }
    if (!destinations.empty()) {
        const int object = newObject();
        std::string list;
        for (const auto& [name, place] : destinations) {
            list += string(object, name) + " [" + std::to_string(pages[place.first].object) + " 0 R /XYZ " + number(place.second.first) + " "
                + number(place.second.second) + " null] ";
        }
        writeObject(object, "<< /Names [" + list + "] >>");
        catalog += " /Names << /Dests " + std::to_string(object) + " 0 R >>";
    }
    std::string xmp = metadata;
    if (document.archival) {
        xmp = "<?xpacket begin=\"\ufeff\" id=\"W5M0MpCehiHzreSzNTczkc9d\"?>\n"
              "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\"><rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">"
              "<rdf:Description rdf:about=\"\" xmlns:pdfaid=\"http://www.aiim.org/pdfa/ns/id/\""
              " xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:xmp=\"http://ns.adobe.com/xap/1.0/\""
              " xmlns:pdf=\"http://ns.adobe.com/pdf/1.3/\">"
              "<pdfaid:part>2</pdfaid:part><pdfaid:conformance>B</pdfaid:conformance>";
        if (!title.empty())
            xmp += "<dc:title><rdf:Alt><rdf:li xml:lang=\"x-default\">" + xml(title) + "</rdf:li></rdf:Alt></dc:title>";
        if (!author.empty())
            xmp += "<dc:creator><rdf:Seq><rdf:li>" + xml(author) + "</rdf:li></rdf:Seq></dc:creator>";
        if (!subject.empty())
            xmp += "<dc:description><rdf:Alt><rdf:li xml:lang=\"x-default\">" + xml(subject) + "</rdf:li></rdf:Alt></dc:description>";
        if (!keywords.empty())
            xmp += "<pdf:Keywords>" + xml(keywords) + "</pdf:Keywords>";
        if (!creator.empty())
            xmp += "<xmp:CreatorTool>" + xml(creator) + "</xmp:CreatorTool>";
        const std::string date = dateText(document.date, true);
        xmp += "<xmp:CreateDate>" + date + "</xmp:CreateDate><xmp:ModifyDate>" + date + "</xmp:ModifyDate>";
        xmp += "</rdf:Description></rdf:RDF></x:xmpmeta>\n<?xpacket end=\"w\"?>";
    }
    if (!xmp.empty()) {
        const int object = newObject();
        writeStream(object, "/Type /Metadata /Subtype /XML", xmp, false);
        catalog += " /Metadata " + std::to_string(object) + " 0 R";
    }
    std::string intents = outputIntents;
    std::string colors = profile;
    int components = document.profileComponents > 0 ? document.profileComponents : 3;
    if (document.archival) {
        colors = srgbProfile();
        intents = "<< /Type /OutputIntent /S /GTS_PDFA1 /OutputConditionIdentifier (sRGB IEC61966-2.1) /Info (sRGB IEC61966-2.1)"
                  " /DestOutputProfile %PROFILE% >>";
        components = 3;
    }
    const int catalogObject = newObject();
    if (!intents.empty()) {
        if (document.encrypt) {
            // The strings in it, which are all hexadecimal.
            std::string encrypted;
            for (size_t i = 0; i < intents.size(); i++) {
                if (intents[i] == '<' && i + 1 < intents.size() && intents[i + 1] != '<' && (i == 0 || intents[i - 1] != '<')) {
                    const size_t end = intents.find('>', i);
                    if (end == std::string::npos)
                        break;
                    std::string bytes;
                    for (size_t j = i + 1; j + 1 < end; j += 2)
                        bytes.push_back(char(std::stoi(intents.substr(j, 2), nullptr, 16)));
                    encrypted += string(catalogObject, bytes);
                    i = end;
                } else {
                    encrypted.push_back(intents[i]);
                }
            }
            intents = encrypted;
        }
        if (colors.empty() && components == 3)
            colors = srgbProfile();
        const size_t place = intents.find("%PROFILE%");
        if (place != std::string::npos) {
            if (!colors.empty()) {
                const int object = newObject();
                writeStream(object, "/N " + std::to_string(components), colors, true);
                intents.replace(place, 9, std::to_string(object) + " 0 R");
            } else {
                intents.replace(place, 9, "null");
            }
        }
        catalog += " /OutputIntents [" + intents + "]";
    }
    std::string kids;
    for (const Page& page : pages)
        kids += std::to_string(page.object) + " 0 R ";
    writeObject(pagesObject, "<< /Type /Pages /Kids [" + kids + "] /Count " + std::to_string(pages.size()) + " >>");
    const int infoObject = newObject();
    std::string info = "<<";
    if (!title.empty())
        info += " /Title " + textString(infoObject, title);
    if (!author.empty())
        info += " /Author " + textString(infoObject, author);
    if (!subject.empty())
        info += " /Subject " + textString(infoObject, subject);
    if (!keywords.empty())
        info += " /Keywords " + textString(infoObject, keywords);
    if (!creator.empty())
        info += " /Creator " + textString(infoObject, creator);
    const std::string date = dateText(document.date, false);
    info += " /CreationDate " + textString(infoObject, date) + " /ModDate " + textString(infoObject, date) + " >>";
    writeObject(infoObject, info);
    int encryptObject = 0;
    if (document.encrypt && !encryption.empty()) {
        encryptObject = newObject();
        writeObject(encryptObject, encryption);
    }
    writeObject(catalogObject, "<< /Type /Catalog /Pages " + std::to_string(pagesObject) + " 0 R" + catalog + " >>");
    const size_t start = offset;
    std::string trailer = "xref\n0 " + std::to_string(offsets.size() + 1) + "\n0000000000 65535 f \n";
    for (size_t place : offsets) {
        char line[32];
        if (place)
            snprintf(line, sizeof line, "%010llu 00000 n \n", static_cast<unsigned long long>(place));
        else
            snprintf(line, sizeof line, "0000000000 00001 f \n");
        trailer += line;
    }
    const std::string id = hexString(identifier);
    trailer += "trailer\n<< /Size " + std::to_string(offsets.size() + 1) + " /Root " + std::to_string(catalogObject) + " 0 R /Info "
        + std::to_string(infoObject) + " 0 R /ID [" + id + " " + id + "]";
    if (encryptObject)
        trailer += " /Encrypt " + std::to_string(encryptObject) + " 0 R";
    trailer += " >>\nstartxref\n" + std::to_string(start) + "\n%%EOF\n";
    write(trailer);
}

PdfWriter::PdfWriter(const reef_ui_pdf_document& document)
    : d(std::make_unique<Private>())
{
    d->document = document;
    d->title = text(document.title);
    d->author = text(document.author);
    d->subject = text(document.subject);
    d->keywords = text(document.keywords);
    d->creator = text(document.creator);
    d->encryption = text(document.encryption);
    d->outputIntents = text(document.outputIntents);
    if (document.profile && document.profileSize)
        d->profile.assign(document.profile, document.profileSize);
    if (document.identifier) {
        d->identifier.assign(document.identifier, 16);
    } else {
        std::random_device random;
        for (int i = 0; i < 16; i++)
            d->identifier += char(random());
    }
    if (document.archival) {
        d->document.encrypt = nullptr;
        d->encryption.clear();
    }
    d->write("%PDF-1.7\n%\xe2\xe3\xcf\xd3\n");
    d->pagesObject = d->newObject();
}

PdfWriter::~PdfWriter()
{
    for (const auto& [face, font] : d->fonts)
        faceRelease(face);
}

void PdfWriter::beginPage(const double* mediaBox, const double* boxes)
{
    if (d->finished)
        return;
    if (d->inPage)
        endPage();
    Page page;
    page.object = d->newObject();
    if (mediaBox)
        page.mediaBox = {mediaBox[0], mediaBox[1], mediaBox[2], mediaBox[3]};
    if (page.mediaBox.isEmpty()) {
        page.mediaBox.width = 612;
        page.mediaBox.height = 792;
    }
    if (boxes) {
        for (int i = 0; i < 4; i++)
            page.boxes[i] = {boxes[i * 4], boxes[i * 4 + 1], boxes[i * 4 + 2], boxes[i * 4 + 3]};
    }
    d->pages.push_back(page);
    d->pageHeight = page.mediaBox.height;
    d->bounds = {page.mediaBox.x, -page.mediaBox.y, page.mediaBox.width, page.mediaBox.height};
    d->state = State();
    d->state.clip = d->bounds;
    d->saved.clear();
    d->groups.clear();
    d->layers = {Layer()};
    d->inPage = true;
    for (int tag : d->tags)
        d->beginMark(tag);
}

void PdfWriter::endPage()
{
    if (!d->inPage)
        return;
    Layer& content = d->layers.front();
    while (d->layers.size() > 1) {
        const Layer layer = d->layers.back();
        d->layers.pop_back();
        const int object = d->form(layer, "<< /Type /Group /S /Transparency >>");
        d->emit("/" + d->formName(object) + " Do\n");
        d->use(d->formName(object));
    }
    for (int i = 0; i < content.saves; i++)
        content.content += "Q\n";
    content.saves = 0;
    for (size_t i = 0; i < d->tags.size(); i++)
        content.content += "EMC\n";
    d->finishPage(content);
    d->layers.clear();
    d->inPage = false;
}

void PdfWriter::close()
{
    if (d->finished)
        return;
    endPage();
    d->finish();
    d->finished = true;
}

void PdfWriter::addLink(double x, double y, double width, double height, const char* url, const char* destination)
{
    if (!d->inPage)
        return;
    Link link;
    link.rect = PdfRect{x, y, width, height}.mapped(d->state.transform);
    link.url = url ? url : "";
    link.destination = destination ? destination : "";
    d->pages.back().links.push_back(link);
}

void PdfWriter::addDestination(const char* name, double x, double y)
{
    if (!d->inPage || !name)
        return;
    double pageX, pageY;
    d->state.transform.map(x, y, &pageX, &pageY);
    d->destinations[name] = {int(d->pages.size()) - 1, {pageX, d->pageHeight - pageY}};
}

void PdfWriter::beginTag(const char* type, const char* actualText, const char* alternateText, const char* title, const char* language)
{
    Element element;
    element.object = d->newObject();
    element.parent = d->tags.empty() ? -1 : d->tags.back();
    element.type = type && *type ? type : "Span";
    element.actualText = actualText ? actualText : "";
    element.alternateText = alternateText ? alternateText : "";
    element.title = title ? title : "";
    element.language = language ? language : "";
    const int index = int(d->elements.size());
    d->elements.push_back(element);
    if (element.parent >= 0) {
        Element& parent = d->elements[element.parent];
        parent.children.push_back(index);
        parent.kids.push_back(index);
    }
    d->tags.push_back(index);
    if (d->inPage && d->layers.size() == 1)
        d->beginMark(index);
}

void PdfWriter::endTag()
{
    if (d->tags.empty())
        return;
    d->tags.pop_back();
    if (d->inPage && d->layers.size() == 1)
        d->emit("EMC\n");
}

void PdfWriter::setOutline(const reef_ui_pdf_outline_item* items, unsigned count)
{
    d->outline.clear();
    d->outlineStrings.clear();
    // The items point into it.
    d->outlineStrings.reserve(size_t(count) * 2);
    for (unsigned i = 0; i < count; i++) {
        reef_ui_pdf_outline_item item = items[i];
        d->outlineStrings.push_back(text(item.title));
        item.title = d->outlineStrings.back().c_str();
        d->outlineStrings.push_back(text(item.url));
        item.url = d->outlineStrings.back().c_str();
        d->outline.push_back(item);
    }
}

void PdfWriter::setMetadata(const char* bytes, size_t size)
{
    d->metadata.assign(bytes, bytes ? size : 0);
}

void PdfWriter::save()
{
    if (d->raster)
        d->raster->canvas->save();
    d->saved.push_back(d->state);
    if (!d->layers.empty()) {
        Layer& layer = d->layers.back();
        layer.content += "q\n";
        layer.saves++;
    }
}

void PdfWriter::restore()
{
    if (d->saved.empty())
        return;
    if (d->raster)
        d->raster->canvas->restore();
    d->state = d->saved.back();
    d->saved.pop_back();
    if (!d->layers.empty() && d->layers.back().saves > 0) {
        Layer& layer = d->layers.back();
        layer.content += "Q\n";
        layer.saves--;
    }
}

void PdfWriter::concat(const double* matrix)
{
    d->state.transform = d->state.transform * toMatrix(matrix);
}

void PdfWriter::setTransform(const double* matrix)
{
    d->state.transform = toMatrix(matrix);
}

void PdfWriter::transform(double* matrix) const
{
    const PdfMatrix& transform = d->state.transform;
    matrix[0] = transform.a;
    matrix[1] = transform.b;
    matrix[2] = transform.c;
    matrix[3] = transform.d;
    matrix[4] = transform.e;
    matrix[5] = transform.f;
}

void PdfWriter::clipToRect(double x, double y, double width, double height)
{
    const std::array<reef_ui_path_element, 5> path = rectPath(x, y, width, height);
    clipToPath(path.data(), unsigned(path.size()), 0);
}

void PdfWriter::clipToPath(const reef_ui_path_element* elements, unsigned count, int evenOdd)
{
    d->state.clip = d->state.clip.intersected(d->pathBounds(elements, count));
    if (ui::Canvas2D* canvas = d->canvas())
        canvas->clipToPath(toPath(elements, count, evenOdd));
    // The clip stays as it is when the transform changes.
    std::vector<reef_ui_path_element> mapped(elements, elements + count);
    for (reef_ui_path_element& element : mapped) {
        for (int i = 0; i < 6; i += 2)
            d->state.transform.map(element.points[i], element.points[i + 1], &element.points[i], &element.points[i + 1]);
    }
    if (count == 0)
        d->emit("0 0 0 0 re W n\n");
    else
        d->emit(d->pathOperators(mapped.data(), count) + (evenOdd & 1 ? "W* n\n" : "W n\n"));
}

void PdfWriter::clipBounds(double* rect) const
{
    const PdfRect bounds = d->state.clip.mapped(d->state.transform.inverted());
    rect[0] = bounds.x;
    rect[1] = bounds.y;
    rect[2] = bounds.width;
    rect[3] = bounds.height;
}

void PdfWriter::setAlpha(double alpha)
{
    d->state.alpha = std::clamp(alpha, 0.0, 1.0);
}

void PdfWriter::setComposite(int composite)
{
    d->state.composite = composite;
}

void PdfWriter::setSmoothing(int smoothing)
{
    d->state.smoothing = smoothing != 0;
}

void PdfWriter::setFillColor(double red, double green, double blue, double alpha)
{
    const double color[] = {red, green, blue, alpha};
    std::copy(color, color + 4, d->state.fill);
}

void PdfWriter::setStrokeColor(double red, double green, double blue, double alpha)
{
    const double color[] = {red, green, blue, alpha};
    std::copy(color, color + 4, d->state.stroke);
}

void PdfWriter::fillColor(double* color) const
{
    std::copy(d->state.fill, d->state.fill + 4, color);
}

void PdfWriter::setLineWidth(double width)
{
    d->state.lineWidth = width;
}

void PdfWriter::setLineCap(int cap)
{
    d->state.lineCap = std::clamp(cap, 0, 2);
}

void PdfWriter::setLineJoin(int join)
{
    d->state.lineJoin = std::clamp(join, 0, 2);
}

void PdfWriter::setMiterLimit(double limit)
{
    d->state.miterLimit = limit;
}

void PdfWriter::setLineDash(const double* lengths, unsigned count, double offset)
{
    d->state.dashes.assign(lengths, lengths + count);
    d->state.dashOffset = offset;
}

void PdfWriter::setShadow(double dx, double dy, double blur, double red, double green, double blue, double alpha)
{
    const double shadow[] = {dx, dy, blur, red, green, blue, alpha};
    std::copy(shadow, shadow + 7, d->state.shadow);
}

void PdfWriter::fillPath(const reef_ui_path_element* elements, unsigned count, int evenOdd)
{
    if (d->layers.empty() || !count)
        return;
    const State& state = d->state;
    if (hasShadow(state)) {
        const ui::ColorF color = toColor(state.fill);
        d->drawShadow(d->pathBounds(elements, count), [&](ui::Canvas2D& canvas) {
            canvas.setFillColor(color);
            canvas.fillPath(toPath(elements, count, evenOdd));
        });
    }
    if (ui::Canvas2D* canvas = d->canvas())
        canvas->fillPath(toPath(elements, count, evenOdd));
    std::string operators = "q\n";
    const double alpha = state.alpha * state.fill[3];
    if (alpha < 1 || state.composite != 0)
        operators += "/" + d->graphicsState(alpha, alpha, state.composite) + " gs\n";
    operators += d->colorOperator(state.fill, false) + numbers(state.transform) + " cm\n" + d->pathOperators(elements, count)
        + (evenOdd & 1 ? "f*\nQ\n" : "f\nQ\n");
    d->emit(operators);
}

void PdfWriter::strokePath(const reef_ui_path_element* elements, unsigned count)
{
    if (d->layers.empty() || !count)
        return;
    const State& state = d->state;
    if (hasShadow(state)) {
        PdfRect bounds = d->pathBounds(elements, count);
        const double reach = state.lineWidth * state.transform.scale() * std::max(1.0, state.miterLimit);
        bounds = {bounds.x - reach, bounds.y - reach, bounds.width + reach * 2, bounds.height + reach * 2};
        const State stroked = state;
        d->drawShadow(bounds, [&](ui::Canvas2D& canvas) {
            canvas.setStrokeColor(toColor(stroked.stroke));
            setStroke(canvas, stroked);
            canvas.strokePath(toPath(elements, count, 0));
        });
    }
    if (ui::Canvas2D* canvas = d->canvas()) {
        setStroke(*canvas, state);
        canvas->strokePath(toPath(elements, count, 0));
    }
    std::string operators = "q\n";
    const double alpha = state.alpha * state.stroke[3];
    if (alpha < 1 || state.composite != 0)
        operators += "/" + d->graphicsState(alpha, alpha, state.composite) + " gs\n";
    operators += d->colorOperator(state.stroke, true) + numbers(state.transform) + " cm\n" + d->strokeOperators()
        + d->pathOperators(elements, count) + "S\nQ\n";
    d->emit(operators);
}

void PdfWriter::fillRect(double x, double y, double width, double height)
{
    const std::array<reef_ui_path_element, 5> path = rectPath(x, y, width, height);
    fillPath(path.data(), unsigned(path.size()), 0);
}

void PdfWriter::strokeRect(double x, double y, double width, double height)
{
    const std::array<reef_ui_path_element, 5> path = rectPath(x, y, width, height);
    strokePath(path.data(), unsigned(path.size()));
}

// Pages have no transparent pixels; in a group nothing can be cleared.
void PdfWriter::clearRect(double x, double y, double width, double height)
{
    if (d->layers.size() != 1)
        return;
    const std::array<reef_ui_path_element, 5> path = rectPath(x, y, width, height);
    d->emit("q\n1 1 1 rg\n" + numbers(d->state.transform) + " cm\n" + d->pathOperators(path.data(), unsigned(path.size())) + "f\nQ\n");
}

void PdfWriter::drawPixels(const void* pixels, int width, int height, size_t stride, int format, double x, double y, double drawnWidth,
                           double drawnHeight, int mirrored)
{
    if (d->layers.empty() || !pixels || width <= 0 || height <= 0)
        return;
    const bool rgba = format == 1;
    if (hasShadow(d->state)) {
        d->drawShadow(PdfRect{x, y, drawnWidth, drawnHeight}.mapped(d->state.transform), [&](ui::Canvas2D& canvas) {
            canvas.drawBitmap(ui::Bitmap::wrap(const_cast<void*>(pixels), width, height, stride, ui::Bitmap::Format(rgba)),
                              {x, y, drawnWidth, drawnHeight}, mirrored);
        });
    }
    if (ui::Canvas2D* canvas = d->canvas()) {
        canvas->drawBitmap(ui::Bitmap::wrap(const_cast<void*>(pixels), width, height, stride, ui::Bitmap::Format(rgba)),
                           {x, y, drawnWidth, drawnHeight}, mirrored);
    }
    const std::string name = d->image(pixels, width, height, stride, rgba, d->state.smoothing);
    PdfMatrix matrix = {drawnWidth, 0, 0, drawnHeight, x, y};
    if (!mirrored) {
        matrix.d = -drawnHeight;
        matrix.f = y + drawnHeight;
    }
    d->drawImage(name, d->state.transform * matrix, d->state.alpha, d->state.composite);
}

void PdfWriter::fillLinearGradient(const double* offsets, const double* colors, unsigned count, double x0, double y0, double x1, double y1,
                                   int extend)
{
    if (ui::Canvas2D* canvas = d->canvas())
        canvas->fillLinearGradient(gradientStops(offsets, colors, count), {x0, y0}, {x1, y1}, extend & 1, extend & 2);
    d->fillShading(2, numbers({x0, y0, x1, y1}), offsets, colors, count, extend);
}

void PdfWriter::fillRadialGradient(const double* offsets, const double* colors, unsigned count, double x0, double y0, double r0, double x1,
                                   double y1, double r1, int extend)
{
    if (ui::Canvas2D* canvas = d->canvas())
        canvas->fillRadialGradient(gradientStops(offsets, colors, count), {x0, y0}, r0, {x1, y1}, r1, extend & 1, extend & 2);
    d->fillShading(3, numbers({x0, y0, std::max(r0, 0.0), x1, y1, std::max(r1, 0.0)}), offsets, colors, count, extend);
}

void PdfWriter::drawText(const std::string& text, const std::vector<PdfTextSpan>& spans, const std::function<void(ui::Canvas2D&)>& draw)
{
    State& state = d->state;
    if (d->layers.empty() || spans.empty())
        return;
    if (hasShadow(state)) {
        PdfRect bounds;
        for (const PdfTextSpan& span : spans) {
            const double begin = span.x(span.begin);
            const double end = span.x(span.end);
            const PdfRect rect = {std::min(begin, end) - span.size, span.baseline - span.size * 1.2, std::abs(end - begin) + span.size * 2,
                                  span.size * 1.8};
            bounds = bounds.united(rect.mapped(state.transform));
        }
        d->drawShadow(bounds, draw);
    }
    if (ui::Canvas2D* canvas = d->canvas())
        draw(*canvas);
    // The glyphs do not draw the shadow or on the canvas again.
    const double shadowAlpha = state.shadow[6];
    state.shadow[6] = 0;
    double fill[4];
    std::copy(state.fill, state.fill + 4, fill);
    std::unique_ptr<Raster> raster = std::move(d->raster);
    for (const PdfTextSpan& span : spans) {
        if (span.begin >= span.end || span.end > text.size())
            continue;
        std::copy(span.color, span.color + 4, state.fill);
        const reef_ui_face_query query = {nullptr, span.family.c_str(), nullptr, span.bold ? 700.0 : 400.0, -1, span.italic, -1, nullptr};
        reef_ui_face* face = faceMatch(&query, nullptr);
        if (!face)
            continue;
        const std::vector<reef_ui_feature> features = parseFeatures(span.features.c_str());
        std::vector<size_t> offsets;
        const std::u32string characters = decoded(text.substr(span.begin, span.end - span.begin), &offsets);
        std::vector<reef_ui_face*> faces(characters.size());
        std::vector<reef_ui_face*> fallbacks;
        for (size_t i = 0; i < characters.size(); i++) {
            const uint32_t character = characters[i];
            if (character < 0x20 || character == 0x7f || character == 0x2028 || character == 0x2029)
                continue;
            uint32_t glyph = 0;
            faceGlyphs(face, &character, 1, &glyph);
            faces[i] = face;
            if (glyph)
                continue;
            if (reef_ui_face* fallback = faceFallback(face, character, nullptr)) {
                fallbacks.push_back(fallback);
                faces[i] = fallback;
            }
        }
        // Runs of one face and direction.
        size_t next;
        for (size_t first = 0; first < characters.size(); first = next) {
            next = first + 1;
            if (!faces[first])
                continue;
            const bool rightToLeft = isRightToLeft(characters[first]);
            while (next < characters.size() && faces[next] == faces[first] && isRightToLeft(characters[next]) == rightToLeft)
                next++;
            std::u16string units;
            // The character of each unit.
            std::vector<size_t> indexes;
            for (size_t i = first; i < next; i++) {
                const size_t size = units.size();
                appendUtf16(units, characters[i]);
                for (size_t j = size; j < units.size(); j++)
                    indexes.push_back(i);
            }
            std::vector<reef_ui_glyph> shaped(units.size() * 2 + 8);
            const auto shape = [&] {
                return faceShape(faces[first], reinterpret_cast<const uint16_t*>(units.data()), uint32_t(units.size()), 0,
                                 uint32_t(units.size()), span.size, rightToLeft, 0, nullptr, features.data(), uint32_t(features.size()),
                                 shaped.data(), uint32_t(shaped.size()));
            };
            uint32_t count = shape();
            if (count > shaped.size()) {
                shaped.resize(count);
                count = shape();
            }
            std::vector<uint32_t> glyphs;
            std::vector<double> positions;
            std::vector<std::u32string> texts;
            uint32_t end;
            for (uint32_t i = 0; i < count; i = end) {
                end = i + 1;
                while (end < count && shaped[end].cluster == shaped[i].cluster)
                    end++;
                double advance = 0;
                for (uint32_t j = i; j < end; j++)
                    advance += shaped[j].xAdvance;
                const size_t begin = shaped[i].cluster < indexes.size() ? indexes[shaped[i].cluster] : first;
                // Up to the next cluster.
                size_t stop = next;
                for (uint32_t j = 0; j < count; j++) {
                    const size_t other = shaped[j].cluster < indexes.size() ? indexes[shaped[j].cluster] : next;
                    if (other > begin)
                        stop = std::min(stop, other);
                }
                double x = span.x(span.begin + offsets[begin]);
                if (rightToLeft)
                    x -= advance;
                for (uint32_t j = i; j < end; j++) {
                    glyphs.push_back(shaped[j].glyph);
                    positions.push_back(x + shaped[j].xOffset);
                    positions.push_back(span.baseline - shaped[j].yOffset);
                    texts.push_back(j == i ? characters.substr(begin, stop - begin) : std::u32string());
                    x += shaped[j].xAdvance;
                }
            }
            d->glyphRun(faces[first], {span.size, 0, 0, -span.size, 0, 0}, glyphs.data(), positions.data(), unsigned(glyphs.size()),
                        texts.data());
        }
        if (span.underline) {
            reef_ui_face_metrics metrics = {};
            faceMetrics(face, &metrics);
            const double em = metrics.unitsPerEm > 0 ? metrics.unitsPerEm : 1000;
            const double thickness = std::max(metrics.underlineThickness / em * span.size, 0.5);
            const double top = span.baseline - metrics.underlinePosition / em * span.size;
            const double begin = span.x(span.begin);
            const double end = span.x(span.end);
            fillRect(std::min(begin, end), top, std::abs(end - begin), thickness);
        }
        for (reef_ui_face* fallback : fallbacks)
            faceRelease(fallback);
        faceRelease(face);
    }
    std::copy(fill, fill + 4, state.fill);
    state.shadow[6] = shadowAlpha;
    d->raster = std::move(raster);
}

void PdfWriter::drawGlyphs(reef_ui_face* face, const double* matrix, const uint32_t* glyphs, const double* positions, uint32_t count, int)
{
    d->glyphRun(face, {matrix[0], matrix[1], matrix[2], matrix[3], 0, 0}, glyphs, positions, count, nullptr);
}

void PdfWriter::beginGroup()
{
    Group group;
    group.alpha = d->state.alpha;
    group.composite = d->state.composite;
    std::copy(d->state.shadow, d->state.shadow + 7, group.shadow);
    if (ui::Canvas2D* canvas = d->canvas())
        canvas->beginGroup();
    if (d->inPage && hasShadow(d->state) && !d->raster) {
        auto raster = std::make_unique<Raster>();
        raster->scale = std::min(2.0, 4096 / std::max(d->bounds.width, d->bounds.height));
        raster->bitmap = ui::Bitmap(int(std::ceil(d->bounds.width * raster->scale)), int(std::ceil(d->bounds.height * raster->scale)),
                                    ui::Bitmap::Format::Argb32Premultiplied);
        if (!raster->bitmap.isNull()) {
            raster->canvas = std::make_unique<ui::Canvas2D>(raster->bitmap);
            if (raster->canvas->isValid()) {
                d->raster = std::move(raster);
                group.raster = true;
            }
        }
    }
    d->groups.push_back(group);
    d->state.alpha = 1;
    d->state.composite = 0;
    d->state.shadow[6] = 0;
    Layer layer;
    layer.depth = d->saved.size();
    d->layers.push_back(layer);
}

void PdfWriter::endGroup()
{
    if (d->groups.empty() || d->layers.size() < 2 || d->groups.back().masked)
        return;
    const Group group = d->groups.back();
    d->groups.pop_back();
    const Layer layer = d->layers.back();
    d->layers.pop_back();
    d->state.alpha = group.alpha;
    d->state.composite = group.composite;
    std::copy(group.shadow, group.shadow + 7, d->state.shadow);
    std::unique_ptr<Raster> raster;
    if (group.raster)
        raster = std::move(d->raster);
    else if (ui::Canvas2D* canvas = d->canvas())
        canvas->endGroup();
    if (!d->inPage)
        return;
    if (raster) {
        const double scale = raster->scale;
        const int width = raster->bitmap.width();
        const int height = raster->bitmap.height();
        const ui::Bitmap shadow = shadowOf(raster->bitmap, group.shadow[0] * scale, group.shadow[1] * scale, group.shadow[2] * scale,
                                           group.alpha, group.shadow);
        if (!shadow.isNull()) {
            const std::string name = d->image(shadow.constBits(), width, height, shadow.stride(), false, true);
            d->drawImage(name, imageMatrix(d->bounds), 1, group.composite);
        }
    }
    const int object = d->form(layer, "<< /Type /Group /S /Transparency >>");
    const std::string name = d->formName(object);
    d->use(name);
    std::string operators = "q\n";
    if (group.alpha < 1 || group.composite != 0)
        operators += "/" + d->graphicsState(group.alpha, group.alpha, group.composite) + " gs\n";
    d->emit(operators + "/" + name + " Do\nQ\n");
}

void PdfWriter::beginMaskedGroup()
{
    if (ui::Canvas2D* canvas = d->canvas())
        canvas->beginGroup();
    Group group;
    group.masked = true;
    d->groups.push_back(group);
    Layer layer;
    layer.depth = d->saved.size();
    d->layers.push_back(layer);
}

void PdfWriter::endMaskedGroup(const void* pixels, int width, int height, size_t stride, double x, double y, double drawnWidth,
                               double drawnHeight, const double* matrix, int mirrored)
{
    if (d->groups.empty() || d->layers.size() < 2 || !d->groups.back().masked)
        return;
    d->groups.pop_back();
    const Layer layer = d->layers.back();
    d->layers.pop_back();
    if (ui::Canvas2D* canvas = d->canvas()) {
        canvas->save();
        canvas->setTransform(onBitmap(toMatrix(matrix), d->bounds, d->raster->scale));
        canvas->setGlobalAlpha(1);
        if (pixels && width > 0 && height > 0) {
            ui::Path outside;
            outside.addRect({-1e6, -1e6, 2e6, 2e6});
            outside.addRect({x, y, drawnWidth, drawnHeight});
            outside.setFillRule(ui::Path::FillRule::EvenOdd);
            canvas->setComposite(ui::Canvas2D::Composite::DestinationOut);
            canvas->setFillColor({0, 0, 0, 1});
            canvas->fillPath(outside);
            canvas->setComposite(ui::Canvas2D::Composite::DestinationIn);
            canvas->drawBitmap(ui::Bitmap::wrap(const_cast<void*>(pixels), width, height, stride, ui::Bitmap::Format::Argb32Premultiplied),
                               {x, y, drawnWidth, drawnHeight}, mirrored);
        } else {
            canvas->clearRect({-1e6, -1e6, 2e6, 2e6});
        }
        canvas->restore();
        canvas->endGroup();
    }
    if (!d->inPage || !pixels || width <= 0 || height <= 0)
        return;
    std::vector<uint8_t> alphas(size_t(width) * height);
    for (int i = 0; i < height; i++) {
        const uint32_t* row = reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(pixels) + size_t(i) * stride);
        for (int j = 0; j < width; j++)
            alphas[size_t(i) * width + j] = uint8_t(row[j] >> 24);
    }
    const std::string image = d->maskImage(alphas, width, height);
    PdfMatrix place = {drawnWidth, 0, 0, drawnHeight, x, y};
    if (!mirrored) {
        place.d = -drawnHeight;
        place.f = y + drawnHeight;
    }
    Layer mask;
    mask.content = numbers(toMatrix(matrix) * place) + " cm\n/" + image + " Do\n";
    mask.resources.insert(image);
    const int maskObject = d->form(mask, "<< /Type /Group /S /Transparency /CS /DeviceGray >>");
    const int object = d->form(layer, "<< /Type /Group /S /Transparency >>");
    const std::string name = d->formName(object);
    d->use(name);
    d->emit("q\n/" + d->graphicsState(1, 1, 0, maskObject) + " gs\n/" + name + " Do\nQ\n");
}

} // namespace cutefish::reef
