// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// RotatedShape (Gestures).

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

public struct RotatedShape<A> where A: SwiftUI.Shape {
    public var shape: A
    public var angle: SwiftUI.Angle
    public var anchor: SwiftUI.UnitPoint
    public init(shape a0: A, angle a1: SwiftUI.Angle, anchor a2: SwiftUI.UnitPoint) { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
}

extension SwiftUI.RotatedShape where A: SwiftUI.InsettableShape {
    public func inset(by a0: CoreGraphics.CGFloat) -> SwiftUI.RotatedShape<A.InsetShape> { _skeleton() }
}

extension SwiftUI.RotatedShape: SwiftUI.Shape {
}

extension SwiftUI.RotatedShape: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.RotatedShape: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI.RotatedShape<A>, SwiftUI.ForegroundStyle>
}

extension SwiftUI.RotatedShape: SwiftUI.InsettableShape where A: SwiftUI.InsettableShape {
    public typealias InsetShape = SwiftUI.RotatedShape<A.InsetShape>
}
