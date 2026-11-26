// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// One jump through reef_cups_slots for every function of libcups.

#include "symbols.h"

#define TRAMPOLINE(i, name) \
    .globl _##name ;\
    .p2align 3 ;\
    _##name: ;\
    jmpq *_reef_cups_slots + i * 8(%rip) ;

.text
SYMBOLS(TRAMPOLINE)
