// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ButtonRole (Controls).

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

public struct ButtonRole {
    let rawValue: Swift.Int
    public static var destructive: SwiftUI.ButtonRole { _skeleton() }
    public static var cancel: SwiftUI.ButtonRole { _skeleton() }
    public static var confirm: SwiftUI.ButtonRole { _skeleton() }
    public static var close: SwiftUI.ButtonRole { _skeleton() }
    public static func == (_ a0: SwiftUI.ButtonRole, _ a1: SwiftUI.ButtonRole) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
}

extension SwiftUI.ButtonRole: Swift.Equatable {
}

extension SwiftUI.ButtonRole: Swift.Hashable {
}
