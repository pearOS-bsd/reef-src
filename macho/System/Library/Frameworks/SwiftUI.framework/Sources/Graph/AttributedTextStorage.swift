// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AttributedTextStorage (Graph).

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

class AttributedTextStorage: SwiftUI.AnyTextStorage {
    let attributed: Foundation.AttributedString = _skeletonValue()
    override var plainString: Swift.String { _skeleton() }
    init(_ a0: Foundation.AttributedString) { _skeleton() }
    deinit {}
}
