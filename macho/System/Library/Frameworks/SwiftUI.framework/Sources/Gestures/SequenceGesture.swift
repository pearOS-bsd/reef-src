// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SequenceGesture (Gestures).

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

public struct SequenceGesture<A, B> where A: SwiftUI.Gesture, B: SwiftUI.Gesture {
    public var first: A
    public var second: B
    public init(_ a0: A, _ a1: B) { _skeleton() }
    @frozen public enum Value {
        case first(A.Value)
        case second(A.Value, B.Value?)
    }
}

extension SwiftUI.SequenceGesture.Value where A.Value: Swift.Equatable, B.Value: Swift.Equatable {
    public static func == (_ a0: SwiftUI.SequenceGesture<A, B>.Value, _ a1: SwiftUI.SequenceGesture<A, B>.Value) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.SequenceGesture: SwiftUI.Gesture {
    public typealias Body = Swift.Never
}

extension SwiftUI.SequenceGesture.Value: Swift.Equatable where A.Value: Swift.Equatable, B.Value: Swift.Equatable {
}
