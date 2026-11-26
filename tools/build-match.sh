#!/bin/sh
# In the container: build and compare.  usage: build-match.sh ORIGINAL_USR SOURCE_DIR TOOLKIT_SRC TOOLKIT_LIB [WEBKIT_INCLUDE [OUT_DIR]]
# OUT_DIR: the 12 built files are copied there (for tools/make-deb.sh).
set -e
H="-g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2 -Wdate-time"
cmake -S "$2" -B /tmp/b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr \
  -DCMAKE_C_FLAGS="$H" -DCMAKE_CXX_FLAGS="$H" -DTOOLKIT_SRC="$3" -DTOOLKIT_LIB="$4" -DWEBKIT_INCLUDE="$5" -DCMAKE_EXE_LINKER_FLAGS=-Wl,-z,relro >/dev/null
ninja -C /tmp/b >/tmp/ninja.log 2>&1 || { tail -60 /tmp/ninja.log; exit 1; }
python3 "$2/tools/total.py" "$1" /tmp/b "$2/match/targets.txt"
if [ -n "$6" ]; then
    mkdir -p "$6"
    awk '$2 != "-" {print "/tmp/b/" $2}' "$2/match/targets.txt" | xargs cp -t "$6"
fi
