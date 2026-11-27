// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DefaultsSubscription (Misc).

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

class DefaultsSubscription {
    var observer: (any NSObjectProtocol)? = _skeletonValue()
    init(node a0: SwiftUI.BodyNode, store a1: UserDefaults?, key a2: Swift.String?) { _skeleton() }
    deinit {}
}

extension SwiftUI.DefaultsSubscription: SwiftUI.GraphTeardown {
    func teardown() { _skeleton() }
}
