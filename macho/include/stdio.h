// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stddef.h>
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct __sFILE FILE;
extern FILE *__stdinp, *__stdoutp, *__stderrp;
#define stdin __stdinp
#define stdout __stdoutp
#define stderr __stderrp
#define EOF (-1)
int printf(const char *, ...);
int fprintf(FILE *, const char *, ...);
int sprintf(char *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int vsnprintf(char *, size_t, const char *, va_list);
int vfprintf(FILE *, const char *, va_list);
int puts(const char *);
int putchar(int);
int fputs(const char *, FILE *);
int fputc(int, FILE *);
int fflush(FILE *);
FILE *fopen(const char *, const char *);
FILE *fdopen(int, const char *);
int fclose(FILE *);
size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int feof(FILE *);
int ferror(FILE *);
int sscanf(const char *, const char *, ...); int scanf(const char *, ...); int fscanf(FILE *, const char *, ...);
int vsprintf(char *, const char *, va_list); int vprintf(const char *, va_list); int vsscanf(const char *, const char *, va_list);
int vfscanf(FILE *, const char *, va_list); int vscanf(const char *, va_list);
int fgetc(FILE *); int getc(FILE *); int getchar(void); int putc(int, FILE *); int ungetc(int, FILE *);
char *fgets(char *, int, FILE *); int fseek(FILE *, long, int); long ftell(FILE *); void rewind(FILE *); void clearerr(FILE *);
int setvbuf(FILE *, char *, int, size_t); void setbuf(FILE *, char *); void perror(const char *); int remove(const char *); int rename(const char *, const char *);
FILE *freopen(const char *, const char *, FILE *); FILE *tmpfile(void); char *tmpnam(char *);
typedef long long fpos_t; int fgetpos(FILE *, fpos_t *); int fsetpos(FILE *, const fpos_t *);
#define BUFSIZ 1024
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2
#define FILENAME_MAX 1024
#define L_tmpnam 1024
#define TMP_MAX 308915776
#define FOPEN_MAX 20
#ifdef __cplusplus
}
#endif
