#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Write the string constants that symvals found on the Mac as C sources of our frameworks.

usage: constants.py SYMVALS.tsv FRAMEWORKS_DIR NAME
SYMVALS.tsv has the lines of symvals (LIB SYMBOL OBJ CLASS VALUE); every constant string of a library becomes
`const CFStringRef X = CFSTR("value");` in FRAMEWORKS_DIR/<framework>/Sources/<NAME>Constants.c (made or replaced).
The definitions are weak: a constant that another source of the same framework defines as well is not a duplicate.
Prints the frameworks written."""
import collections, os, sys

tsv, root, name = sys.argv[1:4]

def framework(lib):
    base = os.path.basename(lib)
    if lib.startswith("/usr/lib/"):
        return base.split(".")[0]
    return base

def cstring(text):
    text = text.replace("\\n", "\n").replace("\\\\", "\\")
    out = []
    for ch in text:
        if ch in '"\\':
            out.append("\\" + ch)
        elif ch == "\n":
            out.append("\\n")
        elif 32 <= ord(ch) < 127:
            out.append(ch)
        else:
            out.extend("\\x%02x\"\"" % b for b in ch.encode())
    return "".join(out)

by = collections.defaultdict(list)
for line in open(tsv):
    parts = line.rstrip("\n").split("\t")
    if len(parts) >= 5 and parts[2] == "OBJ" and parts[3] in ("__NSCFConstantString", "NSTaggedPointerString", "__NSCFString"):
        by[framework(parts[0])].append((parts[1].lstrip("_") if parts[1].startswith("_") else parts[1], parts[4]))

for fw, items in sorted(by.items()):
    folder = os.path.join(root, fw, "Sources")
    os.makedirs(folder, exist_ok=True)
    with open(os.path.join(folder, name + "Constants.c"), "w") as f:
        f.write("// SPDX-FileCopyrightText: 2026 Alexandru Balan\n// SPDX-License-Identifier: BSD-2-Clause\n\n")
        f.write("// String constants of %s that %s imports, with the values of Apple's framework (tools/apple-values).\n\n" % (fw, name))
        f.write("#include <CoreFoundation/CoreFoundation.h>\n\n")
        for sym, value in sorted(set(items)):
            f.write('__attribute__((weak)) const CFStringRef %s = CFSTR("%s");\n' % (sym, cstring(value)))
    print(fw, len(items))
