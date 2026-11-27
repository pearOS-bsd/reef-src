// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SystemColorBox (Drawing).

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

class SystemColorBox: SwiftUI.AnyColorBox {
    let light: (red: Swift.Double, green: Swift.Double, blue: Swift.Double) = _skeletonValue()
    let dark: (red: Swift.Double, green: Swift.Double, blue: Swift.Double) = _skeletonValue()
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool { _skeleton() }
    override func hash(into a0: inout Swift.Hasher) { _skeleton() }
    override var nsColor: NSColor { _skeleton() }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
    init(light a0: (red: Swift.Double, green: Swift.Double, blue: Swift.Double), dark a1: (red: Swift.Double, green: Swift.Double, blue: Swift.Double)) { _skeleton() }
    deinit {}
}
