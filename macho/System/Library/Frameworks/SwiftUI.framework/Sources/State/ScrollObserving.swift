// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollObserving (State).

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

protocol ScrollObserving {
    func geometryChanged(_ a0: SwiftUI.ScrollGeometry, state a1: SwiftUI.ScrollObservation)
    func phaseChanged(from a0: SwiftUI.ScrollPhase, to a1: SwiftUI.ScrollPhase, geometry a2: SwiftUI.ScrollGeometry, velocity a3: CGVector)
}
