// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContinuousHoverState (State).

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

class ContinuousHoverState {
    var space: SwiftUI.CoordinateSpace = _skeletonValue()
    var action: (SwiftUI.HoverPhase) -> () = _skeletonValue()
    var last: SwiftUI.HoverPhase? = _skeletonValue()
    final func update(_ a0: SwiftUI.HoverPhase) { _skeleton() }
    init() { _skeleton() }
    deinit {}
}
