// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShapeRole (Drawing).

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

public enum ShapeRole {
    case fill
    case stroke
    case separator
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.ShapeRole, _ a1: SwiftUI.ShapeRole) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ShapeRole: Swift.Equatable {
}

extension SwiftUI.ShapeRole: Swift.Hashable {
}
