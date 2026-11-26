#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# usage: check.sh Name SYMBOLS.txt : symbols in SYMBOLS.txt (mangled, no leading underscore) that out/lib/Name.dylib does not export.
here=$(cd "$(dirname "$0")" && pwd)
llvm-nm -g --defined-only "$here/out/lib/$1.dylib" | awk '{print $3}' | sort > /tmp/check-have.$$
sed 's/^/_/' "$2" | sort | comm -23 - /tmp/check-have.$$; rm -f /tmp/check-have.$$
