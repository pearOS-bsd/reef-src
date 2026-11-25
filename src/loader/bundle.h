// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Application bundles: the executable and the values of Info.plist (XML or binary).

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace cutefish::reef {

std::string withoutTrailingSlashes(std::string path);
// The string value of a key of the bundle's Info.plist; empty when there is none.
std::string infoString(const std::string& bundle, const std::string& key);
// The pairs of strings of a dictionary of Info.plist.
std::vector<std::pair<std::string, std::string>> infoDictionary(const std::string& bundle, const std::string& key);
// The main executable of a bundle; empty for what is not one.
std::string bundleExecutable(const std::string& path);

} // namespace cutefish::reef
