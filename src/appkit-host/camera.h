// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Cameras of V4L2. Frames are given as the camera makes them.

#pragma once

#include <cstddef>
#include <cstdint>

struct reef_ui_camera;

struct reef_ui_camera_format
{
    uint32_t pixelFormat;
    int width;
    int height;
    // Frames per second.
    double minimumRate;
    double maximumRate;
};

struct reef_ui_camera_info
{
    char path[64];
    char name[128];
    char bus[96];
    int formatCount;
    reef_ui_camera_format formats[96];
};

namespace cutefish::reef {

// Returns how many of `cameras` it filled.
int hostCameraList(reef_ui_camera_info* cameras, int count);
reef_ui_camera* hostCameraOpen(const char* path, uint32_t pixelFormat, int width, int height, double rate);
// Copies the next frame and the time (nanoseconds of CLOCK_MONOTONIC) it was taken at. Returns 0, 1 when there is none
// after `timeout` milliseconds, -1 when the camera is gone.
int hostCameraRead(reef_ui_camera* camera, void* data, size_t capacity, size_t* size, uint64_t* time, int timeout);
void hostCameraClose(reef_ui_camera* camera);

} // namespace cutefish::reef
