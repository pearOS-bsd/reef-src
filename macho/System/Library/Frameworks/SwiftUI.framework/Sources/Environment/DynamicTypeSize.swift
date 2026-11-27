// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DynamicTypeSize (Environment).

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

public enum DynamicTypeSize {
    case xSmall
    case small
    case medium
    case large
    case xLarge
    case xxLarge
    case xxxLarge
    case accessibility1
    case accessibility2
    case accessibility3
    case accessibility4
    case accessibility5
    public static var allCases: [SwiftUI.DynamicTypeSize] { _skeleton() }
    public var isAccessibilitySize: Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.DynamicTypeSize, _ a1: SwiftUI.DynamicTypeSize) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    public static func < (_ a0: SwiftUI.DynamicTypeSize, _ a1: SwiftUI.DynamicTypeSize) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.DynamicTypeSize: Swift.Equatable {
}

extension SwiftUI.DynamicTypeSize: Swift.Hashable {
}

extension SwiftUI.DynamicTypeSize: Swift.Comparable {
}

extension SwiftUI.DynamicTypeSize: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.DynamicTypeSize]
}
