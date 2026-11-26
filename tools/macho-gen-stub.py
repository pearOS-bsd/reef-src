#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Write the source folder of a file without code (an umbrella that re-exports other libraries, an empty Swift overlay) from the original.
usage: macho-gen-stub.py ORIGINAL FOLDER INSTALL_PATH       FOLDER is created; run it in the image, copy the folder to macho/"""
import sys, os, struct, importlib.util
here = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('cm', os.path.join(here, 'compare-macho.py'))
cm = importlib.util.module_from_spec(spec); spec.loader.exec_module(cm)
orig, folder, install = sys.argv[1:4]
I = cm.Image(orig)
assert not I.funcs, 'has code'
os.makedirs(folder, exist_ok=True)
def ver(v): return '%d.%d.%d' % (v >> 16, (v >> 8) & 255, v & 255)
hdr = '''// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
'''
libs = []
for cmd, name, cur, comp in I.dylibs:
    path = name.lstrip('/')
    libs.append(('reexport:' if cmd == cm.LC_REEXPORT else '') + path)
body = ''
for a, name, ext, sect in sorted(I.syms):
    if not ext: continue
    s = I.section(a); size = 8 if s and s['name'] == '__const' and I.data(a, 8) and a + 8 == s['addr'] + s['size'] else 1
    raw = I.data(a, size)
    if size == 1 and raw == b'\0':
        body += 'const char %s __asm__("%s") = 0;\n' % (name.lstrip('_').replace('$', '_').replace('.', '_') or 'x', name)
    elif size == 8:
        body += 'const double %s = %r;\n' % (name.lstrip('_'), struct.unpack('<d', raw)[0])
    else: raise SystemExit('cannot write %s' % name)
open(os.path.join(folder, 'stub.c'), 'w').write(hdr + ('// %s has no code; it only has its load commands%s.\n' % (os.path.basename(install), ' and what it exports' if body else '')) + ('\n' + body if body else ''))
b = I.build
minos = ver(b[2]) if b else '11.0.0'
minos = '.'.join(minos.split('.')[:2])
conf = 'install = %s\n' % install
if I.id: conf += 'id = %s\nversion = %s\ncompat = %s\n' % (I.id[1], ver(I.id[2]), ver(I.id[3]))
conf += 'sources = stub.c\nlibs = %s\nminos = %s\n' % (' '.join(libs), minos)
if I.chained: conf += 'ldflags = -fixup_chains\n'
open(os.path.join(folder, 'target.conf'), 'w').write(conf)
print(folder, 'ok')
