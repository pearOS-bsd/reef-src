// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libxml2: the functions with a variable argument list or that change global variables: the message is formatted here (vasprintf) and passed
// on as "%s", the memory setup functions refresh the variables afterwards. Every function looks the host's one up the first time it is
// called; if the host's library lacks it the program is stopped.

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

extern void *reef_host_symbol(const char *library, const char *name);
extern void reef_xml2_refresh_data(void);

#define HOST_LIBRARY "libxml2.so.2"

#define HOST_FUNCTION(name, type) \
    static inline type host_##name(void) \
    { \
        static type function; \
        if (!function) { \
            void *found = reef_host_symbol(HOST_LIBRARY, #name); \
            if (!found) { \
                fprintf(stderr, "reef: libxml2 lacks %s\n", #name); \
                abort(); \
            } \
            function = found; \
        } \
        return function; \
    }

typedef void (*MessageFunction)(void *, const char *, const char *);
typedef void (*RaiseFunction)(void *, void *, void *, void *, void *, long, long, long, const char *, long, const char *, const char *,
                              const char *, long, long, const char *, const char *);

HOST_FUNCTION(xmlParserError, MessageFunction)
HOST_FUNCTION(xmlParserWarning, MessageFunction)
HOST_FUNCTION(xmlParserValidityError, MessageFunction)
HOST_FUNCTION(xmlParserValidityWarning, MessageFunction)
HOST_FUNCTION(xmlGenericErrorDefaultFunc, MessageFunction)
HOST_FUNCTION(__xmlRaiseError, RaiseFunction)

// The text of a printf-style message; NULL when it could not be formatted.
#define FORMAT_MESSAGE(text, format, last) \
    va_list arguments; \
    va_start(arguments, last); \
    char *text = NULL; \
    int length = vasprintf(&text, format ? format : "", arguments); \
    va_end(arguments)

#define MESSAGE_FUNCTION(name) \
    void name(void *context, const char *format, ...) \
    { \
        FORMAT_MESSAGE(text, format, format); \
        char *message = length < 0 ? NULL : text; \
        MessageFunction function = host_##name(); \
        function(context, "%s", message ? message : ""); \
        free(message); \
    }

MESSAGE_FUNCTION(xmlParserError)
MESSAGE_FUNCTION(xmlParserWarning)
MESSAGE_FUNCTION(xmlParserValidityError)
MESSAGE_FUNCTION(xmlParserValidityWarning)
MESSAGE_FUNCTION(xmlGenericErrorDefaultFunc)

void __xmlRaiseError(void *structuredChannel, void *channel, void *data, void *context, void *node, int domain, int code, int level,
                     const char *file, int line, const char *string1, const char *string2, const char *string3, int integer1, int column,
                     const char *format, ...)
{
    FORMAT_MESSAGE(text, format, format);
    char *message = length < 0 ? NULL : text;
    RaiseFunction function = host___xmlRaiseError();
    function(structuredChannel, channel, data, context, node, domain, code, level, file, line, string1, string2, string3, integer1, column,
             "%s", message ? message : "");
    free(message);
}

typedef int (*SetupFunction)(void *, void *, void *, void *);
typedef int (*GcSetupFunction)(void *, void *, void *, void *, void *);
typedef void (*GenericFunction)(void *, void *);
typedef int (*ErrorsFunction)(void *, void *, void *, void *);

HOST_FUNCTION(xmlMemSetup, SetupFunction)
HOST_FUNCTION(xmlGcMemSetup, GcSetupFunction)
HOST_FUNCTION(xmlSetGenericErrorFunc, GenericFunction)
HOST_FUNCTION(xmlThrDefSetGenericErrorFunc, GenericFunction)
HOST_FUNCTION(xmlRelaxNGSetParserErrors, ErrorsFunction)
HOST_FUNCTION(xmlRelaxNGGetParserErrors, ErrorsFunction)
HOST_FUNCTION(xmlRelaxNGSetValidErrors, ErrorsFunction)
HOST_FUNCTION(xmlRelaxNGGetValidErrors, ErrorsFunction)
HOST_FUNCTION(xmlSchemaSetParserErrors, ErrorsFunction)
HOST_FUNCTION(xmlSchemaGetParserErrors, ErrorsFunction)
HOST_FUNCTION(xmlSchemaSetValidErrors, ErrorsFunction)
HOST_FUNCTION(xmlSchemaGetValidErrors, ErrorsFunction)

int xmlMemSetup(void *freeFunction, void *mallocFunction, void *reallocFunction, void *strdupFunction)
{
    int result = host_xmlMemSetup()(freeFunction, mallocFunction, reallocFunction, strdupFunction);
    reef_xml2_refresh_data();
    return result;
}

int xmlGcMemSetup(void *freeFunction, void *mallocFunction, void *mallocAtomicFunction, void *reallocFunction, void *strdupFunction)
{
    int result = host_xmlGcMemSetup()(freeFunction, mallocFunction, mallocAtomicFunction, reallocFunction, strdupFunction);
    reef_xml2_refresh_data();
    return result;
}

void xmlSetGenericErrorFunc(void *context, void *handler)
{
    host_xmlSetGenericErrorFunc()(context, handler);
}

void xmlThrDefSetGenericErrorFunc(void *context, void *handler)
{
    host_xmlThrDefSetGenericErrorFunc()(context, handler);
}

#define ERRORS_FUNCTION(name, result) \
    result name(void *parser, void *error, void *warning, void *context) \
    { \
        return host_##name()(parser, error, warning, context); \
    }

ERRORS_FUNCTION(xmlRelaxNGSetParserErrors, int)
ERRORS_FUNCTION(xmlRelaxNGGetParserErrors, int)
ERRORS_FUNCTION(xmlRelaxNGSetValidErrors, int)
ERRORS_FUNCTION(xmlRelaxNGGetValidErrors, int)
ERRORS_FUNCTION(xmlSchemaSetParserErrors, int)
ERRORS_FUNCTION(xmlSchemaGetParserErrors, int)
ERRORS_FUNCTION(xmlSchemaSetValidErrors, int)
ERRORS_FUNCTION(xmlSchemaGetValidErrors, int)
