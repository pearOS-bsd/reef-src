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

## CoreFoundation 6.0: stages 1 to 3 (2026-11-27); a target in `macho/targets.txt` since stage 3 (minimum 60 %)
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

Reef's own CoreFoundation code (`macho/.../CoreFoundation.framework/reef/`, written from the binary and the older decompilation; the sizes of the functions match the original almost everywhere):
CFFileSecurity, CFFileDescriptor (two dispatch sources and a run loop source), CFMachPort (watcher thread, `0x52454546` terminate message), CFMessagePort (remote ports unsupported), CFNotificationCenter
(forwards to NSNotificationCenter when Foundation is loaded, else its own table; the Darwin center ignores object and user info), CFUserNotification (NSAlert on the main queue, else `org.freedesktop.Notifications`
through `_reefDBusCall`; keys `kCFUserNotification*Key`), CFStringTokenizer (ICU break iterator), CFXML escaping, CFURL resource properties and bookmarks (messages to NSURL) with FSRef tokens, bound stream pairs,
socket stream pairs (`__CFSocketStreamCreatePair`: a state machine over getaddrinfo, a non-blocking connect and TLS through the host's libssl.so.3 by `reef_host_symbol`), hyphenation (host libhyphen.so.0,
dictionaries in /usr/share/hyphen), the exception handler slot. `reef_host_symbol` and `reef_report_unsupported` are imports of the original (the loader of reef provides them), not definitions.
Per-file flavour found on the way: `CFFileUtilities.c`, `CFBundle_Executable.c`, `CFBundle_Locale.c` and `CFUtilities.c` build with `DEPLOYMENT_RUNTIME_OBJC=0` (XDG paths, FHS bundle directories).
2043 of 2443 functions match by name (69.4 % of the bytes, new functions are counted too).
Still missing: Reef's own changes inside upstream files (the autoupdating calendar and locale in CFCalendar.c and CFLocale.c, the host locale preferences `__CFLocaleReadHostLocked`, `__CFLocaleCopyFileValue`,
the file descriptor watching of CFRunLoop.c `__CFRunLoopWatchDescriptor`; they need a patch step in `build-macho.py`), the locale display names (`__reef*`, with their data tables), the NSInvocation forwarding
(`_reefForwarding*`, `_reefFrame*`, `__CFBridgeInitialize`, `__CFExceptionInitialize`), and the 926 Objective-C methods (`__NSCF*`, NSString, NSURL ...). Without the classes the rebuilt file does not run:
Stage 3 (links, two behaviour tests pass: `tests/cf.c` strings, tokenizer, notification center, bound pair; `tests/cfobjc.c` messages to CF objects through the `__NSCF*` classes, errors, enumerators):
- Patches of upstream files (`patches/NAME.patch`, `patches/include/NAME.patch` for headers, applied to a copy by `tools/build-macho.py`; the Apache-2.0 notice stays in them): CFRuntime (the `libdispatch_init` call
  becomes `__CFBridgeInitialize`; objects take the class of their type in `_CFRuntimeSetInstanceTypeIDAndIsa`; `__CFConstantStringClassReference` is the class `__NSCFConstantString` itself), CFBundle (framework search,
  bundle discovery from the loaded images), CFBundle_Resources (case-folded query table lookup), CFBundle_Locale (fallback languages without ICU), CFLocale/CFCalendar (autoupdating, host locale), CFRunLoop
  (`_CFRunLoopWatchDescriptor`), CFUtilities (version dictionary), CFStringEncodingDatabase (known encodings), CFError (typographic apostrophe).
- `reef/NSClasses.m`: the 61 Objective-C classes (`__NSCF*`, `NS*` that CF itself defines) with their ivars and methods, from the metadata (`tools/macho-gen-objc.py` draft) and written from the binary: the thin methods
  (one call of a CF function) are generated from the disassembly, the others by hand; no method is left as a stub. `reef/CFForwarding.c` + `reef/ReefForwarding.h`: the runtime side of NSMethodSignature and NSInvocation (type encoding
  parser, x86_64 argument layout, frames, the forwarding entry points and `reefFrameCall` as assembly, block forwarding); tests/cfinv.c and tests/cfmisc.c compare it with the original. `reef/CFReefMisc.c`: `__CFBridgeInitialize` (1024 type slots = `__NSCFType`, then the classes by type id), exception handlers.
- Per-file flags of the files in `reef/` are required (`cflags.reef/NAME.c`, one key per file): without them the constant strings take the Swift class.
- Still to do: the locale name tables and `__reef*` functions (`___reefLocaleNamesData`, 214 KB, ICU resource lookup), the encoding tables (`_MacJapaneseEntries` ... 700 KB) with `__reefTableConverter`, the `_os_log_fmt`
  statics of `os_log`, the data symbols `___NSArray0__`/`___NSDictionary0__` (+ `_struct`), libdispatch.

## libdispatch: what was found
`usr/lib/system/libdispatch.dylib` (636 functions) is built as `__APPLE__` (the Apple branch of `internal.h`: `os/availability.h`, `mach/*`, `mach_msg`, `mach_port_*`) with the epoll backend of
`event_config.h` (`__linux__` true there: `epoll_create1`, `eventfd`, `timerfd_create`, `signalfd`) and the ObjC classes `OS_dispatch_*` (`data.m`, `object.m`); it imports `kevent` and `kqueue` too.
The `config/config.h` of the repository is the Apple one. It needs the Apple private headers (`os/tsd.h`, `pthread/private.h`, `mach/*`, voucher, firehose), written from the symbols.

## The small C libraries and the shims with their own code (2026-11-27)
The host shims (`tools/macho-gen-shim.py`) are a table of jumps (`trampolines.S`), a constructor that fills the table from the host's library (`reef_host_symbol`) and `reef_bridge_missing`
for what the host lacks. The generator keeps the names with two leading underscores (`__xmlRaiseError`) and only rewrites `symbols.h` and `trampolines.S` when the folder exists. What the
binary has next to the table is Reef's own wrapper code, written in a second source file linked after the constructor (the original has zeros, not nops, between the two objects):
- `libsqlite3`: 292 of 292 functions. The variable argument functions (`sqlite3_mprintf`, `_snprintf`, `_str_appendf`, `_log`, `_config`, `_db_config`, `_test_control`, `_vtab_config`) cannot be
  jumps: the printf ones call the host's `v` variant with a `va_list`, `sqlite3_log` formats first and logs `"%s"`, the configuration ones copy 16 words from the caller's arguments and pass them on.
  Each looks the host's function up the first time (`static` pointer inside an inline helper `host_<name>`, which is what the symbols `host_sqlite3_vmprintf.function` are).
- `libcurl`: 101 of 102. Every handle gets a record (a list under a mutex): the write callback the caller set, the header callback and data. The defaults are Apple's stdio (`writeStream`
  fwrites to the stream or `stdout`, `readStream` freads from `stdin` and answers `CURL_READFUNC_ABORT` on an error), set for every new, reset and duplicated handle; `curl_easy_setopt`
  keeps the write function and header function/data (`applyHeader`), replaces Apple's `stderr` by the host's (`libc.so.6` `stderr`), and passes 16 words on; `curl_easy_getinfo`,
  `curl_multi_setopt`, `curl_share_setopt` pass 16 words on, `curl_formadd` 40; the `curl_m*printf` family formats with the host's `curl_mvaprintf` and writes through Apple's stdio.
- `libxml2`: 1572 of 1572. The global variables of the library (`xmlMalloc`, `xmlFree`, the SAX handlers, `xmlLastError`, the character tables ...) live in the shim and are copied from the host
  when the library is loaded and after `xmlMemSetup`/`xmlGcMemSetup` (`reef_xml2_refresh_data`); `-fno-common` with 16 byte alignment gives the same layout. The error functions format with
  `vasprintf` and pass `"%s"`; a function the host lacks stops the program with `reef: libxml2 lacks <name>`.
- Other libraries: `libsandbox` (byte for byte; compiles profiles through `reef_sandbox_compile` of libSystem), `libresolv` (14 of 17; the resolver state is read from `/etc/resolv.conf`,
  queries go to the host's resolver and its `h_errno` is copied; `names.c` is built with `-fwrapv`, which is what gives the original's sign extensions), `libcompression` (25 of 33; the stream
  API around the LZFSE reference code of `upstream/lzfse`; LZ4, LZMA and Brotli through the host's libraries, zlib through libz; the host table is one struct, `host`).
- Small frameworks: `ForceFeedback`, `CoreMediaIO`, `CoreDisplay`, `MediaAccessibility`, `GSS` (a fake GSS-API: names, OID sets, status texts, no credentials), `CoreMIDI` (clients and ports are
  numbers above 0x10000; the packet and event list helpers) match function for function except where noted in their `target.conf` (`minimum`).
- Two compiler findings: `static` helpers are placed in the file where they are first referenced, deferred ones last; a constant string used first in an inline helper lands in `__cstring`
  after the strings of the functions before it (the only reason `libsqlite3` and `libxml2` are not byte for byte: the position of `"%s"` and `""`). `-fno-common` globals are `.zerofill
  __DATA,__common` and are accessed directly, `-fcommon` ones through an address register.
Not done (they are real implementations, not wrappers): `libxar` (19 KB, XML parser and writer, CommonCrypto), the C frameworks `Carbon`, `ColorSync`, `VideoToolbox`, `Accelerate`
(vDSP, vImage), `ApplicationServices` (AX, HIShape, printing), `SystemConfiguration`, `CFNetwork`, `ImageIO`, `CoreMedia`, `libswiftsimd` (Swift).

## ObjC frameworks written by hand (small ones)
Each folder has the source, `target.conf` (`cflags = -fno-objc-arc` when the binary releases ivars itself, plus `-fobjc-weak` for weak ivars), a test under `tests/` and a line in `targets.txt`.
Habits of the compiler that decide the match: MRR files get the GOT calls `objc_release`, `objc_retain` and `objc_autorelease` (and `objc_alloc_init` for `[[X alloc] init]`) only when `NSObject` declares `release`, `retain` and `autorelease`
(declare them in a category in the file); nested messages in one statement load the receivers' class references first; `dispatch_once` needs the inline fast path of the SDK header; a program (`type = executable`) is tested with `testruns`.
A test that links AppKit explicitly or runs the initializer of an `NSPanel` subclass cannot run in the image (no host AppKit library): allocate without `init`.

## Libraries that the original package does not have (`macho/additions.txt`)
Apps import open-source libraries that macOS ships in `/usr/lib` and the original package lacks (libpcre, libxslt, libtidy, libedit, libncurses, libsasl2 ...).
They are built from the unchanged upstream source (`upstream/<name>`, a row in `upstream/PROVENANCE.md`) with Apple's install name and versions, as `macho/` targets, but are listed
in `macho/additions.txt` instead of `targets.txt` (there is no original file to compare with). `tests/additions.py` checks them: the exports against `exports.txt` (the list
`dyld_info -exports` shows for the library in the shared cache of macOS 26.7; `imports.txt` is what the apps import, all of it must be exported; `tools/check-exports.py`) and
every `tests/NAME.c` against `NAME.expected`, the output of the same program run on a Mac against Apple's library. `tools/make-deb.sh` adds the files to the package.
- libpcre.0: PCRE 8.44 as Apple's (`pcre_version()` is `8.44 2020-02-12`, `PCRE_CONFIG_JIT` 0: the JIT entry points exist and are stubs), all 31 exports.
