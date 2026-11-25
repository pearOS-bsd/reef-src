// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// JPEG 2000 images, read and written by OpenJPEG.

#pragma once

#include <cstddef>
#include <cstdint>

namespace cutefish::reef {

// A file (JP2) or a bare codestream.
bool isJpeg2000(const void* data, size_t size);
// Pixels are 0xAARRGGBB with premultiplied alpha, allocated by malloc. Null when it cannot be read.
uint32_t* decodeJpeg2000(const void* data, size_t size, int* width, int* height);
// A JP2 file, allocated by malloc. Without loss unless the quality is in [0, 1). Null when it cannot be written.
void* hostImageEncodeJpeg2000(const void* pixels, int width, int height, size_t stride, double quality, size_t* size);

} // namespace cutefish::reef
