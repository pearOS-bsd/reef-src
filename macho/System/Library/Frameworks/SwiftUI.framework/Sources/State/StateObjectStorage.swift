// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StateObjectStorage (State).

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

class StateObjectStorage<A> where A: Combine.ObservableObject {
    var object: A? = _skeletonValue()
    let subscription: SwiftUI.ObjectSubscription = _skeletonValue()
    final func teardown() { _skeleton() }
    init(node a0: SwiftUI.BodyNode) { _skeleton() }
    deinit {}
}

extension SwiftUI.StateObjectStorage: SwiftUI.GraphTeardown {
}
