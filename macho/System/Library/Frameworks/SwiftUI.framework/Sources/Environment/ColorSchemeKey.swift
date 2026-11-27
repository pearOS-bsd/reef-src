// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ColorSchemeKey (Environment).

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

enum ColorSchemeKey {
}

extension SwiftUI.ColorSchemeKey: SwiftUI.EnvironmentKey {
    typealias Value = SwiftUI.ColorScheme
    // the application's appearance
    static var defaultValue: Self.Value { SwiftUI.SchemeAppearance.scheme(of: NSApp?.effectiveAppearance) }
}
