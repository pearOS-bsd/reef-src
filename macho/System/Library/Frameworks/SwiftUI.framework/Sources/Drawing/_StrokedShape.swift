// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _StrokedShape (Drawing).

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

public struct _StrokedShape<A> where A: SwiftUI.Shape {
    public var shape: A
    public var style: SwiftUI.StrokeStyle
    func strokeElement(paint a0: SwiftUI.Paint) -> SwiftUI.ShapeElement { _skeleton() }
    public init(shape a0: A, style a1: SwiftUI.StrokeStyle) { _skeleton() }
    public func path(`in` a0: CGRect) -> SwiftUI.Path { _skeleton() }
}

extension SwiftUI._StrokedShape: SwiftUI.AnyStrokedShape {
}

extension SwiftUI._StrokedShape: SwiftUI.Shape {
}

extension SwiftUI._StrokedShape: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI._StrokedShape: SwiftUI.View {
    public typealias Body = SwiftUI._ShapeView<SwiftUI._StrokedShape<A>, SwiftUI.ForegroundStyle>
}
