// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframeTimeline (Animation).

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

public struct KeyframeTimeline<A> {
    let initialValue: A
    let tracks: [SwiftUI.KeyframeTrackEvaluator]
    public init<A1>(initialValue a0: A, content a1: () -> A1) where A == A1.Value, A1: SwiftUI.Keyframes { _skeleton() }
    init<A1>(initialValue a0: A, keyframes a1: A1) where A1: SwiftUI.Keyframes { _skeleton() }
    public var duration: Swift.Double { _skeleton() }
    public func value(time a0: Swift.Double) -> A { _skeleton() }
    public func value(progress a0: Swift.Double) -> A { _skeleton() }
}
