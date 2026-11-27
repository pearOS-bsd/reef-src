// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LinearGradient (Drawing).

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

public struct LinearGradient {
    var gradient: SwiftUI.Gradient
    var startPoint: SwiftUI.UnitPoint
    var endPoint: SwiftUI.UnitPoint
    public init(colors a0: [SwiftUI.Color], startPoint a1: SwiftUI.UnitPoint, endPoint a2: SwiftUI.UnitPoint) { _skeleton() }
    public init(stops a0: [SwiftUI.Gradient.Stop], startPoint a1: SwiftUI.UnitPoint, endPoint a2: SwiftUI.UnitPoint) { _skeleton() }
    public var body: SwiftUI._ShapeView<SwiftUI.Rectangle, SwiftUI.LinearGradient> { _skeleton() }
    public init(gradient a0: SwiftUI.Gradient, startPoint a1: SwiftUI.UnitPoint, endPoint a2: SwiftUI.UnitPoint) { _skeleton() }
}

extension SwiftUI.LinearGradient: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}

extension SwiftUI.LinearGradient: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Rectangle, SwiftUI.LinearGradient>
}
