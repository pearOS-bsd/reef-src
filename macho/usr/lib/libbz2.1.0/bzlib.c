// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libbz2: the functions that read and write bzip2 files (BZ2_bzWriteOpen, BZ2_bzRead, BZ2_bzopen ...); they are Reef's own and use the stream functions of the host's libbz2.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BZ_OK 0
#define BZ_RUN_OK 1
#define BZ_FINISH 2
#define BZ_FINISH_OK 3
#define BZ_STREAM_END 4
#define BZ_SEQUENCE_ERROR (-1)
#define BZ_PARAM_ERROR (-2)
#define BZ_MEM_ERROR (-3)
#define BZ_IO_ERROR (-6)
#define BZ_UNEXPECTED_EOF (-7)
#define BZ_MAX_UNUSED 5000

typedef struct {
    char *next_in;
    unsigned int avail_in;
    unsigned int total_in_lo32;
    unsigned int total_in_hi32;
    char *next_out;
    unsigned int avail_out;
    unsigned int total_out_lo32;
    unsigned int total_out_hi32;
    void *state;
    void *(*bzalloc)(void *, int, int);
    void (*bzfree)(void *, void *);
    void *opaque;
} bz_stream;

typedef struct {
    FILE *handle;
    char buf[BZ_MAX_UNUSED];
    int bufN;
    char writing;
    bz_stream strm;
    int lastErr;
    char initialisedOk;
    char closeHandle;
} BZFILE;

int BZ2_bzCompressInit(bz_stream *strm, int blockSize100k, int verbosity, int workFactor);
int BZ2_bzCompress(bz_stream *strm, int action);
int BZ2_bzCompressEnd(bz_stream *strm);
int BZ2_bzDecompressInit(bz_stream *strm, int verbosity, int small);
int BZ2_bzDecompress(bz_stream *strm);
int BZ2_bzDecompressEnd(bz_stream *strm);

#define BZ_SETERR(eee)                  \
    do {                                \
        if (bzerror != NULL)            \
            *bzerror = eee;             \
        if (bzf != NULL)                \
            bzf->lastErr = eee;         \
    } while (0)

BZFILE *BZ2_bzWriteOpen(int *bzerror, FILE *f, int blockSize100k, int verbosity, int workFactor)
{
    BZFILE *bzf = NULL;
    int ret;

    BZ_SETERR(BZ_OK);
    if (f == NULL || blockSize100k < 1 || blockSize100k > 9 || workFactor < 0 || workFactor > 250 || verbosity < 0 || verbosity > 4) {
        BZ_SETERR(BZ_PARAM_ERROR);
        return NULL;
    }
    if (ferror(f)) {
        BZ_SETERR(BZ_IO_ERROR);
        return NULL;
    }
    bzf = calloc(1, sizeof(BZFILE));
    if (bzf == NULL) {
        BZ_SETERR(BZ_MEM_ERROR);
        return NULL;
    }
    bzf->handle = f;
    bzf->writing = 1;
    ret = BZ2_bzCompressInit(&bzf->strm, blockSize100k, verbosity, workFactor ? workFactor : 30);
    if (ret != BZ_OK) {
        BZ_SETERR(ret);
        free(bzf);
        return NULL;
    }
    bzf->initialisedOk = 1;
    return bzf;
}

void BZ2_bzWrite(int *bzerror, BZFILE *bzf, void *buf, int len)
{
    BZ_SETERR(BZ_OK);
    if (bzf == NULL || buf == NULL || len < 0) {
        BZ_SETERR(BZ_PARAM_ERROR);
        return;
    }
    if (!bzf->writing) {
        BZ_SETERR(BZ_SEQUENCE_ERROR);
        return;
    }
    if (ferror(bzf->handle))
        goto ioError;
    if (len == 0)
        return;
    bzf->strm.avail_in = len;
    bzf->strm.next_in = buf;
    do {
        bzf->strm.avail_out = BZ_MAX_UNUSED;
        bzf->strm.next_out = bzf->buf;
        int ret = BZ2_bzCompress(&bzf->strm, 0);
        if (ret != BZ_RUN_OK) {
            BZ_SETERR(ret);
            return;
        }
        if (bzf->strm.avail_out != BZ_MAX_UNUSED) {
            unsigned int n = BZ_MAX_UNUSED - bzf->strm.avail_out;
            if (fwrite(bzf->buf, 1, n, bzf->handle) != n)
                goto ioError;
        }
    } while (bzf->strm.avail_in != 0);
    return;
ioError:
    BZ_SETERR(BZ_IO_ERROR);
}

void BZ2_bzWriteClose64(int *bzerror, BZFILE *bzf, int abandon, unsigned int *nbytes_in_lo32, unsigned int *nbytes_in_hi32,
                        unsigned int *nbytes_out_lo32, unsigned int *nbytes_out_hi32)
{
    if (bzf == NULL) {
        BZ_SETERR(BZ_OK);
        return;
    }
    if (!bzf->writing) {
        BZ_SETERR(BZ_SEQUENCE_ERROR);
        return;
    }
    BZ_SETERR(BZ_OK);
    if (!abandon && !ferror(bzf->handle)) {
        int ret;
        do {
            bzf->strm.avail_out = BZ_MAX_UNUSED;
            bzf->strm.next_out = bzf->buf;
            ret = BZ2_bzCompress(&bzf->strm, BZ_FINISH);
            if (ret != BZ_FINISH_OK && ret != BZ_STREAM_END) {
                BZ_SETERR(ret);
                break;
            }
            if (bzf->strm.avail_out != BZ_MAX_UNUSED) {
                unsigned int n = BZ_MAX_UNUSED - bzf->strm.avail_out;
                if (fwrite(bzf->buf, 1, n, bzf->handle) != n) {
                    ret = BZ_IO_ERROR;
                    BZ_SETERR(ret);
                    break;
                }
            }
        } while (ret != BZ_STREAM_END);
        if (fflush(bzf->handle) != 0)
            BZ_SETERR(BZ_IO_ERROR);
    }
    if (nbytes_in_lo32 != NULL)
        *nbytes_in_lo32 = bzf->strm.total_in_lo32;
    if (nbytes_in_hi32 != NULL)
        *nbytes_in_hi32 = bzf->strm.total_in_hi32;
    if (nbytes_out_lo32 != NULL)
        *nbytes_out_lo32 = bzf->strm.total_out_lo32;
    if (nbytes_out_hi32 != NULL)
        *nbytes_out_hi32 = bzf->strm.total_out_hi32;
    BZ2_bzCompressEnd(&bzf->strm);
    if (bzf->closeHandle == 1)
        fclose(bzf->handle);
    free(bzf);
}

void BZ2_bzWriteClose(int *bzerror, BZFILE *b, int abandon, unsigned int *nbytes_in, unsigned int *nbytes_out)
{
    BZ2_bzWriteClose64(bzerror, b, abandon, nbytes_in, NULL, nbytes_out, NULL);
}

BZFILE *BZ2_bzReadOpen(int *bzerror, FILE *f, int verbosity, int small, void *unused, int nUnused)
{
    BZFILE *bzf = NULL;
    int ret;

    BZ_SETERR(BZ_OK);
    if (f == NULL || (small != 0 && small != 1) || (verbosity < 0 || verbosity > 4) || (unused == NULL && nUnused != 0)
        || (unused != NULL && (nUnused < 0 || nUnused > BZ_MAX_UNUSED))) {
        BZ_SETERR(BZ_PARAM_ERROR);
        return NULL;
    }
    if (ferror(f)) {
        BZ_SETERR(BZ_IO_ERROR);
        return NULL;
    }
    bzf = calloc(1, sizeof(BZFILE));
    if (bzf == NULL) {
        BZ_SETERR(BZ_MEM_ERROR);
        return NULL;
    }
    bzf->handle = f;
    if (nUnused != 0)
        memcpy(bzf->buf, unused, nUnused);
    bzf->bufN = nUnused;
    ret = BZ2_bzDecompressInit(&bzf->strm, verbosity, small);
    if (ret != BZ_OK) {
        BZ_SETERR(ret);
        free(bzf);
        return NULL;
    }
    bzf->initialisedOk = 1;
    bzf->strm.avail_in = bzf->bufN;
    bzf->strm.next_in = bzf->buf;
    return bzf;
}

void BZ2_bzReadClose(int *bzerror, BZFILE *bzf)
{
    BZ_SETERR(BZ_OK);
    if (bzf == NULL)
        return;
    if (bzf->writing == 1) {
        BZ_SETERR(BZ_SEQUENCE_ERROR);
        return;
    }
    if (bzf->initialisedOk == 1)
        BZ2_bzDecompressEnd(&bzf->strm);
    if (bzf->closeHandle == 1)
        fclose(bzf->handle);
    free(bzf);
}

int BZ2_bzRead(int *bzerror, BZFILE *bzf, void *buf, int len)
{
    int ret;

    BZ_SETERR(BZ_OK);
    if (bzf == NULL || buf == NULL || len < 0) {
        BZ_SETERR(BZ_PARAM_ERROR);
        return 0;
    }
    if (bzf->writing == 1) {
        BZ_SETERR(BZ_SEQUENCE_ERROR);
        return 0;
    }
    if (len == 0)
        return 0;
    bzf->strm.avail_out = len;
    bzf->strm.next_out = buf;
    do {
        if (ferror(bzf->handle)) {
            BZ_SETERR(BZ_IO_ERROR);
            return 0;
        }
        if (bzf->strm.avail_in == 0 && !feof(bzf->handle)) {
            size_t n = fread(bzf->buf, 1, BZ_MAX_UNUSED, bzf->handle);
            if (ferror(bzf->handle)) {
                BZ_SETERR(BZ_IO_ERROR);
                return 0;
            }
            bzf->bufN = n;
            bzf->strm.avail_in = n;
            bzf->strm.next_in = bzf->buf;
        }
        ret = BZ2_bzDecompress(&bzf->strm);
        if (ret != BZ_OK && ret != BZ_STREAM_END) {
            BZ_SETERR(ret);
            return 0;
        }
        if (ret == BZ_OK) {
            if (feof(bzf->handle) && bzf->strm.avail_in == 0) {
                if (bzf->strm.avail_out == 0)
                    return len;
                BZ_SETERR(BZ_UNEXPECTED_EOF);
                return 0;
            }
        } else if (ret == BZ_STREAM_END) {
            BZ_SETERR(BZ_STREAM_END);
            return len - bzf->strm.avail_out;
        }
    } while (bzf->strm.avail_out != 0);
    return len;
}

void BZ2_bzReadGetUnused(int *bzerror, BZFILE *bzf, void **unused, int *nUnused)
{
    if ((bzf == NULL) | (unused == NULL) | (nUnused == NULL)) {
        BZ_SETERR(BZ_PARAM_ERROR);
        return;
    }
    if (bzf->lastErr != BZ_STREAM_END) {
        BZ_SETERR(BZ_SEQUENCE_ERROR);
        return;
    }
    BZ_SETERR(BZ_OK);
    *nUnused = bzf->strm.avail_in;
    *unused = bzf->strm.next_in;
}

static BZFILE *openWith(const char *path, int fd, const char *mode)
{
    int blockSize100k = 9;
    unsigned char small = 0;
    _Bool writing = 0;
    FILE *fp;
    BZFILE *bzfp;
    int ret;

    if (mode == NULL)
        return NULL;
    while (*mode) {
        switch (*mode) {
        case 'r':
            writing = 0;
            break;
        case 'w':
            writing = 1;
            break;
        case 's':
            small = 1;
            break;
        default:
            if (*mode >= '1' && *mode <= '9')
                blockSize100k = *mode - '0';
        }
        mode++;
    }
    if (path == NULL)
        fp = fdopen(fd, writing ? "wb" : "rb");
    else if (path[0] == '\0')
        fp = writing ? stdout : stdin;
    else
        fp = fopen(path, writing ? "wb" : "rb");
    if (fp == NULL)
        return NULL;

    if (writing) {
        bzfp = NULL;
        if (blockSize100k < 1 || blockSize100k > 9 || ferror(fp) || (bzfp = calloc(1, sizeof(BZFILE))) == NULL)
            goto fail;
        bzfp->handle = fp;
        bzfp->writing = 1;
        ret = BZ2_bzCompressInit(&bzfp->strm, blockSize100k, 0, 30);
        if (ret != BZ_OK) {
            free(bzfp);
            goto fail;
        }
        bzfp->initialisedOk = 1;
    } else {
        bzfp = NULL;
        if (ferror(fp) || (bzfp = calloc(1, sizeof(BZFILE))) == NULL)
            goto fail;
        bzfp->handle = fp;
        ret = BZ2_bzDecompressInit(&bzfp->strm, 0, small);
        if (ret != BZ_OK) {
            free(bzfp);
            goto fail;
        }
        bzfp->initialisedOk = 1;
        bzfp->strm.avail_in = bzfp->bufN;
        bzfp->strm.next_in = bzfp->buf;
    }
    bzfp->closeHandle = fp != stdout && fp != stdin;
    return bzfp;
fail:
    if (fp != stdin && fp != stdout)
        fclose(fp);
    return NULL;
}

BZFILE *BZ2_bzopen(const char *path, const char *mode)
{
    return openWith(path ? path : "", -1, mode);
}

BZFILE *BZ2_bzdopen(int fd, const char *mode)
{
    return openWith(NULL, fd, mode);
}

int BZ2_bzread(BZFILE *b, void *buf, int len)
{
    int bzerr, nread;
    if (b != NULL && b->lastErr == BZ_STREAM_END)
        return 0;
    nread = BZ2_bzRead(&bzerr, b, buf, len);
    if (bzerr == BZ_OK || bzerr == BZ_STREAM_END)
        return nread;
    return -1;
}

int BZ2_bzwrite(BZFILE *b, void *buf, int len)
{
    int bzerr;
    BZ2_bzWrite(&bzerr, b, buf, len);
    if (bzerr == BZ_OK)
        return len;
    return -1;
}

int BZ2_bzflush(BZFILE *b)
{
    return 0;
}

void BZ2_bzclose(BZFILE *b)
{
    int bzerr;

    if (b == NULL)
        return;
    if (b->writing == 1) {
        BZ2_bzWriteClose64(&bzerr, b, 0, NULL, NULL, NULL, NULL);
    } else {
        b->lastErr = BZ_OK;
        if (b->initialisedOk == 1)
            BZ2_bzDecompressEnd(&b->strm);
        if (b->closeHandle == 1)
            fclose(b->handle);
        free(b);
    }
}

static const char *const errorNames[] = {
    "OK", "SEQUENCE_ERROR", "PARAM_ERROR", "MEM_ERROR", "DATA_ERROR", "DATA_ERROR_MAGIC",
    "IO_ERROR", "UNEXPECTED_EOF", "OUTBUFF_FULL", "CONFIG_ERROR", "???"
};

const char *BZ2_bzerror(BZFILE *b, int *errnum)
{
    int err = b ? b->lastErr : 0;
    int shown = err > 0 ? 0 : err;
    if (errnum)
        *errnum = shown;
    return errorNames[err > -10 ? -shown : 10];
}
