# reef-src (private)
Source of the ELF programs and libraries of Reef, reconstructed function by function. Linux only. BSD-3-Clause (see NOTICE).

**Total of the 12 ELF files: 37.0 % of the code bytes (1417103 of 3828323) and 75.7 % of the functions (3828 of 5060) are the
same as in the published binaries** (2026-11-26). `tools/total.py` computes it; every CI run prints it at the end of the match
list (there without `libreef-webkit-host.so`, which needs headers CI does not have). Functions that are written but not the
same yet do not count.
**Written: 100.0 % of the functions (5060 of 5060)**: every file; what was tested against the original is in the table. Now the
functions that are written but not the same yet are tuned. The 199 Mach-O files of Reef (58.6 MB) are not part of this repository.

| target | matched |
|---|---|
| `login` | 1/1 functions, 1269/1269 bytes; differential tests in `tests/small.py` |
| `reef-authhelper` | 2/2 functions, 1960/1960 bytes; differential tests in `tests/small.py` |
| `reef-open` | 74/78 functions, 48.4 % of bytes; open: `main`, `readSignature` and their cold parts |
| `reef-unsupported` | 105/107 functions, 60.2 % of bytes; open: `main` and its cold part |
| `libreef-webkit-host.so` | 213/260 functions, 45.2 % of bytes; all 100 table entries written, the same output as the original in `tests/webkit.py` (JSON, input, data, find, screens); views, downloads and dialogs not run yet |
| `reef-install-package` | 175/210 functions, 38.9 % of bytes; everything written, the same output as the original in `tests/package.py` (listing and command line installation); the window and the D-Bus helper not run yet |
| `reef` (loader) | 610/979 functions, 31.4 % of bytes; all written (the Mach broker last); the same output as the original in `tests/mach.py` (files, ports, processes, signals, sockets, threads) and `tests/loader.py` |
| `reef-hdiutil` | 75/196 functions, 14.1 % of bytes; all written (LZFSE from `upstream/`, its 21 functions match); the same images and output as the original in `tests/hdiutil.py` (46 cases: HFS+, ISO 9660, Joliet, UDF, the seven UDIF formats, verify, errors) |
| `reef-diagnose` | 309/401 functions, 38.9 % of bytes; all written; the same reports as the original in `tests/diagnose.py` (20 cases on made up machines), see `docs/DIAGNOSE.md` |
| `libreef-appkit-host.so` | 1478/1768 functions, 57.4 % of bytes; all 317 table entries written; the same behaviour as the original in `tests/appkit` (drawing, windows, planes, input, menus, OpenGL without a display) and in the tests of the units (fonts, text, PDF, video, sound, cameras, D-Bus, icons, dialogs, drags; see `docs/APPKIT-HOST.md`) |
| `libreef-metal-translator.so` | 511/712 functions, 23.9 % of bytes; all written; the same GLSL, reflection and errors as the original in `tests/metal/run.sh` (323 Metal sources, also from the cache), see `docs/METAL.md` |
| `libreef-metal-msl.so` | 275/346 functions, 23.1 % of bytes; all written; the same modules and errors as the original in `tests/metal/msl.sh` (93 sources; 1530 by hand), see `docs/METAL.md` |

Behaviour: `tests/small.py` 25 cases (login, authhelper), `tests/mach.py` 6 Mac programs on the loader (Mach ports through the broker included), `tests/hdiutil.py` 46 cases, `tests/diagnose.py` 20 cases, `tests/dialogs.py` 1444 cases (the two dialogs, 2026-11-25),
`tests/package.py` 540 cases (packages made there, good and broken: what is listed and the installed tree): no difference.
The dialogs run without a display: `tests/uishim.cpp` prints what they would show. Of the reef-open cases 393 ended in a
verified developer (CotEditor, also with harmless damage) and 922 in a refusal. Not covered: a Mac App Store signature,
colours, real windows. CI has no signed applications, so it runs only the part without them.

    docker build -t reef-build docker
    docker run --rm -v $PWD:/w:ro -v ORIGINAL/usr:/o:ro -v TOOLKIT:/t:ro -v TOOLKIT_LIBS:/l:ro reef-build sh /w/tools/build-match.sh /o /w /t /l

`docs/TOOLCHAIN.md` has the evidence for the compiler and flags; `tools/compare.py -v` lists what differs.
Run `tests/small.py` in the container after `build-match.sh` (build is in /tmp/b): 25 cases, original against ours.
`reef-authhelper` and `login` are security sensitive: run them in a container, never install them setuid.

## Package
`tools/make-deb.sh ORIGINAL.deb BUILD_DIR OUT_DIR [COMMIT]` makes `open-reef`: the original package with its 12 ELF files
replaced by a build of this repository (CMake, as in `tools/build-match.sh`). It conflicts with, replaces and provides the
original package, so it installs in its place.
