// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Files kept between runs: the resolved binds of a program, and the state of an application.

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cutefish::reef {

struct Image;

uint32_t hash(std::string_view text);
// What the cache of a run depends on: the loader, the options and every image as it is on disk.
std::string cacheKey(const std::vector<Image*>& images, const std::string& options, int version);
std::string cachePath(const std::string& key);
bool readBinds(const std::string& path, const std::string& key, std::vector<uint64_t>& addresses, std::vector<uint32_t>& flags);
void writeBinds(const std::string& path, const std::string& key, const std::vector<uint64_t>& addresses, const std::vector<uint32_t>& flags);
bool readState(const std::string& path, const std::string& key, std::string& data);
void writeState(const std::string& path, const std::string& key, const void* data, size_t size);

} // namespace cutefish::reef
