// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextSelectionState (State).

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

class TextSelectionState {
    var range: _NSRange? = _skeletonValue()
    var string: Swift.String = _skeletonValue()
    weak var canvas: SwiftUI.ElementCanvas?
    final func select(_ a0: _NSRange, `in` a1: SwiftUI.ElementCanvas) { _skeleton() }
    final var selectedString: Swift.String? { _skeleton() }
    final var emphasized: Swift.Bool { _skeleton() }
    init() { _skeleton() }
    static var holders: [SwiftUI.TextSelectionState.Holder] { _skeleton() }
    deinit {}
    class Holder {
        weak var state: SwiftUI.TextSelectionState?
        deinit {}
    }
}
