// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// Drawing contexts and paths: random programs on bitmaps; the log has the queries and hashes of the pixels.
// usage: graphics LIBRARY SEEDS FIRST
#include "load.h"

#include "paths.h"

static std::vector<reef_ui_path_element> randomPath(double size)
{
    std::vector<reef_ui_path_element> path;
    const int count = pick(9);
    if (chance(90))
        path.push_back({0, {number(-5, size), number(-5, size)}});
    for (int i = 0; i < count; i++) {
        reef_ui_path_element element = {};
        element.type = chance(3) ? pick(9) - 2 : pick(5);
        for (double& value : element.points)
            value = number(-10, size + 10);
        path.push_back(element);
    }
    if (chance(50))
        path.push_back({4, {}});
    return path;
}

static void sayPath(const char* name, const std::vector<reef_ui_path_element>& path)
{
    std::string text;
    char buffer[400];
    for (const reef_ui_path_element& element : path) {
        snprintf(buffer, sizeof buffer, " %d(%a %a %a %a %a %a)", element.type, element.points[0], element.points[1], element.points[2],
                 element.points[3], element.points[4], element.points[5]);
        text += buffer;
    }
    say("%s:%s", name, text.c_str());
}

static void paths()
{
    const auto a = randomPath(50), b = randomPath(50);
    std::vector<reef_ui_path_element> out(400, reef_ui_path_element{-1, {}});
    const unsigned capacity = chance(90) ? 400 : pick(10);
    unsigned count = host->hostPathStroked(a.data(), unsigned(a.size()), number(0, 12), pick(5) - 1, pick(5) - 1, number(0, 12), out.data(),
                                           capacity);
    say("stroked %u", count);
    out.resize(std::min(std::min(count, capacity) + 1, 400u));
    sayPath("stroked", out);
    out.assign(400, reef_ui_path_element{-1, {}});
    count = host->hostPathCombine(a.data(), unsigned(a.size()), pick(3), b.data(), unsigned(b.size()), pick(3), pick(5), out.data(), capacity);
    say("combined %u", count);
    out.resize(std::min(std::min(count, capacity) + 1, 400u));
    sayPath("combined", out);
}

static void randomColor(double* color)
{
    for (int i = 0; i < 3; i++)
        color[i] = chance(5) ? number(-1, 3) : number(0, 1);
    color[3] = chance(40) ? 1 : number(0, 1);
}

static void randomMatrix(double* matrix)
{
    matrix[0] = chance(40) ? 1 : number(-2, 2);
    matrix[1] = chance(60) ? 0 : number(-1, 1);
    matrix[2] = chance(60) ? 0 : number(-1, 1);
    matrix[3] = chance(40) ? 1 : number(-2, 2);
    matrix[4] = number(-10, 30);
    matrix[5] = number(-10, 30);
}

static void drawing()
{
    const int width = 8 + pick(60), height = 8 + pick(60);
    const size_t stride = size_t(width) * 4 + (chance(30) ? 4 * pick(4) : 0);
    std::vector<unsigned char> pixels(stride * height);
    if (chance(50)) {
        for (unsigned char& byte : pixels)
            byte = (unsigned char)random32();
        // Premultiplied.
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                unsigned char* pixel = &pixels[y * stride + x * 4];
                for (int i = 0; i < 3; i++)
                    pixel[i] = (unsigned char)(pixel[i] * pixel[3] / 255);
            }
        }
    }
    reef_ui_gc* gc = host->gcCreateForPixels(pixels.data(), width, height, stride, chance(2) ? 5 : pick(2));
    say("gc %d %dx%d", gc != nullptr, width, height);
    if (!gc)
        return;
    std::vector<unsigned char> picture(16 * 16 * 4);
    for (size_t i = 0; i < picture.size(); i += 4) {
        picture[i + 3] = (unsigned char)random32();
        for (int j = 0; j < 3; j++)
            picture[i + j] = (unsigned char)(random32() % 256 * picture[i + 3] / 255);
    }
    // What is open: a save, a group or a masked group. Only the last one is closed: the canvas does not survive other orders.
    std::string open;
    const int count = 1 + pick(60);
    for (int i = 0; i < count; i++) {
        double color[4], matrix[6], rect[4] = {number(-10, width), number(-10, height), number(-2, width), number(-2, height)};
        const auto path = randomPath(width);
        switch (pick(47)) {
        case 0:
        case 1:
            host->gcSave(gc);
            open += 's';
            break;
        case 2:
        case 3:
            if (open.ends_with('s')) {
                host->gcRestore(gc);
                open.pop_back();
            }
            break;
        case 4:
            randomMatrix(matrix);
            host->gcConcat(gc, matrix);
            break;
        case 5:
            randomMatrix(matrix);
            host->gcSetTransform(gc, matrix);
            break;
        case 6:
            host->gcTransform(gc, matrix);
            say("transform %a %a %a %a %a %a", matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]);
            break;
        case 7:
            say("scale %a", host->gcDeviceScale(gc));
            break;
        case 8:
            host->gcClipToRect(gc, rect[0], rect[1], rect[2] + 20, rect[3] + 20);
            break;
        case 9:
            if (chance(40))
                host->gcClipToPath(gc, path.data(), unsigned(path.size()), pick(3));
            break;
        case 10:
            host->gcResetClip(gc);
            break;
        case 11:
            host->gcClipBounds(gc, rect);
            say("clip %a %a %a %a", rect[0], rect[1], rect[2], rect[3]);
            break;
        case 12:
            host->gcSetAlpha(gc, chance(30) ? 1 : number(0, 1));
            break;
        case 13:
            host->gcSetComposite(gc, chance(60) ? 0 : pick(32) - 2);
            break;
        case 14:
            host->gcSetAntialias(gc, pick(2));
            break;
        case 15:
            host->gcSetSmoothing(gc, pick(7) - 1);
            break;
        case 16:
        case 17:
        case 18:
            randomColor(color);
            host->gcSetFillColor(gc, color[0], color[1], color[2], color[3]);
            break;
        case 19:
        case 20:
            randomColor(color);
            host->gcSetStrokeColor(gc, color[0], color[1], color[2], color[3]);
            break;
        case 21:
            host->gcSetLineWidth(gc, number(0, 10));
            break;
        case 22:
            host->gcSetLineCap(gc, pick(5) - 1);
            break;
        case 23:
            host->gcSetLineJoin(gc, pick(5) - 1);
            break;
        case 24:
            host->gcSetMiterLimit(gc, number(0, 12));
            break;
        case 25: {
            double lengths[5];
            const int dashes = pick(5);
            for (double& length : lengths)
                length = number(0, 9);
            host->gcSetLineDash(gc, lengths, dashes, number(0, 6));
            break;
        }
        case 26:
            randomColor(color);
            if (chance(30))
                color[3] = 0;
            host->gcSetShadow(gc, number(-5, 5), number(-5, 5), number(0, 6), color[0], color[1], color[2], color[3]);
            break;
        case 27:
        case 28:
        case 29:
            host->gcFillPath(gc, path.data(), unsigned(path.size()), pick(3));
            break;
        case 30:
        case 31:
            host->gcStrokePath(gc, path.data(), unsigned(path.size()));
            break;
        case 32:
        case 33:
        case 34:
            host->gcFillRect(gc, rect[0], rect[1], rect[2], rect[3]);
            break;
        case 35:
            host->gcStrokeRect(gc, rect[0], rect[1], rect[2], rect[3]);
            break;
        case 36:
            host->gcClearRect(gc, rect[0], rect[1], rect[2], rect[3]);
            break;
        case 37:
            host->gcBlurBackdrop(gc, path.data(), unsigned(path.size()), number(0, 8), number(0, 2));
            break;
        case 38:
        case 39: {
            double offsets[4], colors[16];
            const int stops = pick(5);
            double at = 0;
            for (int j = 0; j < 4; j++) {
                offsets[j] = at;
                at = std::min(1.0, at + number(0, 0.6));
                randomColor(colors + j * 4);
            }
            if (chance(50))
                host->gcFillLinearGradient(gc, offsets, colors, stops, rect[0], rect[1], rect[2], rect[3], pick(4));
            else
                host->gcFillRadialGradient(gc, offsets, colors, stops, rect[0], rect[1], number(0, 10), rect[2], rect[3], number(0, 30), pick(4));
            break;
        }
        case 40:
            host->gcBeginGroup(gc);
            open += 'g';
            break;
        case 41:
            if (open.ends_with('g')) {
                host->gcEndGroup(gc);
                open.pop_back();
            }
            break;
        case 42:
            host->gcFillColor(gc, color);
            say("color %a %a %a %a", color[0], color[1], color[2], color[3]);
            break;
        case 43:
            host->gcDrawPixels(gc, picture.data(), 16, 16, 64, pick(3), rect[0], rect[1], rect[2], rect[3], pick(2));
            break;
        case 44:
            host->gcBeginMaskedGroup(gc);
            open += 'm';
            break;
        case 45:
            if (open.ends_with('m')) {
                randomMatrix(matrix);
                const bool with = chance(80);
                host->gcEndGroupMaskedByPixels(gc, with ? picture.data() : nullptr, chance(95) ? 16 : 0, 16, 64, rect[0], rect[1], rect[2],
                                               rect[3], matrix, pick(2));
                open.pop_back();
            }
            break;
        case 46:
            if (chance(30))
                say("cleared %d", host->gcClearBitmap(gc));
            else if (chance(30))
                host->gcExcludeRect(gc, rect[0], rect[1], rect[2] / 2, rect[3] / 2);
            break;
        }
        if (i % 8 == 7)
            say("pixels %016lx", (unsigned long)hashOf(pixels.data(), pixels.size()));
    }
    for (; !open.empty(); open.pop_back()) {
        const double identity[6] = {1, 0, 0, 1, 0, 0};
        if (open.back() == 's')
            host->gcRestore(gc);
        else if (open.back() == 'g')
            host->gcEndGroup(gc);
        else
            host->gcEndGroupMaskedByPixels(gc, picture.data(), 16, 16, 64, 0, 0, width, height, identity, 0);
    }
    say("pixels %016lx", (unsigned long)hashOf(pixels.data(), pixels.size()));
    host->gcDestroy(gc);
    say("pixels %016lx", (unsigned long)hashOf(pixels.data(), pixels.size()));
}

int main(int argc, char** argv)
{
    load(argv[1]);
    const int seeds = argc > 2 ? atoi(argv[2]) : 100;
    const int first = argc > 3 ? atoi(argv[3]) : 1;
    for (int i = 0; i < seeds; i++) {
        seed(first + i);
        say("seed %d", first + i);
        fflush(stdout);
        paths();
        drawing();
    }
    return 0;
}
