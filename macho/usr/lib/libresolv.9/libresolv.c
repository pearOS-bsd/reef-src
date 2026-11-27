// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libresolv: the resolver state is set up here from /etc/resolv.conf (as Apple's res_ninit does); the queries are answered by the host's
// resolver library, whose h_errno is copied to ours. Names are packed and unpacked here.

#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// The table of the host's services (in libSystem): only the function that finds a symbol of the host's libraries is used.
struct ReefHost {
    char reserved[0x250];
    void *(*symbol)(const char *library, const char *name);
};
extern struct ReefHost *reef_host;

#define HOST_LIBRARIES "libc.so.6:libresolv.so.2"

#define RES_INIT 0x00000001
#define RES_USEVC 0x00000008
#define RES_ROTATE 0x00004000
#define RES_USE_EDNS0 0x40000000
#define MAXNS 3
#define MAXDNSRCH 6

union res_sockaddr_union {
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
    char pad[128];
};

struct res_9_state_ext {
    union res_sockaddr_union nsaddrs[MAXNS];
};

// Apple's struct __res_9_state (0x228 bytes).
typedef struct __res_9_state {
    int retrans;
    int retry;
    unsigned long options;
    int nscount;
    struct sockaddr_in nsaddr_list[MAXNS];
    unsigned short id;
    char *dnsrch[MAXDNSRCH + 1];
    char defdname[256];
    unsigned long pfcode;
    unsigned ndots : 4;
    unsigned nsort : 4;
    char unused[3];
    struct {
        struct in_addr addr;
        uint32_t mask;
    } sort_list[10];
    void *qhook;
    void *rhook;
    int res_h_errno;
    int _vcsock;
    unsigned _flags;
    unsigned _pad;
    union {
        char pad[72 - 4 * sizeof(int) - 2 * sizeof(void *)];
        struct {
            uint16_t nscount;
            uint16_t nstimes[MAXNS];
            int nssocks[MAXNS];
            struct res_9_state_ext *ext;
        } _ext;
    } _u;
} *res_state;

static void *hostFunction(const char *name)
{
    return reef_host->symbol(HOST_LIBRARIES, name);
}

// h_errno of the host's resolver, or NO_RECOVERY when it has none.
static int hostHErrno(void)
{
    int *(*location)(void) = hostFunction("__h_errno_location");
    return location ? *location() : 3;
}

int res_9_init(void)
{
    int (*function)(void) = hostFunction("__res_init");
    return function ? function() : 0;
}

int res_9_query(const char *name, int class, int type, unsigned char *answer, int length)
{
    int (*function)(const char *, int, int, unsigned char *, int) = hostFunction("res_query");
    int result = function(name, class, type, answer, length);
    if (result < 0)
        h_errno = hostHErrno();
    return result;
}

int res_9_search(const char *name, int class, int type, unsigned char *answer, int length)
{
    int (*function)(const char *, int, int, unsigned char *, int) = hostFunction("res_search");
    int result = function(name, class, type, answer, length);
    if (result < 0)
        h_errno = hostHErrno();
    return result;
}

int res_9_querydomain(const char *name, const char *domain, int class, int type, unsigned char *answer, int length)
{
    int (*function)(const char *, const char *, int, int, unsigned char *, int) = hostFunction("res_querydomain");
    int result = function(name, domain, class, type, answer, length);
    if (result < 0)
        h_errno = hostHErrno();
    return result;
}

int res_9_send(const unsigned char *message, int length, unsigned char *answer, int answerLength)
{
    int (*function)(const unsigned char *, int, unsigned char *, int) = hostFunction("res_send");
    int result = function(message, length, answer, answerLength);
    if (result < 0)
        h_errno = hostHErrno();
    return result;
}

// Adds a name server (an IPv4 or IPv6 address, with a %scope for link-local ones); at most three.
static void addServer(res_state state, const char *text)
{
    if (state->nscount > 2)
        return;
    union res_sockaddr_union address = {0};
    char buffer[62];
    snprintf(buffer, sizeof buffer, "%s", text);
    char *scope = strchr(buffer, '%');
    if (scope)
        *scope = 0;
    if (inet_pton(AF_INET, buffer, &address.sin.sin_addr) == 1) {
        address.sin.sin_len = sizeof(struct sockaddr_in);
        address.sin.sin_family = AF_INET;
        address.sin.sin_port = htons(53);
        state->nsaddr_list[state->nscount] = address.sin;
    } else if (inet_pton(AF_INET6, buffer, &address.sin6.sin6_addr) == 1) {
        address.sin6.sin6_len = sizeof(struct sockaddr_in6);
        address.sin6.sin6_family = AF_INET6;
        address.sin6.sin6_port = htons(53);
        if (scope)
            address.sin6.sin6_scope_id = if_nametoindex(scope + 1);
    } else {
        return;
    }
    state->_u._ext.ext->nsaddrs[state->nscount++] = address;
}

// The search list: up to six words in the default domain buffer.
static void setSearchList(res_state state, char *words)
{
    bzero(state->dnsrch, sizeof state->dnsrch + sizeof state->defdname);
    size_t used = 0;
    unsigned long i = 0;
    char *word = strtok(words, " \t\r\n");
    while (word) {
        size_t length = strlen(word);
        size_t next = used + length + 1;
        if (next > sizeof state->defdname)
            break;
        char *place = state->defdname + used;
        memcpy(place, word, length);
        state->dnsrch[i] = place;
        word = strtok(NULL, " \t\r\n");
        if (!word)
            break;
        used = next;
        if (!(i++ < 5))
            break;
    }
}

int res_9_ninit(res_state state)
{
    char line[1024];
    int (*hostInit)(void) = hostFunction("__res_init");
    if (hostInit)
        hostInit();
    if (state->options & RES_INIT)
        free(state->_u._ext.ext);
    bzero(state, sizeof *state);
    state->_u._ext.ext = calloc(1, sizeof(struct res_9_state_ext));
    if (!state->_u._ext.ext)
        return -1;
    state->retrans = 5;
    state->retry = 2;
    state->ndots = 1;
    state->options = 0x2c0;
    state->_vcsock = -1;
    FILE *file = fopen("/etc/resolv.conf", "r");
    if (file) {
        while (fgets(line, sizeof line, file)) {
            char *key = line;
            while (isspace((unsigned char)*key))
                key++;
            size_t keyLength = strcspn(key, " \t\r\n");
            char *value = key + keyLength;
            if (*value)
                *value++ = 0;
            if (strcmp(key, "nameserver") == 0) {
                value += strspn(value, " \t");
                value[strcspn(value, " \t\r\n#;")] = 0;
                addServer(state, value);
            } else if (strcmp(key, "search") == 0 || strcmp(key, "domain") == 0) {
                value[strcspn(value, "#;")] = 0;
                setSearchList(state, value);
            } else if (strcmp(key, "options") == 0) {
                for (char *option = strtok(value, " \t\r\n"); option; option = strtok(NULL, " \t\r\n")) {
                    if (strncmp(option, "ndots:", 6) == 0) {
                        int n = atoi(option + 6);
                        state->ndots = n > 15 ? 15 : n;
                    } else if (strncmp(option, "timeout:", 8) == 0) {
                        int n = atoi(option + 8);
                        state->retrans = n > 30 ? 30 : n;
                    } else if (strncmp(option, "attempts:", 9) == 0) {
                        int n = atoi(option + 9);
                        state->retry = n > 5 ? 5 : n;
                    } else if (strcmp(option, "rotate") == 0) {
                        state->options |= RES_ROTATE;
                    } else if (strcmp(option, "edns0") == 0) {
                        state->options |= RES_USE_EDNS0;
                    } else if (strcmp(option, "use-vc") == 0) {
                        state->options |= RES_USEVC;
                    }
                }
            }
        }
        fclose(file);
    }
    if (state->nscount == 0) {
        addServer(state, "127.0.0.1");
        addServer(state, "::1");
    }
    if (!state->dnsrch[0]) {
        if (gethostname(line, 256) == 0) {
            line[255] = 0;
            char *dot = strchr(line, '.');
            if (dot && dot[1])
                setSearchList(state, dot + 1);
        }
    }
    state->_u._ext.nscount = state->nscount;
    state->options |= RES_INIT;
    return 0;
}

void res_9_ndestroy(res_state state)
{
    free(state->_u._ext.ext);
    state->_u._ext.ext = NULL;
    state->options &= ~(unsigned long)RES_INIT;
}

int res_9_getservers(res_state state, union res_sockaddr_union *servers, int count)
{
    if (!state->_u._ext.ext)
        return 0;
    int i;
    for (i = 0; i < count; i++) {
        if (i >= state->nscount)
            break;
        servers[i] = state->_u._ext.ext->nsaddrs[i];
    }
    return i;
}

void res_9_nclose(res_state state)
{
}

int res_9_nquery(res_state state, const char *name, int class, int type, unsigned char *answer, int length)
{
    return res_9_query(name, class, type, answer, length);
}

int res_9_nsearch(res_state state, const char *name, int class, int type, unsigned char *answer, int length)
{
    return res_9_search(name, class, type, answer, length);
}

int res_9_nquerydomain(res_state state, const char *name, const char *domain, int class, int type, unsigned char *answer, int length)
{
    return res_9_querydomain(name, domain, class, type, answer, length);
}

int res_9_nsend(res_state state, const unsigned char *message, int length, unsigned char *answer, int answerLength)
{
    return res_9_send(message, length, answer, answerLength);
}

// Packs a dotted name into labels (no compression).
int res_9_dn_comp(const char *name, unsigned char *buffer, int size, unsigned char **pointers, unsigned char **last)
{
    if (name[0] == '.' && name[1] == 0)
        name = "";
    int n = 0;
    while (*name) {
        const char *dot = strchr(name, '.');
        long length = dot ? dot - name : (long)strlen(name);
        if ((unsigned long)(length - 1) > 62)
            return -1;
        int next = (int)(n + length + 1);
        if (next >= size)
            return -1;
        buffer[n++] = (unsigned char)length;
        memcpy(buffer + n, name, length);
        n = next;
        name = dot + 1;
        if (!dot)
            break;
    }
    if (n >= size)
        return -1;
    buffer[n++] = 0;
    return n;
}

// Unpacks a name, following compression pointers (at most 64 of them); returns the length of the packed name where it starts.
int res_9_dn_expand(const unsigned char *message, const unsigned char *end, const unsigned char *source, char *buffer, int size)
{
    const unsigned char *current = source;
    int used = 0, jumps = 0, packed = -1;
    if (current >= end)
        return -1;
    for (;;) {
        unsigned c = *current;
        unsigned kind = c & 0xc0;
        if (kind == 0xc0) {
            if (current + 1 >= end || jumps > 0x3f)
                return -1;
            jumps++;
            if (packed < 0)
                packed = (int)(current - source) + 2;
            current = message + (((c & 0x3f) << 8) | current[1]);
        } else if (kind == 0) {
            current++;
            if (c == 0)
                break;
            const unsigned char *next = current + c;
            if (next > end)
                return -1;
            if (!(used == 0 || used + 1 < size))
                return -1;
            int start = 0;
            if (used) {
                buffer[used] = '.';
                start = used + 1;
            }
            if ((int)c + start >= size)
                return -1;
            memcpy(buffer + start, current, c);
            used = c + start;
            current = next;
        } else {
            return -1;
        }
        if (current >= end)
            return -1;
    }
    if (used >= size)
        return -1;
    buffer[used] = 0;
    return packed >= 0 ? packed : (int)(current - source);
}

// Builds a standard query: a header with a fresh id and recursion desired, then the name, type and class.
int res_9_mkquery(int op, const char *name, int class, int type, const unsigned char *data, int dataLength, const void *newRecord,
                  unsigned char *buffer, int size)
{
    static unsigned short nextID = 1;
    if (op != 0 || size < 12)
        return -1;
    unsigned short id = nextID++;
    memset(buffer + 3, 0, 9);
    buffer[0] = id >> 8;
    buffer[1] = (unsigned char)id;
    buffer[2] = 1;
    buffer[5] = 1;
    if (name[0] == '.' && name[1] == 0)
        name = "";
    unsigned char *packed = buffer + 12;
    size -= 16;
    int n = 0;
    while (*name) {
        const char *dot = strchr(name, '.');
        long length = dot ? dot - name : (long)strlen(name);
        if ((unsigned long)(length - 1) > 62)
            return -1;
        int next = (int)(n + length + 1);
        if (next >= size)
            return -1;
        packed[n++] = (unsigned char)length;
        memcpy(packed + n, name, length);
        n = next;
        name = dot + 1;
        if (!dot)
            break;
    }
    if (n >= size)
        return -1;
    packed[n] = 0;
    if (n < -1)
        return -1;
    packed[n + 1] = type >> 8;
    packed[n + 2] = (unsigned char)type;
    packed[n + 3] = class >> 8;
    packed[n + 4] = (unsigned char)class;
    return n + 17;
}
