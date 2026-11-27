// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libcompression: the buffer functions and the stream functions of Apple's API. LZFSE (and LZVN inside it) is the reference implementation
// linked in; zlib is the library next to this one; LZ4, LZMA and Brotli are the host's libraries, found when the first stream is made.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lzfse.h"

extern void *reef_host_symbol(const char *library, const char *name);

typedef struct z_stream_s {
    const unsigned char *next_in;
    unsigned avail_in;
    unsigned long total_in;
    unsigned char *next_out;
    unsigned avail_out;
    unsigned long total_out;
    char *msg;
    void *state;
    void *zalloc, *zfree, *opaque;
    int data_type;
    unsigned long adler, reserved;
} z_stream;

extern const char *zlibVersion(void);
extern int deflateInit2_(z_stream *, int, int, int, int, int, const char *, int);
extern int inflateInit2_(z_stream *, int, const char *, int);
extern int deflate(z_stream *, int);
extern int inflate(z_stream *, int);
extern int deflateEnd(z_stream *);
extern int inflateEnd(z_stream *);

typedef struct {
    const void *next_in;
    size_t avail_in;
    uint64_t total_in;
    void *next_out;
    size_t avail_out;
    uint64_t total_out;
    char rest[136 - 48];
} lzma_stream;

// compression_stream of Apple's header.
typedef struct {
    void *dst_ptr;
    size_t dst_size;
    const void *src_ptr;
    size_t src_size;
    void *state;
} compression_stream;

enum {
    COMPRESSION_LZ4 = 0x100,
    COMPRESSION_LZ4_RAW = 0x101,
    COMPRESSION_ZLIB = 0x205,
    COMPRESSION_LZMA = 0x306,
    COMPRESSION_LZFSE = 0x801,
    COMPRESSION_BROTLI = 0xb02,
};

enum { COMPRESSION_STREAM_ENCODE, COMPRESSION_STREAM_DECODE };
enum { COMPRESSION_STREAM_FINALIZE = 1 };
enum { COMPRESSION_STATUS_OK = 0, COMPRESSION_STATUS_ERROR = -1, COMPRESSION_STATUS_END = 1 };

// A growing byte buffer.
typedef struct {
    unsigned char *data;
    size_t length, capacity;
} Buffer;

// What a stream keeps (0x170 bytes). Algorithms that cannot work on pieces (LZ4, LZ4 raw, LZFSE) collect the input in `input` and put
// the result in `output`; the others work on the caller's buffers.
typedef struct {
    int operation;
    int algorithm;
    char finalized;                 // the whole input has been taken
    char finished;                  // COMPRESSION_STATUS_END was returned
    Buffer input;
    size_t inputOffset;             // how much of `input` is used
    Buffer output;
    size_t outputOffset;            // how much of `output` was copied out
    Buffer history;                 // LZ4: the last blocks, the dictionary of the next
    char reserved[8];
    z_stream zlib;
    lzma_stream lzma;
    void *brotli;
} State;

// What was found in the host's libraries (once, when the first stream is made).
static struct {
    bool loaded;
    int (*lz4CompressDefault)(const void *, void *, int, int);
    int (*lz4Decompress)(const void *, void *, int, int);
    int (*lz4DecompressDict)(const void *, void *, int, int, const void *, int);
    int (*lz4Bound)(int);
    int (*lzmaEasyEncoder)(lzma_stream *, unsigned, int);
    int (*lzmaStreamDecoder)(lzma_stream *, uint64_t, unsigned);
    int (*lzmaCode)(lzma_stream *, int);
    void (*lzmaEnd)(lzma_stream *);
    void *(*brotliEncoderCreate)(void *, void *, void *);
    int (*brotliEncoderSetParameter)(void *, int, unsigned);
    int (*brotliEncoderCompressStream)(void *, int, size_t *, const unsigned char **, size_t *, unsigned char **, size_t *);
    int (*brotliEncoderIsFinished)(void *);
    int (*brotliEncoderHasMoreOutput)(void *);
    void (*brotliEncoderDestroy)(void *);
    void *(*brotliDecoderCreate)(void *, void *, void *);
    int (*brotliDecoderDecompressStream)(void *, size_t *, const unsigned char **, size_t *, unsigned char **, size_t *);
    void (*brotliDecoderDestroy)(void *);
} host;

int compression_stream_init(compression_stream *stream, int operation, int algorithm);
int compression_stream_process(compression_stream *stream, int flags);
int compression_stream_destroy(compression_stream *stream);

// Makes room for `more` bytes after the data (the capacity doubles from 4 KB); false when memory is short.
#define RESERVE_BODY \
    size_t needed = buffer->length + more; \
    if (needed > buffer->capacity) { \
        size_t capacity, next = buffer->capacity ? buffer->capacity : 0x1000; \
        do { \
            capacity = next; \
            next = capacity * 2; \
        } while (capacity < needed); \
        unsigned char *larger = realloc(buffer->data, capacity); \
        if (!larger) \
            return false; \
        buffer->data = larger; \
        buffer->capacity = capacity; \
    } \
    return true;

static bool reserve(Buffer *buffer, size_t more)
{
    RESERVE_BODY
}

static inline __attribute__((always_inline)) bool reserveInline(Buffer *buffer, size_t more)
{
    RESERVE_BODY
}

static inline __attribute__((always_inline)) void appendInline(Buffer *buffer, const void *data, size_t size)
{
    if (!size)
        return;
    if (!reserveInline(buffer, size))
        return;
    memcpy(buffer->data + buffer->length, data, size);
    buffer->length += size;
}

void appendBuffer(Buffer *buffer, const void *data, size_t size)
{
    if (!size)
        return;
    if (!reserve(buffer, size))
        return;
    memcpy(buffer->data + buffer->length, data, size);
    buffer->length += size;
}

// One block of the LZ4 stream format: "bv41" (raw size, packed size, data) or, when packing does not help, "bv4-" (size, data).
static bool encodeLZ4Block(State *state, const void *data, size_t size)
{
    int bound = host.lz4Bound((int)size);
    size_t length = state->output.length;
    size_t needed = bound + length + 0x14;
    unsigned char *base = state->output.data;
    if (needed > state->output.capacity) {
        size_t capacity, next = state->output.capacity ? state->output.capacity : 0x1000;
        do {
            capacity = next;
            next = capacity * 2;
        } while (capacity < needed);
        base = realloc(state->output.data, capacity);
        if (!base)
            return false;
        state->output.data = base;
        state->output.capacity = capacity;
        length = state->output.length;
    }
    uint32_t *block = (uint32_t *)(base + length);
    unsigned packed = host.lz4CompressDefault(data, base + length + 12, (int)size, bound);
    unsigned char *bytes = (unsigned char *)block;
    size_t written;
    if ((int)packed < 1 || size <= packed) {
        block[0] = 0x2d347662;                  // "bv4-"
        bytes[4] = (unsigned char)size;
        bytes[5] = (unsigned char)(size >> 8);
        bytes[6] = (unsigned char)(size >> 16);
        bytes[7] = (unsigned char)(size >> 24);
        memcpy(block + 2, data, size);
        written = size + 8;
    } else {
        block[0] = 0x31347662;                  // "bv41"
        bytes[4] = (unsigned char)size;
        bytes[5] = (unsigned char)(size >> 8);
        bytes[6] = (unsigned char)(size >> 16);
        bytes[7] = (unsigned char)(size >> 24);
        bytes[8] = (unsigned char)packed;
        bytes[9] = (unsigned char)(packed >> 8);
        bytes[10] = (unsigned char)(packed >> 16);
        bytes[11] = (unsigned char)(packed >> 24);
        written = (size_t)packed + 12;
    }
    state->output.length += written;
    return true;
}

// Decodes the whole input collected (`inputSize` bytes from the current offset) in one go; `decodedSize` is the size the blocks add up to
// (LZFSE), or unknown (LZ4 raw: the output buffer grows until it fits).
static bool decodeWhole(State *state, size_t inputSize, size_t decodedSize)
{
    const unsigned char *source = state->input.data + state->inputOffset;
    if (state->algorithm == COMPRESSION_LZ4_RAW) {
        size_t estimate = inputSize * 4 + 0x400;
        if (estimate > 0x7e000000)
            return false;
        for (;;) {
            if (!reserve(&state->output, estimate))
                return false;
            int result = host.lz4Decompress(source, state->output.data + state->output.length, (int)inputSize, (int)estimate);
            if (result >= 0) {
                decodedSize = (size_t)result;
                break;
            }
            if (!(estimate < inputSize * 0xff + 0x400 && estimate < 0x3f000001))
                return false;
            estimate *= 2;
        }
    } else if (state->algorithm == COMPRESSION_LZFSE) {
        if (!reserve(&state->output, decodedSize + 1))
            return false;
        void *scratch = malloc(lzfse_decode_scratch_size());
        size_t result = lzfse_decode_buffer(state->output.data + state->output.length, decodedSize + 1, source, inputSize, scratch);
        free(scratch);
        if (result != decodedSize)
            return false;
    } else {
        return false;
    }
    state->output.length += decodedSize;
    state->inputOffset += inputSize;
    return true;
}

static void loadHost(void)
{
    host.lz4CompressDefault = reef_host_symbol("liblz4.so.1", "LZ4_compress_default");
    host.lz4Decompress = reef_host_symbol("liblz4.so.1", "LZ4_decompress_safe");
    host.lz4DecompressDict = reef_host_symbol("liblz4.so.1", "LZ4_decompress_safe_usingDict");
    host.lz4Bound = reef_host_symbol("liblz4.so.1", "LZ4_compressBound");
    host.lzmaEasyEncoder = reef_host_symbol("liblzma.so.5", "lzma_easy_encoder");
    host.lzmaStreamDecoder = reef_host_symbol("liblzma.so.5", "lzma_stream_decoder");
    host.lzmaCode = reef_host_symbol("liblzma.so.5", "lzma_code");
    host.lzmaEnd = reef_host_symbol("liblzma.so.5", "lzma_end");
    host.brotliEncoderCreate = reef_host_symbol("libbrotlienc.so.1", "BrotliEncoderCreateInstance");
    host.brotliEncoderSetParameter = reef_host_symbol("libbrotlienc.so.1", "BrotliEncoderSetParameter");
    host.brotliEncoderCompressStream = reef_host_symbol("libbrotlienc.so.1", "BrotliEncoderCompressStream");
    host.brotliEncoderIsFinished = reef_host_symbol("libbrotlienc.so.1", "BrotliEncoderIsFinished");
    host.brotliEncoderHasMoreOutput = reef_host_symbol("libbrotlienc.so.1", "BrotliEncoderHasMoreOutput");
    host.brotliEncoderDestroy = reef_host_symbol("libbrotlienc.so.1", "BrotliEncoderDestroyInstance");
    host.brotliDecoderCreate = reef_host_symbol("libbrotlidec.so.1", "BrotliDecoderCreateInstance");
    host.brotliDecoderDecompressStream = reef_host_symbol("libbrotlidec.so.1", "BrotliDecoderDecompressStream");
    host.brotliDecoderDestroy = reef_host_symbol("libbrotlidec.so.1", "BrotliDecoderDestroyInstance");
    host.loaded = true;
}

int compression_stream_init(compression_stream *stream, int operation, int algorithm)
{
    if (operation > 1 || !stream)
        return COMPRESSION_STATUS_ERROR;
    if (!host.loaded)
        loadHost();
    if (algorithm < COMPRESSION_LZMA) {
        unsigned lz4 = algorithm - COMPRESSION_LZ4;
        if (lz4 < 2) {
            if ((host.lz4DecompressDict == 0) | (host.lz4Bound == 0) | (host.lz4CompressDefault == 0) | (host.lz4Decompress == 0))
                return COMPRESSION_STATUS_ERROR;
        } else if (algorithm != COMPRESSION_ZLIB) {
            return COMPRESSION_STATUS_ERROR;
        }
    } else if (algorithm == COMPRESSION_LZMA) {
        if ((host.lzmaCode == 0) | (host.lzmaEnd == 0) | (host.lzmaEasyEncoder == 0) | (host.lzmaStreamDecoder == 0))
            return COMPRESSION_STATUS_ERROR;
    } else if (algorithm != COMPRESSION_LZFSE) {
        if (algorithm != COMPRESSION_BROTLI)
            return COMPRESSION_STATUS_ERROR;
        if ((host.brotliDecoderCreate == 0) | (host.brotliDecoderDecompressStream == 0) | (host.brotliDecoderDestroy == 0) | (host.brotliEncoderSetParameter == 0)
            | (host.brotliEncoderIsFinished == 0) | (host.brotliEncoderDestroy == 0) | (host.brotliEncoderCreate == 0) | (host.brotliEncoderCompressStream == 0))
            return COMPRESSION_STATUS_ERROR;
        if (!host.brotliEncoderHasMoreOutput)
            return COMPRESSION_STATUS_ERROR;
    }
    State *state = calloc(1, sizeof(State));
    if (!state)
        return COMPRESSION_STATUS_ERROR;
    state->operation = operation;
    state->algorithm = algorithm;
    if (algorithm == COMPRESSION_BROTLI) {
        if (operation == COMPRESSION_STREAM_ENCODE) {
            state->brotli = host.brotliEncoderCreate(NULL, NULL, NULL);
            if (state->brotli)
                host.brotliEncoderSetParameter(state->brotli, 1, 6);
        } else {
            state->brotli = host.brotliDecoderCreate(NULL, NULL, NULL);
        }
        if (!state->brotli) {
            free(state);
            return COMPRESSION_STATUS_ERROR;
        }
    } else if (algorithm == COMPRESSION_LZMA) {
        int result = operation == COMPRESSION_STREAM_ENCODE ? host.lzmaEasyEncoder(&state->lzma, 6, 4) : host.lzmaStreamDecoder(&state->lzma, UINT64_MAX, 0);
        if (result) {
            free(state);
            return COMPRESSION_STATUS_ERROR;
        }
    } else if (algorithm == COMPRESSION_ZLIB) {
        const char *version = zlibVersion();
        int result = operation == COMPRESSION_STREAM_ENCODE ? deflateInit2_(&state->zlib, 5, 8, -15, 8, 0, version, 0x70)
                                                              : inflateInit2_(&state->zlib, -15, version, 0x70);
        if (result) {
            free(state);
            return COMPRESSION_STATUS_ERROR;
        }
    }
    stream->state = state;
    return COMPRESSION_STATUS_OK;
}

int compression_stream_process(compression_stream *stream, int flags)
{
    if (!stream)
        return COMPRESSION_STATUS_ERROR;
    State *state = stream->state;
    if (!state)
        return COMPRESSION_STATUS_ERROR;
    if (state->finished)
        return COMPRESSION_STATUS_END;
    int status;
    if (state->algorithm == COMPRESSION_ZLIB) {
        unsigned availableIn = stream->src_size < 0xffffffff ? (unsigned)stream->src_size : 0xffffffff;
        unsigned availableOut = stream->dst_size < 0xffffffff ? (unsigned)stream->dst_size : 0xffffffff;
        state->zlib.next_in = stream->src_ptr;
        state->zlib.avail_in = availableIn;
        state->zlib.next_out = stream->dst_ptr;
        state->zlib.avail_out = availableOut;
        int result = state->operation == COMPRESSION_STREAM_ENCODE ? deflate(&state->zlib, (flags & 1) << 2) : inflate(&state->zlib, 0);
        size_t used = availableIn - state->zlib.avail_in;
        stream->src_ptr = (const unsigned char *)stream->src_ptr + used;
        stream->src_size -= used;
        size_t produced = availableOut - state->zlib.avail_out;
        stream->dst_ptr = (unsigned char *)stream->dst_ptr + produced;
        stream->dst_size -= produced;
        if (result == -5)       // Z_BUF_ERROR
            return COMPRESSION_STATUS_OK;
        status = result;
    } else if (state->algorithm == COMPRESSION_LZMA) {
        state->lzma.next_in = stream->src_ptr;
        state->lzma.avail_in = stream->src_size;
        state->lzma.next_out = stream->dst_ptr;
        state->lzma.avail_out = stream->dst_size;
        int action = (flags & 1) ? (state->operation == COMPRESSION_STREAM_ENCODE) * 3 : 0;
        int result = host.lzmaCode(&state->lzma, action);
        stream->src_ptr = (const unsigned char *)stream->src_ptr + (stream->src_size - state->lzma.avail_in);
        stream->src_size = state->lzma.avail_in;
        stream->dst_ptr = (unsigned char *)stream->dst_ptr + (stream->dst_size - state->lzma.avail_out);
        stream->dst_size = state->lzma.avail_out;
        if (result == 10)       // LZMA_BUF_ERROR: nothing could be done
            return COMPRESSION_STATUS_OK;
        status = result;
    } else if (state->algorithm == COMPRESSION_BROTLI) {
        size_t availableIn = stream->src_size, availableOut = stream->dst_size;
        const unsigned char *nextIn = stream->src_ptr;
        unsigned char *nextOut = stream->dst_ptr;
        if (state->operation == COMPRESSION_STREAM_ENCODE) {
            if (!host.brotliEncoderCompressStream(state->brotli, (flags & 1) * 2, &availableIn, &nextIn, &availableOut, &nextOut, NULL)) {
                status = COMPRESSION_STATUS_ERROR;
            } else {
                status = COMPRESSION_STATUS_OK;
                if (flags & 1)
                    status = host.brotliEncoderIsFinished(state->brotli) != 0;
            }
        } else {
            int result = host.brotliDecoderDecompressStream(state->brotli, &availableIn, &nextIn, &availableOut, &nextOut, NULL);
            status = -(result == 0) | (result == 1);
        }
        stream->src_ptr = nextIn;
        stream->src_size = availableIn;
        stream->dst_ptr = nextOut;
        stream->dst_size = availableOut;
        if (status != COMPRESSION_STATUS_END)
            return status;
    } else {
        // The algorithms that work on whole blocks: take the input in.
        if (!state->finalized) {
            if (stream->src_size)
                appendInline(&state->input, stream->src_ptr, stream->src_size);
            stream->src_ptr = (const unsigned char *)stream->src_ptr + stream->src_size;
            stream->src_size = 0;
        }
        if (state->operation == COMPRESSION_STREAM_ENCODE) {
            if (state->algorithm == COMPRESSION_LZ4) {
                size_t offset = state->inputOffset;
                if (!state->finalized) {
                    while (state->input.length - offset >= 0x10000) {
                        if (!encodeLZ4Block(state, state->input.data + offset, 0x10000))
                            return COMPRESSION_STATUS_ERROR;
                        offset = state->inputOffset += 0x10000;
                    }
                }
                if (offset > 0x40000) {
                    memmove(state->input.data, state->input.data + offset, state->input.length - offset);
                    state->input.length -= state->inputOffset;
                    state->inputOffset = 0;
                    offset = 0;
                }
                if ((flags & 1) && !state->finalized) {
                    if (state->input.length != offset) {
                        if (!encodeLZ4Block(state, state->input.data + offset, state->input.length - offset))
                            return COMPRESSION_STATUS_ERROR;
                        offset = state->input.length;
                    }
                    state->inputOffset = offset;
                    uint32_t end = 0x24347662;      // "bv4$"
                    appendBuffer(&state->output, &end, 4);
                    state->finalized = 1;
                }
            } else if ((flags & 1) && !state->finalized) {
                size_t size = state->input.length;
                if (state->algorithm == COMPRESSION_LZ4_RAW) {
                    if (size > 0x7e000000)
                        return COMPRESSION_STATUS_ERROR;
                    int bound = host.lz4Bound((int)size);
                    if (!reserveInline(&state->output, bound))
                        return COMPRESSION_STATUS_ERROR;
                    int packed = host.lz4CompressDefault(state->input.data, state->output.data + state->output.length, (int)size, bound);
                    if (packed < 0 || (packed == 0 && size != 0))
                        return COMPRESSION_STATUS_ERROR;
                    state->output.length += packed;
                    state->finalized = 1;
                } else if (state->algorithm == COMPRESSION_LZFSE) {
                    size_t room = size + (size >> 4) + 0x400;
                    void *scratch = malloc(lzfse_encode_scratch_size());
                    size_t packed;
                    do {
                        if (!reserveInline(&state->output, room)) {
                            free(scratch);
                            return COMPRESSION_STATUS_ERROR;
                        }
                        packed = lzfse_encode_buffer(state->output.data + state->output.length, room, state->input.data, size, scratch);
                        room *= 2;
                    } while (packed == 0);
                    state->output.length += packed;
                    free(scratch);
                    state->finalized = 1;
                } else {
                    return COMPRESSION_STATUS_ERROR;
                }
            }
        } else if (!state->finalized) {
            if (state->algorithm == COMPRESSION_LZFSE) {
                // The blocks ("bvx-", "bvx1", "bvx2", "bvxn", the end "bvx$") tell how much they decode to; nothing is decoded before the end is in.
                const unsigned char *blocks = state->input.data + state->inputOffset;
                size_t available = state->input.length - state->inputOffset;
                size_t decoded = 0, position = 0;
                while (position + 4 <= available) {
                    uint32_t magic;
                    memcpy(&magic, blocks + position, 4);
                    uint32_t rawBytes = 0;
                    size_t blockSize;
                    if (magic == 0x24787662) {      // "bvx$"
                        if (!decodeWhole(state, position + 4, decoded))
                            return COMPRESSION_STATUS_ERROR;
                        state->finalized = 1;
                        goto deliver;
                    } else if (magic == 0x2d787662) {   // "bvx-"
                        if (available < position + 8)
                            break;
                        memcpy(&rawBytes, blocks + position + 4, 4);
                        blockSize = (size_t)rawBytes + 8;
                    } else if (magic == 0x31787662) {   // "bvx1"
                        if (available < position + 0x1c)
                            break;
                        memcpy(&rawBytes, blocks + position + 4, 4);
                        uint32_t a, b;
                        memcpy(&a, blocks + position + 0x14, 4);
                        memcpy(&b, blocks + position + 0x18, 4);
                        blockSize = (size_t)a + b + 0x304;
                    } else if (magic == 0x32787662) {   // "bvx2"
                        if (available < position + 0x20)
                            break;
                        memcpy(&rawBytes, blocks + position + 4, 4);
                        uint64_t packed1, packed2;
                        uint32_t extra;
                        memcpy(&packed1, blocks + position + 8, 8);
                        memcpy(&packed2, blocks + position + 0x10, 8);
                        memcpy(&extra, blocks + position + 0x18, 4);
                        blockSize = (uint32_t)(((packed2 >> 0x28) & 0xfffff) + ((packed1 >> 0x14) & 0xfffff) + extra);
                    } else if (magic == 0x6e787662) {   // "bvxn"
                        if (available < position + 0xc)
                            break;
                        memcpy(&rawBytes, blocks + position + 4, 4);
                        uint32_t packedBytes;
                        memcpy(&packedBytes, blocks + position + 8, 4);
                        blockSize = (size_t)packedBytes + 0xc;
                    } else {
                        return COMPRESSION_STATUS_ERROR;
                    }
                    decoded += rawBytes;
                    position += blockSize;
                    if (position > available)
                        break;
                }
            } else if (state->algorithm == COMPRESSION_LZ4) {
                size_t available = state->input.length - state->inputOffset;
                const unsigned char *blocks = state->input.data + state->inputOffset;
                size_t position = 0;
                while (available - position >= 4) {
                    const unsigned char *block = blocks + position;
                    uint32_t magic;
                    memcpy(&magic, block, 4);
                    size_t blockSize;
                    if (magic == 0x2d347662) {      // "bv4-"
                        if (available < position + 8)
                            break;
                        uint32_t size;
                        memcpy(&size, block + 4, 4);
                        if (available < position + 8 + size)
                            break;
                        if (size) {
                            appendInline(&state->output, block + 8, size);
                            appendInline(&state->history, block + 8, size);
                        }
                        if (state->history.length > 0x20000) {
                            memmove(state->history.data, state->history.data + (state->history.length - 0x10000), 0x10000);
                            state->history.length = 0x10000;
                        }
                        blockSize = (size_t)size + 8;
                    } else if (magic == 0x31347662) {   // "bv41"
                        if (available < position + 12)
                            break;
                        uint32_t packed, raw;
                        memcpy(&packed, block + 8, 4);
                        if (available < position + 12 + packed)
                            break;
                        memcpy(&raw, block + 4, 4);
                        if (!reserveInline(&state->output, raw))
                            return COMPRESSION_STATUS_ERROR;
                        size_t dictionary = state->history.length < 0x10000 ? state->history.length : 0x10000;
                        int result = host.lz4DecompressDict(block + 12, state->output.data + state->output.length, (int)packed, (int)raw,
                                                       state->history.data + state->history.length - dictionary, (int)dictionary);
                        if ((uint32_t)result != raw)
                            return COMPRESSION_STATUS_ERROR;
                        if (raw) {
                            appendInline(&state->history, state->output.data + state->output.length, raw);
                            if (state->history.length > 0x20000) {
                                memmove(state->history.data, state->history.data + (state->history.length - 0x10000), 0x10000);
                                state->history.length = 0x10000;
                            }
                        }
                        state->output.length += raw;
                        blockSize = (size_t)packed + 12;
                    } else if (magic == 0x24347662) {   // "bv4$"
                        state->inputOffset += position + 4;
                        state->finalized = 1;
                        goto deliver;
                    } else {
                        return COMPRESSION_STATUS_ERROR;
                    }
                    position += blockSize;
                    if (position > available)
                        break;
                }
                state->inputOffset += position;
                if (state->inputOffset > 0x40000) {
                    memmove(state->input.data, state->input.data + state->inputOffset, state->input.length - state->inputOffset);
                    state->input.length -= state->inputOffset;
                    state->inputOffset = 0;
                }
            } else if (flags & 1) {
                if (!decodeWhole(state, state->input.length - state->inputOffset, 0))
                    return COMPRESSION_STATUS_ERROR;
                state->finalized = 1;
            }
        }
    deliver:;
        // Hand out what is ready.
        size_t ready = state->output.length - state->outputOffset;
        if (stream->dst_size < ready)
            ready = stream->dst_size;
        memcpy(stream->dst_ptr, state->output.data + state->outputOffset, ready);
        stream->dst_ptr = (unsigned char *)stream->dst_ptr + ready;
        stream->dst_size -= ready;
        state->outputOffset += ready;
        if (state->outputOffset != state->output.length)
            return COMPRESSION_STATUS_OK;
        state->output.length = 0;
        state->outputOffset = 0;
        if (!state->finalized)
            return COMPRESSION_STATUS_OK;
        state->finished = 1;
        return COMPRESSION_STATUS_END;
    }
    if (status != 1) {
        if (status == 0)
            return COMPRESSION_STATUS_OK;
        return COMPRESSION_STATUS_ERROR;
    }
    state->finished = 1;
    return COMPRESSION_STATUS_END;
}

int compression_stream_destroy(compression_stream *stream)
{
    if (stream && stream->state) {
        State *state = stream->state;
        if (state->algorithm == COMPRESSION_BROTLI) {
            if (state->operation == COMPRESSION_STREAM_ENCODE)
                host.brotliEncoderDestroy(state->brotli);
            else
                host.brotliDecoderDestroy(state->brotli);
        } else if (state->algorithm == COMPRESSION_LZMA) {
            host.lzmaEnd(&state->lzma);
        } else if (state->algorithm == COMPRESSION_ZLIB) {
            if (state->operation == COMPRESSION_STREAM_ENCODE)
                deflateEnd(&state->zlib);
            else
                inflateEnd(&state->zlib);
        }
        free(state->input.data);
        memset(&state->input, 0, sizeof state->input);
        free(state->output.data);
        memset(&state->output, 0, sizeof state->output);
        free(state->history.data);
        free(state);
        stream->state = NULL;
    }
    return COMPRESSION_STATUS_OK;
}

size_t compression_encode_scratch_buffer_size(int algorithm)
{
    if (algorithm == COMPRESSION_LZFSE)
        return lzfse_encode_scratch_size();
    return 0;
}

size_t compression_decode_scratch_buffer_size(int algorithm)
{
    if (algorithm == COMPRESSION_LZFSE)
        return lzfse_decode_scratch_size();
    return 0;
}

// One call: LZFSE directly, the others through a stream that is run until nothing more happens.
static size_t codeBuffer(int operation, void *dst, size_t dstSize, const void *src, size_t srcSize, int algorithm)
{
    if (algorithm == COMPRESSION_LZFSE) {
        size_t result;
        if (operation == COMPRESSION_STREAM_ENCODE) {
            void *scratch = malloc(lzfse_encode_scratch_size());
            result = lzfse_encode_buffer(dst, dstSize, src, srcSize, scratch);
            free(scratch);
        } else {
            void *scratch = malloc(lzfse_decode_scratch_size());
            result = lzfse_decode_buffer(dst, dstSize, src, srcSize, scratch);
            free(scratch);
        }
        return result;
    }
    compression_stream stream = { dst, dstSize, src, srcSize, NULL };
    if (compression_stream_init(&stream, operation, algorithm) != 0)
        return 0;
    int status;
    size_t before, beforeSource;
    do {
        before = stream.dst_size;
        beforeSource = stream.src_size;
        status = compression_stream_process(&stream, COMPRESSION_STREAM_FINALIZE);
        if (status != 0 || stream.dst_size == 0)
            break;
    } while (before != stream.dst_size || beforeSource != stream.src_size);
    size_t produced = dstSize - stream.dst_size;
    compression_stream_destroy(&stream);
    size_t result = produced;
    if (status != COMPRESSION_STATUS_END)
        result = 0;
    if (operation != COMPRESSION_STREAM_ENCODE)
        result = produced;
    if (status == COMPRESSION_STATUS_ERROR)
        result = 0;
    return result;
}

size_t compression_encode_buffer(void *dst, size_t dstSize, const void *src, size_t srcSize, void *scratch, int algorithm)
{
    return codeBuffer(COMPRESSION_STREAM_ENCODE, dst, dstSize, src, srcSize, algorithm);
}

size_t compression_decode_buffer(void *dst, size_t dstSize, const void *src, size_t srcSize, void *scratch, int algorithm)
{
    return codeBuffer(COMPRESSION_STREAM_DECODE, dst, dstSize, src, srcSize, algorithm);
}
