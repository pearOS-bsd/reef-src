// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

// dyld: the range of the shared cache; a process without a cache answers false (nothing is written).
bool _dyld_get_shared_cache_range(size_t *length) {
    if (length) *length = 0;
    return false;
}
// Feature flags (os_feature_enabled): none switched on.
bool _os_feature_enabled_impl(const char *domain, const char *feature) { return false; }
// The typed allocation of the C library behaves as calloc.
void *malloc_type_calloc(size_t count, size_t size, unsigned long long type_id) { return calloc(count, size); }
// XPC: requirement on the peer's entitlements; Reef's connections have no code signature to check.
void xpc_connection_set_peer_entitlement_exists_requirement(void *connection, const char *entitlement) {}
