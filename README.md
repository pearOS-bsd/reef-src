# reef-src (private)
Source of the ELF programs and libraries of Reef, reconstructed function by function. Linux only. BSD-3-Clause (see NOTICE).

| target | matched |
|---|---|
| `login` | 1/1 functions, 1269/1269 bytes; differential tests in `tests/small.py` |
| `reef-authhelper` | 2/2 functions, 1960/1960 bytes; differential tests in `tests/small.py` |
| `reef-open` | 74/78 functions, 48.4 % of bytes; open: `main`, `readSignature` and their cold parts |
| `reef-unsupported` | 105/107 functions, 60.2 % of bytes; open: `main` and its cold part |
| the other 8 | not started |

Behaviour: `tests/small.py` 25 cases (login, authhelper), `tests/dialogs.py` 1444 cases (the two dialogs, 2026-11-25): no difference.
The dialogs run without a display: `tests/uishim.cpp` prints what they would show. Of the reef-open cases 393 ended in a
verified developer (CotEditor, also with harmless damage) and 922 in a refusal. Not covered: a Mac App Store signature,
colours, real windows. CI has no signed applications, so it runs only the part without them.

    docker build -t reef-build docker
    docker run --rm -v $PWD:/w:ro -v ORIGINAL/usr:/o:ro -v TOOLKIT:/t:ro -v TOOLKIT_LIBS:/l:ro reef-build sh /w/tools/build-match.sh /o /w /t /l

`docs/TOOLCHAIN.md` has the evidence for the compiler and flags; `tools/compare.py -v` lists what differs.
Run `tests/small.py` in the container after `build-match.sh` (build is in /tmp/b): 25 cases, original against ours.
`reef-authhelper` and `login` are security sensitive: run them in a container, never install them setuid.
