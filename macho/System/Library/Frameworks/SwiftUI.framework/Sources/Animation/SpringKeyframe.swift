// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SpringKeyframe (Animation).

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

public struct SpringKeyframe<A> where A: SwiftUI.Animatable {
    let to: A
    let duration: Swift.Double?
    let spring: SwiftUI.Spring
    func collectSegments<A1>(into a0: inout [SwiftUI.KeyframeSegment<A1>]) where A1: SwiftUI.Animatable { _skeleton() }
    public init(_ a0: A, duration a1: Swift.Double?, spring a2: SwiftUI.Spring, startVelocity a3: A?) { _skeleton() }
}

extension SwiftUI.SpringKeyframe: SwiftUI.KeyframeSegmentSource {
}

extension SwiftUI.SpringKeyframe: SwiftUI.KeyframeTrackContent {
    public typealias Value = A
}
