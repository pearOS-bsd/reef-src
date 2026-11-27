// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LegibilityWeight (Misc).

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

public enum LegibilityWeight {
    case regular
    case bold
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.LegibilityWeight, _ a1: SwiftUI.LegibilityWeight) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.LegibilityWeight: Swift.Equatable {
}

extension SwiftUI.LegibilityWeight: Swift.Hashable {
}
