// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollGeometryObserver (State).

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

struct ScrollGeometryObserver<A> where A: Swift.Equatable {
    let transform: (SwiftUI.ScrollGeometry) -> A
    let action: (A, A) -> ()
    func geometryChanged(_ a0: SwiftUI.ScrollGeometry, state a1: SwiftUI.ScrollObservation) { _skeleton() }
    init(transform a0: @escaping (SwiftUI.ScrollGeometry) -> A, action a1: @escaping (A, A) -> ()) { _skeleton() }
}

extension SwiftUI.ScrollGeometryObserver: SwiftUI.ScrollObserving {
    func phaseChanged(from a0: SwiftUI.ScrollPhase, to a1: SwiftUI.ScrollPhase, geometry a2: SwiftUI.ScrollGeometry, velocity a3: CGVector) { _skeleton() }
}
