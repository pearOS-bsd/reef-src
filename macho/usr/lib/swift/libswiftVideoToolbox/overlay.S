// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Overlay of the framework: apps import no symbol from it (checked with reef --list-missing on the apps of the survey); the library has to exist so that it loads.
.text
.globl _reef_overlay_marker
_reef_overlay_marker:
    ret
