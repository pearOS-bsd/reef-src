// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libiconv: conversions through the host, the names of Apple's list, iconvctl and the error values.
#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef void *iconv_t;
iconv_t iconv_open(const char *, const char *);
size_t iconv(iconv_t, char **, size_t *, char **, size_t *);
int iconv_close(iconv_t);
int iconvctl(iconv_t, int, void *);
void iconvlist(int (*)(unsigned int, const char *const *, void *), void *);
const char *iconv_canonicalize(const char *);

static int show(unsigned int count, const char *const *names, void *data)
{
    int *n = data;
    if ((*n)++ % 6 == 0) {
        printf("name %u:", count);
        for (unsigned i = 0; i < count; i++) printf(" %s", names[i]);
        printf("\n");
    }
    return 0;
}

static int stop(unsigned int count, const char *const *names, void *data)
{
    printf("first %s\n", names[0]);
    return 1;
}

static void convert(const char *to, const char *from, const char *text, size_t length)
{
    iconv_t cd = iconv_open(to, from);
    if (cd == (iconv_t)-1) { printf("%s <- %s: open failed, errno %d\n", to, from, errno); return; }
    char out[200];
    char *in = (char *)text, *o = out;
    size_t inleft = length, outleft = sizeof out;
    size_t r = iconv(cd, &in, &inleft, &o, &outleft);
    printf("%s <- %s: %ld, left %zu, wrote %zu:", to, from, (long)r, inleft, sizeof out - outleft);
    for (char *p = out; p < o; p++) printf(" %02x", (unsigned char)*p);
    printf("\n");
    int value = -1;
    iconvctl(cd, 0, &value);
    printf("trivial %d", value);
    iconvctl(cd, 1, &value);
    printf(" translit %d", value);
    iconvctl(cd, 3, &value);
    printf(" discard %d\n", value);
    printf("close %d\n", iconv_close(cd));
}

int main(void)
{
    const char *text = "h\xc3\xa9llo \xe2\x82\xac";
    convert("UTF-16LE", "UTF-8", text, strlen(text));
    convert("UTF-8", "UTF-8-MAC", text, strlen(text));
    convert("ASCII//TRANSLIT", "UTF-8", text, strlen(text));
    convert("ASCII//IGNORE", "UTF-8", text, strlen(text));
    convert("ASCII//TRANSLIT//IGNORE", "UTF-8", text, strlen(text));
    convert("ISO-8859-1", "wchar_t", "h\0\0\0i\0\0\0", 8);
    convert("", "char", "plain", 5);
    convert("nonexistent-charset", "UTF-8", "x", 1);
    convert("MAC", "UTF-8", "abc", 3);

    int n = 0;
    iconvlist(show, &n);
    printf("%d names\n", n);
    iconvlist(stop, 0);
    printf("%s %s %s %s %s\n", iconv_canonicalize("utf8"), iconv_canonicalize("latin1"), iconv_canonicalize("windows-1252"), iconv_canonicalize("ansi_x3.4-1968"), iconv_canonicalize("nothing"));

    printf("close(-1) %d errno %d\n", iconv_close((iconv_t)-1), errno);
    char *p = 0; size_t z = 0;
    printf("iconv(-1) %ld errno %d\n", (long)iconv((iconv_t)-1, &p, &z, &p, &z), errno);
    int value = 0;
    printf("ctl(-1) %d errno %d\n", iconvctl((iconv_t)-1, 0, &value), errno);
    iconv_t cd = iconv_open("UTF-8", "UTF-16LE");
    printf("ctl bad %d errno %d, set %d\n", iconvctl(cd, 99, &value), errno, iconvctl(cd, 2, &value));
    iconv_close(cd);
    return 0;
}
