// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libedit.3: the readline interface that scripting languages use: history in memory and in a file, the version, tilde expansion. The output must be the same on a Mac and under reef.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void using_history(void);
void add_history(const char *);
void clear_history(void);
int write_history(const char *);
int read_history(const char *);
char *tilde_expand(char *);
int main(void)
{
    char path[64];
    snprintf(path, sizeof path, "/tmp/reef-edit-test-%d", (int)getpid());
    using_history();
    add_history("first line");
    add_history("second line");
    add_history("third line");
    printf("write %d\n", write_history(path));
    clear_history();
    printf("read %d\n", read_history(path));
    FILE *f = fopen(path, "r"); char line[128]; int n = 0;
    while (f && fgets(line, sizeof line, f)) { if (n++ < 6 && line[0] != '_') printf("file: %s", line); }
    if (f) fclose(f);
    unlink(path);
    char *t = tilde_expand("plain/path");
    printf("tilde %s\n", t);
    return 0;
}
