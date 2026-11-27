// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CubicKeyframe (Animation).

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

public struct CubicKeyframe<A> where A: SwiftUI.Animatable {
    let to: A
    let duration: Swift.Double
    func collectSegments<A1>(into a0: inout [SwiftUI.KeyframeSegment<A1>]) where A1: SwiftUI.Animatable { _skeleton() }
    public init(_ a0: A, duration a1: Swift.Double, startVelocity a2: A?, endVelocity a3: A?) { _skeleton() }
}

extension SwiftUI.CubicKeyframe: SwiftUI.KeyframeSegmentSource {
}

extension SwiftUI.CubicKeyframe: SwiftUI.KeyframeTrackContent {
    public typealias Value = A
}
