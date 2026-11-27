// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LocatedInteraction (Misc).

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

final class LocatedInteraction: SwiftUI.ButtonInteraction {
    var onPoint: ((CGPoint, CGRect, Swift.Bool) -> ())? = _skeletonValue()
    override func begin(at a0: CGPoint, event a1: NSEvent) { _skeleton() }
    override func drag(to a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
    override func end(at a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
    deinit {}
}
