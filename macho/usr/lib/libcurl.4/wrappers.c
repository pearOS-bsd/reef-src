// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libcurl: the functions that are more than a jump into the host's library. Every function looks the host's one up the first time it is called.
// A handle's default callbacks write to and read from Apple's stdout and stdin (the host's would be a different FILE), the header callback
// of a handle can be set apart from the body's, and the printf family formats with the host's curl_mvaprintf and writes through Apple's stdio.

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void *reef_host_symbol(const char *library, const char *name);

#define HOST_LIBRARY "libcurl.so.4"

#define OPT_WRITEDATA 0x2711
#define OPT_READDATA 0x2719
#define OPT_HEADERDATA 0x272d
#define OPT_STDERR 0x2735
#define OPT_ERRORDATA 0x27d3
#define OPT_WRITEFUNCTION 0x4e2b
#define OPT_READFUNCTION 0x4e2c
#define OPT_HEADERFUNCTION 0x4e6f
#define OPT_ERRORFUNCTION 0x4ee4

#define WORDS 16
#define SIXTEEN void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *

typedef void CURL;
typedef size_t (*TransferFunction)(void *, size_t, size_t, void *);

// What is remembered about a handle: its body callback and the header callback and data that were set (the header callback is
// installed by applyHeader, which falls back to writeHeader).
typedef struct State {
    CURL *handle;
    TransferFunction writeFunction;
    TransferFunction headerFunction;
    void *headerData;
    struct State *next;
} State;

static State *states;
static pthread_mutex_t statesLock = PTHREAD_MUTEX_INITIALIZER;

static size_t writeStream(void *, size_t, size_t, void *);
static size_t readStream(void *, size_t, size_t, void *);
static size_t writeHeader(void *, size_t, size_t, void *);
static void installDefaults(CURL *);
static int applyHeader(State *);

static State *findState(CURL *handle)
{
    for (State *state = states; state; state = state->next)
        if (state->handle == handle)
            return state;
    return NULL;
}

static State *stateFor(CURL *handle)
{
    State *state = findState(handle);
    if (!state) {
        state = calloc(1, sizeof(State));
        if (state) {
            state->handle = handle;
            state->next = states;
            states = state;
        }
    }
    return state;
}

static inline CURL *host_curl_easy_init(void)
{
    static CURL *(*function)(void);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_easy_init");
    return function();
}

static inline void host_curl_easy_reset(CURL *handle)
{
    static void (*function)(CURL *);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_easy_reset");
    function(handle);
}

static inline void host_curl_easy_cleanup(CURL *handle)
{
    static void (*function)(CURL *);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_easy_cleanup");
    function(handle);
}

static inline CURL *host_curl_easy_duphandle(CURL *handle)
{
    static CURL *(*function)(CURL *);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_easy_duphandle");
    return function(handle);
}

static inline int host_curl_easy_setopt(CURL *handle, int option, void **words)
{
    static int (*function)(CURL *, int, SIXTEEN);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_easy_setopt");
    return function(handle, option, words[0], words[1], words[2], words[3], words[4], words[5], words[6], words[7], words[8], words[9],
                    words[10], words[11], words[12], words[13], words[14], words[15]);
}

static inline int host_curl_easy_getinfo(CURL *handle, int info, void **words)
{
    static int (*function)(CURL *, int, SIXTEEN);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_easy_getinfo");
    return function(handle, info, words[0], words[1], words[2], words[3], words[4], words[5], words[6], words[7], words[8], words[9],
                    words[10], words[11], words[12], words[13], words[14], words[15]);
}

static inline int host_curl_multi_setopt(void *multi, int option, void **words)
{
    static int (*function)(void *, int, SIXTEEN);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_multi_setopt");
    return function(multi, option, words[0], words[1], words[2], words[3], words[4], words[5], words[6], words[7], words[8], words[9],
                    words[10], words[11], words[12], words[13], words[14], words[15]);
}

static inline int host_curl_share_setopt(void *share, int option, void **words)
{
    static int (*function)(void *, int, SIXTEEN);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_share_setopt");
    return function(share, option, words[0], words[1], words[2], words[3], words[4], words[5], words[6], words[7], words[8], words[9],
                    words[10], words[11], words[12], words[13], words[14], words[15]);
}

#define FORTY SIXTEEN, SIXTEEN, void *, void *, void *, void *, void *, void *, void *, void *

static inline int host_curl_formadd(void **first, void **last, void **w)
{
    static int (*function)(void **, void **, FORTY);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_formadd");
    return function(first, last, w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7], w[8], w[9], w[10], w[11], w[12], w[13], w[14], w[15],
                    w[16], w[17], w[18], w[19], w[20], w[21], w[22], w[23], w[24], w[25], w[26], w[27], w[28], w[29], w[30], w[31],
                    w[32], w[33], w[34], w[35], w[36], w[37], w[38], w[39]);
}

static inline char *host_curl_mvaprintf(const char *format, va_list arguments)
{
    static char *(*function)(const char *, va_list);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_mvaprintf");
    return function(format, arguments);
}

static inline int host_curl_mvsnprintf(char *buffer, size_t size, const char *format, va_list arguments)
{
    static int (*function)(char *, size_t, const char *, va_list);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_mvsnprintf");
    return function(buffer, size, format, arguments);
}

static inline void host_curl_free(void *memory)
{
    static void (*function)(void *);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "curl_free");
    function(memory);
}

// The body callbacks write to Apple's stdout and read from Apple's stdin when the stream is NULL.
static size_t writeStream(void *data, size_t size, size_t count, void *stream)
{
    if (!stream)
        stream = stdout;
    return fwrite(data, size, count, stream) * size;
}

static size_t readStream(void *data, size_t size, size_t count, void *stream)
{
    if (!stream)
        stream = stdin;
    size_t items = fread(data, size, count, stream);
    if (!items && ferror(stream))
        return 0x10000000;                      // CURL_READFUNC_ABORT
    return items * size;
}

// Without a header callback the header lines go to the body callback when there is header data, and are dropped otherwise.
static size_t writeHeader(void *data, size_t size, size_t count, void *userData)
{
    State *state = userData;
    if (state->headerData)
        return (state->writeFunction ? state->writeFunction : writeStream)(data, size, count, state->headerData);
    return size * count;
}

static int applyHeader(State *state)
{
    CURL *handle = state->handle;
    TransferFunction function = state->headerFunction;
    void *words[WORDS] = {0};
    void *data;
    if (function) {
        words[0] = function;
        host_curl_easy_setopt(handle, OPT_HEADERFUNCTION, words);
        handle = state->handle;
        data = state->headerData;
    } else {
        words[0] = writeHeader;
        host_curl_easy_setopt(handle, OPT_HEADERFUNCTION, words);
        handle = state->handle;
        data = state;
    }
    words[0] = data;
    return host_curl_easy_setopt(handle, OPT_HEADERDATA, words);
}

static void setDefault(CURL *handle, int option, void *value)
{
    void *words[WORDS] = {0};
    words[0] = value;
    host_curl_easy_setopt(handle, option, words);
}

static void installDefaults(CURL *handle)
{
    setDefault(handle, OPT_WRITEFUNCTION, writeStream);
    setDefault(handle, OPT_READFUNCTION, readStream);
    setDefault(handle, OPT_ERRORFUNCTION, writeStream);
    setDefault(handle, OPT_WRITEDATA, stdout);
    setDefault(handle, OPT_READDATA, stdin);
    setDefault(handle, OPT_ERRORDATA, stdout);
    pthread_mutex_lock(&statesLock);
    State *state = stateFor(handle);
    if (state) {
        state->writeFunction = 0;
        state->headerFunction = 0;
        state->headerData = 0;
        applyHeader(state);
    }
    pthread_mutex_unlock(&statesLock);
}

CURL *curl_easy_init(void)
{
    CURL *handle = host_curl_easy_init();
    if (handle)
        installDefaults(handle);
    return handle;
}

void curl_easy_reset(CURL *handle)
{
    host_curl_easy_reset(handle);
    installDefaults(handle);
}

void curl_easy_cleanup(CURL *handle)
{
    host_curl_easy_cleanup(handle);
    pthread_mutex_lock(&statesLock);
    State **link = &states;
    while (*link && (*link)->handle != handle)
        link = &(*link)->next;
    if (*link) {
        State *state = *link;
        *link = state->next;
        free(state);
    }
    pthread_mutex_unlock(&statesLock);
}

CURL *curl_easy_duphandle(CURL *handle)
{
    CURL *copy = host_curl_easy_duphandle(handle);
    if (copy) {
        pthread_mutex_lock(&statesLock);
        State *source = findState(handle);
        State *state = stateFor(copy);
        if (state) {
            if (source) {
                state->writeFunction = source->writeFunction;
                state->headerFunction = source->headerFunction;
                state->headerData = source->headerData;
            }
            applyHeader(state);
        }
        pthread_mutex_unlock(&statesLock);
    }
    return copy;
}

#define COPY_WORDS(last) \
    void *words[WORDS]; \
    va_list arguments; \
    va_start(arguments, last); \
    for (int i = 0; i < WORDS; i++) \
        words[i] = va_arg(arguments, void *); \
    va_end(arguments)

// A handle's write callback is kept (the header callback may fall back to it); an unset callback becomes Apple's default; Apple's stderr is
// replaced by the host's.
int curl_easy_setopt(CURL *handle, int option, ...)
{
    COPY_WORDS(option);
    switch (option) {
    case OPT_WRITEFUNCTION:
    case OPT_READFUNCTION:
    case OPT_ERRORFUNCTION: {
        TransferFunction function = words[0];
        if (option == OPT_WRITEFUNCTION) {
            pthread_mutex_lock(&statesLock);
            State *state = stateFor(handle);
            if (state)
                state->writeFunction = function;
            pthread_mutex_unlock(&statesLock);
        }
        TransferFunction chosen = option == OPT_READFUNCTION ? readStream : writeStream;
        if (function)
            chosen = function;
        void *zero[WORDS] = {0};
        zero[0] = chosen;
        return host_curl_easy_setopt(handle, option, zero);
    }
    case OPT_HEADERFUNCTION:
    case OPT_HEADERDATA: {
        int result;
        pthread_mutex_lock(&statesLock);
        State *state = stateFor(handle);
        if (state) {
            if (option == OPT_HEADERFUNCTION)
                state->headerFunction = words[0];
            else
                state->headerData = words[0];
            result = applyHeader(state);
        } else {
            result = 27;                        // CURLE_OUT_OF_MEMORY
        }
        pthread_mutex_unlock(&statesLock);
        return result;
    }
    case OPT_STDERR: {
        static void ***hostStderr;
        if (!hostStderr)
            hostStderr = reef_host_symbol("libc.so.6", "stderr");
        words[0] = hostStderr ? *hostStderr : 0;
        break;
    }
    }
    return host_curl_easy_setopt(handle, option, words);
}

int curl_easy_getinfo(CURL *handle, int info, ...)
{
    COPY_WORDS(info);
    return host_curl_easy_getinfo(handle, info, words);
}

int curl_multi_setopt(void *multi, int option, ...)
{
    COPY_WORDS(option);
    return host_curl_multi_setopt(multi, option, words);
}

int curl_share_setopt(void *share, int option, ...)
{
    COPY_WORDS(option);
    return host_curl_share_setopt(share, option, words);
}

int curl_formadd(void **first, void **last, ...)
{
    void *words[40];
    va_list arguments;
    va_start(arguments, last);
    for (int i = 0; i < 40; i++)
        words[i] = va_arg(arguments, void *);
    va_end(arguments);
    return host_curl_formadd(first, last, words);
}

char *curl_mvaprintf(const char *format, va_list arguments)
{
    return host_curl_mvaprintf(format, arguments);
}

char *curl_maprintf(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char *result = host_curl_mvaprintf(format, arguments);
    va_end(arguments);
    return result;
}

int curl_mvsnprintf(char *buffer, size_t size, const char *format, va_list arguments)
{
    return host_curl_mvsnprintf(buffer, size, format, arguments);
}

int curl_msnprintf(char *buffer, size_t size, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int result = host_curl_mvsnprintf(buffer, size, format, arguments);
    va_end(arguments);
    return result;
}

static int copyFormatted(char *buffer, const char *format, va_list arguments)
{
    char *text = host_curl_mvaprintf(format, arguments);
    if (!text)
        return -1;
    int length = (int)strlen(text);
    memcpy(buffer, text, (long)length + 1);
    host_curl_free(text);
    return length;
}

int curl_mvsprintf(char *buffer, const char *format, va_list arguments)
{
    return copyFormatted(buffer, format, arguments);
}

int curl_msprintf(char *buffer, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int result = copyFormatted(buffer, format, arguments);
    va_end(arguments);
    return result;
}

static int writeFormatted(FILE *stream, const char *format, va_list arguments)
{
    char *text = host_curl_mvaprintf(format, arguments);
    if (!text)
        return -1;
    int length = (int)strlen(text);
    if (fwrite(text, 1, length, stream) != (size_t)length)
        length = -1;
    host_curl_free(text);
    return length;
}

int curl_mvfprintf(FILE *stream, const char *format, va_list arguments)
{
    return writeFormatted(stream, format, arguments);
}

int curl_mfprintf(FILE *stream, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int result = writeFormatted(stream, format, arguments);
    va_end(arguments);
    return result;
}

int curl_mvprintf(const char *format, va_list arguments)
{
    return writeFormatted(stdout, format, arguments);
}

int curl_mprintf(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int result = writeFormatted(stdout, format, arguments);
    va_end(arguments);
    return result;
}
