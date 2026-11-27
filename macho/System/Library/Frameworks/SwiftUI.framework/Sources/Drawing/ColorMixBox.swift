// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ColorMixBox (Drawing).

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

class ColorMixBox: SwiftUI.AnyColorBox {
    let base: SwiftUI.AnyColorBox = _skeletonValue()
    let other: SwiftUI.AnyColorBox = _skeletonValue()
    let fraction: Swift.Double = _skeletonValue()
    let colorSpace: SwiftUI.Gradient.ColorSpace = _skeletonValue()
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool { _skeleton() }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
    init(base a0: SwiftUI.AnyColorBox, other a1: SwiftUI.AnyColorBox, fraction a2: Swift.Double, colorSpace a3: SwiftUI.Gradient.ColorSpace) { _skeleton() }
    deinit {}
}
