// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ColorSchemeContrast (Environment).

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

public enum ColorSchemeContrast {
    case standard
    case increased
    public static var allCases: [SwiftUI.ColorSchemeContrast] { [.standard, .increased] }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.ColorSchemeContrast, _ a1: SwiftUI.ColorSchemeContrast) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
}

extension SwiftUI.ColorSchemeContrast: Swift.Equatable {
}

extension SwiftUI.ColorSchemeContrast: Swift.Hashable {
}

extension SwiftUI.ColorSchemeContrast: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.ColorSchemeContrast]
}
