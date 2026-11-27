// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ButtonInteraction (Controls).

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

class ButtonInteraction {
    let pressed: SwiftUI.StoredLocation<Swift.Bool> = _skeletonValue()
    let hovered: SwiftUI.StoredLocation<Swift.Bool> = _skeletonValue()
    var action: () -> () = _skeletonValue()
    var enabled: Swift.Bool = _skeletonValue()
    weak var element: SwiftUI.Element?
    func begin(at a0: CGPoint, event a1: NSEvent) { _skeleton() }
    func drag(to a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
    func end(at a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
    init() { _skeleton() }
    final func contains(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    deinit {}
}

extension SwiftUI.ButtonInteraction: SwiftUI.PressTarget {
}
