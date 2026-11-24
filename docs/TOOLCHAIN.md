# Toolchain (measured 2026-11-25)
Debian trixie, GCC 14.2.0-19, GNU ld 2.44, flags of `dpkg-buildflags` plus CMake build type None, prefix `/usr`:
`-g -O2 -fstack-protector-strong -fstack-clash-protection -fcf-protection -D_FORTIFY_SOURCE=2`.

Known-answer test (`tools/calibrate.sh`): cutefish-toolkit commit 111537f is public and its Debian package is available.
`libcutefish-toolkit-base.so`: 1820 of 1820 functions, 986665 of 986665 bytes match.
Controls: the same flags with `-O3` (Release) match 9.70 %, Release without the Debian flags 1.02 %.

`match` means: same instructions, addresses replaced by what they refer to (import name, read-only data, paired function).
Weaker checks: writable data is compared by section only, and non-string read-only data behind `lea` is not compared
except jump tables. Functions are FDE ranges of `.text` without `_start`.

This proves the toolkit's toolchain. For the target binaries it is confirmed by `login` (1269 of 1269 bytes).
The Metal libraries and the host library have not been checked yet.
