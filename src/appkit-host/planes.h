// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#pragma once

#include <cstdint>

// One pane of glass, see ui::ViewGlass.
struct reef_ui_glass
{
    uint64_t key;
    double rect[4];
    double radius;
    double opacity;
    double tint[4];
    const double* light;
    unsigned lightCount;
    double over[4];
    double edge[4];
    double sideEdge[4];
    double edgeDevicePixels;
    double blur;
    double saturation;
    double bezel;
    double bend;
    double profile;
    double dispersion;
    double zoom;
    double glow[4];
    double glowRadius;
    double glowEdge;
    double glowLag;
    double fadeTop;
    double fadeBottom;
    double shadow[4];
    double shadowBlur;
    double shadowOffset;
    double duration;
    double curve[4];
    double shapeDuration;
    double shapeCurve[4];
    double exponent;
    double sideBezel;
    double bottomEdge[4];
    double multiply[4];
    int blurAtHalfResolution;
    int edgeOnPixels;
    double bendPeak;
    double dispersionSides;
    double shadowGap;
    double bendAlongAxes;
    double edgeSplit;
    double sideBendPeak;
    double exponentSlopes[2];
    double blurPower;
    double fadeBlur;
    const double* fadeProfile;
    unsigned fadeProfileCount;
};
