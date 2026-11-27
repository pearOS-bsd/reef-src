// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GSS: names, buffers and OID sets of the GSS-API work; there are no credentials or security contexts, so everything that needs them
// answers GSS_S_NO_CRED or GSS_S_NO_CONTEXT.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t OM_uint32;
typedef const void *CFTypeRef;
typedef const struct __CFString *CFStringRef;
typedef const struct __CFData *CFDataRef;
typedef struct __CFError *CFErrorRef;
typedef const struct __CFAllocator *CFAllocatorRef;

extern const CFAllocatorRef kCFAllocatorDefault;
extern unsigned long CFGetTypeID(CFTypeRef);
extern unsigned long CFStringGetTypeID(void);
extern unsigned long CFDataGetTypeID(void);
extern long CFStringGetLength(CFStringRef);
extern long CFStringGetMaximumSizeForEncoding(long, unsigned);
extern unsigned char CFStringGetCString(CFStringRef, char *, long, unsigned);
extern const uint8_t *CFDataGetBytePtr(CFDataRef);
extern long CFDataGetLength(CFDataRef);
extern CFStringRef CFStringCreateWithBytes(CFAllocatorRef, const uint8_t *, long, unsigned, unsigned char);
extern CFErrorRef CFErrorCreate(CFAllocatorRef, CFStringRef, long, const void *);

#define CFSTR(string) ((CFStringRef)__builtin___CFStringMakeConstantString(string))
#define kCFStringEncodingUTF8 0x08000100

enum {
    GSS_S_COMPLETE = 0,
    GSS_S_BAD_NAME = 2 << 16,
    GSS_S_BAD_STATUS = 5 << 16,
    GSS_S_NO_CRED = 7 << 16,
    GSS_S_NO_CONTEXT = 8 << 16,
    GSS_S_FAILURE = 13 << 16,
    GSS_S_NAME_NOT_MN = 18 << 16,
    GSS_S_UNAVAILABLE = 16 << 16,
};

// An OID: its length and the encoded bytes. (Packed to 2, as in Apple's header: 12 bytes.)
#pragma pack(push, 2)
typedef struct gss_OID_desc_struct {
    OM_uint32 length;
    void *elements;
} gss_OID_desc, *gss_OID;
#pragma pack(pop)

typedef struct {
    size_t length;
    void *value;
} gss_buffer_desc;

typedef struct {
    size_t count;
    gss_buffer_desc *elements;
} gss_buffer_set_desc;

typedef struct {
    size_t count;
    gss_OID_desc *elements;
} gss_OID_set_desc;

// A name: the text, and the OID of its type.
typedef struct {
    size_t length;
    char *value;
    gss_OID oid;
} gss_name;

typedef struct {
    void *isa;
    int flags, reserved;
    void (*invoke)(void *, void *, void *);
} BlockLiteral;

extern gss_OID_desc __gss_c_nt_user_name_oid_desc;

static unsigned char userNameOID[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x12, 0x01, 0x02, 0x01, 0x01 };
gss_OID_desc __gss_c_nt_user_name_oid_desc = { 10, userNameOID };
static unsigned char hostBasedServiceOID[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x12, 0x01, 0x02, 0x01, 0x04 };
gss_OID_desc __gss_c_nt_hostbased_service_oid_desc = { 10, hostBasedServiceOID };
static unsigned char exportNameOID[] = { 0x2b, 0x06, 0x01, 0x05, 0x06, 0x04 };
gss_OID_desc __gss_c_nt_export_name_oid_desc = { 6, exportNameOID };
static unsigned char krb5MechanismOID[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x12, 0x01, 0x02, 0x02 };
gss_OID_desc __gss_krb5_mechanism_oid_desc = { 9, krb5MechanismOID };
static unsigned char krb5CopyCcacheOID[] = { 0x2a, 0x85, 0x70, 0x2b, 0x0d, 0x01 };
gss_OID_desc __gss_krb5_copy_ccache_x_oid_desc = { 6, krb5CopyCcacheOID };

OM_uint32 gss_import_name(OM_uint32 *minor, const gss_buffer_desc *buffer, gss_OID type, gss_name **result)
{
    if (minor)
        *minor = 0;
    if (!result)
        return GSS_S_FAILURE;
    *result = NULL;
    if (!buffer)
        return GSS_S_BAD_NAME;
    size_t length = buffer->length;
    const char *text = buffer->value;
    OM_uint32 status = 0;
    gss_name *name = calloc(1, sizeof(gss_name));
    if (name) {
        name->value = malloc(length + 1);
        if (!name->value) {
            free(name);
            name = NULL;
        } else {
            memcpy(name->value, text, length);
            name->value[length] = 0;
            name->length = length;
            name->oid = type ? type : &__gss_c_nt_user_name_oid_desc;
        }
    }
    *result = name;
    if (!name)
        status = GSS_S_FAILURE;
    return status;
}

OM_uint32 gss_display_name(OM_uint32 *minor, const gss_name *name, gss_buffer_desc *buffer, gss_OID *type)
{
    if (minor)
        *minor = 0;
    if (!name)
        return GSS_S_BAD_NAME;
    if (type)
        *type = name->oid;
    if (!buffer)
        return GSS_S_COMPLETE;
    size_t length = name->length;
    const char *text = name->value;
    OM_uint32 status = GSS_S_COMPLETE;
    buffer->value = malloc(length + 1);
    if (!buffer->value) {
        status = GSS_S_FAILURE;
        length = 0;
    } else {
        memcpy(buffer->value, text, length);
        ((char *)buffer->value)[length] = 0;
    }
    buffer->length = length;
    return status;
}

OM_uint32 gss_compare_name(OM_uint32 *minor, const gss_name *name1, const gss_name *name2, int *equal)
{
    if (minor)
        *minor = 0;
    if (!name1 || !name2)
        return GSS_S_BAD_NAME;
    if (!equal)
        return GSS_S_COMPLETE;
    int same = 0;
    gss_OID type1 = name1->oid, type2 = name2->oid;
    if (type1 != type2) {
        if (!type1 || !type2)
            goto done;
        if (type1->length != type2->length)
            goto done;
        if (memcmp(type1->elements, type2->elements, type1->length) != 0)
            goto done;
    }
    if (name1->length == name2->length)
        same = memcmp(name1->value, name2->value, name1->length) == 0;
done:
    *equal = same;
    return GSS_S_COMPLETE;
}

OM_uint32 gss_export_name(OM_uint32 *minor, const gss_name *name, gss_buffer_desc *exported)
{
    if (minor)
        *minor = 0;
    if (exported) {
        exported->length = 0;
        exported->value = NULL;
    }
    return GSS_S_BAD_NAME | (name != NULL) << 20;
}

OM_uint32 gss_release_name(OM_uint32 *minor, gss_name **name)
{
    if (minor)
        *minor = 0;
    if (name && *name) {
        free((*name)->value);
        free(*name);
        *name = NULL;
    }
    return GSS_S_COMPLETE;
}

OM_uint32 gss_release_buffer(OM_uint32 *minor, gss_buffer_desc *buffer)
{
    if (minor)
        *minor = 0;
    if (buffer) {
        free(buffer->value);
        buffer->length = 0;
        buffer->value = NULL;
    }
    return GSS_S_COMPLETE;
}

OM_uint32 gss_release_buffer_set(OM_uint32 *minor, gss_buffer_set_desc **set)
{
    if (minor)
        *minor = 0;
    if (set && *set) {
        for (size_t i = 0; i < (*set)->count; i++)
            free((*set)->elements[i].value);
        free((*set)->elements);
        free(*set);
        *set = NULL;
    }
    return GSS_S_COMPLETE;
}

OM_uint32 gss_create_empty_oid_set(OM_uint32 *minor, gss_OID_set_desc **set)
{
    if (minor)
        *minor = 0;
    if (!set)
        return GSS_S_FAILURE;
    *set = calloc(1, sizeof(gss_OID_set_desc));
    return *set ? GSS_S_COMPLETE : GSS_S_FAILURE;
}

OM_uint32 gss_add_oid_set_member(OM_uint32 *minor, const gss_OID member, gss_OID_set_desc **set)
{
    if (minor)
        *minor = 0;
    if (!member || !set || !*set)
        return GSS_S_FAILURE;
    gss_OID_set_desc *oids = *set;
    size_t count = oids->count;
    gss_OID_desc *elements = oids->elements;
    gss_OID_desc *element = elements;
    for (size_t i = count; i; i--, element++) {
        if (element == member)
            return GSS_S_COMPLETE;
        if (elements && element->length == member->length && memcmp(element->elements, member->elements, element->length) == 0)
            return GSS_S_COMPLETE;
    }
    gss_OID_desc *larger = realloc(elements, (count + 1) * sizeof(gss_OID_desc));
    if (!larger)
        return GSS_S_FAILURE;
    oids->elements = larger;
    size_t length = member->length;
    void *copy = malloc(length ? length : 1);
    if (!copy)
        return GSS_S_FAILURE;
    memcpy(copy, member->elements, length);
    larger[oids->count].length = length;
    larger[oids->count].elements = copy;
    oids->count++;
    return GSS_S_COMPLETE;
}

OM_uint32 gss_release_oid_set(OM_uint32 *minor, gss_OID_set_desc **set)
{
    if (minor)
        *minor = 0;
    if (set && *set) {
        for (size_t i = 0; i < (*set)->count; i++)
            free((*set)->elements[i].elements);
        free((*set)->elements);
        free(*set);
        *set = NULL;
    }
    return GSS_S_COMPLETE;
}

OM_uint32 gss_indicate_mechs(OM_uint32 *minor, gss_OID_set_desc **mechs)
{
    if (minor)
        *minor = 0;
    if (!mechs)
        return GSS_S_FAILURE;
    *mechs = calloc(1, sizeof(gss_OID_set_desc));
    return *mechs ? GSS_S_COMPLETE : GSS_S_FAILURE;
}

// The text of a routine error (the second byte of a major status); NULL beyond the table.
static const char *routineErrorText(unsigned routine)
{
    static const char *const texts[] = {
        "No error",
        "An unsupported mechanism was requested",
        "An invalid name was supplied",
        "A supplied name was of an unsupported type",
        "Incorrect channel bindings were supplied",
        "An invalid status code was supplied",
        "A token had an invalid signature",
        "No credentials were supplied, or the credentials were unavailable or inaccessible",
        "No context has been established",
        "A token was invalid",
        "A credential was invalid",
        "The referenced credentials have expired",
        "The context has expired",
        "Unspecified GSS failure",
        "The quality-of-protection requested could not be provided",
        "The operation is forbidden by local security policy",
        "The operation or option is not available",
        "The requested credential element already exists",
        "The provided name was not a mechanism name",
    };
    return routine < 19 ? texts[routine] : NULL;
}

OM_uint32 gss_display_status(OM_uint32 *minor, OM_uint32 value, int type, const gss_OID mech, OM_uint32 *context, gss_buffer_desc *text)
{
    char message[128];
    if (minor)
        *minor = 0;
    if (!text)
        return GSS_S_FAILURE;
    if (context)
        *context = 0;
    if (type == 1) {                    // GSS_C_GSS_CODE
        const char *routine = routineErrorText((value >> 16) & 0xff);
        if (routine)
            snprintf(message, sizeof message, "%s", routine);
        else
            snprintf(message, sizeof message, "Unknown GSS status %u", value);
    } else if (type == 2) {             // GSS_C_MECH_CODE
        snprintf(message, sizeof message, "Unknown mechanism status %u", value);
    } else {
        text->length = 0;
        text->value = NULL;
        return GSS_S_BAD_STATUS;
    }
    size_t length = strlen(message);
    OM_uint32 status = GSS_S_COMPLETE;
    text->value = malloc(length + 1);
    if (!text->value) {
        status = GSS_S_FAILURE;
        length = 0;
    } else {
        memcpy(text->value, message, length);
        ((char *)text->value)[length] = 0;
    }
    text->length = length;
    return status;
}

OM_uint32 gss_acquire_cred(OM_uint32 *minor, const gss_name *name, OM_uint32 time, const gss_OID_set_desc *mechs, int usage, void **cred,
                           gss_OID_set_desc **actualMechs, OM_uint32 *timeRec)
{
    if (minor)
        *minor = 0;
    if (cred)
        *cred = NULL;
    if (actualMechs)
        *actualMechs = NULL;
    if (timeRec)
        *timeRec = 0;
    return GSS_S_NO_CRED;
}

OM_uint32 gss_inquire_cred(OM_uint32 *minor, void *cred, gss_name **name, OM_uint32 *lifetime, int *usage, gss_OID_set_desc **mechs)
{
    if (minor)
        *minor = 0;
    if (name)
        *name = NULL;
    if (lifetime)
        *lifetime = 0;
    if (mechs)
        *mechs = NULL;
    return GSS_S_NO_CRED;
}

OM_uint32 gss_inquire_cred_by_oid(OM_uint32 *minor, void *cred, const gss_OID oid, gss_buffer_set_desc **data)
{
    if (minor)
        *minor = 0;
    if (data)
        *data = NULL;
    return GSS_S_NO_CRED;
}

OM_uint32 gss_release_cred(OM_uint32 *minor, void **cred)
{
    if (minor)
        *minor = 0;
    if (cred)
        *cred = NULL;
    return GSS_S_COMPLETE;
}

OM_uint32 gss_destroy_cred(OM_uint32 *minor, void **cred)
{
    if (minor)
        *minor = 0;
    if (cred)
        *cred = NULL;
    return GSS_S_NO_CRED;
}

OM_uint32 gss_iter_creds(OM_uint32 *minor, OM_uint32 flags, const gss_OID mech, BlockLiteral *useCredential)
{
    if (minor)
        *minor = 0;
    if (useCredential)
        useCredential->invoke(useCredential, NULL, NULL);
    return GSS_S_COMPLETE;
}

OM_uint32 gss_aapl_initial_cred(const gss_OID mech, const gss_OID name, void *options, void **cred, CFErrorRef *error)
{
    if (cred)
        *cred = NULL;
    if (error)
        *error = CFErrorCreate(kCFAllocatorDefault, CFSTR("org.h5l.GSS"), GSS_S_UNAVAILABLE, NULL);
    return GSS_S_UNAVAILABLE;
}

OM_uint32 gss_init_sec_context(OM_uint32 *minor, void *cred, void **context, const gss_name *target, const gss_OID mech, OM_uint32 flags,
                               OM_uint32 time, void *bindings, const gss_buffer_desc *input, gss_OID *actualMech, gss_buffer_desc *output,
                               OM_uint32 *returnedFlags, OM_uint32 *timeRec)
{
    if (minor)
        *minor = 0;
    if (actualMech)
        *actualMech = NULL;
    if (output) {
        output->length = 0;
        output->value = NULL;
    }
    if (returnedFlags)
        *returnedFlags = 0;
    if (timeRec)
        *timeRec = 0;
    return GSS_S_NO_CRED;
}

OM_uint32 gss_delete_sec_context(OM_uint32 *minor, void **context, gss_buffer_desc *output)
{
    if (minor)
        *minor = 0;
    if (output) {
        output->length = 0;
        output->value = NULL;
    }
    OM_uint32 status = GSS_S_NO_CONTEXT;
    if (context && *context) {
        *context = NULL;
        status = GSS_S_COMPLETE;
    }
    return status;
}

OM_uint32 gss_wrap(OM_uint32 *minor, void *context, int confidentiality, OM_uint32 qop, const gss_buffer_desc *input, int *confState,
                   gss_buffer_desc *output)
{
    if (minor)
        *minor = 0;
    if (output) {
        output->length = 0;
        output->value = NULL;
    }
    return GSS_S_NO_CONTEXT;
}

OM_uint32 gss_unwrap(OM_uint32 *minor, void *context, const gss_buffer_desc *input, gss_buffer_desc *output, int *confState, OM_uint32 *qop)
{
    if (minor)
        *minor = 0;
    if (output) {
        output->length = 0;
        output->value = NULL;
    }
    return GSS_S_NO_CONTEXT;
}

gss_name *GSSCreateName(CFTypeRef name, gss_OID type, CFErrorRef *error)
{
    gss_name *result;
    if (error) {
        *error = NULL;
        if (!name)
            goto fail;
    } else if (!name) {
        return NULL;
    }
    if (CFGetTypeID(name) == CFStringGetTypeID()) {
        long size = CFStringGetMaximumSizeForEncoding(CFStringGetLength((CFStringRef)name), kCFStringEncodingUTF8) + 1;
        char *text = malloc(size);
        if (!text || !CFStringGetCString((CFStringRef)name, text, size, kCFStringEncodingUTF8)) {
            free(text);
            if (!error)
                return NULL;
            goto fail;
        }
        size_t length = strlen(text);
        result = calloc(1, sizeof(gss_name));
        if (result) {
            result->value = malloc(length + 1);
            if (!result->value) {
                free(result);
                result = NULL;
            } else {
                memcpy(result->value, text, length);
                result->value[length] = 0;
                result->length = length;
                result->oid = type ? type : &__gss_c_nt_user_name_oid_desc;
            }
        }
        free(text);
        return result;
    }
    if (CFGetTypeID(name) == CFDataGetTypeID()) {
        const uint8_t *bytes = CFDataGetBytePtr((CFDataRef)name);
        long length = CFDataGetLength((CFDataRef)name);
        result = calloc(1, sizeof(gss_name));
        if (!result)
            return NULL;
        result->value = malloc(length + 1);
        if (!result->value) {
            free(result);
            return NULL;
        }
        memcpy(result->value, bytes, length);
        result->value[length] = 0;
        result->length = length;
        result->oid = type ? type : &__gss_c_nt_user_name_oid_desc;
        return result;
    }
    if (!error)
        return NULL;
fail:
    *error = CFErrorCreate(kCFAllocatorDefault, CFSTR("org.h5l.GSS"), GSS_S_BAD_NAME, NULL);
    return NULL;
}

CFStringRef GSSNameCreateDisplayString(const gss_name *name)
{
    if (!name)
        return NULL;
    return CFStringCreateWithBytes(kCFAllocatorDefault, (const uint8_t *)name->value, name->length, kCFStringEncodingUTF8, 0);
}

CFTypeRef GSSCredentialCopyName(void *cred)
{
    return NULL;
}

int GSSCredentialGetLifetime(void *cred)
{
    return 0;
}

CFTypeRef GSSCredentialCopyUUID(void *cred)
{
    return NULL;
}
