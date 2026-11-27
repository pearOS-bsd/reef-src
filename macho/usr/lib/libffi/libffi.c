// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libffi: the calls jump through a table that the host's libffi fills in when the library is loaded; the type descriptors (ffi_type_sint32 ...) are copies of the host's, made by reef_ffi_refresh_data.

#include <stddef.h>

#include "symbols.h"

extern void *reef_host_symbol(const char *library, const char *name);
extern void reef_bridge_missing(void);

typedef struct ffi_type {
    size_t size;
    unsigned short alignment;
    unsigned short type;
    struct ffi_type **elements;
} ffi_type;

enum {
#define COUNT(i, name) +1
    SYMBOL_COUNT = 0 SYMBOLS(COUNT)
#undef COUNT
};

__attribute__((visibility("hidden"))) void *reef_ffi_slots[SYMBOL_COUNT];

ffi_type ffi_type_complex_double __attribute__((aligned(16)));
ffi_type ffi_type_complex_float __attribute__((aligned(16)));
ffi_type ffi_type_complex_longdouble __attribute__((aligned(16)));
ffi_type ffi_type_double __attribute__((aligned(16)));
ffi_type ffi_type_float __attribute__((aligned(16)));
ffi_type ffi_type_longdouble __attribute__((aligned(16)));
ffi_type ffi_type_pointer __attribute__((aligned(16)));
ffi_type ffi_type_sint16 __attribute__((aligned(16)));
ffi_type ffi_type_sint32 __attribute__((aligned(16)));
ffi_type ffi_type_sint64 __attribute__((aligned(16)));
ffi_type ffi_type_sint8 __attribute__((aligned(16)));
ffi_type ffi_type_uint16 __attribute__((aligned(16)));
ffi_type ffi_type_uint32 __attribute__((aligned(16)));
ffi_type ffi_type_uint64 __attribute__((aligned(16)));
ffi_type ffi_type_uint8 __attribute__((aligned(16)));
ffi_type ffi_type_void __attribute__((aligned(16)));

static const char *const names[] = {
#define NAME(i, name) #name,
    SYMBOLS(NAME)
};

__attribute__((constructor)) static void resolve(void)
{
    for (int i = 0; i < SYMBOL_COUNT; i++) {
        if (reef_ffi_slots[i])
            continue;
        void *function = reef_host_symbol("libffi.so.8", names[i]);
        reef_ffi_slots[i] = function ? function : (void *)reef_bridge_missing;
    }
}

#define COPY(type) \
    do {                                                                      \
        const void *host = reef_host_symbol("libffi.so.8", #type);            \
        if (host)                                                             \
            __builtin_memcpy(&type, host, sizeof(ffi_type));                  \
    } while (0)

void reef_ffi_refresh_data(void)
{
    COPY(ffi_type_complex_double);
    COPY(ffi_type_complex_float);
    COPY(ffi_type_complex_longdouble);
    COPY(ffi_type_double);
    COPY(ffi_type_float);
    COPY(ffi_type_longdouble);
    COPY(ffi_type_pointer);
    COPY(ffi_type_sint16);
    COPY(ffi_type_sint32);
    COPY(ffi_type_sint64);
    COPY(ffi_type_sint8);
    COPY(ffi_type_uint16);
    COPY(ffi_type_uint32);
    COPY(ffi_type_uint64);
    COPY(ffi_type_uint8);
    COPY(ffi_type_void);
}

__attribute__((constructor)) static void fill(void)
{
    reef_ffi_refresh_data();
}
