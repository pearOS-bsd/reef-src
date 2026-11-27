// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframesPair (Animation).

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

struct KeyframesPair<A, B> where A: SwiftUI.Keyframes, B: SwiftUI.Keyframes, A.Value == B.Value {
    let first: A
    let second: B
    func collectTracks(into a0: inout [SwiftUI.KeyframeTrackEvaluator]) { _skeleton() }
    init(first a0: A, second a1: B) { _skeleton() }
}

extension SwiftUI.KeyframesPair: SwiftUI.Keyframes {
    typealias Value = A.Value
}

extension SwiftUI.KeyframesPair: SwiftUI.KeyframeTrackSource {
}
