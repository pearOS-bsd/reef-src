#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Build the rebuilt Mach-O files of macho/ (clang and ld64.lld 19, run in the reef-build image).

usage: build-macho.py ORIGINAL_ROOT OUT_DIR [TARGET...]
ORIGINAL_ROOT   the root of the original package (usr/lib/cutefish-reef, or <ORIGINAL_USR>/lib/cutefish-reef): the build links against its
                files, as the original was linked against its own.
OUT_DIR         receives every file at its install path (OUT_DIR/usr/lib/libz.1.dylib), ready for tools/make-deb.sh.
TARGET          a folder under macho/ (default: every line of macho/targets.txt).

Each folder has a target.conf of `key = value` lines:
  install   path of the file under the root                              (required)
  type      dylib (default) | executable
  id        install name, default /<install>
  version, compat   current and compatibility version of a dylib        (default 1.0.0)
  sources   .c .m .S .cpp files in link order                           (required)
  cflags    flags for every source, ldflags for the link, libs: dependencies as paths under the root, in load command order
            (`reexport:PATH` links it with -reexport_library)
  minos     deployment target, default 11.0
Compiler: -O2 with no stack protector and no NDEBUG is what the original was built with (docs/MACHO.md).
"""
import os, sys, subprocess, shlex, shutil, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(os.path.dirname(HERE), 'macho')
LLVM = '/usr/lib/llvm-19/bin'
RESOURCE = '/usr/lib/llvm-19/lib/clang/19/include'

def conf(folder):
    c = {'type': 'dylib', 'version': '1.0.0', 'compat': '1.0.0', 'cflags': '', 'ldflags': '', 'libs': 'usr/lib/libSystem.B.dylib', 'minos': '11.0'}
    for line in open(os.path.join(SRC, folder, 'target.conf')):
        line = line.split('#')[0].strip()
        if not line: continue
        k, v = line.split('=', 1)
        c[k.strip()] = v.strip()
    c.setdefault('id', '/' + c['install'])
    return c

def run(cmd):
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode:
        sys.stderr.write(' '.join(shlex.quote(x) for x in cmd) + '\n' + r.stdout[-3000:] + r.stderr[-3000:])
        raise SystemExit(1)

def build(root, out, folder):
    c = conf(folder)
    target = os.path.join(out, c['install'])
    os.makedirs(os.path.dirname(target), exist_ok=True)
    work = tempfile.mkdtemp()
    try:
        objects = []
        for s in c['sources'].split():
            o = os.path.join(work, '%d_%s.o' % (len(objects), os.path.basename(s)))
            src = os.path.join(SRC, folder, s)
            cmd = [LLVM + '/clang', '--target=x86_64-apple-macos' + c['minos'], '-O2', '-nostdinc', '-isystem', RESOURCE,
                   '-isystem', os.path.join(SRC, 'include'), '-I' + os.path.dirname(src), '-w']
            if s.endswith('.m'): cmd += ['-x', 'objective-c']
            cmd += shlex.split(c['cflags']) + ['-c', src, '-o', o]
            run(cmd); objects.append(o)
        libs = []
        for l in c['libs'].split():
            # reexport:PATH adds the library with -reexport_library (a LC_REEXPORT_DYLIB); a plain PATH is a LC_LOAD_DYLIB
            libs += ['-reexport_library', os.path.join(root, l[9:])] if l.startswith('reexport:') else [os.path.join(root, l)]
        cmd = [LLVM + '/ld64.lld', '-arch', 'x86_64', '-platform_version', 'macos', c['minos'], c['minos'], '-syslibroot', root]
        if c['type'] == 'dylib':
            cmd += ['-dylib', '-install_name', c['id'], '-current_version', c['version'], '-compatibility_version', c['compat']]
        cmd += shlex.split(c['ldflags'])
        # the file name goes into the UUID: link under the name the file will have
        tmp = os.path.join(work, os.path.basename(c['install']))
        run(cmd + ['-o', tmp] + objects + libs)
        shutil.copy(tmp, target)
    finally:
        shutil.rmtree(work)
    return target

def main():
    if len(sys.argv) < 3: sys.exit(__doc__)
    root, out = os.path.abspath(sys.argv[1]), os.path.abspath(sys.argv[2])
    folders = sys.argv[3:] or [l.split('#')[0].strip() for l in open(os.path.join(SRC, 'targets.txt')) if l.split('#')[0].strip()]
    for f in folders:
        print(build(root, out, f))

if __name__ == '__main__':
    main()
