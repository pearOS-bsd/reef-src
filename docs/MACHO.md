# The Mach-O files (rebuilt from source)

199 of the 211 files of the original package are Mach-O (x86-64, linked by ld64.lld 19.1.7, minos 11.0, paths under `/work/reef/third_party/`).
The sources are in `macho/`, one folder per file, with a `target.conf` (see `tools/build-macho.py`).

## Toolchain (measured 2026-11-26)
`clang 19.1.7 --target=x86_64-apple-macos11.0 -O2`, no `-DNDEBUG` (the originals import `___assert_rtn`), no stack protector for the
code of lzfse, `ld64.lld` 19.1.7. Known-answer test: LZFSE (`upstream/lzfse`, public) is inside `libcompression.dylib`; built with
those flags and `-ffile-prefix-map=<checkout>/upstream/lzfse=/work/reef/third_party/lzfse` its 22 functions match 22 of 22
(24318 of 24318 bytes, `tools/compare-macho.py --present`). Controls: `-O0` 0.13 %, `-O1` 0.86 %, `-O3` 20.5 %, `-Os` 2.7 %, and with
`-fstack-protector-strong` 40 %. `__FILE__` in `assert` is the build path, so it matters.
Not shown by lzfse: the functions are hidden in the original (`-fvisibility=hidden`, calibrated on the shims, where the byte for byte
result confirms it).
The UUID of a file is a hash of its content and of its name: link under the final file name (`build-macho.py` does).

## Tools
- `tools/compare-macho.py ORIG OURS [-v] [--meta] [--present]`: functions (LC_FUNCTION_STARTS) paired by name, addresses replaced by what they
  refer to (import, symbol+offset, string, selector); `N/M functions, bytes = %`; `--meta` compares load commands, exports, imports, ObjC names and sections.
- `tools/build-macho.py ORIGINAL_ROOT OUT [FOLDER...]`: builds `macho/targets.txt` into `OUT/<install path>`; `tools/make-deb.sh` takes `BUILD_DIR/macho/...`.
- `tests/macho.py REEF ORIGINAL_ROOT OUT [FOLDER...]`: behaviour: `macho/<folder>/tests/*.c|m` are built against the original file and run by reef with
  the original root and with ours swapped in; output and exit code must be equal.
- `tools/check-macho.py REEF ORIGINAL_ROOT OUT`: the three together, one line per file.

## Upstream C++ (LLVM 19.1.7, `upstream/llvm`)
`libc++abi.dylib`: 647 of 647 functions, 118669 of 118669 bytes (the file differs only in the order of the LINKEDIT tables). Flags: `-O2 -std=c++20
-DNDEBUG -D_LIBCXXABI_FORGIVING_DYNAMIC_CAST`, `__config_site` in `macho/libcxx-config`. NDEBUG here, but not for lzfse: each project is built with
its own CMake build type.
`libc++.1.dylib`: 2116 of 2130 functions, 99.33 % of the bytes (`-O2 -std=c++23 -DNDEBUG`). Open: the filesystem functions that read directories
(`__dir_stream::advance`, `posix_readdir`, `__canonical`, `remove_all_impl`) and the `__time_get_c_storage` getters (a difference of data addresses).
The Darwin C headers in `macho/include` are written by hand; the details that changed code: errno values (Darwin), `intmax_t` = long, `MB_LEN_MAX` = 1,
`stat$INODE64`, `fopen$DARWIN_EXTSN` asm labels.
libunwind (inside `libSystem.B.dylib`): compiles from the same tree, 35 of 77 functions match with plain flags; Reef modified it and the version may differ: open.
CoreFoundation: the original has CFListFormatter, CFRelativeDateTimeFormatter, CFDateComponents and CFCalendar, which swift-corelibs-foundation has from
swift-6.0-RELEASE on: the version is 6.0, 6.1 or 6.2 (to be decided by function match). Tcl: the version string is 8.6.14 (not 8.5).

## Findings on the other upstream libraries (2026-11-26)
- `libdispatch.dylib` (swift-corelibs-libdispatch, version not yet decided; swift-6.0-RELEASE is the first guess): it is the Linux backend (imports `epoll_create1`, `eventfd`,
  `kevent`, `kqueue`) with the Objective-C classes `OS_dispatch_*` (src/data.m, object.m), linked for Mach-O. Building it needs the generated `config/config.h` of that
  port and the Darwin headers `Availability.h`, `TargetConditionals.h`, `os/*`, `mach/*`; the configuration Reef used is not known yet.
- `libxar.1.dylib` is not upstream xar: it uses CommonCrypto (`CC_MD5`, `CC_SHA*`), libcompression, zlib and bzip2, not libxml2 or OpenSSL. It is Reef's own
  code (19 KB): it goes with the small C libraries, written from the decompilation.
- `Tcl` says 8.6.14; not started (needs the Darwin `tclUnixPort.h` header set and the HAVE_* configuration).
