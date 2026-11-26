// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#define WNOHANG 1
#define WUNTRACED 2
#define _W_INT(w) (*(int *)&(w))
#define _WSTOPPED 0177
#define _WSTATUS(x) (_W_INT(x) & 0177)
#define WIFCONTINUED(x) (_WSTATUS(x) == _WSTOPPED && WSTOPSIG(x) == 0x13)
#define WIFSTOPPED(x) (_WSTATUS(x) == _WSTOPPED && WSTOPSIG(x) != 0x13)
#define WIFEXITED(x) (_WSTATUS(x) == 0)
#define WIFSIGNALED(x) (_WSTATUS(x) != _WSTOPPED && _WSTATUS(x) != 0)
#define WEXITSTATUS(x) ((_W_INT(x) >> 8) & 0xff)
#define WSTOPSIG(x) (_W_INT(x) >> 8)
#define WTERMSIG(x) (_WSTATUS(x))
#define WCOREDUMP(x) (_W_INT(x) & 0200)
pid_t waitpid(pid_t, int *, int); pid_t wait(int *);
#ifdef __cplusplus
}
#endif
