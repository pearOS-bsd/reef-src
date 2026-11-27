// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Signatures of xar archives: the archives read here carry none (GarageBand checks them for downloaded content).
#include <stddef.h>
#include <stdint.h>

void *xar_signature_first(void *archive) { return NULL; }
void *xar_signature_next(void *signature) { return NULL; }
const char *xar_signature_type(void *signature) { return NULL; }
void *xar_signature_new(void *archive, const char *type, int32_t length, void *callback, void *context) { return NULL; }
int32_t xar_signature_add_x509certificate(void *signature, const uint8_t *data, uint32_t length) { return -1; }
int32_t xar_signature_get_x509certificate_count(void *signature) { return 0; }
int32_t xar_signature_get_x509certificate_data(void *signature, int32_t index, const uint8_t **data, uint32_t *length) { return -1; }
int32_t xar_signature_copy_signed_data(void *signature, uint8_t **data, uint32_t *length, uint8_t **signed_data, uint32_t *signed_length, long *offset) { return -1; }
uint64_t xar_get_heap_offset(void *archive) { return 0; }
const char *xar_prop_first(void *file, void *iterator) { return NULL; }
const char *xar_prop_next(void *iterator) { return NULL; }
