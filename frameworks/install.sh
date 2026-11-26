#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Copy built frameworks (out/lib) into a Reef root (<dir of bin/reef>/../lib/cutefish-reef).
# usage: install.sh ROOT Name...   e.g. install.sh /tmp/calcroot/lib/cutefish-reef Calculate
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$1; shift
for n in "$@"; do
  ROOT_FILE=
  . "$here/$n/build.cfg"
  f="$root/${ROOT_FILE:-System/Library/$DEST/$n.framework/Versions/${VERSION:-A}/$n}"
  mkdir -p "$(dirname "$f")"
  cp -f "$here/out/lib/$n.dylib" "$f"
  echo "installed $n -> $f"
done
