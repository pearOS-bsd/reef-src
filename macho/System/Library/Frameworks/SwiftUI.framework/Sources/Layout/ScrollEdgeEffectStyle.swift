// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollEdgeEffectStyle (Layout).

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

public struct ScrollEdgeEffectStyle {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.ScrollEdgeEffectStyle { SwiftUI.ScrollEdgeEffectStyle(rawValue: 0) }
    public static var hard: SwiftUI.ScrollEdgeEffectStyle { SwiftUI.ScrollEdgeEffectStyle(rawValue: 1) }
    public static var soft: SwiftUI.ScrollEdgeEffectStyle { SwiftUI.ScrollEdgeEffectStyle(rawValue: 2) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.ScrollEdgeEffectStyle, _ a1: SwiftUI.ScrollEdgeEffectStyle) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.ScrollEdgeEffectStyle: Swift.Hashable {
}

extension SwiftUI.ScrollEdgeEffectStyle: Swift.Equatable {
}
