// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GraphSignal (Graph).

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

class GraphSignal {
    var readers: [Swift.ObjectIdentifier: SwiftUI.GraphSignal.Reader] = _skeletonValue()
    final func noteRead() { _skeleton() }
    final func changed(_ a0: SwiftUI.Transaction) { _skeleton() }
    init() { _skeleton() }
    deinit {}
    struct Reader {
        weak var rule: SwiftUI.GraphRule?
    }
}
