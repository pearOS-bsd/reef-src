// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Ellipse (Drawing).

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

public struct Ellipse {
    public init() { _skeleton() }
    public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.Ellipse._Inset { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
    public struct _Inset {
        public var amount: CoreGraphics.CGFloat
        public init(amount a0: CoreGraphics.CGFloat) { _skeleton() }
        public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
        public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.Ellipse._Inset { _skeleton() }
    }
}

extension SwiftUI.Ellipse: SwiftUI.Shape {
}

extension SwiftUI.Ellipse: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Ellipse: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Ellipse, SwiftUI.ForegroundStyle>
}

extension SwiftUI.Ellipse: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.Ellipse._Inset
}

extension SwiftUI.Ellipse._Inset: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.Ellipse._Inset
}

extension SwiftUI.Ellipse._Inset: SwiftUI.Shape {
}

extension SwiftUI.Ellipse._Inset: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Ellipse._Inset: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Ellipse._Inset, SwiftUI.ForegroundStyle>
}
