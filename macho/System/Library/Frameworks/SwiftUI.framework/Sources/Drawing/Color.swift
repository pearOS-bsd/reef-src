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
    init(provider a0: SwiftUI.AnyColorBox) { provider = a0 }
    /// a system colour: sRGB components 0-255 in the light and the dark appearance
    static func system(_ a0: (Swift.Double, Swift.Double, Swift.Double), _ a1: (Swift.Double, Swift.Double, Swift.Double)) -> SwiftUI.Color {
        Color(provider: SwiftUI.SystemColorBox(light: (red: a0.0, green: a0.1, blue: a0.2), dark: (red: a1.0, green: a1.1, blue: a1.2)))
    }
    /// a colour of AppKit by its class method name
    static func platform(_ a0: NSColor) -> SwiftUI.Color { Color(provider: SwiftUI.PlatformColorBox(a0)) }
    public init(_ a0: CGColor) { _skeleton() }
    public func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
    public init(white a0: Swift.Double, opacity a1: Swift.Double) { _skeleton() }
    public init(nsColor a0: NSColor) { _skeleton() }
    public static var purple: SwiftUI.Color { system((203.0, 48.0, 224.0), (219.0, 52.0, 242.0)) }
    public init(_ a0: Swift.String, bundle a1: Bundle?) { _skeleton() }
    public init(_ a0: SwiftUI.Color.Resolved) { _skeleton() }
    public init(red a0: Swift.Double, green a1: Swift.Double, blue a2: Swift.Double, opacity a3: Swift.Double) { _skeleton() }
    public init<A>(_ a0: A) where A: Swift.Hashable, A: SwiftUI.ShapeStyle, A.Resolved == SwiftUI.Color.Resolved { _skeleton() }
    public static func == (_ a0: SwiftUI.Color, _ a1: SwiftUI.Color) -> Swift.Bool { _skeleton() }
    public static var black: SwiftUI.Color { Color(provider: SwiftUI.ResolvedColorBox(red: 0, green: 0, blue: 0, opacity: 1)) }
    public func opacity(_ a0: Swift.Double) -> SwiftUI.Color { _skeleton() }
    public static var clear: SwiftUI.Color { Color(provider: SwiftUI.ResolvedColorBox(red: 0, green: 0, blue: 0, opacity: 0)) }
    public init(_ a0: SwiftUI.Color.RGBColorSpace, white a1: Swift.Double, opacity a2: Swift.Double) { _skeleton() }
    public init(_ a0: SwiftUI.Color.RGBColorSpace, red a1: Swift.Double, green a2: Swift.Double, blue a3: Swift.Double, opacity a4: Swift.Double) { _skeleton() }
    public init(hue a0: Swift.Double, saturation a1: Swift.Double, brightness a2: Swift.Double, opacity a3: Swift.Double) { _skeleton() }
    public static var transferRepresentation: CoreTransferable.DataRepresentation<SwiftUI.Color> { _skeleton() }
    public init(_ a0: DeveloperToolsSupport.ColorResource) { _skeleton() }
    public func mix(with a0: SwiftUI.Color, by a1: Swift.Double, `in` a2: SwiftUI.Gradient.ColorSpace) -> SwiftUI.Color { _skeleton() }
    public static var pink: SwiftUI.Color { system((255.0, 45.0, 85.0), (255.0, 55.0, 95.0)) }
    public static var mint: SwiftUI.Color { system((0, 200.0, 179.0), (0, 218.0, 195.0)) }
    public static var teal: SwiftUI.Color { system((0, 195.0, 208.0), (0, 210.0, 224.0)) }
    public static var cyan: SwiftUI.Color { system((0, 192.0, 232.0), (60.0, 211.0, 254.0)) }
    public static var indigo: SwiftUI.Color { system((97.0, 85.0, 245.0), (109.0, 124.0, 255.0)) }
    public static var brown: SwiftUI.Color { system((172.0, 127.0, 94.0), (183.0, 138.0, 102.0)) }
    public var gradient: SwiftUI.AnyGradient { _skeleton() }
    public static var red: SwiftUI.Color { system((255.0, 56.0, 60.0), (255.0, 66.0, 69.0)) }
    public static var orange: SwiftUI.Color { system((255.0, 141.0, 40.0), (255.0, 146.0, 48.0)) }
    public static var yellow: SwiftUI.Color { system((255.0, 204.0, 0), (255.0, 214.0, 0)) }
    public static var green: SwiftUI.Color { system((52.0, 199.0, 89.0), (48.0, 209.0, 88.0)) }
    public static var blue: SwiftUI.Color { system((0, 136.0, 255.0), (0, 145.0, 255.0)) }
    public static var white: SwiftUI.Color { Color(provider: SwiftUI.ResolvedColorBox(red: 1, green: 1, blue: 1, opacity: 1)) }
    public static var gray: SwiftUI.Color { system((142.0, 142.0, 147.0), (152.0, 152.0, 157.0)) }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public static var primary: SwiftUI.Color { platform(NSColor.labelColor) }
    public static var secondary: SwiftUI.Color { platform(NSColor.secondaryLabelColor) }
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
        init(linearRed a0: Swift.Float, linearGreen a1: Swift.Float, linearBlue a2: Swift.Float, opacity a3: Swift.Float) {
            linearRed = a0
            linearGreen = a1
            linearBlue = a2
            opacity = a3
        }

        /// a linear component in sRGB
        static func encoded(_ a0: Swift.Float) -> Swift.Float {
            a0 <= 0.0031308 ? a0 * 12.92 : 1.055 * powf(a0, 1 / 2.4) - 0.055
        }

        var nsColor: NSColor {
            NSColor(srgbRed: CoreGraphics.CGFloat(Self.encoded(linearRed)), green: CoreGraphics.CGFloat(Self.encoded(linearGreen)),
                    blue: CoreGraphics.CGFloat(Self.encoded(linearBlue)), alpha: CoreGraphics.CGFloat(opacity))
        }
        public var red: Swift.Float { Self.encoded(linearRed) }
        public var green: Swift.Float { Self.encoded(linearGreen) }
        public var blue: Swift.Float { Self.encoded(linearBlue) }
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
