// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SheetActionsKey (Navigation).

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

struct SheetActionsKey {
    static var defaultValue: SwiftUI.SheetActions { _skeleton() }
}

extension SwiftUI.SheetActionsKey: SwiftUI.PreferenceKey {
    typealias Value = SwiftUI.SheetActions
    static func reduce(value a0: inout Self.Value, nextValue a1: () -> Self.Value) { _skeleton() }
}
