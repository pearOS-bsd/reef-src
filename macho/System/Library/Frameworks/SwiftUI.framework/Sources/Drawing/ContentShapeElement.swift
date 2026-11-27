// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContentShapeElement (Drawing).

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

class ContentShapeElement: SwiftUI.WrapperElement {
    let shape: (CGRect) -> CGPath = _skeletonValue()
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    init(_ a0: SwiftUI.Element, shape a1: @escaping (CGRect) -> CGPath) { _skeleton() }
    deinit {}
}
