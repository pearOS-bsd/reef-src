// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#define IOCPARM_MASK 0x1fff
#define IOC_OUT 0x40000000
#define IOC_IN 0x80000000
#define _IOC(inout, group, num, len) ((unsigned long)((inout) | (((len) & IOCPARM_MASK) << 16) | ((group) << 8) | (num)))
#define _IOR(g, n, t) _IOC(IOC_OUT, (g), (n), sizeof(t))
#define _IOW(g, n, t) _IOC(IOC_IN, (g), (n), sizeof(t))
#define FIONREAD _IOR('f', 127, int)
#define FIONBIO _IOW('f', 126, int)
#define TIOCGWINSZ _IOR('t', 104, struct winsize)
#define TIOCSWINSZ _IOW('t', 103, struct winsize)
struct winsize { unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel; };
int ioctl(int, unsigned long, ...);
#ifdef __cplusplus
}
#endif
