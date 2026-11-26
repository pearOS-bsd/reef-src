#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Regenerate the skeleton, type-check it on the Mac and learn from the errors until nothing new is learnt.
# usage: tools/iterate.sh [ROUNDS]   (writes Sources/ — only while Sources/ is still pure skeleton)
here=$(cd "$(dirname "$0")/.." && pwd)
cd "$here"
n=${1:-10}
while [ $n -gt 0 ]; do
  tools/regen.sh /tmp/swiftui-skeleton > /tmp/swiftui-gen.log 2>&1 || { cat /tmp/swiftui-gen.log; exit 1; }
  rm -rf Sources && cp -r /tmp/swiftui-skeleton Sources
  ./build.sh /tmp/swiftui-build typecheck > /dev/null 2>&1
  e=$(grep -c " error:" /tmp/swiftui-build/errors.txt)
  new=$(python3 tools/learn.py /tmp/swiftui-build/errors.txt)
  echo "round: $e errors, $new new facts"
  [ "$new" = 0 ] && break
  n=$((n-1))
done
