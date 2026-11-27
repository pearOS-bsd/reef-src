// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SymbolVariantsKey (Images).

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

enum SymbolVariantsKey {
}

extension SwiftUI.SymbolVariantsKey: SwiftUI.EnvironmentKey {
    typealias Value = SwiftUI.SymbolVariants
    static var defaultValue: Self.Value { _skeleton() }
}
