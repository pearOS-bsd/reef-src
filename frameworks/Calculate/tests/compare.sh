#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Runs tests/sequences.txt against Apple's real Calculate.framework (Mac VM) and against ours (Reef, container
# calc-ui) and prints the differences. usage: compare.sh [sequence-file]   (needs build.sh Calculate first)
here=$(cd "$(dirname "$0")" && pwd)
seq=${1:-$here/sequences.txt}
set --
while IFS= read -r l; do [ -n "$l" ] && set -- "$@" "$l"; done < "$seq"
"$here/run-on-mac.sh" "$@" > /tmp/engine-real.txt 2>&1
fw="$here/../.."
st=$(mktemp -d); "$fw/install.sh" "$st" Calculate >/dev/null
docker cp "$st/." calc-ui:/tmp/calcroot/lib/cutefish-reef/
docker cp "$fw/out/enginetest" calc-ui:/tmp/enginetest
docker exec calc-ui chmod +x /tmp/enginetest
q=""; for a in "$@"; do q="$q '$a'"; done
docker exec calc-ui sh -c "HOME=/root /tmp/calcroot/bin/reef /tmp/enginetest $q" > /tmp/engine-ours.txt 2>&1
diff /tmp/engine-real.txt /tmp/engine-ours.txt > /tmp/engine-diff.txt && echo "all same" || { grep -c '^<' /tmp/engine-diff.txt | sed 's/$/ differing lines/'; }
