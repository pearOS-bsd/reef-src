#!/usr/bin/env python3
"""Differential test of the loader: the original reef and ours on the same Mach-O files, comparing what they print and the exit code.
usage: loader.py ORIGINAL OURS [PATH...]   ORIGINAL is the installed bin/reef (its ../lib/cutefish-reef has the frameworks); PATH are more
programs or bundles to list (applications of a Mac, for instance)."""
import os, shutil, struct, subprocess, sys, tempfile

original, ours = sys.argv[1], sys.argv[2]
root = os.path.normpath(os.path.join(os.path.dirname(original), '..', 'lib', 'cutefish-reef'))
work = tempfile.mkdtemp()
# Our loader finds the frameworks the same way: next to it, in ../lib/cutefish-reef.
os.makedirs(os.path.join(work, 'bin'))
os.symlink(os.path.dirname(root), os.path.join(work, 'lib'))
loader = os.path.join(work, 'bin', 'reef')
shutil.copy(ours, loader)
for name in os.listdir(os.path.dirname(ours)):
    if name.endswith('.so'):
        shutil.copy(os.path.join(os.path.dirname(ours), name), os.path.join(work, 'bin', name))

def write(name, data):
    path = os.path.join(work, 'files', name)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'wb') as f:
        f.write(data)
    return path

# Files that start like Mach-O and are not, or are broken.
files = os.path.join(work, 'files')
write('empty', b'')
write('text', b'#!/bin/sh\necho hello\n')
write('short', struct.pack('<I', 0xfeedfacf))
write('header-only', struct.pack('<IiiIIIII', 0xfeedfacf, 0x01000007, 3, 2, 0, 0, 0, 0))
write('bad-commands', struct.pack('<IiiIIIII', 0xfeedfacf, 0x01000007, 3, 2, 5, 400, 0, 0) + b'\x01' * 64)
write('arm64', struct.pack('<IiiIIIII', 0xfeedfacf, 0x0100000c, 0, 2, 0, 0, 0, 0))
write('fat-empty', struct.pack('>II', 0xcafebabe, 0))
write('fat-java', struct.pack('>II', 0xcafebabe, 52) + b'\0' * 64)
write('fat-broken', struct.pack('>II', 0xcafebabe, 1) + struct.pack('>iiIII', 0x01000007, 3, 4096, 4096, 12))
write('fat64-broken', struct.pack('>II', 0xcafebabf, 2) + b'\xff' * 64)
os.makedirs(os.path.join(files, 'Empty.app', 'Contents', 'MacOS'))
os.symlink('/nonexistent', os.path.join(files, 'dangling'))

paths = [root, os.path.join(root, 'System', 'Library', 'Frameworks'), os.path.join(root, 'usr'), files, '/nonexistent', '/etc/hostname']
for directory, names, others in os.walk(root):
    for name in sorted(names):
        if name.endswith('.framework') or name.endswith('.app'):
            paths.append(os.path.join(directory, name))
    for name in sorted(others):
        if name.endswith('.dylib') or os.access(os.path.join(directory, name), os.X_OK):
            paths.append(os.path.join(directory, name))
paths += [os.path.join(files, name) for name in sorted(os.listdir(files))]
paths += sys.argv[3:]

failed = 0
cases = 0
for path in paths:
    for option in ('--list-missing', '--list-missing=json'):
        results = []
        for program in (original, loader):
            run = subprocess.run([program, option, path], capture_output=True, timeout=900, cwd=os.path.join(work, 'bin'))
            results.append((run.returncode, run.stdout, run.stderr))
        cases += 1
        if results[0] != results[1]:
            failed += 1
            print('differs:', option, path)
            for label, (code, out, err) in zip(('original', 'ours'), results):
                print('  %s: exit %d, %d bytes out, %d bytes err' % (label, code, len(out), len(err)))
            a, b = results[0][1].decode(errors='replace').splitlines(), results[1][1].decode(errors='replace').splitlines()
            for line_a, line_b in zip(a, b):
                if line_a != line_b:
                    print('  -', line_a[:200])
                    print('  +', line_b[:200])
                    break
print('%d cases, %d differ' % (cases, failed))
shutil.rmtree(work)
sys.exit(1 if failed else 0)
