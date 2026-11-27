// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// SkyLight: there is no display link.
#include <stdio.h>

void *SLSGetDisplayLink(unsigned);

int main(void)
{
    printf("%p %p\n", SLSGetDisplayLink(0), SLSGetDisplayLink(1));
    return 0;
}
