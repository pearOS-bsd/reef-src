#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Behaviour test of the rebuilt Mach-O files: every macho/<folder>/tests/*.c or *.m is built for macOS against the original file, and run
by reef twice, with the original root and with a copy of the root in which our file replaces the original.  Output and exit code must be equal.
usage: macho.py REEF ORIGINAL_ROOT BUILD_DIR [FOLDER...]
REEF           bin/reef of the original package (the loader is the same for both runs)
ORIGINAL_ROOT  its lib/cutefish-reef;  BUILD_DIR  where tools/build-macho.py put the files;  FOLDER  under macho/ (default: macho/targets.txt)
target.conf may set  testlibs = paths under the root the tests link (default: the file itself and libSystem),  testflags = compiler flags,
testargs = arguments after the directory the program gets as first argument."""
import os, re, shutil, subprocess, sys, tempfile, glob
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tools'))
import importlib.util
spec = importlib.util.spec_from_file_location('bm', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tools', 'build-macho.py'))
bm = importlib.util.module_from_spec(spec); spec.loader.exec_module(bm)

reef, root, built = sys.argv[1], os.path.abspath(sys.argv[2]), os.path.abspath(sys.argv[3])
folders = sys.argv[4:] or [l.split('#')[0].strip() for l in open(os.path.join(bm.SRC, 'targets.txt')) if l.split('#')[0].strip()]
frame = re.compile(rb'  #\d+  0x[0-9a-f]{16} reef .*\n')
work = tempfile.mkdtemp()

def make_root(name):
    base = os.path.join(work, name)
    os.makedirs(os.path.join(base, 'bin')); os.makedirs(os.path.join(base, 'lib'))
    subprocess.run(['cp', '-a', root, os.path.join(base, 'lib', 'cutefish-reef')], check=True)
    shutil.copy(reef, os.path.join(base, 'bin', 'reef'))
    return os.path.join(base, 'bin', 'reef')

def swap(loader, install, file):
    t = os.path.join(os.path.dirname(loader), '..', 'lib', 'cutefish-reef', install)
    os.unlink(t); shutil.copy(file, t)

def compile_test(source, conf, name):
    program = os.path.join(work, name)
    flags = bm.shlex.split(conf.get('testflags', ''))
    cc = [bm.LLVM + '/clang', '--target=x86_64-apple-macos12', '-O1', '-fno-stack-protector', '-w', '-nostdinc', '-isystem', bm.RESOURCE,
          '-isystem', os.path.join(bm.SRC, 'include'), '-I' + os.path.dirname(source)]
    if source.endswith('.m'): cc += ['-x', 'objective-c', '-fobjc-arc']
    subprocess.run(cc + flags + ['-c', source, '-o', program + '.o'], check=True)
    libs = conf.get('testlibs') or conf['install'] + ' usr/lib/libSystem.B.dylib'
    subprocess.run([bm.LLVM + '/ld64.lld', '-arch', 'x86_64', '-platform_version', 'macos', '12.0', '12.0', '-syslibroot', root, '-o', program, program + '.o']
                   + [os.path.join(root, l) for l in libs.split()], check=True)
    return program

failed = total = 0
a, b = make_root('a'), make_root('b')
for folder in folders:
    conf = bm.conf(folder)
    sources = sorted(glob.glob(os.path.join(bm.SRC, folder, 'tests', '*.[cm]')))
    if not sources:
        print('%s: no tests' % folder); failed += 1; continue
    ours = os.path.join(built, conf['install'])
    swap(b, conf['install'], ours)
    for source in sources:
        name = os.path.basename(source)
        total += 1
        program = compile_test(source, conf, folder.replace('/', '_') + '_' + name.rsplit('.', 1)[0])
        results = []
        for loader in (a, b):
            directory = os.path.join(work, 'run'); shutil.rmtree(directory, ignore_errors=True); os.mkdir(directory)
            subprocess.run(['pkill', '-f', '--', '--mach-broker'], capture_output=True)
            os.makedirs('/run/user/%d' % os.getuid(), exist_ok=True)
            run = subprocess.run([loader, program, directory] + conf.get('testargs', '').split(), capture_output=True, timeout=120, cwd=directory, stdin=subprocess.DEVNULL)
            results.append((run.returncode, run.stdout, frame.sub(b'', run.stderr)))
        if b'a host library function is missing' in results[0][2] and results[0] == results[1]:
            print('%s/%s: same, but the host has no such library, so the call was not tried' % (folder, name))
        elif results[0][0] != 0 or not results[0][1]:
            print('%s/%s: the test itself fails with the original: exit %d, %d bytes of output %s' % (folder, name, results[0][0], len(results[0][1]), results[0][2][-300:])); failed += 1
        elif results[0] == results[1]:
            print('%s/%s: same, exit %d, %d lines' % (folder, name, results[0][0], results[0][1].count(b'\n')))
        else:
            failed += 1
            print('%s/%s: differs, exit %d and %d' % (folder, name, results[0][0], results[1][0]))
            for stream in (1, 2):
                x, y = (results[i][stream].decode(errors='replace').splitlines() for i in (0, 1))
                for i in range(max(len(x), len(y))):
                    p, q = x[i] if i < len(x) else '(end)', y[i] if i < len(y) else '(end)'
                    if p != q:
                        print('  - %s\n  + %s' % (p[:200], q[:200])); break
    swap(b, conf['install'], os.path.join(root, conf['install']))
shutil.rmtree(work)
print('%d tests, %d failed' % (total, failed))
sys.exit(1 if failed else 0)
