#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Map Apple's mangled Swift names onto the symbols of our support module.

usage: alias.py MODULE APPLE_MODULE apple-symbols.txt nm-output.txt  > link-flags.txt
Both name sets are demangled; "(extension in X):" qualifiers are dropped and the support module's
name is replaced by Apple's, so a symbol of ours that demangles like Apple's gets aliased to Apple's name.
Prints "-Xlinker -alias -Xlinker _ours -Xlinker _apples" per match, the misses go to stderr."""
import re, subprocess, sys

module, apple, symfile, nmfile = sys.argv[1:5]

def demangle(names):
    out = subprocess.run([subprocess.check_output(["xcrun", "-f", "swift-demangle"], text=True).strip()], input="\n".join(names) + "\n",
                         capture_output=True, text=True).stdout.splitlines()
    return out

def norm(text):
    text = re.sub(r"\(extension in [A-Za-z0-9_]+\):", "", text)
    text = text.replace(module, apple)
    return text

apple_syms = [l.split()[0] for l in open(symfile) if l.strip() and not l.startswith("#")]
ours = []
for line in open(nmfile):
    parts = line.split()
    if len(parts) >= 3 and parts[1] in "TtSsDd" and parts[2].startswith("_$s"):
        ours.append(parts[2])
ours_dem = demangle([s[1:] for s in ours])
table = {}
for s, d in zip(ours, ours_dem):
    table.setdefault(norm(d), s)
apple_dem = demangle([s[1:] if s.startswith("_") else s for s in apple_syms])
miss = 0
for s, d in zip(apple_syms, apple_dem):
    key = norm(d)
    if key in table:
        if table[key] != s:
            print("-Xlinker -alias -Xlinker %s -Xlinker %s" % (table[key], s))
    else:
        miss += 1
        sys.stderr.write("NO MATCH: %s\n   %s\n" % (s, d))
sys.stderr.write("aliases: %d of %d matched\n" % (len(apple_syms) - miss, len(apple_syms)))
