#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Print the trampolines of a host shim (functions that are one `jmp *slot(%rip)`) as `slot name`, in slot order.
usage: macho-shim-names.py FILE.dylib"""
import sys, os, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import importlib.util
spec = importlib.util.spec_from_file_location('cm', os.path.join(os.path.dirname(os.path.abspath(__file__)), 'compare-macho.py'))
cm = importlib.util.module_from_spec(spec); spec.loader.exec_module(cm)
I = cm.Image(sys.argv[1])
rows = []
for a, n in I.funcs.items():
    d = I.data(a, 6)
    if len(d) == 6 and d[0] == 0xff and d[1] == 0x25:
        t = a + 6 + struct.unpack_from('<i', d, 2)[0]
        sym = I.symbol_at(t)
        rows.append((a, sym[0], sym[1] // 8, I.fname[a], n))
for a, s, i, name, size in rows: print(i, name.lstrip('_'), size, s)
