// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Capsule (Drawing).

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

public struct Capsule {
    public var style: SwiftUI.RoundedCornerStyle
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
    public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.Capsule._Inset { _skeleton() }
    public init(style a0: SwiftUI.RoundedCornerStyle) { _skeleton() }
    public struct _Inset {
        public var amount: CoreGraphics.CGFloat
        public init(amount a0: CoreGraphics.CGFloat) { _skeleton() }
        public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
        public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.Capsule._Inset { _skeleton() }
    }
}

extension SwiftUI.Capsule._Inset: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.Capsule._Inset
}

extension SwiftUI.Capsule._Inset: SwiftUI.Shape {
}

extension SwiftUI.Capsule._Inset: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Capsule._Inset: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Capsule._Inset, SwiftUI.ForegroundStyle>
}

extension SwiftUI.Capsule: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.Capsule._Inset
}

extension SwiftUI.Capsule: SwiftUI.Shape {
}

extension SwiftUI.Capsule: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.Capsule: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.Capsule, SwiftUI.ForegroundStyle>
}
