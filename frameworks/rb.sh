#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Install built frameworks into the writable Reef root of container rb and run reef there.
#   rb.sh install Name...     copy out/lib/Name.dylib into /tmp/calcroot
#   rb.sh missing             reef --list-missing on Calculator.app (summary: symbols per library)
#   rb.sh run [secs]          run Calculator headless
C=${RB:-rb}; ROOT=/tmp/calcroot/lib/cutefish-reef
here=$(cd "$(dirname "$0")" && pwd)
ENVV="LD_LIBRARY_PATH=/root2 HOME=/root XDG_RUNTIME_DIR=/tmp/xdg"
cmd=$1; shift
case $cmd in
install)
  st=$(mktemp -d); "$here/install.sh" "$st" "$@" && docker cp "$st/System" "$C:$ROOT/" && rm -rf "$st" ;;
missing)
  docker exec $C sh -c "$ENVV /tmp/calcroot/bin/reef --list-missing /root/Applications/Calculator.app" 2>&1 ;;
run)
  docker exec $C sh -c "$ENVV timeout ${1:-20} /tmp/calcroot/bin/reef /root/Applications/Calculator.app -ReefHeadless YES" 2>&1 ;;
esac
