// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GestureElement (Gestures).

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

class GestureElement: SwiftUI.WrapperElement {
    let session: SwiftUI.GestureSession = _skeletonValue()
    var isSimultaneous: Swift.Bool = _skeletonValue()
    override func takesEvents(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    init(_ a0: SwiftUI.Element, session a1: SwiftUI.GestureSession) { _skeleton() }
    deinit {}
}

extension SwiftUI.GestureElement: SwiftUI.PressableElement {
    var pressTarget: any SwiftUI.PressTarget { _skeleton() }
    var focusOnClick: Swift.Bool { _skeleton() }
}
