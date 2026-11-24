# reef-src (private)
Source of the ELF programs and libraries of Reef, reconstructed function by function. Linux only. BSD-3-Clause (see NOTICE).

| target | matched |
|---|---|
| `login` | 1/1 functions, 1269/1269 bytes; differential tests in `tests/small.py` |
| `reef-authhelper` | 2/2 functions, 1960/1960 bytes; differential tests in `tests/small.py` |
| the other 10 | not started |

    docker build -t reef-build docker
    docker run --rm -v $PWD:/w:ro -v ORIGINAL/usr:/o:ro reef-build sh /w/tools/build-match.sh /o /w

`docs/TOOLCHAIN.md` has the evidence for the compiler and flags; `tools/compare.py -v` lists what differs.
Run `tests/small.py` in the container after `build-match.sh` (build is in /tmp/b): 25 cases, original against ours.
`reef-authhelper` and `login` are security sensitive: run them in a container, never install them setuid.
