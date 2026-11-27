// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ConstantLocation (State).

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

class ConstantLocation<A>: SwiftUI.AnyLocation<A> {
    let value: A
    init(_ a0: A) {
        value = a0
        super.init()
    }
    override func set(_ a0: A, transaction a1: SwiftUI.Transaction) {}
    override func get() -> A { value }
    deinit {}
}
