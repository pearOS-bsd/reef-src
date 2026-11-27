// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ModifierKeysMonitor (Views).

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

class ModifierKeysMonitor {
    var monitor: Any? = _skeletonValue()
    var current: SwiftUI.EventModifiers = _skeletonValue()
    deinit {}
}

extension SwiftUI.ModifierKeysMonitor: SwiftUI.GraphTeardown {
    func teardown() { _skeleton() }
}
