// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FontBox (Text).

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

class FontBox<A>: SwiftUI.AnyFontBox where A: SwiftUI.FontProvider {
    let base: A = _skeletonValue()
    override func isEqual(to a0: SwiftUI.AnyFontBox) -> Swift.Bool { _skeleton() }
    override func hash(into a0: inout Swift.Hasher) { _skeleton() }
    override func apply(to a0: inout SwiftUI.FontTraits) { _skeleton() }
    init(_ a0: A) { _skeleton() }
    deinit {}
}
