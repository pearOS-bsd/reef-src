// driver GROUP... : draws the same into the original's PDF context, through its host table, and into PdfWriter, and compares the bytes.
// The fonts functions that PdfWriter needs are the original's, so that the faces are the same.
// A group is one of the names in main(), or fuzz=FIRST-LAST, or glyphs=FONT.
#include "pdf.h"
#include "fonts.h"
#include "paths.h"
#include "ui/bitmap.h"
#include "ui/canvas_2d.h"
#include <algorithm>
#include <cmath>
#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <link.h>
#include <limits>
#include <string>
#include <vector>

using namespace cutefish;
using namespace cutefish::reef;
using Face = reef_ui_face;

struct Table { uint64_t version; void* functions[317]; };
static Table* table;
static size_t base_;
// Which of the two runs, to tell whose crash it is.
static int side;
struct Theirs { int before = side; Theirs() { side = 1; } ~Theirs() { side = before; } };
template<typename R, typename... A> static R call(int index, A... args) { Theirs t; return reinterpret_cast<R (*)(A...)>(table->functions[index])(args...); }
template<typename R, typename... A> static R at(size_t address, A... args) { Theirs t; return reinterpret_cast<R (*)(A...)>(base_ + address)(args...); }

namespace cutefish::reef {
reef_ui_face* faceMatch(const reef_ui_face_query* q, int* e) { return call<Face*>(121, q, e); }
void faceRetain(Face* f) { call<void>(125, f); }
void faceRelease(Face* f) { call<void>(126, f); }
void faceMetrics(Face* f, reef_ui_face_metrics* m) { call<void>(127, f, m); }
int faceName(Face* f, int id, char* b, int s) { return call<int>(128, f, id, b, s); }
void faceGlyphs(Face* f, const uint32_t* c, uint32_t n, uint32_t* g) { call<void>(129, f, c, n, g); }
void faceAdvances(Face* f, const uint32_t* g, uint32_t n, int v, double* a) { call<void>(130, f, g, n, v, a); }
uint32_t faceOutline(Face* f, uint32_t g, reef_ui_path_element* e, uint32_t c) { return call<uint32_t>(132, f, g, e, c); }
Face* faceFallback(Face* f, uint32_t c, const char* l) { return call<Face*>(139, f, c, l); }
uint32_t faceShape(Face* f, const uint16_t* t, uint32_t len, uint32_t start, uint32_t n, double size, int flags, uint32_t script, const char* lang,
                   const reef_ui_feature* fe, uint32_t nf, reef_ui_glyph* g, uint32_t cap)
{
    return call<uint32_t>(140, f, t, len, start, n, size, flags, script, lang, fe, nf, g, cap);
}
double madeObliqueSlant() { return at<double>(0x80d50); }
double faceStretch(Face* f) { return at<double>(0x80d60, f); }
double madeBoldStrength() { return at<double>(0x80d80); }
OutlineFormat faceOutlineFormat(Face* f) { return at<OutlineFormat>(0x80d90, f); }
std::map<uint32_t, uint32_t> faceCharacters(Face* f) { return at<std::map<uint32_t, uint32_t>>(0x80e80, f); }
std::vector<reef_ui_feature> parseFeatures(const char* t) { return at<std::vector<reef_ui_feature>>(0x81a70, t); }
std::string faceSubset(Face* f, const std::vector<uint32_t>& g, OutlineFormat* k)
{
    return reinterpret_cast<std::string (*)(Face*, const std::vector<uint32_t>&, OutlineFormat*)>(base_ + 0x82200)(f, g, k);
}
void drawGlyphs(ui::Canvas2D& c, Face* f, const double* m, const uint32_t* g, const double* p, uint32_t n, int flags)
{
    reinterpret_cast<void (*)(ui::Canvas2D&, Face*, const double*, const uint32_t*, const double*, uint32_t, int)>(base_ + 0x90650)(c, f, m, g, p, n, flags);
}
} // namespace cutefish::reef

static Face* fromFile(const std::string& path, int index = 0) { return call<Face*>(123, path.c_str(), index); }

struct Random
{
    uint64_t state;
    uint64_t next() { state ^= state << 13; state ^= state >> 7; state ^= state << 17; return state * 0x2545f4914f6cdd1dull >> 16; }
    int below(int n) { return int(next() % uint64_t(n)); }
    int from(int a, int b) { return a + below(b - a + 1); }
    bool chance(int percent) { return below(100) < percent; }
    double unit() { return double(next() & 0xffffffff) / 4294967295.0; }
    double in(double a, double b) { return a + (b - a) * unit(); }
};

static std::string outputFolder = "/out/pdf";
static std::string caseName;
static int failures, documents, checks;
static size_t bytes;

static void write(void* context, const void* data, size_t size) { static_cast<std::string*>(context)->append(static_cast<const char*>(data), size); }

static int encryptMode;
static char* encrypt(void*, int object, int generation, const char* data, size_t size, size_t* encryptedSize)
{
    if (encryptMode == 2 && object % 5 == 0) return nullptr;
    const size_t total = encryptMode == 3 ? 0 : size + 16 - size % 16;
    char* result = static_cast<char*>(malloc(total + 1));
    for (size_t i = 0; i < total; i++) result[i] = char((i < size ? data[i] : 0x41) ^ (object * 37 + generation + i * 11));
    *encryptedSize = total;
    return result;
}

struct Pair
{
    std::string name;
    std::string original;
    std::string ours;
    void* gc;
    std::unique_ptr<PdfWriter> pdf;
    bool randomIdentifier = false;

    static reef_ui_pdf_document document()
    {
        reef_ui_pdf_document d = {};
        d.identifier = "0123456789abcdef";
        d.date = 1700000000;
        return d;
    }
    explicit Pair(const std::string& n, reef_ui_pdf_document d = document())
        : name(n)
    {
        randomIdentifier = !d.identifier;
        d.write = write;
        d.context = &original;
        gc = call<void*>(168, &d);
        d.context = &ours;
        pdf = std::make_unique<PdfWriter>(d);
        caseName = name;
        documents++;
    }
    PdfWriter* theirs() const { return *reinterpret_cast<PdfWriter**>(static_cast<char*>(gc) + 0xc8); }
    void beginPage(const double* media, const double* boxes = nullptr) { call<void>(169, gc, media, boxes); pdf->beginPage(media, boxes); }
    void beginPage(double w, double h) { const double m[] = {0, 0, w, h}; beginPage(m); }
    void endPage() { call<void>(170, gc); pdf->endPage(); }
    void addLink(double x, double y, double w, double h, const char* u, const char* d) { call<void>(171, gc, x, y, w, h, u, d); pdf->addLink(x, y, w, h, u, d); }
    void addDestination(const char* n, double x, double y) { call<void>(172, gc, n, x, y); pdf->addDestination(n, x, y); }
    void beginTag(const char* t, const char* a, const char* b, const char* c, const char* l) { call<void>(173, gc, t, a, b, c, l); pdf->beginTag(t, a, b, c, l); }
    void endTag() { call<void>(174, gc); pdf->endTag(); }
    void setOutline(const reef_ui_pdf_outline_item* i, unsigned n) { call<void>(175, gc, i, n); pdf->setOutline(i, n); }
    void setMetadata(const char* b, size_t n) { call<void>(176, gc, b, n); pdf->setMetadata(b, n); }
    void close() { call<void>(177, gc); pdf->close(); }

    void save() { call<void>(33, gc); pdf->save(); }
    void restore() { call<void>(34, gc); pdf->restore(); }
    void concat(const double* m) { call<void>(35, gc, m); pdf->concat(m); }
    void concat(double a, double b, double c, double d, double e, double f) { const double m[] = {a, b, c, d, e, f}; concat(m); }
    void setTransform(const double* m) { call<void>(36, gc, m); pdf->setTransform(m); }
    void setTransform(double a, double b, double c, double d, double e, double f) { const double m[] = {a, b, c, d, e, f}; setTransform(m); }
    void same(const double* a, const double* b, int n, const char* what)
    {
        checks++;
        if (memcmp(a, b, n * sizeof(double)) == 0) return;
        bool equal = true;
        for (int i = 0; i < n; i++) equal &= a[i] == b[i] || (std::isnan(a[i]) && std::isnan(b[i]));
        if (equal) return;
        failures++;
        printf("%s: %s differs:", name.c_str(), what);
        for (int i = 0; i < n; i++) printf(" %.17g/%.17g", a[i], b[i]);
        printf("\n");
    }
    void state()
    {
        double a[6], b[6];
        call<void>(37, gc, a); pdf->transform(b); same(a, b, 6, "transform");
        call<void>(42, gc, a); pdf->clipBounds(b); same(a, b, 4, "clipBounds");
        call<void>(148, gc, a); pdf->fillColor(b); same(a, b, 4, "fillColor");
        a[0] = call<double>(38, gc);
        double m[6]; pdf->transform(m); b[0] = std::sqrt(std::abs(m[0] * m[3] - m[1] * m[2])); same(a, b, 1, "scale");
    }
    void clipToRect(double x, double y, double w, double h) { call<void>(39, gc, x, y, w, h); pdf->clipToRect(x, y, w, h); }
    void clipToPath(const std::vector<reef_ui_path_element>& p, int e) { call<void>(40, gc, p.data(), unsigned(p.size()), e); pdf->clipToPath(p.data(), unsigned(p.size()), e); }
    void setAlpha(double a) { call<void>(43, gc, a); pdf->setAlpha(a); }
    void setComposite(int c) { call<void>(44, gc, c); pdf->setComposite(c); }
    void setSmoothing(int s) { call<void>(46, gc, s); pdf->setSmoothing(s); }
    void setFill(double r, double g, double b, double a) { call<void>(47, gc, r, g, b, a); pdf->setFillColor(r, g, b, a); }
    void setStroke(double r, double g, double b, double a) { call<void>(48, gc, r, g, b, a); pdf->setStrokeColor(r, g, b, a); }
    void setLineWidth(double w) { call<void>(49, gc, w); pdf->setLineWidth(w); }
    void setLineCap(int c) { call<void>(50, gc, c); pdf->setLineCap(c); }
    void setLineJoin(int c) { call<void>(51, gc, c); pdf->setLineJoin(c); }
    void setMiterLimit(double w) { call<void>(52, gc, w); pdf->setMiterLimit(w); }
    void setLineDash(const std::vector<double>& d, double o) { call<void>(53, gc, d.data(), unsigned(d.size()), o); pdf->setLineDash(d.data(), unsigned(d.size()), o); }
    void setShadow(double dx, double dy, double bl, double r, double g, double b, double a) { call<void>(54, gc, dx, dy, bl, r, g, b, a); pdf->setShadow(dx, dy, bl, r, g, b, a); }
    void fillPath(const std::vector<reef_ui_path_element>& p, int e) { call<void>(55, gc, p.data(), unsigned(p.size()), e); pdf->fillPath(p.data(), unsigned(p.size()), e); }
    void strokePath(const std::vector<reef_ui_path_element>& p) { call<void>(56, gc, p.data(), unsigned(p.size())); pdf->strokePath(p.data(), unsigned(p.size())); }
    void fillRect(double x, double y, double w, double h) { call<void>(57, gc, x, y, w, h); pdf->fillRect(x, y, w, h); }
    void strokeRect(double x, double y, double w, double h) { call<void>(58, gc, x, y, w, h); pdf->strokeRect(x, y, w, h); }
    void clearRect(double x, double y, double w, double h) { call<void>(59, gc, x, y, w, h); pdf->clearRect(x, y, w, h); }
    void linear(const std::vector<double>& o, const std::vector<double>& c, double x0, double y0, double x1, double y1, int e)
    {
        call<void>(60, gc, o.data(), c.data(), unsigned(o.size()), x0, y0, x1, y1, e);
        pdf->fillLinearGradient(o.data(), c.data(), unsigned(o.size()), x0, y0, x1, y1, e);
    }
    void radial(const std::vector<double>& o, const std::vector<double>& c, double x0, double y0, double r0, double x1, double y1, double r1, int e)
    {
        call<void>(61, gc, o.data(), c.data(), unsigned(o.size()), x0, y0, r0, x1, y1, r1, e);
        pdf->fillRadialGradient(o.data(), c.data(), unsigned(o.size()), x0, y0, r0, x1, y1, r1, e);
    }
    void drawPixels(const void* p, int w, int h, size_t s, int f, double x, double y, double dw, double dh, int m)
    {
        call<void>(62, gc, p, w, h, s, f, x, y, dw, dh, m);
        pdf->drawPixels(p, w, h, s, f, x, y, dw, dh, m);
    }
    void beginGroup() { call<void>(66, gc); pdf->beginGroup(); }
    void endGroup() { call<void>(67, gc); pdf->endGroup(); }
    void glyphs(Face* f, const double* m, const std::vector<uint32_t>& g, const std::vector<double>& p, int flags = 0)
    {
        call<void>(147, gc, f, m, g.data(), p.data(), uint32_t(g.size()), flags);
        pdf->drawGlyphs(f, m, g.data(), p.data(), uint32_t(g.size()), flags);
    }
    void beginMask() { call<void>(149, gc); pdf->beginMaskedGroup(); }
    void endMask(const void* p, int w, int h, size_t s, double x, double y, double dw, double dh, const double* m, int mirrored)
    {
        call<void>(150, gc, p, w, h, s, x, y, dw, dh, m, mirrored);
        pdf->endMaskedGroup(p, w, h, s, x, y, dw, dh, m, mirrored);
    }
    void text(const std::string& t, const std::vector<PdfTextSpan>& s, const std::function<void(ui::Canvas2D&)>& draw)
    {
        { Theirs t2; reinterpret_cast<void (*)(PdfWriter*, const std::string&, const std::vector<PdfTextSpan>&, const std::function<void(ui::Canvas2D&)>&)>(base_ + 0xedfe0)(theirs(), t, s, draw); }
        pdf->drawText(t, s, draw);
    }

    static void mask(std::string& s)
    {
        const size_t at = s.rfind("/ID [<");
        if (at == std::string::npos) return;
        for (size_t i = at + 6; i < s.size() && s[i] != ']'; i++) if (isxdigit(uint8_t(s[i]))) s[i] = '0';
    }
    void finish(bool closed = true)
    {
        if (closed) close();
        call<void>(178, gc);
        pdf.reset();
        gc = nullptr;
        if (randomIdentifier) { mask(original); mask(ours); }
        bytes += original.size();
        std::string file = outputFolder + "/" + name;
        if (original == ours) {
            if (getenv("KEEP")) std::ofstream(file + ".pdf", std::ios::binary) << original;
            return;
        }
        failures++;
        size_t i = 0;
        while (i < original.size() && i < ours.size() && original[i] == ours[i]) i++;
        printf("%s: differs at %zu (sizes %zu and %zu)\n", name.c_str(), i, original.size(), ours.size());
        std::ofstream(file + ".theirs.pdf", std::ios::binary) << original;
        std::ofstream(file + ".ours.pdf", std::ios::binary) << ours;
    }
    ~Pair() { if (gc) finish(); }
};

static const double NaN = std::numeric_limits<double>::quiet_NaN();
static const double Inf = std::numeric_limits<double>::infinity();
using Path = std::vector<reef_ui_path_element>;

static Path star(double x, double y, double r)
{
    Path p;
    for (int i = 0; i < 5; i++) p.push_back({i ? 1 : 0, {x + r * std::sin(i * 4 * M_PI / 5), y - r * std::cos(i * 4 * M_PI / 5)}});
    p.push_back({4, {}});
    return p;
}
static Path curves(double x, double y, double s)
{
    return {{0, {x, y}}, {2, {x + s, y, x + s, y + s}}, {3, {x + s, y + 2 * s, x, y + 2 * s, x, y + s}}, {4, {}}, {1, {x - s, y - s}}, {0, {x + s / 2, y + s / 2}}, {1, {x + s / 3, y + s / 3}}, {3, {x, y, x + 1, y + 1, x + 2, y + 3}}};
}
static std::vector<uint32_t> picture(int w, int h, int stride, int kind)
{
    std::vector<uint32_t> v(size_t(stride / 4) * h);
    Random r{uint64_t(w * 31 + h * 17 + kind + 1)};
    for (int y = 0; y < h; y++)
        for (int x = 0; x < stride / 4; x++) {
            uint32_t a = kind == 0 ? 255 : kind == 1 ? r.below(256) : kind == 2 ? (x * 255 / std::max(1, w - 1)) : kind == 3 ? 0 : 255 - (x + y == 0);
            uint32_t c[3];
            for (uint32_t& k : c) k = kind == 5 ? r.below(256) : uint32_t(r.below(256)) * a / 255;
            v[size_t(y) * (stride / 4) + x] = a << 24 | c[0] << 16 | c[1] << 8 | c[2];
        }
    return v;
}

static std::vector<std::string> fontFiles()
{
    std::vector<std::string> v;
    for (const auto& e : std::filesystem::recursive_directory_iterator("/tk/app/fonts/fontset")) if (e.is_regular_file()) v.push_back(e.path());
    for (const auto& e : std::filesystem::recursive_directory_iterator("/usr/share/fonts")) if (e.is_regular_file()) v.push_back(e.path());
    std::sort(v.begin(), v.end());
    return v;
}
static void run(Pair& p, Face* face, const std::u32string& text, double x, double y, double size, const double* matrix = nullptr)
{
    if (!face) return;
    std::vector<uint32_t> g(text.size());
    std::vector<double> pos;
    for (size_t i = 0; i < text.size(); i++) {
        uint32_t c = text[i];
        call<void>(129, face, &c, 1u, &g[i]);
        pos.push_back(x + double(i) * size * 0.6);
        pos.push_back(y + double(i % 3));
    }
    const double m[] = {size, 0, 0, -size};
    p.glyphs(face, matrix ? matrix : m, g, pos);
}

static void groupDocument()
{
    { Pair p("empty"); p.close(); p.close(); }
    { Pair p("unclosed"); p.beginPage(100, 100); p.fillRect(1, 2, 3, 4); p.finish(false); }
    { Pair p("nothing"); p.finish(false); }
    { Pair p("afterclose"); p.beginPage(100, 100); p.close(); p.beginPage(10, 10); p.fillRect(0, 0, 1, 1); p.save(); p.restore(); p.endPage(); p.beginGroup(); p.endGroup(); }
    { Pair p("outside"); p.endPage(); p.fillRect(0, 0, 1, 1); p.strokeRect(0, 0, 1, 1); p.clearRect(0, 0, 1, 1); p.save(); p.concat(2, 0, 0, 2, 1, 1); p.state(); p.restore(); p.restore(); p.clipToRect(1, 2, 3, 4); p.state();
      p.addLink(0, 0, 1, 1, "a", nullptr); p.addDestination("d", 1, 2); p.beginGroup(); p.endGroup(); p.beginMask(); const double m[] = {1, 0, 0, 1, 0, 0}; uint32_t px = 0x80ffffff; p.endMask(&px, 1, 1, 4, 0, 0, 1, 1, m, 0);
      p.drawPixels(&px, 1, 1, 4, 0, 0, 0, 1, 1, 0); p.linear({0, 1}, {0, 0, 0, 1, 1, 1, 1, 1}, 0, 0, 1, 1, 3); p.radial({0, 1}, {0, 0, 0, 1, 1, 1, 1, 1}, 0, 0, 1, 1, 1, 2, 3); p.beginTag("P", nullptr, nullptr, nullptr, nullptr); p.endTag(); p.endTag();
      p.setShadow(1, 1, 1, 0, 0, 0, 1); p.beginGroup(); p.beginPage(100, 100); p.fillRect(1, 2, 3, 4); p.endGroup(); }
    { Pair p("shadowgroupoutside"); p.setShadow(1, 1, 1, 0, 0, 0, 1); p.beginGroup(); p.fillRect(0, 0, 5, 5); p.endGroup(); p.beginPage(20, 20); p.fillRect(0, 0, 5, 5); }
    const double media[][4] = {{0, 0, 612, 792}, {10, 20, 300, 400}, {-5, -6.5, 100.25, 200.125}, {0, 0, 0, 10}, {3, 4, -5, 6}, {0, 0, 1e7, 1e-7}, {NaN, 0, 10, 10}, {0, 0, 14400, 14400}, {0, 0, 10, NaN}};
    int i = 0;
    for (const auto& m : media) {
        Pair p("media" + std::to_string(i++));
        double boxes[16];
        for (int k = 0; k < 16; k++) boxes[k] = k * 1.5 + 1;
        p.beginPage(m);
        p.fillRect(1, 1, 5, 5); p.state();
        p.beginPage(m, boxes);
        boxes[2] = 0; boxes[7] = -1; boxes[10] = NaN;
        p.beginPage(nullptr, boxes);
        p.state();
        p.beginPage(nullptr);
        p.state();
    }
    { Pair p("many"); for (int k = 0; k < 30; k++) { p.beginPage(50 + k, 60 + k); p.fillRect(k, k, 5, 5); if (k % 3 == 0) p.endPage(); } }
    const char* strings[] = {nullptr, "", "Plain title (with) \\ <angle>", "Grüße äöü", "日本語 \U0001f600 test", "bad \xff\xc3 utf\xe2\x82", "ctl\n\t\x7f", "& < > \" '", "\xf0\x9f\x98", "\xc3\xa9", "\xed\xa0\x80 \xf8\x88\x80\x80\x80 \x80\xbf", "\xf7\xbf\xbf\xbf \xf4\x90\x80\x80 \xc0\x80 \xe0\x80\x80 \xf0\x80\x80\x80", "\xf4\x8f\xbf\xbf\xef\xbf\xbf\xed\xbf\xbf \xff", "\xe9", "\xe6\x97", "\xf0\x9f"};
    int n = 0;
    for (const char* s : strings)
        for (int archival = 0; archival < 2; archival++) {
            reef_ui_pdf_document d = Pair::document();
            d.title = s; d.author = strings[(n + 3) % 16]; d.subject = strings[(n + 5) % 16]; d.keywords = strings[(n + 7) % 16]; d.creator = strings[(n + 9) % 16];
            d.archival = archival;
            d.date = n % 4 == 0 ? 0 : n % 4 == 1 ? -5 : n % 4 == 2 ? 86400.9 * (n + 1) : 4e9;
            Pair p("strings" + std::to_string(n++), d);
            p.beginPage(100, 100);
            if (n % 3) p.setMetadata("<xmp>own</xmp>", n % 2 ? 14 : 5);
            if (n % 5 == 0) p.setMetadata(nullptr, 10);
            p.fillRect(1, 2, 3, 4);
        }
    const char* intents[] = {nullptr, "", "<< /Type /OutputIntent /S /GTS_PDFX /Info <414243> /Other <> /X <4> /DestOutputProfile %PROFILE% >>", "<< /A (text) >>", "%PROFILE%%PROFILE%", "<41", "<<<41>>>", "<4142> <43 44> <<", "a <0a0B> b < 1> <+1>", "<"};
    n = 0;
    for (const char* s : intents)
        for (int profile = 0; profile < 3; profile++)
            for (int components : {0, 1, 3, 4, -2})
                for (int archival = 0; archival < 2; archival++)
                    for (int e = 0; e < 3; e++) {
                        reef_ui_pdf_document d = Pair::document();
                        d.outputIntents = s;
                        d.archival = archival;
                        d.profileComponents = components;
                        if (profile == 1) { d.profile = "profile\0data"; d.profileSize = 12; }
                        if (profile == 2) { d.profile = "x"; d.profileSize = 0; }
                        encryptMode = e;
                        if (e) { d.encrypt = encrypt; d.encryption = e == 1 ? "<< /Filter /Standard >>" : nullptr; }
                        // std::stoi throws on what has no digits.
                        if (e && s && strstr(s, "< 1>") ) {}
                        Pair p("intents" + std::to_string(n++), d);
                        p.beginPage(10, 10);
                    }
    for (int e = 1; e < 4; e++) {
        reef_ui_pdf_document d = Pair::document();
        d.title = "Title"; d.author = "Grüße";
        encryptMode = e;
        d.encrypt = encrypt;
        d.encryption = "<< /Filter /Standard /V 4 >>";
        Pair p("encrypted" + std::to_string(e), d);
        p.beginPage(100, 100);
        p.fillRect(1, 2, 3, 4);
        p.addLink(1, 2, 3, 4, "https://example.org/", nullptr);
        p.addLink(5, 6, 7, 8, nullptr, "dest");
        p.addDestination("dest", 3, 4);
        p.beginTag("P", "actual", "alt", "title", "en");
        auto px = picture(5, 5, 20, 1);
        p.drawPixels(px.data(), 5, 5, 20, 0, 1, 1, 9, 9, 0);
        run(p, fromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"), U"Hello", 10, 20, 12);
        p.endTag();
        reef_ui_pdf_outline_item items[] = {{"One", 0, 1, NaN, NaN, nullptr}, {"Two", 1, 0, 0, 0, "https://example.org/x"}};
        p.setOutline(items, 2);
        p.setMetadata("<x/>", 4);
    }
    { reef_ui_pdf_document d = Pair::document(); d.identifier = nullptr; Pair p("random", d); p.beginPage(10, 10); }
    { reef_ui_pdf_document d = Pair::document(); static const char id[] = "\0\1\2\3\377\376\0\0abcdefgh"; d.identifier = id; Pair p("identifier", d); p.beginPage(10, 10); }
}

static void groupStructure()
{
    {
        Pair p("links");
        p.beginPage(200, 300);
        p.addLink(1, 2, 30, 40, "https://example.org/?a=(b)", nullptr);
        p.addLink(1, 2, 30, 40, "", "dest");
        p.addLink(1, 2, 30, 40, nullptr, nullptr);
        p.addLink(1, 2, 30, 40, "u", "dest");
        p.concat(0, 1, -1, 0, 100, 5);
        p.addLink(1, 2, -30, 40, "http://\xc3\xa9", nullptr);
        p.addLink(NaN, 2, 30, 40, "nan", nullptr);
        p.addDestination("a", 1, 2);
        p.addDestination("b", 1, 2);
        p.addDestination("a", 3, 4);
        p.addDestination(nullptr, 3, 4);
        p.addDestination("", 3, 4);
        p.addDestination("Grüße", NaN, Inf);
        p.beginPage(nullptr);
        p.addDestination("second", 5.123456789, 6.5);
        p.addLink(0, 0, 10, 10, nullptr, "a");
    }
    std::vector<std::vector<reef_ui_pdf_outline_item>> outlines = {
        {},
        {{"One", 0, 1, 1.5, 2.5, nullptr}},
        {{"One", 0, 1, NaN, NaN, nullptr}, {"Two", 1, 2, NaN, 3, nullptr}, {"Three", 1, 2, 4, NaN, nullptr}, {"Four", 2, 1, 0, 0, nullptr}, {"Five", 0, 1, 0, 0, nullptr}},
        {{nullptr, 5, 0, 0, 0, nullptr}, {"Grüße", -3, 3, 0, 0, nullptr}, {"url", 1, 1, 0, 0, "https://x"}, {"url2", 9, 9, 1, 1, ""}, {"back", 2, -1, 1, 1, nullptr}, {"x", 2, 1, Inf, -Inf, nullptr}, {"y", 1, 1, 1e9, -1e-9, nullptr}},
        {{"a", 0, 1, 0, 0, nullptr}, {"b", 1, 1, 0, 0, nullptr}, {"c", 2, 1, 0, 0, nullptr}, {"d", 3, 1, 0, 0, nullptr}, {"e", 2, 1, 0, 0, nullptr}, {"f", 1, 1, 0, 0, nullptr}, {"g", 0, 1, 0, 0, nullptr}, {"h", 1, 1, 0, 0, nullptr}},
    };
    int n = 0;
    for (const auto& o : outlines) {
        Pair p("outline" + std::to_string(n++));
        p.beginPage(100, 200);
        const double m[] = {5, 6, 70, 80};
        p.beginPage(m);
        p.setOutline(outlines[4].data(), 8);
        p.setOutline(o.data(), unsigned(o.size()));
    }
    { Pair p("outlinenopages"); p.setOutline(outlines[3].data(), 7); }
    {
        Random r{77};
        for (int i = 0; i < 40; i++) {
            Pair p("outlinefuzz" + std::to_string(i));
            for (int k = r.below(4); k >= 0; k--) p.beginPage(50, 60 + k);
            std::vector<reef_ui_pdf_outline_item> o;
            static const char* titles[] = {"A", "B\xc3\xa9", nullptr, ""};
            for (int k = r.below(25); k > 0; k--) o.push_back({titles[r.below(4)], r.from(-1, 5), r.from(-1, 5), r.chance(20) ? NaN : r.in(-9, 99), r.chance(20) ? NaN : r.in(-9, 99), r.chance(15) ? "http://u" : r.chance(10) ? "" : nullptr});
            p.setOutline(o.data(), unsigned(o.size()));
        }
    }
    {
        Pair p("tags");
        p.beginTag("Document", nullptr, nullptr, "Doc title", "en-US");
        p.beginPage(100, 100);
        p.beginTag("H1", nullptr, nullptr, nullptr, nullptr);
        p.fillRect(1, 1, 2, 2);
        p.endTag();
        p.beginTag(nullptr, "actual", nullptr, nullptr, nullptr);
        p.beginTag("", nullptr, "Grüße", nullptr, "de");
        p.fillRect(2, 2, 2, 2);
        p.beginGroup();
        p.beginTag("Figure (1)/x", nullptr, "in a group", nullptr, nullptr);
        p.fillRect(3, 3, 2, 2);
        p.endTag();
        p.endGroup();
        p.beginPage(100, 100);
        p.fillRect(4, 4, 2, 2);
        p.beginTag("Sp an", "a", "b", "c", "d");
        p.endTag();
        p.endTag();
        p.endTag();
        p.endPage();
        p.beginTag("Between", nullptr, nullptr, nullptr, nullptr);
        p.endTag();
        p.beginTag("Open\xc3\xa9", nullptr, nullptr, nullptr, nullptr);
        p.beginPage(100, 100);
        p.save();
        p.beginTag("P", nullptr, nullptr, nullptr, nullptr);
        p.restore();
        p.beginGroup();
        p.endTag();
        p.beginGroup();
        p.fillRect(5, 5, 2, 2);
    }
    {
        Random r{99};
        for (int i = 0; i < 60; i++) {
            Pair p("tagfuzz" + std::to_string(i));
            static const char* s[] = {nullptr, "", "P", "Span", "X#Y", "\xc3\xa9", "T D"};
            for (int k = 0; k < 40; k++) {
                switch (r.below(8)) {
                case 0: p.beginPage(50, 50); break;
                case 1: p.endPage(); break;
                case 2: case 3: p.beginTag(s[r.below(7)], s[r.below(7)], s[r.below(7)], s[r.below(7)], s[r.below(7)]); break;
                case 4: p.endTag(); break;
                case 5: p.beginGroup(); break;
                case 6: p.endGroup(); break;
                case 7: p.fillRect(r.below(9), 1, 2, 3); break;
                }
            }
        }
    }
}

static void groupPaths()
{
    std::vector<Path> paths = {star(50, 50, 40), curves(20, 20, 30), {}, {{0, {1, 1}}}, {{4, {}}}, {{1, {5, 5}}, {2, {6, 6, 7, 7}}}, {{7, {1, 2}}, {-1, {3, 4}}, {0, {1, 1}}, {1, {2, 2}}},
                               {{0, {NaN, 1}}, {1, {Inf, 2}}, {1, {-Inf, 1e300}}, {3, {1e-7, 1e-6, 4.9e-6, 5.1e-6, -5e-6, 123456789.123456}}},
                               {{0, {1.0 / 3, 2.0 / 3}}, {2, {1e-5, -1e-5, 1e5, 1e6}}, {2, {10, 20, 30, 40}}, {4, {}}, {2, {10, 20, 30, 40}}}};
    int n = 0;
    for (const Path& path : paths)
        for (int e = 0; e < 4; e++) {
            Pair p("path" + std::to_string(n++));
            p.beginPage(100, 100);
            p.fillPath(path, e);
            p.strokePath(path);
            p.setFill(0.1, 0.2, 0.3, 0.4);
            p.setStroke(1, 0.5, 0.25, 0.125);
            p.setAlpha(0.5);
            p.fillPath(path, e);
            p.strokePath(path);
            p.concat(1.5, 0.5, -0.25, 1, 10, 20);
            p.setLineWidth(2.5);
            p.setLineCap(e);
            p.setLineJoin(3 - e);
            p.setMiterLimit(e * 2 - 1);
            p.setLineDash({1, 2.5, 3}, e);
            p.fillPath(path, e);
            p.strokePath(path);
            p.save();
            p.clipToPath(path, e);
            p.state();
            p.fillPath(star(50, 50, 30), 0);
            p.restore();
            p.state();
            p.setLineDash({}, 0);
            p.setLineWidth(0);
            p.setLineCap(-4);
            p.setLineJoin(99);
            p.strokePath(path);
        }
    for (int c = -2; c < 32; c++) {
        Pair p("composite" + std::to_string(c + 2));
        p.beginPage(100, 100);
        p.setComposite(c);
        p.fillRect(1, 1, 5, 5);
        p.strokeRect(1, 1, 5, 5);
        p.setAlpha(0.3);
        p.fillRect(1, 1, 5, 5);
        p.strokeRect(1, 1, 5, 5);
        auto px = picture(4, 4, 16, 1);
        p.drawPixels(px.data(), 4, 4, 16, 0, 1, 1, 9, 9, 0);
        p.linear({0, 1}, {0, 0, 0, 1, 1, 1, 1, 1}, 0, 0, 1, 1, 3);
        p.beginGroup();
        p.fillRect(1, 1, 5, 5);
        p.endGroup();
        run(p, fromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"), U"Ab", 10, 20, 12);
        run(p, fromFile("/tk/app/fonts/fontset/type1/NimbusSans-Regular.t1"), U"Ab", 10, 40, 12);
        run(p, fromFile("/tk/app/fonts/fontset/color/NotoColorEmoji.ttf"), U"\U0001f600", 10, 60, 12);
    }
    {
        Pair p("alphas");
        p.beginPage(100, 100);
        for (double a : {-1.0, 0.0, 0.25, 0.999999, 1.0, 1.5, NaN, Inf})
            for (double f : {-1.0, 0.0, 0.5, 1.0, 2.0, NaN}) {
                p.setAlpha(a);
                p.setFill(a, f, -f, f);
                p.setStroke(f, a, 2, f);
                p.fillRect(1, 2, 3, 4);
                p.strokeRect(1, 2, 3, 4);
            }
    }
    {
        Pair p("rects");
        p.beginPage(100, 100);
        for (double w : {0.0, -5.0, 5.0, 1e-9, NaN, Inf}) {
            p.fillRect(1, 2, w, 4);
            p.strokeRect(1, 2, 3, w);
            p.clearRect(1, w, 3, 4);
            p.save();
            p.clipToRect(w, 2, 30, w);
            p.state();
            p.restore();
        }
        p.setFill(0.5, 0.5, 0.5, 0.5);
        p.concat(2, 1, 0, 2, 3, 4);
        p.clearRect(1, 2, 3, 4);
        p.beginGroup();
        p.clearRect(1, 2, 3, 4);
        p.endGroup();
        p.state();
    }
    {
        Pair p("transforms");
        p.beginPage(100, 100);
        const double ms[][6] = {{1, 0, 0, 1, 0, 0}, {2, 0, 0, 3, 4, 5}, {0, 0, 0, 0, 1, 2}, {1e-7, 0, 0, 1e-7, 0, 0}, {1, 2, 2, 4, 5, 6}, {NaN, 0, 0, 1, 0, 0}, {1e6, 0, 0, -1e6, 1e3, 1e3}, {0.1, 0.7, -0.3, 0.2, 12.5, -7}};
        for (const auto& m : ms) {
            p.save();
            p.concat(m);
            p.state();
            p.fillRect(1, 2, 3, 4);
            p.clipToRect(0, 0, 20, 20);
            p.state();
            p.concat(m);
            p.state();
            p.setTransform(m);
            p.state();
            p.strokeRect(1, 2, 3, 4);
            p.restore();
            p.state();
        }
    }
    {
        Pair p("saves");
        p.beginPage(100, 100);
        p.save(); p.save(); p.restore(); p.restore(); p.restore(); p.restore();
        p.save(); p.save(); p.beginGroup(); p.restore(); p.save(); p.save(); p.fillRect(0, 0, 1, 1); p.endGroup(); p.restore(); p.state();
        p.beginGroup(); p.save(); p.clipToRect(0, 0, 5, 5);
        p.endPage();
        p.beginPage(100, 100);
        p.restore();
        p.save();
    }
}

static void groupImages()
{
    int n = 0;
    for (int kind = 0; kind < 6; kind++)
        for (int format = 0; format < 3; format++)
            for (int mirrored = 0; mirrored < 3; mirrored++) {
                Pair p("image" + std::to_string(n++));
                p.beginPage(100, 100);
                auto px = picture(7, 5, 40, kind);
                p.drawPixels(px.data(), 7, 5, 40, format, 10, 20, 30, 40, mirrored);
                p.drawPixels(px.data(), 7, 5, 40, format, 10, 20, 30, 40, mirrored);
                p.setSmoothing(mirrored);
                p.drawPixels(px.data(), 7, 5, 40, format, 10, 20, -30, 40, mirrored);
                p.setSmoothing(0);
                p.drawPixels(px.data(), 7, 5, 40, format, 10, 20, 30, 40, mirrored);
                p.setAlpha(0.5);
                p.concat(0.5, 0.25, -0.125, 2, 3, 4);
                p.drawPixels(px.data(), 5, 5, 40, format, 10, 20, 30, 40, mirrored);
                p.drawPixels(px.data(), 1, 1, 4, format, 10, 20, 30, 40, mirrored);
                p.drawPixels(px.data(), 0, 1, 4, format, 10, 20, 30, 40, mirrored);
                p.drawPixels(px.data(), 1, -1, 4, format, 10, 20, 30, 40, mirrored);
                p.drawPixels(nullptr, 1, 1, 4, format, 10, 20, 30, 40, mirrored);
                p.drawPixels(px.data(), 2, 2, 8, format, NaN, 20, Inf, 40, mirrored);
                p.setShadow(3, 4, 2, 1, 0, 0, 0.75);
                p.drawPixels(px.data(), 7, 5, 40, format, 10, 20, 30, 40, mirrored);
                p.beginPage(100, 100);
                p.drawPixels(px.data(), 7, 5, 40, format, 10, 20, 30, 40, mirrored);
            }
    { Pair p("imagebig"); p.beginPage(300, 300); auto px = picture(400, 300, 1600, 1); p.drawPixels(px.data(), 400, 300, 1600, 0, 0, 0, 300, 300, 0); p.drawPixels(px.data(), 400, 300, 1600, 1, 0, 0, 300, 300, 0); }
}

static void groupGradients()
{
    const std::vector<std::vector<double>> offsets = {{}, {0}, {0.5}, {1}, {0, 1}, {0.25, 0.75}, {0, 0.5, 1}, {0.2, 0.2, 0.8}, {1, 0}, {-1, 2}, {0, 0.1, 0.2, 0.3, 0.4, 0.9, 1}, {NaN, 1}, {0.5, 0.5}, {0, 0}, {1, 1}, {0.9, 0.1, 0.5}};
    int n = 0;
    for (const auto& o : offsets)
        for (int opaque = 0; opaque < 2; opaque++)
            for (int extend = 0; extend < 5; extend++) {
                Pair p("gradient" + std::to_string(n++));
                p.beginPage(100, 100);
                std::vector<double> c;
                Random r{uint64_t(n)};
                for (size_t i = 0; i < o.size(); i++)
                    for (int k = 0; k < 4; k++) c.push_back(k == 3 ? (opaque ? 1 : i == 0 ? 0.5 : r.in(-0.5, 1.5)) : r.in(-0.2, 1.2));
                p.linear(o, c, 0, 0, 100, 50, extend);
                p.radial(o, c, 10, 20, 5, 30, 40, 50, extend);
                p.radial(o, c, 10, 20, -5, 30, 40, -1, extend);
                p.setAlpha(0.5);
                p.concat(0.5, 0.25, -0.125, 2, 3, 4);
                p.linear(o, c, 10, 10, 10, 10, extend);
                p.save();
                p.clipToPath(star(50, 50, 30), 1);
                p.setComposite(3);
                p.linear(o, c, NaN, 0, Inf, 50, extend);
                p.radial(o, c, 0.1234567, 20, 0, 30, 40, NaN, extend);
                p.restore();
                p.setShadow(2, 2, 3, 0, 0, 0, 1);
                p.linear(o, c, 0, 0, 10, 0, extend);
                p.beginGroup();
                p.linear(o, c, 0, 0, 10, 0, extend);
                p.radial(o, c, 10, 20, 5, 30, 40, 50, extend);
                p.endGroup();
            }
}

static void groupGroups()
{
    int n = 0;
    for (int shadow = 0; shadow < 3; shadow++)
        for (double alpha : {1.0, 0.5})
            for (int composite : {0, 1, 5, 26, 27}) {
                Pair p("group" + std::to_string(n++));
                p.beginPage(80, 90);
                p.setAlpha(alpha);
                p.setComposite(composite);
                if (shadow) p.setShadow(3 * shadow, -2, shadow == 2 ? 4 : 0, 0.5, 0.25, 1, 0.75);
                p.setFill(1, 0.5, 0, 0.5);
                p.beginGroup();
                p.state();
                p.fillRect(10, 10, 20, 20);
                p.setFill(0, 0.5, 1, 1);
                p.fillPath(star(40, 40, 20), 1);
                p.strokePath(curves(20, 20, 10));
                p.setAlpha(0.3);
                p.save();
                p.clipToRect(30, 30, 30, 30);
                p.beginGroup();
                p.fillRect(35, 35, 50, 50);
                p.setShadow(1, 1, 1, 0, 0, 0, 1);
                p.fillRect(40, 40, 5, 5);
                auto px = picture(7, 5, 28, 1);
                p.drawPixels(px.data(), 7, 5, 28, 0, 50, 50, 20, 20, 0);
                p.linear({0, 1}, {1, 0, 0, 1, 0, 0, 1, 0.5}, 0, 0, 80, 80, 3);
                p.beginMask();
                p.fillRect(0, 0, 40, 40);
                const double m[] = {1, 0.1, 0.2, 1, 3, 4};
                p.endMask(px.data(), 7, 5, 28, 5, 6, 30, 30, m, shadow);
                p.endGroup();
                p.restore();
                p.clearRect(0, 0, 10, 10);
                p.endGroup();
                p.state();
                p.fillRect(60, 60, 5, 5);
                p.beginGroup();
                p.beginGroup();
                p.fillRect(60, 70, 5, 5);
                p.endPage();
                p.beginPage(50, 50);
                p.endGroup();
                p.endGroup();
                p.fillRect(1, 1, 5, 5);
            }
    n = 0;
    for (int kind = 0; kind < 4; kind++)
        for (int mirrored = 0; mirrored < 2; mirrored++)
            for (int shadowed = 0; shadowed < 2; shadowed++) {
                Pair p("mask" + std::to_string(n++));
                p.beginPage(80, 90);
                auto px = picture(6, 9, 32, kind);
                const double m[] = {0.5, 0.25, -0.5, 1, 20, 10};
                if (shadowed) { p.setShadow(2, 3, 2, 0, 0, 0, 0.5); p.beginGroup(); }
                p.beginMask();
                p.setFill(1, 0, 0, 1);
                p.fillRect(0, 0, 60, 60);
                p.save();
                p.concat(2, 0, 0, 2, 0, 0);
                p.fillRect(0, 0, 10, 10);
                p.endMask(px.data(), 6, 9, 32, 5, 5, 40, 50, m, mirrored);
                p.beginMask();
                p.fillRect(0, 0, 60, 60);
                p.endMask(nullptr, 6, 9, 32, 5, 5, 40, 50, m, mirrored);
                p.beginMask();
                p.fillRect(0, 0, 60, 60);
                p.endMask(px.data(), 0, 9, 32, 5, 5, 40, 50, m, mirrored);
                p.beginMask();
                p.fillRect(1, 1, 60, 60);
                p.endGroup();
                p.beginGroup();
                p.endMask(px.data(), 6, 9, 32, 5, 5, 40, 50, m, mirrored);
                p.endGroup();
                p.endMask(px.data(), 6, 9, 32, 5, 5, 40, -50, m, mirrored);
                p.endMask(px.data(), 6, 9, 32, 5, 5, 40, 50, m, mirrored);
                if (shadowed) p.endGroup();
                p.fillRect(1, 1, 60, 60);
            }
}

static void groupShadows()
{
    int n = 0;
    for (double blur : {0.0, 0.5, 1.0, 3.0, 10.0, -2.0})
        for (double dx : {0.0, 5.0, -7.25}) {
            Pair p("shadow" + std::to_string(n++));
            p.beginPage(60, 50);
            p.setShadow(dx, dx * 0.5, blur, 0.2, 0.4, 1.5, n % 2 ? 1 : 0.5);
            p.setFill(1, 0, 0, 0.75);
            p.fillRect(10, 10, 20, 15);
            p.setStroke(0, 1, 0, 0.5);
            p.setLineWidth(3);
            p.setLineJoin(n % 3);
            p.setLineCap(n % 3);
            p.setLineDash({4, 2}, 1);
            p.strokePath(star(30, 25, 15));
            p.setAlpha(0.5);
            p.concat(1.25, 0.5, -0.5, 0.75, 5, 0);
            p.fillPath(curves(10, 10, 10), 1);
            p.clipToRect(0, 0, 20, 20);
            p.fillPath(curves(10, 10, 10), 0);
            p.setLineDash({}, 0);
            p.setMiterLimit(0.5);
            p.strokeRect(5, 5, 5, 5);
            auto px = picture(6, 9, 32, 1);
            p.drawPixels(px.data(), 6, 9, 32, 0, 5, 5, 10, 10, 1);
            p.drawPixels(px.data(), 6, 9, 32, 1, 5, 5, 10, 10, 0);
            run(p, fromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"), U"Shadow", 2, 12, 10);
            run(p, fromFile("/tk/app/fonts/fontset/type1/NimbusSans-Regular.t1"), U"Shadow", 2, 14, 10);
            run(p, fromFile("/tk/app/fonts/fontset/color/NotoColorEmoji.ttf"), U"\U0001f600", 2, 16, 10);
            p.fillRect(500, 500, 10, 10);
            p.fillRect(0, 0, 0, 0);
            p.setComposite(2);
            p.fillRect(1, 1, 4, 4);
            p.setShadow(dx, 1, blur, 0.2, 0.4, 1.5, 0);
            p.fillRect(1, 1, 4, 4);
            p.setShadow(dx, 1, blur, 0.2, 0.4, 1.5, -1);
            p.fillRect(1, 1, 4, 4);
            p.setShadow(dx, 1, blur, 0.2, 0.4, 1.5, NaN);
            p.fillRect(1, 1, 4, 4);
            p.setShadow(NaN, 1, 1, 0.2, 0.4, 1.5, 1);
            p.fillRect(1, 1, 4, 4);
        }
    { Pair p("shadowbig"); p.beginPage(612, 792); p.setShadow(10, 10, 20, 0, 0, 0, 0.5); p.fillRect(50, 50, 500, 600); p.beginGroup(); p.fillRect(50, 50, 500, 600); p.endGroup(); }
    { Pair p("shadowhuge"); p.beginPage(5000, 3000); p.setShadow(10, 10, 20, 0, 0, 0, 0.5); p.fillRect(50, 50, 4500, 2600); p.beginGroup(); p.fillRect(50, 50, 4500, 2000); p.endGroup(); }
    { Pair p("shadowoffset"); const double m[] = {100, 200, 60, 50}; p.beginPage(m); p.setShadow(3, 3, 2, 0, 0, 0, 0.5); p.fillRect(110, -190, 20, 20); p.beginGroup(); p.fillRect(110, -190, 20, 20); p.endGroup(); p.state(); }
}

static void groupGlyphs(const std::string& filter)
{
    int n = 0;
    for (const std::string& file : fontFiles()) {
        n++;
        if (!filter.empty() && file.find(filter) == std::string::npos) continue;
        for (int index = 0; index < (file.find(".ttc") != std::string::npos ? 2 : 1); index++) {
            Face* face = fromFile(file, index);
            if (!face) { printf("no face of %s\n", file.c_str()); continue; }
            reef_ui_face_metrics metrics = {};
            call<void>(127, face, &metrics);
            Pair p("glyphs" + std::to_string(n) + "-" + std::to_string(index));
            p.beginPage(200, 200);
            run(p, face, U"Hello, World! fi 123", 10, 20, 12);
            run(p, face, U"Привет мир", 10, 40, 12);
            run(p, face, U"日本語", 10, 60, 12);
            run(p, face, U"\U0001f600\U0001f3fd", 10, 80, 12);
            run(p, face, U"שלום مرحبا", 10, 100, 12);
            p.setAlpha(0.5);
            p.setFill(0.2, 0.4, 0.6, 0.8);
            const double m[] = {10, 3, -2, -11};
            run(p, face, U"Matrix", 10, 120, 12, m);
            std::vector<uint32_t> g;
            std::vector<double> pos;
            for (uint32_t i = 0; i < 40; i++) { g.push_back(i * 7 % 64); pos.push_back(i * 5); pos.push_back(150); }
            for (uint32_t x : {metrics.glyphCount - 1, metrics.glyphCount, metrics.glyphCount + 1, 65535u, 65536u, 100000u}) { g.push_back(x); pos.push_back(5); pos.push_back(170); }
            const double size[] = {9, 0, 0, -9};
            p.glyphs(face, size, g, pos);
            p.glyphs(face, size, {}, {});
            p.glyphs(nullptr, size, g, pos);
            p.setComposite(1);
            p.concat(0.5, 0, 0, 0.5, 50, 100);
            run(p, face, U"Composite", 10, 120, 12);
            p.setShadow(1, 2, 2, 1, 0, 0, 0.5);
            run(p, face, U"Shadow", 10, 150, 12);
            p.beginPage(100, 100);
            run(p, face, U"Second page", 10, 20, 12);
            p.beginGroup();
            run(p, face, U"Group", 10, 40, 12);
            p.endGroup();
            p.setShadow(1, 2, 2, 1, 0, 0, 0.5);
            p.beginGroup();
            run(p, face, U"Group", 10, 60, 12);
            p.endGroup();
            p.finish();
            call<void>(126, face);
        }
    }
}

// Faces that are made bold, oblique or stretched, which only a match gives.
static void groupMatched()
{
    int n = 0;
    for (const char* family : {"DejaVu Sans", "Lato", "Liberation Serif", "Noto Sans", "FreeSerif", "Loma", "Symbola", "Unifont", "WenQuanYi Zen Hei", "Noto Color Emoji", "IPAGothic", "C059", "Nimbus Sans", "Hack", "D050000L", "Noto Sans CJK JP"})
        for (double weight : {-1.0, 300.0, 400.0, 700.0, 900.0})
            for (double width : {-1.0, 3.0, 5.0, 7.0})
                for (int italic = 0; italic < 2; italic++) {
                    const reef_ui_face_query q = {nullptr, family, nullptr, weight, width, italic, -1, nullptr};
                    Face* face = call<Face*>(121, &q, nullptr);
                    n++;
                    if (!face) continue;
                    Pair p("matched" + std::to_string(n));
                    p.beginPage(100, 100);
                    run(p, face, U"Hello fi", 10, 20, 12);
                    const double m[] = {10, 3, -2, -11};
                    p.setAlpha(0.5);
                    run(p, face, U"日本 Ж", 10, 60, 12, m);
                    p.setShadow(1, 1, 1, 0, 0, 0, 1);
                    run(p, face, U"S", 10, 80, 12);
                    p.finish();
                    call<void>(126, face);
                }
}

static PdfTextSpan span(const std::string& family, double size, size_t begin, size_t end)
{
    PdfTextSpan s;
    s.begin = begin;
    s.end = end;
    s.family = family;
    s.size = size;
    s.baseline = 30;
    s.x = [size](size_t i) { return 5 + double(i) * size * 0.5; };
    return s;
}

static void groupText()
{
    const std::vector<std::string> texts = {"Hello, World!", "fi ffl AV To.", "Grüße aus Köln", "日本語テキスト", "مرحبا بالعالم 123", "שלום Hello עולם", "mix \U0001f600 emoji \U0001f1e9\U0001f1ea flag \U0001f468‍\U0001f469‍\U0001f467", "a\tb\nc\x7f d e f",
                                            "bad \xff utf\xe2\x82 end\xc3", "हिन्दी देवनागरी", "ไทย ตัวอย่าง", "é ä combining", "", "Привет", "‮override", "\U00010800\U0001e900", "x", "\xf7\xbf\xbf\xbf \xf4\x90\x80\x80 \xc0\x80 \xe0\x80\x80", "\xed\xa0\x80\xed\xb0\x80 \xe6\x97"};
    int n = 0;
    for (const std::string& t : texts)
        for (const char* family : {"DejaVu Sans", "Lato", "Noto Sans", "Nimbus Sans", "Liberation Serif", "Noto Sans CJK JP", "No such family", ""})
            for (int style = 0; style < 4; style++) {
                Pair p("text" + std::to_string(n++));
                p.beginPage(200, 60);
                PdfTextSpan s = span(family, 12.5, 0, t.size());
                s.bold = style & 1;
                s.italic = style & 2;
                s.underline = style == 1 || style == 2;
                s.features = style == 3 ? "liga=0,smcp" : style == 2 ? "kern=0" : "";
                s.color[0] = 0.5; s.color[3] = style == 3 ? 0.5 : 1;
                int calls = 0;
                const auto draw = [&](ui::Canvas2D& c) { calls++; c.setFillColor({1, 0, 0, 1}); c.fillRect({10, 10, 50, 10}); };
                p.text(t, {s}, draw);
                if (style == 2) { p.setShadow(2, 2, 2, 0, 0, 1, 0.5); p.text(t, {s}, draw); }
                if (style == 3) { p.setShadow(2, 2, 2, 0, 0, 1, 0.5); p.beginGroup(); p.text(t, {s}, draw); p.setShadow(1, 1, 1, 1, 1, 1, 1); p.text(t, {s}, draw); p.endGroup(); }
                p.state();
            }
    {
        Pair p("spans");
        p.beginPage(300, 100);
        const std::string t = "One two three مرحبا four";
        std::vector<PdfTextSpan> spans;
        for (size_t i = 0; i + 4 < t.size(); i += 4) {
            PdfTextSpan s = span(i % 8 ? "Lato" : "DejaVu Serif", 8 + double(i) / 3, i, i + 4);
            s.baseline = 20 + double(i) * 2;
            s.underline = i % 12 == 0;
            spans.push_back(s);
        }
        spans.push_back(span("Lato", 10, 5, 5));
        spans.push_back(span("Lato", 10, 7, 6));
        spans.push_back(span("Lato", 10, 7, 600));
        p.text(t, spans, [](ui::Canvas2D&) {});
        p.text(t, {}, [](ui::Canvas2D&) {});
        p.setShadow(2, 2, 2, 0, 0, 1, 0.5);
        p.text(t, spans, [](ui::Canvas2D& c) { c.fillRect({0, 0, 20, 20}); });
        p.setAlpha(0.5);
        p.setComposite(4);
        p.concat(0.5, 0.5, -0.5, 0.5, 100, 10);
        p.text(t, spans, [](ui::Canvas2D& c) { c.fillRect({0, 0, 20, 20}); });
        p.beginMask();
        p.text(t, spans, [](ui::Canvas2D& c) { c.fillRect({0, 0, 20, 20}); });
        p.endPage();
        p.text(t, spans, [](ui::Canvas2D&) {});
    }
}

// What the other groups leave out.
static void groupMore()
{
    { Pair p("gradientoutside"); p.linear({0, 1}, {0, 0, 0, 1, 1, 1, 1, 1}, 0, 0, 1, 1, 3); p.radial({0, 1}, {0, 0, 0, 1, 1, 1, 1, 0}, 0, 0, 1, 1, 1, 2, 3); p.clearRect(0, 0, 1, 1); p.fillRect(0, 0, 1, 1);
      uint32_t px = 0x80ffffff; p.drawPixels(&px, 1, 1, 4, 0, 0, 0, 1, 1, 0); p.text("x", {span("Lato", 10, 0, 1)}, [](ui::Canvas2D&) {}); run(p, fromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"), U"Ab", 10, 20, 12); p.beginPage(10, 10); }
    for (int extend = 0; extend < 4; extend++) { Pair p("gradientnan" + std::to_string(extend)); p.beginPage(50, 50); p.linear({NaN}, {0.25, 0.5, 0.75, extend & 1 ? 0.5 : 1}, 0, 0, 1, 1, extend); p.linear({NaN, NaN, NaN}, {0, 0, 0, 1, 1, 1, 1, NaN, 0, NaN, 0, 1}, 0, 0, 1, 1, extend); p.radial({0.5, NaN}, {0, 0, 0, 1, 1, 1, 1, 0}, 0, 0, 1, 1, 1, 2, extend); }
    {
        Pair p("emojiclipped");
        p.beginPage(100, 100);
        Face* face = fromFile("/tk/app/fonts/fontset/color/NotoColorEmoji.ttf");
        run(p, face, U"\U0001f600", 500, 20, 12);
        run(p, face, U"\U0001f600", 95, 20, 12);
        p.clipToRect(0, 0, 10, 10);
        run(p, face, U"\U0001f600", 50, 50, 12);
        run(p, face, U"\U0001f600", 5, 12, 12);
        p.setShadow(2, 2, 2, 0, 0, 0, 1);
        run(p, face, U"\U0001f600", 50, 50, 12);
        run(p, face, U"\U0001f600", 5, 12, 12);
        const double zero[] = {0, 0, 0, 0};
        run(p, face, U"\U0001f600", 5, 12, 12, zero);
        run(p, fromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"), U"Zero", 5, 12, 12, zero);
    }
    {
        Pair p("emptyspans");
        p.beginPage(100, 100);
        std::vector<PdfTextSpan> spans = {span("Lato", 10, 0, 3), span("Lato", 0, 3, 5), span("Lato", 0, 5, 5), span("Lato", -4, 5, 8)};
        p.text("some text", spans, [](ui::Canvas2D& c) { c.fillRect({0, 0, 20, 20}); });
        p.setShadow(1, 2, 2, 0, 0, 0, 1);
        p.text("some text", spans, [](ui::Canvas2D& c) { c.fillRect({0, 0, 20, 20}); });
        std::swap(spans[0], spans[2]);
        p.text("some text", spans, [](ui::Canvas2D& c) { c.fillRect({0, 0, 20, 20}); });
        p.setTransform(0, 0, 0, 0, 0, 0);
        p.text("some text", spans, [](ui::Canvas2D& c) { c.fillRect({0, 0, 20, 20}); });
    }
    // One character becomes more than two glyphs.
    for (const char* family : {"Noto Sans Tamil", "Noto Sans Devanagari", "Noto Sans Khmer", "Noto Sans Thai", "Noto Naskh Arabic"}) {
        std::string t;
        for (int i = 0; i < 40; i++) t += i % 4 == 3 ? "\u0bcc" : "\u0bca";
        for (int i = 0; i < 40; i++) t += "\u17be\u17c4";
        for (int i = 0; i < 10; i++) t += "\ufdfa";
        for (int i = 0; i < 40; i++) t += "\u0e33\u0949";
        Pair p(std::string("decomposed-") + family);
        p.beginPage(400, 100);
        p.text(t, {span(family, 12, 0, t.size())}, [](ui::Canvas2D&) {});
        std::string u;
        for (int i = 0; i < 60; i++) u += "\u0bca";
        p.text(u, {span(family, 12, 0, u.size())}, [](ui::Canvas2D&) {});
    }
    // Faces that are narrowed.
    int n = 0;
    for (const char* family : {"Avenir Next Condensed", "AvenirNextCondensed"})
        for (double weight : {-1.0, 200.0, 400.0, 500.0, 600.0, 700.0, 800.0, 950.0})
            for (int italic = 0; italic < 2; italic++) {
                const reef_ui_face_query q = {nullptr, family, nullptr, weight, -1, italic, -1, nullptr};
                Face* face = call<Face*>(121, &q, nullptr);
                Pair p("narrow" + std::to_string(n++));
                p.beginPage(200, 100);
                run(p, face, U"Narrow 123 fi", 10, 20, 12);
                const double m[] = {10, 3, -2, -11};
                run(p, face, U"Narrow", 10, 50, 12, m);
                p.setShadow(1, 1, 1, 0, 0, 0, 1);
                run(p, face, U"Narrow", 10, 80, 12, m);
                PdfTextSpan s = span(family, 12, 0, 12);
                s.bold = weight > 500; s.italic = italic; s.underline = true;
                p.text("Narrow \xc3\xa9 ab", {s}, [](ui::Canvas2D& c) { c.fillRect({0, 0, 20, 20}); });
                p.finish();
                if (face) call<void>(126, face);
            }
    // Outlines of more than 256 elements, of a font that is not embedded.
    if (Face* face = fromFile("/tk/app/pdf/test/big.pfa")) {
        Pair p("bigoutline");
        p.beginPage(200, 200);
        reef_ui_path_element e[4];
        for (uint32_t c : {65u, 67u, 68u}) { uint32_t g; call<void>(129, face, &c, 1u, &g); printf("outline of %u elements, format %d\n", call<uint32_t>(132, face, g, e, 4u), at<int>(0x80d90, face)); }
        run(p, face, U"CDBAB", 10, 50, 20);
        const double m[] = {10, 3, -2, -11};
        run(p, face, U"AB", 10, 120, 20, m);
        p.setShadow(1, 1, 1, 0, 0, 0, 1);
        run(p, face, U"A", 10, 150, 20, m);
        p.finish();
        call<void>(126, face);
    } else {
        failures++;
        printf("no big.pfa\n");
    }
    // Every glyph, in runs of 64.
    for (const std::string& file : fontFiles()) {
        Face* face = fromFile(file);
        if (!face) continue;
        reef_ui_face_metrics metrics = {};
        call<void>(127, face, &metrics);
        const bool outlines = !(metrics.flags & 8) && (metrics.flags & 16) && at<int>(0x80d90, face) == 0;
        if (outlines || metrics.glyphCount < 1200) {
            Pair p("all" + std::to_string(n++));
            p.beginPage(700, 700);
            for (uint32_t g = 0; g < metrics.glyphCount && g < 3000; g += 64) {
                std::vector<uint32_t> glyphs; std::vector<double> pos;
                for (uint32_t k = g; k < g + 64 && k < metrics.glyphCount; k++) { glyphs.push_back(k); pos.push_back((k - g) * 10); pos.push_back(20 + g / 64 * 12); }
                const double size[] = {9, 0, 0, -9};
                if (!(metrics.flags & 8) && (metrics.flags & 16)) p.glyphs(face, size, glyphs, pos);
            }
            p.finish();
        }
        call<void>(126, face);
    }
}

// The largest outlines of the faces that cannot be embedded.
static void groupOutlines()
{
    for (const std::string& file : fontFiles()) {
        Face* face = fromFile(file);
        if (!face) continue;
        reef_ui_face_metrics metrics = {};
        call<void>(127, face, &metrics);
        const int format = at<int>(0x80d90, face);
        uint32_t most = 0, which = 0;
        if (format == 0 && (metrics.flags & 16)) {
            std::vector<reef_ui_path_element> e(10);
            for (uint32_t g = 0; g < metrics.glyphCount; g++) { const uint32_t n = call<uint32_t>(132, face, g, e.data(), 10u); if (n > most) { most = n; which = g; } }
        }
        printf("%d %x %u %u %s\n", format, metrics.flags, most, which, file.c_str());
        call<void>(126, face);
    }
}

static void fuzz(uint64_t seed, const std::vector<Face*>& faces)
{
    Random r{seed * 0x9e3779b97f4a7c15ull + 1};
    for (int i = 0; i < 10; i++) r.next();
    reef_ui_pdf_document d = Pair::document();
    static const char* strings[] = {nullptr, "", "Title", "Grüße", "a<b>&\"", "\xff\xfe"};
    d.title = strings[r.below(6)]; d.author = strings[r.below(6)]; d.subject = strings[r.below(6)]; d.keywords = strings[r.below(6)]; d.creator = strings[r.below(6)];
    d.archival = r.chance(10);
    if (r.chance(20)) { encryptMode = r.from(1, 3); d.encrypt = encrypt; d.encryption = strings[r.below(6)]; }
    if (r.chance(10)) d.outputIntents = "<< /Info <41> /P %PROFILE% >>";
    Pair p("fuzz" + std::to_string(seed), d);
    const auto coordinate = [&] { return r.chance(3) ? 0 : r.chance(1) ? NaN : r.in(-20, 100); };
    const auto size = [&] { return r.chance(5) ? 0 : r.chance(5) ? -r.in(0, 40) : r.in(0, 60); };
    const auto path = [&] {
        Path e;
        for (int k = r.below(8); k >= 0; k--) {
            reef_ui_path_element x = {e.empty() ? 0 : r.from(0, 4), {}};
            for (double& v : x.points) v = coordinate();
            e.push_back(x);
        }
        return e;
    };
    const auto stops = [&](std::vector<double>& o, std::vector<double>& c) {
        for (int k = r.below(5); k > 0; k--) {
            o.push_back(r.in(-0.1, 1.1));
            for (int j = 0; j < 4; j++) c.push_back(j == 3 && r.chance(70) ? 1 : r.unit());
        }
        if (r.chance(80)) std::sort(o.begin(), o.end());
    };
    const auto matrix = [&](double* m) {
        if (r.chance(50)) { const double s = r.in(0.25, 2); m[0] = s; m[1] = m[2] = 0; m[3] = r.chance(20) ? -s : s; } else for (int k = 0; k < 4; k++) m[k] = r.in(-1.5, 1.5);
        m[4] = r.in(-20, 40); m[5] = r.in(-20, 40);
    };
    const bool big = r.chance(5);
    int shadows = 0;
    const int steps = r.from(5, 80);
    for (int step = 0; step < steps; step++) {
        double m[6];
        std::vector<double> o, c;
        switch (r.below(40)) {
        case 0: { const double box[] = {r.chance(70) ? 0 : r.in(-20, 20), r.chance(70) ? 0 : r.in(-20, 20), big ? 300 : r.in(20, 90), big ? 400 : r.in(20, 90)}; double boxes[16]; for (double& v : boxes) v = r.in(-5, 50); p.beginPage(r.chance(95) ? box : nullptr, r.chance(20) ? boxes : nullptr); break; }
        case 1: if (r.chance(30)) p.endPage(); break;
        case 2: case 3: p.save(); break;
        case 4: case 5: p.restore(); break;
        case 6: matrix(m); p.concat(m); break;
        case 7: if (r.chance(50)) { matrix(m); p.setTransform(m); } else p.setTransform(1, 0, 0, 1, 0, 0); break;
        case 8: p.clipToRect(coordinate(), coordinate(), size(), size()); break;
        case 9: p.clipToPath(path(), r.below(2)); break;
        case 10: p.setAlpha(r.chance(50) ? 1 : r.in(-0.1, 1.1)); break;
        case 11: p.setComposite(r.chance(60) ? 0 : r.from(-1, 28)); break;
        case 12: p.setFill(r.unit(), r.unit(), r.unit(), r.chance(50) ? 1 : r.unit()); break;
        case 13: p.setStroke(r.unit(), r.unit(), r.unit(), r.chance(50) ? 1 : r.unit()); break;
        case 14: p.setLineWidth(r.in(0, 6)); p.setLineCap(r.below(3)); p.setLineJoin(r.below(3)); p.setMiterLimit(r.in(0, 12)); break;
        case 15: { std::vector<double> v; for (int k = r.below(4); k > 0; k--) v.push_back(r.in(0.5, 9)); p.setLineDash(v, r.in(-3, 3)); break; }
        case 16: if (r.chance(50) && shadows < 12) { shadows++; p.setShadow(r.in(-6, 6), r.in(-6, 6), r.chance(30) ? 0 : r.in(0, 8), r.unit(), r.unit(), r.unit(), r.unit()); } else p.setShadow(0, 0, 0, 0, 0, 0, 0); break;
        case 17: case 18: p.fillPath(path(), r.below(2)); break;
        case 19: p.strokePath(path()); break;
        case 20: p.fillRect(coordinate(), coordinate(), size(), size()); break;
        case 21: p.strokeRect(coordinate(), coordinate(), size(), size()); break;
        case 22: p.clearRect(coordinate(), coordinate(), size(), size()); break;
        case 23: { const int w = r.from(1, 12), h = r.from(1, 12); auto px = picture(w, h, (w + r.below(3)) * 4, r.below(6)); p.setSmoothing(r.below(3)); p.drawPixels(px.data(), w, h, px.size() / h * 4, r.below(2), coordinate(), coordinate(), size(), size(), r.below(2)); break; }
        case 24: stops(o, c); p.linear(o, c, coordinate(), coordinate(), coordinate(), coordinate(), r.below(4)); break;
        case 25: stops(o, c); p.radial(o, c, coordinate(), coordinate(), size(), coordinate(), coordinate(), size(), r.below(4)); break;
        case 26: case 27: p.beginGroup(); break;
        case 28: case 29: p.endGroup(); break;
        case 30: p.beginMask(); break;
        case 31: { const int w = r.from(1, 12), h = r.from(1, 12); auto px = picture(w, h, w * 4, r.below(5)); matrix(m); p.endMask(r.chance(90) ? px.data() : nullptr, w, h, w * 4, coordinate(), coordinate(), size(), size(), m, r.below(2)); break; }
        case 32: case 33: case 34: {
            Face* face = faces[r.below(int(faces.size()))];
            reef_ui_face_metrics metrics = {};
            call<void>(127, face, &metrics);
            std::vector<uint32_t> g; std::vector<double> pos;
            for (int k = r.below(10); k > 0; k--) { g.push_back(r.chance(95) ? uint32_t(r.below(int(std::min(metrics.glyphCount, 4000u)))) : uint32_t(r.below(80000))); pos.push_back(coordinate()); pos.push_back(coordinate()); }
            const double s = r.in(4, 20);
            const double plain[] = {s, 0, 0, -s};
            for (int k = 0; k < 4; k++) m[k] = r.in(-15, 15);
            p.glyphs(face, r.chance(60) ? plain : m, g, pos, r.below(3));
            break;
        }
        case 35: {
            static const char* texts[] = {"Hello", "fi של", "日本 x", "\U0001f600!", "á\t", "مرحبا"};
            static const char* families[] = {"DejaVu Sans", "Lato", "Nimbus Sans", "Noto Sans CJK JP", "Hack"};
            const std::string t = texts[r.below(6)];
            PdfTextSpan s = span(families[r.below(5)], r.in(5, 20), 0, t.size());
            s.bold = r.chance(30); s.italic = r.chance(30); s.underline = r.chance(30);
            s.baseline = coordinate();
            for (double& v : s.color) v = r.unit();
            p.text(t, {s}, [](ui::Canvas2D& c) { c.setFillColor({0, 0, 1, 0.5}); c.fillRect({3, 4, 30, 20}); });
            break;
        }
        case 36: p.addLink(coordinate(), coordinate(), size(), size(), strings[r.below(6)], strings[r.below(6)]); break;
        case 37: p.addDestination(strings[r.below(6)], coordinate(), coordinate()); break;
        case 38: if (r.chance(50)) p.beginTag(strings[r.below(6)], strings[r.below(6)], strings[r.below(6)], strings[r.below(6)], strings[r.below(6)]); else p.endTag(); break;
        case 39: p.state(); break;
        }
        if (step == 0) p.beginPage(big ? 300 : r.in(20, 90), big ? 400 : r.in(20, 90));
    }
}

// Each in a process of its own: what programs never do, such as more restores than saves in a group with a shadow, may crash the toolkit
// or keep it in an endless loop.
static int crashes[2];
static void crashed(int) { _exit(10 + side); }
static void fuzzApart(uint64_t seed, const std::vector<Face*>& faces)
{
    fflush(stdout);
    const pid_t child = fork();
    if (child == 0) {
        for (int s : {SIGSEGV, SIGABRT, SIGBUS, SIGFPE, SIGILL, SIGALRM}) signal(s, crashed);
        alarm(60);
        failures = 0; checks = 0; bytes = 0;
        fuzz(seed, faces);
        fflush(stdout);
        _exit(failures != 0);
    }
    int status = 0;
    waitpid(child, &status, 0);
    documents++;
    const int code = WIFEXITED(status) ? WEXITSTATUS(status) : 99;
    if (code == 1) failures++;
    else if (code == 11) { crashes[1]++; if (getenv("VERBOSE")) printf("seed %llu: the original crashed\n", (unsigned long long)seed); }
    else if (code) { crashes[0]++; failures++; printf("seed %llu: ours crashed (%d)\n", (unsigned long long)seed, code); }
}

int main(int argc, char** argv)
{
    void* lib = dlopen("/o/lib/cutefish-reef/host/libreef-appkit-host.so", RTLD_NOW | RTLD_LOCAL);
    if (!lib) { fprintf(stderr, "%s\n", dlerror()); return 1; }
    table = static_cast<Table*>(reinterpret_cast<void* (*)(int)>(dlsym(lib, "reef_appkit_host_interface"))(0x18));
    link_map* map;
    dlinfo(lib, RTLD_DI_LINKMAP, &map);
    base_ = map->l_addr;
    if (reinterpret_cast<size_t>(table->functions[168]) - base_ != 0x55280) { fprintf(stderr, "another library\n"); return 1; }
    if (const char* f = getenv("OUTPUT")) outputFolder = f;
    std::filesystem::create_directories(outputFolder);
    std::vector<Face*> faces;
    for (int i = 1; i < argc; i++) {
        const std::string g = argv[i];
        const int failuresBefore = failures, documentsBefore = documents, checksBefore = checks;
        const size_t bytesBefore = bytes;
        if (g == "document") groupDocument();
        else if (g == "structure") groupStructure();
        else if (g == "paths") groupPaths();
        else if (g == "images") groupImages();
        else if (g == "gradients") groupGradients();
        else if (g == "groups") groupGroups();
        else if (g == "shadows") groupShadows();
        else if (g == "glyphs") groupGlyphs("");
        else if (g.rfind("glyphs=", 0) == 0) groupGlyphs(g.substr(7));
        else if (g == "matched") groupMatched();
        else if (g == "text") groupText();
        else if (g == "more") groupMore();
        else if (g == "outlines") groupOutlines();
        else if (g.rfind("fuzz=", 0) == 0) {
            if (faces.empty()) {
                for (const std::string& f : fontFiles()) if (f.find("DejaVuSans.ttf") != std::string::npos || f.find("Lato-Regular") != std::string::npos || f.find("C059-Italic.otf") != std::string::npos || f.find("NimbusSans") != std::string::npos || f.find("Emoji") != std::string::npos || f.find("6x13.pcf") != std::string::npos || f.find("CJK") != std::string::npos || f.find("woff2") != std::string::npos)
                    if (Face* face = fromFile(f)) faces.push_back(face);
                for (double weight : {400.0, 700.0}) { const reef_ui_face_query q = {nullptr, "Loma", nullptr, weight, 3, 1, -1, nullptr}; if (Face* face = call<Face*>(121, &q, nullptr)) faces.push_back(face); }
                printf("%zu faces\n", faces.size());
            }
            unsigned long long a = 0, b = 0;
            sscanf(g.c_str() + 5, "%llu-%llu", &a, &b);
            for (unsigned long long s = a; s <= b; s++) fuzzApart(s, faces);
            printf("crashes and endless loops of the original: %d, of ours: %d\n", crashes[1], crashes[0]);
        } else { fprintf(stderr, "unknown %s\n", g.c_str()); return 2; }
        printf("%s: %d documents, %zu bytes, %d values, %d differences\n", g.c_str(), documents - documentsBefore, bytes - bytesBefore, checks - checksBefore, failures - failuresBefore);
        fflush(stdout);
    }
    return failures != 0;
}
