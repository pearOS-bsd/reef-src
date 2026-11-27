// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GestureRecognizer (Gestures).

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

class GestureRecognizer {
    var changeHandlers: [(Any) -> ()] = _skeletonValue()
    var endHandlers: [(Any) -> ()] = _skeletonValue()
    var resetHandlers: [() -> ()] = _skeletonValue()
    func began(at a0: CGPoint, frame a1: CGRect, event a2: NSEvent) { _skeleton() }
    func moved(to a0: CGPoint, frame a1: CGRect, event a2: NSEvent) { _skeleton() }
    func ended(at a0: CGPoint, frame a1: CGRect, inside a2: Swift.Bool, event a3: NSEvent) { _skeleton() }
    func adoptActions(of a0: SwiftUI.GestureRecognizer) { _skeleton() }
    final func changed(_ a0: Any) { _skeleton() }
    final func finished(_ a0: Any) { _skeleton() }
    final func reset() { _skeleton() }
    init() { _skeleton() }
    deinit {}
}
