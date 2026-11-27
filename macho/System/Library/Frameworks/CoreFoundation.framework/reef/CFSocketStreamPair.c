// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Socket streams: a read and a write stream over one TCP connection (host and port resolved with getaddrinfo, or a native socket, or a socket
// signature), with TLS through the host's libssl (reef_host_symbol).  A state machine (_CFSocketStreamAdvance) moves from resolving and
// connecting to the TLS handshake and the open state; a CFSocket reports readiness.
#include "CFInternal.h"
#include "CFStreamAbstract.h"
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

extern void *reef_host_symbol(const char *library, const char *symbol);

enum { stateResolving, stateConnecting, stateHandshake, stateOpen, stateError, stateClosed };

typedef struct {
    pthread_mutex_t lock;
    int references;
    int fd;
    Boolean closeOnClose;
    CFStringRef host;
    uint32_t port;
    int state;
    CFErrorRef error;
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
    Boolean readOpened, writeOpened, readClosed, writeClosed, atEOF;
    CFTypeRef securityLevel;
    CFDictionaryRef sslSettings;
    void *sslContext;
    void *ssl;
    CFSocketRef socket;
    CFMutableArrayRef schedules;
    struct addrinfo *addresses;
    struct addrinfo *current;
} Pair;

// The host's libssl, found when first needed.
static struct {
    void *(*clientMethod)(void);
    void *(*contextNew)(void *method);
    void (*contextFree)(void *ctx);
    int (*setDefaultVerifyPaths)(void *ctx);
    void (*setVerify)(void *ctx, int mode, void *callback);
    void *(*sslNew)(void *ctx);
    void (*sslFree)(void *ssl);
    int (*setFd)(void *ssl, int fd);
    long (*ctrl)(void *ssl, int cmd, long larg, void *parg);
    int (*setHost)(void *ssl, const char *name);
    int (*connect)(void *ssl);
    int (*read)(void *ssl, void *buffer, int length);
    int (*write)(void *ssl, const void *buffer, int length);
    int (*getError)(void *ssl, int result);
    int (*shutdown)(void *ssl);
    int (*pending)(void *ssl);
    long (*verifyResult)(void *ssl);
    Boolean available;
} ssl;
static pthread_once_t sslOnce = PTHREAD_ONCE_INIT;

static void resolveSSL(void) {
#define SYMBOL(field, name) if (!(ssl.field = reef_host_symbol("libssl.so.3", name))) return;
    SYMBOL(clientMethod, "TLS_client_method")
    SYMBOL(contextNew, "SSL_CTX_new")
    SYMBOL(contextFree, "SSL_CTX_free")
    SYMBOL(setDefaultVerifyPaths, "SSL_CTX_set_default_verify_paths")
    SYMBOL(setVerify, "SSL_CTX_set_verify")
    SYMBOL(sslNew, "SSL_new")
    SYMBOL(sslFree, "SSL_free")
    SYMBOL(setFd, "SSL_set_fd")
    SYMBOL(ctrl, "SSL_ctrl")
    SYMBOL(setHost, "SSL_set1_host")
    SYMBOL(connect, "SSL_connect")
    SYMBOL(read, "SSL_read")
    SYMBOL(write, "SSL_write")
    SYMBOL(getError, "SSL_get_error")
    SYMBOL(shutdown, "SSL_shutdown")
    SYMBOL(pending, "SSL_pending")
    SYMBOL(verifyResult, "SSL_get_verify_result")
#undef SYMBOL
    ssl.available = true;
}

static void closeConnection(Pair *p) {
    if (p->socket) {
        CFSocketInvalidate(p->socket);
        CFRelease(p->socket);
        p->socket = NULL;
    }
    if (p->ssl) {
        ssl.shutdown(p->ssl);
        ssl.sslFree(p->ssl);
        p->ssl = NULL;
    }
    if (p->sslContext) {
        ssl.contextFree(p->sslContext);
        p->sslContext = NULL;
    }
    if (p->fd >= 0 && p->closeOnClose) close(p->fd);
    p->fd = -1;
    if (p->addresses) {
        freeaddrinfo(p->addresses);
        p->addresses = NULL;
    }
}

static void releasePair(Pair *p) {
    pthread_mutex_lock(&p->lock);
    int before = p->references--;
    pthread_mutex_unlock(&p->lock);
    if (before > 1) return;
    closeConnection(p);
    if (p->host) CFRelease(p->host);
    if (p->error) CFRelease(p->error);
    if (p->securityLevel) CFRelease(p->securityLevel);
    if (p->sslSettings) CFRelease(p->sslSettings);
    if (p->schedules) CFRelease(p->schedules);
    pthread_mutex_destroy(&p->lock);
    free(p);
}

static void setError(Pair *p, CFStringRef domain, CFIndex code) {
    if (p->error) CFRelease(p->error);
    p->error = CFErrorCreate(kCFAllocatorSystemDefault, domain, code, NULL);
    p->state = stateError;
}

// Tries the next address of the list: starts a non-blocking connect.
static void connectNext(Pair *p) {
    for (;;) {
        struct addrinfo *a = p->current;
        if (!a) {
            setError(p, kCFErrorDomainPOSIX, ECONNREFUSED);
            return;
        }
        p->current = a->ai_next;
        int fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        if (fd < 0) continue;
        fcntl(fd, F_SETFD, 1);
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
        int one = 1;
        setsockopt(fd, SOL_SOCKET, 0x1022, &one, sizeof(one));
        if (connect(fd, a->ai_addr, a->ai_addrlen) == 0 || errno == EINPROGRESS) {
            p->fd = fd;
            p->closeOnClose = true;
            p->state = stateConnecting;
            return;
        }
        close(fd);
    }
}

// Moves the connection along; true when it has reached a state where the streams can be told (open or failed).
static Boolean advance(Pair *p) {
    for (;;) {
        switch (p->state) {
        case stateResolving: {
            if (p->fd >= 0) {
                p->state = stateConnecting;
                break;
            }
            char host[1024], service[16];
            if (!p->host || !CFStringGetCString(p->host, host, sizeof(host), kCFStringEncodingUTF8)) {
                if (p->error) CFRelease(p->error);
                setError(p, kCFErrorDomainPOSIX, EINVAL);
                return true;
            }
            snprintf(service, sizeof(service), "%u", p->port);
            struct addrinfo hints = {0};
            hints.ai_socktype = SOCK_STREAM;
            if (getaddrinfo(host, service, &hints, &p->addresses) == 0) {
                p->current = p->addresses;
                connectNext(p);
                break;
            }
            if (p->error) CFRelease(p->error);
            setError(p, CFSTR("kCFErrorDomainCFNetwork"), 2);
            return true;
        }
        case stateConnecting: {
            struct pollfd pfd = { p->fd, POLLOUT, 0 };
            if (poll(&pfd, 1, 0) < 1) return false;
            int soError = 0;
            socklen_t length = sizeof(soError);
            getsockopt(p->fd, SOL_SOCKET, SO_ERROR, &soError, &length);
            if (soError != 0) {
                if (p->closeOnClose) close(p->fd);
                p->fd = -1;
                if (p->current) {
                    connectNext(p);
                    break;
                }
                if (p->error) CFRelease(p->error);
                setError(p, kCFErrorDomainPOSIX, soError);
                return true;
            }
            if (p->addresses) {
                freeaddrinfo(p->addresses);
                p->addresses = NULL;
                p->current = NULL;
            }
            Boolean wantsTLS = p->sslSettings != NULL;
            if (!wantsTLS) {
                p->state = stateOpen;
                if (p->securityLevel && CFGetTypeID(p->securityLevel) == CFStringGetTypeID() && !CFEqual(p->securityLevel, kCFStreamSocketSecurityLevelNone)) wantsTLS = true;
            }
            if (!wantsTLS) return true;
            pthread_once(&sslOnce, resolveSSL);
            if (!ssl.available || !(p->sslContext = ssl.contextNew(ssl.clientMethod()))) {
                if (p->error) CFRelease(p->error);
                setError(p, kCFErrorDomainOSStatus, -0x2648);
                return true;
            }
            Boolean verify = true;
            if (p->sslSettings) {
                CFTypeRef value = CFDictionaryGetValue(p->sslSettings, CFSTR("kCFStreamSSLValidatesCertificateChain"));
                if (value && CFGetTypeID(value) == CFBooleanGetTypeID()) verify = CFBooleanGetValue((CFBooleanRef)value);
            }
            ssl.setDefaultVerifyPaths(p->sslContext);
            ssl.setVerify(p->sslContext, verify, NULL);
            p->ssl = ssl.sslNew(p->sslContext);
            ssl.setFd(p->ssl, p->fd);
            CFStringRef peer = p->host;
            CFTypeRef peerName = p->sslSettings ? CFDictionaryGetValue(p->sslSettings, CFSTR("kCFStreamSSLPeerName")) : NULL;
            if (peerName && CFGetTypeID(peerName) == CFStringGetTypeID()) peer = (CFStringRef)peerName;
            if (peer) {
                char name[1024];
                if (CFStringGetCString(peer, name, sizeof(name), kCFStringEncodingUTF8)) {
                    ssl.ctrl(p->ssl, 0x37, 0, name);
                    if (verify) ssl.setHost(p->ssl, name);
                }
            }
            p->state = stateHandshake;
            break;
        }
        case stateHandshake: {
            int result = ssl.connect(p->ssl);
            if (result == 1) {
                p->state = stateOpen;
                return true;
            }
            int code = ssl.getError(p->ssl, result);
            if ((code & ~1) == 2) return false;
            if (p->error) CFRelease(p->error);
            setError(p, kCFErrorDomainOSStatus, ssl.verifyResult(p->ssl) == 0 ? -0x2648 : -0x264f);
            return true;
        }
        default:
            return true;
        }
    }
}

static void socketCallback(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *data, void *info) {
    Pair *p = (Pair *)info;
    pthread_mutex_lock(&p->lock);
    p->references++;
    pthread_mutex_unlock(&p->lock);
    if (p->state < stateOpen) {
        if (advance(p)) {
            int state = p->state;
            CFStreamEventType event = state == stateOpen ? kCFStreamEventOpenCompleted : kCFStreamEventErrorOccurred;
            if (p->readStream && p->readOpened && !p->readClosed) CFReadStreamSignalEvent(p->readStream, event, state == stateOpen ? NULL : p->error);
            if (p->writeStream && p->writeOpened && !p->writeClosed) CFWriteStreamSignalEvent(p->writeStream, event, state == stateOpen ? NULL : p->error);
            if (p->state == stateOpen && p->writeStream && p->writeOpened && !p->writeClosed) CFWriteStreamSignalEvent(p->writeStream, kCFStreamEventCanAcceptBytes, NULL);
        }
        if (p->socket) CFSocketEnableCallBacks(p->socket, kCFSocketReadCallBack | kCFSocketWriteCallBack);
    } else if (p->state == stateOpen) {
        if (type == kCFSocketWriteCallBack) {
            if (p->writeStream && p->writeOpened && !p->writeClosed) CFWriteStreamSignalEvent(p->writeStream, kCFStreamEventCanAcceptBytes, NULL);
        } else if (type == kCFSocketReadCallBack && p->readStream && p->readOpened && !p->readClosed) {
            CFReadStreamSignalEvent(p->readStream, kCFStreamEventHasBytesAvailable, NULL);
        }
    }
    releasePair(p);
}

static void ensureSocket(Pair *p) {
    if (p->socket || p->fd < 0) return;
    CFSocketContext context = {0};
    context.info = p;
    p->socket = CFSocketCreateWithNative(kCFAllocatorSystemDefault, p->fd, kCFSocketReadCallBack | kCFSocketWriteCallBack, socketCallback, &context);
    if (!p->socket) return;
    CFSocketSetSocketFlags(p->socket, kCFSocketAutomaticallyReenableReadCallBack);
    for (CFIndex i = 0; p->schedules && i < CFArrayGetCount(p->schedules); i++) {
        CFArrayRef entry = (CFArrayRef)CFArrayGetValueAtIndex(p->schedules, i);
        CFRunLoopSourceRef source = CFSocketCreateRunLoopSource(kCFAllocatorSystemDefault, p->socket, 0);
        CFRunLoopAddSource((CFRunLoopRef)CFArrayGetValueAtIndex(entry, 0), source, (CFStringRef)CFArrayGetValueAtIndex(entry, 1));
        CFRelease(source);
    }
}

static void schedulePair(Pair *p, CFRunLoopRef runLoop, CFStringRef mode) {
    if (!p->schedules) p->schedules = CFArrayCreateMutable(kCFAllocatorSystemDefault, 0, &kCFTypeArrayCallBacks);
    const void *values[2] = { runLoop, mode };
    CFArrayRef entry = CFArrayCreate(kCFAllocatorSystemDefault, values, 2, &kCFTypeArrayCallBacks);
    CFArrayAppendValue(p->schedules, entry);
    CFRelease(entry);
    if (p->socket) {
        CFRunLoopSourceRef source = CFSocketCreateRunLoopSource(kCFAllocatorSystemDefault, p->socket, 0);
        CFRunLoopAddSource(runLoop, source, mode);
        CFRelease(source);
    }
}

static void unschedulePair(Pair *p, CFRunLoopRef runLoop, CFStringRef mode) {
    for (CFIndex i = 0; p->schedules && i < CFArrayGetCount(p->schedules); i++) {
        CFArrayRef entry = (CFArrayRef)CFArrayGetValueAtIndex(p->schedules, i);
        if (CFArrayGetValueAtIndex(entry, 0) == runLoop && CFEqual(CFArrayGetValueAtIndex(entry, 1), mode)) {
            CFArrayRemoveValueAtIndex(p->schedules, i);
            break;
        }
    }
    if (p->socket) {
        CFRunLoopSourceRef source = CFSocketCreateRunLoopSource(kCFAllocatorSystemDefault, p->socket, 0);
        CFRunLoopRemoveSource(runLoop, source, mode);
        CFRelease(source);
    }
}

static CFTypeRef copyPairProperty(Pair *p, CFStringRef name) {
    if (CFEqual(name, kCFStreamPropertySocketNativeHandle)) {
        int fd = p->fd;
        return fd < 0 ? NULL : CFDataCreate(kCFAllocatorSystemDefault, (const UInt8 *)&fd, sizeof(fd));
    }
    CFTypeRef value;
    if (CFEqual(name, kCFStreamPropertySocketRemoteHostName)) {
        value = p->host;
    } else if (CFEqual(name, kCFStreamPropertySocketRemotePortNumber)) {
        int port = (int)p->port;
        return CFNumberCreate(kCFAllocatorSystemDefault, kCFNumberSInt32Type, &port);
    } else if (CFEqual(name, kCFStreamPropertySocketSecurityLevel)) {
        value = p->securityLevel ? p->securityLevel : kCFStreamSocketSecurityLevelNone;
        return CFRetain(value);
    } else if (CFEqual(name, CFSTR("kCFStreamPropertySSLSettings"))) {
        value = p->sslSettings;
    } else if (CFEqual(name, kCFStreamPropertyShouldCloseNativeSocket)) {
        return CFRetain(p->closeOnClose ? kCFBooleanTrue : kCFBooleanFalse);
    } else {
        return NULL;
    }
    return value ? CFRetain(value) : NULL;
}

static Boolean setPairProperty(Pair *p, CFStringRef name, CFTypeRef value) {
    if (CFEqual(name, kCFStreamPropertySocketSecurityLevel)) {
        if (p->state > stateConnecting) return false;
        if (p->securityLevel) CFRelease(p->securityLevel);
        p->securityLevel = value ? CFRetain(value) : NULL;
        return true;
    }
    if (CFEqual(name, CFSTR("kCFStreamPropertySSLSettings"))) {
        if (value && CFGetTypeID(value) != CFDictionaryGetTypeID()) return false;
        if (p->sslSettings) CFRelease(p->sslSettings);
        p->sslSettings = value ? (CFDictionaryRef)CFRetain(value) : NULL;
        return true;
    }
    if (CFEqual(name, kCFStreamPropertyShouldCloseNativeSocket)) {
        p->closeOnClose = value && CFGetTypeID(value) == CFBooleanGetTypeID() && CFBooleanGetValue((CFBooleanRef)value);
        return true;
    }
    return false;
}

static void closeIfBothClosed(Pair *p) {
    if (p->readClosed && p->writeClosed) {
        p->state = stateClosed;
        closeConnection(p);
    }
}

static void *pairCreate(void *stream, void *info) {
    Pair *p = (Pair *)info;
    pthread_mutex_lock(&p->lock);
    p->references++;
    pthread_mutex_unlock(&p->lock);
    return p;
}

static void readFinalize(CFReadStreamRef stream, void *info) {
    Pair *p = (Pair *)info;
    p->readStream = NULL;
    p->readClosed = true;
    closeIfBothClosed(p);
    releasePair(p);
}

static void writeFinalize(CFWriteStreamRef stream, void *info) {
    Pair *p = (Pair *)info;
    p->writeStream = NULL;
    p->writeClosed = true;
    closeIfBothClosed(p);
    releasePair(p);
}

static Boolean openPair(Pair *p, Boolean isRead, CFErrorRef *error, Boolean *openComplete) {
    if (isRead) p->readOpened = true;
    else p->writeOpened = true;
    Boolean done = advance(p);
    if (p->state == stateError) {
        if (error && p->error) *error = (CFErrorRef)CFRetain(p->error);
        return false;
    }
    ensureSocket(p);
    *openComplete = done;
    return true;
}

static Boolean openCompleted(Pair *p, CFErrorRef *error) {
    if (!advance(p)) return false;
    if (error && p->state == stateError && p->error) *error = (CFErrorRef)CFRetain(p->error);
    ensureSocket(p);
    return true;
}

static Boolean readOpen(CFReadStreamRef stream, CFErrorRef *error, Boolean *openComplete, void *info) { return openPair((Pair *)info, true, error, openComplete); }
static Boolean writeOpen(CFWriteStreamRef stream, CFErrorRef *error, Boolean *openComplete, void *info) { return openPair((Pair *)info, false, error, openComplete); }
static Boolean readOpenCompleted(CFReadStreamRef stream, CFErrorRef *error, void *info) { return openCompleted((Pair *)info, error); }
static Boolean writeOpenCompleted(CFWriteStreamRef stream, CFErrorRef *error, void *info) { return openCompleted((Pair *)info, error); }

// Waits until the connection is open; false (with the error) when it failed.
static Boolean waitOpen(Pair *p, CFErrorRef *error) {
    if (!advance(p)) {
        do {
            struct pollfd pfd = { p->fd, (short)((p->state == stateConnecting) ^ 5), 0 };
            poll(&pfd, 1, -1);
        } while (!advance(p));
    }
    if (p->state != stateOpen) {
        if (error && p->error) *error = (CFErrorRef)CFRetain(p->error);
        return false;
    }
    return true;
}

static CFIndex readBytes(CFReadStreamRef stream, UInt8 *buffer, CFIndex length, CFErrorRef *error, Boolean *atEOF, void *info) {
    Pair *p = (Pair *)info;
    if (!waitOpen(p, error)) return -1;
    CFIndex chunk = length < 0x40000000 ? length : 0x40000000;
    for (;;) {
        struct pollfd pfd = { p->fd, 0, 0 };
        if (!p->ssl) {
            ssize_t n = recv(p->fd, buffer, length, 0);
            if (n > 0) return n;
            if (n == 0) {
                *atEOF = true;
                p->atEOF = true;
                return 0;
            }
            if (errno != EAGAIN && errno != EINTR) {
                if (p->error) CFRelease(p->error);
                setError(p, kCFErrorDomainPOSIX, errno);
                if (error) *error = (CFErrorRef)CFRetain(p->error);
                return -1;
            }
            pfd.events = POLLIN;
        } else {
            int n = ssl.read(p->ssl, buffer, (int)chunk);
            if (n > 0) return n;
            int code = ssl.getError(p->ssl, n);
            if (n == 0 || code == 6) {
                *atEOF = true;
                p->atEOF = true;
                return 0;
            }
            if ((code & ~1) != 2) {
                if (p->error) CFRelease(p->error);
                setError(p, kCFErrorDomainOSStatus, -0x264e);
                if (error) *error = (CFErrorRef)CFRetain(p->error);
                return -1;
            }
            pfd.events = (short)((code != 2) * 3 + 1);
        }
        poll(&pfd, 1, -1);
    }
}

static CFIndex writeBytes(CFWriteStreamRef stream, const UInt8 *buffer, CFIndex length, CFErrorRef *error, void *info) {
    Pair *p = (Pair *)info;
    if (!waitOpen(p, error)) return -1;
    CFIndex chunk = length < 0x40000000 ? length : 0x40000000;
    for (;;) {
        struct pollfd pfd = { p->fd, 0, 0 };
        if (!p->ssl) {
            ssize_t n = send(p->fd, buffer, length, 0);
            if (n >= 0) return n;
            if (errno != EAGAIN && errno != EINTR) {
                if (p->error) CFRelease(p->error);
                setError(p, kCFErrorDomainPOSIX, errno);
                if (error) *error = (CFErrorRef)CFRetain(p->error);
                return -1;
            }
            pfd.events = POLLOUT;
        } else {
            int n = ssl.write(p->ssl, buffer, (int)chunk);
            if (n > 0) return n;
            int code = ssl.getError(p->ssl, n);
            if ((code & ~1) != 2) {
                if (p->error) CFRelease(p->error);
                setError(p, kCFErrorDomainOSStatus, -0x264e);
                if (error) *error = (CFErrorRef)CFRetain(p->error);
                return -1;
            }
            pfd.events = (short)((code != 2) * 3 + 1);
        }
        poll(&pfd, 1, -1);
    }
}

static Boolean readCanRead(CFReadStreamRef stream, CFErrorRef *error, void *info) {
    Pair *p = (Pair *)info;
    if (p->state != stateOpen) return false;
    if (p->ssl && ssl.pending(p->ssl) > 0) return true;
    struct pollfd pfd = { p->fd, POLLIN, 0 };
    int ready = poll(&pfd, 1, 0);
    return (pfd.revents & (POLLIN | POLLERR | POLLHUP)) != 0 && ready > 0;
}

static Boolean writeCanWrite(CFWriteStreamRef stream, CFErrorRef *error, void *info) {
    Pair *p = (Pair *)info;
    if (p->state != stateOpen) return false;
    struct pollfd pfd = { p->fd, POLLOUT, 0 };
    int ready = poll(&pfd, 1, 0);
    Boolean writable = (pfd.revents & POLLOUT) != 0 && ready > 0;
    if (!writable && p->socket) CFSocketEnableCallBacks(p->socket, kCFSocketWriteCallBack);
    return writable;
}

static void readClose(CFReadStreamRef stream, void *info) {
    Pair *p = (Pair *)info;
    p->readClosed = true;
    closeIfBothClosed(p);
}

static void writeClose(CFWriteStreamRef stream, void *info) {
    Pair *p = (Pair *)info;
    p->writeClosed = true;
    if (p->fd >= 0 && !p->readClosed && !p->ssl) shutdown(p->fd, SHUT_WR);
    closeIfBothClosed(p);
}

static CFTypeRef readCopyProperty(CFReadStreamRef stream, CFStringRef name, void *info) { return copyPairProperty((Pair *)info, name); }
static CFTypeRef writeCopyProperty(CFWriteStreamRef stream, CFStringRef name, void *info) { return copyPairProperty((Pair *)info, name); }
static Boolean readSetProperty(CFReadStreamRef stream, CFStringRef name, CFTypeRef value, void *info) { return setPairProperty((Pair *)info, name, value); }
static Boolean writeSetProperty(CFWriteStreamRef stream, CFStringRef name, CFTypeRef value, void *info) { return setPairProperty((Pair *)info, name, value); }
static void readSchedule(CFReadStreamRef stream, CFRunLoopRef rl, CFStringRef mode, void *info) { schedulePair((Pair *)info, rl, mode); }
static void writeSchedule(CFWriteStreamRef stream, CFRunLoopRef rl, CFStringRef mode, void *info) { schedulePair((Pair *)info, rl, mode); }
static void readUnschedule(CFReadStreamRef stream, CFRunLoopRef rl, CFStringRef mode, void *info) { unschedulePair((Pair *)info, rl, mode); }
static void writeUnschedule(CFWriteStreamRef stream, CFRunLoopRef rl, CFStringRef mode, void *info) { unschedulePair((Pair *)info, rl, mode); }

static const CFReadStreamCallBacks readCallBacks = {
    2, (void *(*)(CFReadStreamRef, void *))pairCreate, readFinalize, NULL, readOpen, readOpenCompleted, readBytes, NULL, readCanRead, readClose,
    readCopyProperty, readSetProperty, NULL, readSchedule, readUnschedule,
};

static const CFWriteStreamCallBacks writeCallBacks = {
    2, (void *(*)(CFWriteStreamRef, void *))pairCreate, writeFinalize, NULL, writeOpen, writeOpenCompleted, writeBytes, writeCanWrite, writeClose,
    writeCopyProperty, writeSetProperty, NULL, writeSchedule, writeUnschedule,
};

void _CFSocketStreamCreatePair(CFAllocatorRef alloc, CFStringRef host, UInt32 port, CFSocketNativeHandle sock, const CFSocketSignature *sig, CFReadStreamRef *readStream, CFWriteStreamRef *writeStream) {
    Pair *p = (Pair *)calloc(1, sizeof(Pair));
    pthread_mutex_init(&p->lock, NULL);
    p->references = 1;
    p->fd = -1;
    if (!host) {
        if (!sig) {
            p->fd = sock;
            p->closeOnClose = false;
            fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
        } else {
            p->fd = socket(sig->protocolFamily, sig->socketType, sig->protocol);
            if (p->fd >= 0) {
                fcntl(p->fd, F_SETFL, fcntl(p->fd, F_GETFL) | O_NONBLOCK);
                p->closeOnClose = true;
                if (connect(p->fd, (const struct sockaddr *)CFDataGetBytePtr(sig->address), CFDataGetLength(sig->address)) != 0 && errno != EINPROGRESS) {
                    close(p->fd);
                    p->fd = -1;
                    CFStringRef domain = kCFErrorDomainPOSIX;
                    if (p->error) CFRelease(p->error);
                    p->error = CFErrorCreate(kCFAllocatorSystemDefault, domain, errno, NULL);
                    p->state = stateError;
                }
            }
        }
    } else {
        p->host = CFStringCreateCopy(kCFAllocatorSystemDefault, host);
        p->port = port;
    }
    if (!readStream) {
        p->readClosed = true;
    } else {
        *readStream = CFReadStreamCreate(alloc, &readCallBacks, p);
        p->readStream = *readStream;
    }
    if (!writeStream) {
        p->writeClosed = true;
    } else {
        *writeStream = CFWriteStreamCreate(alloc, &writeCallBacks, p);
        p->writeStream = *writeStream;
    }
    releasePair(p);
}
