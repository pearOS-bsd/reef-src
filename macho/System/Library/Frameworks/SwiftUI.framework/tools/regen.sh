#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Regenerate the skeleton into OUT (default: a scratch folder; Sources/ holds the code being written, never overwrite it blindly).
set -e
here=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$here/../../../../../.." && pwd)
BIN=${REEF_SWIFTUI:-$ROOT/upstream/cutefish-reef/usr/lib/cutefish-reef/System/Library/Frameworks/SwiftUI.framework/Versions/A/SwiftUI}
OUT=${1:-/tmp/swiftui-skeleton}
rm -rf "$OUT"
python3 "$here/gen-skeleton.py" "$BIN" "$OUT"
python3 "$here/postfix.py" "$OUT"
