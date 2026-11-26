// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libicucore: character properties, case mapping, collation and number formatting through the host's ICU.
#include <stdio.h>
typedef unsigned short UChar;
int u_charType(int);
int u_toupper(int);
int u_tolower(int);
int u_isalpha(int);
int u_digit(int, signed char);
UChar *u_strFromUTF8(UChar *, int, int *, const char *, int, int *);
char *u_strToUTF8(char *, int, int *, const UChar *, int, int *);
int u_strlen(const UChar *);
int u_strToUpper(UChar *, int, const UChar *, int, const char *, int *);
const char *u_errorName(int);
void *ucol_open(const char *, int *);
int ucol_strcoll(void *, const UChar *, int, const UChar *, int);
void ucol_close(void *);
const char *uloc_getDefault(void);
int uloc_getLanguage(const char *, char *, int, int *);

static void utf16(UChar *out, const char *s) { int n = 0, e = 0; u_strFromUTF8(out, 64, &n, s, -1, &e); }

int main(void)
{
    printf("type A %d, 5 %d, space %d\n", u_charType('A'), u_charType('5'), u_charType(' '));
    printf("upper %c lower %c alpha %d digit %d\n", u_toupper('q'), u_tolower('Q'), u_isalpha('x'), u_digit('7', 10));
    UChar in[64], up[64]; char back[64]; int error = 0, n = 0;
    utf16(in, "stra\xc3\x9f" "e caf\xc3\xa9");
    int len = u_strToUpper(up, 64, in, -1, "de", &error);
    u_strToUTF8(back, 64, &n, up, len, &error);
    printf("%d %d %s %s\n", len, n, back, u_errorName(error));
    int e = 0;
    void *collator = ucol_open("en_US", &e);
    UChar a[64], b[64]; utf16(a, "apple"); utf16(b, "Banana");
    printf("coll %d %d %d\n", ucol_strcoll(collator, a, -1, b, -1), ucol_strcoll(collator, b, -1, a, -1), ucol_strcoll(collator, a, -1, a, -1));
    ucol_close(collator);
    char lang[16]; int status = 0; uloc_getLanguage("fr_CA", lang, sizeof lang, &status);
    printf("lang %s %d\n", lang, status);
    return 0;
}
