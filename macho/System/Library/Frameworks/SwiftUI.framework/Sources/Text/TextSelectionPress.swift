// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextSelectionPress (Text).

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

class TextSelectionPress {
    let element: SwiftUI.SelectableTextElement = _skeletonValue()
    var anchor: _NSRange = _skeletonValue()
    var clicks: Swift.Int = _skeletonValue()
    final func begin(at a0: CGPoint, event a1: NSEvent) { _skeleton() }
    init(_ a0: SwiftUI.SelectableTextElement) { _skeleton() }
    deinit {}
}

extension SwiftUI.TextSelectionPress: SwiftUI.PressTarget {
    func contains(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    func drag(to a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
    func end(at a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent) { _skeleton() }
}
