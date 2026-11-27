// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libcompression: every algorithm through the buffer functions (and back), through the stream functions in small pieces, a corrupt input, and the
// scratch sizes. The sizes of the packed data are printed: they are the same only if both libraries pack alike.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { void *dst_ptr; size_t dst_size; const void *src_ptr; size_t src_size; void *state; } Stream;
size_t compression_encode_buffer(void *, size_t, const void *, size_t, void *, int);
size_t compression_decode_buffer(void *, size_t, const void *, size_t, void *, int);
size_t compression_encode_scratch_buffer_size(int);
size_t compression_decode_scratch_buffer_size(int);
int compression_stream_init(Stream *, int, int);
int compression_stream_process(Stream *, int);
int compression_stream_destroy(Stream *);

static const int algorithms[] = { 0x100, 0x101, 0x205, 0x306, 0x801, 0xb02 };

static unsigned char *makeText(size_t n)
{
    unsigned char *t = malloc(n);
    unsigned x = 12345;
    for (size_t i = 0; i < n; i++) {
        x = x * 1103515245 + 12345;
        t[i] = (i % 97 < 60) ? "the quick brown fox jumps over the lazy dog. "[i % 45] : (unsigned char)(x >> 16);
    }
    return t;
}

int main(void)
{
    size_t sizes[] = { 0, 1, 100, 70000, 300000 };
    for (int a = 0; a < 6; a++) {
        int alg = algorithms[a];
        printf("algorithm %x scratch %zu %zu\n", alg, compression_encode_scratch_buffer_size(alg), compression_decode_scratch_buffer_size(alg));
        for (int s = 0; s < 5; s++) {
            size_t n = sizes[s];
            unsigned char *text = makeText(n ? n : 1);
            size_t cap = n + n / 8 + 4096;
            unsigned char *packed = malloc(cap), *back = malloc(n + 16);
            size_t pn = compression_encode_buffer(packed, cap, text, n, 0, alg);
            size_t bn = pn ? compression_decode_buffer(back, n + 16, packed, pn, 0, alg) : 0;
            printf("  size %zu: packed %zu, back %zu same %d\n", n, pn, bn, bn == n && !memcmp(back, text, n));
            // stream: pieces of 777 bytes in, 500 out
            Stream st = { 0 };
            unsigned char *out = malloc(cap), *dec = malloc(n + 16);
            size_t outn = 0, decn = 0;
            if (compression_stream_init(&st, 0, alg) == 0) {
                size_t pos = 0; int rc;
                do {
                    size_t chunk = n - pos < 777 ? n - pos : 777;
                    st.src_ptr = text + pos; st.src_size = chunk;
                    do {
                        st.dst_ptr = out + outn; st.dst_size = 500;
                        rc = compression_stream_process(&st, pos + chunk == n ? 1 : 0);
                        outn += 500 - st.dst_size;
                    } while (rc == 0 && (st.dst_size == 0 || st.src_size));
                    pos += chunk - st.src_size;
                } while (rc == 0 && pos < n + 1);
                compression_stream_destroy(&st);
                Stream sd = { 0 };
                if (compression_stream_init(&sd, 1, alg) == 0) {
                    size_t ip = 0; int rd = 0;
                    while (rd == 0 && ip <= outn) {
                        size_t chunk = outn - ip < 333 ? outn - ip : 333;
                        sd.src_ptr = out + ip; sd.src_size = chunk;
                        do {
                            sd.dst_ptr = dec + decn; sd.dst_size = 400 < n + 16 - decn ? 400 : n + 16 - decn;
                            size_t before = sd.dst_size;
                            rd = compression_stream_process(&sd, ip + chunk == outn ? 1 : 0);
                            decn += before - sd.dst_size;
                        } while (rd == 0 && (sd.dst_size == 0 || sd.src_size));
                        ip += chunk - sd.src_size;
                        if (chunk == 0) break;
                    }
                    compression_stream_destroy(&sd);
                    printf("  stream: packed %zu, back %zu same %d, status %d\n", outn, decn, decn == n && !memcmp(dec, text, n), rd);
                }
            } else {
                printf("  stream init failed\n");
            }
            free(text); free(packed); free(back); free(out); free(dec);
        }
        unsigned char junk[64] = "this is not compressed data at all, 1234567890";
        unsigned char sink[256];
        printf("  corrupt: %zu\n", compression_decode_buffer(sink, sizeof sink, junk, sizeof junk, 0, alg));
        printf("  tiny output: %zu\n", compression_encode_buffer(sink, 4, junk, sizeof junk, 0, alg));
    }
    Stream bad = { 0 };
    printf("bad algorithm %d, bad operation %d, null %d\n", compression_stream_init(&bad, 0, 0x999), compression_stream_init(&bad, 2, 0x205), compression_stream_init(0, 0, 0x205));
    printf("process null %d destroy null %d\n", compression_stream_process(0, 0), compression_stream_destroy(0));
    return 0;
}
