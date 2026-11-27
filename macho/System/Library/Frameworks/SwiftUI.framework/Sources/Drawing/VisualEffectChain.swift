// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VisualEffectChain (Drawing).

import Foundation
import AppKit
import CoreGraphics
import CoreText
import Combine
import Observation
import CoreTransferable
import UniformTypeIdentifiers
import Symbols
import DeveloperToolsSupport
import Metal
import os

struct VisualEffectChain {
    var steps: [SwiftUI.VisualEffectChain.Step]
    enum Step {
        case offset(CGSize)
        case opacity(Swift.Double)
        case scale(CGSize, SwiftUI.UnitPoint)
        case rotation(SwiftUI.Angle, SwiftUI.UnitPoint)
        case blur(CoreGraphics.CGFloat)
        case brightness(Swift.Double)
        case saturation(Swift.Double)
        case contrast(Swift.Double)
        case grayscale(Swift.Double)
        case hueRotation(SwiftUI.Angle)
    }
}

extension SwiftUI.VisualEffectChain: SwiftUI.Animatable {
    typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.VisualEffectChain: SwiftUI.VisualEffect {
}
