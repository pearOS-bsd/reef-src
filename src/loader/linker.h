// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#pragma once

#include "host.h"

#include <deque>
#include <mutex>

namespace cutefish::reef {

struct ImageInfo
{
    void* unknown[3];
};

class Linker
{
public:
    static Linker* current;
    const std::string& root() const { return m_root; }
    const std::deque<ImageInfo>& infos() const { return m_infos; }

    const ImageInfo* info(uint32_t index) const;
    void* symbol(void* handle, const char* name);
    void observe(void (*mapped)(size_t count, const char* const* paths, const void* const* headers),
                 void (*initialized)(const char* path, const void* header), void* unmapped);
    void* open(const std::string& path, uint32_t mode, std::string& error);
    bool findSymbol(const void* address, HostSymbol* symbol);
    const void* imageContaining(const void* address);
    bool contains(uintptr_t address) const;
    void* readPreferences(const char* name, size_t* size);
    void writePreferences(const char* name, const void* data, size_t size);

private:
    std::mutex m_mutex;
    std::string m_root;
    char m_unknown[0x320 - 0x48];
    std::deque<ImageInfo> m_infos;
};

// image.cpp
void* reserveMemory(size_t size);

} // namespace cutefish::reef
