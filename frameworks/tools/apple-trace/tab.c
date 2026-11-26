// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
extern char my_stf[] __asm__("_reef_my_stf");
extern char orig_stf[] __asm__("_$s7SwiftUI13LayoutSubviewV12sizeThatFitsySo6CGSizeVAA16ProposedViewSizeVF");
__attribute__((used, section("__DATA,__interpose"))) struct { void *r; void *o; } tab[] = {{ my_stf, orig_stf }};
