#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Write the members of the rawValue structs of Reef's SwiftUI that are constants in the binary.

usage: rawvalues.py [--dry]
A struct of Sources whose only stored property is `[public] let rawValue: Swift.[U]Int[8|16|32|64]` gets, for each `_skeleton()` member
(a struct with an init of its own needs `init(rawValue a0: Swift.Int) { rawValue = a0 }` by hand):
  static var X   the integer its getter stores (`movq $N, (%rax)`, following merged `jmp`s)
  ==             rawValue equality (when the binary's == compares the two words)
  hash(into:)    combine(rawValue)   hashValue  _hashValue(for: self)  (when the binary's bodies are those)"""
import os, re, struct, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import machometa, disasm

PRO = b'\x55\x48\x89\xe5'
EQ = PRO + b'\x48\x8b\x07\x48\x3b\x06\x0f\x94\xc0\x5d\xc3'
HASH = PRO + b'\x41\x55\x50\x48\x89\xf8\x49\x8b\x7d\x00\x49\x89\xc5\xe8'


def main():
    dry = '--dry' in sys.argv
    m = machometa.MachO(disasm.BIN)
    syms = {n: v for n, t, s, v in m.symbols if s}
    names = {v: n for n, v in syms.items()}

    def body(a, depth=0):
        b = m.b[m.off(a):m.off(a) + 32]
        if b[:6] == PRO + b'\x5d\xe9' and depth < 4:
            return body(a + 10 + struct.unpack('<i', b[6:10])[0], depth + 1)
        return a, b

    def getter(sym):
        a = syms.get(sym)
        if a is None: return None
        a, b = body(a)
        if b[:7] == PRO + b'\x48\xc7\x00' and b[11:13] == b'\x5d\xc3':      # movq $imm32, (%rax)
            return struct.unpack('<i', b[7:11])[0]
        if b[:6] == PRO + b'\xc7\x00' and b[10:12] == b'\x5d\xc3':          # movl $imm32, (%rax)
            return struct.unpack('<I', b[6:10])[0]
        if b[:6] == PRO + b'\x66\xc7' and b[6:7] == b'\x00' and b[9:11] == b'\x5d\xc3':  # movw $imm16, (%rax)
            return struct.unpack('<H', b[7:9])[0]
        if b[:6] == PRO + b'\xc6\x00' and b[7:9] == b'\x5d\xc3':            # movb $imm8, (%rax)
            return b[6]
        if b[:8] == PRO + b'\x48\xc7\xc0' and b[12:14] == b'\x5d\xc3':     # movq $imm32, %rax (a direct return)
            return struct.unpack('<i', b[8:12])[0]
        if b[:5] == PRO + b'\xb8' and b[9:11] == b'\x5d\xc3':                # movl $imm32, %eax
            return struct.unpack('<I', b[5:9])[0]
        if b[:6] == PRO + b'\x31\xc0' and b[6:8] == b'\x5d\xc3':            # xorl %eax, %eax
            return 0
        return None

    def same(sym, pat):
        a = syms.get(sym)
        if a is None: return False
        a, b = body(a)
        return b[:len(pat)] == pat

    def hashvalue(sym):
        a = syms.get(sym)
        if a is None: return False
        a, b = body(a)
        n = names.get(a, '')
        return n.endswith('hashValueSivgTm') or n.endswith('hashValueSivg')

    src = os.path.join(HERE, '..', 'Sources')
    total = 0
    for root, _, files in os.walk(src):
        for f in sorted(files):
            if not f.endswith('.swift'): continue
            p = os.path.join(root, f)
            lines = open(p).read().split('\n')
            changed = False
            stack = []   # (indent, mangled prefix) of the enclosing public types
            for i, l in enumerate(lines):
                g = re.match(r'^( *)(?:@frozen )?public (struct|enum|final class|class) (\w+)\b.*\{$', l)
                if not g: continue
                ind = len(g.group(1))
                while stack and stack[-1][0] >= ind: stack.pop()
                kind = {'struct': 'V', 'enum': 'O'}.get(g.group(2), 'C')
                name = g.group(3)
                prefix = (stack[-1][1] if stack else '_$s7SwiftUI') + '%d%s%s' % (len(name), name, kind)
                stack.append((ind, prefix))
                if kind != 'V': continue
                pad = ' ' * ind
                end = i + 1
                while end < len(lines) and lines[end] != pad + '}': end += 1
                members = [x[ind:] for x in lines[i + 1:end] if x.startswith(pad + '    ') and not x.startswith(pad + '     ')]
                stored = [x for x in members if re.match(r'^    (public )?(let|var) \w+: [^{]*$', x)]
                if len(stored) != 1 or not re.match(r'^    (public )?let rawValue: Swift\.U?Int(8|16|32|64)?$', stored[0]):
                    continue
                mangled = prefix
                tname = name
                for j in range(i + 1, end):
                    l2 = lines[j]
                    if '_skeleton()' not in l2 or not l2.startswith(pad + '    ') or l2.startswith(pad + '     '): continue
                    new = None
                    s2 = re.match(r'^ *public static var (`?\w+`?): (SwiftUI\.[\w.]+) \{ _skeleton\(\) \}$', l2)
                    if s2 and s2.group(2).endswith('.' + tname):
                        m2 = s2.group(1).strip('`')
                        v = getter('%s%d%sACvgZ' % (mangled, len(m2), m2)) if mangled.count('V') + mangled.count('O') + mangled.count('C') else None
                        if v is None: v = getter('%s%d%sAEvgZ' % (mangled, len(m2), m2))
                        if v is None: v = getter('%s%d%sAGvgZ' % (mangled, len(m2), m2))
                        if v is not None: new = l2.replace('_skeleton()', '%s(rawValue: %d)' % (s2.group(2), v))
                    elif re.search(r'static func == \(_ a0: (SwiftUI\.[\w.]+), _ a1: \1\) -> Swift\.Bool \{ _skeleton\(\) \}', l2):
                        if any(same(mangled + '2eeoiySb' + t, EQ) for t in ('AC_ACtFZ', 'AE_AEtFZ', 'AG_AGtFZ')):
                            new = l2.replace('_skeleton()', 'a0.rawValue == a1.rawValue')
                    elif 'public func hash(into a0: inout Swift.Hasher) { _skeleton() }' in l2:
                        if same(mangled + '4hash4intoys6HasherVz_tF', HASH): new = l2.replace('_skeleton()', 'a0.combine(rawValue)')
                    elif 'public var hashValue: Swift.Int { _skeleton() }' in l2:
                        if hashvalue(mangled + '9hashValueSivg'): new = l2.replace('_skeleton()', '_hashValue(for: self)')
                    if new:
                        lines[j] = new; changed = True; total += 1
            if changed and not dry: open(p, 'w').write('\n'.join(lines))
    print(total, 'members written')


if __name__ == '__main__':
    main()
