#!/usr/bin/env python3
"""Differential test of libreef-webkit-host.so: the original and ours with the same stand-in for WebKit, the same calls
(JSON both ways, input events, website data kinds, find options, screens, application names, the table itself).
usage: webkit.py ORIGINAL OURS   (in the container)"""
import os, subprocess, sys, tempfile
here = os.path.dirname(os.path.abspath(__file__))
out = []
with tempfile.TemporaryDirectory() as d:
    names = set()
    for lib in sys.argv[1:3]:
        t = subprocess.run(['nm', '-D', '--undefined-only', lib], capture_output=True, text=True, check=True).stdout
        names |= {l.split()[-1] for l in t.splitlines() if l.split()[-1].startswith('WK')}
    own = open(here + '/webkit_fake.cpp').read()
    with open(d + '/generic.inc', 'w') as f:
        for n in sorted(names):
            if ' ' + n + '(' not in own.replace('* ' + n, ' ' + n): f.write('F(%s)\n' % n)
    fake = d + '/libcutefish-webkit.so.2'
    subprocess.run(['g++', '-std=c++20', '-O1', '-shared', '-fPIC', '-w', '-I', d, '-Wl,-soname,libcutefish-webkit.so.2', '-o', fake,
                    here + '/webkit_fake.cpp'], check=True)
    subprocess.run(['g++', '-std=c++20', '-O1', '-w', '-o', d + '/driver', here + '/webkit_driver.cpp', fake, '-lglib-2.0', '-ldl'], check=True)
    for lib in sys.argv[1:3]:
        r = subprocess.run([d + '/driver', os.path.abspath(lib)], capture_output=True, env=dict(os.environ, LD_LIBRARY_PATH=d), timeout=300)
        out.append((r.returncode, r.stdout.splitlines(), r.stderr))
(ra, a, ea), (rb, b, eb) = out
bad = [(i, x, y) for i, (x, y) in enumerate(zip(a, b)) if x != y]
print(f'{len(a)} lines, {len(bad) + abs(len(a) - len(b))} differ, exit {ra} {rb}')
for i, x, y in bad[:10]: print(i, x[:200], '\n   ', y[:200])
if ra or rb: print(ea[-300:], eb[-300:])
sys.exit(bool(bad) or len(a) != len(b) or ra != rb or len(a) < 1000)
