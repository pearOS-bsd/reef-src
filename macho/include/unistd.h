// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define _SC_PAGESIZE 29
#define _SC_NPROCESSORS_ONLN 58
#define _SC_NPROCESSORS_CONF 57
#define _SC_IOV_MAX 56
#define F_OK 0
#define R_OK 4
#define W_OK 2
#define X_OK 1
int close(int); ssize_t read(int, void *, size_t); ssize_t write(int, const void *, size_t); off_t lseek(int, off_t, int); int isatty(int);
int unlink(const char *); int unlinkat(int, const char *, int); int rmdir(const char *); int link(const char *, const char *); int symlink(const char *, const char *);
ssize_t readlink(const char *, char *, size_t); ssize_t readlinkat(int, const char *, char *, size_t); int access(const char *, int); char *getcwd(char *, size_t); int chdir(const char *);
int truncate(const char *, off_t); int ftruncate(int, off_t); int fsync(int); long sysconf(int); pid_t getpid(void); uid_t getuid(void); int dup(int); int dup2(int, int);
int rename(const char *, const char *); unsigned sleep(unsigned); int usleep(unsigned); int pipe(int[2]);
#ifdef __cplusplus
}
#endif
