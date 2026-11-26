// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <time.h>
struct stat { dev_t st_dev; mode_t st_mode; nlink_t st_nlink; ino_t st_ino; uid_t st_uid; gid_t st_gid; dev_t st_rdev;
  struct timespec st_atimespec, st_mtimespec, st_ctimespec, st_birthtimespec; off_t st_size; blkcnt_t st_blocks; blksize_t st_blksize; unsigned int st_flags; unsigned int st_gen; int st_lspare; long long st_qspare[2]; };
#define st_atime st_atimespec.tv_sec
#define st_mtime st_mtimespec.tv_sec
#define st_ctime st_ctimespec.tv_sec
#define S_IFMT 0170000
#define S_IFIFO 0010000
#define S_IFCHR 0020000
#define S_IFDIR 0040000
#define S_IFBLK 0060000
#define S_IFREG 0100000
#define S_IFLNK 0120000
#define S_IFSOCK 0140000
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#define S_IRWXU 0700
#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRWXG 070
#define S_IRGRP 040
#define S_IWGRP 020
#define S_IXGRP 010
#define S_IRWXO 07
#define S_IROTH 04
#define S_IWOTH 02
#define S_IXOTH 01
#define S_ISUID 04000
#define S_ISGID 02000
#define S_ISVTX 01000
#define UTIME_NOW -1
#define UTIME_OMIT -2
int stat(const char *, struct stat *) __asm("_stat$INODE64"); int lstat(const char *, struct stat *) __asm("_lstat$INODE64"); int fstat(int, struct stat *) __asm("_fstat$INODE64"); int fstatat(int, const char *, struct stat *, int) __asm("_fstatat$INODE64");
int mkdir(const char *, mode_t); int chmod(const char *, mode_t); int fchmod(int, mode_t); int fchmodat(int, const char *, mode_t, int); mode_t umask(mode_t);
int futimens(int, const struct timespec[2]); int utimensat(int, const char *, const struct timespec[2], int);
#ifdef __cplusplus
}
#endif
