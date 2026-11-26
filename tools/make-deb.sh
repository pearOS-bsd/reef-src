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
Depends: $(field Depends)
Conflicts: $(field Package)
Replaces: $(field Package)
Provides: $(field Package) (= $(field Version))
Section: $(field Section)
Priority: optional
Description: Mac app runtime, its programs and libraries built from source
 The files of $(field Package) $(field Version), with its 12 ELF programs and
 libraries built from reef-src $commit. The Mac frameworks are unchanged.
CONTROL
mkdir -p "$out"
dpkg-deb --root-owner-group -Zxz --build "$work/pkg" "$out/open-reef_${version}_amd64.deb" >/dev/null
echo "$out/open-reef_${version}_amd64.deb"
