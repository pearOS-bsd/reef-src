// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libffi: the closure functions look their host functions up on first use.

#include <stddef.h>

extern void *reef_host_symbol(const char *library, const char *name);

static struct {
    void *(*alloc)(size_t, void **);
    void (*release)(void *);
    int (*prepare)(void *, void *, void *, void *, void *);
} host;

static int available(void)
{
    if (!host.prepare) {
        host.alloc = reef_host_symbol("libffi.so.8", "ffi_closure_alloc");
        host.release = reef_host_symbol("libffi.so.8", "ffi_closure_free");
        host.prepare = reef_host_symbol("libffi.so.8", "ffi_prep_closure_loc");
    }
    return host.alloc && host.release && host.prepare;
}

void *ffi_closure_alloc(size_t size, void **code)
{
    if (!available())
        return NULL;
    return host.alloc(size > 128 ? size : 128, code);
}

void ffi_closure_free(void *closure)
{
    if (closure && available())
        host.release(closure);
}

int ffi_prep_closure_loc(void *closure, void *cif, void *fun, void *user_data, void *codeloc)
{
    if (!available())
        return 2;
    return host.prepare(closure, cif, fun, user_data, codeloc);
}
