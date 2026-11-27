#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Write the source folder of a host shim (a library that is only jumps through a table the host fills in) from the original file.
usage: macho-gen-shim.py ORIGINAL.dylib FOLDER INSTALL_PATH VERSION COMPAT [NAME [SUFFIX]]   (SUFFIX: appended to the name the host is asked for)
FOLDER (a path) is created (symbols.h, trampolines.S, <name>.c, target.conf); the slot table name and the host library name are read from the file."""
import sys, os, struct, importlib.util
here = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('cm', os.path.join(here, 'compare-macho.py'))
cm = importlib.util.module_from_spec(spec); spec.loader.exec_module(cm)
orig, folder, install, version, compat = sys.argv[1:6]
I = cm.Image(orig)
rows = []
for a in sorted(I.funcs):
    d = I.data(a, 6)
    if len(d) == 6 and d[:2] == b'\xff\x25':
        t = a + 6 + struct.unpack_from('<i', d, 2)[0]
        sym = I.symbol_at(t)
        rows.append((sym[1] // 8, I.fname[a][1:], sym[0]))
slots = {r[2] for r in rows}
assert len(slots) == 1, slots
slots = slots.pop()
assert [r[0] for r in rows] == list(range(len(rows))), 'slots are not in function order'
sec = [s for s in I.sections if s['name'] == '__cstring'][0]
host = I.b[sec['off']:].split(b'\0')[0].decode()
suffix = sys.argv[7] if len(sys.argv) > 7 else ''
base = sys.argv[6] if len(sys.argv) > 6 else os.path.basename(folder.rstrip('/')).split('.')[0]
hdr = '''// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
'''
out = folder; os.makedirs(out, exist_ok=True)
lib = os.path.basename(install).split('.')[0]
open(os.path.join(out, 'symbols.h'), 'w').write(hdr + '// The functions of %s, in the order of the table that the host fills in.\n\n#define SYMBOLS(X) \\\n' % lib
    + ' \\\n'.join('    X(%d, %s)' % (i, n) for i, n, _ in rows) + '\n')
open(os.path.join(out, 'trampolines.S'), 'w').write(hdr + '// One jump through %s for every function of %s.\n\n#include "symbols.h"\n\n' % (slots.lstrip('_'), lib) + '''#define TRAMPOLINE(i, name) \\
    .globl _##name ;\\
    .p2align 3 ;\\
    _##name: ;\\
    jmpq *%s + i * 8(%%rip) ;

.text
SYMBOLS(TRAMPOLINE)
''' % slots)
text = (hdr + '// %s: its functions are jumps through a table that the host\'s library fills in when the library is loaded.\n\n#include "symbols.h"\n\n' % lib + '''extern void *reef_host_symbol(const char *library, const char *name);
extern void reef_bridge_missing(void);

enum {
#define COUNT(i, name) +1
    SYMBOL_COUNT = 0 SYMBOLS(COUNT)
#undef COUNT
};

__attribute__((visibility("hidden"))) void *%s[SYMBOL_COUNT];

static const char *const names[] = {
#define NAME(i, name) #name %s,
    SYMBOLS(NAME)
};

__attribute__((constructor)) static void resolve(void)
{
    for (int i = 0; i < SYMBOL_COUNT; i++) {
        if (%s[i])
            continue;
        void *function = reef_host_symbol("%s", names[i]);
        %s[i] = function ? function : (void *)reef_bridge_missing;
    }
}
''' % (slots.lstrip('_'), '"%s"' % suffix if suffix else '', slots.lstrip('_'), host, slots.lstrip('_'))).replace('#name ,', '#name,')
# a file with wrapper code written by hand next to the table (libbz2, libcurl ...) is kept: only symbols.h and trampolines.S are rewritten
if not os.path.exists(os.path.join(out, base + '.c')): open(os.path.join(out, base + '.c'), 'w').write(text)
if not os.path.exists(os.path.join(out, 'target.conf')): open(os.path.join(out, 'target.conf'), 'w').write('install = %s\nversion = %s\ncompat = %s\nsources = trampolines.S %s.c\ncflags = -fcommon\n' % (install, version, compat, base))
print(folder, len(rows), 'functions, slots', slots, 'host', host)
