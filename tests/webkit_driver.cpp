// Calls functions of the table of libreef-webkit-host.so and prints what comes back.
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <string>
#include <vector>
extern "C" void* WKFakeCallback(const char*);
extern "C" void* WKFakeUInt64(uint64_t);
extern "C" { void* WKStringCreateWithUTF8CString(const char*); void* WKDoubleCreate(double); void* WKBooleanCreate(bool); void* WKMutableArrayCreate();
    bool WKArrayAppendItem(void*, void*); void* WKMutableDictionaryCreate(); void WKDictionarySetItem(void*, void*, void*); void* WKURLCreateWithUTF8CString(const char*); }
extern "C" { const char* g_get_prgname(); void g_set_prgname(const char*); }
static void** table;
template<typename... A> static void* call(int i, A... a) { return reinterpret_cast<void* (*)(A...)>(table[i])(a...); }
static void message(void*, const char* name, const char* json, const char* origin, int main, void*) { printf("message %s <%s> %s %d\n", name, json, origin, main); }
struct Record { const char* name; int kinds; };
static void records(void*, const Record* r, int64_t n) { for (int64_t i = 0; i < n; i++) printf("record %s %x\n", r[i].name, r[i].kinds); }
static unsigned seed = 1;
static unsigned next(unsigned n) { seed = seed * 1103515245 + 12345; return (seed >> 16) % n; }
static std::string randomJson(int depth) {
    switch (next(depth > 4 ? 6 : 8)) {
    case 0: return "null"; case 1: return "true"; case 2: return "false";
    case 3: { char b[40]; snprintf(b, sizeof b, "%.*g", int(next(18)), (next(2) ? -1 : 1) * next(100000) / 7.0 * std::pow(10, int(next(40)) - 20)); return b; }
    case 4: case 5: { static const char* parts[] = {"a", "long text here ", "\\n", "\\t\\r\\b\\f", "\\u00e9", "\\u20ac", "\\ud83d\\ude00", "\\ud800x", "\\u12", "\\\"", "\\\\", "\\/", "\\q", "\xc3\xa9", "\\u0041", "\\uD83D"};
        std::string s = "\""; for (unsigned n = next(9); n; n--) s += parts[next(16)]; return s + (next(20) ? "\"" : ""); }
    case 6: { std::string s = "["; for (unsigned n = next(4); n; n--) s += randomJson(depth + 1) + (next(9) ? "," : " "); return s + "]"; }
    default: { std::string s = "{"; for (unsigned n = next(4); n; n--) s += std::string(next(3), ' ') + "\"k" + std::to_string(next(9)) + "\" : " + randomJson(depth + 1) + (next(9) ? ", " : "\n"); return s + (next(9) ? "}" : ""); }
    }
}
static void* randomValue(int depth) {
    switch (next(depth > 4 ? 6 : 8)) {
    case 0: return WKFakeUInt64(uint64_t(next(32768)) * next(32768) * next(32768) * next(3)); case 1: return WKBooleanCreate(next(2));
    case 2: return WKDoubleCreate(next(9) ? next(10000) / 3.0 * std::pow(10, int(next(30)) - 15) : next(2) ? NAN : INFINITY);
    case 3: return WKURLCreateWithUTF8CString("https://example.org/\"a\"");
    case 4: case 5: { std::string s; for (unsigned n = next(40); n; n--) s += char(1 + next(255)); return WKStringCreateWithUTF8CString(s.c_str()); }
    case 6: { void* a = WKMutableArrayCreate(); for (unsigned n = next(4); n; n--) WKArrayAppendItem(a, next(9) ? randomValue(depth + 1) : nullptr); return a; }
    default: { void* d = WKMutableDictionaryCreate(); for (unsigned n = next(4); n; n--) WKDictionarySetItem(d, WKStringCreateWithUTF8CString(("k" + std::to_string(n)).c_str()), randomValue(depth + 1)); return d; }
    }
}
int main(int, char** argv) {
    void* h = dlopen(argv[1], RTLD_NOW);
    if (!h) { puts(dlerror()); return 1; }
    auto get = reinterpret_cast<void** (*)(int)>(dlsym(h, "reef_webkit_host_interface"));
    printf("%d %d %d\n", get(0) == nullptr, get(2) == nullptr, get(-1) == nullptr);
    table = get(1);
    printf("version %d\n", int(reinterpret_cast<uintptr_t>(table[0])));
    for (const char* id : {"", "single", "org.example", "1a.b", "a.1b", "a..b", "a.b.", "a.b c", "org.ex-am_ple.X9", ".a.b"}) {
        g_set_prgname(nullptr); call(84, id); const char* n = g_get_prgname(); printf("id <%s> %s\n", id, n ? n : "-"); }
    { std::string big(255, 'a'); big[3] = '.'; g_set_prgname(nullptr); call(84, big.c_str()); printf("%d\n", g_get_prgname() != nullptr);
      big += 'a'; g_set_prgname(nullptr); call(84, big.c_str()); printf("%d\n", g_get_prgname() != nullptr); }
    call(83, 0, static_cast<const char*>("/some/sandbox/path/that/is/long"));
    printf("available %d\n", int(reinterpret_cast<uintptr_t>(call(1))) & 1);
    printf("%g %d\n", reinterpret_cast<double (*)()>(table[3])(), int(reinterpret_cast<intptr_t>(call(2))));
    for (int i = 0; i < 3000; i++) { const std::string s = randomJson(0); printf("json %s\n", s.c_str()); call(10, reinterpret_cast<void*>(1), s.c_str(), static_cast<const char*>(nullptr)); }
    for (const char* s : {"", "nul", "{", "[", "\"", "{\"a\"", "{\"a\":", "{a:1}", "[1,,2]", "-", "1e999", "\"\\", "\"\\u", "0x10", "nan"}) { printf("json %s\n", s); call(10, reinterpret_cast<void*>(1), s, static_cast<const char*>(nullptr)); }
    call(10, reinterpret_cast<void*>(1), "1", "error");
    void* content = call(4);
    call(8, content, "name", message, static_cast<void*>(nullptr));
    call(86, content, "a longer name than fits", "world", message, static_cast<void*>(nullptr));
    auto received = reinterpret_cast<void (*)(void*, void*, void*)>(WKFakeCallback("message"));
    for (int i = 0; i < 3000; i++) received(randomValue(0), nullptr, WKFakeCallback("context"));
    call(5, content);
    struct { void* view; } view = {nullptr}; char space[0x118] = {}; (void)view;
    for (int k = 0; k < 5; k++) for (int b = 0; b < 6; b++) for (unsigned m = 0; m < 64; m += 7) {
        struct { int kind, button; unsigned buttons; int clicks; unsigned modifiers; double x, y, time; } e = {k, b, m & 7, int(m % 3), m, 1.5 * k, 2.5 * b, 3};
        call(41, static_cast<void*>(space), &e); }
    for (int p = -1; p < 7; p++) for (int q = -1; q < 7; q++) {
        struct { double v[6]; int phase, momentum, precise; unsigned modifiers; double time; } e = {{1, 2, 3, 4, 5, 6}, p, q, p & 1, unsigned(q * 9), 4};
        call(42, static_cast<void*>(space), &e); }
    for (int d = 0; d < 3; d++) for (const char* t : {static_cast<const char*>(nullptr), "", "x"}) {
        struct { int down; unsigned sym, code, modifiers; int repeat; const char* text; double time; } e = {d, 0x61, 38, 0xffu, d, t, 5};
        call(43, static_cast<void*>(space), &e); }
    for (unsigned f = 0; f < 16; f++) call(67, static_cast<void*>(space), "text", f);
    call(67, static_cast<void*>(space), "", 1u);
    void* store[1] = {nullptr};
    for (unsigned k = 0; k < 4096; k += 37) { call(89, static_cast<void*>(store), k, records, static_cast<void*>(nullptr)); call(90, static_cast<void*>(store), k, static_cast<void*>(nullptr), int(k % 5) - 1, static_cast<void*>(nullptr), static_cast<void*>(nullptr)); }
    struct Screen { unsigned id; double v[8]; int depth, per; double dpi; } screens[3] = {{1, {0, 0, 10, 20, 0, 1, 10, 19}, 24, 8, 96}, {2, {1, 2, 3, 4, 5, 6, 7, 8}, 30, 10, 192}, {3, {}, 0, 0, 0}};
    for (int n = -1; n <= 3; n++) call(98, screens, n, 2u);
}
