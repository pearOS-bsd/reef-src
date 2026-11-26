# apple-trace

Debug aids for observing the real Calculator (or any app) on the Mac VM. Not part of the frameworks.
Copy the app to a user folder, re-sign it ad hoc (`codesign --force --deep -s -`) and run it with
`DYLD_INSERT_LIBRARIES`.

- `ilog.swift` + `tab.c`: interpose `LayoutSubview.sizeThatFits` and log proposal and answer.
  Build: `clang -c tab.c -o tab.o; swiftc -target x86_64-apple-macos14.0 -emit-library -module-name ilog -parse-as-library ilog.swift tab.o -o ilog.dylib`
- `dump.m`: after 4 s prints every window (frame, content min/max, view tree).
  Build: `clang -dynamiclib -fobjc-arc -framework AppKit dump.m -o dump.dylib`
