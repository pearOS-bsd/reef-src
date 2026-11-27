// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextAlignment (Layout).

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

@frozen public enum TextAlignment {
    case leading
    case center
    case trailing
    public static var allCases: [SwiftUI.TextAlignment] { [.leading, .center, .trailing] }
    public static func == (_ a0: SwiftUI.TextAlignment, _ a1: SwiftUI.TextAlignment) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
}

extension SwiftUI.TextAlignment: Swift.Equatable {
}

extension SwiftUI.TextAlignment: Swift.Hashable {
}

extension SwiftUI.TextAlignment: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.TextAlignment]
}
