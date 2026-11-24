# reef-src (private)
Source of the ELF programs and libraries of Reef, reconstructed function by function. Linux only. BSD-3-Clause (see NOTICE).

| target | matched |
|---|---|
| `login` | 1/1 functions, 1269/1269 bytes |
| `reef-authhelper` | 1/2 functions, 399/1960 bytes; `main` reconstructed, not matching yet, no differential test yet |
| the other 10 | not started |

    docker build -t reef-build docker
    docker run --rm -v $PWD:/w:ro -v ORIGINAL/usr:/o:ro reef-build sh /w/tools/build-match.sh /o /w

`docs/TOOLCHAIN.md` has the evidence for the compiler and flags; `tools/compare.py -v` lists what differs.
`reef-authhelper` and `login` are security sensitive: run them in a container, never install them setuid.
