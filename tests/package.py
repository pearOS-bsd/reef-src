#!/usr/bin/env python3
"""Differential test of reef-install-package: the original and ours on the same packages (made here, good and broken ones), comparing
what --list and --install print, the exit code and the installed tree (types, modes, dates, contents, links).
usage: package.py ORIGINAL OURS   (in the container; LD_LIBRARY_PATH with the toolkit's libraries)"""
import gzip, lzma, os, random, shutil, stat, struct, subprocess, sys, tempfile, zlib
from xml.sax.saxutils import escape, quoteattr

def odc(entries, trailer=True):
    out = b''
    for n, (name, mode, data, kw) in enumerate(entries + ([('TRAILER!!!', 0, b'', {})] if trailer else [])):
        b = name.encode('utf-8', 'surrogateescape') + b'\0'
        out += b'070707%06o%06o%06o%06o%06o%06o%06o%011o%06o%011o' % (1, kw.get('inode', n + 1) & 0o777777, mode, 0, 0, kw.get('links', 1), 0,
            kw.get('time', 1600000000 + n), len(b), kw.get('size', len(data)))
        out += b + data
    return out

def newc(entries, trailer=True, magic=b'070701'):
    out = b''
    pad = lambda: b'\0' * (-len(out) % 4)
    for n, (name, mode, data, kw) in enumerate(entries + ([('TRAILER!!!', 0, b'', {})] if trailer else [])):
        b = name.encode('utf-8', 'surrogateescape') + b'\0'
        out += magic + b''.join(b'%08x' % v for v in (kw.get('inode', n + 1), mode, 0, 0, kw.get('links', 1), kw.get('time', 1600000000 + n),
            kw.get('size', len(data)), 0, 0, 0, 0, len(b), 0))
        out += b; out += pad(); out += data; out += pad()
    return out

def pbzx(data, chunk=1 << 16, raw=False):
    out = b'pbzx' + struct.pack('>Q', chunk)
    for i in range(0, max(len(data), 1), chunk):
        part = data[i:i + chunk]
        packed = part if raw else lzma.compress(part, check=lzma.CHECK_CRC32)
        out += struct.pack('>QQ', len(part), len(packed)) + packed
    return out

def xar(files, broken=None):
    heap = b''; xml = ''; n = 0
    def add(tree):
        nonlocal heap, xml, n
        for name, value in tree.items():
            n += 1
            xml += '<file id="%d"><name>%s</name>' % (n, escape(name))
            if isinstance(value, dict):
                xml += '<type>directory</type>'; add(value)
            else:
                data, encoding = value if isinstance(value, tuple) else (value, None)
                packed = zlib.compress(data) if encoding == 'application/x-gzip' else data
                xml += '<type>file</type><data><offset>%d</offset><size>%d</size><length>%d</length>' % (len(heap), len(data), len(packed))
                xml += '<encoding style=%s/>' % quoteattr(encoding) if encoding else ''
                xml += '</data>'; heap += packed
            xml += '</file>'
    add(files)
    toc = ('<?xml version="1.0" encoding="UTF-8"?><xar><toc>%s</toc></xar>' % xml).encode()
    if broken == 'xml': toc = toc[:-20]
    if broken == 'notoc': toc = b'<xar/>'
    packed = zlib.compress(toc)
    size = len(toc) + (7 if broken == 'size' else 0)
    if broken == 'zlib': packed = packed[:-4] + b'abcd'
    header = struct.pack('>4sHHQQI', b'xar!' if broken != 'magic' else b'xarx', 28 if broken != 'header' else 20, 1, len(packed), size, 0)
    out = header + packed + heap
    return out[:len(out) - 40] if broken == 'short' else out

def info(location='/', scripts='', identifier='org.example', version='1.2'):
    return ('<pkg-info identifier=%s version=%s%s>%s</pkg-info>' % (quoteattr(identifier), quoteattr(version),
        ' install-location=%s' % quoteattr(location) if location is not None else '', scripts)).encode()

F, D, L = stat.S_IFREG, stat.S_IFDIR, stat.S_IFLNK
def app(name='Example.app', prefix='./Applications/', big=0):
    p = prefix + name
    return [(prefix.rstrip('/') or '.', D | 0o755, b'', {}), (p, D | 0o755, b'', {}), (p + '/Contents', D | 0o700, b'', {'time': 1234567890}),
            (p + '/Contents/Info.plist', F | 0o644, b'<plist/>', {}), (p + '/Contents/MacOS', D | 0o755, b'', {}),
            (p + '/Contents/MacOS/Example', F | 0o755, b'\xcf\xfa\xed\xfe' + bytes(i & 255 for i in range(big)), {'time': 1000000000}),
            (p + '/Contents/Current', L | 0o755, b'MacOS/Example', {})]

def cases(r):
    good = app()
    for name, pack in (('odc', odc), ('newc', newc), ('newc2', lambda e: newc(e, magic=b'070702'))):
        for how, wrap in (('raw', bytes), ('gzip', gzip.compress), ('pbzx', pbzx), ('pbzxraw', lambda d: pbzx(d, raw=True)), ('pbzxsmall', lambda d: pbzx(d, 100))):
            for encoding in (None, 'application/octet-stream', 'application/x-gzip'):
                yield f'{name}-{how}-{encoding}', xar({'PackageInfo': (info(), encoding), 'Payload': (wrap(pack(good)), encoding)})
    yield 'big', xar({'PackageInfo': info(), 'Payload': gzip.compress(odc(app(big=300000)))})
    yield 'nested', xar({'Distribution': b'<installer-gui-script><title>A title</title><product id="x" version="9.9"/></installer-gui-script>',
        'One.pkg': {'PackageInfo': info('/Applications'), 'Payload': gzip.compress(odc(app(prefix='./')))},
        'Two.pkg': {'PackageInfo': info('/Applications', '<scripts><preinstall file="./preinstall"/></scripts>'), 'Payload': odc(app('Two.app', './'))},
        'Not': {'PackageInfo': info()}, 'x.pkgx': {'PackageInfo': info()}, '.pkg': {'PackageInfo': info(), 'Payload': odc(app('No.app'))},
        'Sub': {'Deep.pkg': {'PackageInfo': info(), 'Payload': odc(app('Deep.app'))}}})
    for broken in ('xml', 'notoc', 'size', 'zlib', 'magic', 'header', 'short'):
        yield 'broken-' + broken, xar({'PackageInfo': info(), 'Payload': odc(good)}, broken)
    yield 'empty', b''
    yield 'text', b'not a package at all, but longer than its header would be'
    for scripts in ('<scripts/>', '<scripts><postinstall/></scripts>', '<scripts><preinstall/><postinstall/></scripts>', '<scripts><other/></scripts>'):
        yield 'scripts', xar({'PackageInfo': info(scripts=scripts), 'Payload': odc(good)})
    yield 'scripts-file', xar({'PackageInfo': info(), 'Payload': odc(good), 'Scripts': b'x'})
    yield 'nopayload', xar({'PackageInfo': info()})
    yield 'payload-dir', xar({'PackageInfo': info(), 'Payload': {'a': b'b'}})
    yield 'notpkginfo', xar({'PackageInfo': b'<other/>', 'Payload': odc(good)})
    yield 'badinfo', xar({'PackageInfo': b'<pkg-info', 'Payload': odc(good)})
    yield 'emptyinfo', xar({'PackageInfo': b'', 'Payload': odc(good)})
    yield 'dotnames', xar({'.': b'x', '..': b'x', 'a/b': b'x', '': b'x', 'PackageInfo': info(), 'Payload': odc(good)})
    for location in (None, '', '/', '/Applications', '/Applications/', 'Applications', '/Applications/Utilities', '/opt', '/./Applications/.', '/a/../Applications', '//Applications'):
        yield f'location-{location}', xar({'PackageInfo': info(location), 'Payload': odc(good)})
        yield f'location2-{location}', xar({'PackageInfo': info(location), 'Payload': odc(app(prefix='./'))})
    yield 'format', xar({'PackageInfo': info(), 'Payload': b'PK\3\4 something else'})
    yield 'format-short', xar({'PackageInfo': info(), 'Payload': b'070'})
    yield 'pbzx-header', xar({'PackageInfo': info(), 'Payload': b'pbzx\0\0'})
    yield 'pbzx-cut', xar({'PackageInfo': info(), 'Payload': pbzx(odc(good))[:-50]})
    yield 'pbzx-huge', xar({'PackageInfo': info(), 'Payload': b'pbzx' + struct.pack('>QQQ', 1 << 20, 1 << 40, 10) + b'0123456789'})
    yield 'gzip-cut', xar({'PackageInfo': info(), 'Payload': gzip.compress(odc(good))[:-30]})
    yield 'notrailer', xar({'PackageInfo': info(), 'Payload': odc(good, trailer=False)})
    P = './Applications/A.app'
    base = [(P, D | 0o755, b'', {})]
    special = {
        'through': base + [(P + '/link', L | 0o777, b'/tmp', {}), (P + '/link/file', F | 0o644, b'x', {})],
        'through2': base + [(P + '/a', L | 0o777, b'..', {}), (P + '/a/b/c', F | 0o644, b'x', {}), (P + '/d', F | 0o600, b'y', {})],
        'dotdot': base + [(P + '/../../x', F | 0o644, b'x', {}), ('../up', F | 0o644, b'x', {})],
        'hard': base + [(P + '/one', F | 0o644, b'data', {'inode': 77, 'links': 2}), (P + '/two', F | 0o644, b'', {'inode': 77, 'links': 2})],
        'hard-before': base + [(P + '/one', F | 0o644, b'', {'inode': 77, 'links': 3}), (P + '/two', F | 0o644, b'', {'inode': 77, 'links': 3}), (P + '/three', F | 0o755, b'data', {'inode': 77, 'links': 3})],
        'hard-none': base + [(P + '/one', F | 0o644, b'', {'inode': 77, 'links': 2}), (P + '/two', F | 0o644, b'abc', {'inode': 78, 'links': 2})],
        'noparents': [(P + '/x/y/z', F | 0o644, b'x', {})],
        'fileroot': [(P, F | 0o644, b'x', {})],
        'linkroot': [(P, L | 0o644, b'/etc', {}), (P + '/x', F | 0o644, b'x', {})],
        'dirfile': base + [(P + '/x', F | 0o644, b'x', {}), (P + '/x', D | 0o755, b'', {})],
        'filedir': base + [(P + '/x', D | 0o755, b'', {}), (P + '/x', F | 0o644, b'x', {})],
        'linkover': base + [(P + '/x', F | 0o644, b'x', {}), (P + '/x', L | 0o644, b'y', {})],
        'device': base + [(P + '/dev', stat.S_IFCHR | 0o644, b'', {}), (P + '/fifo', stat.S_IFIFO | 0o644, b'', {}), ('/etc/passwd', F | 0o644, b'x', {})],
        'setuid': base + [(P + '/s', F | 0o6755, b'x', {}), (P + '/n', F | 0, b'x', {})],
        'longlink': base + [(P + '/l', L | 0o777, b'a' * 4097, {})],
        'emptylink': base + [(P + '/l', L | 0o777, b'', {})],
        'zero': base + [(P + '/zero', F | 0o644, b'', {})],
        'names': [('./Applications/.app', D | 0o755, b'', {}), ('./Applications/a.appx', D | 0o755, b'', {}), ('./Applications/noapp/x', F | 0o644, b'', {}), ('Applications/B.app/x', F | 0o644, b'1', {}), ('/Applications/C.app/x', F | 0o644, b'1', {}), ('.//Applications//D.app//x', F | 0o644, b'1', {}), ('./Applications/E.app/./x', F | 0o644, b'1', {})],
        'unicode': [('./Applications/Ünïcode “x”.app/ä', F | 0o644, b'1', {})],
        'longname': base + [(P + '/' + 'n' * 300, F | 0o644, b'x', {})],
        'hugename': base + [(P + '/' + 'n/' * 2100, F | 0o644, b'x', {})],
        'sizelie': base + [(P + '/x', F | 0o644, b'x', {'size': 1000})],
        'deep': base + [(P + '/' + '/'.join('d%d' % i for i in range(n)), D | (0o555 if n % 2 else 0o755), b'', {'time': 10 ** 9 + n}) for n in range(1, 25)],
    }
    for name, entries in special.items():
        yield 'odc-' + name, xar({'PackageInfo': info(), 'Payload': odc(entries)})
        yield 'newc-' + name, xar({'PackageInfo': info(), 'Payload': newc(entries)})
    for seed in range(150):
        r.seed(seed)
        entries = []
        for _ in range(r.randrange(1, 14)):
            name = ''.join(r.choice(['./', '', '/']) if i == 0 else r.choice(['Applications/', 'A.app/', 'B.app/', 'x/', './', 'l/', '../', 'y/']) for i in range(r.randrange(1, 5)))
            if r.random() < .7: name = './Applications/' + r.choice(['A.app', 'B.app', 'A.app/l', 'A.app/x', 'B.app/x']) + r.choice(['', '/f', '/x/y', '/l', '/l/z'])
            kind = r.choice([F, F, D, L, F, stat.S_IFIFO])
            data = b'' if kind == D else r.choice([b'l', b'..', b'x/y', b'/tmp/zz']) if kind == L else bytes(r.randrange(256) for _ in range(r.choice([0, 1, 5, 300])))
            entries.append((name, kind | r.randrange(0o1000), data, {'inode': r.randrange(1, 4), 'links': r.choice([1, 1, 2]), 'time': r.randrange(2 ** 31)}))
        payload = r.choice([odc, newc])(entries)
        if r.random() < .25:
            b = bytearray(payload); b[r.randrange(len(b))] = r.randrange(256); payload = bytes(b)
        if r.random() < .1: payload = payload[:r.randrange(len(payload))]
        payload = r.choice([bytes, gzip.compress, pbzx])(payload)
        yield f'random-{seed}', xar({'PackageInfo': info(r.choice(['/', '/Applications', None])), 'Payload': payload})

def tree(d):
    out = []; inodes = {}
    for root, dirs, files in os.walk(d):
        for n in sorted(dirs + files) + ([''] if root == d else []):
            p = os.path.join(root, n) if n else root
            s = os.lstat(p); what = [os.path.relpath(p, d), oct(s.st_mode)]
            if stat.S_ISREG(s.st_mode):
                os.chmod(p, 0o600); what += [open(p, 'rb').read(), s.st_mtime_ns, inodes.setdefault(s.st_ino, len(inodes)), s.st_nlink]
            elif stat.S_ISLNK(s.st_mode): what += [os.readlink(p)]
            elif stat.S_ISDIR(s.st_mode) and n: what += [s.st_mtime_ns if s.st_mtime_ns < 10 ** 18 * 1.7 else 'now']
            out.append(what)
        dirs.sort()
    return sorted(out, key=lambda x: x[0])

def run(binary, package, d):
    out = []
    for args in (['--list', package], ['--install', package, '--into', d]):
        r = subprocess.run([binary, *args], capture_output=True, timeout=60)
        out.append((r.returncode, r.stdout, r.stderr.replace(binary.encode(), b'BIN')))
    return out, tree(d)

bad = n = 0
with tempfile.TemporaryDirectory() as t:
    for args in ([], ['a', 'b', 'c'], ['--list'], ['--list', '/nonexistent'], ['--what', '/nonexistent'], ['--install', '/nonexistent', '--into'], ['-x', 'y', 'z']):
        res = [subprocess.run([b, *args], capture_output=True, timeout=60) for b in sys.argv[1:3]]; n += 1
        if len({(r.returncode, r.stdout, r.stderr) for r in res}) != 1: bad += 1; print('arguments', args, res)
    for name, data in cases(random.Random(1)):
        for group in (False, True):
            p = t + '/Some Package.v2.pkg'; open(p, 'wb').write(data); res = []
            for b in sys.argv[1:3]:
                d = t + '/out'; os.mkdir(d); os.chmod(d, 0o2775 if group else 0o755)
                res.append(run(b, p, d))
                subprocess.run(['chmod', '-R', 'u+rwx', d], capture_output=True); shutil.rmtree(d)
            n += 1
            if res[0] != res[1]:
                bad += 1
                if bad < 6:
                    print('DIFFERS', name, group)
                    for a, b in zip(res[0][0], res[1][0]): print('  ', a, '\n  ', b)
                    for x in res[0][1]:
                        if x not in res[1][1]: print('   -', x)
                    for x in res[1][1]:
                        if x not in res[0][1]: print('   +', x)
            if not name.startswith(('odc', 'newc', 'random')): break
print(f'{n} cases, {bad} differ')
sys.exit(bool(bad))
