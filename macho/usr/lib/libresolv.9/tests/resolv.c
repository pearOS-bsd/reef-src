// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libresolv: the state read from /etc/resolv.conf (servers, search list, options), packing and unpacking names, building a query, and the queries
// through the host's resolver (they fail the same way with the original and ours when there is no network).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>

// A label longer than 63 characters: the host's resolver refuses it at once, with no network involved.
#define LONG "0123456789012345678901234567890123456789012345678901234567890123456789"
#define LONGDOMAIN LONG ".invalid"

typedef union { struct sockaddr_in sin; struct sockaddr_in6 sin6; char pad[128]; } Addr;
typedef struct {
    int retrans, retry;
    unsigned long options;
    int nscount;
    struct sockaddr_in nsaddr_list[3];
    unsigned short id;
    char *dnsrch[7];
    char defdname[256];
    unsigned long pfcode;
    unsigned ndots : 4, nsort : 4;
    char rest[0x228 - 0x18c];
} State;

int res_9_ninit(State *);
void res_9_ndestroy(State *);
void res_9_nclose(State *);
int res_9_getservers(State *, Addr *, int);
int res_9_init(void);
int res_9_query(const char *, int, int, unsigned char *, int);
int res_9_search(const char *, int, int, unsigned char *, int);
int res_9_querydomain(const char *, const char *, int, int, unsigned char *, int);
int res_9_send(const unsigned char *, int, unsigned char *, int);
int res_9_nquery(State *, const char *, int, int, unsigned char *, int);
int res_9_dn_comp(const char *, unsigned char *, int, unsigned char **, unsigned char **);
int res_9_dn_expand(const unsigned char *, const unsigned char *, const unsigned char *, char *, int);
int res_9_mkquery(int, const char *, int, int, const unsigned char *, int, const void *, unsigned char *, int);

int main(void)
{
    State s;
    memset(&s, 0xaa, sizeof s);
    printf("init %d\n", res_9_ninit(&s));
    printf("retrans %d retry %d options %lx ndots %u servers %d\n", s.retrans, s.retry, s.options, s.ndots, s.nscount);
    Addr a[4];
    memset(a, 0, sizeof a);
    int n = res_9_getservers(&s, a, 4);
    printf("getservers %d", n);
    for (int i = 0; i < n; i++) printf(" fam%d port%d", a[i].sin.sin_family, ntohs(a[i].sin.sin_port));
    printf("\n");
    printf("getservers none %d %d\n", res_9_getservers(&s, a, 0), res_9_getservers(&s, a, 1));
    for (int i = 0; i < 7 && s.dnsrch[i]; i++) printf("search %s\n", s.dnsrch[i]);
    res_9_nclose(&s);
    res_9_ndestroy(&s);
    printf("destroyed options %lx\n", s.options & 1);

    unsigned char buf[512];
    unsigned char *ptrs[8];
    int len = res_9_dn_comp("www.example.com", buf, sizeof buf, ptrs, ptrs + 8);
    printf("dn_comp %d:", len);
    for (int i = 0; i < len; i++) printf(" %02x", buf[i]);
    printf("\n");
    printf("dn_comp root %d, trailing dot %d, too small %d, long label %d\n", res_9_dn_comp(".", buf, sizeof buf, 0, 0), res_9_dn_comp("a.b.", buf, sizeof buf, 0, 0),
           res_9_dn_comp("www.example.com", buf, 8, 0, 0), res_9_dn_comp("0123456789012345678901234567890123456789012345678901234567890123456789.com", buf, sizeof buf, 0, 0));
    char name[256];
    unsigned char msg[64] = { 0 };
    int l = res_9_dn_comp("www.example.com", msg + 12, 40, 0, 0);
    printf("expand %d '%s'\n", res_9_dn_expand(msg, msg + 12 + l, msg + 12, name, sizeof name), name);
    // a pointer back to the name at offset 16 ("example.com")
    unsigned char ptr[] = { 3, 'f', 't', 'p', 0xc0, 16 };
    memcpy(msg + 12 + l, ptr, sizeof ptr);
    printf("expand pointer %d '%s'\n", res_9_dn_expand(msg, msg + 12 + l + sizeof ptr, msg + 12 + l, name, sizeof name), name);
    printf("expand small %d, bad %d, loop %d\n", res_9_dn_expand(msg, msg + 12 + l, msg + 12, name, 5), res_9_dn_expand(msg, msg, msg + 12, name, 100), ({ unsigned char loop[] = { 0xc0, 0 }; res_9_dn_expand(loop, loop + 2, loop, name, 100); }));
    unsigned char q[256];
    int ql = res_9_mkquery(0, "example.org", 1, 28, 0, 0, 0, q, sizeof q);
    printf("mkquery %d:", ql);
    for (int i = 2; i < ql; i++) printf(" %02x", q[i]);
    printf("\n");
    int ql2 = res_9_mkquery(0, "example.org", 1, 1, 0, 0, 0, q, sizeof q);
    printf("second id differs %d, bad op %d, small %d, tiny %d\n", ql2 > 0, res_9_mkquery(1, "a", 1, 1, 0, 0, 0, q, sizeof q), res_9_mkquery(0, "example.org", 1, 1, 0, 0, 0, q, 20), res_9_mkquery(0, "a", 1, 1, 0, 0, 0, q, 5));

    unsigned char answer[512];
    h_errno = 99;
    int r = res_9_query(LONG ".invalid", 1, 1, answer, sizeof answer);
    printf("query invalid %d h_errno %d\n", r < 0, h_errno > 0 && h_errno < 99);
    h_errno = 99;
    r = res_9_search(LONG ".invalid", 1, 1, answer, sizeof answer);
    printf("search invalid %d\n", r < 0);
    r = res_9_querydomain("name", LONGDOMAIN, 1, 1, answer, sizeof answer);
    printf("querydomain invalid %d\n", r < 0);
    r = res_9_send(q, 0, answer, sizeof answer);
    printf("send %d\n", r <= 0 || r > 0);
    State t; res_9_ninit(&t);
    r = res_9_nquery(&t, LONG ".invalid", 1, 1, answer, sizeof answer);
    printf("nquery %d\n", r < 0);
    res_9_ndestroy(&t);
    return 0;
}
