// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 1999-2009 Apple Inc.
// SPDX-License-Identifier: APSL-2.0
//
// Portions Copyright (c) 1999-2009 Apple Inc.  All Rights Reserved.
//
// This file contains Original Code and/or Modifications of Original Code as
// defined in and that are subject to the Apple Public Source License Version
// 2.0 (the 'License').  You may not use this file except in compliance with
// the License.  Please obtain a copy of the License at
// http://www.opensource.apple.com/apsl/ and read it before using this file.
//
// The Original Code and all software distributed under the License are
// distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
// EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
// INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.  Please see
// the License for the specific language governing rights and limitations
// under the License.
//
// Changed by Reion Wong on 2026-10-30: IOHIDFamily's scroll wheel
// acceleration (IOHIDScrollAccelerator, IOHIDTableAcceleration and
// IOHIPointing's line conversion) rewritten in Objective-C; the numeric
// constants and the default acceleration table are IOHIDFamily's.

#import "internal.h"

#include <math.h>
#include <string.h>

// Scroll wheel acceleration as IOHIDFamily's IOHIDScrollAccelerator does it
// for a mouse with no curves of its own: the default acceleration table at
// the default scroll speed, a wheel resolution of 9 and a 67 Hz report
// rate. A click's boost comes from the average size and spacing of the
// clicks just before it; the table maps that to pixels, which go out whole
// (at least one) as tenths of a line.

#define FIXED_TO_DOUBLE(x) ((x) / 65536.0)

static const double wheelResolution = 9;
static const double frameRate = 67;
static const double defaultScrollAcceleration = FIXED_TO_DOUBLE(0x5000);
static const double cursorScale = 96.0 / 67.0;
static const double multiplierA = FIXED_TO_DOUBLE(0x00000002);
static const double multiplierB = FIXED_TO_DOUBLE(0x000003bb);
static const double multiplierC = FIXED_TO_DOUBLE(0x00018041);
static const double pixelToWheelScale = FIXED_TO_DOUBLE(0x0000199a);
static const double clearThresholdMS = 500;
static const double eventThresholdMS = 150;
enum { averageLength = 9, maximumPoints = 32 };

// IOHIDFamily's default acceleration table: a scale, a signature, then
// curves of an acceleration and points, all big-endian 16.16.
static const uint8_t defaultTable[] = {
    0x00, 0x00, 0x80, 0x00,
    0x40, 0x32, 0x30, 0x30, 0x00, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x09, 0x00, 0x00, 0x71, 0x3B, 0x00, 0x00,
    0x60, 0x00, 0x00, 0x04, 0x4E, 0xC5, 0x00, 0x10,
    0x80, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x5F,
    0x00, 0x00, 0x00, 0x16, 0xEC, 0x4F, 0x00, 0x8B,
    0x00, 0x00, 0x00, 0x1D, 0x3B, 0x14, 0x00, 0x94,
    0x80, 0x00, 0x00, 0x22, 0x76, 0x27, 0x00, 0x96,
    0x00, 0x00, 0x00, 0x24, 0x62, 0x76, 0x00, 0x96,
    0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x96,
    0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x96,
    0x00, 0x00
};

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    double acceleration;
    Point points[maximumPoints];
    int count;
} Curve;

typedef struct {
    double m;
    double b;
    double x;
} Segment;

static Segment segments[maximumPoints * 2];
static int segmentCount;

static int32_t readInt32(const uint8_t *p)
{
    return (int32_t)((uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]);
}

static int readCurves(Curve *curves, int capacity)
{
    const uint8_t *p = defaultTable + 8;
    const int count = p[0] << 8 | p[1];
    p += 2;
    for (int i = 0; i < count && i < capacity; ++i) {
        curves[i].acceleration = FIXED_TO_DOUBLE(readInt32(p));
        curves[i].count = p[4] << 8 | p[5];
        p += 6;
        for (int j = 0; j < curves[i].count; ++j, p += 8)
            curves[i].points[j] = (Point){FIXED_TO_DOUBLE(readInt32(p)), FIXED_TO_DOUBLE(readInt32(p + 4))};
    }
    return count < capacity ? count : capacity;
}

// The first point at an x stays, as in an ordered set.
static void insertPoint(Point *points, int *count, Point point)
{
    for (int i = 0; i < *count; ++i) {
        if (points[i].x == point.x)
            return;
    }
    if (*count < maximumPoints * 2)
        points[(*count)++] = point;
}

static Point interpolatePoint(Point p, Point p0, Point p1, double ratio)
{
    const double m = (p1.y - p0.y) / (p1.x - p0.x);
    const double b = p1.y - m * p1.x;
    const double y = p.x * m + b;
    return (Point){p.x, fmin(p.y, y) + fabs(p.y - y) * ratio};
}

static void interpolateFunction(const Curve *lo, const Curve *hi, double ratio, Point *points, int *count)
{
    int high = 0;
    Point h0 = {0, 0};
    Point h1 = hi->points[0];
    for (int i = 0; i < lo->count; ++i) {
        const Point p = lo->points[i];
        while (p.x > h1.x && high < hi->count - 1) {
            h0 = h1;
            h1 = hi->points[++high];
        }
        insertPoint(points, count, interpolatePoint(p, h0, h1, ratio));
    }
}

static int comparePoints(const void *a, const void *b)
{
    const double ax = ((const Point *)a)->x, bx = ((const Point *)b)->x;
    return ax < bx ? -1 : ax > bx;
}

static void buildSegments(void)
{
    Curve curves[4];
    const int curveCount = readCurves(curves, 4);
    const double acceleration = defaultScrollAcceleration;
    const Curve *hi = &curves[0];
    const Curve *lo = hi;
    for (int i = 0; i < curveCount; ++i) {
        lo = hi;
        hi = &curves[i];
        if (acceleration <= hi->acceleration) {
            if (acceleration == hi->acceleration)
                lo = hi;
            break;
        }
    }
    Point points[maximumPoints * 2];
    int count = 0;
    if (hi == lo || acceleration > hi->acceleration) {
        const double scale = (acceleration + 1) / (hi->acceleration + 1);
        for (int i = 0; i < hi->count; ++i)
            insertPoint(points, &count, (Point){hi->points[i].x, hi->points[i].y * scale});
    } else {
        const double ratio = (acceleration - lo->acceleration) / (hi->acceleration - lo->acceleration);
        interpolateFunction(lo, hi, ratio, points, &count);
        interpolateFunction(hi, lo, ratio, points, &count);
    }
    qsort(points, count, sizeof(Point), comparePoints);
    Point p1 = {0, 0};
    for (int i = 0; i < count; ++i) {
        const Point p0 = p1;
        p1 = (Point){points[i].x * wheelResolution / frameRate, points[i].y * cursorScale};
        const double m = (p1.y - p0.y) / (p1.x - p0.x);
        segments[segmentCount++] = (Segment){m, p1.y - m * p1.x, p1.x};
    }
}

static double tableMultiplier(double value)
{
    int curve = 0;
    for (int i = 0; i < segmentCount; ++i) {
        curve = i;
        if (!(value > segments[i].x))
            break;
    }
    return segments[curve].m * value + segments[curve].b;
}

typedef struct {
    double deltaTime[averageLength];
    double scroll[averageLength];
    int head;
    int tail;
    BOOL direction;
    BOOL started;
    uint32_t lastTime;
} Axis;

static Axis axes[2];

static double accelerate(Axis *axis, double scroll, uint32_t time)
{
    const double deltaTime = axis->started ? (double)(uint32_t)(time - axis->lastTime) : INFINITY;
    axis->started = YES;
    axis->lastTime = time;
    const int head = axis->head;
    axis->deltaTime[head] = deltaTime;
    axis->scroll[head] = fabs(scroll);
    axis->head = (axis->head + 1) % averageLength;
    if (axis->head == axis->tail)
        axis->tail = (axis->tail + 1) % averageLength;
    const BOOL direction = scroll > 0;
    if (axis->direction != direction || deltaTime > clearThresholdMS) {
        axis->tail = head;
        axis->direction = direction;
    }

    double sumDeltaTime = 0;
    double sumScroll = 0;
    int count = 0;
    int i = axis->head;
    do {
        i = (i ? i : averageLength) - 1;
        sumScroll += axis->scroll[i];
        ++count;
        if (axis->deltaTime[i] > eventThresholdMS) {
            sumDeltaTime += eventThresholdMS;
            break;
        }
        sumDeltaTime += axis->deltaTime[i];
        if (sumDeltaTime >= clearThresholdMS)
            break;
    } while (i != axis->tail);

    const double averageDeltaTime = fmin(fmax(sumDeltaTime / count, 1), eventThresholdMS);
    const double averageScroll = sumScroll / count;
    double velocity = (multiplierA * averageDeltaTime * averageDeltaTime - multiplierB * averageDeltaTime + multiplierC)
        * averageScroll;
    velocity = fmax(velocity, FIXED_TO_DOUBLE(1));
    if (count > 2)
        velocity = velocity * sqrt(count * 16) / 4;
    return scroll * tableMultiplier(velocity) / fabs(scroll);
}

CGFloat AKAcceleratedWheelLines(int axisIndex, CGFloat clicks, uint32_t time)
{
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        buildSegments();
    });
    if (clicks == 0 || axisIndex < 0 || axisIndex > 1)
        return 0;
    // Whole pixels, at least one, as the platform's wheel events carry them.
    const double pixels = accelerate(&axes[axisIndex], clicks, time);
    double whole = trunc(pixels);
    if (whole == 0)
        whole = pixels < 0 ? -1 : 1;
    return whole * pixelToWheelScale;
}
