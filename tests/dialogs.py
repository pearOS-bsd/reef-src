#!/usr/bin/env python3
"""Differential tests of reef-open and reef-unsupported without a display (uishim.so prints what the dialogs show).
usage: dialogs.py ORIGINAL_USR BUILD_DIR SHIM APPS_DIR   (APPS_DIR: folder of signed .app bundles, not in the repository)"""
import glob, os, random, subprocess, sys, tempfile, struct
orig, ours, shim, apps = sys.argv[1:5]
bad = n = 0
seen = {}
assert os.path.exists('/.dockerenv'), 'container only: the settings of the account are removed'
CONF = os.path.expanduser('~root/.config/cutefish/reef.conf')
# The same layout for both, with a stand-in for the loader that reef-unsupported asks for everything missing.
import shutil
roots = {}
for side, where in (('orig', orig + '/libexec/cutefish-reef'), ('ours', ours)):
    r = roots[side] = tempfile.mkdtemp(); os.makedirs(r + '/libexec/cutefish-reef'); os.makedirs(r + '/bin')
    for x in ('reef-open', 'reef-unsupported'): shutil.copy(f'{where}/{x}', r + '/libexec/cutefish-reef/')
    open(r + '/bin/reef', 'w').write('#!/bin/sh\necho missing "$@"\n'); os.chmod(r + '/bin/reef', 0o755)
def run(p, args, env=None):
    home = tempfile.mkdtemp()
    if os.path.exists(CONF): os.remove(CONF)
    r = subprocess.run([os.path.basename(p), *args], executable=p, capture_output=True, timeout=30,
                       env={'PATH': '/usr/bin:/bin', 'HOME': home, 'LD_PRELOAD': shim, 'LD_LIBRARY_PATH': os.environ.get('LD_LIBRARY_PATH', ''), **(env or {})})
    conf = open(CONF, 'rb').read() if os.path.exists(CONF) else b''
    return r.returncode, r.stdout, r.stderr.replace(home.encode(), b'HOME'), conf
def check(name, prog, args, env=None, show=None):
    global bad, n
    a = run(f"{roots['orig']}/libexec/cutefish-reef/{prog}", args, env); b = run(f"{roots['ours']}/libexec/cutefish-reef/{prog}", args, env)
    for l in a[1].split(b'\n'):
        if prog == 'reef-open' and l.startswith((b'label Dev', b'label The', b'label From')): seen[l] = seen.get(l, 0) + 1
    n += 1; same = a == b; bad += not same
    if not same: print('FAIL', name, *[(x, y) for x, y in zip(a, b) if x != y], sep='\n  ')
    elif show: print('ok  ', name, a[0], [l.decode() for l in a[1].split(b'\n') if l.startswith((b'label', b'text', b'icon')) and b'\xff' not in l], a[3][:80])
    assert a[0] >= 0 or a[0] == b[0], (name, a)
def opened(name, path, show=True, env=None): check('open ' + name, 'reef-open', ['com.x', path, '/bin/echo', 'echo-arg0', 'arg1'], env, show)
check('open usage', 'reef-open', ['a', 'b'], show=True)
for app in sorted(glob.glob(apps + '/*.app')):
    for exe in sorted(glob.glob(app + '/Contents/MacOS/*'))[:1]:
        opened(os.path.basename(app), exe); opened(os.path.basename(app) + ' clicked', exe, env={'CLICK': '1'})
        data = bytearray(open(exe, 'rb').read())
        d = tempfile.mkdtemp(); f = d + '/program'
        def variant(what, b, show=False): open(f, 'wb').write(b); opened(os.path.basename(app) + ' ' + what, f, show)
        variant('outside its bundle', data, True)
        # Cut files, and damage in the headers, the load commands and the signature.
        for cut in (0, 3, 4, 7, 8, 31, 32, 100, 4096, len(data) // 2, len(data) - 1): variant(f'cut at {cut}', data[:cut])
        rnd = random.Random(1)
        at = [i for m in (b'\xfa\xde\x0c\xc0', b'\xfa\xde\x0c\x02', b'\xfa\xde\x0b\x01') for i in {data.find(m), data.rfind(m)} if i >= 0]
        for base, span, count in [(0, 64, 40), *[(i, 120, 60) for i in at], *[(i, 9000, 60) for i in at[-2:]]]:
            for _ in range(count):
                c = bytearray(data); p = min(base + rnd.randrange(span), len(c) - 1); c[p] = rnd.randrange(256); variant(f'byte {p}', c)
                if base in at[:2]: c[p:p + 4] = b'\xff\xff\xff\xff'; variant(f'ones {p}', c)
for name, b in {'text': b'#!/bin/sh\n', 'no slices': struct.pack('>II', 0xcafebabe, 0), 'fat 64 none': struct.pack('>II', 0xcafebabf, 0),
                'many slices': struct.pack('>II', 0xcafebabe, 9999) + b'\0' * 4000}.items():
    d = tempfile.mkdtemp(); open(d + '/p', 'wb').write(b); opened(name, d + '/p')
opened('missing file', '/nonexistent/x/'); opened('directory', '/tmp'); opened('root', '/')
U = 'reef-unsupported'
check('unsupported usage', U, []); check('unsupported bad flag', U, ['--x', 'p', 'r'])
reasons = ['', 'x', 'Foo: no such symbol _bar', 'p has no slice for this processor: it has only arm64, this computer is x86_64',
           'p is built for another processor: it has only i386 and ppc, this computer is x86_64', 'has no slice for this processor',
           'is built for another processor, this computer is x: it has only y', ': it has only a, this computer is b',
           'has no slice for this processor: it has only a, this computer is b, this computer is c']
progs = ['/bin/ls', '/nonexistent', 'p', '/a/b/', '', *sorted(glob.glob(apps + '/*.app/Contents/MacOS/*'))[:3]]
for flag in ('--launch', '--running'):
    for p in progs:
        for r in reasons: check(f'unsupported {flag} {p} {r}', U, [flag, p, r], show=(p == '/bin/ls' and 'i386' in r))
print('reef-open outcomes:', {k.decode(errors='replace'): v for k, v in seen.items()})
print(n, 'cases,', bad, 'differ'); sys.exit(bool(bad))
