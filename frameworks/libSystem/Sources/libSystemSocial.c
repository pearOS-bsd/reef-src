// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// The function behind isfinite(double) in <math.h>.
int __isfinited(double x) { return isfinite(x); }

// gcvt(3): like %g with ndigit significant digits; exponent form keeps the point ("1.e+10"), as observed on macOS 26.
char *gcvt(double value, int ndigit, char *buf) {
    snprintf(buf, 512, "%.*g", ndigit < 1 ? 1 : ndigit, value);
    char *e = strchr(buf, 'e');
    if (e && !memchr(buf, '.', (size_t)(e - buf))) { memmove(e + 1, e, strlen(e) + 1); *e = '.'; }
    return buf;
}

// mkostemp(3): mkstemp with extra open flags (O_APPEND, O_CLOEXEC, O_SYNC ...).
int mkostemp(char *tmpl, int oflags) {
    static const char set[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t len = strlen(tmpl);
    if (len < 6 || strcmp(tmpl + len - 6, "XXXXXX") != 0) { errno = EINVAL; return -1; }
    for (int attempt = 0; attempt < 10000; attempt++) {
        for (int i = 0; i < 6; i++) tmpl[len - 6 + i] = set[arc4random_uniform(sizeof set - 1)];
        int fd = open(tmpl, O_CREAT | O_EXCL | O_RDWR | oflags, 0600);
        if (fd >= 0 || errno != EEXIST) return fd;
    }
    errno = EEXIST;
    return -1;
}

// sbrk(2): a break inside one reserved region (the kernel has no program break for a Mach-O process).
#define SBRK_REGION (4UL << 30)
static char *sbrk_base, *sbrk_cur;
void *sbrk(int incr) {
    if (!sbrk_base) {
        void *p = mmap(NULL, SBRK_REGION, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        if (p == MAP_FAILED) return (void *)-1;
        sbrk_base = sbrk_cur = p;
    }
    char *old = sbrk_cur;
    if ((incr >= 0 && (size_t)incr > SBRK_REGION - (size_t)(old - sbrk_base)) || (incr < 0 && (size_t)-(long)incr > (size_t)(old - sbrk_base))) {
        errno = ENOMEM;
        return (void *)-1;
    }
    sbrk_cur += incr;
    return old;
}

// rcmd(3): run cmd on *ahost over a connection from a reserved port (the rsh protocol).
int rcmd(char **ahost, int rport, const char *locuser, const char *remuser, const char *cmd, int *fd2p) {
    struct addrinfo hints = {0}, *res;
    char service[16];
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    snprintf(service, sizeof service, "%d", ntohs((unsigned short)rport) ? ntohs((unsigned short)rport) : rport);
    if (getaddrinfo(*ahost, service, &hints, &res) != 0) return -1;
    int s = -1, lport = 1023;
    for (; lport > 512; lport--) {
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0) break;
        struct sockaddr_in me = {0};
        me.sin_family = AF_INET;
        me.sin_port = htons((unsigned short)lport);
        if (bind(s, (struct sockaddr *)&me, sizeof me) == 0) break;
        close(s);
        s = -1;
        if (errno != EADDRINUSE) break;
    }
    if (s < 0) { freeaddrinfo(res); return -1; }
    if (connect(s, res->ai_addr, res->ai_addrlen) < 0) { close(s); freeaddrinfo(res); return -1; }
    freeaddrinfo(res);
    int s2 = -1;
    if (fd2p == NULL) {
        write(s, "", 1);
    } else {
        // a second connection back to us for stderr
        int ls = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in me = {0};
        socklen_t l = sizeof me;
        me.sin_family = AF_INET;
        if (ls < 0 || bind(ls, (struct sockaddr *)&me, sizeof me) < 0 || getsockname(ls, (struct sockaddr *)&me, &l) < 0 || listen(ls, 1) < 0) { close(s); if (ls >= 0) close(ls); return -1; }
        char num[16];
        snprintf(num, sizeof num, "%d", ntohs(me.sin_port));
        write(s, num, strlen(num) + 1);
        s2 = accept(ls, NULL, NULL);
        close(ls);
        if (s2 < 0) { close(s); return -1; }
        *fd2p = s2;
    }
    write(s, locuser, strlen(locuser) + 1);
    write(s, remuser, strlen(remuser) + 1);
    write(s, cmd, strlen(cmd) + 1);
    char c;
    if (read(s, &c, 1) != 1 || c != 0) { close(s); if (s2 >= 0) close(s2); return -1; }
    return s;
}

// pthread_key_init_np(3): make the reserved key number "key" a live key with this destructor. The thread keys of
// Reef's host are handed out in order from 0, so the keys below are taken and given back until "key" comes up.
#define KEY_MAX 1024
int pthread_key_init_np(int key, void (*destructor)(void *)) {
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    static unsigned char done[KEY_MAX];
    if (key < 0 || key >= KEY_MAX) return EINVAL;
    pthread_mutex_lock(&lock);
    int res = 0;
    if (!done[key]) {
        pthread_key_t taken[KEY_MAX];
        int n = 0;
        for (;;) {
            pthread_key_t k;
            res = pthread_key_create(&k, destructor);
            if (res) break;
            if ((int)k == key) { done[key] = 1; break; }
            if ((int)k > key) { pthread_key_delete(k); res = EINVAL; break; }
            taken[n++] = k;
        }
        while (n > 0) pthread_key_delete(taken[--n]);
    }
    pthread_mutex_unlock(&lock);
    return res;
}
