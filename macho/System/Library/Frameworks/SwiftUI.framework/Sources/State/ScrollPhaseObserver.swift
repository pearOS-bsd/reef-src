// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollPhaseObserver (State).

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

struct ScrollPhaseObserver {
    let action: (SwiftUI.ScrollPhase, SwiftUI.ScrollPhase, SwiftUI.ScrollPhaseChangeContext) -> ()
}

extension SwiftUI.ScrollPhaseObserver: SwiftUI.ScrollObserving {
    func geometryChanged(_ a0: SwiftUI.ScrollGeometry, state a1: SwiftUI.ScrollObservation) { _skeleton() }
    func phaseChanged(from a0: SwiftUI.ScrollPhase, to a1: SwiftUI.ScrollPhase, geometry a2: SwiftUI.ScrollGeometry, velocity a3: CGVector) { _skeleton() }
}
