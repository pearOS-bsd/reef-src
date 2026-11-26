// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <sys/stat.h>
typedef struct _ftsent { struct _ftsent *fts_cycle, *fts_parent, *fts_link; long fts_number; void *fts_pointer; char *fts_accpath; char *fts_path; int fts_errno; int fts_symfd;
  unsigned short fts_pathlen, fts_namelen; ino_t fts_ino; dev_t fts_dev; nlink_t fts_nlink; short fts_level; unsigned short fts_info, fts_flags, fts_instr; struct stat *fts_statp; char fts_name[1]; } FTSENT;
typedef struct __tag_fts FTS;
#define FTS_COMFOLLOW 0x001
#define FTS_LOGICAL 0x002
#define FTS_NOCHDIR 0x004
#define FTS_NOSTAT 0x008
#define FTS_PHYSICAL 0x010
#define FTS_SEEDOT 0x020
#define FTS_XDEV 0x040
#define FTS_D 1
#define FTS_DC 2
#define FTS_DEFAULT 3
#define FTS_DNR 4
#define FTS_DOT 5
#define FTS_DP 6
#define FTS_ERR 7
#define FTS_F 8
#define FTS_NS 10
#define FTS_NSOK 11
#define FTS_SL 12
#define FTS_SLNONE 13
#define FTS_SKIP 4
FTS *fts_open(char * const *, int, int (*)(const FTSENT **, const FTSENT **)) __asm("_fts_open$INODE64");
FTSENT *fts_read(FTS *) __asm("_fts_read$INODE64"); int fts_close(FTS *) __asm("_fts_close$INODE64");
int fts_set(FTS *, FTSENT *, int);
#ifdef __cplusplus
}
#endif
