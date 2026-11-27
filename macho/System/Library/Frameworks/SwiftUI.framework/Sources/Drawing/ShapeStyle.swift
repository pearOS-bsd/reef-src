// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShapeStyle (Drawing).

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

public protocol ShapeStyle {
    associatedtype Resolved: SwiftUI.ShapeStyle
    static func _makeView<A1>(view a0: SwiftUI._GraphValue<SwiftUI._ShapeView<A1, Self>>, inputs a1: SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs where A1: SwiftUI.Shape
    func _apply(to a0: inout SwiftUI._ShapeStyle_Shape)
    static func _apply(to a0: inout SwiftUI._ShapeStyle_ShapeType)
    func resolve(`in` a0: SwiftUI.EnvironmentValues) -> Self.Resolved
}

extension SwiftUI.ShapeStyle {
    public static func _makeView<A1>(view a0: SwiftUI._GraphValue<SwiftUI._ShapeView<A1, Self>>, inputs a1: SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs where A1: SwiftUI.Shape { _skeleton() }
    public func _apply(to a0: inout SwiftUI._ShapeStyle_Shape) { _skeleton() }
    public static func _apply(to a0: inout SwiftUI._ShapeStyle_ShapeType) { _skeleton() }
    public var secondary: some SwiftUI.ShapeStyle { self }
    public func shadow(_ a0: SwiftUI.ShadowStyle) -> some SwiftUI.ShapeStyle { self }
    public var tertiary: some SwiftUI.ShapeStyle { self }
    public var quaternary: some SwiftUI.ShapeStyle { self }
    public var quinary: some SwiftUI.ShapeStyle { self }
    public func opacity(_ a0: Swift.Double) -> some SwiftUI.ShapeStyle { self }
}

extension SwiftUI.ShapeStyle where Self.Resolved == Swift.Never {
    public func resolve(`in` a0: SwiftUI.EnvironmentValues) -> Swift.Never { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.LinearGradient {
    public static func linearGradient(colors a0: [SwiftUI.Color], startPoint a1: SwiftUI.UnitPoint, endPoint a2: SwiftUI.UnitPoint) -> SwiftUI.LinearGradient { _skeleton() }
    public static func linearGradient(stops a0: [SwiftUI.Gradient.Stop], startPoint a1: SwiftUI.UnitPoint, endPoint a2: SwiftUI.UnitPoint) -> SwiftUI.LinearGradient { _skeleton() }
    public static func linearGradient(_ a0: SwiftUI.Gradient, startPoint a1: SwiftUI.UnitPoint, endPoint a2: SwiftUI.UnitPoint) -> SwiftUI.LinearGradient { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.RadialGradient {
    public static func radialGradient(_ a0: SwiftUI.Gradient, center a1: SwiftUI.UnitPoint, startRadius a2: CoreGraphics.CGFloat, endRadius a3: CoreGraphics.CGFloat) -> SwiftUI.RadialGradient { _skeleton() }
    public static func radialGradient(colors a0: [SwiftUI.Color], center a1: SwiftUI.UnitPoint, startRadius a2: CoreGraphics.CGFloat, endRadius a3: CoreGraphics.CGFloat) -> SwiftUI.RadialGradient { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.AngularGradient {
    public static func conicGradient(_ a0: SwiftUI.Gradient, center a1: SwiftUI.UnitPoint, angle a2: SwiftUI.Angle) -> SwiftUI.AngularGradient { _skeleton() }
    public static func angularGradient(_ a0: SwiftUI.Gradient, center a1: SwiftUI.UnitPoint, startAngle a2: SwiftUI.Angle, endAngle a3: SwiftUI.Angle) -> SwiftUI.AngularGradient { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.EllipticalGradient {
    public static func ellipticalGradient(_ a0: SwiftUI.Gradient, center a1: SwiftUI.UnitPoint, startRadiusFraction a2: CoreGraphics.CGFloat, endRadiusFraction a3: CoreGraphics.CGFloat) -> SwiftUI.EllipticalGradient { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.BackgroundStyle {
    public static var background: SwiftUI.BackgroundStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.ForegroundStyle {
    public static var foreground: SwiftUI.ForegroundStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.HierarchicalShapeStyle {
    public static var quaternary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
    public static var primary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
    public static var quinary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
    public static var tertiary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
    public static var secondary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.PlaceholderTextShapeStyle {
    public static var placeholder: SwiftUI.PlaceholderTextShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.WindowBackgroundShapeStyle {
    public static var windowBackground: SwiftUI.WindowBackgroundShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.FillShapeStyle {
    public static var fill: SwiftUI.FillShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.LinkShapeStyle {
    public static var link: SwiftUI.LinkShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.TintShapeStyle {
    public static var tint: SwiftUI.TintShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.SelectionShapeStyle {
    public static var selection: SwiftUI.SelectionShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.SeparatorShapeStyle {
    public static var separator: SwiftUI.SeparatorShapeStyle { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.Color {
    public static var red: SwiftUI.Color { _skeleton() }
    public static var blue: SwiftUI.Color { _skeleton() }
    public static var cyan: SwiftUI.Color { _skeleton() }
    public static var gray: SwiftUI.Color { _skeleton() }
    public static var mint: SwiftUI.Color { _skeleton() }
    public static var pink: SwiftUI.Color { _skeleton() }
    public static var teal: SwiftUI.Color { _skeleton() }
    public static var black: SwiftUI.Color { _skeleton() }
    public static var brown: SwiftUI.Color { _skeleton() }
    public static var clear: SwiftUI.Color { _skeleton() }
    public static var green: SwiftUI.Color { _skeleton() }
    public static var white: SwiftUI.Color { _skeleton() }
    public static var indigo: SwiftUI.Color { _skeleton() }
    public static var orange: SwiftUI.Color { _skeleton() }
    public static var purple: SwiftUI.Color { _skeleton() }
    public static var yellow: SwiftUI.Color { _skeleton() }
}

extension SwiftUI.ShapeStyle where Self == SwiftUI.Material {
    public static var ultraThickMaterial: SwiftUI.Material { _skeleton() }
    public static var thinMaterial: SwiftUI.Material { _skeleton() }
    public static var thickMaterial: SwiftUI.Material { _skeleton() }
    public static var regularMaterial: SwiftUI.Material { _skeleton() }
    public static var ultraThinMaterial: SwiftUI.Material { _skeleton() }
    public static var bar: SwiftUI.Material { _skeleton() }
}
