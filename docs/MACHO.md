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
swift-6.0-RELEASE on. The version is 6.0: the original has the misspelt `_CFLocaleCreateLocaleIdentiferByReplacingLanguageCodeAndScriptCode` that 6.1 corrected, and not
`_CFPosixSpawnFileActionsChdir` that 6.1 added.

## Findings on the other upstream libraries (2026-11-26)
- `libdispatch.dylib` (swift-corelibs-libdispatch, version not yet decided; swift-6.0-RELEASE is the first guess): it is the Linux backend (imports `epoll_create1`, `eventfd`,
  `kevent`, `kqueue`) with the Objective-C classes `OS_dispatch_*` (src/data.m, object.m), linked for Mach-O. Building it needs the generated `config/config.h` of that
  port and the Darwin headers `Availability.h`, `TargetConditionals.h`, `os/*`, `mach/*`; the configuration Reef used is not known yet.
- `libxar.1.dylib` is not upstream xar: it uses CommonCrypto (`CC_MD5`, `CC_SHA*`), libcompression, zlib and bzip2, not libxml2 or OpenSSL. It is Reef's own
  code (19 KB): it goes with the small C libraries, written from the decompilation.

## Tcl 8.6.14 (`upstream/tcl`, tag core-8-6-14, unchanged)
`System/Library/Frameworks/Tcl.framework`: 2510 of 2510 functions, 1228705 of 1228705 bytes. No build files of Reef exist, so the configuration was read from the binary
(what it imports, its strings) and checked with the function match: threads (`USE_THREAD_ALLOC`, `-D_REENTRANT`), `-DNDEBUG` (no `___assert_rtn` import), `-DBUILD_tcl`
(noreturn on `Tcl_Exit`), `MODULE_SCOPE` hidden, zlib and libtommath, `dlopen` (`tclLoadDl.c`), no CoreFoundation, no DTrace, no `cpuid`, `TCL_CFGVAL_ENCODING` utf-8,
default encoding iso8859-1, `TCL_SHLIB_EXT` .dylib, `getpwnam_r` and friends with 5 arguments, `gethostbyname` without the MT-safe flag, `fts`, `mkstemps`, `nl_langinfo`.
The sources are linked in the ASCII order of their paths (`generic/`, `libtommath/`, `unix/`); `tclTest*`, `tclUnixTest`, `tclStubLib*`, `tclLoadNone` and `tclXtNotify` are not in the binary.
Header facts that changed code (`macho/include`): Darwin `WIFSTOPPED` (`WSTOPSIG != 0x13`), `P_tmpdir` = `/var/tmp/`, `EAI_SYSTEM` 11, `IPV6_V6ONLY` 27, `O_NOCTTY`, `select` without
`$1050`, `realpath$DARWIN_EXTSN`, `htons` as an inline byte swap. Test: 47 scripts through the bytecode compiler, bignums, regexps, zlib, TclOO, files, `exec`, sockets and the event loop.
(`open {|cat} r+` hangs under reef with the original too; it is not in the test.)

## CoreFoundation 6.0: first stage (2026-11-27), not yet a target in `macho/targets.txt`
`upstream/corefoundation` (swift-corelibs-foundation `swift-6.0-RELEASE`, Sources/CoreFoundation) and `upstream/icu` (swift-foundation-icu ICU 74 headers) are unchanged; `upstream/libdispatch`
holds the 6.0 headers (the file itself comes later). `macho/System/Library/Frameworks/CoreFoundation.framework` builds all 89 sources with the configuration read from the binary and
links with `-undefined dynamic_lookup`: 1948 of 2258 functions that exist in both match, 70.7 % of those bytes (`tools/compare-macho.py --present`).
What the function match showed:
- Mach-O, not Linux, but with Linux parts: `-U__APPLE__ -D__linux__ -D__unix__` (CF takes its Linux branches: it defines `OSAtomic*`, `_CFThreadRef`, `/etc/localtime`), `Boolean` is `unsigned char`
  (the Mac `MacTypes.h`, `__MACTYPES__`), `-fno-common -fexceptions -fblocks -fconstant-cfstrings`, constant strings are the builtin (`__cfstring`), the source path in asserts is
  `/work/reef/third_party/corefoundation/` (`-ffile-prefix-map`).
- The runtime flavour is chosen per source file (`cflags.NAME` in `target.conf`, picked by the function match): `DEPLOYMENT_RUNTIME_OBJC=1` (B) for most, `0` for CFBundle*, CFDate*,
  CFLocale*, CFTimeZone ...; `DEPLOYMENT_RUNTIME_SWIFT=1` (A) for CFURL's property keys.
- With `OBJC=1` the Mac build dispatches to Objective-C objects (tagged pointer or foreign isa: `objc_msgSend`); the 6.0 sources compile that to nothing, so `macho/.../internalInclude/CFInternal.h`
  puts it back (`CF_IS_OBJC`, `CF_OBJC_FUNCDISPATCHV`, `CF_OBJC_CALLV`, the constant string class excluded for CFString) and `tools/macho-gen-cf-objc.py` writes the Foundation declarations
  the calls need (`include/Foundation/NSDispatch.h`; BOOL return for Boolean).
- Names that share an address in the original (`NSLocaleCalendar` = `kCFLocaleCalendar` ...) are `.set` aliases in `aliases/<File>.h`, included per file (`tools/macho-gen-aliases.py` finds them).
- Every source is compiled as Objective-C (`-x objective-c`; the block descriptors carry the extended layout then), except CFURLAccess.c. CFRunLoop takes its Linux branch (`epoll`, `timerfd`, `ppoll`) with
  the dispatch timers of `DEPLOYMENT_RUNTIME_OBJC`; `AbsoluteTime` and `<dispatch/private.h>` are Reef's headers. Names that the function match flags as differing only in the use of `kCF...` or `NS...`
  constants are the same symbol (aliases) or a GOT access to a symbol of a file that is not in this image yet.
- Missing from swift-corelibs and so Reef's own (to be reconstructed from the binary and `~/Desktop/Macos_Test/decompiled`): about 145 C functions (CFMachPort, CFMessagePort, CFFileDescriptor,
  CFNotificationCenter, CFUserNotification, CFURL bookmarks and FSRef, CFXML, CFStringTokenizer, `OSAtomic*` ones that CF does not define, `__CFBridgeInitialize`, ...) and about 1000 Objective-C
  methods (`__NSCF*`, `NSString`, `NSURL`, `NSNumber`, `NSInvocation`, `NSCache`, `NSRunLoop` ...). Reef's CF also imports `reef_host_symbol` and `reef_report_unsupported`.

Reef's own CoreFoundation code (`macho/.../CoreFoundation.framework/reef/`, written from the binary and the older decompilation; sizes of the functions match the original almost everywhere):
CFFileSecurity, CFFileDescriptor (two dispatch sources and a run loop source), CFMachPort (watcher thread, `0x52454546` terminate message), CFMessagePort (remote ports unsupported), CFNotificationCenter
(forwards to NSNotificationCenter when Foundation is loaded, else its own table; Darwin center ignores object and user info), CFUserNotification (NSAlert on the main queue, else `org.freedesktop.Notifications`
through `_reefDBusCall`), CFStringTokenizer (ICU break iterator), CFXML escaping, CFURL resource properties and bookmarks (messages to NSURL) with FSRef tokens, bound stream pairs, exception handler slot.
`CFFileUtilities.c` builds with `DEPLOYMENT_RUNTIME_OBJC=0` (that is what has the XDG paths). Still missing (see the list printed by `llvm-nm` difference): the socket stream pair and its TLS glue (`_advance`,
`_connectNext`, `_resolveSSL` ...), hyphenation (`_dictionaryFor`), the locale display names (`__reef*`), the NSInvocation forwarding (`_reefForwarding*`, `_reefFrame*`), bundle and locale odds and ends,
and the Objective-C classes. The function match dropped a little (69.5 %) because new functions are counted.

## libdispatch: what was found
`usr/lib/system/libdispatch.dylib` (636 functions) is built as `__APPLE__` (the Apple branch of `internal.h`: `os/availability.h`, `mach/*`, `mach_msg`, `mach_port_*`) with the epoll backend of
`event_config.h` (`__linux__` true there: `epoll_create1`, `eventfd`, `timerfd_create`, `signalfd`) and the ObjC classes `OS_dispatch_*` (`data.m`, `object.m`); it imports `kevent` and `kqueue` too.
The `config/config.h` of the repository is the Apple one. It needs the Apple private headers (`os/tsd.h`, `pthread/private.h`, `mach/*`, voucher, firehose), written from the symbols.
