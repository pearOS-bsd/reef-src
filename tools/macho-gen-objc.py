#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""First draft of the Objective-C source of a Mach-O file, written from its metadata and the shape of its method bodies.
usage: macho-gen-objc.py ORIGINAL FOLDER INSTALL_PATH [--report]     (writes FOLDER/<name>.m and FOLDER/target.conf)
Classes (superclass, protocols, ivars, properties, methods), categories and string constants come from the ObjC metadata (llvm-objdump
--macho --objc-meta-data).  A method whose body is one of the plain shapes (empty, constant, instance variable read or written,
self, a string) gets its code; accessors of a declared property are left to the compiler (@property); every other body is a stub
that returns a zero value and is listed as `unresolved` for a person to write.  The output is a starting point: tools/compare-macho.py
says what is left."""
import sys, os, re, subprocess, struct, importlib.util, collections
here = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('cm', os.path.join(here, 'compare-macho.py'))
cm = importlib.util.module_from_spec(spec); spec.loader.exec_module(cm)
import capstone

def meta_text(path):
    return subprocess.run(['/usr/lib/llvm-19/bin/llvm-objdump', '--macho', '--objc-meta-data', path], capture_output=True, text=True).stdout.splitlines()

class Cls:
    def __init__(self, name): self.name = name; self.super = None; self.protocols = []; self.ivars = []; self.props = []; self.cprops = []; self.methods = []; self.cmethods = []; self.start = 8; self.size = 8; self.flags = 0

def parse(lines):
    classes, cats = [], []
    i = 0; n = len(lines)
    cur = None; meta = False; section = None
    while i < n:
        l = lines[i]
        m = re.match(r'^Contents of \((\S+)\) section', l)
        if m: section = m.group(1); i += 1; continue
        m = re.match(r'^[0-9a-f]{16} 0x[0-9a-f]+ _OBJC_CLASS_\$_(\S+)$', l)
        if m and section and 'classlist' in section:
            cur = Cls(m.group(1)); classes.append(cur); meta = False
            # an optional pointer to the superclass follows as `superclass 0x... name`
        elif l.strip() == 'Meta Class': meta = True
        elif cur is not None and l.startswith('    superclass') and not meta:
            mm = re.match(r'\s+superclass 0x[0-9a-f]+ _OBJC_CLASS_\$_(\S+)', l)
            cur.super = mm.group(1) if mm else None
        elif cur is not None and 'instanceStart' in l and not meta: cur.start = int(l.split()[1])
        elif cur is not None and 'instanceSize' in l and not meta: cur.size = int(l.split()[1])
        elif cur is not None and l.strip().startswith('flags 0x') and not meta: cur.flags = int(l.split()[1], 16)
        elif cur is not None and l.strip().startswith('baseMethods 0x') and not l.strip().startswith('baseMethods 0x0 '):
            j = i + 1; count = 0
            while j < n and 'count' not in lines[j]: j += 1
            count = int(lines[j].split()[-1]); j += 1
            for _ in range(count):
                # name, types, imp lines
                sel = lines[j].split(None, 2)[2]; typ = lines[j + 1].split(None, 2)[2]; imp = lines[j + 2].strip()[4:]
                (cur.cmethods if meta else cur.methods).append((sel, typ, imp)); j += 3
            i = j; continue
        elif cur is not None and not meta and l.strip().startswith('baseProtocols 0x') and not l.strip().startswith('baseProtocols 0x0'):
            j = i + 1; count = int(lines[j].split()[-1]); j += 1
            names = []
            depth_marker = lines[j][:len(lines[j]) - len(lines[j].lstrip())]
            while j < n and not lines[j].strip().startswith('ivars'):
                s = lines[j]
                if re.match(r'^\t+\s*list\[\d+\]', s): pass
                mm = re.match(r'^\t+\s+name 0x[0-9a-f]+ (\S+)$', s)
                if mm and lines[j - 2].strip().startswith('isa') or (mm and 'isa' in lines[j - 1]): names.append(mm.group(1))
                j += 1
            cur.protocols = names; i = j; continue
        elif cur is not None and not meta and l.strip().startswith('ivars 0x') and not l.strip().startswith('ivars 0x0'):
            j = i + 1
            while j < n and 'count' not in lines[j]: j += 1
            count = int(lines[j].split()[-1]); j += 1
            for _ in range(count):
                off = int(lines[j].split()[-1]); name = lines[j + 1].split()[-1]; typ = lines[j + 2].split(None, 2)[2]; size = int(lines[j + 4].split()[-1])
                cur.ivars.append((off, name, typ, size)); j += 5
            i = j; continue
        elif cur is not None and l.strip().startswith('baseProperties 0x') and not l.strip().startswith('baseProperties 0x0'):
            j = i + 1
            while j < n and 'count' not in lines[j]: j += 1
            count = int(lines[j].split()[-1]); j += 1
            for _ in range(count):
                name = lines[j].split(None, 2)[2]; attrs = lines[j + 1].split(None, 2)[2]; (cur.cprops if meta else cur.props).append((name, attrs)); j += 2
            i = j; continue
        i += 1
    return classes

def split_types(enc):
    """['v', '@', ':', '@'], offsets stripped: the return type, then each argument."""
    out = []; i = 0
    def one(i):
        c = enc[i]
        if c in 'rnNoORV': return one(i + 1)
        if c == '^': return one(i + 1)
        if c == '@':
            if i + 1 < len(enc) and enc[i + 1] == '?': return i + 2
            if i + 1 < len(enc) and enc[i + 1] == '"':
                return enc.index('"', i + 2) + 1
            return i + 1
        if c in '{(':
            close = '}' if c == '{' else ')'; depth = 0; j = i
            while True:
                if enc[j] in '{(': depth += 1
                elif enc[j] in '})': depth -= 1
                j += 1
                if depth == 0: return j
        if c == '[':
            return enc.index(']', i) + 1
        if c == 'b':
            j = i + 1
            while enc[j].isdigit(): j += 1
            return j
        return i + 1
    while i < len(enc):
        j = one(i)
        out.append(enc[i:j])
        i = j
        while i < len(enc) and enc[i].isdigit(): i += 1
    return out

STRUCTS = {'CGSize': 'CGSize', 'CGPoint': 'CGPoint', 'CGRect': 'CGRect', '_NSRange': 'NSRange', 'NSRange': 'NSRange', 'CGAffineTransform': 'CGAffineTransform',
           'CGVector': 'CGVector', '_NSZone': 'struct _NSZone', 'CGImage': 'struct CGImage', 'CGColor': 'struct CGColor', 'CGPath': 'struct CGPath'}
extra_structs = {}
def ctype(e, hint=None):
    q = ''
    while e and e[0] in 'rnNoORV': q += {'r': 'const '}.get(e[0], ''); e = e[1:]
    c = e[0]
    if c == 'v': return 'void'
    if c == '@':
        if e.startswith('@?'): return 'void (^)(void)'
        if e.startswith('@"'): return (e[2:-1] + ' *') if '<' not in e else 'id'
        return 'id'
    if c == '#': return 'Class'
    if c == ':': return 'SEL'
    if c == 'c': return 'BOOL'
    if c == 'C': return 'unsigned char'
    if c == 'i': return 'int'
    if c == 'I': return 'unsigned int'
    if c == 's': return 'short'
    if c == 'S': return 'unsigned short'
    if c == 'l': return 'int'
    if c == 'L': return 'unsigned int'
    if c == 'q': return 'long'
    if c == 'Q': return 'unsigned long'
    if c == 'f': return 'float'
    if c == 'd': return 'double'
    if c == 'B': return 'bool'
    if c == '*': return 'char *'
    if c == '^':
        inner = e[1:]
        if inner[0] == 'v': return 'void *'
        if inner[0] == '{':
            name = inner[1:].split('=')[0]
            return STRUCTS.get(name, 'struct ' + name) + ' *' if not STRUCTS.get(name, '').startswith('struct') else STRUCTS.get(name) + ' *'
        return ctype(inner) + ' *'
    if c == '{':
        name = e[1:].split('=')[0]
        if name in STRUCTS: return STRUCTS[name]
        body = e[len(name) + 2:-1]
        extra_structs[name] = body
        return 'struct ' + name
    return 'void *'

def selparts(sel, enc):
    t = split_types(enc)
    ret = ctype(t[0]); args = t[3:]
    if ':' not in sel: return '(%s)%s' % (ret, sel)
    parts = sel.split(':')[:-1]
    return ' '.join('%s:(%s)a%d' % (p, ctype(a), k) for k, (p, a) in enumerate(zip(parts, args)))
def decl(sel, enc, plus):
    t = split_types(enc); ret = ctype(t[0])
    if ':' not in sel: return '%s (%s)%s' % ('+' if plus else '-', ret, sel)
    return '%s (%s)%s' % ('+' if plus else '-', ret, selparts(sel, enc))

def property_decl(name, attrs, ivars):
    parts = attrs.split(','); t = parts[0][1:]; opts = []; getter = setter = None; ivar = None
    ro = False
    for p in parts[1:]:
        if p == 'R': opts.append('readonly'); ro = True
        elif p == 'C': opts.append('copy')
        elif p == '&': opts.append('strong')
        elif p == 'W': opts.append('weak')
        elif p == 'N': opts.append('nonatomic')
        elif p.startswith('G'): getter = p[1:]
        elif p.startswith('S'): setter = p[1:]
        elif p.startswith('V'): ivar = p[1:]
        elif p == 'D': opts.append('@dynamic')
    if getter: opts.append('getter=' + getter)
    if setter: opts.append('setter=' + setter)
    ty = ctype(t)
    dyn = '@dynamic' in opts
    opts = [o for o in opts if o != '@dynamic']
    return '@property (%s) %s %s;' % (', '.join(opts), ty, name) if opts else '@property %s %s;' % (ty, name), ivar, dyn, getter, setter, ro

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64); md.detail = False
def body_shape(I, addr):
    n = I.funcs[addr]; code = I.data(addr, n)
    ins = [(i.mnemonic, i.op_str, bytes(i.bytes)) for i in md.disasm(code, addr)]
    ins = [i for i in ins if i[0] not in ('nop', 'int3') and not i[0].startswith('nop')]
    if len(ins) >= 3 and ins[0][:2] == ('push', 'rbp') and ins[1][:2] == ('mov', 'rbp, rsp'): ins = ins[2:]
    else: return None
    if ins and ins[-1][0] == 'ret' and len(ins) >= 2 and ins[-2][:2] == ('pop', 'rbp'): ins = ins[:-2] + [('ret', '', b'')]
    elif ins and ins[-1][0] == 'jmp' and len(ins) >= 2 and ins[-2][:2] == ('pop', 'rbp'): ins = ins[:-2] + [ins[-1]]
    else: return None
    return ins

def classify(I, addr, sel, enc, cls, ivar_by_off, props_accessors):
    ins = body_shape(I, addr)
    if ins is None: return None
    t = split_types(enc); rt = t[0].lstrip('rnNoORV'); args = t[3:]
    if ins == [('ret', '', b'')]:
        return '' if rt == 'v' else None
    if len(ins) == 2 and ins[0][:2] == ('xor', 'eax, eax') and ins[1][0] == 'ret' and rt[0] in '@#cCiIsSlLqQB*^:':
        return 'return %s;' % ('nil' if rt[0] in '@#' else 'NO' if rt[0] == 'c' else '0')
    if len(ins) == 2 and ins[0][0] == 'mov' and ins[0][1] == 'eax, 1' and ins[1][0] == 'ret' and rt[0] in 'cCiIsSlLqQB':
        return 'return %s;' % ('YES' if rt[0] == 'c' else '1')
    if len(ins) == 2 and ins[0][0] == 'mov' and ins[0][1] == 'rax, rdi' and ins[1][0] == 'ret' and rt[0] == '@': return 'return self;'
    if len(ins) == 1 and ins[0][0] == 'jmp' and rt == '@' and len(args) == 1:
        pass
    m = re.match(r'^(?:mov|movsx|movzx|movsxd|movsbl|movzbl|movswl|movzwl|movslq) (?:r|e)?ax, (?:\w+ ptr )?\[rdi \+ (0x[0-9a-f]+)\]$', ins[0][0] + ' ' + ins[0][1]) if len(ins) == 2 and ins[1][0] == 'ret' else None
    if m:
        off = int(m.group(1), 16)
        if off in ivar_by_off: return 'return %s;' % ivar_by_off[off][0]
    m = re.match(r'^mov (?:qword ptr |dword ptr |byte ptr |word ptr )?\[rdi \+ (0x[0-9a-f]+)\], (rdx|edx|dl|dx)$', ins[0][0] + ' ' + ins[0][1]) if len(ins) == 2 and ins[1][0] == 'ret' else None
    if m and len(args) == 1:
        off = int(m.group(1), 16)
        if off in ivar_by_off: return '%s = a0;' % ivar_by_off[off][0]
    return None

def cfstrings(I):
    out = {}
    s = [s for s in I.sections if s['name'] == '__cfstring']
    if not s: return out
    s = s[0]
    for k in range(s['size'] // 32):
        a = s['addr'] + 32 * k
        d = I.data(a, 32)
        ptr, = struct.unpack_from('<Q', d, 16); length, = struct.unpack_from('<Q', d, 24)
        sec = I.section(ptr)
        out[a] = I.b[sec['off'] + ptr - sec['addr']: sec['off'] + ptr - sec['addr'] + length].decode('utf-8', 'replace')
    return out

HEADER = '''// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// %s
'''

def main():
    path, folder, install = sys.argv[1], sys.argv[2], sys.argv[3]
    os.makedirs(folder, exist_ok=True)
    base = os.path.basename(install).split('.')[0]
    out = os.path.join(folder, base + '.m')
    I = cm.Image(path)
    classes = parse(meta_text(path))
    defined = {c.name for c in classes}
    text = []; unresolved = []
    foreign = collections.OrderedDict()
    for c in classes:
        if c.super and c.super not in defined and c.super not in foreign and c.super != 'NSObject': foreign[c.super] = c.start
    mapping_count = collections.Counter()
    for c in classes:
        ivar_by_off = {o: (n, t, s) for o, n, t, s in c.ivars}
        props = {}
        accessors = {}
        decls = []
        caccessors = {}
        for name, attrs in c.props:
            d, ivar, dyn, getter, setter, ro = property_decl(name, attrs, c.ivars)
            decls.append(d)
            accessors[getter or name] = ('get', name)
            if not ro: accessors[setter or 'set%s%s:' % (name[0].upper(), name[1:])] = ('set', name)
        for name, attrs in c.cprops:
            d, ivar, dyn, getter, setter, ro = property_decl(name, attrs, c.ivars)
            decls.append(d.replace('@property (', '@property (class, ', 1) if d.startswith('@property (') else d.replace('@property ', '@property (class) ', 1))
            caccessors[getter or name] = ('get', name)
            if not ro: caccessors[setter or 'set%s%s:' % (name[0].upper(), name[1:])] = ('set', name)
        ivars = '\n'.join('    %s %s;' % (ctype(t.replace('"', '') if t[0] != '@' else t), n) if '"' not in t or t[0] == '@' else '    %s %s;' % (ctype(re.sub(r'"[^"]*"', '', t)), n) for o, n, t, s in c.ivars)
        sup = c.super or 'NSObject'
        head = '@interface %s : %s%s\n' % (c.name, sup, (' <%s>' % ', '.join(c.protocols)) if c.protocols else '')
        if c.ivars: head += '{\n%s\n}\n' % ivars
        meths = []
        for plus, lst in ((True, c.cmethods), (False, c.methods)):
            for sel, typ, imp in lst:
                if not plus and sel in accessors: continue
                meths.append((plus, sel, typ, imp))
        body = '\n'.join(decls) + ('\n' if decls else '')
        body += '\n'.join(decl(s, t, p) + ';' for p, s, t, i in meths)
        text.append(head + body + '\n@end\n')
        impl = '@implementation %s\n' % c.name
        for plus, sel, typ, imp in meths:
            addr = I.faddr.get(imp) or I.faddr.get(imp + '#2')
            code = classify(I, addr, sel, typ, c, ivar_by_off, accessors) if addr else None
            if code is None:
                t = split_types(typ); rt = t[0]
                zero = '' if rt[0] == 'v' else 'return %s;' % ('nil' if rt[0] in '@#' else 'NO' if rt[0] == 'c' else '0' if rt[0] not in '{' else '(%s){0}' % ctype(rt))
                impl += '%s\n{\n    // unresolved\n%s\n}\n\n' % (decl(sel, typ, plus), ('    ' + zero) if zero else '')
                unresolved.append((c.name, sel))
                mapping_count['unresolved'] += 1
            else:
                impl += '%s\n{\n%s\n}\n\n' % (decl(sel, typ, plus), ('    ' + code) if code else '')
                mapping_count['written'] += 1
        text.append(impl + '@end\n')
    # strings
    cf = cfstrings(I)
    consts = []
    for a, name, ext, sect in sorted(I.syms):
        if not ext or name.startswith('_OBJC_') or a in I.fname: continue
        s = I.section(a)
        if s and s['name'] == '__const' and s['seg'] == '__DATA_CONST':
            p = I.pointer(a)
            if p in cf: consts.append('NSString *const %s = @"%s";' % (name.lstrip('_'), cf[p].replace('\\', '\\\\').replace('"', '\\"')))
    pre = '#import <Foundation/Foundation.h>\n\n'
    for name, start in foreign.items(): pre += '@interface %s : NSObject%s\n@end\n\n' % (name, ('\n{\n    char _padding[%d];\n}' % (start - 8)) if start > 8 else '')
    structs = ''.join('struct %s { %s };\n' % (n, ' '.join('%s f%d;' % (ctype(e), k) for k, e in enumerate(split_types(b.replace('"', '')) ))) for n, b in extra_structs.items())
    open(out, 'w').write(HEADER % ('%s.' % os.path.basename(path)) + '\n' + pre + structs + '\n' + '\n'.join(text) + '\n' + '\n'.join(consts) + '\n')
    sys.stderr.write('%d classes, %d methods written, %d unresolved\n' % (len(classes), mapping_count['written'], mapping_count['unresolved']))
    def ver(v): return '%d.%d.%d' % (v >> 16, (v >> 8) & 255, v & 255)
    libs = [('reexport:' if cmd == cm.LC_REEXPORT else '') + n.lstrip('/') for cmd, n, cur, comp in I.dylibs]
    conf = 'install = %s\n' % install
    if I.id: conf += 'id = %s\nversion = %s\ncompat = %s\n' % (I.id[1], ver(I.id[2]), ver(I.id[3]))
    conf += 'sources = %s.m\nlibs = %s\nminos = %s\ncflags = -fobjc-arc\n' % (base, ' '.join(libs), '.'.join(ver(I.build[2]).split('.')[:2]))
    open(os.path.join(folder, 'target.conf'), 'w').write(conf)
    if '--report' in sys.argv:
        for c, s in unresolved: print(c, s)

if __name__ == '__main__': main()
