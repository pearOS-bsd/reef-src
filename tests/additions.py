#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Behaviour test of the Mach-O files that are not in the original package (macho/additions.txt).
usage: additions.py REEF ORIGINAL_ROOT BUILD_DIR [FOLDER...]
Every macho/<folder>/tests/NAME.c|m is built for macOS against the files of the root and run by reef with our files added to a copy of the root; its standard output
must be the golden output NAME.expected (recorded on a real Mac) and the exit code 0.  target.conf may set testlibs (paths under the root the test links; default
libSystem) and testflags (compiler flags; $REPO and $SRC as in cflags).  A folder with an exports.txt is also checked with tools/check-exports.py (its optional
imports.txt: the symbols the apps import, which must all be exported)."""
import os, shutil, subprocess, sys, tempfile, glob
import importlib.util
HERE = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('bm', os.path.join(HERE, '..', 'tools', 'build-macho.py'))
bm = importlib.util.module_from_spec(spec); spec.loader.exec_module(bm)

reef, root, built = sys.argv[1], os.path.abspath(sys.argv[2]), os.path.abspath(sys.argv[3])
folders = sys.argv[4:] or [l.split('#')[0].strip() for l in open(os.path.join(bm.SRC, 'additions.txt')) if l.split('#')[0].strip()]
work = tempfile.mkdtemp()
base = os.path.join(work, 'r')
os.makedirs(os.path.join(base, 'bin')); os.makedirs(os.path.join(base, 'lib'))
subprocess.run(['cp', '-a', root, os.path.join(base, 'lib', 'cutefish-reef')], check=True)
shutil.copy(reef, os.path.join(base, 'bin', 'reef'))
loader = os.path.join(base, 'bin', 'reef')
installed = os.path.join(base, 'lib', 'cutefish-reef')
failed = total = 0
for folder in folders:
    conf = bm.conf(folder)
    target = os.path.join(installed, conf['install'])
    os.makedirs(os.path.dirname(target), exist_ok=True)
    shutil.copy(os.path.join(built, conf['install']), target)
for folder in folders:
    conf = bm.conf(folder)
    exports = os.path.join(bm.SRC, folder, 'exports.txt')
    if os.path.exists(exports):
        total += 1
        cmd = [sys.executable, os.path.join(HERE, '..', 'tools', 'check-exports.py'), os.path.join(built, conf['install']), exports]
        imports = os.path.join(bm.SRC, folder, 'imports.txt')
        if os.path.exists(imports): cmd.append(imports)
        r = subprocess.run(cmd, capture_output=True, text=True)
        print('%s exports: %s' % (folder, r.stdout.splitlines()[0] if r.stdout else r.stderr[-200:]))
        if r.returncode:
            failed += 1; print(r.stdout[-1500:])
    for source in sorted(glob.glob(os.path.join(bm.SRC, folder, 'tests', '*.[cm]'))):
        name = os.path.basename(source).rsplit('.', 1)[0]
        expected = source.rsplit('.', 1)[0] + '.expected'
        total += 1
        program = os.path.join(work, folder.replace('/', '_') + '_' + name)
        flags = bm.shlex.split(conf.get('testflags', '').replace('$REPO', bm.REPO).replace('$SRC', os.path.join(bm.SRC, folder)))
        cc = [bm.LLVM + '/clang', '--target=x86_64-apple-macos12', '-O1', '-fno-stack-protector', '-w', '-nostdinc', '-I' + os.path.dirname(source)]
        if source.endswith('.m'): cc += ['-x', 'objective-c', '-fobjc-arc']
        flags += ['-isystem', bm.INCLUDE, '-isystem', bm.RESOURCE]
        subprocess.run(cc + flags + ['-c', source, '-o', program + '.o'], check=True)
        libs = conf.get('testlibs', 'usr/lib/libSystem.B.dylib')
        subprocess.run([bm.LLVM + '/ld64.lld', '-arch', 'x86_64', '-platform_version', 'macos', '12.0', '12.0', '-syslibroot', installed, '-o', program, program + '.o']
                       + [os.path.join(installed, l) for l in libs.split()], check=True)
        directory = os.path.join(work, 'run'); shutil.rmtree(directory, ignore_errors=True); os.mkdir(directory)
        subprocess.run(['pkill', '-f', '--', '--mach-broker'], capture_output=True)
        os.makedirs('/run/user/%d' % os.getuid(), exist_ok=True)
        run = subprocess.run([loader, program] + conf.get('testargs', '').split(), capture_output=True, timeout=300, cwd=directory, stdin=subprocess.DEVNULL)
        want = open(expected, 'rb').read() if os.path.exists(expected) else None
        if run.returncode == 0 and run.stdout == want:
            print('%s/%s: same as the Mac, %d lines' % (folder, name, run.stdout.count(b'\n')))
        else:
            failed += 1
            print('%s/%s: differs, exit %d' % (folder, name, run.returncode))
            x = (want or b'(no .expected)').decode(errors='replace').splitlines(); y = run.stdout.decode(errors='replace').splitlines()
            for i in range(max(len(x), len(y))):
                p, q = x[i] if i < len(x) else '(end)', y[i] if i < len(y) else '(end)'
                if p != q: print('  - %s\n  + %s' % (p[:200], q[:200])); break
            print(run.stderr.decode(errors='replace')[-600:])
shutil.rmtree(work)
print('%d checks, %d failed' % (total, failed))
sys.exit(1 if failed else 0)
