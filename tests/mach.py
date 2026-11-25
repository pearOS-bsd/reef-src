#!/usr/bin/env python3
"""Differential test of the loader with programs: every tests/mach/*.c is built for macOS with clang and lld, against the original's
libSystem, and run by the original reef and by ours; the output and the exit code must be the same.
usage: mach.py ORIGINAL OURS [NAME...]   ORIGINAL is the installed bin/reef (its ../lib/cutefish-reef has the frameworks)"""
import os, re, shutil, subprocess, sys, tempfile

original, ours = sys.argv[1], sys.argv[2]
here = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'mach')
names = sys.argv[3:] or sorted(name[:-2] for name in os.listdir(here) if name.endswith('.c'))
root = os.path.normpath(os.path.join(os.path.dirname(original), '..', 'lib', 'cutefish-reef'))
work = tempfile.mkdtemp()
os.makedirs(os.path.join(work, 'bin'))
os.symlink(os.path.dirname(root), os.path.join(work, 'lib'))
loader = os.path.join(work, 'bin', 'reef')
shutil.copy(ours, loader)
extra = [name for name in os.listdir(os.path.dirname(ours)) if name.endswith('.so')]
# A crash report names the frames of the loader itself: their addresses are never the same.
frame = re.compile(rb'  #\d+  0x[0-9a-f]{16} reef .*\n')

def build(name):
    program = os.path.join(work, name)
    subprocess.run(['clang', '--target=x86_64-apple-macos12', '-O1', '-fno-stack-protector', '-w', '-c', os.path.join(here, name + '.c'), '-o', program + '.o'], check=True)
    subprocess.run(['ld64.lld' if shutil.which('ld64.lld') else 'ld64.lld-19', '-arch', 'x86_64', '-platform_version', 'macos', '12.0', '12.0', '-syslibroot', root,
                    '-o', program, program + '.o', os.path.join(root, 'usr', 'lib', 'libSystem.B.dylib')], check=True)
    return program

failed = 0
for name in names:
    program = build(name)
    results = []
    for which in (original, loader):
        directory = os.path.join(work, 'run')
        shutil.rmtree(directory, ignore_errors=True)
        os.mkdir(directory)
        for library in extra:
            shutil.copy(os.path.join(os.path.dirname(ours), library), directory)
        run = subprocess.run([which, program, directory], capture_output=True, timeout=120, cwd=directory, stdin=subprocess.DEVNULL)
        results.append((run.returncode, run.stdout, frame.sub(b'', run.stderr)))
    if results[0] == results[1]:
        print('%s: same, exit %d, %d lines' % (name, results[0][0], results[0][1].count(b'\n')))
        continue
    failed += 1
    print('%s: differs, exit %d and %d' % (name, results[0][0], results[1][0]))
    for stream in (1, 2):
        a, b = (results[i][stream].decode(errors='replace').splitlines() for i in (0, 1))
        for i in range(max(len(a), len(b))):
            x, y = a[i] if i < len(a) else '(end)', b[i] if i < len(b) else '(end)'
            if x != y:
                print('  - %s\n  + %s' % (x[:200], y[:200]))
                break
shutil.rmtree(work)
print('%d programs, %d differ' % (len(names), failed))
sys.exit(1 if failed else 0)
