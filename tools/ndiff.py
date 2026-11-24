#!/usr/bin/env python3
"""Normalised listing of one function, and a diff of two.
usage: ndiff.py ORIG ADDR OURS ADDR|SYMBOL [-s]     (-s: similarity only)
Registers stay; stack offsets, jump targets and addresses are masked, imports are named, padding is dropped."""
import difflib, subprocess, sys, re
sys.path.insert(0, __file__.rsplit('/', 1)[0])
import compare
def listing(path, where):
    im = compare.Image(path)
    a = int(where, 16) if re.fullmatch('[0-9a-f]+', where) else im.symbols.get(where) or next(v for k, v in im.symbols.items() if where in k and v in im.funcs)
    out = []
    for i in im.md.disasm(im.data(a, im.funcs[a]), a):
        if i.mnemonic in ('nop', 'data16') or 'nop' in i.mnemonic or i.mnemonic == 'xchg' and i.op_str == 'ax, ax': continue
        op = i.op_str
        if i.group(compare.capstone.CS_GRP_JUMP) and not 'rip' in op: op = im.names.get(i.operands[0].imm, 'L') if i.operands[0].type == 2 else op
        elif i.group(compare.capstone.CS_GRP_CALL) and i.operands[0].type == 2:
            t = i.operands[0].imm; op = im.names.get(t) or ('F%d' % im.funcs.get(t, 0))
        elif 'rip' in op:
            for o in i.operands:
                if o.type == 3 and o.mem.base == compare.x86.X86_REG_RIP:
                    r, _ = im.refer(i.address + i.size + o.mem.disp, a, im.funcs[a], 0 if i.mnemonic == 'lea' else o.size)
                    op = re.sub(r'\[rip.*\]', lambda _: str(r), op)
        op = re.sub(r'\[rsp \+ (0x)?[0-9a-f]+\]|\[rsp\]', '[S]', op)
        out.append(i.mnemonic + ' ' + op)
    return out
if __name__ == '__main__':
    a, b = listing(*sys.argv[1:3]), listing(*sys.argv[3:5])
    print('%.4f %d %d' % (difflib.SequenceMatcher(None, a, b, autojunk=False).ratio(), len(a), len(b)))
    if '-s' not in sys.argv: print('\n'.join(difflib.unified_diff(a, b, lineterm='', n=2)))
