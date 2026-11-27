#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
"""Last fixes of the generated skeleton that the binary does not tell the generator (one-offs found by the compiler).

usage: postfix.py OUTDIR   (run by regen.sh after gen-skeleton.py)"""
import os, sys

# (file, text, replacement)
EDITS = [
    # NSView.needsDisplay is settable: an override must be too
    ('Drawing/ElementCanvas.swift', '    var needsDisplay: Swift.Bool { _skeleton() }',
     '    override var needsDisplay: Swift.Bool {\n        get { _skeleton() }\n        set { _skeleton() }\n    }'),
    # a witness of a public protocol's associated type in a conformance of a public type
    ('Extensions/__C_NSLayoutPriority+SwiftUI.swift', '    typealias _ObjectiveCType = NSNumber',
     '    public typealias _ObjectiveCType = NSNumber'),
]

# declarations of things the optimiser removed from the binary but that are still named by others
APPEND = {
    # a private protocol: only its field descriptor (`...FadeLayer...LL_pMF`) is left
    'Support/TypesWithoutMetadata.swift': 'protocol FadeLayer {}\n',
}


def main():
    out = sys.argv[1]
    bad = 0
    for f, a, b in EDITS:
        p = os.path.join(out, f)
        s = open(p).read()
        if a not in s:
            print('postfix: not found in %s: %s' % (f, a.strip()), file=sys.stderr); bad += 1; continue
        open(p, 'w').write(s.replace(a, b, 1))
    for f, text in APPEND.items():
        with open(os.path.join(out, f), 'a') as fh: fh.write(text)
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
