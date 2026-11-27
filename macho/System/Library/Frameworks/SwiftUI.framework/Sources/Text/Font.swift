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
    public static func system(size a0: CoreGraphics.CGFloat, weight a1: SwiftUI.Font.Weight?, design a2: SwiftUI.Font.Design?) -> SwiftUI.Font { _skeleton() }
    public static func system(_ a0: SwiftUI.Font.TextStyle, design a1: SwiftUI.Font.Design?, weight a2: SwiftUI.Font.Weight?) -> SwiftUI.Font { _skeleton() }
    public static var largeTitle: SwiftUI.Font { _skeleton() }
    public static var title: SwiftUI.Font { _skeleton() }
    public static var title2: SwiftUI.Font { _skeleton() }
    public static var title3: SwiftUI.Font { _skeleton() }
    public static var subheadline: SwiftUI.Font { _skeleton() }
    public static var callout: SwiftUI.Font { _skeleton() }
    public static var footnote: SwiftUI.Font { _skeleton() }
    public static func custom(_ a0: Swift.String, size a1: CoreGraphics.CGFloat, relativeTo a2: SwiftUI.Font.TextStyle) -> SwiftUI.Font { _skeleton() }
    public func bold() -> SwiftUI.Font { _skeleton() }
    public func italic() -> SwiftUI.Font { _skeleton() }
    public func monospacedDigit() -> SwiftUI.Font { _skeleton() }
    public func weight(_ a0: SwiftUI.Font.Weight) -> SwiftUI.Font { _skeleton() }
    public func leading(_ a0: SwiftUI.Font.Leading) -> SwiftUI.Font { _skeleton() }
    public func monospaced() -> SwiftUI.Font { _skeleton() }
    public static func == (_ a0: SwiftUI.Font, _ a1: SwiftUI.Font) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public static func system(size a0: CoreGraphics.CGFloat, weight a1: SwiftUI.Font.Weight, design a2: SwiftUI.Font.Design) -> SwiftUI.Font { _skeleton() }
    public static func system(_ a0: SwiftUI.Font.TextStyle, design a1: SwiftUI.Font.Design) -> SwiftUI.Font { _skeleton() }
    public static var body: SwiftUI.Font { _skeleton() }
    public static var headline: SwiftUI.Font { _skeleton() }
    public static var caption: SwiftUI.Font { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static var caption2: SwiftUI.Font { _skeleton() }
    public static func custom(_ a0: Swift.String, fixedSize a1: CoreGraphics.CGFloat) -> SwiftUI.Font { _skeleton() }
    public static func custom(_ a0: Swift.String, size a1: CoreGraphics.CGFloat) -> SwiftUI.Font { _skeleton() }
    public init(_ a0: CTFont) { _skeleton() }
    public struct Width {
        public var value: CoreGraphics.CGFloat
        public static var compressed: SwiftUI.Font.Width { _skeleton() }
        public static var condensed: SwiftUI.Font.Width { _skeleton() }
        public static var standard: SwiftUI.Font.Width { _skeleton() }
        public static var expanded: SwiftUI.Font.Width { _skeleton() }
        public static func == (_ a0: SwiftUI.Font.Width, _ a1: SwiftUI.Font.Width) -> Swift.Bool { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public init(_ a0: CoreGraphics.CGFloat) { _skeleton() }
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
        public init(value a0: CoreGraphics.CGFloat) { _skeleton() }
        public static var ultraLight: SwiftUI.Font.Weight { _skeleton() }
        public static var thin: SwiftUI.Font.Weight { _skeleton() }
        public static var light: SwiftUI.Font.Weight { _skeleton() }
        public static var regular: SwiftUI.Font.Weight { _skeleton() }
        public static var medium: SwiftUI.Font.Weight { _skeleton() }
        public static var semibold: SwiftUI.Font.Weight { _skeleton() }
        public static var bold: SwiftUI.Font.Weight { _skeleton() }
        public static var heavy: SwiftUI.Font.Weight { _skeleton() }
        public static var black: SwiftUI.Font.Weight { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public static func == (_ a0: SwiftUI.Font.Weight, _ a1: SwiftUI.Font.Weight) -> Swift.Bool { _skeleton() }
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
        public static var allCases: [SwiftUI.Font.TextStyle] { _skeleton() }
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
        func apply(to a0: inout SwiftUI.FontTraits) { _skeleton() }
        static func == (_ a0: SwiftUI.Font.ModifierProvider<A>, _ a1: SwiftUI.Font.ModifierProvider<A>) -> Swift.Bool { _skeleton() }
        func hash(into a0: inout Swift.Hasher) { _skeleton() }
        var hashValue: Swift.Int { _skeleton() }
    }
    enum MonospacedDigitModifier {
    }
    enum ItalicModifier {
    }
    enum BoldModifier {
    }
    struct StaticModifierProvider<A> where A: SwiftUI.StaticFontModifier {
        let base: SwiftUI.Font
        static func == (_ a0: SwiftUI.Font.StaticModifierProvider<A>, _ a1: SwiftUI.Font.StaticModifierProvider<A>) -> Swift.Bool { _skeleton() }
        func hash(into a0: inout Swift.Hasher) { _skeleton() }
        func apply(to a0: inout SwiftUI.FontTraits) { _skeleton() }
        var hashValue: Swift.Int { _skeleton() }
    }
    struct NamedProvider {
        let name: Swift.String
        let size: CoreGraphics.CGFloat
        let textStyle: SwiftUI.Font.TextStyle?
    }
    struct TextStyleProvider {
        let textStyle: SwiftUI.Font.TextStyle
        let design: SwiftUI.Font.Design?
        let weight: SwiftUI.Font.Weight?
        func apply(to a0: inout SwiftUI.FontTraits) { _skeleton() }
        func hash(into a0: inout Swift.Hasher) { _skeleton() }
        var hashValue: Swift.Int { _skeleton() }
        static var sizes: [SwiftUI.Font.TextStyle: CoreGraphics.CGFloat] { _skeleton() }
    }
    struct SystemProvider {
        let size: CoreGraphics.CGFloat
        let weight: SwiftUI.Font.Weight?
        let design: SwiftUI.Font.Design?
        func hash(into a0: inout Swift.Hasher) { _skeleton() }
        var hashValue: Swift.Int { _skeleton() }
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
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Font.SystemProvider: Swift.Hashable {
}

extension SwiftUI.Font.SystemProvider: SwiftUI.FontProvider {
    func apply(to a0: inout SwiftUI.FontTraits) { _skeleton() }
}

extension SwiftUI.Font.TextStyleProvider: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Font.TextStyleProvider: Swift.Hashable {
}

extension SwiftUI.Font.TextStyleProvider: SwiftUI.FontProvider {
}

extension SwiftUI.Font.NamedProvider: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Font.NamedProvider: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.Font.NamedProvider: SwiftUI.FontProvider {
    func apply(to a0: inout SwiftUI.FontTraits) { _skeleton() }
}

extension SwiftUI.Font.StaticModifierProvider: Swift.Equatable {
}

extension SwiftUI.Font.StaticModifierProvider: Swift.Hashable {
}

extension SwiftUI.Font.StaticModifierProvider: SwiftUI.FontProvider {
}

extension SwiftUI.Font.BoldModifier: SwiftUI.StaticFontModifier {
    static func modify(_ a0: inout SwiftUI.FontTraits) { _skeleton() }
}

extension SwiftUI.Font.ItalicModifier: SwiftUI.StaticFontModifier {
    static func modify(_ a0: inout SwiftUI.FontTraits) { _skeleton() }
}

extension SwiftUI.Font.MonospacedDigitModifier: SwiftUI.StaticFontModifier {
    static func modify(_ a0: inout SwiftUI.FontTraits) { _skeleton() }
}

extension SwiftUI.Font.ModifierProvider: Swift.Equatable {
}

extension SwiftUI.Font.ModifierProvider: Swift.Hashable {
}

extension SwiftUI.Font.ModifierProvider: SwiftUI.FontProvider {
}

extension SwiftUI.Font.WeightModifier: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Font.WeightModifier: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.Font.WeightModifier: SwiftUI.FontModifier {
    func modify(_ a0: inout SwiftUI.FontTraits) { _skeleton() }
}

extension SwiftUI.Font.LeadingModifier: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Font.LeadingModifier: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.Font.LeadingModifier: SwiftUI.FontModifier {
    func modify(_ a0: inout SwiftUI.FontTraits) { _skeleton() }
}

extension SwiftUI.Font.MonospacedModifier: SwiftUI.StaticFontModifier {
    static func modify(_ a0: inout SwiftUI.FontTraits) { _skeleton() }
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
