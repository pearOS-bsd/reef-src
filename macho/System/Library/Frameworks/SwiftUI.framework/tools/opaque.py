#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""The underlying type of each opaque result (`some View` ...) of Reef's SwiftUI, from its opaque type descriptors.

usage: opaque.py [REGEX]      functions whose mangled name matches REGEX: symbol, then the underlying type
Uses the generator's demangle cache (tools/.cache, made by regen.sh); generic parameters print as A, B ... (depth 0)."""
import importlib.util, json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
spec = importlib.util.spec_from_file_location('gen', os.path.join(HERE, 'gen-skeleton.py'))
gen = importlib.util.module_from_spec(spec); spec.loader.exec_module(gen)
import emit, disasm


def main():
    rx = re.compile(sys.argv[1] if len(sys.argv) > 1 else '.')
    fx = json.load(open(os.path.join(HERE, 'fixes.json')))
    g = gen.Gen(disasm.BIN, '/tmp/opaque-unused', os.path.join(HERE, '.cache'), fx)
    e = emit.Emitter(g)
    names = {v: n for n, t, s, v in g.m.symbols if s}
    for c in g.meta.ctx.values():
        if c.kind != 'opaque' or not c.underlying: continue
        sym = names.get(c.addr, '')
        if not rx.search(sym): continue
        t = g.mtype(c.underlying[0])
        try:
            text = e.ty(t, emit.Env()) if t is not None else '?'
        except Exception as x:
            text = '? (%s)' % x
        print(sym.replace('QOMQ', ''))
        print('    ' + text)


if __name__ == '__main__':
    main()
