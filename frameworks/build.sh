#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Build frameworks on the Mac VM (compiler and SDK stay there) and fetch the dylibs into out/.
# usage: build.sh Name...   (order matters: dependencies first)
set -e
here=$(cd "$(dirname "$0")" && pwd)
KEY=${MAC_KEY:-$HOME/fbsd-vm/id_ed25519}
SSH="ssh -p ${MAC_PORT:-2229} -i $KEY -o IdentitiesOnly=yes -o StrictHostKeyChecking=no ${MAC_USER:-alex}@localhost"
R=fwbuild
tar -C "$here" --exclude=out --exclude=.git -cf - . | $SSH "mkdir -p $R && cd $R && tar xf -"
for n in "$@"; do $SSH "sh $R/mac-build.sh $n"; done
mkdir -p "$here/out"
$SSH "cd $R/out && tar cf - lib mods" | tar -C "$here/out" -xf -
