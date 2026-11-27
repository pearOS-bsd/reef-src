// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SymbolColorRenderingMode (Images).

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

public struct SymbolColorRenderingMode {
    let rawValue: Swift.Int
    public static var flat: SwiftUI.SymbolColorRenderingMode { SwiftUI.SymbolColorRenderingMode(rawValue: 0) }
    public static var gradient: SwiftUI.SymbolColorRenderingMode { SwiftUI.SymbolColorRenderingMode(rawValue: 1) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.SymbolColorRenderingMode, _ a1: SwiftUI.SymbolColorRenderingMode) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.SymbolColorRenderingMode: Swift.Hashable {
}

extension SwiftUI.SymbolColorRenderingMode: Swift.Equatable {
}
