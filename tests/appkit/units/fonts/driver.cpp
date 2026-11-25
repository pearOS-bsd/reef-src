// driver LIBRARY GROUP... : calls the fonts functions of a host library and prints all results as text.
#include "fonts.h"
#include "paths.h"
#include "ui/bitmap.h"
#include "ui/canvas_2d.h"
#include "ui/theme.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <filesystem>
#include <fontconfig/fontconfig.h>
#include <fstream>
#include <functional>
#include <link.h>
#include <set>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using namespace cutefish;
using Face = reef_ui_face;
struct Table { uint64_t version; void* functions[317]; };
static Table* table;
static void* extras[8];
template<typename R, typename... A> static R call(int index, A... args) { return reinterpret_cast<R (*)(A...)>(table->functions[index])(args...); }
static Face* match(const reef_ui_face_query* q, int* e) { return call<Face*>(121, q, e); }
static int list(const reef_ui_face_query* q, char* b, int s) { return call<int>(122, q, b, s); }
static Face* fromFile(const char* p, int i) { return call<Face*>(123, p, i); }
static Face* fromData(const void* p, size_t s, int i) { return call<Face*>(124, p, s, i); }
static void retain(Face* f) { call<void>(125, f); }
static void release(Face* f) { call<void>(126, f); }
static void metrics(Face* f, reef_ui_face_metrics* m) { call<void>(127, f, m); }
static int name(Face* f, int id, char* b, int s) { return call<int>(128, f, id, b, s); }
static void glyphs(Face* f, const uint32_t* c, uint32_t n, uint32_t* g) { call<void>(129, f, c, n, g); }
static void advances(Face* f, const uint32_t* g, uint32_t n, int v, double* a) { call<void>(130, f, g, n, v, a); }
static void bounds(Face* f, const uint32_t* g, uint32_t n, double* b) { call<void>(131, f, g, n, b); }
static uint32_t outline(Face* f, uint32_t g, reef_ui_path_element* e, uint32_t c) { return call<uint32_t>(132, f, g, e, c); }
static int glyphName(Face* f, uint32_t g, char* b, int s) { return call<int>(133, f, g, b, s); }
static uint32_t glyphNamed(Face* f, const char* n) { return call<uint32_t>(134, f, n); }
static size_t tableData(Face* f, uint32_t t, void* b, size_t s) { return call<size_t>(135, f, t, b, s); }
static uint32_t tables(Face* f, uint32_t* t, uint32_t c) { return call<uint32_t>(136, f, t, c); }
static uint32_t ranges(Face* f, uint32_t* r, uint32_t c) { return call<uint32_t>(137, f, r, c); }
static int languages(Face* f, char* b, int s) { return call<int>(138, f, b, s); }
static Face* fallback(Face* f, uint32_t c, const char* l) { return call<Face*>(139, f, c, l); }
static uint32_t shape(Face* f, const uint16_t* t, uint32_t len, uint32_t start, uint32_t n, double size, int flags, uint32_t script, const char* lang,
                      const reef_ui_feature* fe, uint32_t nf, reef_ui_glyph* g, uint32_t cap)
{
    return call<uint32_t>(140, f, t, len, start, n, size, flags, script, lang, fe, nf, g, cap);
}
static uint32_t axes(Face* f, reef_ui_face_axis* a, uint32_t c) { return call<uint32_t>(141, f, a, c); }
static uint32_t coordinates(Face* f, uint32_t* t, double* v, uint32_t c) { return call<uint32_t>(142, f, t, v, c); }
static Face* vary(Face* f, const uint32_t* t, const double* v, uint32_t n) { return call<Face*>(143, f, t, v, n); }
static int registerFont(const char* p) { return call<int>(144, p); }
static int unregisterFont(const char* p) { return call<int>(145, p); }
static double obliqueSlant() { return reinterpret_cast<double (*)()>(extras[0])(); }
static double stretch(Face* f) { return reinterpret_cast<double (*)(Face*)>(extras[1])(f); }
static double boldStrength() { return reinterpret_cast<double (*)()>(extras[2])(); }
static int outlineFormat(Face* f) { return reinterpret_cast<int (*)(Face*)>(extras[3])(f); }
static std::map<uint32_t, uint32_t> characters(Face* f) { return reinterpret_cast<std::map<uint32_t, uint32_t> (*)(Face*)>(extras[4])(f); }
static std::vector<reef_ui_feature> features(const char* t) { return reinterpret_cast<std::vector<reef_ui_feature> (*)(const char*)>(extras[5])(t); }
static std::string subset(Face* f, const std::vector<uint32_t>& g, int* k) { return reinterpret_cast<std::string (*)(Face*, const std::vector<uint32_t>&, int*)>(extras[6])(f, g, k); }
static void draw(ui::Canvas2D& c, Face* f, const double* m, const uint32_t* g, const double* p, uint32_t n, int flags)
{
    reinterpret_cast<void (*)(ui::Canvas2D&, Face*, const double*, const uint32_t*, const double*, uint32_t, int)>(extras[7])(c, f, m, g, p, n, flags);
}

static uint64_t hash(const void* data, size_t size, uint64_t h = 0xcbf29ce484222325)
{
    for (size_t i = 0; i < size; i++) h = (h ^ static_cast<const uint8_t*>(data)[i]) * 0x100000001b3;
    return h;
}
static std::string quoted(const std::string& s)
{
    std::string r = "\"";
    for (unsigned char c : s) { if (c == '\n') r += "\\n"; else if (c == '\t') r += "\\t"; else if (c < 32 || c == '"' || c == 127) { char b[8]; snprintf(b, 8, "\\x%02x", c); r += b; } else r += char(c); }
    return r + "\"";
}
static std::string nameOf(Face* f, int id)
{
    char b[4096];
    memset(b, 0x55, sizeof b);
    const int n = name(f, id, b, sizeof b);
    if (n < 0) return "none:" + std::to_string(n);
    return std::to_string(n) + quoted(std::string(b, std::min<size_t>(n, sizeof b - 1)));
}
// The public part of the face's memory, the same in both libraries.
static void describe(Face* f)
{
    if (!f) { printf("  face: null\n"); return; }
    const char* p = reinterpret_cast<const char*>(f);
    const auto& path = *reinterpret_cast<const std::string*>(p + 8);
    const auto& family = *reinterpret_cast<const std::string*>(p + 0x50);
    const auto& style = *reinterpret_cast<const std::string*>(p + 0x70);
    const auto& variations = *reinterpret_cast<const std::vector<std::pair<uint32_t, double>>*>(p + 0x30);
    printf("  face: refs=%d %s index=%#x bold=%d oblique=%d stretch=%a family=%s style=%s data=%d var=", *reinterpret_cast<const int*>(p), quoted(path).c_str(),
           *reinterpret_cast<const int*>(p + 0x28), p[0x2c], p[0x2d], *reinterpret_cast<const double*>(p + 0x48), quoted(family).c_str(), quoted(style).c_str(),
           *reinterpret_cast<void* const*>(p + 0x90) != nullptr);
    for (const auto& [t, v] : variations) printf("%#x:%a,", t, v);
    printf("\n");
}
static void printMetrics(Face* f)
{
    reef_ui_face_metrics m;
    memset(&m, 0x77, sizeof m);
    metrics(f, &m);
    const double* d = reinterpret_cast<const double*>(&m);
    printf("  metrics:");
    for (int i = 0; i < 15; i++) printf(" %a", d[i]);
    printf(" glyphs=%u flags=%#x stretch=%a format=%d\n", m.glyphCount, m.flags, stretch(f), outlineFormat(f));
}
static void printNames(Face* f)
{
    printf("  names:");
    for (int id = -2; id < 26; id++) printf(" [%d]%s", id, nameOf(f, id).c_str());
    for (int id : {256, 257, 258, 65536 + 1, 65536 + 4}) printf(" [%d]%s", id, nameOf(f, id).c_str());
    printf("\n");
    char b[8];
    for (int size : {0, 1, 2, 5, 8}) { memset(b, 0x55, 8); const int n = name(f, 4, b, size); printf("  short name %d: %d %016llx\n", size, n, *(unsigned long long*)b); }
    printf("  no buffer: %d\n", name(f, 1, nullptr, 100));
}
static std::vector<std::string> fontFiles()
{
    std::vector<std::string> files;
    const std::string roots = getenv("FONT_ROOTS") ? getenv("FONT_ROOTS") : "/tk/app/fonts/fontset:/usr/share/fonts/truetype/dejavu";
    std::istringstream in(roots);
    for (std::string root; std::getline(in, root, ':');)
        for (const auto& e : std::filesystem::recursive_directory_iterator(root))
            if (e.is_regular_file()) files.push_back(e.path());
    std::sort(files.begin(), files.end());
    return files;
}
static void glyphTests(Face* f, bool deep)
{
    std::vector<uint32_t> chars;
    for (uint32_t c = 0; c < 0x250; c++) chars.push_back(c);
    for (uint32_t c : {0x391u, 0x410u, 0x5d0u, 0x627u, 0x905u, 0xe01u, 0x1100u, 0x1820u, 0x2013u, 0x2022u, 0x20acu, 0x2202u, 0x25cau, 0x2603u, 0x3042u, 0x4e00u, 0xac00u, 0xf8ffu, 0xfb01u, 0xfffdu, 0x1f600u, 0x1f468u, 0x1d400u, 0x20000u, 0x110000u, 0xffffffffu}) chars.push_back(c);
    std::vector<uint32_t> g(chars.size() + 2, 0xdeadbeef);
    glyphs(f, chars.data(), chars.size(), g.data());
    printf("  glyphs: %016llx tail %x\n", (unsigned long long)hash(g.data(), g.size() * 4), g.back());
    g.resize(chars.size());
    std::vector<uint32_t> ids(g);
    for (uint32_t i = 0; i < 300; i++) ids.push_back(i);
    ids.push_back(70000); ids.push_back(0xffffffff);
    std::vector<double> a(ids.size()), v(ids.size()), b(ids.size() * 4);
    advances(f, ids.data(), ids.size(), 0, a.data());
    advances(f, ids.data(), ids.size(), 1, v.data());
    bounds(f, ids.data(), ids.size(), b.data());
    printf("  advances: %016llx vertical: %016llx bounds: %016llx\n", (unsigned long long)hash(a.data(), a.size() * 8), (unsigned long long)hash(v.data(), v.size() * 8), (unsigned long long)hash(b.data(), b.size() * 8));
    printf("  sample:");
    for (int i = 'A'; i < 'F'; i++) printf(" %u:%a,%a(%a %a %a %a)", ids[i], a[i], v[i], b[i * 4], b[i * 4 + 1], b[i * 4 + 2], b[i * 4 + 3]);
    printf("\n");
    uint64_t h = 0xcbf29ce484222325; uint64_t total = 0;
    std::vector<reef_ui_path_element> e(4096);
    const size_t step = deep ? 1 : 7;
    for (size_t i = 0; i < ids.size(); i += step) {
        memset(e.data(), 0x33, e.size() * sizeof e[0]);
        const uint32_t n = outline(f, ids[i], e.data(), e.size());
        total += n;
        h = hash(e.data(), std::min<size_t>(n + 1, e.size()) * sizeof e[0], h);
    }
    printf("  outlines: %llu %016llx\n", (unsigned long long)total, (unsigned long long)h);
    memset(e.data(), 0x33, e.size() * sizeof e[0]);
    printf("  outline counts: %u %u %u %016llx\n", outline(f, ids['B'], nullptr, 100), outline(f, ids['B'], e.data(), 3), outline(f, ids['B'], e.data(), 0), (unsigned long long)hash(e.data(), 10 * sizeof e[0]));
    printf("  glyph names:");
    for (uint32_t i = 0; i < ids.size(); i += deep ? 3 : 41) {
        char n[300]; memset(n, 0x55, sizeof n);
        const int l = glyphName(f, ids[i], n, sizeof n);
        printf(" %d%s", l, l >= 0 ? quoted(n).c_str() : "");
        if (l > 0) printf("=%u", glyphNamed(f, n));
    }
    char tiny[4]; memset(tiny, 0x55, 4);
    const int tn = glyphName(f, ids['A'], tiny, 2);
    printf(" tiny %d %08x null %d\n", tn, *(unsigned*)tiny, glyphName(f, ids['A'], nullptr, 0));
    printf("  named: %u %u %u %u %u\n", glyphNamed(f, "A"), glyphNamed(f, "uni0041"), glyphNamed(f, "gid5"), glyphNamed(f, "nonsense"), glyphNamed(f, ""));
}
static void tableTests(Face* f)
{
    std::vector<uint32_t> t(64, 0x11111111);
    const uint32_t n = tables(f, t.data(), 64);
    printf("  tables: %u (%u %u)", n, tables(f, nullptr, 64), tables(f, t.data() + 60, 2));
    for (uint32_t i = 0; i < std::min(n, 64u); i++) {
        const size_t s = tableData(f, t[i], nullptr, 0);
        std::vector<char> d(s == size_t(-1) ? 0 : s + 8, 0x44);
        const size_t r = tableData(f, t[i], d.data(), s);
        printf(" %c%c%c%c:%zd,%zd,%016llx", t[i] >> 24, t[i] >> 16 & 255, t[i] >> 8 & 255, t[i] & 255, s, r, (unsigned long long)hash(d.data(), d.size()));
        if (s != size_t(-1) && s > 0) { std::vector<char> small(s, 0x44); printf("(%zd,%016llx)", tableData(f, t[i], small.data(), s - 1), (unsigned long long)hash(small.data(), s)); }
    }
    printf(" none:%zd\n", tableData(f, 0x41424344, nullptr, 0));
    std::vector<uint32_t> r(4000, 0x22222222);
    const uint32_t nr = ranges(f, r.data(), 2000);
    printf("  ranges: %u %016llx, %u, %u\n", nr, (unsigned long long)hash(r.data(), r.size() * 4), ranges(f, nullptr, 3), ranges(f, r.data(), 2));
    char l[8192]; memset(l, 0x55, sizeof l);
    const int nl = languages(f, l, sizeof l);
    printf("  languages: %d %s", nl, quoted(std::string(l, std::min<size_t>(strlen(l), 2000))).c_str());
    memset(l, 0x55, 8);
    printf(" short %d %016llx none %d\n", languages(f, l, 6), *(unsigned long long*)l, languages(f, nullptr, 6));
    reef_ui_face_axis ax[16];
    memset(ax, 0, sizeof ax);
    const uint32_t na = axes(f, ax, 16);
    printf("  axes: %u (%u %u)", na, axes(f, nullptr, 16), axes(f, ax, 0));
    for (uint32_t i = 0; i < std::min(na, 16u); i++) printf(" [%#x hidden=%d %a %a %a %s]", ax[i].tag, ax[i].hidden, ax[i].minimum, ax[i].standard, ax[i].maximum, ax[i].name ? quoted(ax[i].name).c_str() : "null");
    uint32_t ct[16] = {}; double cv[16] = {};
    const uint32_t nc = coordinates(f, ct, cv, 16);
    printf(" at: %u (%u %u %u)", nc, coordinates(f, nullptr, cv + 8, 8), coordinates(f, ct + 8, nullptr, 8), coordinates(f, ct, cv, 0));
    for (int i = 0; i < 16; i++) printf(" %#x:%a", ct[i], cv[i]);
    printf("\n");
    const auto map = characters(f);
    uint64_t h = 0xcbf29ce484222325;
    for (const auto& [g, c] : map) { const uint32_t p[2] = {g, c}; h = hash(p, 8, h); }
    printf("  characters: %zu %016llx\n", map.size(), (unsigned long long)h);
    int kind = 77;
    std::vector<uint32_t> keep;
    for (uint32_t g = 3; g < 90; g += 2) keep.push_back(g);
    const std::string s1 = subset(f, keep, &kind);
    printf("  subset: kind %d size %zu %016llx", kind, s1.size(), (unsigned long long)hash(s1.data(), s1.size()));
    kind = 78;
    const std::string s2 = subset(f, {}, &kind);
    printf(" empty: kind %d size %zu %016llx\n", kind, s2.size(), (unsigned long long)hash(s2.data(), s2.size()));
}
static void groupFiles(bool deep)
{
    for (const std::string& file : fontFiles()) {
        std::set<int> indices = {0, 1, 2, 5};
        if (deep) for (int i : {3, 4, 6, 9, 10}) indices.insert(i);
        if (file.find("/var/") != std::string::npos) for (int i = 1; i < (deep ? 40 : 12); i++) indices.insert(i << 16);
        indices.insert(0x7fff); indices.insert(-1); indices.insert(0x10001); indices.insert(-65536);
        for (int index : indices) {
            Face* f = fromFile(file.c_str(), index);
            printf("file %s %#x\n", file.c_str(), index);
            describe(f);
            if (!f) continue;
            printMetrics(f); printNames(f); glyphTests(f, deep); tableTests(f);
            Face* again = fromFile(file.c_str(), index);
            printf("  same: %d\n", f == again);
            release(again);
            describe(f);
            release(f);
        }
    }
    printf("nulls: %p %p %p\n", (void*)fromFile(nullptr, 0), (void*)fromFile("", 0), (void*)fromFile("/nonexistent", 0));
}
static void groupData()
{
    for (const std::string& file : fontFiles()) {
        std::ifstream in(file, std::ios::binary); std::stringstream ss; ss << in.rdbuf(); std::string d = ss.str();
        for (int index : {0, 1, 0x20000}) {
            Face* f = fromData(d.data(), d.size(), index);
            printf("data %s %#x\n", file.c_str(), index);
            describe(f);
            if (!f) continue;
            printMetrics(f); printNames(f); glyphTests(f, false); tableTests(f);
            Face* other = fromData(d.data(), d.size(), index);
            printf("  same: %d\n", f == other);
            release(other); release(f);
        }
        // Broken ones.
        for (size_t cut : {size_t(0), size_t(1), size_t(12), d.size() / 3, d.size() - 1}) {
            Face* f = fromData(d.data(), cut, 0);
            printf("cut %s %zu\n", file.c_str(), cut);
            describe(f);
            if (f) { printMetrics(f); printNames(f); release(f); }
        }
    }
    printf("nulls: %p\n", (void*)fromData(nullptr, 10, 0));
}
static std::vector<reef_ui_face_query> queries()
{
    std::vector<reef_ui_face_query> q;
    const reef_ui_face_query none = {nullptr, nullptr, nullptr, 0, 0, -1, -1, nullptr};
    static const char* families[] = {nullptr, "", "Lato", "lato", "LATO", "DejaVu Sans", "DejaVu Serif", "sans-serif", "serif", "monospace", "Times", "Times New Roman", "Times Roman", "Arial", "arial", "Courier New", "Arial Narrow", "Georgia", "Calibri", "Cambria", "Palatino", "Bookman", "New Century Schoolbook", "Helvetica", "Ubuntu", "Ubuntu Sans", "Mutern VF", "Noto Color Emoji", "Noto Sans CJK JP", "Noto Sans Arabic", "Nonexistent Family", "Avenir Next Condensed", "AvenirNextCondensed", "avenir next condensed", "Avenir Next", "Standard Symbols PS", "fixed", "WenQuanYi Zen Hei", "Liberation Sans", "P052", "Unifont Sample", "Nimbus Sans", "C059", "system-ui", "emoji"};
    for (const char* family : families) {
        auto base = none; base.family = family; q.push_back(base);
        for (double w : {100.0, 250.0, 300.0, 400.0, 500.0, 550.0, 599.0, 600.0, 650.0, 700.0, 800.0, 900.0, 1000.0, -5.0}) { auto x = base; x.weight = w; q.push_back(x); }
        for (double w : {0.2, 1.0, 2.0, 3.4, 3.6, 5.0, 7.0, 9.0, 12.0, -3.0}) { auto x = base; x.width = w; q.push_back(x); }
        for (int i : {0, 1, 2}) { auto x = base; x.italic = i; q.push_back(x); x.weight = 700; q.push_back(x); }
        for (int i : {0, 1}) { auto x = base; x.fixedPitch = i; q.push_back(x); }
        for (const char* l : {"", "ja", "zh-cn", "ar", "de", "xx"}) { auto x = base; x.language = l; q.push_back(x); }
        for (const char* s : {"", "Bold", "Italic", "Bold Italic", "Light", "Demi Bold", "DemiBold", "demi bold", "Ultra Light", "UltraLight", "Heavy", "Medium", "Regular", "Nonsense", "Condensed Bold"}) { auto x = base; x.style = s; q.push_back(x); x.weight = 620; x.italic = 1; q.push_back(x); }
    }
    for (const char* n : {"", "Lato-Regular", "Lato-Bold", "lato-bold", "Lato Bold", "DejaVuSans", "DejaVuSans-Bold", "DejaVu Sans Bold", "LiberationSans", "Liberation Sans", "Ubuntu-Regular", "UbuntuSans-Bold", "Ubuntu Sans Bold", "MuternVF", "NotoColorEmoji", "NotoSansCJKjp-Regular", "Noto Sans CJK SC", "AvenirNextCondensed-Regular", "AvenirNextCondensed-Bold", "AvenirNextCondensed-DemiBold", "AvenirNextCondensed-Heavy", "AvenirNextCondensed-UltraLight", "AvenirNextCondensed-Medium", "AvenirNextCondensed-Nonsense", "AvenirNextCondensed", "Avenir Next Condensed", "Avenir Next Condensed-Demi Bold", "avenirnextcondensed-heavy", "AvenirNextCondensed-", "-Bold", "AvenirNext-Bold", "Nonexistent-Name", "Symbola", "C059-Roman"}) {
        for (const char* family : {(const char*)nullptr, "Lato", "Avenir Next Condensed"}) {
            auto x = none; x.name = n; x.family = family; q.push_back(x);
            x.italic = 1; x.weight = 800; q.push_back(x);
            x.style = "Heavy"; q.push_back(x);
        }
    }
    for (double w : {0.0, 50.0, 200.0, 299.0, 300.0, 301.0, 449.0, 450.0, 451.0, 550.0, 650.0, 749.0, 750.0, 751.0, 2000.0}) { auto x = none; x.family = "Avenir Next Condensed"; x.weight = w; q.push_back(x); x.italic = 1; q.push_back(x); x.italic = 0; q.push_back(x); }
    return q;
}
static void printQuery(const reef_ui_face_query& q)
{
    const auto s = [](const char* t) { return t ? quoted(t) : std::string("-"); };
    printf("query name=%s family=%s style=%s weight=%g width=%g italic=%d fixed=%d lang=%s\n", s(q.name).c_str(), s(q.family).c_str(), s(q.style).c_str(), q.weight, q.width, q.italic, q.fixedPitch, s(q.language).c_str());
}
static void groupMatch()
{
    std::vector<Face*> kept;
    for (const auto& q : queries()) {
        printQuery(q);
        int exact = 55;
        Face* f = match(&q, &exact);
        printf("  exact=%d\n", exact);
        describe(f);
        if (f) { printMetrics(f); printf("  %s | %s | %s | %s\n", nameOf(f, 1).c_str(), nameOf(f, 2).c_str(), nameOf(f, 4).c_str(), nameOf(f, 6).c_str()); kept.push_back(f); }
        exact = 56;
        Face* again = match(&q, &exact);
        printf("  again exact=%d same=%d\n", exact, again == f);
        if (again) release(again);
        Face* third = match(&q, nullptr);
        if (third) release(third);
    }
    int exact = 57;
    printf("null: %p %d\n", (void*)match(nullptr, &exact), exact);
    for (Face* f : kept) release(f);
}
static void groupList()
{
    std::vector<char> b(1 << 20, 0x55);
    int n = list(nullptr, b.data(), b.size());
    printf("all: %d\n%s", n, b.data());
    memset(b.data(), 0x55, 64);
    printf("short: %d %s\n", list(nullptr, b.data(), 40), quoted(std::string(b.data(), 45)).c_str());
    printf("none: %d %d\n", list(nullptr, nullptr, 40), list(nullptr, b.data(), 0));
    int i = 0;
    for (const auto& q : queries()) {
        if (i++ > 2000) break;
        printQuery(q);
        memset(b.data(), 0x55, 16);
        n = list(&q, b.data(), b.size());
        printf("  %d %016llx\n", n, (unsigned long long)hash(b.data(), n));
        if (n < 3000 && n > 0) printf("%s", b.data());
        printf("  again %d\n", list(&q, b.data(), b.size()));
    }
}
static std::vector<uint16_t> utf16(const char* s)
{
    std::vector<uint16_t> r;
    while (*s) {
        unsigned char c = *s++; uint32_t u = c;
        int n = c < 0x80 ? 0 : c < 0xe0 ? 1 : c < 0xf0 ? 2 : 3;
        if (n) u = c & (0x3f >> n);
        while (n--) u = u << 6 | (*s++ & 0x3f);
        if (u >= 0x10000) { r.push_back(0xd800 + ((u - 0x10000) >> 10)); r.push_back(0xdc00 + (u & 0x3ff)); } else r.push_back(u);
    }
    return r;
}
static const char* texts[] = {"Hello, World! AVAWA To fi ffl 1/2 0123", "", "Ångström naïve café", "שלום עולם", "مرحبا بالعالم", "नमस्ते दुनिया क्षत्रिय", "สวัสดีชาวโลก", "こんにちは世界、縦書き「テスト」", "👨‍👩‍👧‍👦 😀 🇷🇴", "a\xcc\x81\xcc\xa7 \xcc\x88", "mixed עברית and العربية 中文", "தமிழ்", "ភាសាខ្មែរ", "ᠮᠣᠩᠭᠣᠯ", "\xe2\x80\x8b\xe2\x80\x8d\xef\xbf\xbd"};
static void groupShape()
{
    const char* files[] = {"/tk/app/fonts/fontset/lato/Lato-Regular.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", "/tk/app/fonts/fontset/noto/NotoSansArabic-Regular.ttf", "/tk/app/fonts/fontset/noto/NotoSansDevanagari-Regular.ttf", "/tk/app/fonts/fontset/noto/NotoSansThai-Regular.ttf", "/tk/app/fonts/fontset/noto/NotoSansHebrew-Regular.ttf", "/tk/app/fonts/fontset/cjk/NotoSansCJK-Regular.ttc", "/tk/app/fonts/fontset/color/NotoColorEmoji.ttf", "/tk/app/fonts/fontset/urw/P052-Roman.otf", "/tk/app/fonts/fontset/var/UbuntuSans[wdth,wght].ttf", "/tk/app/fonts/fontset/type1/C059-Roman.t1", "/tk/app/fonts/fontset/pcf/10x20.pcf.gz", "/tk/app/fonts/fontset/misc/FreeSerifBoldItalic.ttf", "/tk/app/fonts/fontset/noto/NotoSansMongolian-Regular.ttf", "/tk/app/fonts/fontset/noto/NotoSansKhmer-Regular.ttf"};
    const char* featureTexts[] = {"", "kern=0", "liga=0,-kern", "smcp,onum=1", "ss01[0:5]=2, tnum", "dlig,calt=0,frac", "vert"};
    for (const char* file : files) {
        for (int index : {0, 0x30000}) {
            Face* f = fromFile(file, index);
            printf("shape %s %#x\n", file, index);
            if (!f) { printf("  null\n"); continue; }
            for (const char* t : texts) {
                const auto u = utf16(t);
                for (const char* ft : featureTexts) {
                    const auto fe = features(ft);
                    for (int flags : {0, 1, 2, 3}) {
                        for (const char* lang : {(const char*)nullptr, "", "ro", "sr", "ja", "ar", "zh-Hant"}) {
                            for (uint32_t script : {0u, 0x4c61746eu, 0x41726162u}) {
                                std::vector<reef_ui_glyph> g(u.size() * 4 + 8);
                                memset(g.data(), 0x66, g.size() * sizeof g[0]);
                                const uint32_t n = shape(f, u.data(), u.size(), 0, u.size(), 13.5, flags, script, lang, fe.data(), fe.size(), g.data(), g.size());
                                printf("%u:%016llx ", n, (unsigned long long)hash(g.data(), g.size() * sizeof g[0]));
                            }
                        }
                    }
                }
                // Parts.
                for (uint32_t start : {0u, 1u, 5u, 100u}) for (uint32_t count : {0u, 1u, 3u, 1000u}) {
                    std::vector<reef_ui_glyph> g(u.size() * 4 + 8);
                    memset(g.data(), 0x66, g.size() * sizeof g[0]);
                    const uint32_t n = shape(f, u.data(), u.size(), start, count, 1000, 0, 0, nullptr, nullptr, 0, g.data(), 2);
                    printf("%u:%016llx ", n, (unsigned long long)hash(g.data(), g.size() * sizeof g[0]));
                }
                printf("\n");
            }
            printf("nulls %u %u\n", shape(nullptr, nullptr, 0, 0, 0, 1, 0, 0, nullptr, nullptr, 0, nullptr, 0), shape(f, nullptr, 0, 0, 0, 1, 0, 0, nullptr, nullptr, 0, nullptr, 0));
            release(f);
        }
    }
}
static void groupFeatures()
{
    for (const char* t : {(const char*)nullptr, "", ",", ",,", "liga", "liga=0", "-liga", "+liga", "liga,kern", "liga, kern=0 ,ss01=3", "ss01[3:5]=2", "aalt=2,", ",smcp", "toolongtag", "a", "kern[]", "kern[3:]", "kern[:5]", "kern[3]", "kern[3:5]", "kern on", "\"kern\" off", "'kern' 1", "x=y=z", "liga =1", " liga"}) {
        const auto f = features(t);
        printf("features %s: %zu", t ? quoted(t).c_str() : "-", f.size());
        for (const auto& x : f) printf(" %#x %u %u %u;", x.tag, x.value, x.start, x.end);
        printf("\n");
    }
    printf("constants %a %a %a %d\n", obliqueSlant(), boldStrength(), stretch(nullptr), outlineFormat(nullptr));
    int kind = 5;
    printf("null subset %zu %d, characters %zu\n", subset(nullptr, {1, 2}, &kind).size(), kind, characters(nullptr).size());
}
static void groupFallback()
{
    std::vector<uint32_t> chars;
    for (uint32_t c = 0x20; c < 0x30000; c += c < 0x3000 ? 0x2b : 0x3a7) chars.push_back(c);
    for (uint32_t c : {0x41u, 0x2603u, 0x1f600u, 0x4e00u, 0x3042u, 0x5d0u, 0x627u, 0xe000u, 0xf8ffu, 0x10ffffu}) chars.push_back(c);
    const char* files[] = {"/tk/app/fonts/fontset/lato/Lato-BoldItalic.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", "/tk/app/fonts/fontset/noto/NotoSansArabic-Regular.ttf", "/tk/app/fonts/fontset/type1/C059-Roman.t1"};
    for (const char* file : files) {
        Face* f = fromFile(file, 0);
        printf("fallback %s\n", file);
        for (const char* lang : {(const char*)nullptr, "", "ja", "zh-tw", "ar", "nonsense"}) {
            for (uint32_t c : chars) {
                Face* g = fallback(f, c, lang);
                printf("U+%04X %s ", c, lang ? lang : "-");
                describe(g);
                if (g) release(g);
                Face* h = fallback(f, c, lang);
                if (h != g) printf("  differs: %d\n", h == g);
                if (h) release(h);
            }
        }
        describe(f);
        release(f);
    }
    std::ifstream in(files[0], std::ios::binary); std::stringstream ss; ss << in.rdbuf(); const std::string d = ss.str();
    Face* f = fromData(d.data(), d.size(), 0);
    for (uint32_t c : {0x41u, 0x4e00u, 0x1f600u}) { Face* g = fallback(f, c, nullptr); describe(g); if (g) release(g); }
    release(f);
    printf("null %p\n", (void*)fallback(nullptr, 65, nullptr));
}
static void groupVary()
{
    for (const std::string& file : fontFiles()) {
        if (file.find("/var/") == std::string::npos && file.find("DejaVuSans.ttf") == std::string::npos) continue;
        for (int index : {0, 0x30000}) {
            Face* f = fromFile(file.c_str(), index);
            if (!f) continue;
            const uint32_t t[] = {0x77676874, 0x77647468, 0x6f70737a, 0x736c6e74, 0x41414141};
            const double sets[][5] = {{700, 100, 14, 0, 1}, {123.456, 62.5, 8, -10, 2}, {9999, 9999, 9999, 9999, 3}, {-1, -1, -1, -1, 4}};
            for (const auto& v : sets) for (uint32_t n : {0u, 1u, 2u, 5u}) {
                Face* g = vary(f, t, v, n);
                printf("vary %s %#x %g %u\n", file.c_str(), index, v[0], n);
                describe(g);
                if (!g) continue;
                printMetrics(g); printNames(g); glyphTests(g, false); tableTests(g);
                // The same in another order, twice.
                uint32_t t2[5]; double v2[5];
                for (uint32_t i = 0; i < n; i++) { t2[i] = t[n - 1 - i]; v2[i] = v[n - 1 - i]; }
                Face* h = vary(f, t2, v2, n);
                printf("  same %d\n", g == h);
                Face* k = vary(g, t, v, 1);
                describe(k);
                if (k) release(k);
                if (h) release(h);
                release(g);
            }
            const uint32_t twice[] = {0x77676874, 0x77676874}; const double tv[] = {300, 600};
            Face* g = vary(f, twice, tv, 2);
            describe(g);
            if (g) { uint32_t ct[4] = {}; double cv[4] = {}; coordinates(g, ct, cv, 4); printf("  twice %#x:%a\n", ct[0], cv[0]); release(g); }
            release(f);
        }
        std::ifstream in(file, std::ios::binary); std::stringstream ss; ss << in.rdbuf(); const std::string d = ss.str();
        Face* f = fromData(d.data(), d.size(), 0);
        if (!f) continue;
        const uint32_t t[] = {0x77676874}; const double v[] = {650};
        Face* g = vary(f, t, v, 1);
        describe(g);
        release(f);
        if (g) { printMetrics(g); glyphTests(g, false); release(g); }
    }
    printf("null %p\n", (void*)vary(nullptr, nullptr, nullptr, 0));
}
static void groupRegister()
{
    reef_ui_face_query q = {nullptr, "Lato", nullptr, 0, 0, -1, -1, nullptr};
    const auto show = [&](const char* what) {
        for (const char* family : {"Hack", "Tuffy", "Lato", "FreeSerif", "IPAPGothic"}) {
            q.family = family;
            int exact = 5;
            Face* f = match(&q, &exact);
            printf("%s %s exact=%d\n", what, family, exact);
            describe(f);
            if (f) release(f);
            char b[4000] = ""; const int n = list(&q, b, sizeof b);
            printf("  list %d %s\n", n, quoted(b).c_str());
        }
    };
    show("before");
    const char* a = getenv("APPFONT1"); const char* c = getenv("APPFONT2");
    printf("register %d %d %d %d %d\n", registerFont(nullptr), registerFont("/nonexistent.ttf"), registerFont(a), registerFont(a), registerFont(c));
    show("after");
    printf("unregister %d %d %d\n", unregisterFont(nullptr), unregisterFont("/nonexistent.ttf"), unregisterFont(a));
    show("one");
    printf("unregister %d %d\n", unregisterFont(a), unregisterFont(c));
    show("none");
    for (int i = 0; i < 12; i++) { const std::string p = std::string("/tmp/fonts/app") + std::to_string(i) + ".ttf"; printf("%d ", registerFont(p.c_str())); }
    for (int i = 0; i < 12; i++) { const std::string p = std::string("/tmp/fonts/app") + std::to_string(i) + ".ttf"; printf("%d ", unregisterFont(p.c_str())); }
    printf("\n");
}
static void groupDraw(const char* dump)
{
    struct Case { const char* file; int index; uint32_t made; const char* text; };
    const Case cases[] = {
        {"/tk/app/fonts/fontset/lato/Lato-Regular.ttf", 0, 0, "Hamburgefonstiv Wj,&@"},
        {"/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf", 0, 0, "Agjpq 123"},
        {"/tk/app/fonts/fontset/urw/URWBookman-LightItalic.otf", 0, 0, "Quick fox"},
        {"/tk/app/fonts/fontset/cjk/NotoSansCJK-Regular.ttc", 0, 0, "漢字かな한글"},
        {"/tk/app/fonts/fontset/color/NotoColorEmoji.ttf", 0, 0, "😀👍🌍⚽"},
        {"/tk/app/fonts/fontset/var/UbuntuSans[wdth,wght].ttf", 0x70000, 0, "Variable"},
        {"/tk/app/fonts/fontset/type1/C059-Roman.t1", 0, 0, "Type One"},
        {"/tk/app/fonts/fontset/pcf/10x20.pcf.gz", 0, 0, "Bitmap 10x20"},
        {"/tk/app/fonts/fontset/pcf/18x18ja.pcf.gz", 0, 0, "漢字ABC"},
        {"/tk/app/fonts/fontset/cjk/wqy-zenhei.ttc", 0, 0, "中文字"},
        {"/tk/app/fonts/fontset/misc/unifont_sample.otf", 0, 0, "Ωλ♥"},
        {"/tk/app/fonts/fontset/misc/hack-bold.woff", 0, 0, "woff {}"},
        {"@bold", 0, 1, "Made bold"}, {"@oblique", 0, 2, "Made oblique"}, {"@both", 0, 3, "Made both"}, {"@condensed", 0, 4, "Condensed 888"},
    };
    int serial = 0;
    for (const Case& c : cases) {
        Face* f = nullptr;
        if (c.made) {
            reef_ui_face_query q = {nullptr, "Noto Sans Arabic", nullptr, 0, 0, -1, -1, nullptr};
            if (c.made == 4) q.family = "Avenir Next Condensed";
            else { q.family = "Tuffy"; q.family = "Symbola"; if (c.made & 1) q.weight = 700; if (c.made & 2) q.italic = 1; }
            int exact = 0;
            f = match(&q, &exact);
        } else {
            f = fromFile(c.file, c.index);
        }
        printf("draw %s\n", c.file);
        describe(f);
        if (!f) continue;
        std::vector<uint32_t> chars;
        { const auto u = utf16(c.text); for (size_t i = 0; i < u.size(); i++) { if (u[i] >= 0xd800 && u[i] < 0xdc00) { chars.push_back(0x10000 + ((u[i] - 0xd800) << 10) + (u[i + 1] - 0xdc00)); i++; } else chars.push_back(u[i]); } }
        std::vector<uint32_t> ids(chars.size());
        glyphs(f, chars.data(), chars.size(), ids.data());
        std::vector<double> adv(ids.size());
        advances(f, ids.data(), ids.size(), 0, adv.data());
        reef_ui_face_metrics m; metrics(f, &m);
        for (double size : {7.0, 11.0, 13.3, 20.0, 47.0, 128.0, 300.0}) {
            for (int variant = 0; variant < 14; variant++) {
                for (int flags : {0, 1, 2, 3}) {
                    const int w = 360, h = 200;
                    ui::Bitmap bitmap(w, h, ui::Bitmap::Format::Argb32Premultiplied);
                    bitmap.fill(variant % 3 == 0 ? 0xffffffff : variant % 3 == 1 ? 0 : 0x80402010);
                    {
                        ui::Canvas2D canvas(bitmap);
                        const double s = size;
                        double matrix[4] = {s, 0, 0, -s};
                        canvas.translate(10.3, 120.7);
                        switch (variant) {
                        case 1: canvas.scale(1.25, 1.25); break;
                        case 2: canvas.scale(2, 2); break;
                        case 3: canvas.rotate(0.3); break;
                        case 4: canvas.scale(1.5, 0.8); break;
                        case 5: matrix[2] = s * 0.2; break;
                        case 6: matrix[3] = s; break;
                        case 7: canvas.scale(1, -1); matrix[3] = s; break;
                        case 8: canvas.scale(-1, 1); matrix[0] = -s; break;
                        case 9: canvas.rotate(1.5707963267948966); break;
                        case 10: canvas.scale(0.5, 0.5); break;
                        case 11: canvas.scale(1e-10, 1e-10); break;
                        case 12: canvas.setGlobalAlpha(0.4); break;
                        case 13: canvas.clipToRect({5, -40, 60, 50}); break;
                        }
                        ui::ColorF color = {0, 0, 0, 1};
                        if (variant % 4 == 1) color = {1, 0.2, 0.1, 0.7};
                        if (variant % 4 == 2) color = {0.1, 1.4, -0.3, 1.0};
                        if (variant % 4 == 3) color = {0.9, 0.9, 1, 2.0};
                        canvas.setFillColor(color);
                        std::vector<double> pos;
                        double x = 0;
                        for (size_t i = 0; i < ids.size(); i++) { pos.push_back(x); pos.push_back(i % 3 == 2 ? 0.37 : 0); x += adv[i] * s / m.unitsPerEm; }
                        // Twice: the second one is cached.
                        draw(canvas, f, matrix, ids.data(), pos.data(), ids.size(), flags);
                        canvas.translate(3, 40);
                        draw(canvas, f, matrix, ids.data(), pos.data(), ids.size(), flags);
                        draw(canvas, f, nullptr, ids.data(), pos.data(), ids.size(), flags);
                        draw(canvas, f, matrix, nullptr, pos.data(), ids.size(), flags);
                        draw(canvas, f, matrix, ids.data(), nullptr, ids.size(), flags);
                        draw(canvas, f, matrix, ids.data(), pos.data(), 0, flags);
                        draw(canvas, nullptr, matrix, ids.data(), pos.data(), 1, flags);
                        printf("  state %a %a %d", canvas.transform().m11(), canvas.transform().dy(), int(canvas.fillColor() == color));
                    }
                    uint64_t sum = 0;
                    const uint8_t* bits = bitmap.constBits();
                    for (size_t i = 0; i < bitmap.sizeInBytes(); i++) sum += bits[i];
                    printf(" %g/%d/%d #%d: %016llx sum %llu\n", size, variant, flags, serial, (unsigned long long)hash(bits, bitmap.sizeInBytes()), (unsigned long long)sum);
                    if (dump && (!*dump || atoi(dump) == serial)) bitmap.savePng("/tmp/fonts/draw" + std::to_string(serial) + (getenv("SIDE") ? getenv("SIDE") : "") + ".png");
                    serial++;
                }
            }
        }
        release(f);
    }
}
// Many different runs, so that the caches of bitmaps overflow.
static void groupCache()
{
    Face* f = fromFile("/tk/app/fonts/fontset/lato/Lato-Regular.ttf", 0);
    Face* e = fromFile("/tk/app/fonts/fontset/color/NotoColorEmoji.ttf", 0);
    uint64_t h = 0xcbf29ce484222325;
    for (int round = 0; round < 3; round++) {
        for (int i = 0; i < 700; i++) {
            ui::Bitmap bitmap(1100, 420, ui::Bitmap::Format::Argb32Premultiplied);
            bitmap.fill(0xffffffff);
            ui::Canvas2D canvas(bitmap);
            canvas.translate(20, 300);
            canvas.setFillColor({i % 10 / 10.0, 0.2, 0.3, 1});
            const double s = 20 + i % 230;
            const double matrix[4] = {s, 0, 0, -s};
            std::vector<uint32_t> g; std::vector<double> p;
            for (int k = 0; k < 24; k++) { g.push_back((i * 7 + k * 13) % 900 + 3); p.push_back(k * 40.25); p.push_back(k % 2 * 3.5); }
            draw(canvas, i % 9 == 8 ? e : f, matrix, g.data(), p.data(), g.size(), i % 5 == 4 ? 1 : 0);
            h = hash(bitmap.constBits(), bitmap.sizeInBytes(), h);
        }
        printf("round %d %016llx\n", round, (unsigned long long)h);
    }
    // Larger than a single bitmap may be.
    ui::Bitmap bitmap(6000, 900, ui::Bitmap::Format::Argb32Premultiplied);
    bitmap.fill(0xffffffff);
    { ui::Canvas2D canvas(bitmap); canvas.translate(0, 850); canvas.setFillColor({0, 0, 0, 1});
      const double matrix[4] = {250, 0, 0, -250}; std::vector<uint32_t> g; std::vector<double> p;
      for (int k = 0; k < 60; k++) { g.push_back(k + 30); p.push_back(k * 100); p.push_back(k % 4 * -190); }
      draw(canvas, f, matrix, g.data(), p.data(), g.size(), 0); draw(canvas, f, matrix, g.data(), p.data(), g.size(), 0); }
    printf("large %016llx\n", (unsigned long long)hash(bitmap.constBits(), bitmap.sizeInBytes()));
    release(f); release(e);
}
// A few questions, for the tests of the file of answers.
static void groupMini()
{
    const reef_ui_face_query none = {nullptr, nullptr, nullptr, 0, 0, -1, -1, nullptr};
    std::vector<reef_ui_face_query> q;
    for (const char* family : {"Lato", "DejaVu Sans", "Times", "Avenir Next Condensed", "Nonexistent"}) {
        auto x = none; x.family = family; q.push_back(x); x.weight = 700; q.push_back(x); x.italic = 1; q.push_back(x);
    }
    { auto x = none; x.name = "Lato-Bold"; q.push_back(x); x.name = "UbuntuSans-Bold"; q.push_back(x); x.language = "ja"; q.push_back(x); }
    std::vector<Face*> kept;
    for (const auto& x : q) {
        printQuery(x);
        int exact = 9;
        Face* f = match(&x, &exact);
        printf("  exact=%d\n", exact);
        describe(f);
        if (f) { printMetrics(f); kept.push_back(f); }
    }
    std::vector<char> b(1 << 20);
    for (int i : {0, 3, 6, 15}) printf("list %d %s\n", list(&q[i], b.data(), b.size()), b.data());
    { const int n = list(nullptr, b.data(), b.size()); printf("all %d %016llx\n", n, (unsigned long long)hash(b.data(), n)); }
    for (uint32_t c : {0x4e00u, 0x1f600u, 0x627u, 0x10ffffu})
        for (const char* l : {(const char*)nullptr, "ja"}) { Face* g = fallback(kept[0], c, l); printf("fallback %x\n", c); describe(g); if (g) { printMetrics(g); release(g); } }
    for (Face* f : kept) release(f);
}
static void guarded(const char* what, const std::function<void()>& f)
{
    fflush(stdout);
    if (const pid_t child = fork()) { int status = 0; waitpid(child, &status, 0); if (status) printf("%s status %x\n", what, status); return; }
    f();
    fflush(stdout);
    _exit(0);
}
static void groupNulls()
{
    Face* f = fromFile("/tk/app/fonts/fontset/lato/Lato-Regular.ttf", 0);
    char b[100]; uint32_t g[10] = {1, 2, 3}, o[10] = {}; double d[40] = {}; reef_ui_path_element e[10]; reef_ui_glyph gl[10]; uint16_t t[10] = {65, 66, 67}; reef_ui_face_axis ax[4]; reef_ui_face_metrics m;
    memset(&m, 0x33, sizeof m);
#define G(x) guarded(#x, [&] { x; printf("%s done\n", #x); })
    G(retain(nullptr)); G(release(nullptr)); G(metrics(nullptr, &m)); G(metrics(f, nullptr)); G(metrics(nullptr, nullptr));
    guarded("metrics null", [&] { metrics(nullptr, &m); printf("%016llx\n", (unsigned long long)hash(&m, sizeof m)); });
    for (int id : {-2, -1, 0, 1, 2, 4, 6, 16, 70000}) G(printf("%d ", name(nullptr, id, b, 100)));
    guarded("name", [&] { memset(b, 0x55, 100); const int n = name(nullptr, 1, b, 100); printf("%d %x\n", n, b[0]); });
    G(glyphs(nullptr, g, 3, o)); G(glyphs(f, nullptr, 3, o)); G(glyphs(f, g, 3, nullptr)); G(glyphs(f, g, 0, o));
    guarded("glyphs", [&] { memset(o, 0x55, sizeof o); glyphs(nullptr, g, 3, o); printf("%x %x\n", o[0], o[3]); });
    for (int v : {0, 1}) { G(advances(nullptr, g, 3, v, d)); G(advances(f, nullptr, 3, v, d)); G(advances(f, g, 3, v, nullptr)); G(advances(f, g, 0, v, d)); }
    guarded("advances", [&] { memset(d, 0x55, sizeof d); advances(nullptr, g, 3, 0, d); printf("%a %a\n", d[0], d[3]); });
    G(bounds(nullptr, g, 3, d)); G(bounds(f, nullptr, 3, d)); G(bounds(f, g, 3, nullptr)); G(bounds(f, g, 0, d));
    guarded("bounds", [&] { memset(d, 0x55, sizeof d); bounds(nullptr, g, 3, d); printf("%a %a\n", d[0], d[12]); });
    G(printf("%u ", outline(nullptr, 5, e, 10))); G(printf("%u ", outline(f, 5, nullptr, 10))); G(printf("%u ", outline(f, 36, nullptr, 0)));
    G(printf("%d ", glyphName(nullptr, 5, b, 100))); G(printf("%d ", glyphName(f, 5, nullptr, 100))); G(printf("%d ", glyphName(f, 36, b, 0))); G(printf("%d ", glyphName(f, 36, b, -1)));
    G(printf("%u ", glyphNamed(nullptr, "A"))); G(printf("%u ", glyphNamed(f, nullptr)));
    G(printf("%zd ", tableData(nullptr, 0x68656164, b, 100))); G(printf("%zd ", tableData(f, 0x68656164, nullptr, 100))); G(printf("%zd ", tableData(f, 0, b, 100))); G(printf("%zd ", tableData(f, 1, nullptr, 0)));
    G(printf("%u ", tables(nullptr, g, 3))); G(printf("%u ", tables(f, nullptr, 3))); G(printf("%u ", tables(f, o, 0)));
    G(printf("%u ", ranges(nullptr, g, 3))); G(printf("%u ", ranges(f, nullptr, 3))); G(printf("%u ", ranges(f, o, 0)));
    G(printf("%d ", languages(nullptr, b, 100))); G(printf("%d ", languages(f, nullptr, 100))); G(printf("%d ", languages(f, b, 0))); G(printf("%d ", languages(f, b, -3)));
    G(printf("%p ", (void*)fallback(nullptr, 65, "en"))); G(printf("%u ", shape(nullptr, t, 3, 0, 3, 10, 0, 0, nullptr, nullptr, 0, gl, 10)));
    G(printf("%u ", shape(f, nullptr, 3, 0, 3, 10, 0, 0, nullptr, nullptr, 0, gl, 10))); G(printf("%u ", shape(f, t, 3, 4, 3, 10, 0, 0, nullptr, nullptr, 0, gl, 10))); G(printf("%u ", shape(f, t, 3, 0, 3, 10, 0, 0, nullptr, nullptr, 0, nullptr, 10)));
    G(printf("%u ", shape(f, t, 3, 0, 3, 10, 0, 0, nullptr, nullptr, 1, gl, 10)));
    G(printf("%u ", axes(nullptr, ax, 4))); G(printf("%u ", axes(f, nullptr, 4))); G(printf("%u ", coordinates(nullptr, g, d, 3))); G(printf("%u ", coordinates(f, nullptr, d, 3)));
    Face* v = fromFile("/tk/app/fonts/fontset/var/Ubuntu[wdth,wght].ttf", 0);
    printf("%d\n", v != nullptr);
    if (v) { G(printf("%u ", axes(v, nullptr, 4))); G(printf("%u ", axes(v, ax, 1))); G(printf("%u ", coordinates(v, nullptr, d, 3))); G(printf("%u ", coordinates(v, o, nullptr, 3))); G(printf("%u ", coordinates(v, o, d, 1))); G(describe(vary(v, nullptr, d, 1))); G(describe(vary(v, g, nullptr, 1))); G(describe(vary(v, nullptr, nullptr, 0))); G(describe(vary(v, g, d, 0))); }
    G(printf("%p ", (void*)vary(nullptr, g, d, 1))); G(printf("%d ", registerFont(nullptr))); G(printf("%d ", unregisterFont(nullptr))); G(printf("%d ", registerFont(""))); G(printf("%d ", unregisterFont("")));
    G(printf("%p ", (void*)fromFile(nullptr, 0))); G(printf("%p ", (void*)fromFile("", 0))); G(printf("%p ", (void*)fromData(nullptr, 5, 0))); G(printf("%p ", (void*)fromData(b, 0, 0)));
    G(printf("%d ", list(nullptr, nullptr, 0))); G(printf("%d ", list(nullptr, b, -5))); G(printf("%p ", (void*)match(nullptr, nullptr)));
    int kind = 9;
    G(printf("%zu ", subset(nullptr, {1}, &kind).size())); G(printf("%zu ", subset(f, {}, nullptr).size())); G(printf("%zu ", characters(nullptr).size())); G(printf("%a %d ", stretch(nullptr), outlineFormat(nullptr))); G(printf("%zu ", subset(f, {70000, 0xffffffff, 5}, &kind).size()));
    release(f);
}
// All names that fontconfig knows, also those of variable fonts.
static void groupNames()
{
    FcPattern* pattern = FcPatternCreate();
    FcObjectSet* objects = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FULLNAME, FC_POSTSCRIPT_NAME, FC_VARIABLE, nullptr);
    FcFontSet* set = FcFontList(nullptr, pattern, objects);
    std::set<std::string> full, families;
    std::set<std::pair<std::string, std::string>> styles;
    for (int i = 0; i < set->nfont; i++) {
        FcChar8* value;
        for (const char* object : {FC_FULLNAME, FC_POSTSCRIPT_NAME}) for (int k = 0; FcPatternGetString(set->fonts[i], object, k, &value) == FcResultMatch; k++) full.insert((const char*)value);
        for (int k = 0; FcPatternGetString(set->fonts[i], FC_FAMILY, k, &value) == FcResultMatch; k++) { const std::string family = (const char*)value; families.insert(family); for (int j = 0; FcPatternGetString(set->fonts[i], FC_STYLE, j, &value) == FcResultMatch; j++) styles.insert({family, (const char*)value}); }
    }
    printf("%zu %zu %zu\n", full.size(), families.size(), styles.size());
    reef_ui_face_query none = {nullptr, nullptr, nullptr, 0, 0, -1, -1, nullptr};
    std::vector<char> b(1 << 20);
    const auto ask = [&](const reef_ui_face_query& q) {
        printQuery(q);
        int exact = 5;
        Face* f = match(&q, &exact);
        printf("  %d\n", exact);
        describe(f);
        if (f) { printMetrics(f); printNames(f); release(f); }
        const int n = list(&q, b.data(), b.size());
        printf("  %d %s\n", n, n > 0 && n < 1500 ? quoted(b.data()).c_str() : "");
    };
    for (const auto& n : full) { auto q = none; q.name = n.c_str(); ask(q); }
    for (const auto& n : families) for (const double w : {0.0, 800.0}) for (int it : {-1, 1}) { auto q = none; q.family = n.c_str(); q.weight = w; q.italic = it; ask(q); }
    for (const auto& [f, t] : styles) { auto q = none; q.family = f.c_str(); q.style = t.c_str(); ask(q); }
}
// More questions than are kept.
static void groupMany()
{
    reef_ui_face_query q = {nullptr, "Lato", nullptr, 0, 0, -1, -1, nullptr};
    for (int round = 0; round < 2; round++) {
        uint64_t h = 0xcbf29ce484222325;
        for (int i = 0; i < 4300; i++) {
            const std::string family = "Family " + std::to_string(i % 4250);
            q.family = family.c_str();
            q.weight = 100 + i % 4250 % 9 * 100;
            int exact = 0;
            Face* f = match(&q, &exact);
            reef_ui_face_metrics m; metrics(f, &m);
            h = hash(&m, sizeof m, h); h = hash(&exact, 4, h);
            if (i % 500 == 0) describe(f);
            release(f);
        }
        printf("round %d %016llx\n", round, (unsigned long long)h);
    }
    std::vector<char> b(1 << 20);
    for (int round = 0; round < 2; round++) for (int i = 0; i < 300; i++) { const std::string family = "Family " + std::to_string(i); q.family = family.c_str(); q.weight = 0; const int n = list(&q, b.data(), b.size()); if (n) printf("%d\n", n); }
    Face* f = fromFile("/tk/app/fonts/fontset/misc/tuffy_bold.ttf", 0);
    for (const char* l : {(const char*)nullptr, "ja"}) {
        uint64_t h = 0xcbf29ce484222325; int found = 0;
        for (uint32_t c = 0x3400; c < 0x3400 + 8300; c++) { Face* g = fallback(f, c, l); if (g) { found++; const auto& path = *reinterpret_cast<const std::string*>(reinterpret_cast<const char*>(g) + 8); h = hash(path.data(), path.size(), h); release(g); } }
        for (uint32_t c = 0x3400; c < 0x3400 + 8300; c += 8200) { Face* g = fallback(f, c, l); describe(g); if (g) release(g); }
        for (uint32_t c : {0x627u, 0x5d0u, 0x1f600u}) { Face* g = fallback(f, c, l); describe(g); if (g) release(g); }
        printf("fallback %d %016llx\n", found, (unsigned long long)h);
    }
    describe(f);
    release(f);
}
// Faces with something of their own ask for others.
static void groupVaried()
{
    const uint32_t t[] = {0x77676874, 0x77647468}; const double v[] = {650.5, 88};
    Face* u = fromFile("/tk/app/fonts/fontset/var/Ubuntu[wdth,wght].ttf", 0);
    std::vector<Face*> all = {u, vary(u, t, v, 1), vary(u, t, v, 2), fromFile("/tk/app/fonts/fontset/var/Ubuntu[wdth,wght].ttf", 0x30000)};
    for (const char* family : {"Avenir Next Condensed", "FreeSerif", "Symbola", "Noto Sans Math", "C059", "Standard Symbols PS", "10x20", "fixed"})
        for (int i : {0, 1}) { const reef_ui_face_query q = {nullptr, family, nullptr, i ? 750.0 : 0.0, 0, i, -1, nullptr}; all.push_back(match(&q, nullptr)); }
    for (Face* f : all) {
        describe(f);
        if (!f) continue;
        printMetrics(f); printNames(f); glyphTests(f, false); tableTests(f);
        for (uint32_t c : {0x41u, 0x416u, 0x4e00u, 0x1f600u, 0x627u, 0x10ffffu}) for (const char* l : {(const char*)nullptr, "ja"}) { Face* g = fallback(f, c, l); describe(g); if (g) release(g); }
        Face* again = vary(f, t, v, 2);
        describe(again);
        if (again) { for (uint32_t c : {0x4e00u, 0x627u}) { Face* g = fallback(again, c, nullptr); describe(g); if (g) release(g); } release(again); }
        // Text in it.
        const auto text = utf16("Wave office \xc4\x83\xc8\x99 12");
        for (int flags : {0, 1, 2}) { std::vector<reef_ui_glyph> g(100); memset(g.data(), 0x66, 100 * sizeof g[0]); const uint32_t n = shape(f, text.data(), text.size(), 0, text.size(), 17, flags, 0, nullptr, nullptr, 0, g.data(), 100); printf("  shape %u %016llx\n", n, (unsigned long long)hash(g.data(), 100 * sizeof g[0])); }
    }
    for (Face* f : all) if (f) release(f);
}
// Files that go after fontconfig saw them.
static void groupGone()
{
    const std::string dir = getenv("GONE");
    for (const char* n : {"lato/Lato-Black.ttf", "noto/NotoSansKhmer-Regular.ttf", "misc/tuffy_bold.ttf"}) std::filesystem::copy_file(std::string("/tk/app/fonts/fontset/") + n, dir + "/" + std::filesystem::path(n).filename().string(), std::filesystem::copy_options::overwrite_existing);
    std::vector<char> b(1 << 20);
    printf("%d\n", list(nullptr, b.data(), b.size()));
    for (const auto& e : std::filesystem::directory_iterator(dir)) std::filesystem::remove(e.path());
    reef_ui_face_query q = {nullptr, nullptr, nullptr, 0, 0, -1, -1, nullptr};
    for (const char* n : {"Lato-Black", "Tuffy-Bold", "Lato Black"}) { q.name = n; int exact = 4; Face* f = match(&q, &exact); printf("%s %d\n", n, exact); describe(f); if (f) release(f); }
    q.name = nullptr;
    for (const char* n : {"Lato", "Tuffy", "Noto Sans Khmer"}) for (double w : {0.0, 900.0}) { q.family = n; q.weight = w; for (int i = 0; i < 2; i++) { int exact = 4; Face* f = match(&q, &exact); printf("%s %g %d\n", n, w, exact); describe(f); if (f) release(f); } }
    Face* f = fromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0);
    for (uint32_t c : {0x1780u, 0x17a0u, 0x4e00u}) for (int i = 0; i < 2; i++) { Face* g = fallback(f, c, nullptr); describe(g); if (g) release(g); }
    for (const char* n : {"/Lato-Black.ttf", "/Tuffy.ttf"}) describe(fromFile((dir + n).c_str(), 0));
    if (f) release(f);
    q.family = "Lato"; q.weight = 0;
    printf("%d %s\n", list(&q, b.data(), b.size()), b.data());
}
// The fonts of the program after fontconfig read everything again.
static void groupReread()
{
    reef_ui_face_query q = {nullptr, "Tuffy", nullptr, 0, 0, -1, -1, nullptr};
    char b[4000];
    const auto show = [&](const char* what) { int exact = 5; Face* f = match(&q, &exact); printf("%s %d\n", what, exact); describe(f); if (f) release(f); const int n = list(&q, b, sizeof b); printf("  %d %s\n", n, quoted(b).c_str()); };
    show("before");
    printf("%d %d\n", registerFont("/tmp/fonts/app0.ttf"), registerFont("/tmp/fonts/apphack.ttf"));
    show("registered");
    printf("%d\n", FcInitReinitialize());
    show("read again");
    printf("%d\n", unregisterFont("/tmp/fonts/app0.ttf"));
    show("without");
    printf("%d\n", FcInitReinitialize());
    show("read again");
    printf("%d\n", unregisterFont("/tmp/fonts/apphack.ttf"));
    show("none");
}
static void groupPua()
{
    std::vector<char> b(1 << 16);
    const int all = list(nullptr, b.data(), b.size());
    printf("%d\n", all);
    std::filesystem::remove(getenv("PUA_GONE"));
    Face* f = fromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0);
    for (int i = 0; i < 2; i++) for (const char* l : {(const char*)nullptr, "en"}) { Face* g = fallback(f, 0xe123, l); describe(g); if (g) release(g); }
    describe(f);
    release(f);
}
// The same text with every font at many sizes.
static void groupSizes()
{
    for (const std::string& file : fontFiles()) {
        Face* f = fromFile(file.c_str(), 0);
        if (!f) continue;
        std::vector<uint32_t> chars = {'A', 'B', 'C', 'D', 'a', 'b', 0x1f600, 0x1f44d, 0x1f30d, 0x26bd}, ids(chars.size());
        glyphs(f, chars.data(), chars.size(), ids.data());
        printf("sizes %s\n", file.c_str());
        for (double size : {0.4, 5.0, 8.9, 9.0, 10.0, 12.0, 15.0, 16.0, 17.0, 19.99, 20.0, 22.0, 24.0, 30.0, 33.0, 40.0, 50.0, 52.0, 64.0, 100.0, 109.0, 110.0, 130.0, 160.0, 200.0, 255.0, 256.0, 257.0})
            for (int flags : {0, 1, 2, 4}) for (double turn : {0.0, 0.5}) {
                ui::Bitmap bitmap(500, 300, ui::Bitmap::Format::Argb32Premultiplied);
                bitmap.fill(0xffffffff);
                { ui::Canvas2D canvas(bitmap); canvas.translate(20.4, 200.6); if (turn) canvas.rotate(turn); canvas.setFillColor({0.2, 0.4, 0.9, 0.8});
                  const double matrix[4] = {size, 0, 0, -size}; std::vector<double> p; for (size_t i = 0; i < ids.size(); i++) { p.push_back(i * 45.3); p.push_back(i % 2 * 3.7); }
                  draw(canvas, f, matrix, ids.data(), p.data(), ids.size(), flags); }
                uint64_t sum = 0;
                for (size_t i = 0; i < bitmap.sizeInBytes(); i++) sum += 255 - bitmap.constBits()[i];
                printf(" %g/%d/%g %016llx %llu", size, flags, turn, (unsigned long long)hash(bitmap.constBits(), bitmap.sizeInBytes()), (unsigned long long)sum);
            }
        printf("\n");
        release(f);
    }
}
// Random calls, the same for a seed.
#include <random>
struct Random
{
    std::mt19937_64 engine;
    uint64_t operator()(uint64_t n) { return n ? engine() % n : 0; }
    double real(double from, double to) { return from + (to - from) * (engine() >> 11) / 9007199254740992.0; }
    bool chance(int in) { return (*this)(in) == 0; }
};
static bool oddNumbers = true;
static double oddNumber(Random& r, double usual)
{
    if (!oddNumbers) return usual;
    static const double odd[] = {0, -0.0, 1e-9, -1e-9, 1e9, -1e9, 1e300, -1e300, 1.0 / 0.0, -1.0 / 0.0, 0.0 / 0.0, 0.5, 65535.5, 2147483648.0, -2147483649.0};
    return r.chance(40) ? odd[r(15)] : usual;
}
static void fuzzDraw(uint64_t seed, int count)
{
    const auto files = fontFiles();
    for (int k = 0; k < count; k++) {
        Random r = {std::mt19937_64(seed * 1000003 + k)};
        const std::string& file = files[r(files.size())];
        fflush(stdout);
        static const int indices[] = {0, 0, 0, 1, 0x10000, 0x20000, 0x50000};
        Face* f = nullptr;
        if (r.chance(6)) {
            static const char* const families[] = {"Avenir Next Condensed", "Symbola", "Noto Sans Arabic", "Noto Sans Math", "Tuffy", "10x20"};
            reef_ui_face_query q = {nullptr, families[r(6)], nullptr, r.chance(2) ? 700.0 : 0.0, 0, int(r(3)) - 1, -1, nullptr};
            f = match(&q, nullptr);
        } else {
            f = fromFile(file.c_str(), indices[r(7)]);
        }
        if (!f) { printf("%d null %s\n", k, file.c_str()); continue; }
        reef_ui_face_metrics m;
        metrics(f, &m);
        const int w = 1 + r(300), h = 1 + r(200);
        ui::Bitmap bitmap(w, h, ui::Bitmap::Format::Argb32Premultiplied);
        static const uint32_t backs[] = {0, 0xffffffff, 0x80402010, 0xff00ff00};
        bitmap.fill(backs[r(4)]);
        std::vector<uint32_t> g; std::vector<double> p;
        const int n = 1 + r(12);
        const double size = oddNumber(r, std::exp(r.real(std::log(0.5), std::log(r.chance(3) ? 600 : 60))));
        std::vector<uint32_t> chars, ids;
        for (int i = 0; i < n; i++) chars.push_back(r.chance(3) ? r(0x3000) : 33 + r(90));
        ids.resize(n);
        glyphs(f, chars.data(), n, ids.data());
        double x = 0;
        for (int i = 0; i < n; i++) {
            g.push_back(r.chance(12) ? uint32_t(r(0x20000)) : r.chance(60) ? 0xffffffff : ids[i]);
            p.push_back(oddNumber(r, x)); p.push_back(oddNumber(r, r.chance(3) ? r.real(-40, 40) : 0));
            x += r.real(0, size);
        }
        double matrix[4] = {size, 0, 0, r.chance(6) ? size : -size};
        switch (r(8)) {
        case 0: for (double& v : matrix) v = r.real(-size, size); break;
        case 1: matrix[2] = size * r.real(-1, 1); break;
        case 2: matrix[1] = size * r.real(-1, 1); break;
        case 3: { const double a = r.real(0, 6.3); matrix[0] = size * std::cos(a); matrix[1] = size * std::sin(a); matrix[2] = size * std::sin(a); matrix[3] = -size * std::cos(a); break; }
        case 4: matrix[r(4)] = oddNumber(r, 0); break;
        case 5: matrix[0] *= r.real(0.2, 3); break;
        }
        const int flags = r(8);
        ui::ColorF color = {r.real(0, 1), r.real(0, 1), r.real(0, 1), r.chance(2) ? 1 : r.real(0, 1)};
        if (r.chance(10)) color = {oddNumber(r, r.real(-2, 3)), oddNumber(r, r.real(-2, 3)), r.real(-2, 3), r.real(-2, 3)};
        std::string what;
        {
            ui::Canvas2D canvas(bitmap);
            canvas.translate(oddNumber(r, r.real(-20, w)), oddNumber(r, r.real(0, h + 20)));
            switch (r(9)) {
            case 0: canvas.scale(2, 2); break;
            case 1: canvas.scale(r.real(0.3, 3), r.real(0.3, 3)); break;
            case 2: canvas.rotate(r.real(0, 6.3)); break;
            case 3: canvas.scale(1, -1); break;
            case 4: canvas.scale(-1, 1); break;
            case 5: canvas.rotate(1.5707963267948966 * r(4)); break;
            case 6: canvas.scale(1.25, 1.25); break;
            }
            if (r.chance(5)) canvas.setGlobalAlpha(r.real(0, 1));
            if (r.chance(5)) canvas.clipToRect({r.real(0, 50), r.real(-80, 20), r.real(1, 100), r.real(1, 100)});
            canvas.setFillColor(color);
            draw(canvas, f, matrix, g.data(), p.data(), n, flags);
            if (r.chance(3)) { canvas.translate(r(9), r(5)); draw(canvas, f, matrix, g.data(), p.data(), n, flags); }
            const auto t = canvas.transform();
            char b[200]; snprintf(b, 200, "%a %a %a", t.m11(), t.dx(), t.dy()); what = b;
        }
        uint64_t sum = 0;
        for (size_t i = 0; i < bitmap.sizeInBytes(); i++) sum += bitmap.constBits()[i];
        printf("%d %s size %g flags %d n %d %dx%d %s: %016llx %llu\n", k, file.c_str() + 22, size, flags, n, w, h, what.c_str(), (unsigned long long)hash(bitmap.constBits(), bitmap.sizeInBytes()), (unsigned long long)sum);
        release(f);
    }
}
static void fuzzShape(uint64_t seed, int count)
{
    const auto files = fontFiles();
    static const char* const langs[] = {nullptr, "", "en", "ro", "tr", "ja", "zh-Hans", "zh-Hant-HK", "ar", "sr", "x-unknown", "de-DE-1996", "nonsense language"};
    static const uint32_t scripts[] = {0, 0, 0, 0x4c61746e, 0x41726162, 0x44657661, 0x48616e69, 0x6c61746e, 0x1, 0xffffffff, 0x5a797979};
    static const uint32_t tags[] = {0x6b65726e, 0x6c696761, 0x63616c74, 0x736d6370, 0x66726163, 0x73733031, 0x76657274, 0x6f6e756d, 0x41414141, 0};
    for (int k = 0; k < count; k++) {
        Random r = {std::mt19937_64(seed * 1000003 + k)};
        const std::string& file = files[r(files.size())];
        Face* f = fromFile(file.c_str(), r.chance(4) ? 0x10000 * r(6) : 0);
        if (!f) { printf("%d null\n", k); continue; }
        std::vector<uint16_t> u;
        static const uint32_t blocks[] = {0x20, 0x20, 0xa0, 0x300, 0x370, 0x400, 0x590, 0x600, 0x900, 0xb80, 0xe00, 0x1780, 0x1800, 0x2000, 0x3040, 0x4e00, 0xac00, 0xd800, 0xdc00, 0xfb00, 0xfe00};
        const int n = r(30);
        uint32_t block = blocks[r(21)];
        for (int i = 0; i < n; i++) { if (r.chance(5)) block = blocks[r(21)]; if (r.chance(15)) { const uint32_t c = 0x1f300 + r(0x300); u.push_back(0xd800 + ((c - 0x10000) >> 10)); u.push_back(0xdc00 + (c & 0x3ff)); } else u.push_back(block + r(0x60)); }
        const uint32_t length = u.size();
        u.push_back(0x41);
        const uint32_t start = r.chance(3) ? r(length + 3) : 0, part = r.chance(3) ? r(length + 3) : r.chance(9) ? 0xffffffff : length;
        std::vector<reef_ui_feature> fe;
        for (int i = r.chance(2) ? r(4) : 0; i > 0; i--) fe.push_back({tags[r(10)], uint32_t(r(4)), r.chance(2) ? 0 : uint32_t(r(20)), r.chance(2) ? 0xffffffff : uint32_t(r(30))});
        std::vector<reef_ui_glyph> g(r.chance(6) ? r(6) : length * 5 + 10);
        g.push_back({});
        memset(g.data(), 0x66, g.size() * sizeof g[0]);
        const double size = oddNumber(r, r.real(1, 300));
        const uint32_t count2 = shape(f, u.data(), length, start, part, size, r(8), scripts[r(11)], langs[r(13)], fe.empty() ? nullptr : fe.data(), fe.size(), r.chance(20) ? nullptr : g.data(), g.size() - 1);
        printf("%d %s %u %u %u: %u %016llx\n", k, file.c_str() + 22, length, start, part, count2, (unsigned long long)hash(g.data(), g.size() * sizeof g[0]));
        release(f);
    }
}
static void fuzzMatch(uint64_t seed, int count)
{
    std::vector<char> b(1 << 22);
    list(nullptr, b.data(), b.size());
    std::vector<std::string> names = {"", "sans-serif", "serif", "monospace", "Times", "Helvetica", "Arial", "Courier", "Menlo", "Avenir Next Condensed", "Nonexistent"}, families = names, styles = {"", "Bold", "Italic", "Demi Bold", "Light", "Nonsense"};
    std::istringstream in(b.data());
    for (std::string line; std::getline(in, line);) {
        std::vector<std::string> fields; std::istringstream l(line);
        for (std::string field; std::getline(l, field, '\t');) fields.push_back(field);
        if (fields.size() > 4) { names.push_back(fields[2]); families.push_back(fields[3]); styles.push_back(fields[4]); }
    }
    static const char* const langs[] = {"", "en", "ja", "ko", "zh-cn", "zh-tw", "ar", "he", "th", "ro", "xx", "nonsense"};
    const auto change = [](Random& r, std::string s) {
        for (int i = r.chance(3) ? 1 + r(2) : 0; i > 0 && !s.empty(); i--) switch (r(6)) {
        case 0: for (char& c : s) c = toupper(c); break;
        case 1: for (char& c : s) c = tolower(c); break;
        case 2: s.erase(std::remove(s.begin(), s.end(), ' '), s.end()); break;
        case 3: s += r.chance(2) ? " Bold" : "-Bold"; break;
        case 4: s.erase(r(s.size()), 1); break;
        case 5: std::replace(s.begin(), s.end(), ' ', '-'); break;
        }
        return s;
    };
    for (int k = 0; k < count; k++) {
        Random r = {std::mt19937_64(seed * 1000003 + k)};
        const std::string nm = change(r, names[r(names.size())]), fm = change(r, families[r(families.size())]), st = change(r, styles[r(styles.size())]);
        reef_ui_face_query q = {r.chance(3) ? nm.c_str() : nullptr, r.chance(4) ? nullptr : fm.c_str(), r.chance(3) ? st.c_str() : nullptr,
                                r.chance(2) ? 0 : oddNumber(r, r.real(50, 1000)), r.chance(2) ? 0 : oddNumber(r, r.real(0.5, 10)), int(r(4)) - 1, int(r(3)) - 1, r.chance(3) ? langs[r(12)] : nullptr};
        printQuery(q);
        int exact = 9;
        Face* f = match(&q, &exact);
        printf("%d exact=%d\n", k, exact);
        describe(f);
        if (f) {
            printMetrics(f);
            printf("  %s %s %s\n", nameOf(f, 1).c_str(), nameOf(f, 2).c_str(), nameOf(f, 6).c_str());
            if (r.chance(4)) { Face* g = fallback(f, 0x100 + r(0x11000), q.language); describe(g); if (g) release(g); }
            if (r.chance(4)) { const uint32_t t[] = {0x77676874, 0x77647468}; const double v[] = {r.real(0, 1000), r.real(40, 120)}; Face* g = vary(f, t, v, r(3)); describe(g); if (g) { printMetrics(g); release(g); } }
            release(f);
        }
        if (r.chance(3)) { const int n = list(&q, b.data(), b.size()); printf("  list %d %016llx\n", n, (unsigned long long)hash(b.data(), n > 0 ? n : 0)); }
    }
}
// Damaged fonts from memory.
static void fuzzData(uint64_t seed, int count)
{
    auto files = fontFiles();
    files.erase(std::remove_if(files.begin(), files.end(), [](const std::string& f) { return std::filesystem::file_size(f) > 1500000; }), files.end());
    for (int k = 0; k < count; k++) {
        Random r = {std::mt19937_64(seed * 1000003 + k)};
        const std::string& file = files[r(files.size())];
        std::ifstream in(file, std::ios::binary); std::stringstream ss; ss << in.rdbuf(); std::string d = ss.str();
        for (int i = 1 + r(6); i > 0; i--) {
            // In a table of the directory, or anywhere.
            size_t from = 0, size = d.size();
            const int tablesCount = d.size() > 12 ? uint8_t(d[4]) << 8 | uint8_t(d[5]) : 0;
            if (tablesCount > 0 && tablesCount < 64 && !r.chance(4)) {
                const size_t at = 12 + 16 * r(tablesCount);
                const auto u32 = [&](size_t o) { return o + 4 <= d.size() ? uint32_t(uint8_t(d[o]) << 24 | uint8_t(d[o + 1]) << 16 | uint8_t(d[o + 2]) << 8 | uint8_t(d[o + 3])) : 0u; };
                if (r.chance(8)) { from = at; size = 16; }
                else if (u32(at + 8) < d.size() && u32(at + 12) <= d.size() - u32(at + 8) && u32(at + 12) > 0) { from = u32(at + 8); size = std::min<size_t>(u32(at + 12), r.chance(2) ? 100 : 1u << 30); }
            }
            d[from + r(size)] = char(r(256));
        }
        Face* f = fromData(d.data(), d.size(), r.chance(4) ? 0x10000 * r(4) : 0);
        printf("%d %s\n", k, file.c_str() + 22);
        describe(f);
        if (!f) continue;
        printMetrics(f); printNames(f); glyphTests(f, false); tableTests(f);
        release(f);
    }
}
int main(int argc, char** argv)
{
    void* lib = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (!lib) { fprintf(stderr, "%s\n", dlerror()); return 1; }
    table = static_cast<Table*>(reinterpret_cast<void* (*)(int)>(dlsym(lib, "reef_appkit_host_interface"))(0x18));
    if (void* e = dlsym(lib, "fonts_extra")) {
        for (int i = 0; i < 8; i++) extras[i] = reinterpret_cast<void* (*)(int)>(e)(i);
    } else {
        link_map* map; dlinfo(lib, RTLD_DI_LINKMAP, &map);
        const size_t offsets[] = {0x80d50, 0x80d60, 0x80d80, 0x80d90, 0x80e80, 0x81a70, 0x82200, 0x90650};
        for (int i = 0; i < 8; i++) extras[i] = reinterpret_cast<void*>(map->l_addr + offsets[i]);
    }
    setvbuf(stdout, nullptr, argc > 2 && strncmp(argv[2], "fuzz", 4) == 0 ? _IOLBF : _IOFBF, 1 << 20);
    if (const char* family = getenv("THEME_FAMILY")) { ui::Theme t = ui::Theme::light(); t.fontFamily = family; ui::Theme::setCurrent(t); }
    for (int i = 2; i < argc; i++) {
        const std::string g = argv[i];
        printf("=== %s\n", g.c_str());
        if (g == "files") groupFiles(false); else if (g == "deep") groupFiles(true); else if (g == "data") groupData(); else if (g == "match") groupMatch();
        else if (g == "list") groupList(); else if (g == "shape") groupShape(); else if (g == "features") groupFeatures(); else if (g == "fallback") groupFallback();
        else if (g == "vary") groupVary(); else if (g == "register") groupRegister(); else if (g == "draw") groupDraw(nullptr); else if (g.rfind("draw=", 0) == 0) groupDraw(g.c_str() + 5);
        else if (g == "cache") groupCache(); else if (g == "mini") groupMini();
        else if (g == "nulls") groupNulls(); else if (g == "pua") groupPua(); else if (g == "sizes") groupSizes(); else if (g == "names") groupNames(); else if (g == "many") groupMany(); else if (g == "varied") groupVaried(); else if (g == "gone") groupGone(); else if (g == "reread") groupReread();
        else if (g.rfind("fuzz", 0) == 0) { char what[20] = ""; unsigned long long seed = 1; int count = 100; sscanf(g.c_str(), "fuzz-%19[a-z]-%llu-%d", what, &seed, &count); const std::string w = what; if (w == "draw") { oddNumbers = false; fuzzDraw(seed, count); } else if (w == "odd") {
            // Each in a process of its own, some of them crash.
            for (int k = 0; k < count; k++) {
                fflush(stdout);
                if (const pid_t child = fork()) { int status = 0; waitpid(child, &status, 0); printf("%d status %x\n", k, status); }
                else { freopen("/dev/null", "w", stderr); fuzzDraw(seed + k, 4); fflush(stdout); _exit(0); }
            }
        } else if (w == "shape") fuzzShape(seed, count); else if (w == "match") fuzzMatch(seed, count); else if (w == "data") fuzzData(seed, count); } else { fprintf(stderr, "unknown %s\n", g.c_str()); return 2; }
        fflush(stdout);
    }
    return 0;
}
