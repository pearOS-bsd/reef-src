#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Which parts of two Mach-O files of the same size differ: the sections, load commands and LINKEDIT tables that hold the differing bytes.
usage: macho-diff.py ORIGINAL OURS"""
import sys, os, importlib.util, collections
spec = importlib.util.spec_from_file_location('cm', os.path.join(os.path.dirname(os.path.abspath(__file__)), 'compare-macho.py'))
cm = importlib.util.module_from_spec(spec); spec.loader.exec_module(cm)
A, B = cm.Image(sys.argv[1]), cm.Image(sys.argv[2])
if len(A.b) != len(B.b): print('sizes differ: %d and %d' % (len(A.b), len(B.b)))
regions = [(s['off'], s['off'] + s['size'], s['seg'] + ',' + s['name']) for s in A.sections if s['type'] not in (1, 0xc, 0xf, 0x12)]
import struct
off = 32
for i in range(struct.unpack_from('<I', A.b, 16)[0]):
    c, sz = struct.unpack_from('<II', A.b, off); regions.append((off, off + sz, 'load command 0x%x' % c))
    if c in (0x22, 0x80000022):
        v = struct.unpack_from('<10I', A.b, off + 8)
        for n, (o, z) in zip(('rebase', 'bind', 'weak_bind', 'lazy_bind', 'export'), zip(v[0::2], v[1::2])): regions.append((o, o + z, 'linkedit ' + n))
    if c == 2:
        so, ns, sto, ssz = struct.unpack_from('<IIII', A.b, off + 8); regions += [(so, so + 16 * ns, 'linkedit symtab'), (sto, sto + ssz, 'linkedit strings')]
    if c == 0x26: o, z = struct.unpack_from('<II', A.b, off + 8); regions.append((o, o + z, 'linkedit function starts'))
    off += sz
n = min(len(A.b), len(B.b)); diff = collections.Counter(); first = {}
for i in range(n):
    if A.b[i] != B.b[i]:
        r = next((r[2] for r in regions if r[0] <= i < r[1]), 'other')
        diff[r] += 1; first.setdefault(r, i)
for r, c in diff.most_common(): print('%6d bytes differ in %s (first at %#x)' % (c, r, first[r]))
if not diff: print('identical')
