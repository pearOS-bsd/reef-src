// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Images: those that the GPU keeps, those of other processes, files, icons and symbols.

#pragma once

#include "ui/canvas_2d.h"

#include <cstddef>
#include <cstdint>

struct reef_ui_gc;

// Pixels that are drawn many times.
struct reef_ui_image
{
    cutefish::ui::RetainedBitmap bitmap;
};

// What another process draws into a DMA-BUF.
struct reef_ui_dma_buf
{
    cutefish::ui::DmaBufImage image;
};

namespace cutefish::reef {

// Pixels are 0xAARRGGBB with premultiplied alpha. Those that a program gets are for pixelsFree; the rows have no gaps, and
// there is null when they cannot be had.

reef_ui_image* imageCreate(const void* pixels, int width, int height, size_t stride);
void imageDestroy(reef_ui_image* image);
// `pixels` are those of the whole image; the rectangle of them is taken.
void imageUpdate(reef_ui_image* image, const void* pixels, size_t stride, int x, int y, int width, int height);

// The descriptors stay the program's.
reef_ui_dma_buf* dmaBufCreate(int width, int height, uint32_t fourcc, uint64_t modifier, int planeCount, const int* fds,
                              const uint32_t* offsets, const uint32_t* strides);
void dmaBufDestroy(reef_ui_dma_buf* dmaBuf);
// The sync file that signals when the drawing is done, which the program gives away.
void dmaBufSetFence(reef_ui_dma_buf* dmaBuf, int fd);

// Both return 0 when nothing was drawn.
int gcDrawImage(reef_ui_gc* gc, reef_ui_image* image, double x, double y, double width, double height, int mirrored);
int gcDrawDmaBuf(reef_ui_gc* gc, reef_ui_dma_buf* dmaBuf, double x, double y, double width, double height, int mirrored);

// A file, or bytes without a path.
void* pixelsDecode(const char* path, const void* bytes, size_t size, int* width, int* height);
// A PNG file, for pixelsFree too.
void* pixelsEncodePng(const void* pixels, int width, int height, size_t stride, size_t* size);
void pixelsFree(void* pixels);

// An icon of the theme, the one for the size.
void* iconPixels(const char* name, int size, int* width, int* height);
// The same drawn larger or smaller.
void* iconPixelsScaled(const char* name, int size, int pixelSize, int* width, int* height);

// Symbols are files next to the library, drawn in lines on 24 units.
int symbolExists(const char* name);
// Of its lines as if they had no width: x, y, width and height.
int symbolBounds(const char* name, double* bounds);
// Fits the lines, `weight` pixels wide, into the rectangle of a picture that has a margin on all sides.
void* symbolPixels(const char* name, int width, int height, const double* rect, double weight, int filled, int margin,
                   int* pixelWidth, int* pixelHeight);

} // namespace cutefish::reef
