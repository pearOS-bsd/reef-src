#!/usr/bin/env python3
"""Compare every file of match/targets.txt with the one built here and print the total of Reef's ELF files.
usage: total.py ORIGINAL_USR BUILD_DIR TARGETS
A line of TARGETS is: original file, ours (- when not started), percent of the code bytes that must match.
A file that is not built here counts with all of its bytes and no match; the status is 1 when a file has less than its minimum."""
import os, sys
sys.dont_write_bytecode = True
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import compare

usr, build, targets = sys.argv[1:4]
status = 0
functions = [0, 0]
written = 0
bytes_ = [0, 0]
for line in open(targets):
    original, ours, least = line.split()
    A = compare.Image(os.path.join(usr, original))
    size = sum(A.funcs.values())
    functions[1] += len(A.funcs)
    bytes_[1] += size
    name = os.path.basename(original)
    if ours != '-':
        written += len(A.funcs)
    path = os.path.join(build, ours)
    if ours == '-' or not os.path.exists(path):
        print('%s: %s, %d functions, %d bytes' % (name, 'not started' if ours == '-' else 'not built here', len(A.funcs), size), flush=True)
        continue
    match, waiting = compare.match(A, compare.Image(path))
    same = sum(A.funcs[f] for f in match)
    functions[0] += len(match)
    bytes_[0] += same
    print('%s: %s' % (name, compare.summary(A, match, waiting)), flush=True)
    if same * 100 < float(least) * size:
        print('%s: less than %s %%' % (name, least))
        status = 1
print('written: %d/%d functions = %.2f%%' % (written, functions[1], 100 * written / functions[1]))
print('total: %d/%d functions, %d/%d bytes = %.2f%%' % (functions[0], functions[1], bytes_[0], bytes_[1], 100 * bytes_[0] / bytes_[1]))
sys.exit(status)
