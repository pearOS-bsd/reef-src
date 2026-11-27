// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LongPressRecognizer (Gestures).

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

class LongPressRecognizer: SwiftUI.GestureRecognizer {
    let duration: Swift.Double = _skeletonValue()
    let maximumDistance: CoreGraphics.CGFloat = _skeletonValue()
    var pressing: ((Swift.Bool) -> ())? = _skeletonValue()
    var start: CGPoint = _skeletonValue()
    var timer: Timer? = _skeletonValue()
    var fired: Swift.Bool = _skeletonValue()
    init(duration a0: Swift.Double, maximumDistance a1: CoreGraphics.CGFloat) { _skeleton() }
    override func began(at a0: CGPoint, frame a1: CGRect, event a2: NSEvent) { _skeleton() }
    override func moved(to a0: CGPoint, frame a1: CGRect, event a2: NSEvent) { _skeleton() }
    override func ended(at a0: CGPoint, frame a1: CGRect, inside a2: Swift.Bool, event a3: NSEvent) { _skeleton() }
    override func adoptActions(of a0: SwiftUI.GestureRecognizer) { _skeleton() }
    deinit {}
}
