// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libncurses.5.4: the termcap interface (tgetent, tgetnum, tgetstr, tgetflag) on the xterm entry. The output must be the same on a Mac and under reef.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int tgetent(char *, const char *);
int tgetnum(const char *);
int tgetflag(const char *);
char *tgetstr(const char *, char **);
static void show(const char *id, const char *s)
{
    printf("%s =", id);
    if (!s) { printf(" (none)\n"); return; }
    for (; *s; s++) printf(*s >= 32 && *s < 127 ? " %c" : " \\%03o", (unsigned char)*s);
    printf("\n");
}
int main(void)
{
    char buf[2048], area[2048], *p = area;
    printf("tgetent xterm: %d\n", tgetent(buf, "xterm"));
    printf("co %d, li %d, pa %d, am %d, bs %d\n", tgetnum("co"), tgetnum("li"), tgetnum("pa"), tgetflag("am"), tgetflag("bs"));
    show("cl", tgetstr("cl", &p));
    show("cm", tgetstr("cm", &p));
    show("cd", tgetstr("cd", &p));
    show("nonexistent", tgetstr("zz", &p));
    printf("tgetent nonexistent-terminal-xyz: %d\n", tgetent(buf, "nonexistent-terminal-xyz"));
    return 0;
}
