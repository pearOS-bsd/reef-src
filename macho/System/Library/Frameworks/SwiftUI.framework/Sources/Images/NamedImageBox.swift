// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NamedImageBox (Images).

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

class NamedImageBox: SwiftUI.AnyImageProviderBox {
    let name: Swift.String = _skeletonValue()
    let bundle: Bundle? = _skeletonValue()
    let isSystem: Swift.Bool = _skeletonValue()
    lazy var assetSymbol: NSImage? = _skeletonValue()
    override func isEqual(to a0: SwiftUI.AnyImageProviderBox) -> Swift.Bool { _skeleton() }
    init(name a0: Swift.String, bundle a1: Bundle?, isSystem a2: Swift.Bool) { _skeleton() }
    deinit {}
}
