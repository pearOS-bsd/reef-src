// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
int isalnum(int); int isalpha(int); int isblank(int); int iscntrl(int); int isdigit(int); int isgraph(int); int islower(int);
int isprint(int); int ispunct(int); int isspace(int); int isupper(int); int isxdigit(int); int tolower(int); int toupper(int);
typedef unsigned int __uint32_t;
typedef int __darwin_rune_t;
typedef struct { __darwin_rune_t __min, __max, __map; __uint32_t *__types; } _RuneRange;
typedef struct {
    char __magic[8];
    char __encoding[32];
    __darwin_rune_t (*__sgetrune)(const char *, unsigned long, char const **);
    int (*__sputrune)(__darwin_rune_t, char *, unsigned long, char **);
    __darwin_rune_t __invalid_rune;
    __uint32_t __runetype[256];
    __darwin_rune_t __maplower[256];
    __darwin_rune_t __mapupper[256];
    _RuneRange __runetype_ext, __maplower_ext, __mapupper_ext;
    void *__variable;
    int __variable_len;
    int __ncharclasses;
    void *__charclasses;
} _RuneLocale;
extern _RuneLocale _DefaultRuneLocale;
#define _CTYPE_A 0x00000100L
#define _CTYPE_C 0x00000200L
#define _CTYPE_D 0x00000400L
#define _CTYPE_G 0x00000800L
#define _CTYPE_L 0x00001000L
#define _CTYPE_P 0x00002000L
#define _CTYPE_S 0x00004000L
#define _CTYPE_U 0x00008000L
#define _CTYPE_X 0x00010000L
#define _CTYPE_B 0x00020000L
#define _CTYPE_R 0x00040000L
#define _CTYPE_I 0x00080000L
#define _CTYPE_T 0x00100000L
#define _CTYPE_Q 0x00200000L
int isascii(int); int toascii(int);
#ifdef __cplusplus
}
#endif
