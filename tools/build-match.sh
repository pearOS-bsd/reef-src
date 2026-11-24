#!/bin/sh
# In the container: build and compare.  usage: build-match.sh ORIGINAL_USR SOURCE_DIR TOOLKIT_SRC TOOLKIT_LIB
set -e
H="-g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2 -Wdate-time"
cmake -S "$2" -B /tmp/b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr \
  -DCMAKE_C_FLAGS="$H" -DCMAKE_CXX_FLAGS="$H" -DTOOLKIT_SRC="$3" -DTOOLKIT_LIB="$4" -DCMAKE_EXE_LINKER_FLAGS=-Wl,-z,relro >/dev/null
ninja -C /tmp/b >/dev/null
s=0
while read -r orig ours min; do
  printf '%s: ' "$ours"; python3 "$2/tools/compare.py" --min "$min" "$1/$orig" "/tmp/b/$ours" || s=1
done < "$2/match/targets.txt"
exit $s
