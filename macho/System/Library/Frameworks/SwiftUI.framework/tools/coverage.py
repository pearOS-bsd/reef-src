#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Exported symbols of Reef's SwiftUI that ours lacks.

usage: coverage.py ORIGINAL_BINARY_OR_LIST OURS_BINARY_OR_LIST [-v] [--kinds]
A list is a text file with one symbol per line; a binary is read with llvm-nm -gU (defined, external).
Prints `present/total` and, with -v, the missing names (with --kinds, counted by the kind of symbol)."""
import subprocess, sys, os, collections

def symbols(p):
    with open(p, 'rb') as f:
        magic = f.read(4)
    if magic in (b'\xcf\xfa\xed\xfe', b'\xca\xfe\xba\xbe'):
        out = subprocess.run(['llvm-nm', '-gU', '--format=just-symbols', p], capture_output=True, text=True).stdout
        return set(l.strip() for l in out.splitlines() if l.strip())
    return set(l.split()[-1] for l in open(p) if l.strip())

def kind(s):
    # the suffix of a Swift symbol says what it is
    for suf, k in (('Mn', 'type descriptor'), ('Ma', 'metadata accessor'), ('MV', 'property descriptor'), ('Mc', 'conformance descriptor'),
                   ('WP', 'witness table'), ('QOMQ', 'opaque type descriptor'), ('Tq', 'method descriptor'), ('Tj', 'dispatch thunk'),
                   ('WC', 'enum case'), ('Mp', 'protocol descriptor'), ('TL', 'requirements base'), ('Tl', 'associated type descriptor'),
                   ('Tn', 'associated conformance descriptor'), ('Tb', 'base conformance descriptor'), ('N', 'type metadata'),
                   ('fC', 'allocating init'), ('fc', 'init'), ('vg', 'getter'), ('vs', 'setter'), ('vM', 'modify'), ('F', 'function'),
                   ('FZ', 'static function'), ('vgZ', 'static getter'), ('ig', 'subscript getter'), ('Mo', 'class base offset'),
                   ('Mu', 'method lookup'), ('fD', 'deallocator'), ('fd', 'destructor'), ('Mm', 'metaclass')):
        pass
    for suf, k in sorted(((k, v) for k, v in KINDS.items()), key=lambda x: -len(x[0])):
        if s.endswith(suf): return k
    return 'other'

KINDS = {'Mn': 'type descriptor', 'Ma': 'metadata accessor', 'MV': 'property descriptor', 'Mc': 'conformance descriptor',
         'WP': 'witness table', 'QOMQ': 'opaque type descriptor', 'Tq': 'method descriptor', 'Tj': 'dispatch thunk',
         'WC': 'enum case', 'Mp': 'protocol descriptor', 'TL': 'requirements base', 'Tl': 'associated type descriptor',
         'Tn': 'associated conformance descriptor', 'Tb': 'base conformance descriptor', 'N': 'type metadata',
         'fC': 'allocating init', 'fc': 'init', 'vg': 'getter', 'vs': 'setter', 'vM': 'modify', 'F': 'function', 'FZ': 'static function',
         'vgZ': 'static getter', 'vsZ': 'static setter', 'vMZ': 'static modify', 'ig': 'subscript getter', 'is': 'subscript setter',
         'iM': 'subscript modify', 'Mo': 'class base offset', 'Mu': 'method lookup', 'fD': 'deallocator', 'fd': 'destructor', 'Mm': 'metaclass',
         'Wvd': 'field offset', 'vau': 'addressor', 'vr': 'read', 'Tu': 'async function pointer'}

def main():
    a = [x for x in sys.argv[1:] if not x.startswith('-')]
    orig, ours = symbols(a[0]), symbols(a[1])
    miss = sorted(orig - ours)
    print('%d/%d exported symbols of the original present (%.1f %%), %d missing, %d extra'
          % (len(orig) - len(miss), len(orig), 100.0 * (len(orig) - len(miss)) / max(1, len(orig)), len(miss), len(ours - orig)))
    if '--kinds' in sys.argv:
        c = collections.Counter(kind(s) for s in miss)
        for k, v in c.most_common(): print('  %5d %s' % (v, k))
    if '-v' in sys.argv:
        for s in miss: print(s)

if __name__ == '__main__':
    main()
