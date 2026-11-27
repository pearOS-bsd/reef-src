// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Circle (Drawing).

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

public struct Circle {
    public init() { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
    public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.Circle._Inset { _skeleton() }
    public struct _Inset {
        public var amount: CoreGraphics.CGFloat
        public init(amount a0: CoreGraphics.CGFloat) { _skeleton() }
        public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
        public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.Circle._Inset { _skeleton() }
    }
}

extension SwiftUI.Circle: SwiftUI.Shape {
}

extension SwiftUI.Circle: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Circle: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Circle, SwiftUI.ForegroundStyle>
}

extension SwiftUI.Circle: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.Circle._Inset
}

extension SwiftUI.Circle._Inset: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.Circle._Inset
}

extension SwiftUI.Circle._Inset: SwiftUI.Shape {
}

extension SwiftUI.Circle._Inset: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Circle._Inset: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Circle._Inset, SwiftUI.ForegroundStyle>
}
