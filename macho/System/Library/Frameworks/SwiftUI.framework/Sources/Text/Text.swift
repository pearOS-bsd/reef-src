// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Text (Text).

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

public struct Text {
    public var storage: SwiftUI.Text.Storage
    public var modifiers: [SwiftUI.Text.Modifier]
    public init(_ a0: SwiftUI.LocalizedStringKey, tableName a1: Swift.String?, bundle a2: Bundle?, comment a3: Swift.StaticString?) { _skeleton() }
    public init<A>(_ a0: A) where A: Swift.StringProtocol {
        storage = .verbatim(Swift.String(a0))
        modifiers = []
    }
    public init(verbatim a0: Swift.String) {
        storage = .verbatim(a0)
        modifiers = []
    }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public static func + (_ a0: SwiftUI.Text, _ a1: SwiftUI.Text) -> SwiftUI.Text {
        // two plain unmodified texts join into one string, others into a concatenated storage
        if case .verbatim(let s0) = a0.storage, case .verbatim(let s1) = a1.storage, a0.modifiers.isEmpty, a1.modifiers.isEmpty {
            return Text(verbatim: s0 + s1)
        }
        var text = Text(verbatim: "")
        text.storage = .anyTextStorage(SwiftUI.ConcatenatedTextStorage(a0, a1))
        return text
    }
    public func bold() -> SwiftUI.Text { _skeleton() }
    public func italic() -> SwiftUI.Text { _skeleton() }
    public func tracking(_ a0: CoreGraphics.CGFloat) -> SwiftUI.Text { _skeleton() }
    public func underline(_ a0: Swift.Bool, color a1: SwiftUI.Color?) -> SwiftUI.Text { _skeleton() }
    public func strikethrough(_ a0: Swift.Bool, color a1: SwiftUI.Color?) -> SwiftUI.Text { _skeleton() }
    public func accessibilityLabel<A>(_ a0: A) -> SwiftUI.Text where A: Swift.StringProtocol { _skeleton() }
    public func textScale(_ a0: SwiftUI.Text.Scale, isEnabled a1: Swift.Bool) -> SwiftUI.Text { _skeleton() }
    public func accessibilityTextContentType(_ a0: SwiftUI.AccessibilityTextContentType) -> SwiftUI.Text { _skeleton() }
    public func accessibilityLabel(_ a0: SwiftUI.Text) -> SwiftUI.Text { _skeleton() }
    public func accessibilityLabel(_ a0: SwiftUI.LocalizedStringKey) -> SwiftUI.Text { _skeleton() }
    public func speechSpellsOutCharacters(_ a0: Swift.Bool) -> SwiftUI.Text { _skeleton() }
    public func font(_ a0: SwiftUI.Font?) -> SwiftUI.Text { _skeleton() }
    public func monospaced(_ a0: Swift.Bool) -> SwiftUI.Text { _skeleton() }
    public static func == (_ a0: SwiftUI.Text, _ a1: SwiftUI.Text) -> Swift.Bool { _skeleton() }
    public func foregroundStyle<A>(_ a0: A) -> SwiftUI.Text where A: SwiftUI.ShapeStyle { _skeleton() }
    public func foregroundColor(_ a0: SwiftUI.Color?) -> SwiftUI.Text { _skeleton() }
    public init(_ a0: Foundation.AttributedString) { _skeleton() }
    public init(_ a0: Foundation.LocalizedStringResource) { _skeleton() }
    public init(_ a0: Foundation.Date, style a1: SwiftUI.Text.DateStyle) { _skeleton() }
    public init(_ a0: Swift.ClosedRange<Foundation.Date>) { _skeleton() }
    public init(_ a0: Foundation.DateInterval) { _skeleton() }
    public init<A>(_ a0: A.FormatInput, format a1: A) where A: Foundation.FormatStyle, A.FormatInput: Swift.Equatable, A.FormatOutput == Swift.String { _skeleton() }
    public init<A>(_ a0: A, formatter a1: Formatter) where A: Foundation.ReferenceConvertible { _skeleton() }
    public init<A>(_ a0: A, formatter a1: Formatter) where A: NSObject { _skeleton() }
    public func monospacedDigit() -> SwiftUI.Text { _skeleton() }
    public func kerning(_ a0: CoreGraphics.CGFloat) -> SwiftUI.Text { _skeleton() }
    public init(_ a0: SwiftUI.Image) { _skeleton() }
    public func italic(_ a0: Swift.Bool) -> SwiftUI.Text { _skeleton() }
    public func fontWeight(_ a0: SwiftUI.Font.Weight?) -> SwiftUI.Text { _skeleton() }
    public func fontDesign(_ a0: SwiftUI.Font.Design?) -> SwiftUI.Text { _skeleton() }
    public func fontWidth(_ a0: SwiftUI.Font.Width?) -> SwiftUI.Text { _skeleton() }
    public func baselineOffset(_ a0: CoreGraphics.CGFloat) -> SwiftUI.Text { _skeleton() }
    public func bold(_ a0: Swift.Bool) -> SwiftUI.Text { _skeleton() }
    static func markdownStorage(_ a0: Swift.String) -> SwiftUI.Text.Storage? { _skeleton() }
    public func accessibilityHeading(_ a0: SwiftUI.AccessibilityHeadingLevel) -> SwiftUI.Text { _skeleton() }
    public func speechAlwaysIncludesPunctuation(_ a0: Swift.Bool) -> SwiftUI.Text { _skeleton() }
    public struct LineStyle {
        public let pattern: SwiftUI.Text.LineStyle.Pattern
        public let color: SwiftUI.Color?
        public init(pattern a0: SwiftUI.Text.LineStyle.Pattern, color a1: SwiftUI.Color?) { _skeleton() }
        public static var single: SwiftUI.Text.LineStyle { _skeleton() }
        public static func == (_ a0: SwiftUI.Text.LineStyle, _ a1: SwiftUI.Text.LineStyle) -> Swift.Bool { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public struct Pattern {
            let rawValue: Swift.Int
            public static var solid: SwiftUI.Text.LineStyle.Pattern { _skeleton() }
            public static var dot: SwiftUI.Text.LineStyle.Pattern { _skeleton() }
            public static var dash: SwiftUI.Text.LineStyle.Pattern { _skeleton() }
            public var hashValue: Swift.Int { _skeleton() }
            public static func == (_ a0: SwiftUI.Text.LineStyle.Pattern, _ a1: SwiftUI.Text.LineStyle.Pattern) -> Swift.Bool { _skeleton() }
            public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        }
    }
    public enum TruncationMode {
        case head
        case tail
        case middle
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Text.TruncationMode, _ a1: SwiftUI.Text.TruncationMode) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
    public struct Scale {
        let factor: CoreGraphics.CGFloat
        public static var `default`: SwiftUI.Text.Scale { _skeleton() }
        public static var secondary: SwiftUI.Text.Scale { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public static func == (_ a0: SwiftUI.Text.Scale, _ a1: SwiftUI.Text.Scale) -> Swift.Bool { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
    }
    public enum Case {
        case uppercase
        case lowercase
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Text.Case, _ a1: SwiftUI.Text.Case) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
    @frozen public enum Storage {
        case verbatim(Swift.String)
        case anyTextStorage(SwiftUI.AnyTextStorage)
        public static func == (_ a0: SwiftUI.Text.Storage, _ a1: SwiftUI.Text.Storage) -> Swift.Bool { _skeleton() }
    }
    @frozen public enum Modifier {
        case color(SwiftUI.Color?)
        case font(SwiftUI.Font?)
        case anyTextModifier(SwiftUI.AnyTextModifier)
        public static func == (_ a0: SwiftUI.Text.Modifier, _ a1: SwiftUI.Text.Modifier) -> Swift.Bool { _skeleton() }
    }
    public struct DateStyle {
        let kind: SwiftUI.Text.DateStyle.Kind
        public static var time: SwiftUI.Text.DateStyle { _skeleton() }
        public static var timer: SwiftUI.Text.DateStyle { _skeleton() }
        public static var date: SwiftUI.Text.DateStyle { _skeleton() }
        public static var relative: SwiftUI.Text.DateStyle { _skeleton() }
        public static var offset: SwiftUI.Text.DateStyle { _skeleton() }
        public static func == (_ a0: SwiftUI.Text.DateStyle, _ a1: SwiftUI.Text.DateStyle) -> Swift.Bool { _skeleton() }
        enum Kind {
            case time
            case date
            case relative
            case offset
            case timer
        }
    }
}

extension SwiftUI.Text: SwiftUI.MenuContributor {
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
}

extension SwiftUI.Text: SwiftUI.PrimitiveView {
}

extension SwiftUI.Text: Swift.Equatable {
}

extension SwiftUI.Text: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Text.LineStyle.Pattern: Swift.Hashable {
}

extension SwiftUI.Text.LineStyle.Pattern: Swift.Equatable {
}

extension SwiftUI.Text.LineStyle: Swift.Hashable {
}

extension SwiftUI.Text.LineStyle: Swift.Equatable {
}

extension SwiftUI.Text.TruncationMode: Swift.Equatable {
}

extension SwiftUI.Text.TruncationMode: Swift.Hashable {
}

extension SwiftUI.Text.Scale: Swift.Hashable {
}

extension SwiftUI.Text.Scale: Swift.Equatable {
}

extension SwiftUI.Text.Case: Swift.Equatable {
}

extension SwiftUI.Text.Case: Swift.Hashable {
}

extension SwiftUI.Text.Storage: Swift.Equatable {
}

extension SwiftUI.Text.Modifier: Swift.Equatable {
}

extension SwiftUI.Text.DateStyle: Swift.Equatable {
}

extension SwiftUI.Text.DateStyle.Kind: Swift.Equatable {
}

extension SwiftUI.Text.DateStyle.Kind: Swift.Hashable {
}
