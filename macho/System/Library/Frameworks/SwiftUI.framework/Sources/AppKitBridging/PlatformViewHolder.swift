// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PlatformViewHolder (AppKitBridging).

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

class PlatformViewHolder<A> where A: NSView {
    let view: A = _skeletonValue()
    var coordinator: Any? = _skeletonValue()
    var dismantle: (() -> ())? = _skeletonValue()
    init(_ a0: A) { _skeleton() }
    final func leave() { _skeleton() }
    final func teardown() { _skeleton() }
    deinit {}
}

extension SwiftUI.PlatformViewHolder: SwiftUI.GraphPresence {
}

extension SwiftUI.PlatformViewHolder: SwiftUI.GraphTeardown {
}
