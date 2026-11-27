// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SpatialTapRecognizer (Gestures).

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

class SpatialTapRecognizer: SwiftUI.GestureRecognizer {
    let count: Swift.Int = _skeletonValue()
    let space: SwiftUI.CoordinateSpace = _skeletonValue()
    override func ended(at a0: CGPoint, frame a1: CGRect, inside a2: Swift.Bool, event a3: NSEvent) { _skeleton() }
    init(count a0: Swift.Int, space a1: SwiftUI.CoordinateSpace) { _skeleton() }
    deinit {}
}
