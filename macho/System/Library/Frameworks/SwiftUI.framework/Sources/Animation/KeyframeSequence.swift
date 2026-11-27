// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframeSequence (Animation).

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

struct KeyframeSequence<A, B> where A: SwiftUI.KeyframeTrackContent, B: SwiftUI.KeyframeTrackContent, A.Value == B.Value {
    let first: A
    let second: B
    func collectSegments<A1>(into a0: inout [SwiftUI.KeyframeSegment<A1>]) where A1: SwiftUI.Animatable { _skeleton() }
    init(first a0: A, second a1: B) { _skeleton() }
}

extension SwiftUI.KeyframeSequence: SwiftUI.KeyframeTrackContent {
    typealias Value = A.Value
}

extension SwiftUI.KeyframeSequence: SwiftUI.KeyframeSegmentSource {
}
