// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GraphicsContext (Graph).

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

public struct GraphicsContext {
    public var opacity: Swift.Double
    public var blendMode: SwiftUI.GraphicsContext.BlendMode
    public var environment: SwiftUI.EnvironmentValues
    public var transform: CGAffineTransform
    var target: SwiftUI.GraphicsTarget?
    var clips: [(SwiftUI.Path, SwiftUI.FillStyle)]
    var filters: [(SwiftUI.GraphicsContext.Filter, SwiftUI.GraphicsContext.FilterOptions)]
    public var clipBoundingRect: CGRect { _skeleton() }
    public func scaleBy(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) { _skeleton() }
    public func translateBy(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) { _skeleton() }
    public func rotate(by a0: SwiftUI.Angle) { _skeleton() }
    public func concatenate(_ a0: CGAffineTransform) { _skeleton() }
    public func clip(to a0: SwiftUI.Path, style a1: SwiftUI.FillStyle, options a2: SwiftUI.GraphicsContext.ClipOptions) { _skeleton() }
    public func fill(_ a0: SwiftUI.Path, with a1: SwiftUI.GraphicsContext.Shading, style a2: SwiftUI.FillStyle) { _skeleton() }
    public func stroke(_ a0: SwiftUI.Path, with a1: SwiftUI.GraphicsContext.Shading, style a2: SwiftUI.StrokeStyle) { _skeleton() }
    public func stroke(_ a0: SwiftUI.Path, with a1: SwiftUI.GraphicsContext.Shading, lineWidth a2: CoreGraphics.CGFloat) { _skeleton() }
    public func draw(_ a0: SwiftUI.Image, `in` a1: CGRect, style a2: SwiftUI.FillStyle) { _skeleton() }
    public func draw(_ a0: SwiftUI.Image, at a1: CGPoint, anchor a2: SwiftUI.UnitPoint) { _skeleton() }
    public func draw(_ a0: SwiftUI.Text, at a1: CGPoint, anchor a2: SwiftUI.UnitPoint) { _skeleton() }
    public func draw(_ a0: SwiftUI.Text, `in` a1: CGRect) { _skeleton() }
    public func draw(_ a0: SwiftUI.GraphicsContext.ResolvedSymbol, at a1: CGPoint, anchor a2: SwiftUI.UnitPoint) { _skeleton() }
    public func resolveSymbol<A>(id a0: A) -> SwiftUI.GraphicsContext.ResolvedSymbol? where A: Swift.Hashable { _skeleton() }
    public func addFilter(_ a0: SwiftUI.GraphicsContext.Filter, options a1: SwiftUI.GraphicsContext.FilterOptions) { _skeleton() }
    public func drawLayer(content a0: (inout SwiftUI.GraphicsContext) throws -> ()) throws { _skeleton() }
    public struct GradientOptions {
        public let rawValue: Swift.UInt32
        public init(rawValue a0: Swift.UInt32) { _skeleton() }
        public static var `repeat`: SwiftUI.GraphicsContext.GradientOptions { _skeleton() }
        public static var mirror: SwiftUI.GraphicsContext.GradientOptions { _skeleton() }
        public static var linearColor: SwiftUI.GraphicsContext.GradientOptions { _skeleton() }
    }
    public struct Shading {
        let style: SwiftUI.AnyShapeStyle
        var placed: ((CGRect) -> SwiftUI.AnyShapeStyle)?
        public static var foreground: SwiftUI.GraphicsContext.Shading { _skeleton() }
        public static func color(_ a0: SwiftUI.Color) -> SwiftUI.GraphicsContext.Shading { _skeleton() }
        public static func style<A>(_ a0: A) -> SwiftUI.GraphicsContext.Shading where A: SwiftUI.ShapeStyle { _skeleton() }
        public static func linearGradient(_ a0: SwiftUI.Gradient, startPoint a1: CGPoint, endPoint a2: CGPoint, options a3: SwiftUI.GraphicsContext.GradientOptions) -> SwiftUI.GraphicsContext.Shading { _skeleton() }
        public static func radialGradient(_ a0: SwiftUI.Gradient, center a1: CGPoint, startRadius a2: CoreGraphics.CGFloat, endRadius a3: CoreGraphics.CGFloat, options a4: SwiftUI.GraphicsContext.GradientOptions) -> SwiftUI.GraphicsContext.Shading { _skeleton() }
    }
    public struct ResolvedSymbol {
        public var size: CGSize { _skeleton() }
    }
    public struct ClipOptions {
        public let rawValue: Swift.UInt32
        public init(rawValue a0: Swift.UInt32) { _skeleton() }
        public static var inverse: SwiftUI.GraphicsContext.ClipOptions { _skeleton() }
    }
    public struct BlendMode {
        public let rawValue: Swift.Int32
        public static var normal: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public init(rawValue a0: Swift.Int32) { _skeleton() }
        public static var multiply: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var screen: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var overlay: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var darken: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var lighten: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var colorDodge: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var colorBurn: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var softLight: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var hardLight: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var difference: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var exclusion: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var hue: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var saturation: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var color: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var luminosity: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var clear: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var copy: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var sourceIn: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var sourceOut: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var sourceAtop: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var destinationOver: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var destinationIn: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var destinationOut: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var destinationAtop: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var xor: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var plusDarker: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
        public static var plusLighter: SwiftUI.GraphicsContext.BlendMode { _skeleton() }
    }
    public struct BlurOptions {
        public let rawValue: Swift.UInt32
        public init(rawValue a0: Swift.UInt32) { _skeleton() }
        public static var opaque: SwiftUI.GraphicsContext.BlurOptions { _skeleton() }
        public static var dithersResult: SwiftUI.GraphicsContext.BlurOptions { _skeleton() }
    }
    public struct ShadowOptions {
        public let rawValue: Swift.UInt32
        public static var invertsAlpha: SwiftUI.GraphicsContext.ShadowOptions { _skeleton() }
        public static var disablesGroup: SwiftUI.GraphicsContext.ShadowOptions { _skeleton() }
        public init(rawValue a0: Swift.UInt32) { _skeleton() }
        public static var shadowAbove: SwiftUI.GraphicsContext.ShadowOptions { _skeleton() }
        public static var shadowOnly: SwiftUI.GraphicsContext.ShadowOptions { _skeleton() }
    }
    public struct FilterOptions {
        public let rawValue: Swift.UInt32
        public init(rawValue a0: Swift.UInt32) { _skeleton() }
        public static var linearColor: SwiftUI.GraphicsContext.FilterOptions { _skeleton() }
    }
    public struct Filter {
        let kind: SwiftUI.GraphicsContext.Filter.Kind
        public static func blur(radius a0: CoreGraphics.CGFloat, options a1: SwiftUI.GraphicsContext.BlurOptions) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func shadow(color a0: SwiftUI.Color, radius a1: CoreGraphics.CGFloat, x a2: CoreGraphics.CGFloat, y a3: CoreGraphics.CGFloat, blendMode a4: SwiftUI.GraphicsContext.BlendMode, options a5: SwiftUI.GraphicsContext.ShadowOptions) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func brightness(_ a0: Swift.Double) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func contrast(_ a0: Swift.Double) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func saturation(_ a0: Swift.Double) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func grayscale(_ a0: Swift.Double) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func hueRotation(_ a0: SwiftUI.Angle) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static var colorInvert: SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func colorInvert(_ a0: Swift.Double) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static var luminanceToAlpha: SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func colorMultiply(_ a0: SwiftUI.Color) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        public static func alphaThreshold(min a0: Swift.Double, max a1: Swift.Double, color a2: SwiftUI.Color) -> SwiftUI.GraphicsContext.Filter { _skeleton() }
        enum Kind {
            case blur(CoreGraphics.CGFloat, SwiftUI.GraphicsContext.BlurOptions)
            case shadow(SwiftUI.Color, CoreGraphics.CGFloat, CGSize, SwiftUI.GraphicsContext.BlendMode, SwiftUI.GraphicsContext.ShadowOptions)
            case matrix(SwiftUI.ColorMatrix)
            case alphaThreshold(Swift.Double, SwiftUI.Color)
        }
    }
}

extension SwiftUI.GraphicsContext.GradientOptions: Swift.OptionSet {
    public typealias Element = SwiftUI.GraphicsContext.GradientOptions
}

extension SwiftUI.GraphicsContext.GradientOptions: Swift.SetAlgebra {
}

extension SwiftUI.GraphicsContext.GradientOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt32
}

extension SwiftUI.GraphicsContext.GradientOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.GraphicsContext.GradientOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.GraphicsContext.GradientOptions
}

extension SwiftUI.GraphicsContext.ClipOptions: Swift.OptionSet {
    public typealias Element = SwiftUI.GraphicsContext.ClipOptions
}

extension SwiftUI.GraphicsContext.ClipOptions: Swift.SetAlgebra {
}

extension SwiftUI.GraphicsContext.ClipOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt32
}

extension SwiftUI.GraphicsContext.ClipOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.GraphicsContext.ClipOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.GraphicsContext.ClipOptions
}

extension SwiftUI.GraphicsContext.BlendMode: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int32
}

extension SwiftUI.GraphicsContext.BlendMode: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.GraphicsContext.BlurOptions: Swift.OptionSet {
    public typealias Element = SwiftUI.GraphicsContext.BlurOptions
}

extension SwiftUI.GraphicsContext.BlurOptions: Swift.SetAlgebra {
}

extension SwiftUI.GraphicsContext.BlurOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt32
}

extension SwiftUI.GraphicsContext.BlurOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.GraphicsContext.BlurOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.GraphicsContext.BlurOptions
}

extension SwiftUI.GraphicsContext.ShadowOptions: Swift.OptionSet {
    public typealias Element = SwiftUI.GraphicsContext.ShadowOptions
}

extension SwiftUI.GraphicsContext.ShadowOptions: Swift.SetAlgebra {
}

extension SwiftUI.GraphicsContext.ShadowOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt32
}

extension SwiftUI.GraphicsContext.ShadowOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.GraphicsContext.ShadowOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.GraphicsContext.ShadowOptions
}

extension SwiftUI.GraphicsContext.FilterOptions: Swift.OptionSet {
    public typealias Element = SwiftUI.GraphicsContext.FilterOptions
}

extension SwiftUI.GraphicsContext.FilterOptions: Swift.SetAlgebra {
}

extension SwiftUI.GraphicsContext.FilterOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt32
}

extension SwiftUI.GraphicsContext.FilterOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.GraphicsContext.FilterOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.GraphicsContext.FilterOptions
}
