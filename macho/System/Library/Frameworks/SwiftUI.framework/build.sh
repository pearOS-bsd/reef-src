#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Build Reef's SwiftUI from Sources/ on the Mac VM (compiler and SDK stay there) and fetch the result into OUT.
# usage: build.sh [OUT] [typecheck|onone]     OUT defaults to ./out
#   typecheck  only type-check (fast; errors in OUT/errors.txt)
#   onone      build without optimisation (fast; for the symbol coverage)
# Environment: MAC_PORT (2229), MAC_USER (alex), MAC_KEY (~/fbsd-vm/id_ed25519), MAC_SDK (the macOS 15 SDK of the
# Command Line Tools: its AppKit does not import SwiftUICore, which would clash with a module named SwiftUI).
set -e
here=$(cd "$(dirname "$0")" && pwd)
OUT=${1:-$here/out}; MODE=${2:-release}
mkdir -p "$OUT"
KEY=${MAC_KEY:-$HOME/fbsd-vm/id_ed25519}
SSH="ssh -p ${MAC_PORT:-2229} -i $KEY -o IdentitiesOnly=yes -o BatchMode=yes -o StrictHostKeyChecking=no ${MAC_USER:-alex}@localhost"
SDK=${MAC_SDK:-/Library/Developer/CommandLineTools/SDKs/MacOSX15.sdk}
R=suire/build
$SSH "rm -rf $R/Sources && mkdir -p $R"
tar -C "$here" -cf - Sources | $SSH "cd $R && tar xf -"
INSTALL=/System/Library/Frameworks/SwiftUI.framework/Versions/A/SwiftUI
COMMON="-target x86_64-apple-macos14.0 -sdk $SDK -enable-library-evolution -module-name SwiftUI -parse-as-library -swift-version 5 \
 -Xfrontend -disable-availability-checking -suppress-warnings -enable-upcoming-feature MemberImportVisibility"
case $MODE in
typecheck)
  $SSH "cd $R && find Sources -name '*.swift' | sort > files.txt && swiftc $COMMON -typecheck -j4 -continue-building-after-errors @files.txt > errors.txt 2>&1; echo exit \$?" ;;
onone|release)
  OPT=-O; [ $MODE = onone ] && OPT=-Onone
  $SSH "cd $R && find Sources -name '*.swift' | sort > files.txt && rm -rf lib && mkdir -p lib && swiftc $COMMON $OPT -wmo -autolink-force-load -module-link-name SwiftUI -emit-library \
 -emit-module -emit-module-interface -no-verify-emitted-module-interface -emit-module-path lib/SwiftUI.swiftmodule -o lib/SwiftUI \
 -Xlinker -install_name -Xlinker $INSTALL -Xlinker -current_version -Xlinker 1.0.0 -Xlinker -compatibility_version -Xlinker 1.0.0 \
 -Xlinker -alias_list -Xlinker Sources/Support/aliases.txt \
 @files.txt > errors.txt 2>&1; echo exit \$?; ls -la lib/SwiftUI 2>/dev/null && nm -gU lib/SwiftUI | awk '{print \$3}' > lib/exports.txt" ;;
esac
$SSH "cat $R/errors.txt" > "$OUT/errors.txt"
rm -rf "$OUT/lib"; $SSH "cd $R && tar cf - lib 2>/dev/null" | tar -C "$OUT" -xf - 2>/dev/null || true
grep -c "error:" "$OUT/errors.txt" || true
