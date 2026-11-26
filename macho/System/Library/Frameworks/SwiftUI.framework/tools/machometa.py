# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Read an x86-64 Mach-O file with chained fixups: sections, symbols, fixups, and the Swift metadata
(context descriptors, fields, conformances, associated types, opaque types).

Mangled names are returned as latin-1 strings in which every symbolic reference is the byte 0x01 followed by
our own 4-byte index k (k >= 0 a type, -k-1 a protocol, 0x1000000 + k an opaque type descriptor), the form
demangle-refs.cpp understands; `MachO.refs[k]` says what k stands for."""
import struct

LC_SEGMENT_64, LC_SYMTAB, LC_DYSYMTAB, LC_LOAD_DYLIB, LC_ID_DYLIB = 0x19, 0x2, 0xb, 0xc, 0xd
LC_LOAD_WEAK_DYLIB, LC_REEXPORT_DYLIB, LC_CHAINED_FIXUPS, LC_EXPORTS_TRIE = 0x80000018, 0x8000001f, 0x80000034, 0x80000033

KIND_NAMES = {0: 'module', 1: 'extension', 2: 'anonymous', 3: 'protocol', 4: 'opaque', 16: 'class', 17: 'struct', 18: 'enum'}
KIND_LETTER = {'class': 'C', 'struct': 'V', 'enum': 'O', 'protocol': 'P'}


class MachO:
    def __init__(self, path):
        self.b = b = open(path, 'rb').read()
        magic, cpu, sub, ftype, ncmds, sizecmds, flags, _ = struct.unpack_from('<8I', b, 0)
        assert magic == 0xfeedfacf, 'not a 64-bit Mach-O'
        self.segments = []; self.sections = {}; self.dylibs = []; self.symbols = []
        self.fix = {}  # vmaddr -> ('rebase', target) | ('bind', name, addend)
        o = 32
        chained = None
        for _ in range(ncmds):
            cmd, size = struct.unpack_from('<II', b, o)
            if cmd == LC_SEGMENT_64:
                name = b[o + 8:o + 24].rstrip(b'\0').decode()
                vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<4Q', b, o + 24)
                nsect = struct.unpack_from('<I', b, o + 64)[0]
                self.segments.append((name, vmaddr, vmsize, fileoff, filesize))
                s = o + 72
                for _ in range(nsect):
                    sname = b[s:s + 16].rstrip(b'\0').decode()
                    seg = b[s + 16:s + 32].rstrip(b'\0').decode()
                    addr, ssize = struct.unpack_from('<2Q', b, s + 32)
                    soff = struct.unpack_from('<I', b, s + 48)[0]
                    self.sections.setdefault(sname, []).append((seg, addr, ssize, soff))
                    s += 80
            elif cmd == LC_SYMTAB:
                symoff, nsyms, stroff, strsize = struct.unpack_from('<4I', b, o + 8)
                for i in range(nsyms):
                    strx, ntype, nsect, ndesc, value = struct.unpack_from('<IBBHQ', b, symoff + 16 * i)
                    e = b.index(b'\0', stroff + strx)
                    self.symbols.append((b[stroff + strx:e].decode(errors='replace'), ntype, nsect, value))
            elif cmd in (LC_LOAD_DYLIB, LC_LOAD_WEAK_DYLIB, LC_REEXPORT_DYLIB):
                noff = struct.unpack_from('<I', b, o + 8)[0]
                self.dylibs.append(b[o + noff:b.index(b'\0', o + noff)].decode())
            elif cmd == LC_CHAINED_FIXUPS:
                chained = struct.unpack_from('<II', b, o + 8)
            o += size
        self.base = min(s[1] for s in self.segments if s[0] == '__TEXT')
        self.sym_at = {}
        for name, ntype, nsect, value in self.symbols:
            if nsect and not (ntype & 0xe0):
                self.sym_at.setdefault(value, []).append(name)
        self.addr_of = {}
        for name, ntype, nsect, value in self.symbols:
            if nsect and not (ntype & 0xe0):
                self.addr_of.setdefault(name, value)
        if chained:
            self._chained(*chained)
        self.refs = []      # placeholder index -> ('local', addr) | ('extern', symbol)
        self._refkey = {}

    # ---- raw access
    def off(self, a):
        for name, vmaddr, vmsize, fileoff, filesize in self.segments:
            if vmaddr <= a < vmaddr + filesize:
                return a - vmaddr + fileoff
        raise ValueError('address 0x%x not in the file' % a)

    def u8(self, a): return self.b[self.off(a)]
    def u16(self, a): return struct.unpack_from('<H', self.b, self.off(a))[0]
    def u32(self, a): return struct.unpack_from('<I', self.b, self.off(a))[0]
    def i32(self, a): return struct.unpack_from('<i', self.b, self.off(a))[0]
    def u64(self, a): return struct.unpack_from('<Q', self.b, self.off(a))[0]

    def cstr(self, a):
        o = self.off(a)
        return self.b[o:self.b.index(b'\0', o)].decode(errors='replace')

    def section(self, name):
        return self.sections.get(name, [])

    def name_at(self, a, prefer=None):
        n = self.sym_at.get(a)
        if not n: return None
        if prefer:
            for x in n:
                if x.endswith(prefer): return x
        return n[0]

    # ---- chained fixups
    def _chained(self, dataoff, datasize):
        b = self.b
        ver, starts, imports, syms, icount, ifmt, sfmt = struct.unpack_from('<7I', b, dataoff)
        names = []
        for i in range(icount):
            if ifmt == 1:
                v = struct.unpack_from('<I', b, dataoff + imports + 4 * i)[0]; noff = v >> 9; add = 0; lib = v & 0xff
            elif ifmt == 2:
                v, add = struct.unpack_from('<Ii', b, dataoff + imports + 8 * i); noff = v >> 9; lib = v & 0xff
            else:
                v, add = struct.unpack_from('<QQ', b, dataoff + imports + 16 * i); noff = v >> 32; lib = v & 0xffff
            p = dataoff + syms + noff
            names.append((b[p:b.index(b'\0', p)].decode(), add, lib))
        self.imports = names
        sa = dataoff + starts
        segcount = struct.unpack_from('<I', b, sa)[0]
        for si in range(segcount):
            so = struct.unpack_from('<I', b, sa + 4 + 4 * si)[0]
            if not so: continue
            s = sa + so
            size, psize, pfmt, segoff, maxp, pcount = struct.unpack_from('<IHHQIH', b, s)
            segvm = self.base + segoff
            for pi in range(pcount):
                start = struct.unpack_from('<H', b, s + 22 + 2 * pi)[0]
                if start == 0xffff: continue
                assert not (start & 0x8000), 'multi-start pages are not handled'
                a = segvm + pi * psize + start
                while True:
                    v = struct.unpack_from('<Q', b, self.off(a))[0]
                    nxt = (v >> 51) & 0xfff
                    if v >> 63:
                        ordinal = v & 0xffffff; addend = (v >> 24) & 0xff
                        n, a2, lib = names[ordinal]
                        self.fix[a] = ('bind', n, addend + a2)
                    else:
                        target = v & 0xfffffffff; high8 = (v >> 36) & 0xff
                        if pfmt == 6: target += self.base
                        self.fix[a] = ('rebase', target | (high8 << 56))
                    if not nxt: break
                    a += nxt * 4

    def pointer(self, a):
        """Contents of a pointer slot: ('rebase', addr) / ('bind', name, addend) / ('raw', value)"""
        f = self.fix.get(a)
        if f: return f
        return ('raw', self.u64(a))

    # ---- relative pointers
    def rel(self, a):
        """direct relative pointer -> address or None"""
        v = self.i32(a)
        return a + v if v else None

    def rel_ind(self, a):
        """relative indirectable pointer -> ('local', addr) | ('extern', symbol) | None"""
        v = self.i32(a)
        if not v: return None
        if v & 1:
            slot = a + (v & ~1)
            p = self.pointer(slot)
            if p[0] == 'bind': return ('extern', p[1])
            if p[0] == 'rebase': return ('local', p[1])
            return ('local', p[1]) if p[1] else None
        return ('local', a + v)

    def ref_name(self, r):
        """symbol name for a resolved reference"""
        if r is None: return None
        if r[0] == 'extern': return r[1]
        return self.name_at(r[1]) or '0x%x' % r[1]

    # ---- mangled names with symbolic references
    def placeholder(self, ref, kind):
        key = (ref, kind)
        k = self._refkey.get(key)
        if k is None:
            k = len(self.refs); self.refs.append(ref); self._refkey[key] = k
        v = -k - 1 if kind == 'P' else (0x1000000 + k if kind == 'Q' else k)
        return '\x01' + struct.pack('<i', v).decode('latin-1')

    def ref_kind(self, ref):
        """kind letter of what a context reference points to"""
        if ref[0] == 'local':
            try:
                k = KIND_NAMES.get(self.u32(ref[1]) & 0x1f)
            except ValueError:
                k = None
            if k == 'opaque': return 'Q'
            return KIND_LETTER.get(k, 'V')
        n = ref[1]
        if n.endswith('Mp'): return 'P'
        if n.endswith('MQ'): return 'Q'
        if n.endswith('CMn') or n.endswith('Mm') or 'OBJC_CLASS' in n: return 'C'
        if n.endswith('OMn'): return 'O'
        return 'V'

    def mangled(self, a):
        """read a mangled name at a, symbolic references replaced by placeholders"""
        out = []
        o = self.off(a)
        b = self.b
        while True:
            c = b[o]
            if c == 0: break
            if 1 <= c <= 0x17:
                v = struct.unpack_from('<i', b, o + 1)[0]
                here = a + (o + 1 - self.off(a))
                tgt = here + v
                if c == 1:
                    ref = ('local', tgt)
                elif c == 2:
                    p = self.pointer(tgt)
                    ref = ('extern', p[1]) if p[0] == 'bind' else ('local', p[1])
                elif c == 0x0c:
                    p = self.pointer(tgt)
                    ref = ('objcproto', p[1] if p[0] == 'bind' else self.name_at(p[1]) or hex(p[1]))
                    out.append(self.placeholder(ref, 'P')); o += 5; continue
                else:
                    ref = ('other%d' % c, tgt)
                kind = self.ref_kind(ref) if c in (1, 2) else 'V'
                out.append(self.placeholder(ref, kind))
                o += 5
                continue
            if 0x18 <= c <= 0x1f:
                v = struct.unpack_from('<Q', b, o + 1)[0]
                out.append(self.placeholder(('abs%d' % c, v), 'V'))
                o += 9
                continue
            out.append(chr(c))
            o += 1
        r = ''.join(out)
        # a few names (protocols of associated type records) are stored with the symbol prefix
        if r.startswith('$s'): r = r[2:]
        return r

    def rel_mangled(self, a):
        t = self.rel(a)
        return self.mangled(t) if t else None


# ---- Swift metadata
class Ctx:
    """a context descriptor"""
    def __init__(self, **kw): self.__dict__.update(kw)
    def __repr__(self): return 'Ctx(%s %s @0x%x)' % (self.kind, getattr(self, 'name', ''), self.addr)


class SwiftMeta:
    def __init__(self, m):
        self.m = m
        self.ctx = {}       # addr -> Ctx
        self.types = []; self.protocols = []; self.conformances = []; self.fields = {}; self.assoc = []
        for seg, addr, size, off in m.section('__swift5_fieldmd'):
            self._fields(addr, size)
        for seg, addr, size, off in m.section('__swift5_types'):
            for a in range(addr, addr + size, 4):
                r = m.rel_ind(a)
                if r and r[0] == 'local': self.types.append(self.context(r[1]))
        for seg, addr, size, off in m.section('__swift5_protos'):
            for a in range(addr, addr + size, 4):
                r = m.rel_ind(a)
                if r and r[0] == 'local': self.protocols.append(self.context(r[1]))
        for seg, addr, size, off in m.section('__swift5_proto'):
            for a in range(addr, addr + size, 4):
                r = m.rel(a)
                if r: self.conformances.append(self.conformance(r))
        for seg, addr, size, off in m.section('__swift5_assocty'):
            self._assoc(addr, size)
        # opaque type descriptors are found through their symbols
        self.opaques = []
        for name, ntype, nsect, value in m.symbols:
            if nsect and name.endswith('MQ') and not (ntype & 0xe0):
                try:
                    if (m.u32(value) & 0x1f) == 4: self.opaques.append(self.context(value))
                except ValueError:
                    pass

    def _fields(self, addr, size):
        m = self.m; a = addr
        while a < addr + size:
            tn = m.rel_mangled(a); sc = m.rel_mangled(a + 4)
            kind, rsize, n = struct.unpack_from('<HHI', m.b, m.off(a + 8))
            recs = []
            for i in range(n):
                r = a + 16 + i * rsize
                flags = m.u32(r)
                ty = m.rel_mangled(r + 4)
                fname = m.cstr(m.rel(r + 8)) if m.i32(r + 8) else ''
                recs.append(dict(flags=flags, type=ty, name=fname, indirect=bool(flags & 1), var=bool(flags & 2)))
            self.fields[a] = dict(type=tn, superclass=sc, kind=kind, records=recs)
            a += 16 + n * rsize

    def _assoc(self, addr, size):
        m = self.m; a = addr
        while a < addr + size:
            conf = m.rel_mangled(a); proto = m.rel_mangled(a + 4)
            n, rsize = struct.unpack_from('<II', m.b, m.off(a + 8))
            recs = []
            for i in range(n):
                r = a + 16 + i * rsize
                recs.append((m.cstr(m.rel(r)), m.rel_mangled(r + 4)))
            self.assoc.append(dict(type=conf, protocol=proto, records=recs))
            a += 16 + n * rsize

    def generic(self, a, typed):
        """generic context at a -> (dict, end address)"""
        m = self.m
        g = {}
        if typed: a += 8
        np_, nreq, nkey, gflags = struct.unpack_from('<4H', m.b, m.off(a))
        a += 8
        params = [m.u8(a + i) for i in range(np_)]
        a += (np_ + 3) & ~3
        reqs = []
        for i in range(nreq):
            reqs.append(self.requirement(a)); a += 12
        g.update(params=params, requirements=reqs, flags=gflags)
        if gflags & 1:   # type packs
            npk, nsh = struct.unpack_from('<HH', m.b, m.off(a)); a += 4
            g['packs'] = [struct.unpack_from('<4H', m.b, m.off(a + 8 * i)) for i in range(npk)]
            a += 8 * npk
        if gflags & 2:   # conditional invertible protocols
            g['inverted'] = m.u16(a); a += 2
            # followed by counts and requirements; align
            a = (a + 3) & ~3
        if gflags & 4:   # values
            nv = m.u32(a); a += 4
            g['values'] = [m.u32(a + 4 * i) for i in range(nv)]; a += 4 * nv
        return g, a

    def requirement(self, a):
        m = self.m
        flags = m.u32(a)
        kind = flags & 0x1f
        param = m.rel_mangled(a + 4)
        r = dict(kind=kind, flags=flags, param=param)
        if kind == 0:     # protocol
            v = m.i32(a + 8)
            if v & 2:
                # Objective-C protocol
                if v & 1:
                    slot = a + 8 + (v & ~3)
                    p = m.pointer(slot)
                    r['objc'] = p[1] if p[0] == 'bind' else (m.name_at(p[1]) or hex(p[1]))
                else:
                    r['objc'] = m.name_at(a + 8 + (v & ~3)) or hex(a + 8 + (v & ~3))
            else:
                if v & 1:
                    slot = a + 8 + (v & ~1); p = m.pointer(slot)
                    r['protocol'] = ('extern', p[1]) if p[0] == 'bind' else ('local', p[1])
                else:
                    r['protocol'] = ('local', a + 8 + v)
        elif kind in (1, 2, 4):   # same type, base class, same shape
            r['type'] = m.rel_mangled(a + 8)
        elif kind == 0x1f:        # layout
            r['layout'] = m.u32(a + 8)
        elif kind == 3:
            r['conformance'] = m.rel_ind(a + 8)
        elif kind == 5:           # inverted protocols
            r['inverted'] = (m.u16(a + 8), m.u16(a + 10))
        return r

    def context(self, a):
        if a in self.ctx: return self.ctx[a]
        m = self.m
        flags = m.u32(a)
        kind = KIND_NAMES.get(flags & 0x1f, 'kind%d' % (flags & 0x1f))
        c = Ctx(addr=a, flags=flags, kind=kind, generic_flag=bool(flags & 0x80), ksf=flags >> 16)
        self.ctx[a] = c
        pr = m.rel_ind(a + 4)
        c.parent_ref = pr
        c.parent = self.context(pr[1]) if pr and pr[0] == 'local' else (pr[1] if pr else None)
        c.symbol = m.name_at(a)
        if kind == 'module':
            c.name = m.cstr(m.rel(a + 8))
        elif kind == 'extension':
            c.extended = m.rel_mangled(a + 8)
            if c.generic_flag: c.generic, _ = self.generic(a + 12, False)
        elif kind == 'anonymous':
            e = a + 8
            if c.generic_flag: c.generic, e = self.generic(e, False)
            if c.ksf & 1: c.mangled = m.rel_mangled(e)
        elif kind == 'protocol':
            c.name = m.cstr(m.rel(a + 8))
            nsig, nreq = m.u32(a + 12), m.u32(a + 16)
            at = m.rel(a + 20)
            c.assoc_names = m.cstr(at).split() if at else []
            e = a + 24
            c.signature = []
            for i in range(nsig):
                c.signature.append(self.requirement(e)); e += 12
            c.requirements = []
            for i in range(nreq):
                rf = m.u32(e)
                c.requirements.append(dict(addr=e, flags=rf, kind=rf & 0xf, instance=bool(rf & 0x10), isasync=bool(rf & 0x20),
                                           default=m.rel(e + 4), symbol=m.name_at(e)))
                e += 8
        elif kind == 'opaque':
            e = a + 8
            if c.generic_flag: c.generic, e = self.generic(e, False)
            n = c.ksf
            c.underlying = [m.rel_mangled(e + 4 * i) for i in range(n)]
        elif kind in ('struct', 'enum', 'class'):
            c.name = m.cstr(m.rel(a + 8))
            c.accessor = m.rel(a + 12)
            fd = m.rel(a + 16)
            c.fields = self.fields.get(fd) if fd else None
            if kind == 'struct':
                c.num_fields, c.fov = m.u32(a + 20), m.u32(a + 24); e = a + 28
            elif kind == 'enum':
                pc, c.num_empty = m.u32(a + 20), m.u32(a + 24); c.num_payload = pc & 0xffffff; e = a + 28
            else:
                c.superclass = m.rel_mangled(a + 20) if m.i32(a + 20) else None
                c.neg, c.pos, c.num_members, c.num_fields, c.fov = struct.unpack_from('<5I', m.b, m.off(a + 24))
                e = a + 44
            if c.generic_flag: c.generic, e = self.generic(e, True)
            if kind == 'class':
                ksf = c.ksf
                if ksf & (1 << 13):
                    c.resilient_super = m.rel_ind(e); e += 4
                mi = ksf & 3
                if mi == 1: e += 12
                elif mi == 2: e += 4
                c.vtable = []
                if ksf & (1 << 15):
                    voff, vsize = m.u32(e), m.u32(e + 4); e += 8
                    c.vtable_offset = voff
                    for i in range(vsize):
                        mf = m.u32(e); impl = m.rel(e + 4)
                        c.vtable.append(dict(addr=e, flags=mf, kind=mf & 0xf, instance=bool(mf & 0x10), dynamic=bool(mf & 0x20),
                                             isasync=bool(mf & 0x40), impl=impl, impl_name=m.name_at(impl) if impl else None,
                                             symbol=m.name_at(e)))
                        e += 8
                c.overrides = []
                if ksf & (1 << 14):
                    n = m.u32(e); e += 4
                    for i in range(n):
                        cls = m.rel_ind(e); meth = m.rel_ind(e + 4); impl = m.rel(e + 8)
                        c.overrides.append(dict(cls=m.ref_name(cls), method=m.ref_name(meth),
                                                impl=impl, impl_name=m.name_at(impl) if impl else None))
                        e += 12
        return c

    def conformance(self, a):
        m = self.m
        proto = m.rel_ind(a)
        tflags = m.u32(a + 12)
        tk = (tflags >> 3) & 7
        if tk == 0:
            t = m.rel(a + 4); ty = ('ctx', self.context(t)) if t else None
        elif tk == 1:
            r = m.rel_ind(a + 4) if False else None
            slot = a + 4 + m.i32(a + 4); p = m.pointer(slot)
            ty = ('extern', p[1]) if p[0] == 'bind' else ('ctx', self.context(p[1]))
        elif tk == 2:
            ty = ('objc', m.cstr(m.rel(a + 4)))
        else:
            slot = a + 4 + m.i32(a + 4); p = m.pointer(slot)
            ty = ('objcref', p[1])
        wt = m.rel(a + 8)
        c = dict(addr=a, symbol=m.name_at(a), protocol=proto, protocol_name=m.ref_name(proto), type=ty, flags=tflags,
                 witness_table=wt, witness_name=m.name_at(wt) if wt else None)
        e = a + 16
        if tflags & 0x40:  # retroactive
            c['retro'] = m.rel_ind(e); e += 4
        ncond = (tflags >> 8) & 0xff
        c['conditions'] = []
        for i in range(ncond):
            c['conditions'].append(self.requirement(e)); e += 12
        if tflags & 0x10000:  # resilient witnesses
            n = m.u32(e); e += 4
            ws = []
            for i in range(n):
                req = m.rel_ind(e); impl = m.rel(e + 4)
                ws.append((m.ref_name(req), m.name_at(impl) if impl else None))
                e += 8
            c['witnesses'] = ws
        return c
