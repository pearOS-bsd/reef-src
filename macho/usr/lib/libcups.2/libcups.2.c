// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libcups: its functions are jumps through a table that the host's library fills in when the library is loaded.

#include "symbols.h"

extern void *reef_host_symbol(const char *library, const char *name);
extern void reef_bridge_missing(void);

enum {
#define COUNT(i, name) +1
    SYMBOL_COUNT = 0 SYMBOLS(COUNT)
#undef COUNT
};

__attribute__((visibility("hidden"))) void *reef_cups_slots[SYMBOL_COUNT];

static const char *const names[] = {
#define NAME(i, name) #name,
    SYMBOLS(NAME)
};

__attribute__((constructor)) static void resolve(void)
{
    for (int i = 0; i < SYMBOL_COUNT; i++) {
        if (reef_cups_slots[i])
            continue;
        void *function = reef_host_symbol("libcups.so.2", names[i]);
        reef_cups_slots[i] = function ? function : (void *)reef_bridge_missing;
    }
}
