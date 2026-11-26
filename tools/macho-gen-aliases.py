#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""List the symbols of a Mach-O file that share an address with another exported data symbol, as `existing alias` pairs for ld64's -alias.
The existing name is the one that a CONST_STRING_DECL or another definition of the sources has.  usage: macho-gen-aliases.py MACHO SOURCE_DIR"""
import subprocess, sys, re, os, collections
macho, src = sys.argv[1:3]
text = ''
for dp, _, fs in os.walk(src):
    for f in fs:
        if f.endswith(('.c', '.m', '.h')): text += open(os.path.join(dp, f), errors='replace').read()
defined = set(re.findall(r'(?:CONST_STRING_DECL|PE_CONST_STRING_DECL)\(\s*(\w+)\s*,', text))
by = collections.defaultdict(list)
for l in subprocess.run(['/usr/lib/llvm-19/bin/llvm-nm', macho], capture_output=True, text=True).stdout.splitlines():
    p = l.split()
    if len(p) == 3 and p[1] == 'S': by[p[0]].append(p[2])
for a, names in sorted(by.items()):
    if len(names) < 2: continue
    base = [n for n in names if n[1:] in defined]
    if len(base) != 1: print('# no single definition at', a, names, file=sys.stderr); continue
    for n in names:
        if n != base[0]: print(base[0], n)
