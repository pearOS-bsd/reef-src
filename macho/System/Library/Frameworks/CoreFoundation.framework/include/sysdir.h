// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
typedef unsigned int sysdir_search_path_directory_t;
typedef unsigned int sysdir_search_path_domain_mask_t;
typedef struct { unsigned int dir; unsigned int domainMask; int state; } sysdir_search_path_enumeration_state_t;
#define sysdir_search_path_enumeration_state unsigned int
unsigned int sysdir_start_search_path_enumeration(sysdir_search_path_directory_t dir, sysdir_search_path_domain_mask_t domainMask);
unsigned int sysdir_get_next_search_path_enumeration(unsigned int state, char *path);
