#!/usr/bin/env python3 -I
"""Differential SwiftUI test runner for Reef.

usage: run.py [--swiftui CANDIDATE] [--apps a,b,c] [--runs DIR] [--container NAME] [--timeout S]
              [--rerun-orig] [--apps-dir DIR] [--table]

Runs every built app (see build.sh) inside the Reef test container (headless sway + grim), once with the original
SwiftUI twice (to find nondeterminism) and, with --swiftui, once with the candidate file. Prints per app 'same' or
the differences. Without --swiftui it prints the baseline (what the original does).
Results are kept in DIR/orig1, DIR/orig2, DIR/cand per app; original runs are reused unless --rerun-orig.
"""
import argparse, glob, json, os, re, subprocess, sys, shutil

HOME = os.path.expanduser('~')
FW = '/usr/lib/cutefish-reef/System/Library/Frameworks/SwiftUI.framework/Versions/A/SwiftUI'
HERE = os.path.dirname(os.path.abspath(__file__))

ap = argparse.ArgumentParser()
ap.add_argument('--swiftui')
ap.add_argument('--apps')
ap.add_argument('--runs', default=HOME + '/.pear/tmp/tk/swiftui/runs')
ap.add_argument('--apps-dir', default=HOME + '/.pear/tmp/tk/swiftui/apps')
ap.add_argument('--container', default='reef-test-orig')
ap.add_argument('--timeout', type=int, default=40)
ap.add_argument('--rerun-orig', action='store_true')
ap.add_argument('--table', action='store_true', help='print the baseline as a markdown table')
a = ap.parse_args()

C = a.container
def sh(*cmd, check=True):
    return subprocess.run(cmd, check=check, capture_output=True, text=True)
def dx(script, check=False):
    return sh('docker', 'exec', C, 'sh', '-c', script, check=check)

names = sorted(os.path.basename(p)[:-4] for p in glob.glob(a.apps_dir + '/*.app'))
if a.apps:
    names = [n for n in a.apps.split(',') if n in names]
if not names:
    sys.exit('no built apps in ' + a.apps_dir + ' (run build.sh)')

# container setup: scripts, apps, original kept as SwiftUI.keep
dx('mkdir -p /tmp/sui/apps /tmp/sui/out', True)
sh('docker', 'cp', HERE + '/inside.sh', C + ':/tmp/sui/inside.sh')
dx('[ -e %s.keep ] || cp -a %s %s.keep' % (FW, FW, FW), True)
for n in names:
    sh('docker', 'exec', C, 'rm', '-rf', '/tmp/sui/apps/%s.app' % n)
    sh('docker', 'cp', '%s/%s.app' % (a.apps_dir, n), '%s:/tmp/sui/apps/%s.app' % (C, n))

def use_original():
    dx('cp -f %s.keep %s' % (FW, FW), True)
def use_candidate():
    sh('docker', 'cp', a.swiftui, C + ':' + FW)

def run(n, label, d):
    out = '%s/%s/%s' % (a.runs, n, label)
    if os.path.isdir(out): shutil.rmtree(out)
    os.makedirs(os.path.dirname(out), exist_ok=True)
    dx('sh /tmp/sui/inside.sh /tmp/sui/apps/%s.app /tmp/sui/out/%s %d' % (n, n, a.timeout))
    sh('docker', 'cp', '%s:/tmp/sui/out/%s' % (C, n), out)
    return out

# ---- comparison -------------------------------------------------------------------------------
def rd(d, n):
    try: return open(os.path.join(d, n), 'rb').read().decode('utf-8', 'replace')
    except OSError: return ''
def mask(s):
    s = re.sub(r'\d{4}-\d\d-\d\d \d\d:\d\d:\d\d(\.\d+)?( [+-]\d{4})?', 'TIME', s)
    s = re.sub(r'\[\d+:\d+\]', '[PID]', s)
    s = re.sub(r'0x[0-9a-fA-F]{6,}', 'ADDR', s)
    s = re.sub(r'pid[ =:]\d+', 'pid N', s, flags=re.I)
    return s.splitlines()
def sections(out):
    """stdout split into: layout/notes lines, view tree lines, other lines"""
    lay, tree, other, intree = [], [], [], False
    for l in rd(out, 'stdout').splitlines():
        if l == 'VIEWTREE-BEGIN': intree = True; continue
        if l == 'VIEWTREE-END': intree = False; continue
        (tree if intree else lay if l.startswith(('LAYOUT ', 'NOTE ')) else other).append(l)
    return lay, tree, other
def status(out):
    s = rd(out, 'status').strip()
    return 'running-at-end' if s in ('124', 'killed') else 'exit ' + s
def windows(out):
    try: t = json.loads(rd(out, 'tree.json'))
    except ValueError: return None
    res = []
    def walk(n):
        if n.get('type') in ('con', 'floating_con') and n.get('name') is not None and not n.get('nodes'):
            r = n['rect']; res.append((n['name'], r['x'], r['y'], r['width'], r['height']))
        for c in n.get('nodes', []) + n.get('floating_nodes', []): walk(c)
    walk(t); return sorted(res, key=str)
def shot_diff(x, y):
    px, py = os.path.join(x, 'shot.ppm'), os.path.join(y, 'shot.ppm')
    if not (os.path.exists(px) and os.path.exists(py)):
        return None if os.path.exists(px) == os.path.exists(py) else 'screenshot missing in one run'
    if open(px, 'rb').read() == open(py, 'rb').read(): return None
    try:
        from PIL import Image, ImageChops
    except ImportError:
        return 'screenshots differ (no PIL for details)'
    ia, ib = Image.open(px).convert('RGB'), Image.open(py).convert('RGB')
    if ia.size != ib.size: return 'screenshot size %s vs %s' % (ia.size, ib.size)
    diff = ImageChops.difference(ia, ib)
    box = diff.getbbox()
    if not box: return None
    n = sum(1 for p in diff.convert('L').getdata() if p > 8)
    if n == 0: return None
    return 'screenshot: %d pixels differ in box %s' % (n, box)
def compare(x, y):
    d = []
    if status(x) != status(y): d.append('status %s vs %s' % (status(x), status(y)))
    if rd(x, 'final').strip() != rd(y, 'final').strip(): d.append('final-state line: %s vs %s' % (rd(x, 'final').strip(), rd(y, 'final').strip()))
    lx, tx, ox = sections(x); ly, ty, oy = sections(y)
    for what, p, q in (('layout/notes', lx, ly), ('view tree', tx, ty), ('stdout', ox, oy), ('stderr', mask(rd(x, 'stderr')), mask(rd(y, 'stderr')))):
        if p != q:
            i = next((i for i in range(max(len(p), len(q))) if i >= len(p) or i >= len(q) or p[i] != q[i]), 0)
            d.append('%s differs (%d vs %d lines); first: %r | %r' % (what, len(p), len(q), p[i][:140] if i < len(p) else None, q[i][:140] if i < len(q) else None))
    if windows(x) != windows(y): d.append('windows %s vs %s' % (windows(x), windows(y)))
    s = shot_diff(x, y)
    if s: d.append(s)
    return d

# ---- run --------------------------------------------------------------------------------------
def firstline(t):
    for l in t.splitlines():
        if l.strip(): return l.strip()[:110]
    return ''
rows = []
try:
    for n in names:
        o1, o2 = '%s/%s/orig1' % (a.runs, n), '%s/%s/orig2' % (a.runs, n)
        if a.rerun_orig or not (os.path.isdir(o1) and os.path.isdir(o2)):
            use_original()
            run(n, 'orig1', None); run(n, 'orig2', None)
        nd = compare(o1, o2)
        res = {'app': n, 'final': rd(o1, 'final').strip() == '1', 'status': status(o1), 'nondet': nd, 'note': firstline(rd(o1, 'stderr'))}
        line = '%-16s orig: %s, final %s%s' % (n, status(o1), 'yes' if res['final'] else 'NO', '' if not nd else '  NONDETERMINISTIC: ' + '; '.join(nd)[:200])
        if a.swiftui:
            use_candidate()
            c = run(n, 'cand', None)
            diffs = compare(o1, c)
            if nd: diffs = [x for x in diffs if x not in nd]
            line = '%-16s %s' % (n, 'same' if not diffs else 'DIFFERENT')
            if nd: line += ' (original itself is nondeterministic: ' + '; '.join(nd)[:120] + ')'
            for x in diffs: line += '\n    - ' + x
            res['diffs'] = diffs
        print(line, flush=True)
        rows.append(res)
finally:
    use_original()
if not a.swiftui and a.table:
    print('\n| app | final state | exit | nondeterministic | first stderr line |\n|---|---|---|---|---|')
    for r in rows:
        print('| %s | %s | %s | %s | %s |' % (r['app'], 'yes' if r['final'] else '**no**', r['status'], 'yes' if r['nondet'] else 'no', r['note'].replace('|', '/')))
