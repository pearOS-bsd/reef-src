// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ExclusiveGesture (Gestures).

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

public struct ExclusiveGesture<A, B> where A: SwiftUI.Gesture, B: SwiftUI.Gesture {
    public var first: A
    public var second: B
    public init(_ a0: A, _ a1: B) { _skeleton() }
    @frozen public enum Value {
        case first(A.Value)
        case second(B.Value)
    }
}

extension SwiftUI.ExclusiveGesture.Value where A.Value: Swift.Equatable, B.Value: Swift.Equatable {
    public static func == (_ a0: SwiftUI.ExclusiveGesture<A, B>.Value, _ a1: SwiftUI.ExclusiveGesture<A, B>.Value) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ExclusiveGesture: SwiftUI.Gesture {
    public typealias Body = Swift.Never
}

extension SwiftUI.ExclusiveGesture.Value: Swift.Equatable where A.Value: Swift.Equatable, B.Value: Swift.Equatable {
}
