// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libbsm: every field of an audit token and the unpacking function.
#include <stdio.h>
typedef struct { unsigned int val[8]; } audit_token_t;
int audit_token_to_auid(audit_token_t); int audit_token_to_euid(audit_token_t); int audit_token_to_egid(audit_token_t);
int audit_token_to_ruid(audit_token_t); int audit_token_to_rgid(audit_token_t); int audit_token_to_pid(audit_token_t);
int audit_token_to_asid(audit_token_t); int audit_token_to_pidversion(audit_token_t);
void audit_token_to_au32(audit_token_t, unsigned int *, unsigned int *, unsigned int *, unsigned int *, unsigned int *, int *, unsigned int *);
int main(void)
{
    audit_token_t t = {{101, 102, 103, 104, 105, 106, 107, 108}};
    printf("%d %d %d %d %d %d %d %d\n", audit_token_to_auid(t), audit_token_to_euid(t), audit_token_to_egid(t), audit_token_to_ruid(t),
           audit_token_to_rgid(t), audit_token_to_pid(t), audit_token_to_asid(t), audit_token_to_pidversion(t));
    unsigned int a = 0, e = 0, g = 0, r = 0, rg = 0, as = 0; int pid = 0;
    audit_token_to_au32(t, &a, &e, &g, &r, &rg, &pid, &as);
    printf("%u %u %u %u %u %d %u\n", a, e, g, r, rg, pid, as);
    audit_token_to_au32(t, 0, 0, 0, 0, 0, 0, 0);
    return 0;
}
