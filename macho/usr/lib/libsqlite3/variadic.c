// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libsqlite3: the functions with a variable argument list, written out: they look the host's function up the first time they are called.

#include <stdarg.h>

extern void *reef_host_symbol(const char *library, const char *name);

#define HOST_LIBRARY "libsqlite3.so.0"

static inline char *host_sqlite3_vmprintf(const char *format, va_list arguments)
{
    static char *(*function)(const char *, va_list);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_vmprintf");
    return function(format, arguments);
}

static inline char *host_sqlite3_vsnprintf(int size, char *buffer, const char *format, va_list arguments)
{
    static char *(*function)(int, char *, const char *, va_list);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_vsnprintf");
    return function(size, buffer, format, arguments);
}

static inline void host_sqlite3_str_vappendf(void *string, const char *format, va_list arguments)
{
    static void (*function)(void *, const char *, va_list);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_str_vappendf");
    function(string, format, arguments);
}

static inline void host_sqlite3_log(int code, const char *message)
{
    static void (*function)(int, const char *, const char *);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_log");
    function(code, "%s", message ? message : "");
}

static inline void host_sqlite3_free(void *memory)
{
    static void (*function)(void *);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_free");
    function(memory);
}

char *sqlite3_vmprintf(const char *format, va_list arguments)
{
    return host_sqlite3_vmprintf(format, arguments);
}

char *sqlite3_mprintf(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char *result = host_sqlite3_vmprintf(format, arguments);
    va_end(arguments);
    return result;
}

char *sqlite3_vsnprintf(int size, char *buffer, const char *format, va_list arguments)
{
    return host_sqlite3_vsnprintf(size, buffer, format, arguments);
}

char *sqlite3_snprintf(int size, char *buffer, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char *result = host_sqlite3_vsnprintf(size, buffer, format, arguments);
    va_end(arguments);
    return result;
}

void sqlite3_str_vappendf(void *string, const char *format, va_list arguments)
{
    host_sqlite3_str_vappendf(string, format, arguments);
}

void sqlite3_str_appendf(void *string, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    host_sqlite3_str_vappendf(string, format, arguments);
    va_end(arguments);
}

void sqlite3_log(int code, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char *message = host_sqlite3_vmprintf(format, arguments);
    va_end(arguments);
    host_sqlite3_log(code, message);
    host_sqlite3_free(message);
}

// The host's variadic configuration functions are called with sixteen words copied from the caller's arguments.

#define CONFIG_WORDS 16

#define COPY_WORDS(last) \
    void *words[CONFIG_WORDS]; \
    va_list arguments; \
    va_start(arguments, last); \
    for (int i = 0; i < CONFIG_WORDS; i++) \
        words[i] = va_arg(arguments, void *); \
    va_end(arguments)

#define WORDS w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7], w[8], w[9], w[10], w[11], w[12], w[13], w[14], w[15]

#define P16 void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *

static inline int host_sqlite3_config(int option, void **w)
{
    static int (*function)(int, P16);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_config");
    return function(option, WORDS);
}

static inline int host_sqlite3_db_config(void *database, int option, void **w)
{
    static int (*function)(void *, int, P16);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_db_config");
    return function(database, option, WORDS);
}

static inline int host_sqlite3_test_control(int option, void **w)
{
    static int (*function)(int, P16);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_test_control");
    return function(option, WORDS);
}

static inline int host_sqlite3_vtab_config(void *database, int option, void **w)
{
    static int (*function)(void *, int, P16);
    if (!function)
        function = reef_host_symbol(HOST_LIBRARY, "sqlite3_vtab_config");
    return function(database, option, WORDS);
}

int sqlite3_config(int option, ...)
{
    COPY_WORDS(option);
    return host_sqlite3_config(option, words);
}

int sqlite3_db_config(void *database, int option, ...)
{
    COPY_WORDS(option);
    return host_sqlite3_db_config(database, option, words);
}

int sqlite3_test_control(int option, ...)
{
    COPY_WORDS(option);
    return host_sqlite3_test_control(option, words);
}

int sqlite3_vtab_config(void *database, int option, ...)
{
    COPY_WORDS(option);
    return host_sqlite3_vtab_config(database, option, words);
}
