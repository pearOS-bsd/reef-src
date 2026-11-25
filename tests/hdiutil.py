#!/usr/bin/env python3
"""Differential test of hdiutil: the same folders made into images by the original and by ours, with the clock fixed
(LD_PRELOAD of time()); images, output and exit codes must be the same.  What stays random is masked: the HFS+ volume
UUID and the UDIF segment id.
usage: hdiutil.py ORIGINAL OURS"""
import os, shutil, subprocess, sys, tempfile, time

original, ours = sys.argv[1], sys.argv[2]
work = tempfile.mkdtemp()
clock = os.path.join(work, 'fixedtime.so')
with open(clock + '.c', 'w') as f:
    f.write('#include <time.h>\ntime_t time(time_t* t) { if (t) *t = 1795622163; return 1795622163; }\n')
subprocess.run(['gcc', '-shared', '-fPIC', '-o', clock, clock + '.c'], check=True)
environment = dict(os.environ, LD_PRELOAD=clock)

def folder(name, build):
    path = os.path.join(work, name)
    os.makedirs(path)
    build(path)
    # Times that reading does not change (relatime): access after modification and recent.
    now = int(time.time()) + 3600
    for root, dirs, files in os.walk(path):
        for item in dirs + files + ['.']:
            os.utime(os.path.join(root, item), (now, 1700000000), follow_symlinks=False)
    return path

def write(path, data):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(data)

def small(p):
    write(os.path.join(p, 'a.txt'), 'hello\n')
    write(os.path.join(p, 'sub', 'b.bin'), '0' * 5000)
    os.symlink('a.txt', os.path.join(p, 'link'))

def names(p):
    for name in ['lower.txt', 'UPPER.TXT', 'a very long file name that exceeds limits.document', 'dots.in.the.name.tar.gz', 'café.txt',
                 'noext', 'semi;colon', '日本語.txt', 'Ωmega.txt', '.hidden', 'a.b.', 'x.averyveryverylongextensionthatgoesonandon',
                 'j' * 70 + '.txt', 'j' * 70 + '-other.txt', 'emoji-😀', 'colon:name', 'ﬁ ligature']:
        write(os.path.join(p, name), name)
    for i in range(14):
        write(os.path.join(p, 'coll%s.txt' % ('-' * i)), str(i))
    for i in range(12):
        write(os.path.join(p, 'averyveryverylongbasename_that_is_long%s.c' % ('+' * (i + 1))), str(i))
    for name in ['same name', 'same-name', 'SAME NAME']:
        write(os.path.join(p, 'zeta', name), name)
    deep = p
    for i in range(10):
        deep = os.path.join(deep, 'level %d' % i)
    write(os.path.join(deep, 'deep.txt'), 'deep')
    os.makedirs(os.path.join(p, 'empty folder'))
    write(os.path.join(p, 'big.bin'), 'x' * 300000)
    open(os.path.join(p, 'empty'), 'w').close()
    os.symlink('/etc/hostname', os.path.join(p, 'absolute'))
    os.symlink('../../x/./y', os.path.join(p, 'zeta', 'relative'))

def many(p):
    for d in range(12):
        for f in range(150):
            write(os.path.join(p, 'dir%02d' % d, 'file-with-a-longer-name-%04d.txt' % f), 'x' * (f * 37 % 9000))

folders = {'small': folder('small', small), 'names': folder('names', names), 'many': folder('many', many)}
failed = 0
total = 0

def run(program, arguments, cwd):
    result = subprocess.run([program] + arguments, cwd=cwd, env=environment, capture_output=True, timeout=600)
    text = lambda b: b.decode(errors='replace').replace(cwd, 'WORK')
    return result.returncode, text(result.stdout), text(result.stderr)

def masked(path):
    if not os.path.exists(path):
        return None
    data = bytearray(open(path, 'rb').read())
    if len(data) > 1536 and data[1024:1026] in (b'H+', b'HX'):
        for base in (1024, len(data) - 1024):
            data[base + 104:base + 112] = bytes(8)
    if len(data) >= 512 and data[-512:-508] == b'koly':
        data[-512 + 64:-512 + 80] = bytes(16)
    return bytes(data)

def compare(name, arguments, outputs):
    global failed, total
    total += 1
    results = []
    for program, side in ((original, 'a'), (ours, 'b')):
        directory = os.path.join(work, 'run-' + side)
        shutil.rmtree(directory, ignore_errors=True)
        os.makedirs(directory)
        code, out, err = run(program, [argument.replace('OUT', directory) for argument in arguments], directory)
        out = out.replace(directory, 'OUT')
        err = err.replace(directory, 'OUT')
        results.append((code, out, err, [masked(os.path.join(directory, o)) for o in outputs]))
    if results[0] == results[1]:
        print('%s: same, exit %d' % (name, results[0][0]))
        return
    failed += 1
    print('%s: differs' % name)
    for i in range(3):
        if results[0][i] != results[1][i]:
            print('  -', str(results[0][i])[:300])
            print('  +', str(results[1][i])[:300])
    for o, a, b in zip(outputs, results[0][3], results[1][3]):
        if a != b:
            print('  %s: %s and %s bytes' % (o, len(a) if a else None, len(b) if b else None))
            if a and b:
                at = sorted(set(i // 2048 for i in range(min(len(a), len(b))) if a[i] != b[i]))
                print('    sectors of 2048 that differ:', at[:20])

for name, path in folders.items():
    for options in ([], ['-hfs'], ['-iso'], ['-joliet'], ['-udf'], ['-iso', '-udf'], ['-joliet', '-udf'], ['-hfs', '-iso']):
        compare('makehybrid %s %s' % (name, ' '.join(options)), ['makehybrid'] + options + ['-o', 'OUT/image.img', path], ['image.img'])
compare('makehybrid volume names', ['makehybrid', '-udf-volume-name', 'Ünïcode Vol', '-iso-volume-name', 'my.volume', '-o', 'OUT/v.img',
                                    folders['names']], ['v.img'])
# A raw HFS+ image made once, then converted to every format by both.
raw = os.path.join(work, 'raw.img')
subprocess.run([original, 'makehybrid', '-hfs', '-o', raw, folders['many']], env=environment, capture_output=True, check=True)
for image_format in ('UDZO', 'UDRO', 'UDBZ', 'ULFO', 'ULMO', 'UDTO', 'UDRW'):
    extension = '.cdr' if image_format == 'UDTO' else '.dmg'
    compare('convert %s' % image_format, ['convert', raw, '-format', image_format, '-o', 'OUT/c', '-puppetstrings'], ['c' + extension])
compare('convert zlib level 9', ['convert', raw, '-format', 'UDZO', '-imagekey', 'zlib-level=9', '-o', 'OUT/z'], ['z.dmg'])
udzo = os.path.join(work, 'udzo.dmg')
subprocess.run([original, 'convert', raw, '-format', 'ULFO', '-o', udzo], env=environment, capture_output=True, check=True)
compare('convert UDIF to raw', ['convert', udzo, '-format', 'UDTO', '-o', 'OUT/r'], ['r.cdr'])
compare('verify', ['verify', udzo], [])
compare('verify raw', ['verify', raw], [])
damaged = os.path.join(work, 'damaged.dmg')
data = bytearray(open(udzo, 'rb').read())
data[1000] ^= 0xff
open(damaged, 'wb').write(data)
compare('verify damaged', ['verify', damaged], [])
compare('create', ['create', '-srcfolder', folders['small'], '-volname', 'Vol', '-format', 'UDTO', 'OUT/created'], ['created.cdr'])
compare('create usage', ['create', 'OUT/x'], [])
compare('create two folders', ['create', '-srcfolder', folders['small'], '-srcfolder', folders['names'], 'OUT/x'], [])
compare('create file system', ['create', '-srcfolder', folders['small'], '-fs', 'APFS', 'OUT/x'], [])
compare('create encrypted', ['create', '-srcfolder', folders['small'], '-encryption', 'AES-128', 'OUT/x'], [])
compare('convert format', ['convert', raw, '-format', 'XYZ', '-o', 'OUT/x'], [])
compare('convert missing', ['convert', os.path.join(work, 'nothing'), '-format', 'UDZO', '-o', 'OUT/x'], [])
compare('unknown command', ['attach', raw], [])
case = folder('case', lambda p: (write(os.path.join(p, 'Readme'), 'a'), write(os.path.join(p, 'README'), 'b')))
compare('makehybrid case', ['makehybrid', '-hfs', '-o', 'OUT/x.img', case], ['x.img'])
shutil.rmtree(work)
print('%d cases, %d differ' % (total, failed))
sys.exit(1 if failed else 0)
