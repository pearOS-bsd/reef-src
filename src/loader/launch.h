// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The first start of an application that came from elsewhere: reef-open asks the user before it runs.

#pragma once

#include <string>

namespace cutefish::reef {

// Becomes reef-open when the application was never opened, or another team signed it since; returns otherwise.
void launchThroughOpen(const std::string& bundle, const std::string& identifier, const std::string& executable, char** argv);

} // namespace cutefish::reef
