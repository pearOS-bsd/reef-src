// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BuiltinButtonKind (Controls).

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

enum BuiltinButtonKind {
    case bordered
    case prominent
    case borderless
    case plain
    case link
    case accessoryBar
    case accessoryBarAction
}

extension SwiftUI.BuiltinButtonKind: Swift.Hashable {
}

extension SwiftUI.BuiltinButtonKind: Swift.Equatable {
}
