// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// UnevenRoundedRectangle (Drawing).

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

public struct UnevenRoundedRectangle {
    public var cornerRadii: SwiftUI.RectangleCornerRadii
    public var style: SwiftUI.RoundedCornerStyle
    public init(cornerRadii a0: SwiftUI.RectangleCornerRadii, style a1: SwiftUI.RoundedCornerStyle) { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
    public init(topLeadingRadius a0: CoreGraphics.CGFloat, bottomLeadingRadius a1: CoreGraphics.CGFloat, bottomTrailingRadius a2: CoreGraphics.CGFloat, topTrailingRadius a3: CoreGraphics.CGFloat, style a4: SwiftUI.RoundedCornerStyle) { _skeleton() }
}

extension SwiftUI.UnevenRoundedRectangle: SwiftUI.Shape {
}

extension SwiftUI.UnevenRoundedRectangle: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.UnevenRoundedRectangle: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.UnevenRoundedRectangle, SwiftUI.ForegroundStyle>
}
