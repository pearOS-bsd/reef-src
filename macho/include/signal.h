// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGEMT 7
#define SIGFPE 8
#define SIGKILL 9
#define SIGBUS 10
#define SIGSEGV 11
#define SIGSYS 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGURG 16
#define SIGSTOP 17
#define SIGTSTP 18
#define SIGCONT 19
#define SIGCHLD 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGIO 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGINFO 29
#define SIGUSR1 30
#define SIGUSR2 31
#define SIG_DFL ((void (*)(int))0)
#define SIG_IGN ((void (*)(int))1)
#define SIG_ERR ((void (*)(int))-1)
#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3
#define SA_RESTART 2
typedef unsigned int sigset_t;
typedef int sig_atomic_t;
struct sigaction { void (*sa_handler)(int); sigset_t sa_mask; int sa_flags; };
void (*signal(int, void (*)(int)))(int);
int raise(int); int kill(pid_t, int);
int sigemptyset(sigset_t *); int sigfillset(sigset_t *); int sigaddset(sigset_t *, int); int sigdelset(sigset_t *, int); int sigismember(const sigset_t *, int);
int sigaction(int, const struct sigaction *, struct sigaction *); int sigprocmask(int, const sigset_t *, sigset_t *); int pthread_sigmask(int, const sigset_t *, sigset_t *);
#ifdef __cplusplus
}
#endif
