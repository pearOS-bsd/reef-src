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
VERSION=${VERSION:-A}
INSTALL=${INSTALL_NAME:-/System/Library/$DEST/$N.framework/Versions/$VERSION/$N}
OUT=$PWD/out; mkdir -p $OUT/mods $OUT/lib $OUT/inc
DEPFLAGS=""
for d in $DEPS; do DEPFLAGS="$DEPFLAGS $OUT/lib/$d.dylib"; done
CT="-target x86_64-apple-macos14.0"
CCT="$CT -isysroot $SDK"
LD="-Xlinker -install_name -Xlinker $INSTALL -Xlinker -current_version -Xlinker 1.0.0 -Xlinker -compatibility_version -Xlinker 1.0.0"
OBJS=""
# Clang modules of our frameworks (Name/Sources/include/module.modulemap) are shared with every later build.
if [ -d $N/Sources/include ]; then mkdir -p $OUT/inc/$N; cp -R $N/Sources/include/. $OUT/inc/$N/; fi
CM=""
for m in $OUT/inc/*/module.modulemap; do [ -f "$m" ] && CM="$CM -Xcc -fmodule-map-file=$m -Xcc -I$(dirname $m)"; done
mkdir -p $OUT/obj/$N
for f in $N/Sources/*.c $N/Sources/*.m $N/Sources/*.mm; do
    [ -f "$f" ] || continue
    clang $CCT -fobjc-arc -fPIC -I $OUT/inc/$N -O1 -I $N/Sources $EXTRA_C -c $f -o $OUT/obj/$N/$(basename $f).o
    OBJS="$OBJS $OUT/obj/$N/$(basename $f).o"
done
if [ "$KIND" = wrapper ]; then
  # Support module whose symbols get Apple's names as aliases; Reef's original stays beside it as <name>.reef-orig and is re-exported.
  mkdir -p $OUT/obj/$N
  echo 'void reef_orig_stub(void){}' > $OUT/obj/$N/stub.c
  # SLOT_ALIASES="_apples=_orig ...": Apple's symbol answers with a symbol of Reef's original (named in the stub so ld accepts the alias)
  EXTRA_ALIAS=""
  for pair in $SLOT_ALIASES; do
    a=${pair%%=*}; o=${pair#*=}
    echo "char $(echo $o | sed 's/^_//') = 0;" >> $OUT/obj/$N/stub.c
    EXTRA_ALIAS="$EXTRA_ALIAS -Xlinker -alias -Xlinker $o -Xlinker $a"
  done
  clang $CCT -dynamiclib -install_name $INSTALL.reef-orig $OUT/obj/$N/stub.c -o $OUT/obj/$N/stub.dylib
  # linked under a temporary install name: ld ignores a dependency whose install name equals the output's
  LD="-Xlinker -install_name -Xlinker $INSTALL.wrapper"
  if ! ls $N/Sources/*.swift >/dev/null 2>&1; then
    # C / Objective-C only: the names are the same, no aliases needed
    clang $CCT -dynamiclib $LD -Xlinker -reexport_library -Xlinker $OUT/obj/$N/stub.dylib $SYSLIBS $DEPFLAGS $OBJS -o $OUT/lib/$N.dylib
    install_name_tool -id $INSTALL -change $INSTALL $INSTALL.reef-orig $OUT/lib/$N.dylib
    echo "built wrapper $N (C)"
    exit 0
  fi
  build_wrapper() {
    swiftc $CT -sdk $SDK -emit-library -emit-module -module-name $MODULE -parse-as-library -enable-library-evolution \
      -swift-version 5 -O -I $OUT/mods $CM $EXTRA \
      -emit-module-path $OUT/mods/$MODULE.swiftmodule \
      $LD -Xlinker -reexport_library -Xlinker $OUT/obj/$N/stub.dylib $SYSLIBS $DEPFLAGS $OBJS $N/Sources/*.swift "$@" -o $OUT/lib/$N.dylib
  }
  build_wrapper
  nm -gU $OUT/lib/$N.dylib > $OUT/obj/$N/nm.txt
  python3 ./alias.py $MODULE $APPLE_MODULE $N/apple-symbols.txt $OUT/obj/$N/nm.txt > $OUT/obj/$N/flags.txt
  build_wrapper $(cat $OUT/obj/$N/flags.txt) $EXTRA_ALIAS
  # references to the framework itself must reach the original, not the wrapper
  install_name_tool -id $INSTALL -change $INSTALL $INSTALL.reef-orig $OUT/lib/$N.dylib
  # libraries Reef folds into the original (SwiftUICore into SwiftUI): REDIRECT="from ..."
  for r in $REDIRECT; do install_name_tool -change $r ${REDIRECT_TO:-$INSTALL.reef-orig} $OUT/lib/$N.dylib; done
  echo "built wrapper $N ($(wc -l < $OUT/obj/$N/flags.txt) aliases)"
  exit 0
fi
if ls $N/Sources/*.swift >/dev/null 2>&1; then
  swiftc $CT -sdk $SDK -emit-library -emit-module -module-name $N -parse-as-library -enable-library-evolution \
    -swift-version 5 -O -I $OUT/mods $CM $EXTRA \
    -emit-module-path $OUT/mods/$N.swiftmodule \
    $LD $SYSLIBS $DEPFLAGS $OBJS $N/Sources/*.swift -o $OUT/lib/$N.dylib
else
  clang $CCT -dynamiclib $LD $SYSLIBS $DEPFLAGS $OBJS -o $OUT/lib/$N.dylib
fi
echo "built $N -> $INSTALL"
