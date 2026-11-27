// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Streams in pairs: a bound pair (a ring buffer between a read and a write stream, CFStreamCreateBoundPair) and the conversion between
// CFStreamError and CFError.
#include "CFInternal.h"
#include "CFStreamAbstract.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

CFErrorRef _CFErrorCreateWithStreamError(CFAllocatorRef allocator, CFStreamError *streamError) {
    CFStringRef domain = kCFErrorDomainPOSIX;
    if (streamError->domain != kCFStreamErrorDomainPOSIX) {
        domain = streamError->domain == kCFStreamErrorDomainMacOSStatus ? kCFErrorDomainOSStatus : CFSTR("kCFErrorDomainCFNetwork");
    }
    return CFErrorCreate(allocator, domain, (CFIndex)(int)streamError->error, NULL);
}

CFStreamError _CFStreamErrorFromCFError(CFErrorRef error) {
    CFStreamError result = { -1, 0 };
    if (error) {
        CFStringRef domain = CFErrorGetDomain(error);
        result.error = (SInt32)CFErrorGetCode(error);
        if (CFEqual(domain, kCFErrorDomainPOSIX)) result.domain = kCFStreamErrorDomainPOSIX;
        else result.domain = CFEqual(domain, kCFErrorDomainOSStatus) ? kCFStreamErrorDomainMacOSStatus : -1;
    }
    return result;
}

// The state shared by the two streams of a bound pair; it lives until both have been finalized.
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int references;
    uint8_t *buffer;
    CFIndex capacity;
    CFIndex head;
    CFIndex count;
    Boolean readClosed;
    Boolean writeClosed;
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
} BoundPair;

static void releaseBound(BoundPair *pair) {
    pthread_mutex_lock(&pair->lock);
    int before = pair->references--;
    pthread_mutex_unlock(&pair->lock);
    if (before < 2) {
        free(pair->buffer);
        pthread_cond_destroy(&pair->cond);
        pthread_mutex_destroy(&pair->lock);
        free(pair);
    }
}

static void *boundReadCreate(CFReadStreamRef stream, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    pair->references++;
    pthread_mutex_unlock(&pair->lock);
    return pair;
}

static void boundReadFinalize(CFReadStreamRef stream, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    pair->readStream = NULL;
    pair->readClosed = true;
    pthread_cond_broadcast(&pair->cond);
    pthread_mutex_unlock(&pair->lock);
    releaseBound(pair);
}

static Boolean boundReadOpen(CFReadStreamRef stream, CFErrorRef *error, Boolean *openComplete, void *info) {
    *openComplete = true;
    return true;
}

static CFIndex boundRead(CFReadStreamRef stream, UInt8 *buffer, CFIndex bufferLength, CFErrorRef *error, Boolean *atEOF, void *info) {
    BoundPair *pair = (BoundPair *)info;
    CFIndex done = 0;
    pthread_mutex_lock(&pair->lock);
    while (pair->count == 0) {
        if (pair->writeClosed) {
            pthread_mutex_unlock(&pair->lock);
            *atEOF = true;
            return 0;
        }
        pthread_cond_wait(&pair->cond, &pair->lock);
    }
    CFIndex available = pair->count;
    while (done < bufferLength && available > 0) {
        CFIndex chunk = pair->capacity - pair->head;
        if (available < chunk) chunk = available;
        if (bufferLength - done < chunk) chunk = bufferLength - done;
        memcpy(buffer + done, pair->buffer + pair->head, chunk);
        pair->head = (pair->head + chunk) % pair->capacity;
        done += chunk;
        available = pair->count -= chunk;
    }
    CFWriteStreamRef writer = pair->writeStream ? (CFWriteStreamRef)CFRetain(pair->writeStream) : NULL;
    pthread_cond_broadcast(&pair->cond);
    pthread_mutex_unlock(&pair->lock);
    *atEOF = false;
    if (writer) {
        CFWriteStreamSignalEvent(writer, kCFStreamEventCanAcceptBytes, NULL);
        CFRelease(writer);
    }
    return done;
}

static Boolean boundCanRead(CFReadStreamRef stream, CFErrorRef *error, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    Boolean result = pair->count > 0 || pair->writeClosed;
    pthread_mutex_unlock(&pair->lock);
    return result;
}

static void boundReadClose(CFReadStreamRef stream, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    pair->readClosed = true;
    pthread_cond_broadcast(&pair->cond);
    pthread_mutex_unlock(&pair->lock);
}

static void *boundWriteCreate(CFWriteStreamRef stream, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    pair->references++;
    pthread_mutex_unlock(&pair->lock);
    return pair;
}

static void boundWriteFinalize(CFWriteStreamRef stream, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    pair->writeStream = NULL;
    pair->writeClosed = true;
    pthread_cond_broadcast(&pair->cond);
    pthread_mutex_unlock(&pair->lock);
    releaseBound(pair);
}

static Boolean boundWriteOpen(CFWriteStreamRef stream, CFErrorRef *error, Boolean *openComplete, void *info) {
    *openComplete = true;
    return true;
}

static CFIndex boundWrite(CFWriteStreamRef stream, const UInt8 *buffer, CFIndex bufferLength, CFErrorRef *error, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    while (pair->count == pair->capacity && !pair->readClosed) pthread_cond_wait(&pair->cond, &pair->lock);
    if (pair->readClosed) {
        pthread_mutex_unlock(&pair->lock);
        if (error) *error = CFErrorCreate(kCFAllocatorSystemDefault, kCFErrorDomainPOSIX, EPIPE, NULL);
        return -1;
    }
    CFIndex done = 0;
    while (done < bufferLength) {
        CFIndex space = pair->capacity - pair->count;
        if (space <= 0) break;
        CFIndex tail = (pair->count + pair->head) % pair->capacity;
        CFIndex chunk = pair->capacity;
        if (tail < pair->head) chunk = pair->head;
        chunk -= tail;
        if (space < chunk) chunk = space;
        if (bufferLength - done < chunk) chunk = bufferLength - done;
        memcpy(pair->buffer + tail, buffer + done, chunk);
        done += chunk;
        pair->count += chunk;
    }
    CFReadStreamRef reader = pair->readStream ? (CFReadStreamRef)CFRetain(pair->readStream) : NULL;
    pthread_cond_broadcast(&pair->cond);
    pthread_mutex_unlock(&pair->lock);
    if (reader) {
        CFReadStreamSignalEvent(reader, kCFStreamEventHasBytesAvailable, NULL);
        CFRelease(reader);
    }
    return done;
}

static Boolean boundCanWrite(CFWriteStreamRef stream, CFErrorRef *error, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    CFIndex count = pair->count, capacity = pair->capacity;
    pthread_mutex_unlock(&pair->lock);
    return count < capacity;
}

static void boundWriteClose(CFWriteStreamRef stream, void *info) {
    BoundPair *pair = (BoundPair *)info;
    pthread_mutex_lock(&pair->lock);
    pair->writeClosed = true;
    CFReadStreamRef reader = pair->readStream ? (CFReadStreamRef)CFRetain(pair->readStream) : NULL;
    pthread_cond_broadcast(&pair->cond);
    pthread_mutex_unlock(&pair->lock);
    if (reader) {
        CFReadStreamSignalEvent(reader, kCFStreamEventHasBytesAvailable, NULL);
        CFRelease(reader);
    }
}

static const CFReadStreamCallBacks boundReadCallBacks = {
    2, boundReadCreate, boundReadFinalize, NULL, boundReadOpen, NULL, boundRead, NULL, boundCanRead, boundReadClose,
};

static const CFWriteStreamCallBacks boundWriteCallBacks = {
    2, boundWriteCreate, boundWriteFinalize, NULL, boundWriteOpen, NULL, boundWrite, boundCanWrite, boundWriteClose,
};

void CFStreamCreateBoundPair(CFAllocatorRef alloc, CFReadStreamRef *readStream, CFWriteStreamRef *writeStream, CFIndex transferBufferSize) {
    BoundPair *pair = (BoundPair *)calloc(1, sizeof(BoundPair));
    pthread_mutex_init(&pair->lock, NULL);
    pthread_cond_init(&pair->cond, NULL);
    pair->references = 1;
    pair->capacity = transferBufferSize > 0 ? transferBufferSize : 0x1000;
    pair->buffer = (uint8_t *)malloc(pair->capacity);
    if (readStream) {
        *readStream = CFReadStreamCreate(alloc, &boundReadCallBacks, pair);
        pair->readStream = *readStream;
    } else {
        pair->readClosed = true;
    }
    if (writeStream) {
        *writeStream = CFWriteStreamCreate(alloc, &boundWriteCallBacks, pair);
        pair->writeStream = *writeStream;
    } else {
        pair->writeClosed = true;
    }
    releaseBound(pair);
}
