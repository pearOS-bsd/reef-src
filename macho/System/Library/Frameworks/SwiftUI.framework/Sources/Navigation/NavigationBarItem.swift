// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationBarItem (Navigation).

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

public struct NavigationBarItem {
    public enum TitleDisplayMode {
        case automatic
        case inline
        case large
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.NavigationBarItem.TitleDisplayMode, _ a1: SwiftUI.NavigationBarItem.TitleDisplayMode) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
    }
}

extension SwiftUI.NavigationBarItem.TitleDisplayMode: Swift.Equatable {
}

extension SwiftUI.NavigationBarItem.TitleDisplayMode: Swift.Hashable {
}
