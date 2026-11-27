// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GraphRule (Graph).

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

class GraphRule {
    var sources: [SwiftUI.GraphSignal] = _skeletonValue()
    var isDirty: Swift.Bool = _skeletonValue()
    weak var host: SwiftUI.ViewHost?
    static func untracked<A>(_ a0: () -> A) -> A { _skeleton() }
    func invalidate(_ a0: SwiftUI.Transaction) { _skeleton() }
    func track<A>(_ a0: () -> A) -> A { _skeleton() }
    static func reading<A>(`as` a0: SwiftUI.GraphRule, _ a1: () -> A) -> A { _skeleton() }
    init() { _skeleton() }
    static var current: SwiftUI.GraphRule? { _skeleton() }
    deinit {}
}
