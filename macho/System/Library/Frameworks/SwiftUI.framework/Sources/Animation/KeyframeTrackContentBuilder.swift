// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframeTrackContentBuilder (Animation).

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

public struct KeyframeTrackContentBuilder<A> where A: SwiftUI.Animatable {
    public static func buildBlock() -> some Any { _skeleton() }
    public static func buildPartialBlock<A1, B1>(accumulated a0: A1, next a1: B1) -> some Any where A == A1.Value, A1: SwiftUI.KeyframeTrackContent, B1: SwiftUI.KeyframeTrackContent, A1.Value == B1.Value { _skeleton() }
    public static func buildEither<A1, B1>(first a0: A1) -> SwiftUI.KeyframeTrackContentBuilder<A>.Conditional<A, A1, B1> where A == A1.Value, A1: SwiftUI.KeyframeTrackContent, B1: SwiftUI.KeyframeTrackContent, A1.Value == B1.Value { _skeleton() }
    public static func buildEither<A1, B1>(second a0: B1) -> SwiftUI.KeyframeTrackContentBuilder<A>.Conditional<A, A1, B1> where A == A1.Value, A1: SwiftUI.KeyframeTrackContent, B1: SwiftUI.KeyframeTrackContent, A1.Value == B1.Value { _skeleton() }
    public static func buildExpression<A1>(_ a0: A1) -> A1 where A == A1.Value, A1: SwiftUI.KeyframeTrackContent { _skeleton() }
    public static func buildPartialBlock<A1>(first a0: A1) -> A1 where A == A1.Value, A1: SwiftUI.KeyframeTrackContent { _skeleton() }
    public struct Conditional<A1, B1, C1> where A1 == B1.Value, B1: SwiftUI.KeyframeTrackContent, C1: SwiftUI.KeyframeTrackContent, B1.Value == C1.Value {
        let storage: SwiftUI.KeyframeTrackContentBuilder<A>.Conditional<A1, B1, C1>.Storage
        func collectSegments<A2>(into a0: inout [SwiftUI.KeyframeSegment<A2>]) where A2: SwiftUI.Animatable { _skeleton() }
        init(storage a0: SwiftUI.KeyframeTrackContentBuilder<A>.Conditional<A1, B1, C1>.Storage) { _skeleton() }
        enum Storage {
            case first(B1)
            case second(C1)
        }
    }
}

extension SwiftUI.KeyframeTrackContentBuilder.Conditional: SwiftUI.KeyframeSegmentSource {
}

extension SwiftUI.KeyframeTrackContentBuilder.Conditional: SwiftUI.KeyframeTrackContent {
    public typealias Value = A1
}
