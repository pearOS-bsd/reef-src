// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyColorBox (Drawing).

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

open class AnyColorBox {
    var nsColor: NSColor { NSColor.clear }
    func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved {
        // the AppKit colour read in the environment's colour scheme
        SwiftUI.Color.Resolved(nsColor, colorScheme: a0[SwiftUI.ColorSchemeKey.self])
    }
    func hash(into a0: inout Swift.Hasher) { a0.combine(ObjectIdentifier(self)) }
    func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool { self === a0 }
    init() {
    }
    deinit {}
}
