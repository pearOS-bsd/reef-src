// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Rectangle (Drawing).

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

public struct Rectangle {
    public init() { _skeleton() }
    public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.Rectangle._Inset { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
    public struct _Inset {
        public var amount: CoreGraphics.CGFloat
        public init(amount a0: CoreGraphics.CGFloat) { _skeleton() }
        public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
        public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.Rectangle._Inset { _skeleton() }
    }
}

extension SwiftUI.Rectangle: SwiftUI.Shape {
}

extension SwiftUI.Rectangle: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Rectangle: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Rectangle, SwiftUI.ForegroundStyle>
}

extension SwiftUI.Rectangle: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.Rectangle._Inset
}

extension SwiftUI.Rectangle._Inset: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.Rectangle._Inset
}

extension SwiftUI.Rectangle._Inset: SwiftUI.Shape {
}

extension SwiftUI.Rectangle._Inset: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Rectangle._Inset: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Rectangle._Inset, SwiftUI.ForegroundStyle>
}
