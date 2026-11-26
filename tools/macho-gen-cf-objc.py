#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Write the Foundation declarations that CoreFoundation's Objective-C dispatch needs: for every CF_OBJC_FUNCDISPATCHV,
CF_OBJC_RETAINED_FUNCDISPATCHV and CF_OBJC_CALLV in the sources, the class, the selector, the argument types (the casts in the call) and the
return type, as an @interface.  usage: macho-gen-cf-objc.py SOURCE_DIR > Foundation/NSDispatch.h"""
import re, sys, os, collections

def split_top(s, ch):
    out, depth, cur = [], 0, ''
    for c in s:
        if c in '([{': depth += 1
        if c in ')]}': depth -= 1
        if c == ch and depth == 0: out.append(cur); cur = ''
        else: cur += c
    out.append(cur); return out

# argument types the calls do not show (the arguments are plain variables)
OVERRIDES = {
    '_rangeOfUnit:startTime:interval:forAT:': ['NSCalendarUnit', 'CFAbsoluteTime *', 'CFTimeInterval *', 'CFAbsoluteTime'],
    '_rangeOfUnit:inUnit:forAT:': ['NSCalendarUnit', 'NSCalendarUnit', 'CFAbsoluteTime'],
    '_ordinalityOfUnit:inUnit:forAT:': ['NSCalendarUnit', 'NSCalendarUnit', 'CFAbsoluteTime'],
    '_daylightSavingTimeOffsetForAbsoluteTime:': ['CFAbsoluteTime'],
    '_nextDaylightSavingTimeTransitionAfterAbsoluteTime:': ['CFAbsoluteTime'],
}
def selector(rest):
    """'getBytes:(void *)b range:NSMakeRange(a, b)' -> ('getBytes:range:', ['void *', 'NSRange'])"""
    rest = rest.strip()
    if not re.search(r'\w\s*:', rest): return rest, []
    parts, types, i, n = [], [], 0, len(rest)
    for m in re.finditer(r'(\w+)\s*:', rest):
        pass
    # tokenise at depth 0
    depth, start, names, spans = 0, 0, [], []
    i = 0
    toks = []
    while i < n:
        c = rest[i]
        if c in '([{': depth += 1
        elif c in ')]}': depth -= 1
        elif depth == 0:
            m = re.match(r'(\w+)\s*:', rest[i:])
            if m and (i == 0 or not (rest[i-1].isalnum() or rest[i-1] == '_')):
                toks.append((i, m.group(1), i + m.end())); i += m.end(); continue
        i += 1
    for k, (pos, name, after) in enumerate(toks):
        end = toks[k+1][0] if k + 1 < len(toks) else n
        arg = rest[after:end].strip()
        m = re.match(r'\(([^()]*(?:\([^()]*\))?[^()]*)\)', arg)
        if m and not arg.startswith('(*'): t = m.group(1).strip()
        elif arg.startswith('NSMakeRange'): t = 'NSRange'
        elif arg in ('NULL', '0'): t = 'void *' if arg == 'NULL' else 'long'
        elif arg in ('YES', 'NO'): t = 'BOOL'
        else: t = 'id'
        names.append(name); types.append(t)
    sel = ':'.join(names) + ':'
    return sel, OVERRIDES.get(sel, types)

classes = collections.OrderedDict()
def add(cls, rtype, rest):
    sel, types = selector(rest)
    m = classes.setdefault(cls, {})
    if sel not in m: m[sel] = (rtype, types, rest)

root = sys.argv[1]
for dp, _, fs in os.walk(root):
    for f in sorted(fs):
        if not f.endswith('.c'): continue
        text = open(os.path.join(dp, f), errors='replace').read()
        text = re.sub(r'//[^\n]*', '', text)
        for m in re.finditer(r'CF_OBJC_(?:RETAINED_)?FUNCDISPATCHV\(', text):
            j = m.end(); depth = 1; k = j
            while depth:
                if text[k] == '(': depth += 1
                elif text[k] == ')': depth -= 1
                k += 1
            args = split_top(text[j:k-1], ',')
            if len(args) < 4: continue
            rtype = args[1].strip(); mo = re.match(r'\(\s*(\w+)\s*\*\s*\)', args[2].strip())
            if not mo: continue
            add(mo.group(1), rtype, ','.join(args[3:]).strip())
        for m in re.finditer(r'CF_OBJC_CALLV\(', text):
            j = m.end(); depth = 1; k = j
            while depth:
                if text[k] == '(': depth += 1
                elif text[k] == ')': depth -= 1
                k += 1
            args = split_top(text[j:k-1], ',')
            mo = re.match(r'\(\s*(\w+)\s*\*?\s*\)', args[0].strip())
            if mo and mo.group(1) != 'id': add(mo.group(1), 'id', ','.join(args[1:]).strip())

print('''// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Declarations of the Foundation classes that CoreFoundation messages (written by tools/macho-gen-cf-objc.py from the CF_OBJC_* calls).
#pragma once''')
print('@class %s;' % ', '.join(classes))
for c, sels in classes.items():
    sup = 'NSObject'
    if c.startswith('NSMutable') and ('NS' + c[9:]) in classes: sup = 'NS' + c[9:]
    print('@interface %s : %s' % (c, sup))
    for sel, (r, types, rest) in sels.items():
        r = 'BOOL' if r == 'Boolean' else r      # the Foundation methods return BOOL; the call site converts to Boolean
        if not types: print('- (%s)%s;' % (r, sel))
        else:
            names = sel.split(':')[:-1]
            print('- (%s)%s;' % (r, ' '.join('%s:(%s)a%d' % (n, t, i) for i, (n, t) in enumerate(zip(names, types)))))
    print('@end')
