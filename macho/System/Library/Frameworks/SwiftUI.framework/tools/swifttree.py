# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Demangle trees (from `swift demangle --tree-only`) and printing them as Swift source types."""
import re


class N:
    __slots__ = ('k', 't', 'c', 'i')

    def __init__(s, k, t=None, i=0, c=None):
        s.k = k; s.t = t; s.c = c if c is not None else []; s.i = i

    def __repr__(s): return 'N(%s%s,%d)' % (s.k, ',' + repr(s.t) if s.t is not None else '', len(s.c))

    def first(s, *kinds):
        for x in s.c:
            if x.k in kinds: return x
        return None

    def all(s, *kinds): return [x for x in s.c if x.k in kinds]

    def dump(s, ind=0):
        out = ['  ' * ind + s.k + (' ' + repr(s.t) if s.t is not None else '')]
        for c in s.c: out.append(c.dump(ind + 1))
        return '\n'.join(out)

    def key(s):
        """structural key (for comparing nodes)"""
        return s.k + ('=' + s.t if s.t is not None else '') + ('(' + ','.join(c.key() for c in s.c) + ')' if s.c else '')


RX = re.compile(r'^( *)kind=(\w+)(?:, (?:text|index)=(.*))?$')


def parse(lines):
    """parse demangler tree output -> {symbol: [Global nodes]}"""
    out = {}; cur = None; stack = []
    for line in lines:
        line = line.rstrip('\n')
        if line.startswith('Demangling for '):
            cur = line[15:]; out[cur] = []; stack = []; continue
        m = RX.match(line)
        if not m or cur is None:
            cur = None; continue
        t = m[3]
        if t is not None and len(t) >= 2 and t[0] == '"' and t[-1] == '"': t = t[1:-1]
        ind = len(m[1]) // 2; n = N(m[2], t, ind)
        while stack and stack[-1].i >= ind: stack.pop()
        if stack: stack[-1].c.append(n)
        else: out[cur].append(n)
        stack.append(n)
    return out


def gp_name(depth, index):
    """generic parameter names as the demangler prints them: A, B, ... (depth 0), A1, B1 ... (depth 1)"""
    letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
    s = letters[index % 26] + (str(index // 26) if index >= 26 else '')
    return s if depth == 0 else s + str(depth)
