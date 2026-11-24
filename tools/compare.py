#!/usr/bin/env python3
"""Compare the functions of two x86-64 ELF files after normalising addresses.

usage: compare.py ORIGINAL OURS [--map FILE] [-v]
Functions are the FDE ranges of .eh_frame.  An address operand is replaced by what it refers to: a PLT import by name,
a dynamic symbol by name, read-only data by its bytes, a function by the pair it is matched with, other data by
section (a weaker check, counted as `data`).  Every original function is paired with at most one of ours.
FILE maps 'original_address our_symbol' (ours must not be stripped) for functions that cannot be paired by content.
"""
import sys, collections, argparse
from elftools.elf.elffile import ELFFile
from elftools.dwarf.callframe import FDE
import capstone
from capstone import x86

class Image:
    def __init__(self, path):
        self.f = open(path, 'rb'); self.e = e = ELFFile(self.f)
        self.sections = [(s['sh_addr'], s['sh_size'], s.name, s.data() if s['sh_type'] != 'SHT_NOBITS' else None)
                         for s in e.iter_sections() if s['sh_flags'] & 2 and s.name]
        self.names = {}
        self.got = {}
        ds = e.get_section_by_name('.dynsym')
        for n in ('.rela.dyn', '.rela.plt'):
            s = e.get_section_by_name(n)
            for r in (s.iter_relocations() if s else ()):
                if r['r_info_sym']:
                    sym = ds.get_symbol(r['r_info_sym'])
                    self.got[r['r_offset']] = sym.name
        for s in ds.iter_symbols():
            if s['st_value'] and s['st_shndx'] != 'SHN_UNDEF' and s.name and s['st_info']['type'] == 'STT_OBJECT':
                self.names[s['st_value']] = s.name
        self.symbols = {}
        st = e.get_section_by_name('.symtab')
        if st:
            for s in st.iter_symbols():
                if s['st_info']['type'] == 'STT_FUNC' and s['st_value']:
                    self.symbols[s.name] = s['st_value']
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64); self.md.detail = True
        for name in ('.plt', '.plt.got', '.plt.sec'):
            s = e.get_section_by_name(name)
            if not s: continue
            for i in self.md.disasm(s.data(), s['sh_addr']):
                if i.mnemonic.endswith('jmp') and 'rip' in i.op_str:
                    t = i.address + i.size + i.operands[0].mem.disp
                    if t in self.got:
                        a = i.address & ~0xf
                        self.names[a] = 'plt:' + self.got[t]
                        if name != '.plt': self.names[i.address] = 'plt:' + self.got[t]
        self.funcs = {}
        dw = e.get_dwarf_info(relocate_dwarf_sections=False)
        for c in dw.EH_CFI_entries():
            if isinstance(c, FDE):
                a, n = c.header['initial_location'], c.header['address_range']
                sec = self.section(a)
                if sec and sec[2] == '.text': self.funcs[a] = n
        start = e.header['e_entry']
        self.funcs.pop(start, None)
    def section(self, a):
        for s in self.sections:
            if s[0] <= a < s[0] + s[1]: return s
    def data(self, a, n):
        s = self.section(a)
        return s[3][a - s[0]:a - s[0] + n] if s and s[3] else b''
    def refer(self, a, fstart, fsize, size):
        if fstart <= a < fstart + fsize: return ('local', a - fstart), None
        if a in self.names: return ('sym', self.names[a]), None
        if a in self.got: return ('got', self.got[a]), None
        s = self.section(a)
        if not s: return ('abs', a), None
        if s[2] == '.text': return ('func',), a
        if s[2].startswith('.rodata'):
            if size: return ('ro', self.data(a, size)), None
            d = s[3][a - s[0]:]
            z = d.find(b'\0')
            text = d[:z]
            if z >= 0 and all(9 <= c < 127 or c >= 128 for c in text): return ('str', text), None
            return ('rodata',), None
        return ('data', s[2]), None
    def jump_table(self, a, fstart, fsize):
        out = []
        while True:
            d = self.data(a + 4 * len(out), 4)
            if len(d) < 4: break
            t = a + int.from_bytes(d, 'little', signed=True) - fstart
            if not 0 <= t < fsize: break
            out.append(t)
        return tuple(out)
    def normal(self, a):
        n = self.funcs[a]; out = []; calls = []
        code = self.data(a, n); done = 0
        for i in self.md.disasm(code, a):
            done = i.address + i.size - a
            ref = None
            if i.group(capstone.CS_GRP_JUMP) or i.group(capstone.CS_GRP_CALL):
                o = i.operands[0]
                if o.type == x86.X86_OP_IMM:
                    r, f = self.refer(o.imm, a, n, 0); out.append((i.mnemonic, r))
                    if f: calls.append(f)
                    continue
            for o in i.operands:
                if o.type == x86.X86_OP_MEM and o.mem.base == x86.X86_REG_RIP:
                    ref = (i.address + i.size + o.mem.disp, 0 if i.mnemonic == 'lea' else o.size)
            if ref:
                r, f = self.refer(ref[0], a, n, ref[1])
                if f: calls.append(f)
                if r == ('rodata',) or r[0] == 'str':
                    table = self.jump_table(ref[0], a, n)
                    if i.mnemonic == 'lea' and len(table) > 2: r = ('table', table)
                enc = bytes(i.bytes); d = i.disp_offset
                out.append((i.mnemonic, enc[:d] + b'....' + enc[d + 4:], r))
            else: out.append(bytes(i.bytes))
        if done != n: out.append(('undecoded', code[done:]))
        return out, calls

def describe(x): return x.hex() if isinstance(x, bytes) else ' '.join(y.hex() if isinstance(y, bytes) else str(y) for y in x)

def main():
    p = argparse.ArgumentParser(); p.add_argument('orig'); p.add_argument('ours')
    p.add_argument('--map'); p.add_argument('-v', action='store_true'); p.add_argument('--min', type=float, default=0)
    a = p.parse_args()
    A, B = Image(a.orig), Image(a.ours)
    na = {f: A.normal(f) for f in A.funcs}; nb = {f: B.normal(f) for f in B.funcs}
    index = collections.defaultdict(list)
    for f, (o, _) in nb.items(): index[repr(o)].append(f)
    pairs = {}
    for f in sorted(na):
        l = index[repr(na[f][0])]
        if l: pairs[f] = l
    # A pair must also agree on the functions it refers to: drop candidates until nothing changes.
    changed = True
    while changed:
        changed = False
        for f in list(pairs):
            keep = [g for g in pairs[f] if len(nb[g][1]) == len(na[f][1]) and
                    all((x not in na and y not in nb) or y in pairs.get(x, ()) for x, y in zip(na[f][1], nb[g][1]))]
            if keep != pairs[f]:
                changed = True
                if keep: pairs[f] = keep
                else: del pairs[f]
    used, match = set(), {}
    for f in sorted(pairs, key=lambda f: len(pairs[f])):
        for g in pairs[f]:
            if g not in used: used.add(g); match[f] = g; break
    total = sum(A.funcs.values()); ok = sum(A.funcs[f] for f in match)
    print(f'{len(match)}/{len(A.funcs)} functions, {ok}/{total} bytes = {100 * ok / max(total, 1):.2f}%')
    if a.v:
        left = [g for g in nb if g not in used]
        for f in sorted(set(na) - set(match)):
            if not left: print(f'{f:x} {A.funcs[f]} no candidate'); continue
            import difflib
            sc = [(difflib.SequenceMatcher(None, [repr(x) for x in na[f][0]], [repr(x) for x in nb[g][0]], autojunk=False), g) for g in left]
            m, g = max(sc, key=lambda t: t[0].ratio())
            print(f'{f:x} size {A.funcs[f]} / {g:x} size {B.funcs[g]} similar {m.ratio():.3f}')
            for tag, i, j, k, l in m.get_opcodes():
                if tag != 'equal':
                    for x in na[f][0][i:j]: print('  -', describe(x))
                    for x in nb[g][0][k:l]: print('  +', describe(x))
    return 0 if ok * 100 >= a.min * total else 1
sys.exit(main())
