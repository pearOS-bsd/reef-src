// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlSize (Environment).

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

public enum ControlSize {
    case mini
    case small
    case regular
    case large
    case extraLarge
    public static var allCases: [SwiftUI.ControlSize] { [.mini, .small, .regular, .large, .extraLarge] }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.ControlSize, _ a1: SwiftUI.ControlSize) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
}

extension SwiftUI.ControlSize: Swift.Equatable {
}

extension SwiftUI.ControlSize: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.ControlSize]
}

extension SwiftUI.ControlSize: Swift.Hashable {
}
