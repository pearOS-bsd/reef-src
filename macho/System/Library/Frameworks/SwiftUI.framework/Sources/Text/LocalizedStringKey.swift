// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LocalizedStringKey (Text).

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

public struct LocalizedStringKey {
    public var key: Swift.String
    public var hasFormatting: Swift.Bool
    public var arguments: [SwiftUI.LocalizedStringKey.FormatArgument]
    public init(stringLiteral a0: Swift.String) {
        key = a0
        hasFormatting = false
        arguments = []
    }
    func resolved(tableName a0: Swift.String?, bundle a1: Bundle?) -> Swift.String {
        // the bundle's (main bundle's) translation of the key, formatted with the arguments when it has any
        let string = (a1 ?? Bundle.main).localizedString(forKey: key, value: key, table: a0)
        if !hasFormatting { return string }
        return Swift.String(format: string, arguments: arguments.map { $0.value })
    }
    func resolvedText(tableName a0: Swift.String?, bundle a1: Bundle?) -> SwiftUI.Text { _skeleton() }
    public static func == (_ a0: SwiftUI.LocalizedStringKey, _ a1: SwiftUI.LocalizedStringKey) -> Swift.Bool { _skeleton() }
    public init(stringInterpolation a0: SwiftUI.LocalizedStringKey.StringInterpolation) {
        key = a0.key
        hasFormatting = true
        arguments = a0.arguments
    }
    public init(_ a0: Swift.String) { self.init(stringLiteral: a0) }
    public struct FormatArgument {
        let value: any Swift.CVarArg
        var text: SwiftUI.Text?
    }
    public struct StringInterpolation {
        var key: Swift.String
        var arguments: [SwiftUI.LocalizedStringKey.FormatArgument]
        public mutating func appendInterpolation<A>(_ a0: A, formatter a1: Formatter?) where A: NSObject { _skeleton() }
        public mutating func appendInterpolation(_ a0: Swift.String) {
            key.append("%@")
            arguments.append(SwiftUI.LocalizedStringKey.FormatArgument(value: a0 as NSString, text: nil))
        }
        public mutating func appendInterpolation<A>(_ a0: A.FormatInput, format a1: A) where A: Foundation.FormatStyle, A.FormatInput: Swift.Equatable, A.FormatOutput == Swift.String { _skeleton() }
        public mutating func appendInterpolation(_ a0: Foundation.AttributedString) { _skeleton() }
        public mutating func appendInterpolation(_ a0: Foundation.LocalizedStringResource) { _skeleton() }
        public init(literalCapacity a0: Swift.Int, interpolationCount a1: Swift.Int) {
            key = ""
            key.reserveCapacity(a0 + a1 * 2)
            arguments = []
        }
        public mutating func appendLiteral(_ a0: Swift.String) { key.append(a0.replacingOccurrences(of: "%", with: "%%")) }
        public mutating func appendInterpolation<A>(_ a0: A, specifier a1: Swift.String) where A: SwiftUI._FormatSpecifiable { _skeleton() }
        public mutating func appendInterpolation<A>(_ a0: A) where A: SwiftUI._FormatSpecifiable {
            key.append(a0._specifier)
            arguments.append(SwiftUI.LocalizedStringKey.FormatArgument(value: a0._arg, text: nil))
        }
        public mutating func appendInterpolation(_ a0: SwiftUI.Text) { _skeleton() }
    }
}

extension SwiftUI.LocalizedStringKey: Swift.Equatable {
}

extension SwiftUI.LocalizedStringKey: Swift.ExpressibleByStringLiteral {
    public typealias StringLiteralType = Swift.String
}

extension SwiftUI.LocalizedStringKey: Swift.ExpressibleByExtendedGraphemeClusterLiteral {
    public typealias ExtendedGraphemeClusterLiteralType = Swift.String
}

extension SwiftUI.LocalizedStringKey: Swift.ExpressibleByUnicodeScalarLiteral {
    public typealias UnicodeScalarLiteralType = Swift.String
}

extension SwiftUI.LocalizedStringKey: Swift.ExpressibleByStringInterpolation {
}

extension SwiftUI.LocalizedStringKey.StringInterpolation: Swift.StringInterpolationProtocol {
    public typealias StringLiteralType = Swift.String
}
