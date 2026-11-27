// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Color (Drawing).

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

public struct Color {
    public var provider: SwiftUI.AnyColorBox
    public init(_ a0: CGColor) { _skeleton() }
    public func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
    public init(white a0: Swift.Double, opacity a1: Swift.Double) { _skeleton() }
    public init(nsColor a0: NSColor) { _skeleton() }
    public static var purple: SwiftUI.Color { _skeleton() }
    public init(_ a0: Swift.String, bundle a1: Bundle?) { _skeleton() }
    public init(_ a0: SwiftUI.Color.Resolved) { _skeleton() }
    public init(red a0: Swift.Double, green a1: Swift.Double, blue a2: Swift.Double, opacity a3: Swift.Double) { _skeleton() }
    public init<A>(_ a0: A) where A: Swift.Hashable, A: SwiftUI.ShapeStyle, A.Resolved == SwiftUI.Color.Resolved { _skeleton() }
    public static func == (_ a0: SwiftUI.Color, _ a1: SwiftUI.Color) -> Swift.Bool { _skeleton() }
    public static var black: SwiftUI.Color { _skeleton() }
    public func opacity(_ a0: Swift.Double) -> SwiftUI.Color { _skeleton() }
    public static var clear: SwiftUI.Color { _skeleton() }
    public init(_ a0: SwiftUI.Color.RGBColorSpace, white a1: Swift.Double, opacity a2: Swift.Double) { _skeleton() }
    public init(_ a0: SwiftUI.Color.RGBColorSpace, red a1: Swift.Double, green a2: Swift.Double, blue a3: Swift.Double, opacity a4: Swift.Double) { _skeleton() }
    public init(hue a0: Swift.Double, saturation a1: Swift.Double, brightness a2: Swift.Double, opacity a3: Swift.Double) { _skeleton() }
    public static var transferRepresentation: CoreTransferable.DataRepresentation<SwiftUI.Color> { _skeleton() }
    public init(_ a0: DeveloperToolsSupport.ColorResource) { _skeleton() }
    public func mix(with a0: SwiftUI.Color, by a1: Swift.Double, `in` a2: SwiftUI.Gradient.ColorSpace) -> SwiftUI.Color { _skeleton() }
    public static var pink: SwiftUI.Color { _skeleton() }
    public static var mint: SwiftUI.Color { _skeleton() }
    public static var teal: SwiftUI.Color { _skeleton() }
    public static var cyan: SwiftUI.Color { _skeleton() }
    public static var indigo: SwiftUI.Color { _skeleton() }
    public static var brown: SwiftUI.Color { _skeleton() }
    public var gradient: SwiftUI.AnyGradient { _skeleton() }
    public static var red: SwiftUI.Color { _skeleton() }
    public static var orange: SwiftUI.Color { _skeleton() }
    public static var yellow: SwiftUI.Color { _skeleton() }
    public static var green: SwiftUI.Color { _skeleton() }
    public static var blue: SwiftUI.Color { _skeleton() }
    public static var white: SwiftUI.Color { _skeleton() }
    public static var gray: SwiftUI.Color { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public static var primary: SwiftUI.Color { _skeleton() }
    public static var secondary: SwiftUI.Color { _skeleton() }
    public static var accentColor: SwiftUI.Color { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public init(_ a0: NSColor) { _skeleton() }
    public struct Resolved {
        public var linearRed: Swift.Float
        public var linearGreen: Swift.Float
        public var linearBlue: Swift.Float
        public var opacity: Swift.Float
        public var cgColor: CGColor { _skeleton() }
        public init(colorSpace a0: SwiftUI.Color.RGBColorSpace, red a1: Swift.Float, green a2: Swift.Float, blue a3: Swift.Float, opacity a4: Swift.Float) { _skeleton() }
        var nsColor: NSColor { _skeleton() }
        public var red: Swift.Float { _skeleton() }
        public var green: Swift.Float { _skeleton() }
        public var blue: Swift.Float { _skeleton() }
        public static func == (_ a0: SwiftUI.Color.Resolved, _ a1: SwiftUI.Color.Resolved) -> Swift.Bool { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
        init(_ a0: NSColor, colorScheme a1: SwiftUI.ColorScheme) {
            // the components in sRGB (else device RGB, else black) under the scheme's appearance, made linear
            var red: CoreGraphics.CGFloat = 0, green: CoreGraphics.CGFloat = 0, blue: CoreGraphics.CGFloat = 0
            var alpha: CoreGraphics.CGFloat = 1
            let read = {
                let rgb = a0.usingColorSpace(.sRGB) ?? a0.usingColorSpace(.deviceRGB) ?? NSColor.black
                rgb.getRed(&red, green: &green, blue: &blue, alpha: &alpha)
            }
            if let appearance = a1 == .dark ? SwiftUI.SchemeAppearance.dark : SwiftUI.SchemeAppearance.light {
                appearance.performAsCurrentDrawingAppearance(read)
            } else {
                read()
            }
            func linear(_ c: Swift.Float) -> Swift.Float { c <= 0.04045 ? c / 12.92 : powf((c + 0.055) / 1.055, 2.4) }
            linearRed = linear(Float(red))
            linearGreen = linear(Float(green))
            linearBlue = linear(Float(blue))
            opacity = Float(alpha)
        }
    }
    public enum RGBColorSpace {
        case sRGB
        case sRGBLinear
        case displayP3
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Color.RGBColorSpace, _ a1: SwiftUI.Color.RGBColorSpace) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
}

extension SwiftUI.Color: SwiftUI.PrimitiveView {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.Color: CoreTransferable.Transferable {
    public typealias Representation = CoreTransferable.DataRepresentation<SwiftUI.Color>
}

extension SwiftUI.Color: Swift.Hashable {
}

extension SwiftUI.Color: SwiftUI.ShapeStyle {
}

extension SwiftUI.Color: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Color: Swift.Equatable {
}

extension SwiftUI.Color.Resolved: Swift.Hashable {
}

extension SwiftUI.Color.Resolved: Swift.Equatable {
}

extension SwiftUI.Color.Resolved: SwiftUI.ShapeStyle {
    public typealias Resolved = SwiftUI.Color.Resolved
}

extension SwiftUI.Color.RGBColorSpace: Swift.Equatable {
}

extension SwiftUI.Color.RGBColorSpace: Swift.Hashable {
}
