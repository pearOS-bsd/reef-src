// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libcups: options, URI parsing, IPP strings and the client defaults (no server is contacted).
#include <stdio.h>
typedef struct { char *name, *value; } cups_option_t;
int cupsAddOption(const char *, const char *, int, cups_option_t **);
const char *cupsGetOption(const char *, int, cups_option_t *);
int cupsParseOptions(const char *, int, cups_option_t **);
void cupsFreeOptions(int, cups_option_t *);
int httpSeparateURI(int, const char *, char *, int, char *, int, char *, int, int *, char *, int);
const char *ippErrorString(int);
const char *ippOpString(int);
const char *httpStatus(int);
int ippPort(void);
const char *cupsUser(void);

int main(void)
{
    cups_option_t *options = 0;
    int count = cupsAddOption("media", "A4", 0, &options);
    count = cupsAddOption("sides", "two-sided-long-edge", count, &options);
    count = cupsParseOptions("copies=2 landscape fit-to-page=true", count, &options);
    printf("%d options\n", count);
    printf("media=%s copies=%s landscape=%s missing=%s\n", cupsGetOption("media", count, options), cupsGetOption("copies", count, options),
           cupsGetOption("landscape", count, options), cupsGetOption("missing", count, options) ? "?" : "none");
    cupsFreeOptions(count, options);
    char scheme[32], user[64], host[128], resource[256]; int port = 0;
    int status = httpSeparateURI(0, "ipp://alice@printer.example.com:8631/printers/office", scheme, sizeof scheme, user, sizeof user, host, sizeof host, &port, resource, sizeof resource);
    printf("%d %s %s %s %d %s\n", status, scheme, user, host, port, resource);
    printf("%s | %s | %s | port %d\n", ippErrorString(0x0406), ippOpString(0x0002), httpStatus(404), ippPort());
    printf("user set %d\n", cupsUser() != 0);
    return 0;
}
