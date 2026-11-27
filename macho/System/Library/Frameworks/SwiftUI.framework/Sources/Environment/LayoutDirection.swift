// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LayoutDirection (Environment).

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

public enum LayoutDirection {
    case leftToRight
    case rightToLeft
    public static var allCases: [SwiftUI.LayoutDirection] { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.LayoutDirection, _ a1: SwiftUI.LayoutDirection) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
}

extension SwiftUI.LayoutDirection: Swift.Equatable {
}

extension SwiftUI.LayoutDirection: Swift.Hashable {
}

extension SwiftUI.LayoutDirection: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.LayoutDirection]
}
