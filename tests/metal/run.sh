#!/bin/sh
# The Metal translator: every shader of shaders/ through the C interface of the original and of ours (each next to the same MSL
# compiler library, with an empty home, so that nothing comes from the cache), then once more on the cache each wrote; the
# output must be the same.  usage: run.sh ORIGINAL_TRANSLATOR OURS [MSL_LIBRARY]
here=$(cd "$(dirname "$0")" && pwd)
original=$1 ours=$2 msl=${3:-$(dirname "$1")/libreef-metal-msl.so}
work=$(mktemp -d)
g++ -O1 -std=c++20 -o "$work/drive" "$here/drive.cpp" -ldl || exit 1
for side in original ours; do
    mkdir -p "$work/$side/lib" "$work/$side/home"
    eval "cp \"\$$side\" \"$work/$side/lib/libreef-metal-translator.so\""
    ln -s "$msl" "$work/$side/lib/libreef-metal-msl.so"
done
failed=0
for run in 1 2; do
    for side in original ours; do
        HOME="$work/$side/home" CFFIXED_USER_HOME="$work/$side/home" timeout 1200 "$work/drive" "$work/$side/lib/libreef-metal-translator.so" \
            -o "$here"/shaders/*.metal >"$work/$side.txt" 2>&1
        echo "exit $?" >>"$work/$side.txt"
    done
    if cmp -s "$work/original.txt" "$work/ours.txt"; then
        echo "run $run: the same, $(wc -l <"$work/original.txt") lines"
    else
        echo "run $run: differs"
        diff "$work/original.txt" "$work/ours.txt" | head -20
        failed=1
    fi
done
rm -rf "$work"
exit $failed
