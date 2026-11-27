// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// GSS: names (import, display, compare, export, release), buffers, OID sets, status texts, and the calls that fail for lack of credentials
// and contexts; GSSCreateName from a string and from data.
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef uint32_t U;
#pragma pack(push, 2)
typedef struct { U length; void *elements; } OID;
#pragma pack(pop)
typedef struct { size_t length; void *value; } Buf;
typedef struct { size_t count; OID *elements; } OIDSet;
typedef struct { size_t count; Buf *elements; } BufSet;
typedef struct { size_t length; char *value; OID *oid; } Name;

extern OID __gss_c_nt_user_name_oid_desc, __gss_c_nt_hostbased_service_oid_desc, __gss_c_nt_export_name_oid_desc, __gss_krb5_mechanism_oid_desc, __gss_krb5_copy_ccache_x_oid_desc;
U gss_import_name(U *, const Buf *, OID *, Name **);
U gss_display_name(U *, const Name *, Buf *, OID **);
U gss_compare_name(U *, const Name *, const Name *, int *);
U gss_export_name(U *, const Name *, Buf *);
U gss_release_name(U *, Name **);
U gss_release_buffer(U *, Buf *);
U gss_release_buffer_set(U *, BufSet **);
U gss_create_empty_oid_set(U *, OIDSet **);
U gss_add_oid_set_member(U *, OID *, OIDSet **);
U gss_release_oid_set(U *, OIDSet **);
U gss_indicate_mechs(U *, OIDSet **);
U gss_display_status(U *, U, int, OID *, U *, Buf *);
U gss_acquire_cred(U *, Name *, U, OIDSet *, int, void **, OIDSet **, U *);
U gss_inquire_cred(U *, void *, Name **, U *, int *, OIDSet **);
U gss_release_cred(U *, void **);
U gss_destroy_cred(U *, void **);
U gss_delete_sec_context(U *, void **, Buf *);
U gss_wrap(U *, void *, int, U, const Buf *, int *, Buf *);
U gss_unwrap(U *, void *, const Buf *, Buf *, int *, U *);
U gss_aapl_initial_cred(OID *, OID *, void *, void **, void **);
extern Name *GSSCreateName(const void *, OID *, void **);
extern const void *GSSNameCreateDisplayString(const Name *);
extern const void *CFStringCreateWithCString(const void *, const char *, unsigned);
extern const void *CFDataCreate(const void *, const unsigned char *, long);
extern unsigned char CFStringGetCString(const void *, char *, long, unsigned);
extern long CFErrorGetCode(const void *);
extern void CFRelease(const void *);

int main(void)
{
    U minor = 9;
    Buf in = { 5, "alice" };
    Name *a = 0, *b = 0, *c = 0;
    printf("import %x\n", gss_import_name(&minor, &in, 0, &a));
    printf("import null %x %x\n", gss_import_name(0, 0, 0, &b), gss_import_name(0, &in, 0, 0));
    Buf host = { 6, "x@host" };
    printf("import host %x\n", gss_import_name(0, &host, &__gss_c_nt_hostbased_service_oid_desc, &b));
    gss_import_name(0, &in, &__gss_c_nt_user_name_oid_desc, &c);
    OID *type = 0;
    Buf shown = { 99, 0 };
    printf("display %x '%s' %zu type %u\n", gss_display_name(0, a, &shown, &type), (char *)shown.value, shown.length, type ? type->length : 0);
    printf("display none %x %x\n", gss_display_name(0, 0, &shown, 0), gss_display_name(0, a, 0, 0));
    int eq = 7;
    printf("compare %x %d", gss_compare_name(0, a, c, &eq), eq);
    gss_compare_name(0, a, b, &eq); printf(" %d", eq);
    gss_compare_name(0, c, a, &eq); printf(" %d\n", eq);
    printf("compare bad %x %x\n", gss_compare_name(0, 0, c, &eq), gss_compare_name(0, a, c, 0));
    Buf ex = { 1, (void *)1 };
    printf("export %x %x len %zu\n", gss_export_name(0, a, &ex), gss_export_name(0, 0, 0), ex.length);
    printf("release buffer %x %zu\n", gss_release_buffer(&minor, &shown), shown.length);
    gss_release_buffer(0, 0);
    printf("release name %x %d", gss_release_name(0, &a), a == 0);
    printf(" %x\n", gss_release_name(0, &a));
    gss_release_name(0, &b); gss_release_name(0, &c);

    OIDSet *set = 0;
    printf("oid set %x %x\n", gss_create_empty_oid_set(0, &set), gss_create_empty_oid_set(0, 0));
    printf("add %x %x %x %x\n", gss_add_oid_set_member(0, &__gss_krb5_mechanism_oid_desc, &set), gss_add_oid_set_member(0, &__gss_krb5_mechanism_oid_desc, &set),
           gss_add_oid_set_member(0, &__gss_krb5_copy_ccache_x_oid_desc, &set), gss_add_oid_set_member(0, 0, &set));
    unsigned char copy[9]; memcpy(copy, __gss_krb5_mechanism_oid_desc.elements, 9);
    OID same = { 9, copy };
    printf("add equal bytes %x count %zu\n", gss_add_oid_set_member(0, &same, &set), set->count);
    printf("release set %x %d\n", gss_release_oid_set(0, &set), set == 0);
    printf("mechs %x ", gss_indicate_mechs(0, &set)); printf("%zu\n", set->count); gss_release_oid_set(0, &set);

    for (U code = 0; code <= 19; code += 1) {
        Buf t = { 0, 0 }; U ctx = 5;
        U r = gss_display_status(0, code << 16, 1, 0, &ctx, &t);
        printf("status %u: %x '%s' ctx %u\n", code, r, t.value ? (char *)t.value : "", ctx);
        gss_release_buffer(0, &t);
    }
    Buf t = { 0, 0 };
    printf("mech status %x '%s'\n", gss_display_status(0, 7, 2, 0, 0, &t), (char *)t.value); gss_release_buffer(0, &t);
    printf("bad type %x %zu, no buffer %x\n", gss_display_status(0, 7, 3, 0, 0, &t), t.length, gss_display_status(0, 7, 1, 0, 0, 0));

    void *cred = (void *)1; OIDSet *m = (OIDSet *)1; U life = 3;
    printf("acquire %x cred %p\n", gss_acquire_cred(0, 0, 0, 0, 0, &cred, &m, &life), cred);
    Name *n = (Name *)1; int usage;
    printf("inquire %x\n", gss_inquire_cred(0, 0, &n, &life, &usage, &m));
    cred = (void *)1; printf("release cred %x destroy %x\n", gss_release_cred(0, &cred), gss_destroy_cred(0, &cred));
    void *ctx = (void *)1; Buf out = { 1, (void *)1 };
    printf("delete ctx %x ", gss_delete_sec_context(0, &ctx, &out)); printf("%x\n", gss_delete_sec_context(0, &ctx, 0));
    printf("wrap %x unwrap %x\n", gss_wrap(0, 0, 0, 0, 0, 0, &out), gss_unwrap(0, 0, 0, &out, 0, 0));
    void *err = 0, *c2 = (void *)1;
    printf("initial cred %x code %ld\n", gss_aapl_initial_cred(0, 0, 0, &c2, &err), err ? CFErrorGetCode(err) : -1);

    const void *s = CFStringCreateWithCString(0, "bob", 0x08000100);
    Name *n1 = GSSCreateName(s, 0, &err);
    printf("create string %d %zu %s err %d\n", n1 != 0, n1 ? n1->length : 0, n1 ? n1->value : "", err != 0);
    const void *d = GSSNameCreateDisplayString(n1); char buf[32] = "?"; CFStringGetCString(d, buf, sizeof buf, 0x08000100);
    printf("display string %s\n", buf);
    gss_release_name(0, &n1);
    const void *data = CFDataCreate(0, (const unsigned char *)"carol", 5);
    n1 = GSSCreateName(data, &__gss_krb5_mechanism_oid_desc, &err);
    printf("create data %d %s type %u\n", n1 != 0, n1 ? n1->value : "", n1 ? n1->oid->length : 0);
    gss_release_name(0, &n1);
    n1 = GSSCreateName(0, 0, &err);
    printf("create none %d err %ld\n", n1 != 0, err ? CFErrorGetCode(err) : -1);
    printf("create none no error %d\n", GSSCreateName(0, 0, 0) != 0);
    return 0;
}
