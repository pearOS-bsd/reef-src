// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ConcentricRectangle (Drawing).

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

public struct ConcentricRectangle {
    let corners: SwiftUI.Edge.Corner.Style
    let isUniform: Swift.Bool
    public init(corners a0: SwiftUI.Edge.Corner.Style, isUniform a1: Swift.Bool) { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
}

extension SwiftUI.ConcentricRectangle: SwiftUI.Shape {
}

extension SwiftUI.ConcentricRectangle: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.ConcentricRectangle: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.ConcentricRectangle, SwiftUI.ForegroundStyle>
}
