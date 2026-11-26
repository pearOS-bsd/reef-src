#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Generate the Swift skeleton of Reef's SwiftUI from the published binary.

usage: gen-skeleton.py BINARY OUTDIR [--cache DIR] [--fixes FILE]

Every type (with its stored fields, generic signature and superclass), every conformance (with its associated
types), every protocol (with its requirements) and every function, accessor, initializer and subscript of the
symbol table is written as a Swift declaration with a stub body.  The input is the Mach-O file itself (symbols
and Swift metadata, read by machometa.py) and the demangler's trees of the symbol names and of the metadata's
mangled names; the demangler runs on the Mac VM (`swift demangle --tree-only`), its output is cached.

Files are grouped by area (areas.py); FIXES (fixes.json) holds what the compiler told us on earlier runs
(Objective-C names renamed in Swift, members that need `override`, ...)."""
import json, os, re, subprocess, sys, pickle, collections, hashlib

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import machometa
import swifttree
from swifttree import N, gp_name
import areas

MAC = os.environ.get('MAC_SSH', 'ssh -p 2229 -i %s -o IdentitiesOnly=yes -o BatchMode=yes -o StrictHostKeyChecking=no alex@localhost'
                     % os.path.expanduser('~/fbsd-vm/id_ed25519'))

HEADER = '''// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// %s
'''

KEYWORDS = set('''associatedtype class deinit enum extension fileprivate func import init inout internal let open operator private
precedencegroup protocol public rethrows static struct subscript typealias var break case catch continue default defer do else
fallthrough for guard if in repeat return throw switch where while Any as await false is nil self Self super throws true try
_ Type Protocol some any'''.split())
KEYWORDS_MEMBER = set('''associatedtype class deinit enum extension fileprivate func import init inout internal let open operator private
precedencegroup protocol public rethrows static struct subscript typealias var break case catch continue default defer do else
fallthrough for guard if in repeat return throw switch where while as false is nil super throws true try'''.split())


def esc(name, member=False):
    kw = KEYWORDS_MEMBER if member else KEYWORDS
    if name in kw or name in ('Type', 'self', 'Self') and not member:
        return '`%s`' % name
    if member and name in ('init', 'self', 'Type', 'Protocol', 'default', 'subscript', 'deinit'):
        return '`%s`' % name
    return name


# ------------------------------------------------------------------------------------------------ demangling
def mac(cmd, data=b''):
    return subprocess.run(MAC + " '" + cmd + "'", shell=True, input=data, capture_output=True).stdout


def demangle(symbols, types, cache_path):
    """trees of symbol names (swift demangle) and of metadata type names (demangle-refs on the Mac)"""
    cache = {}
    if os.path.exists(cache_path):
        cache = pickle.load(open(cache_path, 'rb'))
    todo = sorted(set(s for s in symbols if ('s', s) not in cache))
    if todo:
        print('demangling %d symbols on the Mac' % len(todo), file=sys.stderr)
        out = mac('swift demangle --tree-only', ('\n'.join(todo) + '\n').encode()).decode()
        trees = swifttree.parse(out.splitlines())
        for s in todo: cache[('s', s)] = trees.get(s, [])
    todo = sorted(set(s for s in types if ('t', s) not in cache))
    if todo:
        print('demangling %d metadata names on the Mac' % len(todo), file=sys.stderr)
        src = open(os.path.join(HERE, 'demangle-refs.cpp'), 'rb').read()
        mac('mkdir -p ~/suire/tools && cat > ~/suire/tools/demangle-refs.cpp && cd ~/suire/tools && clang++ -std=c++17 -O1 '
            'demangle-refs.cpp -L/Library/Developer/CommandLineTools/usr/lib -lswiftDemangle '
            '-Wl,-rpath,/Library/Developer/CommandLineTools/usr/lib -o demangle-refs', src)
        hexes = [s.encode('latin-1').hex() for s in todo]
        out = mac('~/suire/tools/demangle-refs', ('\n'.join(hexes) + '\n').encode()).decode()
        trees = swifttree.parse(out.splitlines())
        for s, h in zip(todo, hexes): cache[('t', s)] = trees.get(h, [])
    pickle.dump(cache, open(cache_path, 'wb'))
    return cache


# ------------------------------------------------------------------------------------------------ model
class Gen:
    def __init__(self, binary, out, cache, fixes):
        self.out = out
        self.fixes = fixes
        self.m = m = machometa.MachO(binary)
        self.meta = meta = machometa.SwiftMeta(m)
        os.makedirs(cache, exist_ok=True)
        # every string we need trees for
        names = set()
        for name, ntype, nsect, value in m.symbols:
            if name.startswith('_$s') or name.startswith('$s'): names.add(name)
        ms = set()
        def addm(s):
            if s: ms.add(s)
        for f in meta.fields.values():
            addm(f['type']); addm(f['superclass'])
            for r in f['records']: addm(r['type'])
        def addg(g):
            if not g: return
            for r in g['requirements']:
                addm(r['param']); addm(r.get('type'))
        for c in meta.ctx.values():
            addg(getattr(c, 'generic', None))
            if c.kind == 'extension': addm(c.extended)
            if c.kind == 'anonymous': addm(getattr(c, 'mangled', None))
            if c.kind == 'class': addm(c.superclass)
            if c.kind == 'protocol':
                for r in c.signature: addm(r['param']); addm(r.get('type'))
            if c.kind == 'opaque':
                for u in c.underlying[:1]: addm(u)
        for cf in meta.conformances:
            for r in cf['conditions']: addm(r['param']); addm(r.get('type'))
        for a in meta.assoc:
            addm(a['type']); addm(a['protocol'])
            for n, t in a['records']: addm(t)
        for r in m.refs:
            if r[0] == 'extern' and r[1].startswith('_$s'): names.add(r[1])
        self.mangled_strings = ms
        self.trees = demangle(names, ms, os.path.join(cache, 'trees.pkl'))
        self.renames = fixes.get('renames', {})

    # --- trees
    def tree(self, sym):
        t = self.trees.get(('s', sym))
        return t[0] if t else None

    def mtype(self, s):
        """tree (Type node) of a metadata mangled name"""
        if not s: return None
        t = self.trees.get(('t', s))
        return t[0] if t else None


def main():
    args = sys.argv[1:]
    cache = os.path.join(HERE, '.cache')
    fixes = os.path.join(HERE, 'fixes.json')
    if '--cache' in args:
        i = args.index('--cache'); cache = args[i + 1]; del args[i:i + 2]
    if '--fixes' in args:
        i = args.index('--fixes'); fixes = args[i + 1]; del args[i:i + 2]
    binary, out = args
    fx = json.load(open(fixes)) if os.path.exists(fixes) else {}
    import emit
    g = Gen(binary, out, cache, fx)
    emit.Emitter(g).run()


if __name__ == '__main__':
    main()
