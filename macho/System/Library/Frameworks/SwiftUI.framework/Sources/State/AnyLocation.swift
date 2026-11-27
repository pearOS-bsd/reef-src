// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyLocation (State).

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

open class AnyLocation<A>: SwiftUI.AnyLocationBase {
    public override init() { super.init() }
    func get() -> A { fatalError("abstract location") }
    func set(_ a0: A, transaction a1: SwiftUI.Transaction) { fatalError("abstract location") }
    deinit {}
}
