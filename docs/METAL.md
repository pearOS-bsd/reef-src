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

`lib/cutefish-reef/lib/libreef-metal-msl.so`: `cutefish::reef::metal::compileMsl(llvm::LLVMContext&, const std::string& source,
const MslOptions&, std::string& errors)` (`msl.h`): Metal Shading Language to an LLVM module in AIR form, with clang 19.1
(`libclang-cpp.so.19.1`). The translator loads it from its own folder.

| file | what |
|---|---|
| `attributes.cpp` | Metal's attributes (`[[position]]`, `[[buffer(n)]]` ...) on clang's AST |
| `compile.cpp` | the compiler invocation, options (language version, fast math, macros), diagnostics (at most 20) |
| `entry_points.cpp` | vertex, fragment and kernel functions: arguments, roles, the AIR metadata |
| `types.cpp`, `argument_buffers.cpp` | AIR type names, argument buffers |
| `lower.cpp` | the IR rewritten into AIR: address spaces, intrinsic names, program-scope constructors, clean-up passes |
| `source.cpp` | the source rewritten before clang sees it |
| `include/` | the Metal standard library headers clang is given (163321 bytes, the same as in the binary's read-only data; no
  licence line in them, which would change their bytes: they are part of the BSD-3-Clause binary) |
| `embed.cmake` | makes `headers.cpp` of `include/` at build time |

Checked: `tests/metal/msl.sh` (CI): 93 sources (the standard library, stages, atomics, every language version from 1.0 to 4.0,
fast math off, macros, error limits, warnings, `#error`, missing headers, program-scope constructors, the source rewriting)
compiled by both: the same LLVM text, bitcode and errors; 1530 sources by hand, the same (19 crash both: `function_constant` on a
parameter or a struct member, AIR functions declared again by the program with another type). `tests/metal/run.sh` also runs
with both of ours against both originals: the same 77680 lines.
