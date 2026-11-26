#!/bin/bash
# Runs on the Mac VM: build-mac.sh NAME...   (up to 4 builds in parallel; only errors are shown)
cd "$(dirname "$0")"; mkdir -p out
build() {
  n=$1; app=out/$n.app; rm -rf $app; mkdir -p $app/Contents/MacOS $app/Contents/Resources
  swiftc -O -swift-version 5 -target x86_64-apple-macos14.0 common/*.swift apps/$n/*.swift -o $app/Contents/MacOS/$n >out/$n.log 2>&1 || { echo "FAIL $n"; grep -A3 'error:' out/$n.log | head -30; rm -rf $app; return; }
  [ -d apps/$n/res ] && cp -R apps/$n/res/. $app/Contents/Resources/
  cat > $app/Contents/Info.plist <<PL
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>$n</string>
<key>CFBundleIdentifier</key><string>pear.swiftuitest.$n</string>
<key>CFBundleName</key><string>$n</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>CFBundleShortVersionString</key><string>1.0</string>
<key>CFBundleVersion</key><string>1</string>
<key>LSMinimumSystemVersion</key><string>14.0</string>
<key>NSPrincipalClass</key><string>NSApplication</string>
<key>NSHighResolutionCapable</key><true/>
</dict></plist>
PL
  codesign -s - -f $app >/dev/null 2>&1 || true
  echo "ok $n"
}
export -f build 2>/dev/null
for n in "$@"; do
  build $n &
  while [ $(jobs -r | wc -l) -ge 4 ]; do sleep 1; done
done
wait
