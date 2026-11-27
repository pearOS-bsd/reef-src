// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ConcatenatedTextStorage (State).

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

class ConcatenatedTextStorage: SwiftUI.AnyTextStorage {
    let first: SwiftUI.Text = _skeletonValue()
    let second: SwiftUI.Text = _skeletonValue()
    override var plainString: Swift.String { _skeleton() }
    init(_ a0: SwiftUI.Text, _ a1: SwiftUI.Text) { _skeleton() }
    deinit {}
}
