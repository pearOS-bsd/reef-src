// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OpacityColorBox (Drawing).

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

class OpacityColorBox: SwiftUI.AnyColorBox {
    let base: SwiftUI.AnyColorBox = _skeletonValue()
    let opacity: Swift.Double = _skeletonValue()
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool { _skeleton() }
    override var nsColor: NSColor { _skeleton() }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
    init(base a0: SwiftUI.AnyColorBox, opacity a1: Swift.Double) { _skeleton() }
    deinit {}
}
