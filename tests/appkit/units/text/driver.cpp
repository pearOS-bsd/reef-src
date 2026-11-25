// Runs the text and paragraph functions of the original library and ours on the same inputs and compares what they do.
// See run.sh.
#include "text.h"
#include "paragraph.h"
#include "fonts.h"
#include "graphics.h"
#include "ui/application.h"
#include "ui/canvas_2d.h"
#include "ui/theme.h"
#include <cmath>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <link.h>
#include <string>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using namespace cutefish;
namespace ours = cutefish::reef;
extern "C" uintptr_t originalBase;

static void** table;
template <typename R, typename... A>
static R orig(int index, A... a)
{
    return reinterpret_cast<R (*)(A...)>(table[1 + index])(a...);
}

static unsigned long checks, failures;
static std::string context;
static bool same(double a, double b)
{
    return memcmp(&a, &b, sizeof a) == 0 || (std::isnan(a) && std::isnan(b)) || (a == 0 && b == 0);
}
static void fail(const char* what)
{
    failures++;
    if (failures < 60)
        printf("DIFFERENT %s: %s\n", what, context.c_str());
}
static void check(const char* what, double a, double b)
{
    checks++;
    if (!same(a, b)) {
        fail(what);
        if (failures < 60)
            printf("    %.17g %.17g\n", a, b);
    }
}
static void check(const char* what, size_t a, size_t b)
{
    checks++;
    if (a != b) {
        fail(what);
        if (failures < 60)
            printf("    %zu %zu\n", a, b);
    }
}
static void check(const char* what, const std::string& a, const std::string& b)
{
    checks++;
    if (a != b) {
        fail(what);
        if (failures < 60)
            printf("    %zu %zu bytes\n", a.size(), b.size());
    }
}

static uint32_t seed = 1;
static uint32_t rnd(uint32_t n)
{
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) % n;
}
static double rndf(double lo, double hi)
{
    return lo + (hi - lo) * (rnd(10000) / 9999.0);
}

struct Canvas
{
    static constexpr int width = 900, height = 700;
    std::vector<uint32_t> pixels = std::vector<uint32_t>(width * height, 0xffffffff);
    reef_ui_gc* gc;
    explicit Canvas(int format = 0)
    {
        gc = orig<reef_ui_gc*>(69, (void*)pixels.data(), width, height, size_t(width * 4), format);
    }
    ~Canvas() { orig<void>(70, gc); }
    size_t ink() const
    {
        size_t n = 0;
        for (uint32_t p : pixels)
            n += p != 0xffffffff;
        return n;
    }
};

static size_t inked;
template <typename F, typename G>
static void drawBoth(const char* what, F drawOriginal, G drawOurs, const base::AffineTransform* transform = nullptr, double alpha = 1)
{
    Canvas a, b;
    for (Canvas* c : {&a, &b}) {
        if (transform)
            c->gc->canvas->setTransform(*transform);
        c->gc->canvas->setGlobalAlpha(alpha);
    }
    drawOriginal(a.gc);
    drawOurs(b.gc);
    checks++;
    inked += a.ink() > 0;
    if (a.pixels != b.pixels)
        fail(what);
}

static const char* families[] = {nullptr, "", "sans-serif", "Noto Sans", "monospace", "Menlo", "Monaco", "Courier", "DejaVu Serif", "Liberation Sans",
                                 "Liberation Mono", "No Such Family", "Noto Sans Mono", "serif", "Helvetica", "DejaVu Sans"};
static const double sizes[] = {13, 11, 9, 10, 12, 15, 24, 14.5, 7.25, 40, 0, -3, 12.5, 17, 64};
static const double scales[] = {1, 2, 1.5, 0, -1, 1.25, 3};

// U+XXXX text of many kinds.
static const char* texts[] = {
    "", " ", "Hello, World!", "The quick brown fox jumps over the lazy dog", "fi ffi fl AVAWAT To Ty", "a\tb\tcc\tddd\t", "\t\tindented", "naïve café Ångström",
    "e\xcc\x81o\xcc\x88u\xcc\x83 combining", "\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d \xd7\xa2\xd7\x95\xd7\x9c\xd7\x9d",
    "abc \xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d def \xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa7\xd9\x85 xyz 123", "\xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa7\xd9\x85 \xd8\xb9\xd9\x84\xd9\x8a\xd9\x83\xd9\x85",
    "\xf0\x9f\x98\x80 smile \xf0\x9f\x91\xa8\xe2\x80\x8d\xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x91\xa7 family \xf0\x9f\x87\xb7\xf0\x9f\x87\xb4 flag \xe2\x9d\xa4\xef\xb8\x8f heart",
    "\xe4\xbd\xa0\xe5\xa5\xbd\xe4\xb8\x96\xe7\x95\x8c \xe3\x81\x93\xe3\x82\x93\xe3\x81\xab\xe3\x81\xa1\xe3\x81\xaf \xec\x95\x88\xeb\x85\x95",
    "\xe0\xa4\xa8\xe0\xa4\xae\xe0\xa4\xb8\xe0\xa5\x8d\xe0\xa4\xa4\xe0\xa5\x87", "line\nbreak\r\nhere", "trailing spaces   ", "   leading", "a b  c   d    e",
    "Averyverylongwordwithoutanyspacesthatcannotbebrokenexceptanywhereinthemiddleofitself andthenanother", "\x01\x02 control \x7f chars",
    "bad \xff\xfe utf8 \xc3( \xe2\x82 \xf0\x9f end\xc3", "\xc2\xa0nbsp\xc2\xa0here", "0123456789", "\xe2\x80\x8bzero\xe2\x80\x8bwidth\xc2\xadshy",
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
    "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum.",
    "\xd8\xa8\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x84\xd9\x87 1234 \xd7\xa9\xd7\x9c mixed (parens) \xd7\x94\xd7\x99\xd7\x99 [x]"};
constexpr size_t textCount = sizeof texts / sizeof *texts;

static std::string randomText()
{
    std::string s;
    const unsigned n = rnd(5) == 0 ? rnd(40) : 1 + rnd(4);
    for (unsigned i = 0; i < n; i++) {
        s += texts[rnd(textCount)];
        if (rnd(2))
            s += ' ';
    }
    return s.substr(0, 1500);
}

static void testFontMetrics()
{
    for (const char* family : families) {
        for (double size : sizes) {
            for (double scale : scales) {
                for (int style = 0; style < 4; style++) {
                    for (int again = 0; again < (size == 13 ? 2 : 1); again++) {
                        double a[3] = {-1, -1, -1}, b[3] = {-1, -1, -1};
                        context = std::string("metrics ") + (family ? family : "null") + " " + std::to_string(size) + " " + std::to_string(scale) + " "
                                + std::to_string(style);
                        orig<void>(76, family, size, style & 1, style & 2, scale, a, a + 1, a + 2);
                        ours::textFontMetrics(family, size, style & 1, style & 2, scale, b, b + 1, b + 2);
                        check("ascent", a[0], b[0]);
                        check("descent", a[1], b[1]);
                        check("leading", a[2], b[2]);
                    }
                }
            }
        }
    }
}

static void testFamilies()
{
    context = "families";
    for (int size : {0, 1, 5, 64, 100000, -1}) {
        std::vector<char> a(100001, 'x'), b(100001, 'x');
        check("length", size_t(orig<int>(97, a.data(), size)), size_t(ours::textFontFamilies(b.data(), size)));
        check("families", std::string(a.begin(), a.end()), std::string(b.begin(), b.end()));
    }
    check("null", size_t(orig<int>(97, (char*)nullptr, 10)), size_t(ours::textFontFamilies(nullptr, 10)));
}

static void testText()
{
    for (int round = 0; round < 900; round++) {
        const bool null = round == 5;
        const std::string string = round < int(textCount) ? texts[round] : randomText();
        const char* text = null ? nullptr : string.c_str();
        const char* family = families[rnd(sizeof families / sizeof *families)];
        double size = sizes[rnd(sizeof sizes / sizeof *sizes)];
        const double scale = scales[rnd(sizeof scales / sizeof *scales)];
        const int bold = rnd(3) == 0, italic = rnd(3) == 0 ? 7 : 0, kerned = rnd(3) == 0;
        double c[4] = {rndf(-0.2, 1.2), rndf(-0.2, 1.2), rndf(-0.2, 1.2), rnd(2) ? 1 : rndf(-0.2, 1.2)};
        if (rnd(20) == 0)
            c[rnd(4)] = NAN;
        context = "text '" + string + "' " + (family ? family : "null") + " " + std::to_string(size) + " scale " + std::to_string(scale);
        reef_ui_text* a = orig<reef_ui_text*>(71, text, family, size, bold, italic, scale, c[0], c[1], c[2], c[3], kerned);
        reef_ui_text* b = ours::textCreate(text, family, size, bold, italic, scale, c[0], c[1], c[2], c[3], kerned);
        double m[3], n[3];
        orig<void>(73, a, m, m + 1, m + 2);
        ours::textMetrics(b, n, n + 1, n + 2);
        check("width", m[0], n[0]);
        check("ascent", m[1], n[1]);
        check("descent", m[2], n[2]);
        check("text", a->text, b->text);
        check("family", a->family, b->family);
        check("size", a->size, b->size);
        check("bold", size_t(a->bold), size_t(b->bold));
        check("italic", size_t(a->italic), size_t(b->italic));
        for (int i = 0; i < 4; i++)
            check("color", a->color[i], b->color[i]);
        for (size_t offset = 0; offset <= string.size() + 3; offset++)
            check("offset to x", orig<double>(74, a, offset), ours::textOffsetToX(b, offset));
        check("offset to x", orig<double>(74, a, size_t(-1)), ours::textOffsetToX(b, size_t(-1)));
        for (double x = -20; x < m[0] + 30; x += 3.7)
            check("x to offset", orig<size_t>(75, a, x), ours::textXToOffset(b, x));
        check("x to offset", orig<size_t>(75, a, double(INFINITY)), ours::textXToOffset(b, INFINITY));
        // The functions of one on what the other made.
        check("offset to x of the other", orig<double>(74, b, string.size() / 2), ours::textOffsetToX(a, string.size() / 2));
        const base::AffineTransform transforms[] = {{1, 0, 0, 1, 0, 0}, {2, 0, 0, 2, 3, 4}, {1, 0, 0, -1, 0, 600}, {1.5, 0, 0, 1.5, 0.3, 0.7},
                                                    {0.8, 0.3, -0.3, 0.8, 50, 50}, {1, 0, 0, -2, 10, 650}, {0, 0, 0, 0, 0, 0}};
        const base::AffineTransform& transform = transforms[rnd(7)];
        const double x = rndf(0, 60), y = rndf(0, 60);
        drawBoth("text pixels", [&](reef_ui_gc* gc) { orig<void>(68, gc, a, x, y); }, [&](reef_ui_gc* gc) { ours::gcDrawText(gc, b, x, y); },
                 &transform, rnd(4) ? 1 : 0.5);
        orig<void>(72, a);
        ours::textDestroy(b);
    }
    orig<void>(72, (void*)nullptr);
    ours::textDestroy(nullptr);
}

static const char* features[] = {nullptr, "", "kern=0", "liga=0", "ss01", "smcp", "tnum,lnum", "kern=0,liga=0,calt=0", "bogus=,x", "frac", "c2sc,smcp", "zero=1", "dlig"};

struct Input
{
    std::string text;
    std::vector<reef_ui_text_run> runs;
    reef_ui_paragraph_style style;
    double tabs[3];
};

// The styles of `kind`: 0 one run, 1 the same font in several runs, 2 several fonts, 3 with features, 4 anything.
static void randomInput(Input& in, int kind)
{
    in.text = rnd(3) ? randomText() : texts[rnd(textCount)];
    in.runs.clear();
    const size_t length = in.text.size();
    const unsigned count = kind == 0 ? rnd(2) : 1 + rnd(kind == 4 ? 9 : 5);
    size_t position = 0;
    const char* family = families[rnd(16)];
    const double size = sizes[rnd(15)];
    for (unsigned i = 0; i < count; i++) {
        reef_ui_text_run run;
        memset(&run, 0, sizeof run);
        if (kind == 4 && rnd(3) == 0) {
            run.start = rnd(uint32_t(length + 10));
            run.length = rnd(uint32_t(length + 10));
        } else {
            run.start = position;
            run.length = i + 1 == count ? length - position : rnd(uint32_t(length - position + 1));
            position += run.length;
        }
        // On a character, mostly.
        if (kind != 4 || rnd(4)) {
            while (position < length && (in.text[position] & 0xc0) == 0x80) {
                position++;
                run.length++;
            }
        }
        const bool own = kind >= 2 && rnd(3);
        run.family = own ? families[rnd(16)] : family;
        run.size = own && rnd(2) ? sizes[rnd(15)] : size;
        run.bold = rnd(4) == 0;
        run.italic = rnd(4) == 0 ? 2 : 0;
        run.red = rndf(-0.1, 1.1);
        run.green = rndf(-0.1, 1.1);
        run.blue = rndf(-0.1, 1.1);
        run.alpha = rnd(3) ? 1 : rndf(-0.1, 1.1);
        run.underline = rnd(4) == 0 ? 3 : 0;
        run.kerned = rnd(4) == 0;
        run.features = kind >= 3 && rnd(2) ? features[rnd(13)] : rnd(10) ? nullptr : "";
        in.runs.push_back(run);
    }
    reef_ui_paragraph_style& s = in.style;
    memset(&s, 0, sizeof s);
    s.width = rnd(5) == 0 ? (rnd(2) ? 0 : rnd(2) ? 1e7 : -5) : rndf(10, 700);
    s.firstLineIndent = rnd(2) ? 0 : rndf(-20, 60);
    s.indent = rnd(2) ? 0 : rndf(-20, 60);
    s.tailIndent = rnd(2) ? 0 : rndf(-60, 300);
    s.tabCount = rnd(4);
    in.tabs[0] = rndf(-10, 60);
    in.tabs[1] = in.tabs[0] + rndf(-10, 60);
    in.tabs[2] = in.tabs[1] + 10;
    s.tabs = in.tabs;
    s.alignment = int(rnd(5)) - 1;
    s.tabInterval = rnd(3) ? rndf(-5, 80) : 0;
    s.wraps = rnd(4);
    s.lineSpacing = rnd(2) ? 0 : rndf(-5, 10);
    s.minimumLineHeight = rnd(3) ? 0 : rndf(0, 40);
    s.maximumLineHeight = rnd(3) ? 0 : rndf(1, 40);
    s.lineHeightMultiple = rnd(3) ? 0 : rndf(0.5, 2);
    s.scale = scales[rnd(7)];
    if (rnd(25) == 0) {
        double* values[] = {&s.width, &s.firstLineIndent, &s.indent, &s.tailIndent, &s.tabInterval, &s.lineSpacing, &s.minimumLineHeight,
                            &s.maximumLineHeight, &s.lineHeightMultiple, &s.scale, &in.tabs[0], &in.tabs[1]};
        static const double odd[] = {NAN, INFINITY, -INFINITY, 1e300, 1e-300, 999999.5, 1e6};
        *values[rnd(12)] = odd[rnd(7)];
    }
}

// How a call ends in a process of its own: 0, or the signal, which is SIGALRM or an abort without memory for a call that does not end.
static size_t ending(const std::function<void()>& call)
{
    fflush(stdout);
    const pid_t child = fork();
    if (!child) {
        rlimit limit = {3000000000u, 3000000000u};
        setrlimit(RLIMIT_AS, &limit);
        alarm(4);
        fclose(stderr);
        call();
        _exit(0);
    }
    int status;
    waitpid(child, &status, 0);
    return WIFSIGNALED(status) ? 1 : 0;
}
static size_t endless;

static bool oddStyle(const reef_ui_paragraph_style& s)
{
    for (double v : {s.width, s.firstLineIndent, s.indent, s.tailIndent, s.tabInterval, s.lineSpacing, s.minimumLineHeight, s.maximumLineHeight,
                     s.lineHeightMultiple, s.scale, s.tabs[0], s.tabs[1]}) {
        if (!(std::abs(v) < 2000) || (v != 0 && std::abs(v) < 1e-100))
            return true;
    }
    return false;
}

static void testParagraph(int round)
{
    Input in;
    const int kind = rnd(5);
    randomInput(in, kind);
    const char* text = round == 3 ? nullptr : in.text.c_str();
    context = "paragraph " + std::to_string(round) + " kind " + std::to_string(kind) + " '" + in.text.substr(0, 200) + "' runs " + std::to_string(in.runs.size())
            + " width " + std::to_string(in.style.width) + " wraps " + std::to_string(in.style.wraps);
    if (getenv("VERBOSE")) {
        printf("%s\n", context.c_str());
        fflush(stdout);
    }
    // Where runs do not cover the text, the layout does not end.
    if (kind == 4 || oddStyle(in.style)) {
        const size_t first = ending([&] {
            orig<reef_ui_paragraph*>(90, text, (const reef_ui_text_run*)in.runs.data(), unsigned(in.runs.size()), (const reef_ui_paragraph_style*)&in.style);
        });
        if (first) {
            endless++;
            check("the end", first, ending([&] { ours::paragraphCreate(text, in.runs.data(), unsigned(in.runs.size()), &in.style); }));
            return;
        }
    }
    reef_ui_paragraph* a = orig<reef_ui_paragraph*>(90, text, (const reef_ui_text_run*)in.runs.data(), unsigned(in.runs.size()), (const reef_ui_paragraph_style*)&in.style);
    reef_ui_paragraph* b = ours::paragraphCreate(text, in.runs.data(), unsigned(in.runs.size()), &in.style);
    const unsigned lines = orig<unsigned>(92, a);
    check("lines", size_t(lines), size_t(ours::paragraphLineCount(b)));
    for (unsigned i = 0; i < lines + 2; i++) {
        reef_ui_paragraph_line m, n;
        memset(&m, 0x55, sizeof m);
        memset(&n, 0x55, sizeof n);
        orig<void>(93, a, i, &m);
        ours::paragraphLine(b, i, &n);
        check("line start", m.start, n.start);
        check("line length", m.length, n.length);
        check("line x", m.x, n.x);
        check("line y", m.y, n.y);
        check("line height", m.height, n.height);
        check("line baseline", m.baseline, n.baseline);
        check("line width", m.width, n.width);
        const size_t step = in.text.size() > 400 ? 1 + rnd(5) : 1;
        for (size_t offset = 0; offset <= in.text.size() + 3; offset += step)
            check("line offset to x", orig<double>(94, a, i, offset), ours::paragraphOffsetToX(b, i, offset));
        for (double x = -30; x < 760; x += rndf(3, 12))
            check("line x to offset", orig<size_t>(95, a, i, x), ours::paragraphXToOffset(b, i, x));
    }
    check("offset to x beyond", orig<double>(94, a, ~0u, size_t(0)), ours::paragraphOffsetToX(b, ~0u, 0));
    check("x to offset beyond", orig<size_t>(95, a, ~0u, 1.0), ours::paragraphXToOffset(b, ~0u, 1));
    const base::AffineTransform transforms[] = {{1, 0, 0, 1, 0, 0}, {2, 0, 0, 2, 3, 4}, {1, 0, 0, -1, 0, 650}, {1.5, 0, 0, 1.5, 0.3, 0.7},
                                                {0.8, 0.3, -0.3, 0.8, 50, 50}, {1, 0, 0, -2, 10, 650}, {-1, 0, 0, 1, 800, 0}, {1, 0.2, 0, 1, 0, 0}};
    const base::AffineTransform& transform = transforms[rnd(8)];
    const double x = rndf(0, 60), y = rndf(0, 60);
    drawBoth("paragraph pixels", [&](reef_ui_gc* gc) { orig<void>(96, gc, a, x, y); }, [&](reef_ui_gc* gc) { ours::gcDrawParagraph(gc, b, x, y); },
             &transform, rnd(4) ? 1 : 0.5);
    orig<void>(91, a);
    ours::paragraphDestroy(b);
}

struct Pdf
{
    std::string bytes;
    reef_ui_gc* gc = nullptr;
    reef_ui_pdf_document document = {};
    explicit Pdf(int archival)
    {
        document.write = [](void* context, const void* data, size_t size) { static_cast<Pdf*>(context)->bytes.append(static_cast<const char*>(data), size); };
        document.context = this;
        document.date = 1700000000;
        document.identifier = "0123456789abcdef";
        document.archival = archival;
        gc = orig<reef_ui_gc*>(168, (const reef_ui_pdf_document*)&document);
        const double box[4] = {0, 0, 600, 800}, boxes[16] = {};
        orig<void>(169, gc, (const double*)box, (const double*)boxes);
    }
    void finish()
    {
        orig<void>(170, gc);
        orig<void>(177, gc);
        orig<void>(178, gc);
        gc = nullptr;
    }
};

static void testPdf(int round)
{
    Input in;
    do
        randomInput(in, rnd(4));
    while (oddStyle(in.style));
    const double flip[6] = {1, 0, 0, -1, 0, 800}, other[6] = {1.2, 0.1, -0.1, 1.2, 20, 30}, still[6] = {1, 0, 0, 1, 0, 0};
    const double* matrix = round % 3 == 0 ? flip : round % 3 == 1 ? other : still;
    Pdf a(round % 7 == 0), b(round % 7 == 0);
    context = "pdf " + std::to_string(round) + " '" + in.text.substr(0, 200) + "'";
    const std::string line = randomText().substr(0, 80);
    const char* family = families[rnd(16)];
    const double size = sizes[rnd(15)];
    const int bold = rnd(2), italic = rnd(2);
    double x = rndf(0, 100), y = rndf(0, 100);
    for (int which = 0; which < 2; which++) {
        Pdf& pdf = which ? b : a;
        if (round % 3 != 2)
            orig<void>(35, pdf.gc, matrix);
        if (round % 5 == 0)
            orig<void>(54, pdf.gc, 3.0, 4.0, 2.0, 0.1, 0.2, 0.3, 0.5);
        if (which) {
            reef_ui_text* t = ours::textCreate(line.c_str(), family, size, bold, italic, 1, 0.1, 0.5, 0.9, 1, 0);
            ours::gcDrawText(pdf.gc, t, x, y);
            ours::textDestroy(t);
            reef_ui_paragraph* p = ours::paragraphCreate(in.text.c_str(), in.runs.data(), unsigned(in.runs.size()), &in.style);
            ours::gcDrawParagraph(pdf.gc, p, x + 10, y + 50);
            ours::paragraphDestroy(p);
        } else {
            reef_ui_text* t = orig<reef_ui_text*>(71, line.c_str(), family, size, bold, italic, 1.0, 0.1, 0.5, 0.9, 1.0, 0);
            orig<void>(68, pdf.gc, t, x, y);
            orig<void>(72, t);
            reef_ui_paragraph* p = orig<reef_ui_paragraph*>(90, in.text.c_str(), (const reef_ui_text_run*)in.runs.data(), unsigned(in.runs.size()),
                                                             (const reef_ui_paragraph_style*)&in.style);
            orig<void>(96, pdf.gc, p, x + 10, y + 50);
            orig<void>(91, p);
        }
        double m[6] = {};
        orig<void>(37, pdf.gc, m);
        pdf.bytes += "matrix";
        for (double v : m)
            pdf.bytes += " " + std::to_string(v);
    }
    // Only now, for the faces to be the same ones: their order in the document is that of their addresses.
    a.finish();
    b.finish();
    check("pdf", a.bytes, b.bytes);
    if (a.bytes != b.bytes && getenv("DUMP")) {
        for (int which = 0; which < 2; which++) {
            FILE* f = fopen((std::string("/tmp/text/pdf") + char('a' + which)).c_str(), "w");
            fwrite((which ? b : a).bytes.data(), 1, (which ? b : a).bytes.size(), f);
            fclose(f);
        }
        exit(1);
    }
    static size_t total;
    total += a.bytes.size();
    if (round == 0)
        printf("first pdf: %zu bytes\n", a.bytes.size());
}

static void testGlyphs()
{
    for (int round = 0; round < 200; round++) {
        reef_ui_face_query query = {};
        query.family = families[2 + rnd(14)];
        query.weight = rnd(2) ? 400 : 700;
        query.width = 5;
        reef_ui_face* face = orig<reef_ui_face*>(121, (const reef_ui_face_query*)&query, (int*)nullptr);
        const std::string string = "Glyphs 0123 fox";
        std::vector<uint32_t> characters(string.begin(), string.end()), glyphs(characters.size());
        orig<void>(129, face, (const uint32_t*)characters.data(), uint32_t(characters.size()), glyphs.data());
        std::vector<double> positions;
        double x = rndf(0, 20), size = rndf(6, 60);
        for (size_t i = 0; i < glyphs.size(); i++) {
            positions.push_back(x);
            positions.push_back(80 + rndf(-5, 5));
            x += size * 0.6;
        }
        const double matrix[4] = {size, 0, rnd(3) ? 0 : 3, rnd(2) ? -size : size};
        const int flags = rnd(4);
        const uint32_t count = rnd(8) ? uint32_t(glyphs.size()) : 0;
        context = "glyphs";
        drawBoth("glyphs", [&](reef_ui_gc* gc) { orig<void>(147, gc, face, matrix, (const uint32_t*)glyphs.data(), (const double*)positions.data(), count, flags); },
                 [&](reef_ui_gc* gc) { ours::gcDrawGlyphs(gc, face, matrix, glyphs.data(), positions.data(), count, flags); });
        if (round % 10 == 0) {
            Pdf a(0), b(0);
            orig<void>(147, a.gc, face, matrix, (const uint32_t*)glyphs.data(), (const double*)positions.data(), count, flags);
            ours::gcDrawGlyphs(b.gc, face, matrix, glyphs.data(), positions.data(), count, flags);
            a.finish();
            b.finish();
            check("glyphs pdf", a.bytes, b.bytes);
        }
        orig<void>(126, face);
    }
}

int main(int argc, char** argv)
{
    void* library = dlopen(argv[1], RTLD_NOW);
    if (!library) {
        puts(dlerror());
        return 1;
    }
    table = reinterpret_cast<void** (*)(int)>(dlsym(library, "reef_appkit_host_interface"))(0x18);
    link_map* map;
    dlinfo(library, RTLD_DI_LINKMAP, &map);
    originalBase = map->l_addr;
    const char* arguments[] = {"driver", "-ReefHeadless", "YES", nullptr};
    void* callbacks[64] = {};
    printf("start %d\n", orig<int>(0, 3, (char**)arguments, "org.example.driver", (void*)callbacks));
    const int rounds = argc > 2 ? atoi(argv[2]) : 600;
    if (argc > 3)
        seed = uint32_t(atoi(argv[3]));
    const std::string only = argc > 4 ? argv[4] : "";
    const auto report = [](const char* what) { printf("%s: %lu checks, %lu different, %zu pictures with ink, %zu that do not end\n", what, checks, failures, inked, endless); fflush(stdout); };
    const char* themes[] = {nullptr, "DejaVu Sans", "Liberation Sans", "Menlo", "monospace", "No Such", ""};
    for (int theme = getenv("THEMES") ? atoi(getenv("THEMES")) : 0; theme < (getenv("THEMES") ? 7 : 3); theme++) {
        if (theme) {
            ui::Theme changed = ui::Theme::current();
            changed.fontFamily = themes[theme];
            ui::Theme::setCurrent(changed);
        }
        for (const char* name : {"monospace", "Menlo", "Monaco", "Courier"})
            printf("%s: %s  ", name, ours::fixedFamily(name) ? "stood in for" : "installed");
        puts("");
        if (only.empty() || only == "FontMetrics")
            testFontMetrics();
        report("font metrics");
        if (only.empty() || only == "Families")
            testFamilies();
        report("families");
        if (only.empty() || only == "Text")
            testText();
        report("texts");
        for (int i = 0; i < rounds && (only.empty() || only == "Paragraph"); i++)
            testParagraph(i);
        report("paragraphs");
        for (int i = 0; i < rounds / 4 && (only.empty() || only == "Pdf"); i++)
            testPdf(i);
        report("documents");
        if (only.empty() || only == "Glyphs")
            testGlyphs();
        report("glyphs");
    }
    return failures != 0;
}
