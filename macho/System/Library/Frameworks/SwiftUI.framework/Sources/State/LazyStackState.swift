// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LazyStackState (State).

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

class LazyStackState {
    var realized: Swift.Set<[Swift.AnyHashable]> = _skeletonValue()
    var present: Swift.Set<[Swift.AnyHashable]> = _skeletonValue()
    var lengths: [[Swift.AnyHashable]: CoreGraphics.CGFloat] = _skeletonValue()
    var rowLength: CoreGraphics.CGFloat = _skeletonValue()
    weak var element: SwiftUI.LazyStackElement?
    weak var holder: SwiftUI.ScrollHolder?
    var layoutScheduled: Swift.Bool = _skeletonValue()
    final func scheduleLayout() { _skeleton() }
    init() { _skeleton() }
    deinit {}
}

extension SwiftUI.LazyStackState: SwiftUI.GraphTeardown {
    func teardown() { _skeleton() }
}
