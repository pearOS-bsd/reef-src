// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListSelectInteraction (Lists).

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

class ListSelectInteraction {
    weak var list: SwiftUI.ListContentElement?
    var start: CGPoint? = _skeletonValue()
    var pendingSelect: Swift.Bool = _skeletonValue()
    var dragging: Swift.Bool = _skeletonValue()
    final func begin(at a0: CGPoint, event a1: NSEvent) { _skeleton() }
    final func drag(to a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
    init() { _skeleton() }
    deinit {}
}

extension SwiftUI.ListSelectInteraction: SwiftUI.PressTarget {
    func contains(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    func end(at a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
}
