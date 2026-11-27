// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ValueState (State).

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

class ValueState {
    weak var host: SwiftUI.ViewHost?
    var value: Any? = _skeletonValue()
    var task: (Swift.Task<(), Swift.Never>)? = _skeletonValue()
    var generation: Swift.Int = _skeletonValue()
    var isAway: Swift.Bool = _skeletonValue()
    final func teardown() { _skeleton() }
    init() { _skeleton() }
    deinit {}
}

extension SwiftUI.ValueState: SwiftUI.GraphPresence {
    func leave() { _skeleton() }
}

extension SwiftUI.ValueState: SwiftUI.GraphTeardown {
}
