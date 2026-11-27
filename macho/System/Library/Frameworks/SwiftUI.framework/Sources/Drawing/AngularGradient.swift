// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AngularGradient (Drawing).

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

public struct AngularGradient {
    var gradient: SwiftUI.Gradient
    var center: SwiftUI.UnitPoint
    var startAngle: SwiftUI.Angle
    var endAngle: SwiftUI.Angle
    public init(colors a0: [SwiftUI.Color], center a1: SwiftUI.UnitPoint, angle a2: SwiftUI.Angle) { _skeleton() }
    public init(stops a0: [SwiftUI.Gradient.Stop], center a1: SwiftUI.UnitPoint, angle a2: SwiftUI.Angle) { _skeleton() }
    public init(colors a0: [SwiftUI.Color], center a1: SwiftUI.UnitPoint, startAngle a2: SwiftUI.Angle, endAngle a3: SwiftUI.Angle) { _skeleton() }
    public init(gradient a0: SwiftUI.Gradient, center a1: SwiftUI.UnitPoint, startAngle a2: SwiftUI.Angle, endAngle a3: SwiftUI.Angle) { _skeleton() }
    public init(stops a0: [SwiftUI.Gradient.Stop], center a1: SwiftUI.UnitPoint, startAngle a2: SwiftUI.Angle, endAngle a3: SwiftUI.Angle) { _skeleton() }
    public var body: SwiftUI._ShapeView<SwiftUI.Rectangle, SwiftUI.AngularGradient> { _skeleton() }
    public init(gradient a0: SwiftUI.Gradient, center a1: SwiftUI.UnitPoint, angle a2: SwiftUI.Angle) { _skeleton() }
}

extension SwiftUI.AngularGradient: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}

extension SwiftUI.AngularGradient: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Rectangle, SwiftUI.AngularGradient>
}
