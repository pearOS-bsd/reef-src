// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StrokeBorderShapeView (Drawing).

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

public struct StrokeBorderShapeView<A, B, C> where A: SwiftUI.InsettableShape, B: SwiftUI.ShapeStyle, C: SwiftUI.View {
    public var shape: A
    var view: SwiftUI.ModifiedContent<SwiftUI._ShapeView<SwiftUI._StrokedShape<A.InsetShape>, B>, SwiftUI._BackgroundModifier<C>>
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public var style: B {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var strokeStyle: SwiftUI.StrokeStyle {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var background: C {
        get { _skeleton() }
        set { _skeleton() }
    }
    public init(shape a0: A, style a1: B, strokeStyle a2: SwiftUI.StrokeStyle, isAntialiased a3: Swift.Bool, background a4: C) { _skeleton() }
    public var isAntialiased: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
}

extension SwiftUI.StrokeBorderShapeView: SwiftUI.PrimitiveView {
}

extension SwiftUI.StrokeBorderShapeView: SwiftUI.ShapeView {
    public typealias Content = A
}

extension SwiftUI.StrokeBorderShapeView: SwiftUI.View {
    public typealias Body = Swift.Never
}
