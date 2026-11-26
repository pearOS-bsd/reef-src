# Frameworks for Reef

Our own implementations of Apple frameworks (or of the symbols Reef's frameworks lack), so that
real Mac apps start on Reef. Everything is built on the Mac VM (compiler and SDK stay there; no Apple
file is copied into this repository) and installed into a Reef root.

Layout of a framework `Name/`:
- `Sources/*.swift|*.c|*.m` the code (`Sources/include/module.modulemap` makes a Clang module shared with later builds)
- `build.cfg` shell variables: `KIND` (`swift`, `c` or `wrapper`), `DEST` (`Frameworks` or `PrivateFrameworks`),
  `VERSION` (default `A`), `DEPS` (our frameworks to link), and for wrappers `MODULE`, `APPLE_MODULE`, `REDIRECT`,
  `REDIRECT_TO`, `SLOT_ALIASES`, `INSTALL_NAME`, `ROOT_FILE`.
- `apple-symbols.txt` (wrappers) the mangled names the app imports and Reef lacks (from `reef --list-missing`).

## Build
`./build.sh Name...` (dependencies first) uploads this tree to `~/fwbuild` on the Mac VM
(`ssh -p 2229`, key `~/fbsd-vm/id_ed25519`, always batch mode), runs `mac-build.sh` there
(`-target x86_64-apple-macos14.0`, library evolution, Apple's install name) and fetches `out/lib/Name.dylib`.

## Install and test
`./install.sh ROOT Name...` copies into a Reef root (`<folder of bin/reef>/../lib/cutefish-reef`).
For the test container: `./rb.sh install Name...`, `./rb.sh missing` (`reef --list-missing` on Calculator),
`./rb.sh run [secs]`. `reef --stub-missing=zero` lists every symbol our code needs that Reef lacks.

## Wrapper technique (symbols that belong to a framework Reef already has)
The code lives in a support module of ours (`FoundationSupport`, `SwiftUISupport`, ...). The wrapper dylib has
Apple's install name and re-exports Reef's original, which stays beside it as `<name>.reef-orig`
(`rb.sh install` keeps it). `alias.py` demangles Apple's names and our symbols, drops `(extension in X):`,
renames the support module to Apple's and emits `-alias _ours _apples` for each match (misses are printed).
The aliases cover functions, types, descriptors and opaque result descriptors. References to the framework itself
(and `REDIRECT`ed libraries such as SwiftUICore, which Reef folds into SwiftUI) are rewritten to the original.
Rules: the generic signature (requirements) of our declaration must be Apple's, or the calling convention differs;
a protocol requirement Reef's protocol lacks cannot be added (see `SLOT_ALIASES`).
