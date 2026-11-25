// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The code signature of a Mach-O file, as much of it as tells who signed it.

#pragma once

#include <optional>
#include <string>

namespace cutefish::reef {

struct Signature
{
    bool adhoc = false;
    std::string team;
    std::string codeDirectory;
    // Empty for an ad hoc signature.
    std::string cms;
};

std::optional<Signature> readSignature(const std::string& path);

} // namespace cutefish::reef
