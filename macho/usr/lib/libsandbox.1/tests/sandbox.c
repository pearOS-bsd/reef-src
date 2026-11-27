// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libsandbox: the parameter list, compiling a profile from a string and from a file (also a long one and a missing one), named profiles,
// and applying nothing.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char **items; size_t count, capacity; } Params;
Params *sandbox_create_params(void);
int sandbox_set_param(Params *, const char *, const char *);
void sandbox_free_params(Params *);
void *sandbox_compile_string(const char *, Params *, char **);
void *sandbox_compile_file(const char *, Params *, char **);
void *sandbox_compile_named(const char *, Params *, char **);
void sandbox_free_profile(void *);
int sandbox_apply(void *);

static void report(const char *what, void *profile, char *error)
{
    printf("%s: %s, error %s\n", what, profile ? "profile" : "none", error ? error : "(none)");
    if (profile) sandbox_free_profile(profile);
    free(error);
}

int main(void)
{
    Params *p = sandbox_create_params();
    printf("params %d\n", p != 0);
    printf("set %d %d %d\n", sandbox_set_param(p, "HOME", "/Users/x"), sandbox_set_param(0, "a", "b"), sandbox_set_param(p, 0, "b"));
    for (int i = 0; i < 20; i++) {
        char k[16], v[16];
        sprintf(k, "key%d", i); sprintf(v, "value%d", i);
        sandbox_set_param(p, k, v);
    }
    printf("count %zu capacity %zu last %s=%s end %d\n", p->count, p->capacity, p->items[p->count - 2], p->items[p->count - 1], p->items[p->count] == 0);

    char *error = (char *)1;
    void *profile = sandbox_compile_string("(version 1)\n(allow default)\n", p, &error);
    report("string", profile, error);
    error = 0;
    profile = sandbox_compile_string("(version 1)\n(allow default)\n", 0, 0);
    report("string without params", profile, 0);
    printf("apply nothing %d\n", sandbox_apply(0));

    FILE *f = fopen("/tmp/reef-sandbox-test.sb", "w");
    fputs("(version 1)\n(allow default)\n", f);
    fclose(f);
    error = 0;
    report("file", sandbox_compile_file("/tmp/reef-sandbox-test.sb", p, &error), error);
    f = fopen("/tmp/reef-sandbox-long.sb", "w");
    fputs("(version 1)\n(allow default)\n", f);
    for (int i = 0; i < 2000; i++) fprintf(f, ";; padding line number %d to make the file longer than one buffer\n", i);
    fclose(f);
    error = 0;
    report("long file", sandbox_compile_file("/tmp/reef-sandbox-long.sb", 0, &error), error);
    error = 0;
    report("missing file", sandbox_compile_file("/tmp/does-not-exist.sb", p, &error), error);
    report("null path", sandbox_compile_file(0, p, 0), 0);
    error = 0;
    report("named", sandbox_compile_named("pure-computation", p, &error), error);
    sandbox_free_profile(0);
    sandbox_free_params(p);
    sandbox_free_params(0);
    return 0;
}
