// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libcurl: transfers of file:// URLs through the host's library with the default callbacks (Apple's stdout), a callback of the caller, a
// FILE of Apple's as the target, header callbacks, a duplicated and a reset handle, and the printf family.
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef void CURL;
CURL *curl_easy_init(void);
void curl_easy_cleanup(CURL *);
void curl_easy_reset(CURL *);
CURL *curl_easy_duphandle(CURL *);
int curl_easy_setopt(CURL *, int, ...);
int curl_easy_getinfo(CURL *, int, ...);
int curl_easy_perform(CURL *);
const char *curl_easy_strerror(int);
int curl_global_init(long);
char *curl_maprintf(const char *, ...);
int curl_mprintf(const char *, ...);
int curl_mfprintf(FILE *, const char *, ...);
int curl_msprintf(char *, const char *, ...);
int curl_msnprintf(char *, size_t, const char *, ...);
int curl_mvprintf(const char *, va_list);
void curl_free(void *);

#define URL 10002
#define WRITEFUNCTION 20011
#define WRITEDATA 10001
#define HEADERFUNCTION 20079
#define HEADERDATA 10029
#define SIZE_DOWNLOAD 0x300008

static size_t total;
static size_t count(void *data, size_t size, size_t n, void *user)
{
    total += size * n;
    if (user) *(int *)user += 1;
    return size * n;
}

static size_t header(void *data, size_t size, size_t n, void *user)
{
    (*(int *)user)++;
    return size * n;
}

static void vprint(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    curl_mvprintf(format, ap);
    va_end(ap);
}

int main(void)
{
    FILE *f = fopen("/tmp/reef-curl-test.txt", "w");
    fputs("line one\nline two\n", f);
    fclose(f);
    printf("global %d\n", curl_global_init(3));

    CURL *c = curl_easy_init();
    printf("init %d\n", c != 0);
    printf("url %d\n", curl_easy_setopt(c, URL, "file:///tmp/reef-curl-test.txt"));
    fflush(stdout);
    printf("default write: %d\n", curl_easy_perform(c));
    fflush(stdout);
    double size = 0;
    printf("getinfo %d size %.0f\n", curl_easy_getinfo(c, SIZE_DOWNLOAD, &size), size);

    int calls = 0;
    curl_easy_setopt(c, WRITEFUNCTION, count);
    curl_easy_setopt(c, WRITEDATA, &calls);
    printf("callback %d: %zu bytes in %d calls\n", curl_easy_perform(c), total, calls);

    FILE *out = fopen("/tmp/reef-curl-copy.txt", "w");
    curl_easy_setopt(c, WRITEFUNCTION, 0);          // back to the default function, now on a FILE of ours
    curl_easy_setopt(c, WRITEDATA, out);
    printf("to file %d\n", curl_easy_perform(c));
    fclose(out);
    char text[64] = {0};
    f = fopen("/tmp/reef-curl-copy.txt", "r");
    fread(text, 1, sizeof text - 1, f);
    fclose(f);
    printf("copy: %zu bytes, starts '%.8s'\n", strlen(text), text);

    int headers = 0;
    curl_easy_setopt(c, HEADERFUNCTION, header);
    curl_easy_setopt(c, HEADERDATA, &headers);
    total = 0;
    curl_easy_setopt(c, WRITEFUNCTION, count);
    curl_easy_setopt(c, WRITEDATA, 0);
    printf("headers %d: %d header calls, %zu bytes\n", curl_easy_perform(c), headers, total);

    CURL *d = curl_easy_duphandle(c);
    headers = 0; total = 0;
    printf("dup %d: %d, %zu\n", curl_easy_perform(d), headers, total);
    curl_easy_cleanup(d);
    curl_easy_reset(c);
    curl_easy_setopt(c, URL, "file:///tmp/reef-curl-copy.txt");
    fflush(stdout);
    printf("after reset %d\n", curl_easy_perform(c));
    printf("missing %d\n", (curl_easy_setopt(c, URL, "file:///tmp/does-not-exist-reef"), curl_easy_perform(c)));
    curl_easy_cleanup(c);

    char *a = curl_maprintf("%d:%s:%05.2f", 7, "maprintf", 3.14159);
    printf("maprintf %s\n", a);
    curl_free(a);
    char buf[32];
    int n = curl_msprintf(buf, "%s-%d", "sprintf", 12);
    printf("msprintf %d %s\n", n, buf);
    n = curl_msnprintf(buf, 8, "%s", "truncated string");
    printf("msnprintf %d %s\n", n, buf);
    fflush(stdout);
    n = curl_mprintf("mprintf %d %s\n", 5, "five");
    printf("returned %d\n", n);
    n = curl_mfprintf(stdout, "mfprintf %lld\n", 1234567890123LL);
    printf("returned %d\n", n);
    vprint("mvprintf %s\n", "vprint");
    fflush(stdout);
    return 0;
}
