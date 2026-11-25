// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The sections of the report.

#pragma once

#include "report.h"

#include <memory>

namespace cutefish::reef::diagnose {

// activity.cpp
void addActivity(Report& report);

// system.cpp
void addSystem(Report& report);
void addKernelLog(Report& report);
// The lines of the kernel log that tell of a GPU reset.
std::vector<std::string> resetLines();
// Those that are not among `before`.
void addResets(Report& report, const std::vector<std::string>& before);
void addEglInfo(Report& report);

// egl.cpp
void addEgl(Report& report);

// driver.cpp
void addDriver(Report& report);
// What the process does that imports a texture: reef-diagnose --import-check WIDTH HEIGHT FOURCC MODIFIER COLOR PLANES
// [DESCRIPTOR OFFSET PITCH]...
int importCheck(int argc, char** argv);

// glass.cpp
void addGlass(Report& report);

// Reads the clocks of the GPUs ten times a second in a thread.
class ClockSampler
{
public:
    ClockSampler();
    ~ClockSampler();

    void start();
    void stop();
    void add(Report& report);

private:
    struct State;
    std::unique_ptr<State> m_state;
};

} // namespace cutefish::reef::diagnose
