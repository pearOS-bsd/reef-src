# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# relax.py OUT_DIR PACKAGE.deb... : the packages again, installable on this system: a version constraint that neither this
# system's apt nor one of the given packages can meet is dropped (the dependency itself stays); the version gets +local1.
import os, re, subprocess, sys, tempfile
out, debs = sys.argv[1], sys.argv[2:]
field = lambda deb, f: subprocess.run(['dpkg-deb', '-f', deb, f], capture_output=True, text=True).stdout.strip()
given = {field(d, 'Package'): field(d, 'Version') + '+local1' for d in debs}
def available(name):
    if name in given: return [given[name]]
    p = subprocess.run(['apt-cache', 'policy', name], capture_output=True, text=True).stdout
    return [v for v in re.findall(r'^ (?:\*\*\*| {3}) (\S+) \d+', p, re.M)]
def meets(version, op, want):
    ops = {'>=': 'ge', '<=': 'le', '=': 'eq', '>>': 'gt', '<<': 'lt'}
    return subprocess.run(['dpkg', '--compare-versions', version, ops[op], want]).returncode == 0
def relax(text, dropped):
    parts = []
    for alternatives in text.split(','):
        alts = []
        for a in alternatives.split('|'):
            a = a.strip()
            m = re.fullmatch(r'(\S+)\s*\((>=|<=|=|>>|<<)\s*(\S+)\)', a)
            if m and not any(meets(v, m[2], m[3]) for v in available(m[1].split(':')[0])):
                dropped.append(a); a = m[1]
            alts.append(a)
        parts.append(' | '.join(alts))
    return ', '.join(parts)
for deb in debs:
    work = tempfile.mkdtemp()
    subprocess.run(['dpkg-deb', '-R', deb, work], check=True)
    path = os.path.join(work, 'DEBIAN', 'control')
    lines = open(path).read().split('\n')
    dropped = []
    for i, line in enumerate(lines):
        key, _, value = line.partition(': ')
        if key in ('Depends', 'Pre-Depends', 'Recommends'): lines[i] = key + ': ' + relax(value, dropped)
        if key == 'Version': lines[i] = 'Version: ' + value + '+local1'
    open(path, 'w').write('\n'.join(lines))
    name = field(deb, 'Package')
    target = os.path.join(out, f"{name}_{given[name].split(':')[-1]}_{field(deb, 'Architecture')}.deb")
    subprocess.run(['dpkg-deb', '--root-owner-group', '-Zxz', '-b', work, target], check=True, capture_output=True)
    subprocess.run(['rm', '-rf', work])
    print(os.path.basename(target), 'dropped:', ', '.join(dropped) or '-')
