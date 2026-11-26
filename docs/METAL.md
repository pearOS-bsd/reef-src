# The Metal libraries

## libreef-metal-translator.so

`lib/cutefish-reef/lib/libreef-metal-translator.so`: Metal libraries (metallib files with AIR bitcode, or Metal sources compiled by
`libreef-metal-msl.so`, which it loads from its own folder) to GLSL, with LLVM 19.1 (`libLLVM.so.19.1`). C interface `reef_mtl_*`
(`translator.h`, 36 functions): libraries, functions, function constants, translation (also linked with visible functions),
reflection (bindings, constant samplers, attributes, varyings, render targets, uniforms, the threadgroup size). Translations and
compiled sources are kept in `~/.cache/cutefish-reef/metal` (`$CFFIXED_USER_HOME` first), keyed by SHA-256 of what went in.

| file | binary | what |
|---|---|---|
| `emitter.cpp`, `emitter.h` | `0x25c80`-`0x4bf40` | the pass pipeline, the class `Emitter`: values, types, constants, arithmetic, function constants, entry functions, `translate` |
| `intrinsics.cpp` | `0x4bf40`-`0x617f0` | `air.*` and `llvm.*` calls: math, integers, conversions, SIMD groups, atomics, barriers, derivatives |
| `intrinsics_commands.cpp` | `0x617f0`-`0x6bac0` | indirect command buffers |
| `structure.cpp` | `0x6bac0`-`0x83940` | structured control flow, the state machine for the rest, the assembly of the GLSL text |
| `memory.cpp`, `pointers.cpp` | `0x83940`-`0x98100`, `0x98100`-`0xa5d80` | roots and addresses, loads and stores of every memory, the passes that rewrite pointers and argument buffers |
| `textures.cpp`, `textures.h` | `0xa5d80`-`0xb6f00` | textures and samplers |
| `metadata.cpp`, `metadata.h` | `0xb6f00`-`0xc0a70` | AIR metadata of functions and arguments |
| `metallib.cpp`, `metallib.h` | `0xc0a70`-`0xc2480` | the metallib container |
| `cache.cpp`, `cache.h` | `0xc2480`-`0xd01c0` | the cache on disk (`REEFMTL1`, `REEFMSL1`) |
| `translator.cpp`, `translator.h`, `shader.h` | `0xd01c0`-`0xe0bca` | the C interface |

Checked: `tests/metal/run.sh` (CI): 323 Metal sources (every intrinsic family, control flow, memory, texture kind and option,
argument buffers, linked functions, function constants) through the C interface of the original and of ours, with and without
options, then once more from the cache: 77680 lines of GLSL, reflection and errors, the same. Each part was also checked alone
inside the original (its functions patched in), on more than 20000 random AIR modules, real metallibs and damaged ones.
Known in the original: RenderBox's `resolve_color_fragment` translates differently depending on the heap; damaged bitcode can crash
LLVM's reader.

## libreef-metal-msl.so

In progress.
