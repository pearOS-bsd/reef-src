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
    public init(stringLiteral a0: Swift.String) { _skeleton() }
    func resolved(tableName a0: Swift.String?, bundle a1: Bundle?) -> Swift.String { _skeleton() }
    func resolvedText(tableName a0: Swift.String?, bundle a1: Bundle?) -> SwiftUI.Text { _skeleton() }
    public static func == (_ a0: SwiftUI.LocalizedStringKey, _ a1: SwiftUI.LocalizedStringKey) -> Swift.Bool { _skeleton() }
    public init(stringInterpolation a0: SwiftUI.LocalizedStringKey.StringInterpolation) { _skeleton() }
    public init(_ a0: Swift.String) { _skeleton() }
    public struct FormatArgument {
        let value: any Swift.CVarArg
        var text: SwiftUI.Text?
    }
    public struct StringInterpolation {
        var key: Swift.String
        var arguments: [SwiftUI.LocalizedStringKey.FormatArgument]
        public func appendInterpolation<A>(_ a0: A, formatter a1: Formatter?) where A: NSObject { _skeleton() }
        public func appendInterpolation(_ a0: Swift.String) { _skeleton() }
        public func appendInterpolation<A>(_ a0: A.FormatInput, format a1: A) where A: Foundation.FormatStyle, A.FormatInput: Swift.Equatable, A.FormatOutput == Swift.String { _skeleton() }
        public func appendInterpolation(_ a0: Foundation.AttributedString) { _skeleton() }
        public func appendInterpolation(_ a0: Foundation.LocalizedStringResource) { _skeleton() }
        public init(literalCapacity a0: Swift.Int, interpolationCount a1: Swift.Int) { _skeleton() }
        public func appendLiteral(_ a0: Swift.String) { _skeleton() }
        public func appendInterpolation<A>(_ a0: A, specifier a1: Swift.String) where A: SwiftUI._FormatSpecifiable { _skeleton() }
        public func appendInterpolation<A>(_ a0: A) where A: SwiftUI._FormatSpecifiable { _skeleton() }
        public func appendInterpolation(_ a0: SwiftUI.Text) { _skeleton() }
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
