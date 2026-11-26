// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#define DT_UNKNOWN 0
#define DT_WHT 14
#define d_fileno d_ino
#define DT_DIR 4
#define DT_REG 8
#define DT_LNK 10
struct dirent { ino_t d_ino; unsigned long long d_seekoff; unsigned short d_reclen; unsigned short d_namlen; unsigned char d_type; char d_name[1024]; };
typedef struct __dirstream DIR;
DIR *opendir(const char *) __asm("_opendir$INODE64"); DIR *fdopendir(int) __asm("_fdopendir$INODE64"); struct dirent *readdir(DIR *) __asm("_readdir$INODE64"); int closedir(DIR *); void rewinddir(DIR *) __asm("_rewinddir$INODE64"); int dirfd(DIR *);
#ifdef __cplusplus
}
#endif
