// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShapeView (Drawing).

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

public protocol ShapeView: SwiftUI.View {
    associatedtype Content: SwiftUI.Shape
    var shape: Self.Content { get }
}

extension SwiftUI.ShapeView {
    public func fill<A1>(_ a0: A1, style a1: SwiftUI.FillStyle) -> SwiftUI.FillShapeView<Self.Content, A1, Self> where A1: SwiftUI.ShapeStyle { _skeleton() }
    public func stroke<A1>(_ a0: A1, style a1: SwiftUI.StrokeStyle, antialiased a2: Swift.Bool) -> SwiftUI.StrokeShapeView<Self.Content, A1, Self> where A1: SwiftUI.ShapeStyle { _skeleton() }
    public func stroke<A1>(_ a0: A1, lineWidth a1: CoreGraphics.CGFloat, antialiased a2: Swift.Bool) -> SwiftUI.StrokeShapeView<Self.Content, A1, Self> where A1: SwiftUI.ShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeView where Self.Content: SwiftUI.InsettableShape {
    public func strokeBorder<A1>(_ a0: A1, style a1: SwiftUI.StrokeStyle, antialiased a2: Swift.Bool) -> SwiftUI.StrokeBorderShapeView<Self.Content, A1, Self> where A1: SwiftUI.ShapeStyle { _skeleton() }
    public func strokeBorder<A1>(_ a0: A1, lineWidth a1: CoreGraphics.CGFloat, antialiased a2: Swift.Bool) -> SwiftUI.StrokeBorderShapeView<Self.Content, A1, Self> where A1: SwiftUI.ShapeStyle { _skeleton() }
}
