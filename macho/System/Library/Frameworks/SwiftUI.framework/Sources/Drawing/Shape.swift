// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Shape (Drawing).

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

public protocol Shape: SwiftUI.Animatable, SwiftUI.View {
    static var role: SwiftUI.ShapeRole { get }
    var layoutDirectionBehavior: SwiftUI.LayoutDirectionBehavior { get }
    func path(`in` a0: CGRect) -> SwiftUI.Path
    func sizeThatFits(_ a0: SwiftUI.ProposedViewSize) -> CGSize
}

extension SwiftUI.Shape {
    public static var role: SwiftUI.ShapeRole { _skeleton() }
    public var layoutDirectionBehavior: SwiftUI.LayoutDirectionBehavior { _skeleton() }
    public func sizeThatFits(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    public var body: SwiftUI._ShapeView<Self, SwiftUI.ForegroundStyle> { _skeleton() }
    public static func _makeView(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public static func _makeViewList(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs { _skeleton() }
    public func trim(from a0: CoreGraphics.CGFloat, to a1: CoreGraphics.CGFloat) -> some SwiftUI.Shape { self }
    public func fill<A1>(_ a0: A1, style a1: SwiftUI.FillStyle) -> SwiftUI._ShapeView<Self, A1> where A1: SwiftUI.ShapeStyle { _skeleton() }
    public func fill(style a0: SwiftUI.FillStyle) -> some SwiftUI.View { _skeleton() }
    public func stroke<A1>(_ a0: A1, style a1: SwiftUI.StrokeStyle, antialiased a2: Swift.Bool) -> SwiftUI.StrokeShapeView<Self, A1, SwiftUI.EmptyView> where A1: SwiftUI.ShapeStyle { _skeleton() }
    public func stroke<A1>(_ a0: A1, lineWidth a1: CoreGraphics.CGFloat, antialiased a2: Swift.Bool) -> SwiftUI.StrokeShapeView<Self, A1, SwiftUI.EmptyView> where A1: SwiftUI.ShapeStyle { _skeleton() }
    public func stroke(lineWidth a0: CoreGraphics.CGFloat) -> some SwiftUI.Shape { self }
    public func stroke(style a0: SwiftUI.StrokeStyle) -> some SwiftUI.Shape { self }
    public func offset(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) -> SwiftUI.OffsetShape<Self> { _skeleton() }
    public func rotation(_ a0: SwiftUI.Angle, anchor a1: SwiftUI.UnitPoint) -> SwiftUI.RotatedShape<Self> { _skeleton() }
    public func fill<A1>(_ a0: A1, style a1: SwiftUI.FillStyle) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle { _skeleton() }
}
