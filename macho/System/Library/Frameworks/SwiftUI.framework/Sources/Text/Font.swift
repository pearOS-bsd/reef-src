// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Font (Text).

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

public struct Font {
    public var provider: SwiftUI.AnyFontBox
    public static func system(size a0: CoreGraphics.CGFloat, weight a1: SwiftUI.Font.Weight?, design a2: SwiftUI.Font.Design?) -> SwiftUI.Font {
        Font(provider: SwiftUI.FontBox(SystemProvider(size: a0, weight: a1, design: a2)))
    }
    public static func system(_ a0: SwiftUI.Font.TextStyle, design a1: SwiftUI.Font.Design?, weight a2: SwiftUI.Font.Weight?) -> SwiftUI.Font {
        Font(provider: SwiftUI.FontBox(TextStyleProvider(textStyle: a0, design: a1, weight: a2)))
    }
    public static var largeTitle: SwiftUI.Font { textStyle(.largeTitle) }
    public static var title: SwiftUI.Font { textStyle(.title) }
    public static var title2: SwiftUI.Font { textStyle(.title2) }
    public static var title3: SwiftUI.Font { textStyle(.title3) }
    public static var subheadline: SwiftUI.Font { textStyle(.subheadline) }
    public static var callout: SwiftUI.Font { textStyle(.callout) }
    public static var footnote: SwiftUI.Font { textStyle(.footnote) }
    public static func custom(_ a0: Swift.String, size a1: CoreGraphics.CGFloat, relativeTo a2: SwiftUI.Font.TextStyle) -> SwiftUI.Font {
        named(a0, a1, a2)
    }
    public func bold() -> SwiftUI.Font { Font(provider: SwiftUI.FontBox(StaticModifierProvider<BoldModifier>(base: self))) }
    public func italic() -> SwiftUI.Font { Font(provider: SwiftUI.FontBox(StaticModifierProvider<ItalicModifier>(base: self))) }
    public func monospacedDigit() -> SwiftUI.Font { Font(provider: SwiftUI.FontBox(StaticModifierProvider<MonospacedDigitModifier>(base: self))) }
    public func weight(_ a0: SwiftUI.Font.Weight) -> SwiftUI.Font {
        Font(provider: SwiftUI.FontBox(ModifierProvider(base: self, modifier: WeightModifier(weight: a0))))
    }
    public func leading(_ a0: SwiftUI.Font.Leading) -> SwiftUI.Font {
        Font(provider: SwiftUI.FontBox(ModifierProvider(base: self, modifier: LeadingModifier(leading: a0))))
    }
    public func monospaced() -> SwiftUI.Font { Font(provider: SwiftUI.FontBox(StaticModifierProvider<MonospacedModifier>(base: self))) }
    public static func == (_ a0: SwiftUI.Font, _ a1: SwiftUI.Font) -> Swift.Bool { a0.provider.isEqual(to: a1.provider) }
    public func hash(into a0: inout Swift.Hasher) { provider.hash(into: &a0) }
    public static func system(size a0: CoreGraphics.CGFloat, weight a1: SwiftUI.Font.Weight, design a2: SwiftUI.Font.Design) -> SwiftUI.Font {
        system(size: a0, weight: Optional(a1), design: Optional(a2))
    }
    public static func system(_ a0: SwiftUI.Font.TextStyle, design a1: SwiftUI.Font.Design) -> SwiftUI.Font {
        system(a0, design: Optional(a1), weight: nil)
    }
    public static var body: SwiftUI.Font { textStyle(.body) }
    public static var headline: SwiftUI.Font { textStyle(.headline) }
    public static var caption: SwiftUI.Font { textStyle(.caption) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static var caption2: SwiftUI.Font { textStyle(.caption2) }
    public static func custom(_ a0: Swift.String, fixedSize a1: CoreGraphics.CGFloat) -> SwiftUI.Font { named(a0, a1, nil) }
    public static func custom(_ a0: Swift.String, size a1: CoreGraphics.CGFloat) -> SwiftUI.Font { named(a0, a1, .body) }
    public init(_ a0: CTFont) { self.init(provider: SwiftUI.FontBox(PlatformFontProvider(font: a0))) }

    init(provider a0: SwiftUI.AnyFontBox) { provider = a0 }

    /// the font of a text style
    static func textStyle(_ a0: SwiftUI.Font.TextStyle) -> SwiftUI.Font {
        Font(provider: SwiftUI.FontBox(TextStyleProvider(textStyle: a0, design: nil, weight: nil)))
    }

    /// a named font (a size of 0 or less is 12)
    static func named(_ a0: Swift.String, _ a1: CoreGraphics.CGFloat, _ a2: SwiftUI.Font.TextStyle?) -> SwiftUI.Font {
        Font(provider: SwiftUI.FontBox(NamedProvider(name: a0, size: a1 > 0 ? a1 : 12, textStyle: a2)))
    }
    public struct Width {
        public var value: CoreGraphics.CGFloat
        public static var compressed: SwiftUI.Font.Width { .init(-0.3) }
        public static var condensed: SwiftUI.Font.Width { .init(-0.2) }
        public static var standard: SwiftUI.Font.Width { .init(0) }
        public static var expanded: SwiftUI.Font.Width { .init(0.2) }
        public static func == (_ a0: SwiftUI.Font.Width, _ a1: SwiftUI.Font.Width) -> Swift.Bool { a0.value == a1.value }
        public func hash(into a0: inout Swift.Hasher) { a0.combine(value) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public init(_ a0: CoreGraphics.CGFloat) { self.value = a0 }
    }
    public enum Leading {
        case standard
        case tight
        case loose
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Font.Leading, _ a1: SwiftUI.Font.Leading) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
    public struct Weight {
        public var value: CoreGraphics.CGFloat
        public init(value a0: CoreGraphics.CGFloat) { self.value = a0 }
        public static var ultraLight: SwiftUI.Font.Weight { .init(value: -0.8) }
        public static var thin: SwiftUI.Font.Weight { .init(value: -0.6) }
        public static var light: SwiftUI.Font.Weight { .init(value: -0.4) }
        public static var regular: SwiftUI.Font.Weight { .init(value: 0) }
        public static var medium: SwiftUI.Font.Weight { .init(value: 0.23) }
        public static var semibold: SwiftUI.Font.Weight { .init(value: 0.3) }
        public static var bold: SwiftUI.Font.Weight { .init(value: 0.4) }
        public static var heavy: SwiftUI.Font.Weight { .init(value: 0.56) }
        public static var black: SwiftUI.Font.Weight { .init(value: 0.62) }
        public func hash(into a0: inout Swift.Hasher) { a0.combine(value) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Font.Weight, _ a1: SwiftUI.Font.Weight) -> Swift.Bool { a0.value == a1.value }
    }
    public enum Design {
        case `default`
        case serif
        case rounded
        case monospaced
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Font.Design, _ a1: SwiftUI.Font.Design) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
    public enum TextStyle {
        case largeTitle
        case title
        case title2
        case title3
        case headline
        case subheadline
        case body
        case callout
        case footnote
        case caption
        case caption2
        public static var allCases: [SwiftUI.Font.TextStyle] { [.largeTitle, .title, .title2, .title3, .headline, .subheadline, .body, .callout, .footnote, .caption, .caption2] }
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Font.TextStyle, _ a1: SwiftUI.Font.TextStyle) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
    struct PlatformFontProvider {
        let font: CTFont
    }
    enum MonospacedModifier {
    }
    struct LeadingModifier {
        let leading: SwiftUI.Font.Leading
    }
    struct WeightModifier {
        let weight: SwiftUI.Font.Weight
    }
    struct ModifierProvider<A> where A: SwiftUI.FontModifier {
        let base: SwiftUI.Font
        let modifier: A
        func apply(to a0: inout SwiftUI.FontTraits) {
            base.provider.apply(to: &a0)
            modifier.modify(&a0)
        }
        static func == (_ a0: SwiftUI.Font.ModifierProvider<A>, _ a1: SwiftUI.Font.ModifierProvider<A>) -> Swift.Bool {
            a0.base == a1.base && a0.modifier == a1.modifier
        }
        func hash(into a0: inout Swift.Hasher) {
            a0.combine(base)
            a0.combine(modifier)
        }
        var hashValue: Swift.Int { _hashValue(for: self) }
    }
    enum MonospacedDigitModifier {
    }
    enum ItalicModifier {
    }
    enum BoldModifier {
    }
    struct StaticModifierProvider<A> where A: SwiftUI.StaticFontModifier {
        let base: SwiftUI.Font
        static func == (_ a0: SwiftUI.Font.StaticModifierProvider<A>, _ a1: SwiftUI.Font.StaticModifierProvider<A>) -> Swift.Bool { a0.base == a1.base }
        func hash(into a0: inout Swift.Hasher) { a0.combine(base) }
        func apply(to a0: inout SwiftUI.FontTraits) {
            base.provider.apply(to: &a0)
            A.modify(&a0)
        }
        var hashValue: Swift.Int { _hashValue(for: self) }
    }
    struct NamedProvider {
        let name: Swift.String
        let size: CoreGraphics.CGFloat
        let textStyle: SwiftUI.Font.TextStyle?
        static func == (_ a0: SwiftUI.Font.NamedProvider, _ a1: SwiftUI.Font.NamedProvider) -> Swift.Bool {
            a0.name == a1.name && a0.size == a1.size && a0.textStyle == a1.textStyle
        }
    }
    struct TextStyleProvider {
        let textStyle: SwiftUI.Font.TextStyle
        let design: SwiftUI.Font.Design?
        let weight: SwiftUI.Font.Weight?
        func apply(to a0: inout SwiftUI.FontTraits) {
            // the size of the text style (13 when unknown); headline is bold unless a weight is given
            a0.size = Self.sizes[textStyle] ?? 13
            if let weight { a0.weight = weight } else if textStyle == .headline { a0.weight = .bold }
            if let design { a0.design = design }
            a0.name = nil
        }
        func hash(into a0: inout Swift.Hasher) {
            a0.combine(textStyle)
            a0.combine(design)
            a0.combine(weight)
        }
        var hashValue: Swift.Int { _hashValue(for: self) }
        static func == (_ a0: SwiftUI.Font.TextStyleProvider, _ a1: SwiftUI.Font.TextStyleProvider) -> Swift.Bool {
            a0.textStyle == a1.textStyle && a0.design == a1.design && a0.weight == a1.weight
        }
        static var sizes: [SwiftUI.Font.TextStyle: CoreGraphics.CGFloat] =
            [.largeTitle: 26, .title: 22, .title2: 17, .title3: 15, .headline: 13, .subheadline: 11, .body: 13, .callout: 12,
             .footnote: 10, .caption: 10, .caption2: 10]
    }
    struct SystemProvider {
        let size: CoreGraphics.CGFloat
        let weight: SwiftUI.Font.Weight?
        let design: SwiftUI.Font.Design?
        func hash(into a0: inout Swift.Hasher) {
            a0.combine(size)
            a0.combine(weight)
            a0.combine(design)
        }
        var hashValue: Swift.Int { _hashValue(for: self) }
        static func == (_ a0: SwiftUI.Font.SystemProvider, _ a1: SwiftUI.Font.SystemProvider) -> Swift.Bool {
            a0.size == a1.size && a0.weight == a1.weight && a0.design == a1.design
        }
    }
}

extension SwiftUI.Font.Width: Swift.Hashable {
}

extension SwiftUI.Font.Width: Swift.Equatable {
}

extension SwiftUI.Font.Leading: Swift.Equatable {
}

extension SwiftUI.Font.Leading: Swift.Hashable {
}

extension SwiftUI.Font.Weight: Swift.Hashable {
}

extension SwiftUI.Font.Weight: Swift.Equatable {
}

extension SwiftUI.Font.Design: Swift.Equatable {
}

extension SwiftUI.Font.Design: Swift.Hashable {
}

extension SwiftUI.Font.TextStyle: Swift.Equatable {
}

extension SwiftUI.Font.TextStyle: Swift.Hashable {
}

extension SwiftUI.Font.TextStyle: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.Font.TextStyle]
}

extension SwiftUI.Font: Swift.Hashable {
}

extension SwiftUI.Font: Swift.Equatable {
}

extension SwiftUI.Font.SystemProvider: Swift.Equatable {
}

extension SwiftUI.Font.SystemProvider: Swift.Hashable {
}

extension SwiftUI.Font.SystemProvider: SwiftUI.FontProvider {
    func apply(to a0: inout SwiftUI.FontTraits) {
        a0.size = size
        if let weight { a0.weight = weight }
        if let design { a0.design = design }
        a0.name = nil
    }
}

extension SwiftUI.Font.TextStyleProvider: Swift.Equatable {
}

extension SwiftUI.Font.TextStyleProvider: Swift.Hashable {
}

extension SwiftUI.Font.TextStyleProvider: SwiftUI.FontProvider {
}

extension SwiftUI.Font.NamedProvider: Swift.Equatable {
}

extension SwiftUI.Font.NamedProvider: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) {
        a0.combine(name)
        a0.combine(size)
        a0.combine(textStyle)
    }
}

extension SwiftUI.Font.NamedProvider: SwiftUI.FontProvider {
    func apply(to a0: inout SwiftUI.FontTraits) {
        a0.size = size
        a0.name = name
    }
}

extension SwiftUI.Font.StaticModifierProvider: Swift.Equatable {
}

extension SwiftUI.Font.StaticModifierProvider: Swift.Hashable {
}

extension SwiftUI.Font.StaticModifierProvider: SwiftUI.FontProvider {
}

extension SwiftUI.Font.BoldModifier: SwiftUI.StaticFontModifier {
    static func modify(_ a0: inout SwiftUI.FontTraits) { a0.weight = .bold }
}

extension SwiftUI.Font.ItalicModifier: SwiftUI.StaticFontModifier {
    static func modify(_ a0: inout SwiftUI.FontTraits) { a0.italic = true }
}

extension SwiftUI.Font.MonospacedDigitModifier: SwiftUI.StaticFontModifier {
    static func modify(_ a0: inout SwiftUI.FontTraits) { a0.monospacedDigit = true }
}

extension SwiftUI.Font.ModifierProvider: Swift.Equatable {
}

extension SwiftUI.Font.ModifierProvider: Swift.Hashable {
}

extension SwiftUI.Font.ModifierProvider: SwiftUI.FontProvider {
}

extension SwiftUI.Font.WeightModifier: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { a0.weight == a1.weight }
}

extension SwiftUI.Font.WeightModifier: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { a0.combine(weight) }
}

extension SwiftUI.Font.WeightModifier: SwiftUI.FontModifier {
    func modify(_ a0: inout SwiftUI.FontTraits) { a0.weight = weight }
}

extension SwiftUI.Font.LeadingModifier: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { a0.leading == a1.leading }
}

extension SwiftUI.Font.LeadingModifier: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { a0.combine(leading) }
}

extension SwiftUI.Font.LeadingModifier: SwiftUI.FontModifier {
    func modify(_ a0: inout SwiftUI.FontTraits) { a0.leading = leading }
}

extension SwiftUI.Font.MonospacedModifier: SwiftUI.StaticFontModifier {
    static func modify(_ a0: inout SwiftUI.FontTraits) { a0.monospaced = true }
}

extension SwiftUI.Font.PlatformFontProvider: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Font.PlatformFontProvider: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.Font.PlatformFontProvider: SwiftUI.FontProvider {
    func apply(to a0: inout SwiftUI.FontTraits) { _skeleton() }
}
