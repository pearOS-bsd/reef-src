#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""A shared library to link against in place of a real one: the same soname and the same exported names, functions and
objects of the same sizes, but no code. CI links the WebKit host with it (the real library is 129 MB).
usage: link-stub.py LIBRARY OUT.so"""
import re, subprocess, sys, tempfile, os
library, out = sys.argv[1:3]
soname = re.search(r'SONAME\)\s+Library soname: \[(.*)\]', subprocess.run(['readelf', '-dW', library], capture_output=True, text=True).stdout)[1]
lines = []
for line in subprocess.run(['nm', '-D', '--defined-only', '-S', library], capture_output=True, text=True).stdout.splitlines():
    parts = line.split()
    if len(parts) == 4: _, size, kind, name = parts
    elif len(parts) == 3: _, kind, name = parts; size = '1'
    else: continue
    if '@' in name: continue
    if kind in 'TtWi': lines.append(f'void {name}(void) {{}}')
    elif kind in 'DdBbRrVvGgSsu': lines.append(f'char {name}[{max(int(size, 16), 1)}];')
with tempfile.NamedTemporaryFile('w', suffix='.c', delete=False) as c:
    c.write('\n'.join(lines) + '\n')
subprocess.run(['gcc', '-shared', '-fPIC', '-w', '-o', out, c.name, '-Wl,-soname,' + soname], check=True)
os.unlink(c.name)
print(out, soname, len(lines), 'symbols')
