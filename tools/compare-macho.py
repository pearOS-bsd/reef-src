#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Compare the functions of two x86-64 Mach-O files after normalising addresses.

usage: compare-macho.py ORIGINAL OURS [-v] [--meta] [--strict] [--present] [--min PERCENT]

Functions are the LC_FUNCTION_STARTS entries; they are paired by symbol name (several local symbols of one name are paired in
order, functions without a name by their order).  An address operand is replaced by what it refers to: a stub or a bound
pointer by the imported symbol, a symbol by its name and offset, a C string by its text, a selector reference by the selector,
read-only data by its bytes, other data by its section.  NOPs and the padding behind the last instruction are dropped and local
jump targets are counted in instructions (as tools/compare.py does for the ELF files).  --strict keeps them.
--meta also compares what is not code: load commands, exports, imports, ObjC classes and methods, section sizes.
Prints `N/M functions, bytes = %` (bytes of the original that are matched).
"""
import sys, struct, argparse, collections
import capstone
from capstone import x86

LC_SEGMENT_64, LC_SYMTAB, LC_DYSYMTAB, LC_DYLD_INFO, LC_DYLD_INFO_ONLY = 0x19, 2, 0xb, 0x22, 0x80000022
LC_FUNCTION_STARTS, LC_ID_DYLIB, LC_LOAD_DYLIB, LC_REEXPORT, LC_WEAK, LC_LAZY = 0x26, 0xd, 0xc, 0x8000001f, 0x80000018, 0x20
LC_BUILD_VERSION, LC_CHAINED_FIXUPS = 0x32, 0x80000034

def uleb(b, o):
    r = s = 0
    while True:
        c = b[o]; o += 1; r |= (c & 0x7f) << s; s += 7
        if not c & 0x80: return r, o

def sleb(b, o):
    r = s = 0
    while True:
        c = b[o]; o += 1; r |= (c & 0x7f) << s; s += 7
        if not c & 0x80:
            if c & 0x40: r -= 1 << s
            return r, o

def cstr(b, o):
    e = b.find(b'\0', o)
    return b[o:e].decode('latin1')

class Image:
    strict = False
    def __init__(self, path):
        self.path = path
        b = self.b = open(path, 'rb').read()
        magic, cpu, sub, ftype, ncmds, sizeofcmds, flags = struct.unpack_from('<IiiIIII', b, 0)
        if magic != 0xfeedfacf: raise SystemExit(f'{path}: not a 64-bit Mach-O file')
        self.ftype, self.flags = ftype, flags
        self.segments, self.sections, self.dylibs, self.cmds = [], [], [], []
        self.symoff = self.nsyms = self.stroff = self.strsize = 0
        self.dyld = None; self.fstarts = None; self.chained = False; self.id = None; self.build = None
        off = 32
        for _ in range(ncmds):
            cmd, size = struct.unpack_from('<II', b, off)
            self.cmds.append(cmd)
            if cmd == LC_SEGMENT_64:
                name = b[off + 8:off + 24].split(b'\0')[0].decode()
                vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', b, off + 24)
                nsects = struct.unpack_from('<I', b, off + 64)[0]
                self.segments.append((name, vmaddr, vmsize, fileoff, filesize))
                so = off + 72
                for _ in range(nsects):
                    sn = b[so:so + 16].split(b'\0')[0].decode(); sg = b[so + 16:so + 32].split(b'\0')[0].decode()
                    addr, ssize, foff, align = struct.unpack_from('<QQII', b, so + 32)
                    typ = struct.unpack_from('<I', b, so + 64)[0] & 0xff
                    res1 = struct.unpack_from('<I', b, so + 68)[0]
                    self.sections.append(dict(seg=sg, name=sn, addr=addr, size=ssize, off=foff, type=typ, res1=res1))
                    so += 80
            elif cmd == LC_SYMTAB:
                self.symoff, self.nsyms, self.stroff, self.strsize = struct.unpack_from('<IIII', b, off + 8)
            elif cmd in (LC_DYLD_INFO, LC_DYLD_INFO_ONLY):
                self.dyld = struct.unpack_from('<10I', b, off + 8)
            elif cmd == LC_FUNCTION_STARTS:
                self.fstarts = struct.unpack_from('<II', b, off + 8)
            elif cmd == LC_CHAINED_FIXUPS: self.chained = True
            elif cmd in (LC_LOAD_DYLIB, LC_REEXPORT, LC_WEAK, LC_LAZY, LC_ID_DYLIB):
                no = struct.unpack_from('<I', b, off + 8)[0]
                cur, comp = struct.unpack_from('<II', b, off + 16)
                rec = (cmd, cstr(b, off + no), cur, comp)
                if cmd == LC_ID_DYLIB: self.id = rec
                else: self.dylibs.append(rec)
            elif cmd == LC_BUILD_VERSION:
                self.build = struct.unpack_from('<IIII', b, off + 8)
            off += size
        self.base = self.segments[0][1] if self.segments else 0
        self.read_symbols()
        self.binds = {}      # address of a pointer -> imported symbol
        self.lazy = {}
        self.rebases = set()
        if self.dyld:
            try: self.read_dyld()
            except IndexError: pass      # a table of a kind not decoded here (not needed for the function list)
        self.read_funcs()
        self.stubs = {}
        self.read_stubs()
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64); self.md.detail = True
        self.cache = {}

    # ---- parsing
    def section(self, a):
        for s in self.sections:
            if s['addr'] <= a < s['addr'] + s['size']: return s
        for s in self.sections:  # zero-sized or bss at the end
            if s['addr'] == a: return s
    def data(self, a, n):
        s = self.section(a)
        if not s or s['type'] in (1, 0xc, 0xf, 0x12): return b''   # zerofill
        o = s['off'] + a - s['addr']
        return self.b[o:o + min(n, s['addr'] + s['size'] - a)]
    def read_symbols(self):
        b = self.b
        self.syms = []     # (addr, name, external, sect)
        self.exports = set(); self.imports = []
        for i in range(self.nsyms):
            strx, typ, sect, desc, value = struct.unpack_from('<IBBHQ', b, self.symoff + 16 * i)
            name = cstr(b, self.stroff + strx)
            if typ & 0xe0: continue           # debugging
            t = typ & 0x0e
            if t == 0xe:
                self.syms.append((value, name, bool(typ & 1), sect))
                if typ & 1: self.exports.add(name)
            elif t == 0 and typ & 1: self.imports.append((name, (desc >> 8) & 0xff))
        self.by_addr = collections.defaultdict(list)
        for s in self.syms: self.by_addr[s[0]].append(s)
        self.sorted_syms = sorted({s[0] for s in self.syms})
    def symbol_at(self, a):
        """(name, offset) of the symbol at or before a, within the same section."""
        import bisect
        i = bisect.bisect_right(self.sorted_syms, a) - 1
        if i < 0: return None
        base = self.sorted_syms[i]
        sec = self.section(a)
        if not sec or not (sec['addr'] <= base): return None
        cands = [s for s in self.by_addr[base] if not s[1].startswith('ltmp') and not s[1].startswith('L')]
        if not cands: return None
        pick = sorted(cands, key=lambda s: (not s[2], s[1]))[0]
        return pick[1], a - base
    def read_dyld(self):
        b = self.b
        rebase_off, rebase_size, bind_off, bind_size, weak_off, weak_size, lazy_off, lazy_size, exp_off, exp_size = self.dyld
        segs = self.segments
        dylib_names = [d[1] for d in self.dylibs]
        def interp(off, size, lazy):
            o = off; end = off + size
            seg = 0; segoff = 0; name = None
            while o < end:
                op = b[o]; imm = op & 0xf; opc = op & 0xf0; o += 1
                if opc == 0x00: pass
                elif opc == 0x10 or opc == 0x30: pass
                elif opc == 0x20: _, o = uleb(b, o)
                elif opc == 0x40:
                    e = b.find(b'\0', o); name = b[o:e].decode('latin1'); o = e + 1
                elif opc == 0x50: pass
                elif opc == 0x60: _, o = sleb(b, o)
                elif opc == 0x70: seg = imm; segoff, o = uleb(b, o)
                elif opc == 0x80: v, o = uleb(b, o); segoff += v
                elif opc == 0x90:
                    (self.lazy if lazy else self.binds)[segs[seg][1] + segoff] = name; segoff += 8
                elif opc == 0xa0:
                    (self.lazy if lazy else self.binds)[segs[seg][1] + segoff] = name; v, o = uleb(b, o); segoff += 8 + v
                elif opc == 0xb0:
                    (self.lazy if lazy else self.binds)[segs[seg][1] + segoff] = name; segoff += 8 + imm * 8
                elif opc == 0xc0:
                    cnt, o = uleb(b, o); skip, o = uleb(b, o)
                    for _ in range(cnt):
                        (self.lazy if lazy else self.binds)[segs[seg][1] + segoff] = name; segoff += 8 + skip
        interp(bind_off, bind_size, False)
        interp(weak_off, weak_size, False)
        interp(lazy_off, lazy_size, True)
        # rebases
        o = rebase_off; end = o + rebase_size; seg = 0; segoff = 0
        while o < end:
            op = b[o]; imm = op & 0xf; opc = op & 0xf0; o += 1
            if opc == 0x00: break
            elif opc == 0x10 or opc == 0x20: pass
            elif opc == 0x30: seg = imm; segoff, o = uleb(b, o)
            elif opc == 0x40: pass
            elif opc == 0x50: v, o = uleb(b, o); segoff += v
            elif opc == 0x60: self.rebases.add(segs[seg][1] + segoff); segoff += 8
            elif opc == 0x70:
                for _ in range(imm): self.rebases.add(segs[seg][1] + segoff); segoff += 8
            elif opc == 0x80:
                v, o = uleb(b, o); self.rebases.add(segs[seg][1] + segoff); segoff += 8 + v
            elif opc == 0x90:
                cnt, o = uleb(b, o); skip, o = uleb(b, o)
                for _ in range(cnt): self.rebases.add(segs[seg][1] + segoff); segoff += 8 + skip
            elif opc == 0xa0: pass
    def read_funcs(self):
        self.funcs = {}
        if not self.fstarts: return
        off, size = self.fstarts; b = self.b; o = off; a = self.base
        starts = []
        while o < off + size:
            v, o = uleb(b, o)
            if v == 0: break
            a += v; starts.append(a)
        text = [s for s in self.sections if s['seg'] == '__TEXT' and s['name'] == '__text']
        end_of = {}
        for t in text:
            for st in starts:
                if t['addr'] <= st < t['addr'] + t['size']: end_of[st] = t['addr'] + t['size']
        for i, st in enumerate(starts):
            e = starts[i + 1] if i + 1 < len(starts) else end_of.get(st, st)
            e = min(e, end_of.get(st, e))
            self.funcs[st] = e - st
        # names
        self.fname = {}
        counts = collections.Counter()
        for st in starts:
            names = [s[1] for s in self.by_addr.get(st, []) if not s[1].startswith('ltmp')]
            ext = [s[1] for s in self.by_addr.get(st, []) if s[2]]
            n = (ext or names or [None])[0]
            if n is not None:
                counts[n] += 1
                n = n if counts[n] == 1 else f'{n}#{counts[n]}'
            self.fname[st] = n
        un = 0
        for st in starts:
            if self.fname[st] is None:
                self.fname[st] = f'<unnamed {un}>'; un += 1
        self.faddr = {n: a for a, n in self.fname.items()}
    def read_stubs(self):
        for s in self.sections:
            if s['type'] == 8 and s['seg'] == '__TEXT':   # S_SYMBOL_STUBS
                data = self.b[s['off']:s['off'] + s['size']]
                for i in range(0, len(data) - 5, 6 if s['name'] == '__stubs' else 6):
                    if data[i] == 0xff and data[i + 1] == 0x25:
                        ptr = s['addr'] + i + 6 + struct.unpack_from('<i', data, i + 2)[0]
                        self.stubs[s['addr'] + i] = self.lazy.get(ptr) or self.binds.get(ptr) or f'ptr@{ptr:x}'
    def pointer(self, a):
        d = self.data(a, 8)
        return int.from_bytes(d, 'little') if len(d) == 8 else None

    # ---- normalising
    def refer(self, a, fstart, fsize, size, lea):
        if fstart <= a < fstart + fsize: return ('local', a - fstart)
        if a in self.stubs: return ('import', self.stubs[a])
        if a in self.binds: return ('bound', self.binds[a])
        if a in self.lazy: return ('bound', self.lazy[a])
        if a in self.fname: return ('func', self.fname[a])
        s = self.section(a)
        if not s: return ('abs', a)
        n = s['name']
        if n == '__text': return ('func', 'mid', self.symbol_at(a))
        if s['type'] == 2:      # cstring
            d = self.b[s['off'] + a - s['addr']: s['off'] + s['size']]
            return ('str', d[:d.find(b'\0')])
        if s['type'] in (4, 6, 0xe, 0xd):   # 4, 8, 16 byte literals
            return ('lit', self.data(a, {4: 4, 6: 8, 0xe: 16}.get(s['type'], 8)))
        if n in ('__objc_selrefs', '__objc_classrefs', '__objc_superrefs', '__objc_protorefs'):
            if a in self.binds: return ('bound', self.binds[a])
            p = self.pointer(a)
            return (n, self.describe_ptr(p, 0))
        sym = self.symbol_at(a)
        if sym: return ('sym', sym[0], sym[1], n)
        if s['seg'] == '__TEXT' and size and not lea: return ('ro', self.data(a, size))
        return ('data', s['seg'], n)
    def describe_ptr(self, p, depth):
        if p is None: return None
        s = self.section(p)
        if not s: return ('abs',)
        if s['type'] == 2:
            d = self.b[s['off'] + p - s['addr']: s['off'] + s['size']]
            return ('str', d[:d.find(b'\0')])
        sym = self.symbol_at(p)
        return ('sym', sym[0], sym[1]) if sym else ('sec', s['name'])
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
        if a in self.cache: return self.cache[a]
        n = self.funcs[a]; out = []; code = self.data(a, n)
        done = 0; at = {}
        for i in self.md.disasm(code, a):
            done = i.address + i.size - a
            at[i.address - a] = len(out)
            if i.id == x86.X86_INS_NOP and not self.strict: continue
            if i.id == x86.X86_INS_INT3 and not self.strict: continue
            if i.group(capstone.CS_GRP_JUMP) or i.group(capstone.CS_GRP_CALL):
                o = i.operands[0] if i.operands else None
                if o is not None and o.type == x86.X86_OP_IMM:
                    out.append((i.mnemonic, self.refer(o.imm, a, n, 0, False))); continue
            ref = None
            for o in i.operands:
                if o.type == x86.X86_OP_MEM and o.mem.base == x86.X86_REG_RIP:
                    ref = (i.address + i.size + o.mem.disp, 0 if i.mnemonic == 'lea' else o.size)
            if ref:
                r = self.refer(ref[0], a, n, ref[1], i.mnemonic == 'lea')
                if i.mnemonic == 'lea':
                    table = self.jump_table(ref[0], a, n)
                    if len(table) > 2 and r[0] in ('data', 'sym', 'ro'): r = ('table', table)
                enc = bytes(i.bytes); d = i.disp_offset
                out.append((i.mnemonic, enc[:d] + b'....' + enc[d + 4:], r))
            else: out.append(bytes(i.bytes))
        if done != len(code): out.append(('undecoded', code[done:]))
        if not self.strict:
            def place(x):
                if type(x) is not tuple: return x
                if x[:1] == ('local',): return ('local', at.get(x[1], ('byte', x[1])))
                if x[:1] == ('table',): return ('table', tuple(at.get(t, ('byte', t)) for t in x[1]))
                return tuple(place(y) for y in x)
            out = [place(x) for x in out]
        self.cache[a] = out
        return out

def describe(x): return x.hex() if isinstance(x, bytes) else ' '.join(y.hex() if isinstance(y, bytes) else str(y) for y in x)

def pair(A, B):
    """Original function address -> our function address, by name."""
    found = {}
    for a, n in A.fname.items():
        if n.startswith('<unnamed'):
            continue
        b = B.faddr.get(n)
        if b is not None: found[a] = b
    ua = [a for a, n in A.fname.items() if n.startswith('<unnamed')]
    ub = [a for a, n in B.fname.items() if n.startswith('<unnamed')]
    for a, b in zip(ua, ub): found[a] = b
    return found

def compare(A, B):
    found = pair(A, B); same = {}
    for a, b in found.items():
        if A.normal(a) == B.normal(b): same[a] = b
    return found, same

def summary(A, same):
    total = sum(A.funcs.values()); ok = sum(A.funcs[f] for f in same)
    return f'{len(same)}/{len(A.funcs)} functions, {ok}/{total} bytes = {100 * ok / max(total, 1):.2f}%'

def objc_names(I):
    """Class and method names of the ObjC metadata, read from the method name strings the symbols do not have."""
    out = set()
    for n in I.exports:
        if n.startswith('_OBJC_CLASS_$_') or n.startswith('_OBJC_METACLASS_$_'): out.add(n)
    for a, n in I.fname.items():
        if n[:2] in ('-[', '+['): out.add(n.split('#')[0])
    return out

def meta(A, B):
    lines = []
    def diff(what, x, y):
        x, y = set(x), set(y)
        if x != y:
            lines.append(f'{what}: {len(x - y)} only in the original, {len(y - x)} only in ours')
            for v in sorted(x - y, key=str)[:6]: lines.append(f'  - {v}')
            for v in sorted(y - x, key=str)[:6]: lines.append(f'  + {v}')
    diff('load commands', collections.Counter(A.cmds).items(), collections.Counter(B.cmds).items())
    diff('dylibs', A.dylibs, B.dylibs); diff('id', [A.id], [B.id])
    diff('exports', A.exports, B.exports); diff('imports', A.imports, B.imports)
    diff('objc', objc_names(A), objc_names(B))
    diff('sections', [(s['seg'], s['name'], s['size']) for s in A.sections], [(s['seg'], s['name'], s['size']) for s in B.sections])
    diff('build version', [A.build], [B.build])
    lines.append('identical bytes' if A.b == B.b else f'bytes differ (sizes {len(A.b)} and {len(B.b)})')
    return lines

def main():
    p = argparse.ArgumentParser(); p.add_argument('orig'); p.add_argument('ours')
    p.add_argument('-v', action='store_true'); p.add_argument('--strict', action='store_true'); p.add_argument('--meta', action='store_true')
    p.add_argument('--min', type=float, default=0)
    p.add_argument('--present', action='store_true', help='count only the functions of the original that ours has (a library built from part of the source)')
    a = p.parse_args()
    Image.strict = a.strict
    A, B = Image(a.orig), Image(a.ours)
    if a.present:
        for f in [f for f in A.funcs if A.fname[f] not in B.faddr]: del A.funcs[f]
    if A.chained or B.chained: print('warning: chained fixups are not decoded, imports through the GOT compare as data')
    found, same = compare(A, B)
    print(summary(A, same))
    missing = [x for x in A.funcs if x not in found]
    if missing: print(f'{len(missing)} functions of the original have no function of that name in ours')
    if a.v:
        for f in sorted(set(found) - set(same)):
            g = found[f]; x, y = A.normal(f), B.normal(g)
            print(f'{A.fname[f]} orig {f:x} size {A.funcs[f]}, ours {g:x} size {B.funcs[g]}')
            import difflib
            m = difflib.SequenceMatcher(None, [repr(i) for i in x], [repr(i) for i in y], autojunk=False)
            for tag, i, j, k, l in m.get_opcodes():
                if tag != 'equal':
                    for v in x[i:j]: print('  -', describe(v))
                    for v in y[k:l]: print('  +', describe(v))
        for f in missing: print(f'{A.fname[f]} orig {f:x}: missing')
    if a.meta:
        for l in meta(A, B): print(l)
    total = sum(A.funcs.values()); ok = sum(A.funcs[f] for f in same)
    return 0 if ok * 100 >= a.min * total else 1

if __name__ == '__main__':
    sys.exit(main())
