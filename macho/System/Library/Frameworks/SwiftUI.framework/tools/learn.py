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
    # members: "<qualified type>|<member signature>" (the signature as emit.member_sig prints it)
    src = os.path.join(HERE, '..')
    for what, rx in (('override', r"error: overriding declaration requires an 'override' keyword"),
                     ('required', r"error: 'required' modifier must be present on all overrides of a required initializer"),
                     ('not_override', r"error: (method|property|initializer|subscript) does not override any (method|property|initializer|subscript) from its superclass")):
        lst = fx.setdefault(what, [])
        for m in re.finditer(r'^(Sources/[^:]+):(\d+):\d+: ' + rx, errs, re.M):
            k = member_at(os.path.join(src, m[1]), int(m[2]))
            if k and k not in lst: lst.append(k); new += 1
    json.dump(fx, open(path, 'w'), indent=1, sort_keys=True)
    print(new)


def member_at(path, line):
    try:
        lines = open(path).read().split('\n')
    except OSError:
        return None
    text = lines[line - 1]
    ind = len(text) - len(text.lstrip())
    names = []
    for i in range(line - 2, -1, -1):
        l = lines[i]
        li = len(l) - len(l.lstrip())
        if li < ind and l.strip():
            m = re.match(r'\s*(?:@\w+ )*(?:public |open |final |internal )*(?:class|struct|enum|extension) ([A-Za-z_0-9.]+)', l)
            if m:
                names.insert(0, m[1]); ind = li
                if l.lstrip().startswith('extension') or li == 0: break
    if not names: return None
    q = '.'.join(names)
    if not q.startswith('SwiftUI.'): q = 'SwiftUI.' + q
    t = text.strip()
    static = bool(re.search(r'\b(static|class) (func|var|subscript)', t))
    m = re.search(r'\bfunc ([^(<\s]+)\s*(?:<[^(]*>)?\(', t)
    if m:
        nm = m[1].strip('`')
    elif re.search(r'\binit[?!]?\s*(?:<[^(]*>)?\(', t):
        nm = 'init'
    elif re.search(r'\bsubscript', t):
        nm = 'subscript'
    else:
        m = re.search(r'\bvar ([^:\s]+)\s*:', t)
        if not m: return None
        return '%s|%s%s()' % (q, 'static ' if static else '', m[1].strip('`'))
    labs = re.findall(r'(?:\(|, )([A-Za-z_`][A-Za-z_0-9`]*) a\d+:', t)
    return '%s|%s%s(%s)' % (q, 'static ' if static else '', nm, ':'.join(x.strip('`') for x in labs))

if __name__ == '__main__':
    main()
