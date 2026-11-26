#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Learn facts for the generator from the compiler's errors (fixes.json): Objective-C names renamed in Swift,
members that need `override` / `required`.  usage: learn.py ERRORS.txt [FIXES.json]  -> number of new facts"""
import json, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))

def main():
    errs = open(sys.argv[1]).read()
    path = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HERE, 'fixes.json')
    fx = json.load(open(path)) if os.path.exists(path) else {}
    new = 0
    ren = fx.setdefault('renames', {})
    for a, b in re.findall(r"error: '([A-Za-z_0-9.]+)' has been renamed to '([A-Za-z_0-9.]+)'", errs):
        if ren.get(a) != b: ren[a] = b; new += 1
    for a, b in re.findall(r"error: '([A-Za-z_0-9.]+)' is unavailable in Swift: use '([A-Za-z_0-9.]+)'", errs):
        if ren.get(a) != b: ren[a] = b; new += 1
    json.dump(fx, open(path, 'w'), indent=1, sort_keys=True)
    print(new)

if __name__ == '__main__':
    main()
