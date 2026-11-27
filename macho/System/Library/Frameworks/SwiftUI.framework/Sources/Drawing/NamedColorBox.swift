// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NamedColorBox (Drawing).

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

class NamedColorBox: SwiftUI.AnyColorBox {
    let name: Swift.String = _skeletonValue()
    let bundle: Bundle? = _skeletonValue()
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool { _skeleton() }
    override func hash(into a0: inout Swift.Hasher) { _skeleton() }
    override var nsColor: NSColor { _skeleton() }
    init(name a0: Swift.String, bundle a1: Bundle?) { _skeleton() }
    deinit {}
}
