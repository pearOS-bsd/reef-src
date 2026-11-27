// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libsasl2.2: base64 helpers, error strings, the version, the library initialisation and a client context. The output must be the same on a Mac and under reef.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct sasl_conn sasl_conn_t;
typedef struct sasl_callback { unsigned long id; int (*proc)(void); void *context; } sasl_callback_t;
int sasl_encode64(const char *, unsigned, char *, unsigned, unsigned *);
int sasl_decode64(const char *, unsigned, char *, unsigned, unsigned *);
const char *sasl_errstring(int, const char *, const char **);
void sasl_version(const char **, int *);
int sasl_client_init(const sasl_callback_t *);
int sasl_client_new(const char *, const char *, const char *, const char *, const sasl_callback_t *, unsigned, sasl_conn_t **);
void sasl_dispose(sasl_conn_t **);
void sasl_done(void);
int main(void)
{
    char out[128], back[128]; unsigned n = 0, m = 0;
    int rc = sasl_encode64("hello sasl", 10, out, sizeof out, &n);
    printf("encode64 %d '%s' %u\n", rc, out, n);
    rc = sasl_decode64(out, n, back, sizeof back, &m); back[m] = 0;
    printf("decode64 %d '%s' %u\n", rc, back, m);
    rc = sasl_decode64("!!!", 3, back, sizeof back, &m);
    printf("decode64 bad %d\n", rc);
    for (int code = 0; code >= -7; code--) printf("error %d: %s\n", code, sasl_errstring(code, 0, 0));
    int major = 0; sasl_version(0, &major);
    printf("version major %d\n", major >> 24);
    sasl_callback_t none[] = { { 0, 0, 0 } };
    printf("client_init %d\n", sasl_client_init(none));
    sasl_conn_t *conn = 0;
    rc = sasl_client_new("imap", "mail.example.com", 0, 0, none, 0, &conn);
    printf("client_new %d, %s\n", rc, conn ? "context" : "no context");
    if (conn) sasl_dispose(&conn);
    sasl_done();
    return 0;
}
