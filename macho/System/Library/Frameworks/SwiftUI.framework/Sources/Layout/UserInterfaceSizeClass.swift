// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// UserInterfaceSizeClass (Layout).

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

public enum UserInterfaceSizeClass {
    case compact
    case regular
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.UserInterfaceSizeClass, _ a1: SwiftUI.UserInterfaceSizeClass) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.UserInterfaceSizeClass: Swift.Equatable {
}

extension SwiftUI.UserInterfaceSizeClass: Swift.Hashable {
}
