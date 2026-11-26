// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libcharset: the name of the character set.
#include <stdio.h>
const char *locale_charset(void);
void libcharset_set_relocation_prefix(const char *, const char *);
int main(void)
{
    libcharset_set_relocation_prefix("/a", "/b");
    printf("%s\n", locale_charset());
    return 0;
}
