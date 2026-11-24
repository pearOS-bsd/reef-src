# Toolchain (measured 2026-11-25)
**Two profiles.** The toolkit is built with build type None (`-O2`); the target programs with Release on top of the same
Debian flags, so `-O3 -DNDEBUG` wins. Evidence: `reef-open`, same source, 50 of 78 functions (15.57 %) with `-O2`,
71 of 78 (40.45 %) with `-O3`. `login` and `reef-authhelper` match with both, so they do not tell the profiles apart.
Below is the toolkit's profile.

Debian trixie, GCC 14.2.0-19, GNU ld 2.44, flags of `dpkg-buildflags` plus CMake build type None, prefix `/usr`:
`-g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2`.

Known-answer test (`tools/calibrate.sh`): cutefish-toolkit commit 111537f is public and its Debian package is available.
`libcutefish-toolkit-base.so`: 1820 of 1820 functions, 986665 of 986665 bytes match.
Controls: the same flags with `-O3` (Release) match 9.70 %, Release without the Debian flags 1.02 %.

`match` means: same instructions, addresses replaced by what they refer to (import name, read-only data, paired function).
Weaker checks: writable data is compared by section only, and non-string read-only data behind `lea` is not compared
except jump tables. Functions are FDE ranges of `.text` without `_start`.

This proves the compiler version; `login`, `reef-authhelper` and `reef-open` confirm it for the target binaries.
The Metal libraries and the host library have not been checked yet.

## Reading the original
- Byte buffers (`char[]`) sit next to the stack canary: their offsets tell arrays from structs.
- A loop that appears twice is one loop unswitched by `-O3`.
- Values kept in registers across calls that could change memory were passed by value or held in a `string_view`.
- The author uses `memset` rather than `= {}` for C structs; order of conditions matters.
- RTTI names tell the namespace: `7Content` is global, `*N12_GLOBAL__N_1...` anonymous. It decides what is inlined.
- A `const` local captured by a lambda is copied where a non-const one is moved.
- Tools: `tools/ndiff.py` (listing diff with stack offsets masked), `tools/variants.py` (ranks source variants).
- Code that matches nothing still has to behave the same: the test found a wrong layout hint in a function that looked right.
