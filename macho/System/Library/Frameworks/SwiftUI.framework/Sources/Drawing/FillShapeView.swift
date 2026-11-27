// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FillShapeView (Drawing).

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

public struct FillShapeView<A, B, C> where A: SwiftUI.Shape, B: SwiftUI.ShapeStyle, C: SwiftUI.View {
    public var view: SwiftUI.ModifiedContent<SwiftUI._ShapeView<A, B>, SwiftUI._BackgroundModifier<C>>
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public var style: B {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var fillStyle: SwiftUI.FillStyle {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var background: C {
        get { _skeleton() }
        set { _skeleton() }
    }
    public init(shape a0: A, style a1: B, fillStyle a2: SwiftUI.FillStyle, background a3: C) { _skeleton() }
    public var shape: A {
        get { _skeleton() }
        set { _skeleton() }
    }
}

extension SwiftUI.FillShapeView: SwiftUI.PrimitiveView {
}

extension SwiftUI.FillShapeView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.FillShapeView: SwiftUI.ShapeView {
    public typealias Content = A
}
