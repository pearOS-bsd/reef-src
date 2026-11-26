// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct _copyfile_state *copyfile_state_t;
typedef unsigned int copyfile_flags_t;
#define COPYFILE_DATA (1 << 3)
copyfile_state_t copyfile_state_alloc(void);
int copyfile_state_free(copyfile_state_t);
int fcopyfile(int, int, copyfile_state_t, copyfile_flags_t);
int copyfile(const char *, const char *, copyfile_state_t, copyfile_flags_t);
#ifdef __cplusplus
}
#endif
