// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libbz2: the buffer interface (host), the file interface (Reef's own code) and its error values.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef void BZFILE;
int BZ2_bzBuffToBuffCompress(char *, unsigned *, char *, unsigned, int, int, int);
int BZ2_bzBuffToBuffDecompress(char *, unsigned *, char *, unsigned, int, int);
const char *BZ2_bzlibVersion(void);
BZFILE *BZ2_bzWriteOpen(int *, FILE *, int, int, int);
void BZ2_bzWrite(int *, BZFILE *, void *, int);
void BZ2_bzWriteClose(int *, BZFILE *, int, unsigned *, unsigned *);
BZFILE *BZ2_bzReadOpen(int *, FILE *, int, int, void *, int);
int BZ2_bzRead(int *, BZFILE *, void *, int);
void BZ2_bzReadClose(int *, BZFILE *);
void BZ2_bzReadGetUnused(int *, BZFILE *, void **, int *);
BZFILE *BZ2_bzopen(const char *, const char *);
int BZ2_bzread(BZFILE *, void *, int);
int BZ2_bzwrite(BZFILE *, void *, int);
int BZ2_bzflush(BZFILE *);
void BZ2_bzclose(BZFILE *);
BZFILE *BZ2_bzdopen(int, const char *);
const char *BZ2_bzerror(BZFILE *, int *);

int main(void)
{
    char text[20000];
    for (int i = 0; i < (int)sizeof text; i++) text[i] = "reef bzip2 test "[i % 16] + (i / 4000);
    printf("version %.5s\n", BZ2_bzlibVersion());

    static char packed[30000], back[20000];
    unsigned psize = sizeof packed, bsize = sizeof back;
    int rc = BZ2_bzBuffToBuffCompress(packed, &psize, text, sizeof text, 5, 0, 0);
    int rc2 = BZ2_bzBuffToBuffDecompress(back, &bsize, packed, psize, 0, 0);
    printf("buffer: %d %u, %d %u, same %d\n", rc, psize, rc2, bsize, bsize == sizeof text && !memcmp(back, text, bsize));

    char path[] = "/tmp/reef-bz2-XXXXXX";
    int fd = mkstemp(path);
    FILE *f = fdopen(fd, "wb");
    int err;
    BZFILE *w = BZ2_bzWriteOpen(&err, f, 6, 0, 0);
    printf("write open %d %d\n", w != 0, err);
    BZ2_bzWrite(&err, w, text, 10000);
    BZ2_bzWrite(&err, w, text + 10000, 10000);
    unsigned in, out;
    BZ2_bzWriteClose(&err, w, 0, &in, &out);
    printf("write close %d in %u out %u\n", err, in, out);
    fclose(f);

    f = fopen(path, "rb");
    BZFILE *r = BZ2_bzReadOpen(&err, f, 0, 0, 0, 0);
    char got[20000];
    int n = BZ2_bzRead(&err, r, got, 7000);
    printf("read %d %d\n", n, err);
    n += BZ2_bzRead(&err, r, got + n, 13000);
    printf("read %d %d same %d\n", n, err, n == sizeof text && !memcmp(got, text, n));
    n = BZ2_bzRead(&err, r, got, 10);
    printf("after the end: %d %d\n", n, err);
    void *unused = 0; int nunused = -7;
    BZ2_bzReadGetUnused(&err, r, &unused, &nunused);
    printf("unused %d %d\n", nunused, err);
    BZ2_bzReadClose(&err, r);
    printf("read close %d\n", err);
    fclose(f);

    // parameter errors and the names of the errors
    BZFILE *bad = BZ2_bzWriteOpen(&err, 0, 6, 0, 0);
    printf("bad open %d %d\n", bad != 0, err);
    bad = BZ2_bzReadOpen(&err, stdin, 9, 0, 0, 0);
    printf("bad read open %d %d\n", bad != 0, err);
    for (int e = 1; e <= 10; e++) printf("%d ", e);
    printf("\n");

    // the zlib-style interface
    BZFILE *z = BZ2_bzopen(path, "w9");
    printf("bzopen w %d, wrote %d, flush %d\n", z != 0, BZ2_bzwrite(z, text, 5000), BZ2_bzflush(z));
    BZ2_bzclose(z);
    z = BZ2_bzopen(path, "r");
    char buf[6000];
    int total = 0, k;
    while ((k = BZ2_bzread(z, buf, 1000)) > 0) total += k;
    BZ2_bzerror(z, &err);
    printf("bzread total %d last %d error %d %s\n", total, k, err, BZ2_bzerror(z, &err));
    BZ2_bzclose(z);
    int fd2 = fileno(fopen(path, "rb"));
    z = BZ2_bzdopen(fd2, "r");
    printf("bzdopen %d\n", z != 0);
    if (z) { printf("read %d\n", BZ2_bzread(z, buf, 100)); BZ2_bzclose(z); }
    z = BZ2_bzopen("/nonexistent/file", "r");
    printf("open missing %d\n", z != 0);
    // a damaged file
    f = fopen(path, "r+b"); fseek(f, 30, SEEK_SET); fputc(0x55, f); fclose(f);
    z = BZ2_bzopen(path, "r");
    k = BZ2_bzread(z, buf, 5000);
    const char *m = BZ2_bzerror(z, &err);
    printf("damaged: %d %d %s\n", k, err, m);
    BZ2_bzclose(z);
    unlink(path);
    return 0;
}
