// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libpcre.0: compile, match, captures, names, UTF-8 properties, DFA, JIT. The output must be the same on a Mac and under reef.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef struct real_pcre pcre;
typedef struct pcre_extra pcre_extra;
static pcre *(*pcre_compile)(const char *, int, const char **, int *, const unsigned char *);
static int (*pcre_exec)(const pcre *, const pcre_extra *, const char *, int, int, int, int *, int);
static int (*pcre_dfa_exec)(const pcre *, const pcre_extra *, const char *, int, int, int, int *, int, int *, int);
static int (*pcre_fullinfo)(const pcre *, const pcre_extra *, int, void *);
static pcre_extra *(*pcre_study)(const pcre *, int, const char **);
static int (*pcre_get_named_substring)(const pcre *, const char *, const int *, int, const char *, const char **);
static int (*pcre_get_stringnumber)(const pcre *, const char *);
static int (*pcre_config)(int, void *);
static const char *(*pcre_version)(void);
static void (*pcre_free_substring)(const char *);

#define SYM(n) do { *(void **)&n = dlsym(h, #n); if (!n) { printf("no %s\n", #n); return 1; } } while (0)

static void show(const char *pattern, int options, const char *subject)
{
    const char *err; int erroff;
    pcre *re = pcre_compile(pattern, options, &err, &erroff, 0);
    if (!re) { printf("/%s/ error '%s' at %d\n", pattern, err, erroff); return; }
    int ov[60];
    int rc = pcre_exec(re, 0, subject, (int)strlen(subject), 0, 0, ov, 60);
    printf("/%s/ on '%s': %d", pattern, subject, rc);
    for (int i = 0; i < rc; i++) printf(" [%d,%d]", ov[2*i], ov[2*i+1]);
    printf("\n");
}

int main(int argc, char **argv)
{
    void *h = dlopen(argc > 1 ? argv[1] : "/usr/lib/libpcre.0.dylib", RTLD_NOW);
    if (!h) { printf("dlopen failed\n"); return 1; }
    SYM(pcre_compile); SYM(pcre_exec); SYM(pcre_dfa_exec); SYM(pcre_fullinfo); SYM(pcre_study);
    SYM(pcre_get_named_substring); SYM(pcre_get_stringnumber); SYM(pcre_config); SYM(pcre_version); SYM(pcre_free_substring);
    int v;
    printf("version %s\n", pcre_version());
    pcre_config(0, &v); printf("utf8 %d\n", v);
    pcre_config(6, &v); printf("unicode properties %d\n", v);
    pcre_config(9, &v); printf("jit %d\n", v);
    pcre_config(1, &v); printf("newline %d\n", v);
    pcre_config(2, &v); printf("link size %d\n", v);
    pcre_config(4, &v); printf("match limit %d\n", v);
    show("(\\d+)-(\\d+)", 0, "tel 555-1234 end");
    show("^(?<year>\\d{4})-(?<mon>\\d\\d)$", 0, "2026-11");
    show("a(b|c)*d", 0, "xxabcbcdyy");
    show("(unclosed", 0, "x");
    show("\\bfoo\\b", 1, "a FOO b");          /* PCRE_CASELESS */
    show("\\p{Lu}\\p{Ll}+", 0x800 /* UTF8 */ | 0x20000000 /* UCP */, "x \xc3\x89" "cole y");
    show("^\\w+$", 0x800 | 0x20000000, "caf\xc3\xa9");
    show("(?i)stra\\xdfe", 0, "STRA\xdf" "E");
    show("(a+)+b", 0, "aaaaaaaaaaaaaaaaaaaaaaaaaaaac");
    show("(?s).+", 0, "line1\nline2");
    show("^.+$", 0x4 /* MULTILINE */, "one\ntwo");
    {
        const char *err; int off;
        pcre *re = pcre_compile("(?<first>\\w+) (?<second>\\w+)", 0, &err, &off, 0);
        int ov[30], rc = pcre_exec(re, 0, "hello world", 11, 0, 0, ov, 30);
        const char *s; int n = pcre_get_named_substring(re, "hello world", ov, rc, "second", &s) ;
        (void)n; printf("named second = '%s' number %d\n", s, pcre_get_stringnumber(re, "second"));
        pcre_free_substring(s);
        int caps = -99; size_t size = 0; pcre_fullinfo(re, 0, 2 /* CAPTURECOUNT */, &caps); pcre_fullinfo(re, 0, 1 /* SIZE */, &size);
        printf("captures %d size %d\n", caps, (int)size);
        const char *serr; pcre_extra *ex = pcre_study(re, 0x0001 /* JIT_COMPILE */, &serr);
        int jit = 0; if (ex) pcre_fullinfo(re, ex, 16 /* JIT */, &jit);
        printf("study %s jit %d\n", ex ? "ok" : "none", jit);
        rc = pcre_exec(re, ex, "jit run here", 12, 0, 0, ov, 30);
        printf("jit exec %d [%d,%d]\n", rc, ov[0], ov[1]);
        int ws[40]; rc = pcre_dfa_exec(re, 0, "ab cd", 5, 0, 0, ov, 30, ws, 40);
        printf("dfa %d [%d,%d]\n", rc, ov[0], ov[1]);
    }
    return 0;
}
