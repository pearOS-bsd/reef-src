// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PublisherSubscription (Misc).

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

class PublisherSubscription {
    var cancellable: Combine.AnyCancellable? = _skeletonValue()
    var publisherBits: [Swift.UInt8] = _skeletonValue()
    init() { _skeleton() }
    deinit {}
}

extension SwiftUI.PublisherSubscription: SwiftUI.GraphTeardown {
    func teardown() { _skeleton() }
}
