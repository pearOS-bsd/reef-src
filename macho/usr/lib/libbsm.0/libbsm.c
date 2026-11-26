// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libbsm: the fields of an audit token (eight words: auid, euid, egid, ruid, rgid, pid, asid, pid version).

typedef struct { unsigned int val[8]; } audit_token_t;

int audit_token_to_pid(audit_token_t token) { return token.val[5]; }
int audit_token_to_auid(audit_token_t token) { return token.val[0]; }
int audit_token_to_euid(audit_token_t token) { return token.val[1]; }
int audit_token_to_egid(audit_token_t token) { return token.val[2]; }
int audit_token_to_ruid(audit_token_t token) { return token.val[3]; }
int audit_token_to_rgid(audit_token_t token) { return token.val[4]; }
int audit_token_to_asid(audit_token_t token) { return token.val[6]; }
int audit_token_to_pidversion(audit_token_t token) { return token.val[7]; }

void audit_token_to_au32(audit_token_t token, unsigned int *auid, unsigned int *euid, unsigned int *egid, unsigned int *ruid,
                         unsigned int *rgid, int *pid, unsigned int *asid)
{
    if (auid) *auid = token.val[0];
    if (euid) *euid = token.val[1];
    if (egid) *egid = token.val[2];
    if (ruid) *ruid = token.val[3];
    if (rgid) *rgid = token.val[4];
    if (pid) *pid = token.val[5];
    if (asid) *asid = token.val[6];
}
