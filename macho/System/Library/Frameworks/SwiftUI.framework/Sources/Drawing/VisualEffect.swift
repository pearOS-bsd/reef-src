// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VisualEffect (Drawing).

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

public protocol VisualEffect: SwiftUI.Animatable {
}

extension SwiftUI.VisualEffect {
    var visualEffectSteps: [SwiftUI.VisualEffectChain.Step] { _skeleton() }
    func appending(_ a0: SwiftUI.VisualEffectChain.Step) -> SwiftUI.VisualEffectChain { _skeleton() }
    public func offset(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) -> some SwiftUI.VisualEffect { self }
    public func opacity(_ a0: Swift.Double) -> some SwiftUI.VisualEffect { self }
    public func scaleEffect(_ a0: CoreGraphics.CGFloat, anchor a1: SwiftUI.UnitPoint) -> some SwiftUI.VisualEffect { self }
    public func scaleEffect(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat, anchor a2: SwiftUI.UnitPoint) -> some SwiftUI.VisualEffect { self }
    public func rotationEffect(_ a0: SwiftUI.Angle, anchor a1: SwiftUI.UnitPoint) -> some SwiftUI.VisualEffect { self }
    public func blur(radius a0: CoreGraphics.CGFloat, opaque a1: Swift.Bool) -> some SwiftUI.VisualEffect { self }
    public func brightness(_ a0: Swift.Double) -> some SwiftUI.VisualEffect { self }
    public func saturation(_ a0: Swift.Double) -> some SwiftUI.VisualEffect { self }
    public func contrast(_ a0: Swift.Double) -> some SwiftUI.VisualEffect { self }
    public func grayscale(_ a0: Swift.Double) -> some SwiftUI.VisualEffect { self }
    public func hueRotation(_ a0: SwiftUI.Angle) -> some SwiftUI.VisualEffect { self }
    public func offset(_ a0: CGSize) -> some SwiftUI.VisualEffect { self }
    public func scaleEffect(_ a0: CGSize, anchor a1: SwiftUI.UnitPoint) -> some SwiftUI.VisualEffect { self }
}
