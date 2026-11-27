// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LiftedElements (Misc).

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

final class LiftedElements: SwiftUI.Element {
    var lifting: [SwiftUI.LiftedElements.WeakLifting] = _skeletonValue()
    var active: [any SwiftUI.LiftingElement] { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    var traceDescription: Swift.String { _skeleton() }
    deinit {}
    struct WeakLifting {
        weak var element: (any SwiftUI.LiftingElement)?
    }
}
