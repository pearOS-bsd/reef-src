// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControllerHolder (Controls).

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

class ControllerHolder<A> where A: NSViewController {
    let controller: A = _skeletonValue()
    var coordinator: Any? = _skeletonValue()
    init(_ a0: A) { _skeleton() }
    final func leave() { _skeleton() }
    final func teardown() { _skeleton() }
    deinit {}
}

extension SwiftUI.ControllerHolder: SwiftUI.GraphPresence {
}

extension SwiftUI.ControllerHolder: SwiftUI.GraphTeardown {
}
