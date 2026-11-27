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
    public init(_ a0: CGColor) {
        // an AppKit colour of the CGColor (clear when AppKit has none)
        self.init(provider: SwiftUI.PlatformColorBox(NSColor(cgColor: a0) ?? NSColor.clear))
    }
    public func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { provider.resolve(in: a0) }
    public init(white a0: Swift.Double, opacity a1: Swift.Double) {
        self.init(provider: SwiftUI.ResolvedColorBox(red: a0, green: a0, blue: a0, opacity: a1))
    }
    public init(nsColor a0: NSColor) { self.init(provider: SwiftUI.PlatformColorBox(a0)) }
    public static var purple: SwiftUI.Color { Stored.purple }
    public init(_ a0: Swift.String, bundle a1: Bundle?) { self.init(provider: SwiftUI.NamedColorBox(name: a0, bundle: a1)) }
    public init(_ a0: SwiftUI.Color.Resolved) {
        // the components encoded back to sRGB
        self.init(provider: SwiftUI.ResolvedColorBox(red: Double(a0.red), green: Double(a0.green), blue: Double(a0.blue), opacity: Double(a0.opacity)))
    }
    public init(red a0: Swift.Double, green a1: Swift.Double, blue a2: Swift.Double, opacity a3: Swift.Double) {
        self.init(provider: SwiftUI.ResolvedColorBox(red: a0, green: a1, blue: a2, opacity: a3))
    }
    public init<A>(_ a0: A) where A: Swift.Hashable, A: SwiftUI.ShapeStyle, A.Resolved == SwiftUI.Color.Resolved {
        // resolved once in an empty environment
        self.init(a0.resolve(in: SwiftUI.EnvironmentValues()))
    }
    public static func == (_ a0: SwiftUI.Color, _ a1: SwiftUI.Color) -> Swift.Bool { a0.provider.isEqual(to: a1.provider) }
    public static var black: SwiftUI.Color { Stored.black }
    public func opacity(_ a0: Swift.Double) -> SwiftUI.Color { Color(provider: SwiftUI.OpacityColorBox(base: provider, opacity: a0)) }
    public static var clear: SwiftUI.Color { Stored.clear }
    public init(_ a0: SwiftUI.Color.RGBColorSpace, white a1: Swift.Double, opacity a2: Swift.Double) {
        // the colour space is not used: the components are taken as sRGB
        self.init(provider: SwiftUI.ResolvedColorBox(red: a1, green: a1, blue: a1, opacity: a2))
    }
    public init(_ a0: SwiftUI.Color.RGBColorSpace, red a1: Swift.Double, green a2: Swift.Double, blue a3: Swift.Double, opacity a4: Swift.Double) {
        // the colour space is not used: the components are taken as sRGB
        self.init(provider: SwiftUI.ResolvedColorBox(red: a1, green: a2, blue: a3, opacity: a4))
    }
    public init(hue a0: Swift.Double, saturation a1: Swift.Double, brightness a2: Swift.Double, opacity a3: Swift.Double) {
        self.init(provider: SwiftUI.PlatformColorBox(NSColor(calibratedHue: a0, saturation: a1, brightness: a2, alpha: a3)))
    }
    public static var transferRepresentation: CoreTransferable.DataRepresentation<SwiftUI.Color> { _skeleton() }
    public init(_ a0: DeveloperToolsSupport.ColorResource) { self.init(provider: SwiftUI.PlatformColorBox(NSColor(resource: a0))) }
    public func mix(with a0: SwiftUI.Color, by a1: Swift.Double, `in` a2: SwiftUI.Gradient.ColorSpace) -> SwiftUI.Color {
        Color(provider: SwiftUI.ColorMixBox(base: provider, other: a0.provider, fraction: a1, colorSpace: a2))
    }
    public static var pink: SwiftUI.Color { Stored.pink }
    public static var mint: SwiftUI.Color { Stored.mint }
    public static var teal: SwiftUI.Color { Stored.teal }
    public static var cyan: SwiftUI.Color { Stored.cyan }
    public static var indigo: SwiftUI.Color { Stored.indigo }
    public static var brown: SwiftUI.Color { Stored.brown }
    public var gradient: SwiftUI.AnyGradient { SwiftUI.AnyGradient(provider: SwiftUI.AnyGradientBox(base: self)) }
    public static var red: SwiftUI.Color { Stored.red }
    public static var orange: SwiftUI.Color { Stored.orange }
    public static var yellow: SwiftUI.Color { Stored.yellow }
    public static var green: SwiftUI.Color { Stored.green }
    public static var blue: SwiftUI.Color { Stored.blue }
    public static var white: SwiftUI.Color { Stored.white }
    public static var gray: SwiftUI.Color { Stored.gray }
    public func hash(into a0: inout Swift.Hasher) { provider.hash(into: &a0) }
    public static var primary: SwiftUI.Color { Stored.primary }
    public static var secondary: SwiftUI.Color { Stored.secondary }
    public static var accentColor: SwiftUI.Color { Stored.accentColor }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public init(_ a0: NSColor) { self.init(nsColor: a0) }
    /// the named colours, each made once on first use (one box shared by every use)
    enum Stored {
        static let red = system((255.0, 56.0, 60.0), (255.0, 66.0, 69.0))
        static let orange = system((255.0, 141.0, 40.0), (255.0, 146.0, 48.0))
        static let yellow = system((255.0, 204.0, 0), (255.0, 214.0, 0))
        static let green = system((52.0, 199.0, 89.0), (48.0, 209.0, 88.0))
        static let mint = system((0, 200.0, 179.0), (0, 218.0, 195.0))
        static let teal = system((0, 195.0, 208.0), (0, 210.0, 224.0))
        static let cyan = system((0, 192.0, 232.0), (60.0, 211.0, 254.0))
        static let blue = system((0, 136.0, 255.0), (0, 145.0, 255.0))
        static let indigo = system((97.0, 85.0, 245.0), (109.0, 124.0, 255.0))
        static let purple = system((203.0, 48.0, 224.0), (219.0, 52.0, 242.0))
        static let pink = system((255.0, 45.0, 85.0), (255.0, 55.0, 95.0))
        static let brown = system((172.0, 127.0, 94.0), (183.0, 138.0, 102.0))
        static let gray = system((142.0, 142.0, 147.0), (152.0, 152.0, 157.0))
        static let black = Color(provider: SwiftUI.ResolvedColorBox(red: 0, green: 0, blue: 0, opacity: 1))
        static let white = Color(provider: SwiftUI.ResolvedColorBox(red: 1, green: 1, blue: 1, opacity: 1))
        static let clear = Color(provider: SwiftUI.ResolvedColorBox(red: 0, green: 0, blue: 0, opacity: 0))
        static let primary = platform(NSColor.labelColor)
        static let secondary = platform(NSColor.secondaryLabelColor)
        static let accentColor = platform(NSColor.controlAccentColor)
    }
    public struct Resolved {
        public var linearRed: Swift.Float
        public var linearGreen: Swift.Float
        public var linearBlue: Swift.Float
        public var opacity: Swift.Float
        public var cgColor: CGColor { nsColor.cgColor }
        public init(colorSpace a0: SwiftUI.Color.RGBColorSpace, red a1: Swift.Float, green a2: Swift.Float, blue a3: Swift.Float, opacity a4: Swift.Float) {
            // the colour space is not used: the components are taken as sRGB and made linear
            self.init(linearRed: Self.linear(a1), linearGreen: Self.linear(a2), linearBlue: Self.linear(a3), opacity: a4)
        }

        /// an sRGB component made linear
        static func linear(_ a0: Swift.Float) -> Swift.Float {
            a0 <= 0.04045 ? a0 / 12.92 : powf((a0 + 0.055) / 1.055, 2.4)
        }
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
        public static func == (_ a0: SwiftUI.Color.Resolved, _ a1: SwiftUI.Color.Resolved) -> Swift.Bool {
            a0.linearRed == a1.linearRed && a0.linearGreen == a1.linearGreen && a0.linearBlue == a1.linearBlue && a0.opacity == a1.opacity
        }
        public func hash(into a0: inout Swift.Hasher) {
            a0.combine(linearRed)
            a0.combine(linearGreen)
            a0.combine(linearBlue)
            a0.combine(opacity)
        }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { self }
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
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        // a rectangle filled with the colour resolved in the context's environment
        [SwiftUI.ShapeBuild.element(SwiftUI.Rectangle(), paint: .color(resolve(in: a0.environment)), stroke: nil, eoFill: false)]
    }
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
