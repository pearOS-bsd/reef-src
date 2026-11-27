#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Compare the exported symbols of a built dylib with the list of the Apple library it replaces.
usage: check-exports.py DYLIB EXPORTS.TXT [IMPORTED.TXT]
EXPORTS.TXT   one symbol per line: what `dyld_info -exports` shows for the library in the cache of macOS
IMPORTED.TXT  optional: the symbols apps import from it (all must be exported)
Prints the symbols Apple exports and the dylib lacks, and the extra ones; exit status 1 when an imported symbol is missing."""
import subprocess, sys
nm = '/usr/lib/llvm-19/bin/llvm-nm'
out = subprocess.run([nm, '-gU', '--defined-only', sys.argv[1]], capture_output=True, text=True, check=True).stdout
ours = {l.split()[-1] for l in out.splitlines() if l.strip()}
apple = {l.strip() for l in open(sys.argv[2]) if l.strip()}
print('exports: apple %d, ours %d, common %d' % (len(apple), len(ours), len(apple & ours)))
missing = sorted(apple - ours)
print('missing (%d): %s' % (len(missing), ' '.join(missing)))
extra = sorted(ours - apple)
print('extra (%d): %s' % (len(extra), ' '.join(extra)))
bad = 0
if len(sys.argv) > 3:
    need = {l.strip() for l in open(sys.argv[3]) if l.strip()}
    lack = sorted(need - ours)
    print('imported by apps: %d, lacking: %s' % (len(need), ' '.join(lack) or 'none'))
    bad = bool(lack)
sys.exit(1 if bad else 0)
