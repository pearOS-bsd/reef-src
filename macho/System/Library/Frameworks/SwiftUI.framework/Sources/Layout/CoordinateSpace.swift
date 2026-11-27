// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoordinateSpace (Layout).

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

public enum CoordinateSpace {
    case named(Swift.AnyHashable)
    case global
    case local
    public static func == (_ a0: SwiftUI.CoordinateSpace, _ a1: SwiftUI.CoordinateSpace) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
}

extension SwiftUI.CoordinateSpace: Swift.Hashable {
}

extension SwiftUI.CoordinateSpace: Swift.Equatable {
}
