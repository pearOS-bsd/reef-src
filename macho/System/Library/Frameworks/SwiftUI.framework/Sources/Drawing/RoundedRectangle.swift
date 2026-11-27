// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// RoundedRectangle (Drawing).

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

public struct RoundedRectangle {
    public var cornerSize: CGSize
    public var style: SwiftUI.RoundedCornerStyle
    public init(cornerSize a0: CGSize, style a1: SwiftUI.RoundedCornerStyle) { _skeleton() }
    public init(cornerRadius a0: CoreGraphics.CGFloat, style a1: SwiftUI.RoundedCornerStyle) { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
    public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.RoundedRectangle._Inset { _skeleton() }
    public struct _Inset {
        public var base: SwiftUI.RoundedRectangle
        public var amount: CoreGraphics.CGFloat
        public init(base a0: SwiftUI.RoundedRectangle, amount a1: CoreGraphics.CGFloat) { _skeleton() }
        public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
        public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.RoundedRectangle._Inset { _skeleton() }
    }
}

extension SwiftUI.RoundedRectangle: SwiftUI.Shape {
}

extension SwiftUI.RoundedRectangle: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.RoundedRectangle: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.RoundedRectangle, SwiftUI.ForegroundStyle>
}

extension SwiftUI.RoundedRectangle: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.RoundedRectangle._Inset
}

extension SwiftUI.RoundedRectangle._Inset: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.RoundedRectangle._Inset
}

extension SwiftUI.RoundedRectangle._Inset: SwiftUI.Shape {
}

extension SwiftUI.RoundedRectangle._Inset: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.RoundedRectangle._Inset: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.RoundedRectangle._Inset, SwiftUI.ForegroundStyle>
}
