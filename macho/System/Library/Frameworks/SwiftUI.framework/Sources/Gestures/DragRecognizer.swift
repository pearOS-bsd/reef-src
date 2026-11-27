// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DragRecognizer (Gestures).

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

class DragRecognizer: SwiftUI.GestureRecognizer {
    let minimumDistance: CoreGraphics.CGFloat = _skeletonValue()
    let space: SwiftUI.CoordinateSpace = _skeletonValue()
    var start: CGPoint = _skeletonValue()
    var origin: CGPoint = _skeletonValue()
    var active: Swift.Bool = _skeletonValue()
    override func began(at a0: CGPoint, frame a1: CGRect, event a2: NSEvent) { _skeleton() }
    override func moved(to a0: CGPoint, frame a1: CGRect, event a2: NSEvent) { _skeleton() }
    override func ended(at a0: CGPoint, frame a1: CGRect, inside a2: Swift.Bool, event a3: NSEvent) { _skeleton() }
    init(minimumDistance a0: CoreGraphics.CGFloat, space a1: SwiftUI.CoordinateSpace) { _skeleton() }
    deinit {}
}
