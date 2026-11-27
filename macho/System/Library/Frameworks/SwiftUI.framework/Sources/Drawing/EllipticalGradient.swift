// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EllipticalGradient (Drawing).

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

public struct EllipticalGradient {
    var gradient: SwiftUI.Gradient
    var center: SwiftUI.UnitPoint
    var startRadiusFraction: CoreGraphics.CGFloat
    var endRadiusFraction: CoreGraphics.CGFloat
    public init(colors a0: [SwiftUI.Color], center a1: SwiftUI.UnitPoint, startRadiusFraction a2: CoreGraphics.CGFloat, endRadiusFraction a3: CoreGraphics.CGFloat) { _skeleton() }
    public init(gradient a0: SwiftUI.Gradient, center a1: SwiftUI.UnitPoint, startRadiusFraction a2: CoreGraphics.CGFloat, endRadiusFraction a3: CoreGraphics.CGFloat) { _skeleton() }
    public var body: SwiftUI._ShapeView<SwiftUI.Rectangle, SwiftUI.EllipticalGradient> { _skeleton() }
}

extension SwiftUI.EllipticalGradient: SwiftUI.ResolvableShapeStyle {
    func paint(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint { _skeleton() }
}

extension SwiftUI.EllipticalGradient: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}

extension SwiftUI.EllipticalGradient: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Rectangle, SwiftUI.EllipticalGradient>
}
