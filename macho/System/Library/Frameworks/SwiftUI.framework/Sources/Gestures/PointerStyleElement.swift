// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PointerStyleElement (Gestures).

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

class PointerStyleElement: SwiftUI.WrapperElement {
    let cursor: NSCursor = _skeletonValue()
    init(_ a0: SwiftUI.Element, cursor a1: NSCursor) { _skeleton() }
    deinit {}
}

extension SwiftUI.PointerStyleElement: SwiftUI.CursorElement {
    func cursor(at a0: CGPoint) -> NSCursor? { _skeleton() }
}
