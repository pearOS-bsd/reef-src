// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <stdbool.h>

// Release builds of macOS answer false: no internal diagnostics on a customer system.
bool os_variant_has_internal_diagnostics(const char *subsystem) {
    (void)subsystem;
    return false;
}
