// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The reader of Metal library files (.metallib): the functions and their AIR bitcode.
#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace cutefish::reef::metal {

// One function of a Metal library file. (0x48 bytes)
struct LibraryFunction
{
    std::string name;       // 0x00  NAME
    int stage = 0;          // 0x20  TYPE: 1 vertex, 2 fragment, 3 kernel, 5 visible, 0 other
    std::string bitcode;    // 0x28  the AIR bitcode, OFFT and MDSZ in the bitcode section
};

// Reads the function list of a Metal library file into `functions`. On failure, says why in `error`.
bool readLibrary(const void *data, size_t size, std::vector<LibraryFunction> &functions, std::string &error); // 0xc0bd0

}
