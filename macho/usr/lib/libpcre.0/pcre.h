// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// The public header of PCRE: the upstream file with the export attribute of a shared library, as `./configure` sets it.
#define PCRE_EXP_DECL extern __attribute__((visibility("default")))
#include "pcre.h.generic"
