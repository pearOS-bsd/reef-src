// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StrokeShapeView (Drawing).

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

public struct StrokeShapeView<A, B, C> where A: SwiftUI.Shape, B: SwiftUI.ShapeStyle, C: SwiftUI.View {
    public var view: SwiftUI.ModifiedContent<SwiftUI._ShapeView<SwiftUI._StrokedShape<A>, B>, SwiftUI._BackgroundModifier<C>>
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
    public var shape: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var isAntialiased: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
}

extension SwiftUI.StrokeShapeView: SwiftUI.PrimitiveView {
}

extension SwiftUI.StrokeShapeView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.StrokeShapeView: SwiftUI.ShapeView {
    public typealias Content = A
}
