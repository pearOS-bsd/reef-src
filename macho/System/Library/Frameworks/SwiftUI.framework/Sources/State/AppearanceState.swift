// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AppearanceState (State).

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

class AppearanceState {
    weak var host: SwiftUI.ViewHost?
    var disappear: (() -> ())? = _skeletonValue()
    var isPresent: Swift.Bool = _skeletonValue()
    final func leave() { _skeleton() }
    final func teardown() { _skeleton() }
    init() { _skeleton() }
    deinit {}
}

extension SwiftUI.AppearanceState: SwiftUI.GraphPresence {
}

extension SwiftUI.AppearanceState: SwiftUI.GraphTeardown {
}
