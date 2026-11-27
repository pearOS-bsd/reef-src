// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MappedRecognizer (Misc).

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

class MappedRecognizer: SwiftUI.GestureRecognizer {
    let inner: SwiftUI.GestureRecognizer = _skeletonValue()
    init(_ a0: SwiftUI.GestureRecognizer, transform a1: @escaping (Any) -> Any?) { _skeleton() }
    override func began(at a0: CGPoint, frame a1: CGRect, event a2: NSEvent) { _skeleton() }
    override func moved(to a0: CGPoint, frame a1: CGRect, event a2: NSEvent) { _skeleton() }
    override func ended(at a0: CGPoint, frame a1: CGRect, inside a2: Swift.Bool, event a3: NSEvent) { _skeleton() }
    deinit {}
}
