#!/bin/sh
# usage: build.sh [NAME...]   builds apps/NAME (default all) on the Mac VM into ~/.pear/tmp/tk/swiftui/apps/NAME.app
# Sources: apps/NAME/*.swift + common/*.swift. Resources: apps/NAME/res/* (copied to Contents/Resources).
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
OUT=${SWIFTUI_APPS:-$HOME/.pear/tmp/tk/swiftui/apps}
SSH="ssh -p 2229 -o BatchMode=yes alex@localhost"
NAMES=${*:-$(ls "$HERE/apps")}
mkdir -p "$OUT"
$SSH 'rm -rf ~/swiftui-tests && mkdir -p ~/swiftui-tests'
tar -C "$HERE" -cf - apps common build-mac.sh | $SSH 'tar -C ~/swiftui-tests -xf -'
$SSH "cd ~/swiftui-tests && sh build-mac.sh $NAMES"
for n in $NAMES; do
  rm -rf "$OUT/$n.app"
  $SSH "cd ~/swiftui-tests/out && tar -cf - $n.app" | tar -C "$OUT" -xf - && echo "built $n"
done
