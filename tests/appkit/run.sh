#!/bin/sh
# The AppKit host without a display: the drivers of this folder with the original and ours, the logs must be the same.
# usage: run.sh ORIGINAL OURS TOOLKIT_SRC TOOLKIT_LIB [SEEDS]
here=$(cd "$(dirname "$0")" && pwd)
original=$1 ours=$2 toolkit=$3 seeds=${5:-30}
export LD_LIBRARY_PATH="$4"
work=$(mktemp -d)
gcc -shared -fPIC -O2 -o "$work/clock.so" "$here/clock.c" || exit 1
failed=0
for name in graphics windows gl; do
    g++ -std=c++20 -O1 -w -I"$here/../../src/appkit-host" -I"$toolkit/src" -o "$work/$name" "$here/$name.cpp" -ldl -lEGL || exit 1
done
both() { # NAME ARGUMENTS...
    name=$1
    shift
    for side in original ours; do
        eval "library=\$$side"
        LD_PRELOAD="$work/clock.so" timeout 300 "$work/$name" "$library" "$@" >"$work/$side" 2>&1
        echo "exit $?" >>"$work/$side"
    done
    if cmp -s "$work/original" "$work/ours"; then
        same=$((same + 1))
    else
        failed=$((failed + 1))
        echo "$name $*: differs"
        diff "$work/original" "$work/ours" | head -6
    fi
}
same=0
both graphics $((seeds * 5)) 1
seed=1
while [ $seed -le "$seeds" ]; do
    both windows 400 $seed
    both gl 100 $seed
    seed=$((seed + 1))
done
echo "AppKit host: $same runs the same, $failed differ"
rm -r "$work"
[ $failed = 0 ]
