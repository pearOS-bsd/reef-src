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
  sourcedir folder under the repository where sources not found in the target folder are (upstream code); $REPO and $SRC in cflags are
            the repository and the target folder
  cflags.NAME  extra flags for the source NAME (as in `sources`), after cflags
  data.NAME  `IMAGE SYMBOL...`: before the compilation tools/macho-extract-data.py cuts the named data objects out of IMAGE (a path under the original root: tables that
            are the original's data, not our code, and are never kept in the repository) and writes the assembler source gen/NAME.S, to be listed in `sources`
  cflags    flags for every source, ldflags for the link, libs: dependencies as paths under the root, in load command order
            (`reexport:PATH` links it with -reexport_library)
  minos     deployment target, default 11.0
patches/NAME.patch in a target folder is a unified diff (`patch -p0`, paths ignored) of the upstream source NAME: the build applies it to a copy of the
file, which keeps its place for `#include "..."` (the upstream folder is a -iquote path) and in assert paths (the file prefix map is repeated for the copy).
patches/include/NAME.patch does the same for a header of the upstream include folders (include/, internalInclude/): the patched copy comes first in the include path.
The patch keeps the licence notice of the upstream file (Apache-2.0 with Runtime Library Exception) in its header: the patched file is a derived work.
Compiler: -O2 with no stack protector and no NDEBUG is what the original was built with (docs/MACHO.md).
"""
import os, sys, subprocess, shlex, shutil, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
INCLUDE = os.path.join(os.path.dirname(HERE), 'macho', 'include')
SRC = os.environ.get('MACHO_SRC', os.path.join(os.path.dirname(HERE), 'macho'))   # MACHO_SRC: drafts outside the checkout
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
        overlay = os.path.join(work, 'overlay'); os.makedirs(overlay)
        pinc = os.path.join(SRC, folder, 'patches', 'include')
        for h in sorted(os.listdir(pinc)) if os.path.isdir(pinc) else []:
            name = h[:-len('.patch')]
            found = [os.path.join(REPO, c['sourcedir'], d, name) for d in ('include', 'internalInclude', '.') if os.path.exists(os.path.join(REPO, c['sourcedir'], d, name))]
            shutil.copy(found[0], os.path.join(overlay, name))
            run(['patch', '-s', '-p0', os.path.join(overlay, name), os.path.join(pinc, h)])
        gen = os.path.join(work, 'gen'); os.makedirs(gen)
        for key, value in sorted(c.items()):
            if key.startswith('data.'):
                image, *symbols = value.split()
                run([sys.executable, os.path.join(HERE, 'macho-extract-data.py'), os.path.join(root, image), os.path.join(gen, key[5:] + '.S')] + symbols)
        objects = []
        for s in c['sources'].split():
            o = os.path.join(work, '%d_%s.o' % (len(objects), os.path.basename(s)))
            src = os.path.join(SRC, folder, s)
            if s.startswith('gen/'): src = os.path.join(work, s)
            if not os.path.exists(src) and 'sourcedir' in c: src = os.path.join(REPO, c['sourcedir'], s)
            patch = os.path.join(SRC, folder, 'patches', os.path.basename(s) + '.patch')
            orig_dir, extra = os.path.dirname(src), []
            if os.path.exists(patch) and not src.startswith(os.path.join(SRC, folder)):
                pdir = os.path.join(work, 'patched_%d' % len(objects)); os.makedirs(pdir)
                copy = os.path.join(pdir, os.path.basename(src)); shutil.copy(src, copy)
                run(['patch', '-s', '-p0', copy, patch])
                extra = ['-iquote', orig_dir]      # files next to the original
                src = copy
            cmd = [LLVM + '/clang', '--target=x86_64-apple-macos' + c['minos'], '-O2', '-nostdinc', '-iquote', os.path.dirname(src)] + extra + ['-w']
            if s.endswith('.m'): cmd += ['-x', 'objective-c']
            if s.endswith('.cpp'): cmd += ['-x', 'c++']
            # the flags come first: libc++'s headers must be found before the C headers they wrap
            cmd += ['-I', overlay]
            cmd += shlex.split((c['cflags'] + ' ' + c.get('cflags.' + s, '')).replace('$REPO', REPO).replace('$SRC', os.path.join(SRC, folder)))
            if extra:       # assert paths: the copy is mapped like the original folder
                for m in [x for x in cmd if x.startswith('-ffile-prefix-map=' + orig_dir + '=')]:
                    cmd.insert(cmd.index(m), '-ffile-prefix-map=' + os.path.dirname(src) + '=' + m.split('=', 2)[2])
            if s.endswith('.S'): cmd += ['-x', 'assembler']
            cmd += ['-isystem', INCLUDE, '-isystem', RESOURCE, '-c', src, '-o', o]
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
        if os.environ.get('MACHO_KEEP_OBJECTS'):     # for the analysis of a file that is built from many sources: <dir>/<source>.o
            os.makedirs(os.environ['MACHO_KEEP_OBJECTS'], exist_ok=True)
            for s_, o_ in zip(c['sources'].split(), objects):
                d_ = os.path.join(os.environ['MACHO_KEEP_OBJECTS'], s_.replace('/', '_') + '.o'); shutil.copy(o_, d_)
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
