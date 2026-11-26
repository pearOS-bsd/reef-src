#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Builds the engine test on the Mac VM, linked to out/lib/Calculate.dylib: its install name is Apple's, so on the Mac
# the test runs against Apple's real Calculate.framework. Then runs it there. usage: run-on-mac.sh [-base16] "1 + 2 =" ...
# Needs `build.sh Calculate` first. The binary stays in out/enginetest (run it under Reef for the other side).
here=$(cd "$(dirname "$0")" && pwd)
SSH="ssh -p 2229 -o IdentitiesOnly=yes -o BatchMode=yes alex@localhost"
$SSH 'mkdir -p fwbuild/enginetest'
scp -q -P 2229 -o BatchMode=yes "$here/main.swift" alex@localhost:fwbuild/enginetest/main.swift
$SSH 'cd fwbuild && swiftc -target x86_64-apple-macos14.0 -sdk $(xcrun --show-sdk-path) -I out/mods $(for m in out/inc/*/module.modulemap; do echo -Xcc -fmodule-map-file=$m; done) enginetest/main.swift -module-name enginetest out/lib/Calculate.dylib -o out/enginetest 2>&1 | grep -v "warning\|rpath" | head -20'
if [ "$1" != "--build-only" ]; then
  q=""; for a in "$@"; do q="$q '$a'"; done
  $SSH "cd fwbuild && out/enginetest $q"
fi
scp -q -P 2229 -o BatchMode=yes alex@localhost:fwbuild/out/enginetest "$here/../../out/enginetest" 2>/dev/null
