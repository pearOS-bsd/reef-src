// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SpacingKind (Layout).

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

enum SpacingKind {
    case text(CoreGraphics.CGFloat)
    case image
    case control
    case checkbox
    case toggleSwitch
    case spacer
    case other
    static func == (_ a0: SwiftUI.SpacingKind, _ a1: SwiftUI.SpacingKind) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.SpacingKind: Swift.Equatable {
}
