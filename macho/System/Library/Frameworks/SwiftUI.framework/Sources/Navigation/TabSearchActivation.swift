// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabSearchActivation (Navigation).

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

public struct TabSearchActivation {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.TabSearchActivation { SwiftUI.TabSearchActivation(rawValue: 0) }
    public static var searchTabSelection: SwiftUI.TabSearchActivation { _skeleton() }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.TabSearchActivation, _ a1: SwiftUI.TabSearchActivation) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.TabSearchActivation: Swift.Hashable {
}

extension SwiftUI.TabSearchActivation: Swift.Equatable {
}
