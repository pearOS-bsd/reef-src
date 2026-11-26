// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libz: checksums, one-shot compression and a stream through the host's zlib.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned long adler32(unsigned long, const unsigned char *, unsigned);
unsigned long crc32(unsigned long, const unsigned char *, unsigned);
int compress2(unsigned char *, unsigned long *, const unsigned char *, unsigned long, int);
int uncompress(unsigned char *, unsigned long *, const unsigned char *, unsigned long);
unsigned long compressBound(unsigned long);
const char *zlibVersion(void);
const char *zError(int);

int main(void)
{
    char text[4096];
    for (int i = 0; i < (int)sizeof text; i++) text[i] = "reef zlib test "[i % 15];
    printf("version %s\n", zlibVersion());
    printf("adler %08lx crc %08lx\n", adler32(adler32(0, 0, 0), (unsigned char *)text, sizeof text), crc32(crc32(0, 0, 0), (unsigned char *)text, sizeof text));
    for (int level = 0; level <= 9; level += 3) {
        unsigned long size = compressBound(sizeof text);
        unsigned char *packed = malloc(size);
        int rc = compress2(packed, &size, (unsigned char *)text, sizeof text, level);
        unsigned long back = sizeof text;
        char out[4096];
        int rc2 = uncompress((unsigned char *)out, &back, packed, size);
        printf("level %d: %d %lu, %d %lu, same %d\n", level, rc, size, rc2, back, back == sizeof text && !memcmp(out, text, back));
        free(packed);
    }
    printf("error %s\n", zError(-3));
    return 0;
}
