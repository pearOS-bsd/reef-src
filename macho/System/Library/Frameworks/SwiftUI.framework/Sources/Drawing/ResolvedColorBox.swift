// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ResolvedColorBox (Drawing).

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

class ResolvedColorBox: SwiftUI.AnyColorBox {
    let red: Swift.Double = _skeletonValue()
    let green: Swift.Double = _skeletonValue()
    let blue: Swift.Double = _skeletonValue()
    let opacity: Swift.Double = _skeletonValue()
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool { _skeleton() }
    override func hash(into a0: inout Swift.Hasher) { _skeleton() }
    override var nsColor: NSColor { _skeleton() }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
    init(red a0: Swift.Double, green a1: Swift.Double, blue a2: Swift.Double, opacity a3: Swift.Double) { _skeleton() }
    deinit {}
}
