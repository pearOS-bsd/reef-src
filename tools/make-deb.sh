#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandru Balan
# SPDX-License-Identifier: BSD-2-Clause
# Package open-reef: the files of the original package kept in upstream/cutefish-reef, with the 12 ELF programs and libraries
# built from this repository (match/targets.txt: path under usr, name in the build folder). open-reef conflicts with,
# replaces and provides the original package, so packages that depend on it stay satisfied.
# usage: make-deb.sh BUILD_DIR OUT_DIR [COMMIT]   (COMMIT: what BUILD_DIR was built from, HEAD by default)
set -e
build=$1 out=$2
here=$(cd "$(dirname "$0")/.." && pwd)
base=$here/upstream/cutefish-reef
commit=$(git -C "$here" rev-parse --short "${3:-HEAD}")
version=0.1~git$(git -C "$here" log -1 --format=%cd --date=format:%Y%m%d%H%M%S "$commit").$commit
field() { sed -n "s/^$1: //p" "$base/control"; }
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir "$work/pkg"
cp -a "$base/usr" "$work/pkg/usr"
while read -r path name minimum; do
    [ "$name" = "-" ] && continue
    [ -f "$build/$name" ] || { echo "make-deb.sh: $build/$name is missing" >&2; exit 1; }
    mkdir -p "$(dirname "$work/pkg/usr/$path")"
    install -m 755 "$build/$name" "$work/pkg/usr/$path"
    # As dh_strip does: no debug information, no .comment and .note sections.
    case $name in
        *.so) strip --remove-section=.comment --remove-section=.note --strip-unneeded "$work/pkg/usr/$path" ;;
        *) strip --remove-section=.comment --remove-section=.note "$work/pkg/usr/$path" ;;
    esac
done < "$here/match/targets.txt"
# The rebuilt Mach-O files (tools/build-macho.py): BUILD_DIR/macho/<path under lib/cutefish-reef>; each replaces the file of the original
# at that path and keeps its mode.
macho=0
if [ -d "$build/macho" ]; then
    for file in $(cd "$build/macho" && find . -type f | sort); do
        dest=$work/pkg/usr/lib/cutefish-reef/${file#./}
        if [ ! -e "$dest" ] && grep -qsx "install = ${file#./}" $(sed 's/#.*//' "$here/macho/additions.txt" | sed "s|^\(.\+\)|$here/macho/\1/target.conf|"); then
            mkdir -p "$(dirname "$dest")" && : > "$dest"      # a file that the original package does not have (macho/additions.txt)
        fi
        [ -f "$dest" ] && [ ! -L "$dest" ] || { echo "make-deb.sh: $dest is not a file of the original package" >&2; exit 1; }
        cat "$build/macho/$file" > "$dest"
        macho=$((macho + 1))
    done
fi
# The modes of the package, whatever the umask of the checkout: folders 755, files 644 or 755.
chmod -R u+rwX,go+rX,go-w "$work/pkg"
mkdir "$work/pkg/DEBIAN"
(cd "$work/pkg" && find usr -type f -print0 | sort -z | xargs -0 md5sum > DEBIAN/md5sums)
cat > "$work/pkg/DEBIAN/control" <<CONTROL
Package: open-reef
Version: $version
Architecture: $(field Architecture)
Maintainer: $(git -C "$here" log -1 --format='%an <%ae>' "$commit")
Installed-Size: $(du -s --apparent-size -k "$work/pkg/usr" | cut -f1)
Depends: $(field Depends), libedit2, libsasl2-2, libncursesw6
Conflicts: $(field Package)
Replaces: $(field Package)
Provides: $(field Package) (= $(field Version))
Section: $(field Section)
Priority: optional
Description: Mac app runtime, its programs and libraries built from source
 The files of $(field Package) $(field Version), with its 12 ELF programs and
 libraries and $macho of its Mach-O files built from reef-src $commit.
 The other Mac frameworks are unchanged.
CONTROL
mkdir -p "$out"
dpkg-deb --root-owner-group -Zxz --build "$work/pkg" "$out/open-reef_${version}_amd64.deb" >/dev/null
echo "$out/open-reef_${version}_amd64.deb"
