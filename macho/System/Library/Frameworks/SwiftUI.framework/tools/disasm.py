#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Disassemble functions of Reef's SwiftUI with the names of the imported functions its stubs jump to.

usage: disasm.py REGEX [BINARY]      every defined function whose symbol matches REGEX
       disasm.py --const ADDR...     the 8 bytes at ADDR as a double (constants named by `## 0x...` comments)"""
import os, re, struct, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import machometa

ROOT = os.path.abspath(os.path.join(HERE, '../../../../../..'))
BIN = os.path.join(ROOT, 'upstream/cutefish-reef/usr/lib/cutefish-reef/System/Library/Frameworks/SwiftUI.framework/Versions/A/SwiftUI')


def stub_names(m):
    cache = {}
    def name(a):
        if a not in cache:
            cache[a] = None
            try:
                b = m.b[m.off(a):m.off(a) + 6]
                if b[:2] == b'\xff\x25':
                    f = m.fix.get(a + 6 + struct.unpack('<i', b[2:6])[0])
                    if f and f[0] == 'bind': cache[a] = f[1]
            except Exception:
                pass
        return cache[a]
    return name


def annotator(m):
    """Name what a `## 0xADDR` comment points at: an import, a string (C string, Swift literal at ADDR+0x20),
    a selector or class reference, or a symbol."""
    sect = []
    for sname, lst in m.sections.items():
        for seg, addr, size, off in lst: sect.append((addr, addr + size, sname))
    sym = {v: n for n, t, ns, v in m.symbols if ns}
    def where(a):
        for lo, hi, n in sect:
            if lo <= a < hi: return n
    def cstr(a):
        try:
            o = m.off(a); e = m.b.index(b'\0', o, o + 200)
            t = m.b[o:e].decode()
            return t if t and t.isprintable() else None
        except Exception:
            return None
    def note(a):
        if a in sym: return sym[a]
        f = m.fix.get(a)
        if f:
            if f[0] == 'bind': return f[1]
            t = f[1] & 0xffffffffffff
            if t in sym: return '&' + sym[t]
            s = cstr(t)
            if s: return '&"%s"' % s
            return None
        w = where(a)
        if w in ('__cstring', '__objc_methname', '__swift5_reflstr', '__const'):
            s, l = cstr(a), cstr(a + 0x20)
            if s or l: return ' | '.join(x for x in ('"%s"' % s if s else '', 'literal "%s"' % l if l else '') if x)
        return None
    return note


def main():
    args = sys.argv[1:]
    if args and args[0] == '--const':
        m = machometa.MachO(BIN)
        for a in args[1:]:
            a = int(a, 16)
            print(hex(a), struct.unpack('<d', m.b[m.off(a):m.off(a) + 8])[0])
        return
    rx = re.compile(args[0]); binary = args[1] if len(args) > 1 else BIN
    m = machometa.MachO(binary)
    name = stub_names(m)
    note = annotator(m)
    syms = subprocess.run(['llvm-nm', binary], capture_output=True, text=True).stdout.split('\n')
    for l in syms:
        p = l.split()
        if len(p) != 3 or p[1] not in 'tT' or not rx.search(p[2]): continue
        out = subprocess.run(['llvm-objdump', '-d', '--no-show-raw-insn', '--disassemble-symbols=' + p[2], binary],
                             capture_output=True, text=True).stdout.split('\n')[4:]
        for line in out:
            line = re.sub(r'0x([0-9a-f]+) <dyld_stub_binder\+0x[0-9a-f]+>', lambda g: name(int(g[1], 16)) or g[0], line)
            g = re.search(r'## 0x([0-9a-f]+)(?: <dyld_stub_binder[^>]*>)?$', line)
            if g:
                n = note(int(g[1], 16))
                if n: line = re.sub(r' <dyld_stub_binder[^>]*>$', '', line) + '  ' + n
            if line.strip() and not line.strip().endswith(('nop', 'int3')): print(line)


if __name__ == '__main__':
    main()
