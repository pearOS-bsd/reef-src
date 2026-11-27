// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <stdint.h>

// CSSM object identifiers (DER contents): 1.2.840.113635.100.1.2 and 1.2.840.113635.100.1.10
typedef struct { uint64_t length; const uint8_t *data; } ReefOID;
static const uint8_t x509Basic[] = {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x63, 0x64, 0x01, 0x02};
static const uint8_t softwareUpdateSigning[] = {0x2a, 0x86, 0x48, 0x86, 0xf7, 0x63, 0x64, 0x01, 0x0a};
__attribute__((weak)) const ReefOID CSSMOID_APPLE_X509_BASIC = {sizeof x509Basic, x509Basic};
__attribute__((weak)) const ReefOID CSSMOID_APPLE_TP_SW_UPDATE_SIGNING = {sizeof softwareUpdateSigning, softwareUpdateSigning};
