// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libffi: a call through ffi_call (the host's table), the type descriptors that are copies of the host's, and a closure (looked up on first use).
#include <stdio.h>
#include <stddef.h>

typedef struct ffi_type {
    size_t size;
    unsigned short alignment;
    unsigned short type;
    struct ffi_type **elements;
} ffi_type;
typedef struct {
    int abi;
    unsigned nargs;
    ffi_type **arg_types;
    ffi_type *rtype;
    unsigned bytes;
    unsigned flags;
} ffi_cif;

extern ffi_type ffi_type_sint32, ffi_type_double, ffi_type_pointer, ffi_type_uint8, ffi_type_void, ffi_type_sint64, ffi_type_float;
int ffi_prep_cif(ffi_cif *, int, unsigned, ffi_type *, ffi_type **);
void ffi_call(ffi_cif *, void (*)(void), void *, void **);
void *ffi_closure_alloc(size_t, void **);
void ffi_closure_free(void *);
int ffi_prep_closure_loc(void *, ffi_cif *, void (*)(ffi_cif *, void *, void **, void *), void *, void *);

static int add(int a, int b) { return a + b * 10; }
static double mix(double x, int n, const char *s) { return x * n + s[0]; }

static void handler(ffi_cif *cif, void *result, void **args, void *data)
{
    *(int *)result = *(int *)args[0] * 3 + (int)(long)data;
}

#define SHOW(t) printf(#t " %zu %u %u\n", ffi_type_##t.size, ffi_type_##t.alignment, ffi_type_##t.type)

int main(void)
{
    SHOW(sint32); SHOW(double); SHOW(pointer); SHOW(uint8); SHOW(void); SHOW(sint64); SHOW(float);
    ffi_cif cif;
    ffi_type *args[3] = { &ffi_type_sint32, &ffi_type_sint32 };
    int rc = ffi_prep_cif(&cif, 2 /* FFI_UNIX64 */, 2, &ffi_type_sint32, args);
    int a = 4, b = 5, result = 0;
    void *values[2] = { &a, &b };
    ffi_call(&cif, (void (*)(void))add, &result, values);
    printf("prep %d call %d\n", rc, result);

    ffi_cif cif2;
    ffi_type *args2[3] = { &ffi_type_double, &ffi_type_sint32, &ffi_type_pointer };
    rc = ffi_prep_cif(&cif2, 2, 3, &ffi_type_double, args2);
    double x = 1.5; int n = 4; const char *s = "A"; double r2 = 0;
    void *values2[3] = { &x, &n, &s };
    ffi_call(&cif2, (void (*)(void))mix, &r2, values2);
    printf("prep %d mix %.1f\n", rc, r2);

    void *code;
    void *closure = ffi_closure_alloc(64, &code);
    printf("closure %d\n", closure != 0);
    if (closure) {
        ffi_cif cif3;
        ffi_type *args3[1] = { &ffi_type_sint32 };
        ffi_prep_cif(&cif3, 2, 1, &ffi_type_sint32, args3);
        rc = ffi_prep_closure_loc(closure, &cif3, handler, (void *)7, code);
        int (*fn)(int) = (int (*)(int))code;
        printf("prep closure %d, call %d\n", rc, fn(5));
        ffi_closure_free(closure);
    }
    return 0;
}
