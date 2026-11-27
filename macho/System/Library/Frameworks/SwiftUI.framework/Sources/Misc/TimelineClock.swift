// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TimelineClock (Misc).

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

class TimelineClock {
    weak var scope: SwiftUI.BodyNode?
    var animating: Swift.Bool = _skeletonValue()
    var timer: Timer? = _skeletonValue()
    var lastFrame: Swift.Double = _skeletonValue()
    var minimumInterval: Swift.Double? = _skeletonValue()
    final func animate(_ a0: Swift.Bool) { _skeleton() }
    final func wake(at a0: Foundation.Date?) { _skeleton() }
    final func teardown() { _skeleton() }
    init() { _skeleton() }
    deinit {}
}

extension SwiftUI.TimelineClock: SwiftUI.GraphTeardown {
}
