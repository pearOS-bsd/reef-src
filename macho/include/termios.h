// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
typedef unsigned long tcflag_t; typedef unsigned char cc_t; typedef unsigned long speed_t;
#define NCCS 20
struct termios { tcflag_t c_iflag, c_oflag, c_cflag, c_lflag; cc_t c_cc[NCCS]; speed_t c_ispeed, c_ospeed; };
#define IGNBRK 0x1
#define BRKINT 0x2
#define IGNPAR 0x4
#define PARMRK 0x8
#define INPCK 0x10
#define ISTRIP 0x20
#define INLCR 0x40
#define IGNCR 0x80
#define ICRNL 0x100
#define IXON 0x200
#define IXOFF 0x400
#define IXANY 0x800
#define OPOST 0x1
#define ONLCR 0x2
#define CSIZE 0x300
#define CS5 0
#define CS6 0x100
#define CS7 0x200
#define CS8 0x300
#define CSTOPB 0x400
#define CREAD 0x800
#define PARENB 0x1000
#define PARODD 0x2000
#define HUPCL 0x4000
#define CLOCAL 0x8000
#define CRTSCTS 0x30000
#define ECHOE 0x2
#define ECHOK 0x4
#define ECHO 0x8
#define ECHONL 0x10
#define ISIG 0x80
#define ICANON 0x100
#define IEXTEN 0x400
#define NOFLSH 0x80000000
#define TOSTOP 0x400000
#define VEOF 0
#define VEOL 1
#define VERASE 3
#define VINTR 8
#define VKILL 5
#define VMIN 16
#define VQUIT 9
#define VSTART 12
#define VSTOP 13
#define VSUSP 10
#define VTIME 17
#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2
#define TCIFLUSH 1
#define TCOFLUSH 2
#define TCIOFLUSH 3
#define B0 0
#define B50 50
#define B75 75
#define B110 110
#define B134 134
#define B150 150
#define B200 200
#define B300 300
#define B600 600
#define B1200 1200
#define B1800 1800
#define B2400 2400
#define B4800 4800
#define B9600 9600
#define B19200 19200
#define B38400 38400
#define B57600 57600
#define B115200 115200
#define B230400 230400
int tcgetattr(int, struct termios *); int tcsetattr(int, int, const struct termios *); int tcdrain(int); int tcflush(int, int);
speed_t cfgetospeed(const struct termios *); speed_t cfgetispeed(const struct termios *);
int cfsetospeed(struct termios *, speed_t); int cfsetispeed(struct termios *, speed_t);
#ifdef __cplusplus
}
#endif
