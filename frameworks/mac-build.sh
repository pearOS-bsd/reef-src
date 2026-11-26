#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Runs on the Mac VM, inside the uploaded frameworks/ tree: mac-build.sh Name
# Reads Name/build.cfg: KIND=swift|c|mixed, DEST=Frameworks|PrivateFrameworks,
# DEPS="Other ..." (our frameworks to link), SYSLIBS="-framework X ..." (Reef already has), EXTRA="flags".
set -e
cd "$(dirname "$0")"
N=$1
. ./$N/build.cfg
SDK=$(xcrun --show-sdk-path)
INSTALL=/System/Library/$DEST/$N.framework/Versions/A/$N
OUT=$PWD/out; mkdir -p $OUT/mods $OUT/lib
DEPFLAGS=""
for d in $DEPS; do DEPFLAGS="$DEPFLAGS $OUT/lib/$d.dylib"; done
CT="-target x86_64-apple-macos14.0"
CCT="$CT -isysroot $SDK"
LD="-Xlinker -install_name -Xlinker $INSTALL -Xlinker -current_version -Xlinker 1.0.0 -Xlinker -compatibility_version -Xlinker 1.0.0"
OBJS=""
mkdir -p $OUT/obj/$N
for f in $N/Sources/*.c $N/Sources/*.m $N/Sources/*.mm; do
    [ -f "$f" ] || continue
    clang $CCT -fobjc-arc -fPIC -O1 -I $N/Sources $EXTRA_C -c $f -o $OUT/obj/$N/$(basename $f).o
    OBJS="$OBJS $OUT/obj/$N/$(basename $f).o"
done
if ls $N/Sources/*.swift >/dev/null 2>&1; then
  swiftc $CT -sdk $SDK -emit-library -emit-module -module-name $N -parse-as-library -enable-library-evolution \
    -swift-version 5 -O -I $OUT/mods $EXTRA \
    -emit-module-path $OUT/mods/$N.swiftmodule -emit-module-interface-path $OUT/mods/$N.swiftinterface \
    $LD $SYSLIBS $DEPFLAGS $OBJS $N/Sources/*.swift -o $OUT/lib/$N.dylib
else
  clang $CCT -dynamiclib $LD $SYSLIBS $DEPFLAGS $OBJS -o $OUT/lib/$N.dylib
fi
echo "built $N -> $INSTALL"
