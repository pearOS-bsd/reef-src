// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ImageOptionsBox (Images).

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

class ImageOptionsBox: SwiftUI.AnyImageProviderBox {
    let base: SwiftUI.AnyImageProviderBox = _skeletonValue()
    var resizable: Swift.Bool = _skeletonValue()
    var renderingMode: SwiftUI.Image.TemplateRenderingMode? = _skeletonValue()
    var symbolRenderingMode: SwiftUI.SymbolRenderingMode? = _skeletonValue()
    init(base a0: SwiftUI.AnyImageProviderBox) { _skeleton() }
    override func isEqual(to a0: SwiftUI.AnyImageProviderBox) -> Swift.Bool { _skeleton() }
    deinit {}
}
