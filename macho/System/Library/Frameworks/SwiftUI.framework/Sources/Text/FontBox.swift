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
    let base: A
    override func isEqual(to a0: SwiftUI.AnyFontBox) -> Swift.Bool {
        guard let other = a0 as? FontBox<A> else { return false }
        return other === self || other.base == base
    }
    override func hash(into a0: inout Swift.Hasher) { base.hash(into: &a0) }
    override func apply(to a0: inout SwiftUI.FontTraits) { base.apply(to: &a0) }
    init(_ a0: A) {
        base = a0
        super.init()
    }
    deinit {}
}
