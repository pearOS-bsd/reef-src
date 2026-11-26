# SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause
#
# Reconstructed from the published binary.
# The table of the Metal standard library headers, each file a raw string literal, in the order clang is given them.
#
# cmake -DINPUT=<src/metal-msl/include> -DOUTPUT=<headers.cpp> -P embed.cmake
set(names metal_stdlib metal_types metal_type_traits metal_swizzle metal_matrix metal_math metal_common metal_integer
    metal_geometric metal_relational metal_texture metal_compute metal_atomic metal_pack metal_graphics simd/simd.h)
set(text "// Generated from src/metal-msl/include by embed.cmake.\n\n#include \"frontend.h\"\n\nnamespace cutefish::reef::metal::msl {\n\nconst Header headers[] = {\n")
foreach(name IN LISTS names)
    file(READ "${INPUT}/${name}" content)
    string(FIND "${content}" ")__msl__\"" clash)
    if(NOT clash EQUAL -1)
        message(FATAL_ERROR "${name} holds the delimiter of the raw string")
    endif()
    string(APPEND text "    {\"${name}\", R\"__msl__(${content})__msl__\"},\n")
endforeach()
string(APPEND text "    {nullptr, nullptr},\n};\n\n}\n")
file(WRITE "${OUTPUT}.new" "${text}")
file(COPY_FILE "${OUTPUT}.new" "${OUTPUT}" ONLY_IF_DIFFERENT)
file(REMOVE "${OUTPUT}.new")
