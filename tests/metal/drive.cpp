// drive ORIGINAL [OURS] [-o] [-n] [-l LINKSOURCE] FILE...: the C interface of a translator library on Metal sources (.metal) and
// libraries (.metallib), everything it says printed as text. With OURS: ORIGINAL is loaded for the trampolines of OURS
// (reef_patch) and OURS is driven. -o: also translate with non-default options. -n: also the odd and null arguments.
// -l FILE: a source whose visible functions are linked into every translation of the following files.
#include <dlfcn.h>
#include <unistd.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct SourceOptions { int languageVersion, fastMath, macroCount, reserved; const char *const *names, *const *values; };
struct Value { int index, type; const char *name; const void *data; };
struct Length { int index, length; };
struct TranslateOptions { int first[4]; int size[3]; int valueCount; const Value *values; int lengthCount; int flags; const Length *lengths; };

#define API(r, n, ...) r (*n)(__VA_ARGS__)
static API(int, translator_version);
static API(void *, library_create_from_data, const void *, long, char *, long);
static API(void *, library_create_from_source, const char *, long, const SourceOptions *, char *, long);
static API(void, library_release, void *);
static API(int, library_function_count, void *);
static API(const char *, library_function_name, void *, int);
static API(int, library_function_stage, void *, int);
static API(void *, function_create, void *, const char *, char *, long);
static API(void, function_release, void *);
static API(const char *, function_name, void *);
static API(int, function_stage, void *);
static API(int, function_attribute_count, void *);
static API(const void *, function_attribute, void *, int);
static API(int, function_argument_buffer, void *, int, int *, int *);
static API(const void *, function_argument_member, void *, int, int);
static API(int, function_constant_count, void *);
static API(const void *, function_constant_at, void *, int);
static API(void *, function_translate, void *, const TranslateOptions *, char *, long);
static API(void *, function_translate_linked, void *, void *const *, int, const TranslateOptions *, char *, long);
static API(void, shader_release, void *);
static API(int, shader_stage, void *);
static API(const char *, shader_glsl, void *);
static API(uint64_t, shader_flags, void *);
static API(int, shader_binding_count, void *);
static API(const void *, shader_binding, void *, int);
static API(int, shader_constant_sampler_count, void *);
static API(const void *, shader_constant_sampler, void *, int);
static API(int, shader_attribute_count, void *);
static API(const void *, shader_attribute, void *, int);
static API(int, shader_varying_count, void *);
static API(const void *, shader_varying, void *, int);
static API(int, shader_render_target_count, void *);
static API(const void *, shader_render_target, void *, int);
static API(int, shader_clip_distance_count, void *);
static API(int, shader_uniform_location, void *, int);
static API(void, shader_threadgroup_size, void *, int *, int *, int *);

static void load(void *h)
{
#define L(n) n = (decltype(n))dlsym(h, "reef_mtl_" #n); if (!n) { printf("missing reef_mtl_%s\n", #n); exit(1); }
    L(translator_version) L(library_create_from_data) L(library_create_from_source) L(library_release) L(library_function_count)
    L(library_function_name) L(library_function_stage) L(function_create) L(function_release) L(function_name) L(function_stage)
    L(function_attribute_count) L(function_attribute) L(function_argument_buffer) L(function_argument_member)
    L(function_constant_count) L(function_constant_at) L(function_translate) L(function_translate_linked) L(shader_release)
    L(shader_stage) L(shader_glsl) L(shader_flags) L(shader_binding_count) L(shader_binding) L(shader_constant_sampler_count)
    L(shader_constant_sampler) L(shader_attribute_count) L(shader_attribute) L(shader_varying_count) L(shader_varying)
    L(shader_render_target_count) L(shader_render_target) L(shader_clip_distance_count) L(shader_uniform_location)
    L(shader_threadgroup_size)
}

// The bytes of a structure: ints, and strings at the given offsets.
static void fields(const char *what, const void *p, size_t n, int s1 = -1, int s2 = -1)
{
    if (!p) { printf("  %s: (null)\n", what); return; }
    const unsigned char *b = (const unsigned char *)p;
    printf("  %s:", what);
    for (size_t i = 0; i < n;) {
        if ((int)i == s1 || (int)i == s2) {
            const char *s;
            memcpy(&s, b + i, 8);
            printf(" \"%s\"", s ? s : "(null)");
            i += 8;
        } else {
            int v;
            memcpy(&v, b + i, 4);
            printf(" %d", v);
            i += 4;
        }
    }
    printf("\n");
}

static char error[4096];

// The error buffer after a call: its text, and whether the bytes after the NUL are untouched.
static const char *err()
{
    static std::string text;
    text = error;
    size_t n = strlen(error);
    for (size_t i = n + 1; i < sizeof error; ++i)
        if (error[i] != 'X') { text += " [written past the NUL]"; break; }
    memset(error, 'X', sizeof error);
    error[0] = 0;
    return text.c_str();
}

static void shader(void *sh)
{
    printf("%s", shader_glsl(sh));
    int x = -7, y = -7, z = -7;
    shader_threadgroup_size(sh, &x, &y, &z);
    printf("  stage %d flags %#llx clip %d uniforms %d %d %d %d size %d %d %d\n", shader_stage(sh), (unsigned long long)shader_flags(sh),
           shader_clip_distance_count(sh), shader_uniform_location(sh, 0), shader_uniform_location(sh, 1),
           shader_uniform_location(sh, 2), shader_uniform_location(sh, -1), x, y, z);
    int n = shader_binding_count(sh);
    for (int b = 0; b < n; ++b) fields("binding", shader_binding(sh, b), 88, 0x30, 0x38);
    fields("binding past", shader_binding(sh, n), 4);
    n = shader_constant_sampler_count(sh);
    for (int b = 0; b < n; ++b) fields("sampler", shader_constant_sampler(sh, b), 56);
    fields("sampler past", shader_constant_sampler(sh, n), 4);
    n = shader_attribute_count(sh);
    for (int b = 0; b < n; ++b) fields("attribute", shader_attribute(sh, b), 32, 0x18);
    fields("attribute past", shader_attribute(sh, n), 4);
    n = shader_varying_count(sh);
    for (int b = 0; b < n; ++b) fields("varying", shader_varying(sh, b), 32, 0x10, 0x18);
    fields("varying past", shader_varying(sh, n), 4);
    n = shader_render_target_count(sh);
    for (int b = 0; b < n; ++b) fields("target", shader_render_target(sh, b), 8);
    fields("target past", shader_render_target(sh, n), 4);
}

static std::vector<void *> linkedFunctions;

static void function(void *lib, const char *name, bool options)
{
    void *fn = function_create(lib, name, error, sizeof error);
    printf("-- %s: %s\n", name, err());
    if (!fn) return;
    printf("  name %s stage %d\n", function_name(fn), function_stage(fn));
    int n = function_attribute_count(fn);
    for (int i = 0; i < n; ++i) fields("attribute", function_attribute(fn, i), 32, 0x18);
    fields("attribute past", function_attribute(fn, n), 4);
    fields("attribute -1", function_attribute(fn, -1), 4);
    n = function_constant_count(fn);
    for (int i = 0; i < n; ++i) fields("constant", function_constant_at(fn, i), 24, 0x10);
    fields("constant past", function_constant_at(fn, n), 4);
    for (int b = -1; b < 32; ++b) {
        int size = -9, alignment = -9;
        int count = function_argument_buffer(fn, b, &size, &alignment);
        if (!count && !size && !alignment && !function_argument_member(fn, b, 0)) continue;
        printf("  argument buffer %d: %d members, size %d alignment %d\n", b, count, size, alignment);
        for (int m = 0; m <= count; ++m) fields("member", function_argument_member(fn, b, m), 64, 0x30, 0x38);
        fields("member -1", function_argument_member(fn, b, -1), 4);
        function_argument_buffer(fn, b, nullptr, nullptr);
    }
    // Values for function constants 0..3 (true, true, false, 5) and two threadgroup lengths.
    static const unsigned char yes = 1, no = 0;
    static const int five = 5;
    static const Value values[] = {{0, 53, nullptr, &yes}, {1, 53, "b", &yes}, {2, 53, nullptr, &no}, {3, 29, nullptr, &five}};
    static const Length lengths[] = {{0, 256}, {1, 64}, {0, 128}};
    const TranslateOptions plain = {{0, 0, 0, 0}, {1, 1, 1}, 4, values, 0, 0, nullptr};
    const TranslateOptions other = {{1, 2, 3, 4}, {8, -4, 0}, 4, values, 3, 1, lengths};
    for (int pass = 0; pass < (options ? 3 : 1); ++pass) {
        const TranslateOptions *o = pass == 0 ? &plain : pass == 1 ? &other : nullptr;
        void *sh = linkedFunctions.empty()
                       ? function_translate(fn, o, error, sizeof error)
                       : function_translate_linked(fn, linkedFunctions.data(), linkedFunctions.size(), o, error, sizeof error);
        printf("  translate %d: %s\n", pass, err());
        if (!sh) continue;
        shader(sh);
        // Once more: the same translation is shared while it is alive.
        void *again = linkedFunctions.empty()
                          ? function_translate(fn, o, error, sizeof error)
                          : function_translate_linked(fn, linkedFunctions.data(), linkedFunctions.size(), o, error, sizeof error);
        printf("  again: %s %s\n", again && shader_glsl(again) == shader_glsl(sh) ? "shared" : "not shared", err());
        shader_release(again);
        shader_release(sh);
    }
    function_release(fn);
}

static void library(void *lib, bool options)
{
    int n = library_function_count(lib);
    printf("  %d functions\n", n);
    for (int k = 0; k < n; ++k)
        printf("  %d %s %d\n", k, library_function_name(lib, k), library_function_stage(lib, k));
    printf("  past %s %d, -1 %s %d\n", library_function_name(lib, n) ? "?" : "(null)", library_function_stage(lib, n),
           library_function_name(lib, -1) ? "?" : "(null)", library_function_stage(lib, -1));
    for (int k = 0; k < n; ++k)
        if ((!getenv("FUNC") || strstr((std::string(",") + getenv("FUNC") + ",").c_str(),
                                       (std::string(",") + library_function_name(lib, k) + ",").c_str())) &&
            (!getenv("SKIPFUNC") || !strstr((std::string(",") + getenv("SKIPFUNC") + ",").c_str(),
                                            (std::string(",") + library_function_name(lib, k) + ",").c_str())))
            function(lib, library_function_name(lib, k), options);
}

static std::string readFile(const char *path)
{
    std::ifstream f(path, std::ios::binary);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// Null and odd arguments everywhere.
static void odd(void *anyLibrary, const char *anyName)
{
    printf("==== odd arguments\n");
    printf("version %d\n", translator_version());
    printf("null library: %d %p %d\n", library_function_count(nullptr), (void *)library_function_name(nullptr, 0),
           library_function_stage(nullptr, 0));
    library_release(nullptr);
    function_release(nullptr);
    shader_release(nullptr);
    void *f = function_create(nullptr, "x", error, sizeof error);
    printf("create null library: %p %s\n", f, err());
    f = function_create(anyLibrary, nullptr, error, sizeof error);
    printf("create null name: %p %s\n", f, err());
    f = function_create(anyLibrary, "no_such_function", error, sizeof error);
    printf("create unknown: %p %s\n", f, err());
    f = function_create(anyLibrary, "", error, sizeof error);
    printf("create empty name: %p %s\n", f, err());
    for (long size : {0L, 1L, 2L, 5L, 21L, 22L, -3L}) {
        f = function_create(nullptr, "x", size == -3 ? nullptr : error, size == -3 ? 10 : size);
        printf("error buffer %ld: %s\n", size, err());
    }
    printf("null function: %p %d %d %p %d %p %d %p\n", (void *)function_name(nullptr), function_stage(nullptr),
           function_attribute_count(nullptr), function_attribute(nullptr, 0), function_constant_count(nullptr),
           function_constant_at(nullptr, 0), function_argument_buffer(nullptr, 0, nullptr, nullptr),
           function_argument_member(nullptr, 0, 0));
    int size = 5, alignment = 5;
    function_argument_buffer(nullptr, 0, &size, &alignment);
    printf("null function argument buffer: %d %d\n", size, alignment);
    void *sh = function_translate(nullptr, nullptr, error, sizeof error);
    printf("translate null: %p %s\n", sh, err());
    printf("null shader: %p %llu %d %p %d %p %d %p %d %p %d %p %d %d %d\n", (void *)shader_glsl(nullptr),
           (unsigned long long)shader_flags(nullptr), shader_binding_count(nullptr), shader_binding(nullptr, 0),
           shader_constant_sampler_count(nullptr), shader_constant_sampler(nullptr, 0), shader_attribute_count(nullptr),
           shader_attribute(nullptr, 0), shader_varying_count(nullptr), shader_varying(nullptr, 0),
           shader_render_target_count(nullptr), shader_render_target(nullptr, 0), shader_clip_distance_count(nullptr),
           shader_uniform_location(nullptr, 0), shader_stage(nullptr));
    int x = 9, y = 9, z = 9;
    shader_threadgroup_size(nullptr, &x, &y, &z);
    shader_threadgroup_size(nullptr, nullptr, nullptr, nullptr);
    printf("null shader size: %d %d %d\n", x, y, z);
    // Libraries from data.
    void *lib = library_create_from_data(nullptr, 10, error, sizeof error);
    printf("data null: %p %s\n", lib, err());
    lib = library_create_from_data("abc", 0, error, sizeof error);
    printf("data empty: %p %s\n", lib, err());
    lib = library_create_from_data("abc", -1, error, sizeof error);
    printf("data negative: %p %s\n", lib, err());
    lib = library_create_from_data("not a metal library at all", 26, error, sizeof error);
    printf("data garbage: %p %s\n", lib, err());
    lib = library_create_from_data("MTLB", 4, error, sizeof error);
    printf("data magic only: %p %s\n", lib, err());
    lib = library_create_from_data("MTLB", 4, error, 6);
    printf("data magic only, buffer 6: %p %s\n", lib, err());
    // Libraries from source.
    lib = library_create_from_source(nullptr, -1, nullptr, error, sizeof error);
    printf("source null: %s %s\n", lib ? "library" : "(null)", err());
    if (lib) { printf("  %d functions\n", library_function_count(lib)); library_release(lib); }
    lib = library_create_from_source("kernel void k() {}\nkernel void q() {}", 18, nullptr, error, sizeof error);
    printf("source cut: %s %s\n", lib ? "library" : "(null)", err());
    if (lib) { library(lib, false); library_release(lib); }
    lib = library_create_from_source("kernel void k() { int unused = 1; }", 0, nullptr, error, sizeof error);
    printf("source length 0: %s %s\n", lib ? "library" : "(null)", err());
    if (lib) { printf("  %d functions\n", library_function_count(lib)); library_release(lib); }
    lib = library_create_from_source("kernel void k() { int unused = 1; }", -1, nullptr, error, 30);
    printf("source warning, buffer 30: %s %s\n", lib ? "library" : "(null)", err());
    if (lib) library_release(lib);
    lib = library_create_from_source("kernel void k( {", -1, nullptr, error, sizeof error);
    printf("source error: %s %s\n", lib ? "library" : "(null)", err());
    const char *names[] = {"A", nullptr, "B", "C"};
    const char *values[] = {"3", "x", nullptr, ""};
    const SourceOptions so[] = {{0x20004, 0, 4, 0, names, values}, {0x10001, 1, 4, 0, names, nullptr}, {0, 1, 4, 0, nullptr, values},
                                {0x30000, 7, -2, 0, names, values}};
    for (const SourceOptions &o : so) {
        lib = library_create_from_source("kernel void k(device int *a [[buffer(0)]]) { a[0] = A + B + 1; }", -1, &o, error, sizeof error);
        printf("source options %#x %d %d: %s %s\n", o.languageVersion, o.fastMath, o.macroCount, lib ? "library" : "(null)", err());
        if (lib) { library(lib, false); library_release(lib); }
    }
    // Translation options that are wrong.
    lib = library_create_from_source(
        "constant bool b [[function_constant(0)]];\nconstant int n [[function_constant(1)]];\n"
        "kernel void k(device int *a [[buffer(0)]]) { a[0] = b ? n : 2; }\n"
        "[[visible]] int twice(int x) { return x * 2; }\n"
        "vertex float4 v() { return float4(0); }\n", -1, nullptr, error, sizeof error);
    printf("source constants: %s %s\n", lib ? "library" : "(null)", err());
    if (!lib) return;
    library(lib, false);
    void *k = function_create(lib, "k", error, sizeof error);
    void *visible = function_create(lib, "twice", error, sizeof error);
    void *vertex = function_create(lib, "v", error, sizeof error);
    static const unsigned char yes = 1;
    static const int seven = 7;
    const Value bad[][2] = {{{0, 0, nullptr, &yes}, {1, 29, nullptr, &seven}}, {{0, 53, nullptr, nullptr}, {1, 29, nullptr, &seven}},
                            {{0, 53, "b", &yes}, {1, 999, nullptr, &seven}}, {{-1, 53, "b", &yes}, {-1, 29, "n", &seven}},
                            {{5, 53, nullptr, &yes}, {1, 29, nullptr, &seven}}};
    for (const auto &v : bad) {
        const TranslateOptions o = {{0, 0, 0, 0}, {0, 0, 0}, 2, v, 0, 0, nullptr};
        sh = function_translate(k, &o, error, sizeof error);
        printf("values %d/%d %d/%d: %s %s\n", v[0].index, v[0].type, v[1].index, v[1].type, sh ? "shader" : "(null)", err());
        if (sh) shader(sh);
        shader_release(sh);
    }
    const TranslateOptions noValues = {{0, 0, 0, 0}, {1, 1, 1}, 2, nullptr, 2, 0, nullptr};
    sh = function_translate(k, &noValues, error, sizeof error);
    printf("counts without arrays: %s %s\n", sh ? "shader" : "(null)", err());
    if (sh) shader(sh);
    shader_release(sh);
    sh = function_translate(visible, nullptr, error, sizeof error);
    printf("translate visible: %s %s\n", sh ? "shader" : "(null)", err());
    shader_release(sh);
    void *list[] = {visible, nullptr};
    sh = function_translate_linked(k, nullptr, 1, nullptr, error, sizeof error);
    printf("linked null list: %s %s\n", sh ? "shader" : "(null)", err());
    shader_release(sh);
    sh = function_translate_linked(k, list, 2, nullptr, error, sizeof error);
    printf("linked null entry: %s %s\n", sh ? "shader" : "(null)", err());
    shader_release(sh);
    list[1] = vertex;
    sh = function_translate_linked(k, list, 2, nullptr, error, sizeof error);
    printf("linked vertex: %s %s\n", sh ? "shader" : "(null)", err());
    shader_release(sh);
    sh = function_translate_linked(k, list, -1, nullptr, error, sizeof error);
    printf("linked count -1: %s %s\n", sh ? "shader" : "(null)", err());
    shader_release(sh);
    sh = function_translate_linked(k, list, 1, nullptr, error, sizeof error);
    printf("linked unused visible: %s %s\n", sh ? "shader" : "(null)", err());
    if (sh) shader(sh);
    shader_release(sh);
    // The same with the constants given: linked functions that the kernel does not call, short error buffers.
    const Value good[] = {{0, 53, nullptr, &yes}, {1, 29, nullptr, &seven}};
    const TranslateOptions withValues = {{0, 0, 0, 0}, {1, 1, 1}, 2, good, 0, 0, nullptr};
    list[1] = visible;
    sh = function_translate_linked(k, list, 2, &withValues, error, sizeof error);
    printf("linked unused visible, values: %s %s\n", sh ? "shader" : "(null)", err());
    if (sh) shader(sh);
    shader_release(sh);
    for (long size : {1L, 2L, 3L, 10L}) {
        sh = function_translate(vertex, &withValues, error, size);
        printf("translate error buffer %ld: %s %s\n", size, sh ? "shader" : "(null)", err());
        shader_release(sh);
        sh = function_translate(visible, &withValues, error, size);
        printf("visible error buffer %ld: %s %s\n", size, sh ? "shader" : "(null)", err());
        shader_release(sh);
    }
    sh = function_translate(k, nullptr, error, 0);
    printf("translate buffer 0: %s %s\n", sh ? "shader" : "(null)", err());
    if (sh) printf("  same as with options: %d\n", strlen(shader_glsl(sh)) > 0);
    shader_release(sh);
    function_release(k);
    function_release(visible);
    function_release(vertex);
    library_release(lib);
    (void)anyName;
}

int main(int argc, char **argv)
{
    memset(error, 'X', sizeof error);
    error[0] = 0;
    void *h = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (!h) { printf("%s\n", dlerror()); return 1; }
    int i = 2;
    if (argc > 2 && argv[2][0] != '-' && strstr(argv[2], ".so")) {
        void *p = dlopen(argv[2], RTLD_NOW | RTLD_LOCAL);
        if (!p) { printf("%s\n", dlerror()); return 1; }
        ((void (*)(void *))dlsym(p, "reef_patch"))(dlsym(h, "reef_mtl_translator_version"));
        h = p;
        i = 3;
    }
    load(h);
    bool options = false;
    void *anyLibrary = nullptr;
    for (; i < argc; ++i) {
        if (!strcmp(argv[i], "-o")) { options = true; continue; }
        if (!strcmp(argv[i], "-n")) {
            void *lib = library_create_from_source("kernel void k() {}", -1, nullptr, error, sizeof error);
            err();
            odd(lib, "k");
            library_release(lib);
            continue;
        }
        if (!strcmp(argv[i], "-l")) {
            std::string src = readFile(argv[++i]);
            void *lib = library_create_from_source(src.c_str(), src.size(), nullptr, error, sizeof error);
            printf("==== linked %s: %s\n", argv[i], err());
            for (int k = 0; lib && k < library_function_count(lib); ++k)
                if (library_function_stage(lib, k) == 5)
                    linkedFunctions.push_back(function_create(lib, library_function_name(lib, k), error, sizeof error));
            printf("  %zu visible functions\n", linkedFunctions.size());
            continue;
        }
        std::string data = readFile(argv[i]);
        const char *base = strrchr(argv[i], '/');
        printf("==== %s\n", base ? base + 1 : argv[i]);
        // MSLVERSION=0x20004: compile with that language version (the default lacks indirect command buffers).
        const SourceOptions version = {getenv("MSLVERSION") ? (int)strtol(getenv("MSLVERSION"), nullptr, 0) : 0, 1, 0, 0, nullptr, nullptr};
        void *lib = strstr(argv[i], ".metallib")
                        ? library_create_from_data(data.data(), data.size(), error, sizeof error)
                        : library_create_from_source(data.c_str(), data.size(), getenv("MSLVERSION") ? &version : nullptr, error, sizeof error);
        printf("  library: %s %s\n", lib ? "yes" : "no", err());
        if (!lib) continue;
        library(lib, options);
        library_release(lib);
    }
    fflush(stdout);
    if (getenv("EXIT"))
        exit(0);    // runs the exit handlers: gcov, leak checks
    _exit(0);
}
