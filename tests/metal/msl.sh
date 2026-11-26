#!/bin/sh
# The Metal compiler library: every source of msl/ compiled by the original and by ours (each with its options in a first
# line "//opt ..."); the modules (LLVM text, verified, bitcode hash) and the errors must be the same.
# usage: msl.sh ORIGINAL OURS
here=$(cd "$(dirname "$0")" && pwd)
work=$(mktemp -d)
g++ -std=c++17 -O1 -w -I/usr/lib/llvm-19/include -o "$work/drive" "$here/msl/drive.cpp" /usr/lib/llvm-19/lib/libLLVM.so.19.1 -ldl || exit 1
same=0 failed=0
for source in "$here"/msl/*.metal; do
    BC=1 timeout 120 "$work/drive" "$1" "$source" >"$work/original" 2>&1
    a=$?
    BC=1 timeout 120 "$work/drive" "$2" "$source" >"$work/ours" 2>&1
    b=$?
    if [ $a = $b ] && cmp -s "$work/original" "$work/ours"; then
        same=$((same + 1))
    else
        failed=$((failed + 1))
        echo "$(basename "$source"): differs (exit $a and $b)"
        diff "$work/original" "$work/ours" | head -6
    fi
done
echo "Metal compiler: $same sources the same, $failed differ"
rm -rf "$work"
[ $failed = 0 ]
