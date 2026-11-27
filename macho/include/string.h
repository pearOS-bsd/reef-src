// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void *memcpy(void *, const void *, size_t); void *memmove(void *, const void *, size_t); void *memset(void *, int, size_t);
int memcmp(const void *, const void *, size_t); void *memchr(const void *, int, size_t); void *memccpy(void *, const void *, int, size_t);
size_t strlen(const char *); size_t strnlen(const char *, size_t); int strcmp(const char *, const char *); int strncmp(const char *, const char *, size_t);
int strcoll(const char *, const char *); size_t strxfrm(char *, const char *, size_t);
char *strcpy(char *, const char *); char *strncpy(char *, const char *, size_t); char *strcat(char *, const char *); char *strncat(char *, const char *, size_t);
char *strchr(const char *, int); char *strrchr(const char *, int); char *strstr(const char *, const char *); char *strpbrk(const char *, const char *);
size_t strspn(const char *, const char *); size_t strcspn(const char *, const char *); char *strtok(char *, const char *);
char *strdup(const char *); char *strndup(const char *, size_t); char *strerror(int); int strerror_r(int, char *, size_t);
void bzero(void *, size_t); void bcopy(const void *, void *, size_t); int strcasecmp(const char *, const char *); char *strcasestr(const char *, const char *); int strncasecmp(const char *, const char *, size_t);
size_t strlcpy(char *, const char *, size_t); size_t strlcat(char *, const char *, size_t);
#ifdef __cplusplus
}
#endif
