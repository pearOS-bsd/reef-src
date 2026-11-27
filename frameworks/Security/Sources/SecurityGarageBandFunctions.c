// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Legacy CSSM era functions of Security
#include <stdint.h>
#include <stddef.h>

int32_t SecCertificateGetData(const void *certificate, void *data) { return -4; }   // errSecUnimplemented
int32_t SecPolicySearchCreate(uint32_t certType, const void *oid, const void *value, void **search) { return -4; }
int32_t SecPolicySearchCopyNext(void *search, void **policy) { return -25241; }   // errSecPolicyNotFound
int32_t SecTrustGetCssmResultCode(const void *trust, int32_t *code) { return -4; }
