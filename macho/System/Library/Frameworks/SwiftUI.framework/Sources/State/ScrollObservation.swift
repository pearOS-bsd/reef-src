// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollObservation (State).

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

final class ScrollObservation: NSObject {
    var observer: (any SwiftUI.ScrollObserving)? = _skeletonValue()
    var lastValue: Any? = _skeletonValue()
    weak var holder: SwiftUI.ScrollHolder?
    var phase: SwiftUI.ScrollPhase = _skeletonValue()
    var liveScrolling: Swift.Bool = _skeletonValue()
    var settleGeneration: Swift.Int = _skeletonValue()
    var samples: [(time: Swift.Double, offset: CGPoint)] = _skeletonValue()
    func attach(_ a0: SwiftUI.ScrollHolder) { _skeleton() }
    func boundsChanged() { _skeleton() }
    func setPhase(_ a0: SwiftUI.ScrollPhase) { _skeleton() }
    func frameChanged(_ a0: Foundation.Notification) { _skeleton() }
    deinit {}
}
