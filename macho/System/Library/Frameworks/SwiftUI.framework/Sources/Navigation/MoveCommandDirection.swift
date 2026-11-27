// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MoveCommandDirection (Navigation).

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

public enum MoveCommandDirection {
    case up
    case down
    case left
    case right
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.MoveCommandDirection, _ a1: SwiftUI.MoveCommandDirection) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.MoveCommandDirection: Swift.Equatable {
}

extension SwiftUI.MoveCommandDirection: Swift.Hashable {
}
