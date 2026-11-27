// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libiconv: iconv_open, iconv and iconv_close forward to the host's C library (found on first use); the names of the encodings are Apple's
// list (iconvlist, iconv_canonicalize) and the library names that Apple's libiconv knows are mapped to the host's (hostName).

#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef void *iconv_t;

extern void *reef_host_symbol(const char *library, const char *name);
extern int reef_bridge_errno(void);

// What iconv_open returns: the host's descriptor and what the names asked for.
typedef struct {
    iconv_t host;
    char transliterate;
    char discardIllegal;
    char trivial;
} Descriptor;

int _libiconv_version = 0x010b;

typedef struct {
    const char *name, *alias, *otherAlias, *end;
} Encoding;

static const Encoding encodingNames[29] = {
    { "UTF-8", "UTF8" },
    { "UTF-8-MAC", "UTF8-MAC" },
    { "UTF-16" },
    { "UTF-16BE" },
    { "UTF-16LE" },
    { "UTF-32" },
    { "UTF-32BE" },
    { "UTF-32LE" },
    { "UCS-2" },
    { "UCS-4" },
    { "UTF-7" },
    { "ASCII", "US-ASCII", "ANSI_X3.4-1968" },
    { "ISO-8859-1", "LATIN1", "ISO_8859-1" },
    { "ISO-8859-2", "LATIN2" },
    { "ISO-8859-15", "LATIN-9" },
    { "MACINTOSH", "MAC", "MACROMAN" },
    { "CP1250", "WINDOWS-1250" },
    { "CP1251", "WINDOWS-1251" },
    { "CP1252", "WINDOWS-1252" },
    { "KOI8-R" },
    { "SHIFT_JIS", "SJIS" },
    { "EUC-JP" },
    { "ISO-2022-JP" },
    { "GB2312", "EUC-CN" },
    { "GBK", "CP936" },
    { "GB18030" },
    { "BIG5" },
    { "EUC-KR" },
    { "CP949" },
};

static struct {
    iconv_t (*open)(const char *, const char *);
    size_t (*convert)(iconv_t, char **, size_t *, char **, size_t *);
    int (*close)(iconv_t);
} host;

static void hostName(const char *name, char *host, char *transliterate, char *ignore);

iconv_t iconv_open(const char *tocode, const char *fromcode)
{
    char to[128], from[128];
    char combined[160];
    char scratch;

    if (!host.open) {
        host.convert = reef_host_symbol("libc.so.6", "iconv");
        host.close = reef_host_symbol("libc.so.6", "iconv_close");
        host.open = reef_host_symbol("libc.so.6", "iconv_open");
        if (!host.open || !host.convert || !host.close) {
            errno = EINVAL;
            return (iconv_t)-1;
        }
    }
    Descriptor *cd = calloc(1, sizeof(Descriptor));
    if (!cd) {
        errno = ENOMEM;
        return (iconv_t)-1;
    }
    hostName(tocode, to, &cd->transliterate, &cd->discardIllegal);
    hostName(fromcode, from, &scratch, &scratch);
    strlcpy(combined, to, sizeof combined);
    if (cd->transliterate == 1)
        strlcat(combined, "//TRANSLIT", sizeof combined);
    if (cd->discardIllegal == 1)
        strlcat(combined, cd->transliterate ? ",IGNORE" : "//IGNORE", sizeof combined);
    cd->host = host.open(combined, from);
    if (cd->host == (iconv_t)-1) {
        int error = reef_bridge_errno();
        free(cd);
        errno = error;
        return (iconv_t)-1;
    }
    cd->trivial = strcasecmp(to, from) == 0;
    return cd;
}

static void hostName(const char *name, char *host, char *transliterate, char *ignore)
{
    strlcpy(host, name, 128);
    char *options = strstr(host, "//");
    if (options) {
        if (strcasestr(options, "TRANSLIT"))
            *transliterate = 1;
        if (strcasestr(options, "IGNORE"))
            *ignore = 1;
        *options = '\0';
    }
    if (!strcasecmp(host, "UTF-8-MAC") || !strcasecmp(host, "UTF8-MAC")) {
        strcpy(host, "UTF-8");
    } else if (!strcasecmp(host, "MACINTOSH") || !strcasecmp(host, "MAC")) {
        strcpy(host, "MACINTOSH");
    } else if (!strcasecmp(host, "") || !strcasecmp(host, "char")) {
        strcpy(host, "UTF-8");
    } else if (!strcasecmp(host, "wchar_t")) {
        strcpy(host, "UTF-32LE");
    }
}

size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
    int error = EBADF;
    if (cd != NULL && cd != (iconv_t)-1) {
        size_t result = host.convert(((Descriptor *)cd)->host, inbuf, inbytesleft, outbuf, outbytesleft);
        if (result != (size_t)-1)
            return result;
        error = reef_bridge_errno();
    }
    errno = error;
    return (size_t)-1;
}

int iconv_close(iconv_t cd)
{
    if (cd == NULL || cd == (iconv_t)-1) {
        errno = EBADF;
        return -1;
    }
    int result = host.close(((Descriptor *)cd)->host);
    free(cd);
    return result;
}

int iconvctl(iconv_t cd, int request, void *argument)
{
    Descriptor *d = cd;
    if (cd == NULL || cd == (iconv_t)-1) {
        errno = EBADF;
        return -1;
    }
    switch (request) {
    case 0:
        *(unsigned int *)argument = d->trivial;
        return 0;
    case 1:
        *(unsigned int *)argument = d->transliterate;
        return 0;
    case 2:
        if (d->transliterate != (*(unsigned int *)argument != 0))
            break;
        return 0;
    case 3:
        *(unsigned int *)argument = d->discardIllegal;
        return 0;
    case 4:
        if (d->discardIllegal != (*(unsigned int *)argument != 0))
            break;
        return 0;
    case 5:
    case 6:
        return 0;
    }
    errno = EINVAL;
    return -1;
}

void iconvlist(int (*callback)(unsigned int, const char *const *, void *), void *data)
{
    for (int i = 0; i < 29; i++) {
        const Encoding *e = &encodingNames[i];
        unsigned int count = e->alias == NULL ? 1 : e->otherAlias == NULL ? 2 : 3;
        if (callback(count, &e->name, data))
            break;
    }
}

const char *iconv_canonicalize(const char *name)
{
    for (int i = 0; i < 29; i++) {
        const Encoding *e = &encodingNames[i];
        if (!strcasecmp(name, e->name))
            return e->name;
        if (e->alias) {
            if (!strcasecmp(name, e->alias))
                return e->name;
            if (e->otherAlias && !strcasecmp(name, e->otherAlias))
                return e->name;
        }
    }
    return name;
}

// The names of the GNU library, with the same behaviour.
iconv_t libiconv_open(const char *tocode, const char *fromcode)
{
    return iconv_open(tocode, fromcode);
}

size_t libiconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
    int error = EBADF;
    if (cd != NULL && cd != (iconv_t)-1) {
        size_t result = host.convert(((Descriptor *)cd)->host, inbuf, inbytesleft, outbuf, outbytesleft);
        if (result != (size_t)-1)
            return result;
        error = reef_bridge_errno();
    }
    errno = error;
    return (size_t)-1;
}

int libiconv_close(iconv_t cd)
{
    if (cd == NULL || cd == (iconv_t)-1) {
        errno = EBADF;
        return -1;
    }
    int result = host.close(((Descriptor *)cd)->host);
    free(cd);
    return result;
}

int libiconvctl(iconv_t cd, int request, void *argument)
{
    Descriptor *d = cd;
    if (cd == NULL || cd == (iconv_t)-1) {
        errno = EBADF;
        return -1;
    }
    switch (request) {
    case 0:
        *(unsigned int *)argument = d->trivial;
        return 0;
    case 1:
        *(unsigned int *)argument = d->transliterate;
        return 0;
    case 2:
        if (d->transliterate != (*(unsigned int *)argument != 0))
            break;
        return 0;
    case 3:
        *(unsigned int *)argument = d->discardIllegal;
        return 0;
    case 4:
        if (d->discardIllegal != (*(unsigned int *)argument != 0))
            break;
        return 0;
    case 5:
    case 6:
        return 0;
    }
    errno = EINVAL;
    return -1;
}

void libiconvlist(int (*callback)(unsigned int, const char *const *, void *), void *data)
{
    for (int i = 0; i < 29; i++) {
        const Encoding *e = &encodingNames[i];
        unsigned int count = e->alias == NULL ? 1 : e->otherAlias == NULL ? 2 : 3;
        if (callback(count, &e->name, data))
            break;
    }
}

void libiconv_set_relocation_prefix(const char *orig, const char *curr)
{
}
