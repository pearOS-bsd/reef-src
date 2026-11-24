#!/usr/bin/env python3
"""Try source variants of one function and rank them by how close the code is to the original.
usage: variants.py OPTIONS   (in the container)
OPTIONS is a Python file that sets: source, orig, addr, symbol, flags (list), link (list), and options: a list of lists of
alternatives, each alternative a list of (old, new) text replacements ([] = as it is).  One axis at a time first
(coordinate descent, two rounds), so the cost is the number of alternatives and not their product.
The best source is written to /out/best.cpp."""
import difflib, os, subprocess, sys
from concurrent.futures import ProcessPoolExecutor
sys.dont_write_bytecode = True
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ndiff
cfg = {}; exec(open(sys.argv[1]).read(), cfg)
S = open(cfg['source']).read(); A = ndiff.listing(cfg['orig'], cfg['addr'])
def make(k):
    s = S
    for o, i in zip(cfg['options'], k):
        for a, b in o[i]:
            if a not in s: return None
            s = s.replace(a, b)
    return s
def score(k):
    s = make(k)
    if s is None: return -1, k, 'does not apply'
    n = '/tmp/var_' + '_'.join(map(str, k)); open(n + '.cpp', 'w').write(s)
    r = subprocess.run(['g++', *cfg['flags'], '-o', n, n + '.cpp', *cfg['link']], capture_output=True, text=True)
    if not os.path.exists(n): return -1, k, r.stderr[-300:]
    b = ndiff.listing(n, cfg['symbol']); os.remove(n)
    return round(difflib.SequenceMatcher(None, A, b, autojunk=False).ratio(), 5), k, len(b)
if __name__ == '__main__':
    best = tuple(0 for _ in cfg['options']); seen = {}
    with ProcessPoolExecutor(8) as e:
        for _ in range(2):
            before = best
            ks = [best] + [best[:x] + (i,) + best[x + 1:] for x, o in enumerate(cfg['options']) for i in range(len(o)) if i != best[x]]
            new = [k for k in ks if k not in seen]
            for r in e.map(score, new): seen[r[1]] = r
            base = seen[best][0]
            print('base', seen[best], 'target length', len(A))
            for k in ks[1:]: print('  ', seen[k])
            # every axis takes its best alternative, if together they beat the best single step
            pick = tuple(max(range(len(o)), key=lambda i, x=x: seen.get(best[:x] + (i,) + best[x + 1:], (-1,))[0]) for x, o in enumerate(cfg['options']))
            if pick not in seen: seen[pick] = score(pick)
            best = max(seen.values(), key=lambda r: r[0])[1]
            print('joined', seen[pick], 'best', seen[best])
            if best == before: break
    open('/out/best.cpp', 'w').write(make(best))
