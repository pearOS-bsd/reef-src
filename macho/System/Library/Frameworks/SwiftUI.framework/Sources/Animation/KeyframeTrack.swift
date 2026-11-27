// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframeTrack (Animation).

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

public struct KeyframeTrack<A, B, C> where B == C.Value, C: SwiftUI.KeyframeTrackContent {
    let keyPath: Swift.WritableKeyPath<A, B>
    let content: C
    func collectTracks(into a0: inout [SwiftUI.KeyframeTrackEvaluator]) { _skeleton() }
    public init(_ a0: Swift.WritableKeyPath<A, B>, content a1: () -> C) { _skeleton() }
}

extension SwiftUI.KeyframeTrack: SwiftUI.KeyframeTrackSource {
}

extension SwiftUI.KeyframeTrack: SwiftUI.Keyframes {
    public typealias Value = B
}
