#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Build every rebuilt Mach-O file, compare it with the original and run its behaviour tests.
usage: check-macho.py REEF ORIGINAL_ROOT OUT_DIR [FOLDER...]      (in the reef-build image)
Per file: functions and bytes that match (tools/compare-macho.py), whether the file is byte for byte the original, and the tests
(tests/macho.py).  target.conf may set  minimum = percent of function bytes that must match (default 100).  Exit status 1 if one is below."""
import os, sys, subprocess, importlib.util
here = os.path.dirname(os.path.abspath(__file__))
def load(name, file):
    spec = importlib.util.spec_from_file_location(name, os.path.join(here, file)); m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m); return m
bm, cm = load('bm', 'build-macho.py'), load('cm', 'compare-macho.py')
reef, root, out = sys.argv[1], os.path.abspath(sys.argv[2]), os.path.abspath(sys.argv[3])
folders = sys.argv[4:] or [l.split('#')[0].strip() for l in open(os.path.join(bm.SRC, 'targets.txt')) if l.split('#')[0].strip()]
bad = 0
rebuilt = {}
for f in folders:
    conf = bm.conf(f)
    bm.build(root, out, f)
    A, B = cm.Image(os.path.join(root, conf['install'])), cm.Image(os.path.join(out, conf['install']))
    found, same = cm.compare(A, B)
    total = sum(A.funcs.values()); ok = sum(A.funcs[x] for x in same)
    pct = 100 * ok / max(total, 1) if total else 100.0
    identical = A.b == B.b
    t = subprocess.run([sys.executable, os.path.join(here, '..', 'tests', 'macho.py'), reef, root, out, f], capture_output=True, text=True)
    tests = t.stdout.strip().splitlines()[-1] if t.stdout.strip() else 'no output'
    need = float(conf.get('minimum', 100))
    flag = 'ok' if pct >= need and t.returncode == 0 else 'FAIL'
    if flag == 'FAIL':
        bad += 1; sys.stdout.write(t.stdout + t.stderr[-500:])
    rebuilt[conf['install']] = (len(same), len(A.funcs), ok, total)
    print('%-4s %-45s %d/%d functions, %.2f%%, %s, %s' % (flag, f, len(same), len(A.funcs), pct, 'identical' if identical else 'not identical', tests))
# The total over every Mach-O file of the original package: a file that is not rebuilt counts with all its functions and bytes and no match.
if not sys.argv[4:]:
    files = fn = fb = same_fn = same_b = 0
    for d, _, names in os.walk(root):
        for n in names:
            p = os.path.join(d, n)
            if os.path.islink(p) or not os.path.isfile(p): continue
            with open(p, 'rb') as h:
                if h.read(4) != b'\xcf\xfa\xed\xfe': continue
            rel = os.path.relpath(p, root)
            files += 1
            if rel in rebuilt:
                a, b, c, e = rebuilt[rel]
            else:
                I = cm.Image(p); a, b, c, e = 0, len(I.funcs), 0, sum(I.funcs.values())
            fn += b; fb += e; same_fn += a; same_b += c
    print('macho: %d/%d files rebuilt, %d/%d functions, %d/%d bytes = %.2f%%' % (len(rebuilt), files, same_fn, fn, same_b, fb, 100 * same_b / max(fb, 1)))
sys.exit(1 if bad else 0)
