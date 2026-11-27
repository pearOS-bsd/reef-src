// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NamedColorBox (Drawing).

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

class NamedColorBox: SwiftUI.AnyColorBox {
    let name: Swift.String
    let bundle: Bundle?
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool {
        guard let other = a0 as? Self, name == other.name else { return false }
        switch (bundle, other.bundle) {
        case (nil, nil): return true
        case let (a?, b?): return a == b
        default: return false
        }
    }
    override func hash(into a0: inout Swift.Hasher) { a0.combine(name) }
    override var nsColor: NSColor { NSColor(named: name, bundle: bundle) ?? NSColor.clear }
    init(name a0: Swift.String, bundle a1: Bundle?) {
        name = a0
        bundle = a1
        super.init()
    }
    deinit {}
}
