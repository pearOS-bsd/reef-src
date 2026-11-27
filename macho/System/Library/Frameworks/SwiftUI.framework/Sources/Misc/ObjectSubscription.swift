// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ObjectSubscription (Misc).

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

class ObjectSubscription {
    weak var node: SwiftUI.BodyNode?
    var object: AnyObject? = _skeletonValue()
    var cancellable: Combine.AnyCancellable? = _skeletonValue()
    final func observe<A>(_ a0: A) where A: Combine.ObservableObject { _skeleton() }
    init(node a0: SwiftUI.BodyNode) { _skeleton() }
    deinit {}
}

extension SwiftUI.ObjectSubscription: SwiftUI.GraphTeardown {
    func teardown() { _skeleton() }
}
