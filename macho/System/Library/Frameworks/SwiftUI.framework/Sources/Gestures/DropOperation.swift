// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DropOperation (Gestures).

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

public enum DropOperation {
    case copy
    case move
    case delete
    case cancel
    case forbidden
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.DropOperation, _ a1: SwiftUI.DropOperation) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.DropOperation: Swift.Equatable {
}

extension SwiftUI.DropOperation: Swift.Hashable {
}
