// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PreferredColorSchemeKey (Environment).

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

public struct PreferredColorSchemeKey {
    public static var defaultValue: SwiftUI.ColorScheme? { _skeleton() }
    public static func reduce(value a0: inout SwiftUI.ColorScheme?, nextValue a1: () -> SwiftUI.ColorScheme?) { _skeleton() }
}

extension SwiftUI.PreferredColorSchemeKey: SwiftUI.PreferenceKey {
    public typealias Value = SwiftUI.ColorScheme?
}
